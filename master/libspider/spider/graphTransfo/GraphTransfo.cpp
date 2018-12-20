/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include <cinttypes>

#include <graphTransfo/GraphTransfo.h>
#include <graphTransfo/LinkVertices.h>
#include <graphTransfo/AddVertices.h>
#include <graphTransfo/ComputeBRV.h>
#include <scheduling/Scheduler.h>
#include <monitor/TimeMonitor.h>
#include <scheduling/MemAlloc.h>
#include <graphTransfo/Optims.h>
#include <launcher/Launcher.h>
#include <tools/Queue.h>
#include <lrt.h>

#define SCHEDULE_SIZE 10000

static void initJob(transfoJob *job, SRDAGVertex *nextHierVx) {
    memset(job, 0, sizeof(transfoJob));
    job->graph = nextHierVx->getSubGraph();
    job->graphIter = nextHierVx->getRefId();

    /* Add Static and Herited parameter values */
    job->paramValues = CREATE_MUL(TRANSFO_STACK, job->graph->getNParam(), Param);
    for (int paramIx = 0; paramIx < job->graph->getNParam(); paramIx++) {
        PiSDFParam *param = job->graph->getParam(paramIx);
        switch (param->getType()) {
            case PISDF_PARAM_STATIC:
                job->paramValues[paramIx] = param->getStaticValue();
                break;
            case PISDF_PARAM_HERITED:
                job->paramValues[paramIx] = nextHierVx->getInParam(param->getParentId());
                break;
            case PISDF_PARAM_DYNAMIC:
                // Do nothing, cannot be evaluated yet
                job->paramValues[paramIx] = -1;
                break;
            case PISDF_PARAM_DEPENDENT_STATIC:
                job->paramValues[paramIx] = param->getExpression()->evaluate(job->graph->getParams(), job);
                break;
            case PISDF_PARAM_DEPENDENT_DYNAMIC:
                job->paramValues[paramIx] = -1;
                break;
        }
    }

    /* Add edge interfaces in job */
    job->inputIfs = CREATE_MUL(TRANSFO_STACK, nextHierVx->getNConnectedInEdge(), SRDAGEdge*);
    job->outputIfs = CREATE_MUL(TRANSFO_STACK, nextHierVx->getNConnectedOutEdge(), SRDAGEdge*);

    memcpy(job->inputIfs, nextHierVx->getInEdges(), nextHierVx->getNConnectedInEdge() * sizeof(SRDAGEdge *));
    memcpy(job->outputIfs, nextHierVx->getOutEdges(), nextHierVx->getNConnectedOutEdge() * sizeof(SRDAGEdge *));

}

static void freeJob(transfoJob *job) {
    if (job->configs != nullptr)
        StackMonitor::free(TRANSFO_STACK, job->configs);

    if (job->bodies != nullptr) {
        for (int i = 0; i < job->graph->getNBody(); i++) {
            if (job->bodies[i] != nullptr) {
                StackMonitor::free(TRANSFO_STACK, job->bodies[i]);
            }
        }
        StackMonitor::free(TRANSFO_STACK, job->bodies);
    }

    StackMonitor::free(TRANSFO_STACK, job->paramValues);
    StackMonitor::free(TRANSFO_STACK, job->inputIfs);
    StackMonitor::free(TRANSFO_STACK, job->outputIfs);
}

static SRDAGVertex *getNextHierVx(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) { // todo check executable
        SRDAGVertex *vertex = topDag->getVertex(i);
        if (vertex->isHierarchical() && vertex->getState() == SRDAG_EXEC) {
            return vertex;
        }
    }
    return nullptr;
}

void jit_ms(
        PiSDFGraph *topPisdf,
        Archi *archi,
        SRDAGGraph *topSrdag,
        MemAlloc *memAlloc,
        Scheduler *scheduler) {

    /* Initialize topDag */

    auto *schedule = CREATE(TRANSFO_STACK, Schedule)(archi->getNPE(), SCHEDULE_SIZE);

    /* Add initial top actor */
    PiSDFVertex *root = topPisdf->getBody(0);
    if (!root->isHierarchical()) {
        printf("Error top graph without subgraph\n");
        abort();
    }
    topSrdag->addVertex(root, 0, 0);
    topSrdag->updateState();

    Queue<transfoJob *> jobQueue(TRANSFO_STACK);

    // Check nb of config //

    /* Look for hierrachical actor in topDag */
    TimeMonitor::startMonitoring();

    do {
        SRDAGVertex *nextHierVx = getNextHierVx(topSrdag);

        /* Exit loop if no hierarchical actor found */
        if (!nextHierVx) break;

        do {
            /* Fill the transfoJob data */
            auto *job = CREATE(TRANSFO_STACK, transfoJob);
            initJob(job, nextHierVx);

            /* Remove Hierachical vertex */
            topSrdag->delVertex(nextHierVx);

            if (job->graph->getNConfig() > 0) {
                /* Put CA in topDag */
                addCAVertices(topSrdag, job);

                /* Link CA in topDag */
                linkCAVertices(topSrdag, job);

                jobQueue.push(job);
            } else {
                if (Spider::getVerbose()) {
                    /* Display Param values */
                    fprintf(stderr, "\nINFO: Parameter values:\n");
                    for (int i = 0; i < job->graph->getNParam(); i++) {
                        fprintf(stderr, "INFO: >> Name: %s -- Value: %" PRId64"\n", job->graph->getParam(i)->getName(),
                                job->paramValues[i]);
                    }
                }

                auto *brv = CREATE_MUL(TRANSFO_STACK, job->graph->getNBody(), int);
                computeBRV(job, brv);
                if (Spider::getVerbose()) {
                    /* Display BRV values */
                    fprintf(stderr, "\nINFO: BRV values:\n");
                    for (int i = 0; i < job->graph->getNBody(); i++) {
                        fprintf(stderr, "INFO: >> Vertex: %s -- RV: %d\n", job->graph->getBody(i)->getName(), brv[i]);
                    }
                }

                addSRVertices(topSrdag, job, brv);

                linkSRVertices(topSrdag, job, brv);

                freeJob(job);

                StackMonitor::free(TRANSFO_STACK, brv);
                StackMonitor::free(TRANSFO_STACK, job);
            }

            /* Find next hierarchical vertex */
            topSrdag->updateState();
            nextHierVx = getNextHierVx(topSrdag);

        } while (nextHierVx);

        TimeMonitor::endMonitoring(TRACE_SPIDER_GRAPH);

        if (Spider::getGraphOptim()) {
            TimeMonitor::startMonitoring();
            optims(topSrdag);
            TimeMonitor::endMonitoring(TRACE_SPIDER_OPTIM);
        }

        /* Schedule and launch execution */
        TimeMonitor::startMonitoring();
        scheduler->scheduleOnlyConfig(topSrdag, memAlloc, schedule, archi);
        TimeMonitor::endMonitoring(TRACE_SPIDER_SCHED);

        if (Spider::getVerbose()) {
            fprintf(stderr, "INFO: Launching config actors...\n");
        }

        // Run
        Platform::get()->getLrt()->runUntilNoMoreJobs();


        /* Resolve params must be done by itself */
        Launcher::get()->resolveParams(archi, topSrdag);

        if (Spider::getVerbose()) {
            fprintf(stderr, "INFO: Resolved parameters.\n");
        }

        TimeMonitor::startMonitoring();

        while (!jobQueue.isEmpty()) {

            /* Pop job from queue */
            transfoJob *job = jobQueue.pop();

            /* Recompute Dependent Dynamic Params */
            for (int paramIx = 0; paramIx < job->graph->getNParam(); paramIx++) {
                PiSDFParam *param = job->graph->getParam(paramIx);
                if (param->getType() == PISDF_PARAM_DEPENDENT_DYNAMIC) {
                    job->paramValues[paramIx] = param->getExpression()->evaluate(job->graph->getParams(), job);
                }
            }

            if (Spider::getVerbose()) {
                /* Display Param values */
                fprintf(stderr, "\nINFO: Parameter values:\n");
                for (int i = 0; i < job->graph->getNParam(); i++) {
                    fprintf(stderr, "INFO: >> Name: %s -- Value: %" PRId64"\n", job->graph->getParam(i)->getName(),
                            job->paramValues[i]);
                }
            }

            /* Compute BRV */
            auto *brv = CREATE_MUL(TRANSFO_STACK, job->graph->getNBody(), int);
            computeBRV(job, brv);
            if (Spider::getVerbose()) {
                /* Display BRV values */
                fprintf(stderr, "\nINFO: BRV values:\n");
                for (int i = 0; i < job->graph->getNBody(); i++) {
                    fprintf(stderr, "INFO: >> Vertex: %s -- RV: %d\n", job->graph->getBody(i)->getName(), brv[i]);
                }
            }

            /* Add vertices */
            addSRVertices(topSrdag, job, brv);

            /* Link vertices */
            linkSRVertices(topSrdag, job, brv);

            freeJob(job);

            StackMonitor::free(TRANSFO_STACK, brv);
            StackMonitor::free(TRANSFO_STACK, job);

            TimeMonitor::endMonitoring(TRACE_SPIDER_GRAPH);
            TimeMonitor::startMonitoring();
        }

        if (Spider::getVerbose()) {
            fprintf(stderr, "INFO: Finished resolving everything.\n");
        }

        // TODO
        topSrdag->updateState();

        TimeMonitor::endMonitoring(TRACE_SPIDER_GRAPH);

        if (Spider::getGraphOptim()) {
            TimeMonitor::startMonitoring();
            optims(topSrdag);
            TimeMonitor::endMonitoring(TRACE_SPIDER_OPTIM);
        }

        TimeMonitor::startMonitoring();

//        printf("Finish one iter\n");
    } while (true);

    topSrdag->updateState();
    TimeMonitor::endMonitoring(TRACE_SPIDER_GRAPH);

    if (Spider::getGraphOptim()) {
        TimeMonitor::startMonitoring();
        optims(topSrdag);
        TimeMonitor::endMonitoring(TRACE_SPIDER_OPTIM);
    }

    /* Schedule and launch execution */
    TimeMonitor::startMonitoring();
    scheduler->schedule(topSrdag, memAlloc, schedule, archi);
    TimeMonitor::endMonitoring(TRACE_SPIDER_SCHED);

    Platform::get()->getLrt()->runUntilNoMoreJobs();

    schedule->~Schedule();
    StackMonitor::free(TRANSFO_STACK, schedule);
    StackMonitor::freeAll(TRANSFO_STACK);
}

Schedule *static_scheduler(SRDAGGraph *topSrdag,
                           MemAlloc *memAlloc,
                           Scheduler *scheduler) {
    PiSDFGraph *topGraph = Spider::getGraph();

    auto *schedule = CREATE(TRANSFO_STACK, Schedule)(Spider::getArchi()->getNPE(), SCHEDULE_SIZE);

    /* Add initial top actor */
    PiSDFVertex *root = topGraph->getBody(0);
    if (!root->isHierarchical()) {
        throwSpiderException("Top graph is empty!");
    }
    topSrdag->addVertex(root, 0, 0);
    topSrdag->updateState();


    // Check nb of config //

    /* Look for hierrachical actor in topDag */
    TimeMonitor::startMonitoring();

    SRDAGVertex *nextHierVx = getNextHierVx(topSrdag);

    do {
        /* Fill the transfoJob data */
        auto *job = CREATE(TRANSFO_STACK, transfoJob);
        initJob(job, nextHierVx);

        /* Remove Hierachical vertex */
        topSrdag->delVertex(nextHierVx);

        if (Spider::getVerbose()) {
            /* Display Param values */
            Platform::get()->fprintf(stdout, "\nParam Values:\n");
            for (int i = 0; i < job->graph->getNParam(); i++) {
                Platform::get()->fprintf(stdout, "%s: %d\n", job->graph->getParam(i)->getName(), job->paramValues[i]);
            }
        }

        auto *brv = CREATE_MUL(TRANSFO_STACK, job->graph->getNBody(), int);
        computeBRV(job, brv);
        if (Spider::getVerbose()) {
            /* Display BRV values */
            Platform::get()->fprintf(stdout, "\nBRV Values:\n");
            for (int i = 0; i < job->graph->getNBody(); i++) {
                Platform::get()->fprintf(stdout, "%s: %d\n", job->graph->getBody(i)->getName(), brv[i]);
            }
        }

        addSRVertices(topSrdag, job, brv);

        linkSRVertices(topSrdag, job, brv);

        freeJob(job);

        StackMonitor::free(TRANSFO_STACK, brv);
        StackMonitor::free(TRANSFO_STACK, job);

        /* Find next hierarchical vertex */
        topSrdag->updateState();
        nextHierVx = getNextHierVx(topSrdag);

    } while (nextHierVx);

    TimeMonitor::endMonitoring(TRACE_SPIDER_GRAPH);

    if (Spider::getGraphOptim()) {
        TimeMonitor::startMonitoring();
        optims(topSrdag);
        TimeMonitor::endMonitoring(TRACE_SPIDER_OPTIM);
    }

    topSrdag->updateState();

    /* Schedule and launch execution */
    TimeMonitor::startMonitoring();
    scheduler->schedule(topSrdag, memAlloc, schedule, Spider::getArchi());
    TimeMonitor::endMonitoring(TRACE_SPIDER_SCHED);

    Platform::get()->getLrt()->runUntilNoMoreJobs();

    return schedule;
}
