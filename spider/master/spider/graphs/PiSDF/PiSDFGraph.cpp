/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFEdge.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFParam.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <tools/Set.h>

#include <cstdio>

PiSDFGraph::PiSDFGraph(
		int nEdges, int nParams,
		int nInputIf, int nOutputIf,
		int nConfig, int nBody,
		Archi *archi,
		Stack *stack):

		edges_(nEdges, stack),
		params_(nParams, stack),
		bodies_(nBody, stack),
		configs_(nConfig, stack),
		inputIfs_(nInputIf, stack),
		outputIfs_(nInputIf, stack)
		{
	stack_ = stack;
	parent_ = 0;
	archi_ = archi;
}

PiSDFGraph::~PiSDFGraph() {
	while(edges_.getN() > 0)
		delEdge(edges_[0]);
	while(bodies_.getN() > 0){
		if(bodies_[0]->isHierarchical()){
			bodies_[0]->getSubGraph()->~PiSDFGraph();
			stack_->free(bodies_[0]->getSubGraph());
		}
		delVertex(bodies_[0]);
	}
	while(configs_.getN() > 0)
		delVertex(configs_[0]);
	while(inputIfs_.getN() > 0)
		delVertex(inputIfs_[0]);
	while(outputIfs_.getN() > 0)
		delVertex(outputIfs_[0]);
	while(params_.getN() > 0)
		delParam(params_[0]);
}

PiSDFVertex* PiSDFGraph::addBodyVertex(
		const char* vertexName, int fctId,
		int nInEdge, int nOutEdge,
		int nInParam){
	PiSDFVertex* body = CREATE(stack_, PiSDFVertex)(
			vertexName, fctId,
			bodies_.getN(),
			PISDF_TYPE_BODY, PISDF_SUBTYPE_NORMAL,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, 0,
			archi_,
			stack_);
	bodies_.add(body);
	return body;
}

PiSDFVertex* PiSDFGraph::addHierVertex(
		const char* vertexName,
		PiSDFGraph* graph,
		int nInEdge, int nOutEdge,
		int nInParam){
	PiSDFVertex* body = CREATE(stack_, PiSDFVertex)(
			vertexName, -1,
			bodies_.getN(),
			PISDF_TYPE_BODY, PISDF_SUBTYPE_NORMAL,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, 0,
			archi_,
			stack_);
	body->setSubGraph(graph);
	graph->setParentVertex(body);
	bodies_.add(body);
	return body;
}

PiSDFVertex* PiSDFGraph::addSpecialVertex(
		PiSDFSubType type,
		int nInEdge, int nOutEdge,
		int nInParam){
	PiSDFVertex* body = CREATE(stack_, PiSDFVertex)(
			0, -1,
			bodies_.getN(),
			PISDF_TYPE_BODY, type,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, 0,
			archi_,
			stack_);
	bodies_.add(body);
	return body;
}

PiSDFVertex* PiSDFGraph::addConfigVertex(
		const char* vertexName, int fctId,
		PiSDFSubType subType,
		int nInEdge, int nOutEdge,
		int nInParam, int nOutParam){
	PiSDFVertex* config = CREATE(stack_, PiSDFVertex)(
			vertexName, fctId,
			configs_.getN(),
			PISDF_TYPE_CONFIG, subType,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, nOutParam,
			archi_,
			stack_);
	configs_.add(config);
	return config;

}

PiSDFVertex* PiSDFGraph::addInputIf(
		const char* name,
		int nInParam){
	PiSDFVertex* inIf = CREATE(stack_, PiSDFVertex)(
			name, -1,
			inputIfs_.getN(),
			PISDF_TYPE_IF, PISDF_SUBTYPE_INPUT_IF,
			this, 0,
			0, 1,
			nInParam, 0,
			archi_,
			stack_);
	inputIfs_.add(inIf);
	return inIf;
}

PiSDFVertex* PiSDFGraph::addOutputIf(
		const char* name,
		int nInParam){
	PiSDFVertex* outIf = CREATE(stack_, PiSDFVertex)(
			name, -1,
			outputIfs_.getN(),
			PISDF_TYPE_IF, PISDF_SUBTYPE_OUTPUT_IF,
			this, 0,
			1, 0,
			nInParam, 0,
			archi_,
			stack_);
	outputIfs_.add(outIf);
	return outIf;
}


PiSDFParam* PiSDFGraph::addStaticParam(const char* name, const char* expr){
	PiSDFParam* param = CREATE(stack_, PiSDFParam)(
			name, params_.getN(),
			this, PISDF_PARAM_STATIC);

	// TODO set value

	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addStaticParam(const char* name, int value){
	PiSDFParam* param = CREATE(stack_, PiSDFParam)(
			name, params_.getN(),
			this, PISDF_PARAM_STATIC);
	param->setValue(value);
	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addHeritedParam(const char* name, int parentId){
	PiSDFParam* param = CREATE(stack_, PiSDFParam)(
			name, params_.getN(),
			this, PISDF_PARAM_HERITED);
	param->setParentId(parentId);
	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addDynamicParam(const char* name){
	PiSDFParam* param = CREATE(stack_, PiSDFParam)(
			name, params_.getN(),
			this, PISDF_PARAM_DYNAMIC);
	params_.add(param);
	return param;
}

PiSDFEdge* PiSDFGraph::addEdge(int nParam) {
	PiSDFEdge* edge = CREATE(stack_, PiSDFEdge)(
			this, nParam, stack_);
	edges_.add(edge);
	return edge;
}

PiSDFEdge* PiSDFGraph::connect(
		PiSDFVertex* src, int srcPortId, const char* prod,
		PiSDFVertex* snk, int snkPortId, const char* cons,
		const char* delay, int nParam){
	PiSDFEdge* edge = this->addEdge(nParam);
	edge->connectSrc(src, srcPortId, prod, stack_);
	edge->connectSnk(snk, snkPortId, cons, stack_);
	edge->setDelay(delay, stack_);
	src->connectOutEdge(srcPortId, edge);
	snk->connectInEdge(snkPortId, edge);
	return edge;
}


void PiSDFGraph::delVertex(PiSDFVertex* vertex){
	switch(vertex->getType()){
	case PISDF_TYPE_BODY:
		bodies_.del(vertex);
		break;
	case PISDF_TYPE_CONFIG:
		configs_.del(vertex);
		break;
	case PISDF_TYPE_IF:
		if(vertex->getSubType() == PISDF_SUBTYPE_INPUT_IF)
			inputIfs_.del(vertex);
		else
			outputIfs_.del(vertex);
		break;
	}
	vertex->~PiSDFVertex();
	stack_->free(vertex);
}

void PiSDFGraph::delParam(PiSDFParam* param){
	params_.del(param);
	param->~PiSDFParam();
	stack_->free(param);
}

void PiSDFGraph::delEdge(PiSDFEdge* edge){
	edges_.del(edge);
	edge->~PiSDFEdge();
	stack_->free(edge);
}

/** Print Fct */
void PiSDFGraph::print(const char *path){
	int file = Platform::get()->fopen(path);
	if(file == -1){
		printf("cannot open %s\n", path);
		return;
	}

	// Writing header
	Platform::get()->fprintf(file, "digraph csdag {\n");
	Platform::get()->fprintf(file, "\tnode [color=\"#433D63\"];\n");
	Platform::get()->fprintf(file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	Platform::get()->fprintf(file, "\trankdir=LR;\n\n");


	// Drawing parameters.
	Platform::get()->fprintf(file, "\t# Parameters\n");
	for(int i=0; i<params_.getN(); i++){
		PiSDFParam *param = params_[i];
		Platform::get()->fprintf(file, "\t%s [label=\"%s\" shape=house];\n",
				param->getName(),
				param->getName());
	}

	// Drawing Config PiSDF vertices.
	Platform::get()->fprintf(file, "\n\t# Configs\n");
	for(int i=0; i<configs_.getN(); i++){
		PiSDFVertex *config = configs_[i];
		Platform::get()->fprintf(file, "\t%d [shape=doubleoctagon,label=\"%s\"];\n",
				config->getId(),
				config->getName());

		// Drawing lines : vertex -> parameters.
		for (int j = 0; j < config->getNOutParam(); j++) {
			Platform::get()->fprintf(file, "\t%d->%s [style=dotted];\n",
					config->getId(),
					config->getOutParam(j)->getName());
		}

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < config->getNInParam(); j++) {
			Platform::get()->fprintf(file, "\t%s->%d [style=dotted];\n",
					config->getInParam(j)->getName(),
					config->getId());
		}
		Platform::get()->fprintf(file, "\n");
	}

	// Drawing Body PiSDF vertices.
	Platform::get()->fprintf(file, "\t# Body Vertices\n");
	for(int i=0; i<bodies_.getN(); i++){
		PiSDFVertex *body = bodies_[i];
		if(body->isHierarchical()){
			char name[100];
			sprintf(name, "%s_sub.gv", body->getName());
			body->getSubGraph()->print(name);
		}

		Platform::get()->fprintf(file, "\t%d [label=\"%s\"];\n",
			body->getId(),
			body->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < body->getNInParam(); j++) {
			Platform::get()->fprintf(file, "\t%s->%d [style=dotted];\n",
				body->getInParam(j)->getName(),
				body->getId());
		}
		Platform::get()->fprintf(file, "\n");
	}

	// Drawing Input vertices.
	Platform::get()->fprintf(file, "\t# Input Ifs\n");
	for(int i=0; i<inputIfs_.getN(); i++){
		PiSDFVertex *inIf = inputIfs_[i];
		Platform::get()->fprintf(file, "\t%d [shape=cds,label=\"%s\"];\n",
				inIf->getId(),
				inIf->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < inIf->getNInParam(); j++) {
			Platform::get()->fprintf(file, "\t%s->%d [style=dotted];\n",
				inIf->getInParam(j)->getName(),
				inIf->getId());
		}
		Platform::get()->fprintf(file, "\n");
	}

	// Drawing Output vertices.
	Platform::get()->fprintf(file, "\t# Output Ifs\n");
	for(int i=0; i<outputIfs_.getN(); i++){
		PiSDFVertex *outIf = outputIfs_[i];
		Platform::get()->fprintf(file, "\t%d [shape=cds,label=\"%s\"];\n",
				outIf->getId(),
				outIf->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < outIf->getNInParam(); j++) {
			Platform::get()->fprintf(file, "\t%s->%d [style=dotted];\n",
				outIf->getInParam(j)->getName(),
				outIf->getId());
		}
		Platform::get()->fprintf(file, "\n");
	}

	// Drawing edges.
	Platform::get()->fprintf(file, "\t# Edges\n");
	for(int i=0; i<edges_.getN(); i++){
		PiSDFEdge *edge = edges_[i];
		char prodExpr[100];
		char consExpr[100];
		char delayExpr[100];

		edge->getProdExpr(prodExpr, 100);
		edge->getConsExpr(consExpr, 100);
		edge->getDelayExpr(delayExpr, 100);

//		Parser_toString(&(edge->production), &(graph->params), shortenedPExpr);
//		Parser_toString(&(edge->consumption), &(graph->params), shortenedCExpr);

		/*Platform::get()->fprintf("\t%s->%s [taillabel=\"%s\" headlabel=\"%s\" labeldistance=%d labelangle=50];\n",
			edge->getSource()->getName(),edge->getSink()->getName(),
			shortenedPExpr,shortenedCExpr,labelDistance);*/
//		Platform::get()->fprintf(file, "\t%s->%s [taillabel=\"(%d):%s\" headlabel=\"(%d):%s\"];\n",
//			edge->source->name,
//			edge->sink->name,
//			edge->sourcePortIx,
//			shortenedPExpr,
//			edge->sinkPortIx,
//			shortenedCExpr);
		//labelDistance = 3 + labelDistance%(3*4); // Oscillating the label distance to keep visibility
		Platform::get()->fprintf(file, "\t%d->%d [taillabel=\"(%d):%s\" headlabel=\"(%d):%s\" label=\"%s\"];\n",
			edge->getSrc()->getId(),
			edge->getSnk()->getId(),
			edge->getSrcPortIx(),
			prodExpr,
			edge->getSnkPortIx(),
			consExpr,
			delayExpr);
	}

	Platform::get()->fprintf(file, "}\n");
	Platform::get()->fclose(file);
}
