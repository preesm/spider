/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
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
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/Archi/ArchiOld.h>
#include <scheduling/Schedule.h>
#include <Message.h>

class Launcher {
public:
    static Launcher *get();

    void sendJob(PiSDFScheduleJob *job);

    void sendJob(SRDAGScheduleJob *job);

    void resolveParams(ArchiOld *archi, SRDAGGraph *topDag);

    void sendTraceSpider(TraceSpiderType type, Time start, Time end);

    /**
     * @brief Send a notification to all (or given) LRT to enable TRACE
     * @param lrtID LRT to send the notification to (-1 to send to every LRT)
     */
    void sendEnableTrace(int lrtID);

    /**
     * @brief Send a notification to all (or given) LRT to disable TRACE
     * @param lrtID LRT to send the notification to (-1 to send to every LRT)
     */
    void sendDisableTrace(int lrtID);

    void sendRepeatJobQueue(bool enable);

    /**
     * @brief Send a notification to all LRT to signal that they won't receive other jobs
     *
     * @param schedule Schedule containing information for last job of each PE
     */
    void sendEndNotification(Schedule *schedule);

    /**
     * @brief Send a notification to all LRT to broadcast their job stamp to other LRT
     */
    void sendBroadCastNotification(bool delayBroadcoast);

    int getNLaunched();

    void rstNLaunched();

protected:
    Launcher();

private:
    int nLaunched_;
    int curNParam_;
    static Launcher instance_;

    void send_ClearTimeMsg(int lrtIx);

    void send_ResetLrtMsg(int lrtIx);

};

#endif/*LAUNCHER_H*/
