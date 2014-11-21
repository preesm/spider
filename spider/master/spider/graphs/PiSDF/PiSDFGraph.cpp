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
#include <platform_file.h>

#include <cstdio>

PiSDFGraph::PiSDFGraph() {
	stack_ = 0;
	parent_ = 0;
}

PiSDFGraph::PiSDFGraph(
		int nEdges, int nParams,
		int nInputIf, int nOutputIf,
		int nConfig, int nBody,
		Stack *stack){
	stack_ = stack;
	parent_ = 0;

	edges_ = PiSDFEdgeSet(nEdges, stack);
	params_ = PiSDFParamSet(nParams, stack);
	bodies_ = PiSDFVertexSet(nBody, stack);
	configs_ = PiSDFVertexSet(nConfig, stack);
	inputIfs_ = PiSDFVertexSet(nInputIf, stack);
	outputIfs_ = PiSDFVertexSet(nOutputIf, stack);
}

PiSDFGraph::~PiSDFGraph() {
}

PiSDFVertex* PiSDFGraph::addBodyVertex(
		const char* vertexName, int fctId,
		PiSDFSubType subType,
		int nInEdge, int nOutEdge,
		int nInParam){
	PiSDFVertex* body = sAlloc(stack_, 1, PiSDFVertex);
	*body = PiSDFVertex(vertexName, fctId,
			bodies_.getN(),
			PISDF_TYPE_BODY, subType,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, 0,
			stack_);
	bodies_.add(body);
	return body;
}

PiSDFVertex* PiSDFGraph::addConfigVertex(
		const char* vertexName, int fctId,
		PiSDFSubType subType,
		int nInEdge, int nOutEdge,
		int nInParam, int nOutParam){
	PiSDFVertex* config = sAlloc(stack_, 1, PiSDFVertex);
	*config = PiSDFVertex(vertexName, fctId,
			configs_.getN(),
			PISDF_TYPE_CONFIG, subType,
			this, 0,
			nInEdge, nOutEdge,
			nInParam, nOutParam,
			stack_);
	configs_.add(config);
	return config;

}

PiSDFVertex* PiSDFGraph::addInputIf(
		const char* name,
		int nInParam){
	PiSDFVertex* inIf = sAlloc(stack_, 1, PiSDFVertex);
	*inIf = PiSDFVertex(name, -1,
			inputIfs_.getN(),
			PISDF_TYPE_IF, PISDF_SUBTYPE_INPUT_IF,
			this, 0,
			0, 1,
			nInParam, 0,
			stack_);
	inputIfs_.add(inIf);
	return inIf;
}

PiSDFVertex* PiSDFGraph::addOutputIf(
		const char* name,
		int nInParam){
	PiSDFVertex* outIf = sAlloc(stack_, 1, PiSDFVertex);
	*outIf = PiSDFVertex(name, -1,
			outputIfs_.getN(),
			PISDF_TYPE_IF, PISDF_SUBTYPE_OUTPUT_IF,
			this, 0,
			1, 0,
			nInParam, 0,
			stack_);
	outputIfs_.add(outIf);
	return outIf;
}


PiSDFParam* PiSDFGraph::addStaticParam(const char* name, const char* expr){
	PiSDFParam* param = sAlloc(stack_, 1, PiSDFParam);
	*param = PiSDFParam(name, params_.getN(), this, PISDF_PARAM_STATIC);

	// TODO set value

	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addStaticParam(const char* name, int value){
	PiSDFParam* param = sAlloc(stack_, 1, PiSDFParam);
	*param = PiSDFParam(name, params_.getN(), this, PISDF_PARAM_STATIC);
	param->setValue(value);
	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addHeritedParam(const char* name){
	PiSDFParam* param = sAlloc(stack_, 1, PiSDFParam);
	*param = PiSDFParam(name, params_.getN(), this, PISDF_PARAM_HERITED);
	params_.add(param);
	return param;
}

PiSDFParam* PiSDFGraph::addDynamicParam(const char* name){
	PiSDFParam* param = sAlloc(stack_, 1, PiSDFParam);
	*param = PiSDFParam(name, params_.getN(), this, PISDF_PARAM_DYNAMIC);
	params_.add(param);
	return param;
}

PiSDFEdge* PiSDFGraph::addEdge(int nParam) {
	PiSDFEdge* edge = sAlloc(stack_, 1, PiSDFEdge);
	*edge = PiSDFEdge(this,
			nParam,
			stack_);
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

/** Print Fct */
void PiSDFGraph::print(const char *path){
	int file = platform_fopen (path);
	if(file == -1){
		printf("cannot open %s\n", path);
		return;
	}

	// Writing header
	platform_fprintf (file, "digraph csdag {\n");
	platform_fprintf (file, "\tnode [color=\"#433D63\"];\n");
	platform_fprintf (file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	platform_fprintf (file, "\trankdir=LR;\n\n");

	/** Declare Iterator */
	PiSDFEdgeIterator edgeIt 	= edges_.getIterator();
	PiSDFParamIterator paramIt 	= params_.getIterator();
	PiSDFVertexIterator bodyIt 	= bodies_.getIterator();
	PiSDFVertexIterator inIfIt	 = inputIfs_.getIterator();
	PiSDFVertexIterator configIt = configs_.getIterator();
	PiSDFVertexIterator outIfIt = outputIfs_.getIterator();


	// Drawing parameters.
	platform_fprintf (file, "\t# Parameters\n");
	for (paramIt.first(); paramIt.finished(); paramIt.next()){
		platform_fprintf (file, "\t%s [label=\"%s\" shape=house];\n",
				paramIt.current()->getName(),
				paramIt.current()->getName());
	}

	// Drawing Config PiSDF vertices.
	platform_fprintf (file, "\n\t# Configs\n");
	for (configIt.first(); configIt.finished(); configIt.next()){
		platform_fprintf (file, "\t%s [shape=doubleoctagon,label=\"%s\"];\n",
				configIt.current()->getName(),
				configIt.current()->getName());

		// Drawing lines : vertex -> parameters.
		for (int j = 0; j < configIt.current()->getNOutParam(); j++) {
			platform_fprintf(file, "\t%s->%s [style=dotted];\n",
					configIt.current()->getName(),
					configIt.current()->getOutParam(j)->getName());
		}

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < configIt.current()->getNInParam(); j++) {
			platform_fprintf(file, "\t%s->%s [style=dotted];\n",
					configIt.current()->getInParam(j)->getName(),
					configIt.current()->getName());
		}
		platform_fprintf (file, "\n");
	}

	// Drawing Body PiSDF vertices.
	platform_fprintf (file, "\t# Body Vertices\n");
	for (bodyIt.first(); bodyIt.finished(); bodyIt.next()){
		if(bodyIt.current()->isHierarchical()){
			char name[100];
			sprintf(name, "%s_sub.gv", bodyIt.current()->getName());
			bodyIt.current()->getSubGraph()->print(name);
		}

		platform_fprintf (file, "\t%s [label=\"%s\"];\n",
			bodyIt.current()->getName(),
			bodyIt.current()->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < bodyIt.current()->getNInParam(); j++) {
			platform_fprintf(file, "\t%s->%s [style=dotted];\n",
				bodyIt.current()->getInParam(j)->getName(),
				bodyIt.current()->getName());
		}
		platform_fprintf (file, "\n");
	}

	// Drawing Input vertices.
	platform_fprintf (file, "\t# Input Ifs\n");
	for (inIfIt.first(); inIfIt.finished(); inIfIt.next()) {
		platform_fprintf (file, "\t%s [shape=cds,label=\"%s\"];\n",
				inIfIt.current()->getName(),
				inIfIt.current()->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < inIfIt.current()->getNInParam(); j++) {
			platform_fprintf(file, "\t%s->%s [style=dotted];\n",
				inIfIt.current()->getInParam(j)->getName(),
				inIfIt.current()->getName());
		}
		platform_fprintf (file, "\n");
	}

	// Drawing Output vertices.
	platform_fprintf (file, "\t# Output Ifs\n");
	for (outIfIt.first(); outIfIt.finished(); outIfIt.next()) {
		platform_fprintf (file, "\t%s [shape=cds,label=\"%s\"];\n",
				outIfIt.current()->getName(),
				outIfIt.current()->getName());

		// Drawing lines : parameter -> vertex.
		for (int j = 0; j < outIfIt.current()->getNInParam(); j++) {
			platform_fprintf(file, "\t%s->%s [style=dotted];\n",
				outIfIt.current()->getInParam(j)->getName(),
				outIfIt.current()->getName());
		}
		platform_fprintf (file, "\n");
	}

	// Drawing edges.
	platform_fprintf (file, "\t# Edges\n");
	for (edgeIt.first(); edgeIt.finished(); edgeIt.next()) {
		char prodExpr[100];
		char consExpr[100];
		char delayExpr[100];

		edgeIt.current()->getProdExpr(prodExpr, 100);
		edgeIt.current()->getConsExpr(consExpr, 100);
		edgeIt.current()->getDelayExpr(delayExpr, 100);

//		Parser_toString(&(edge->production), &(graph->params), shortenedPExpr);
//		Parser_toString(&(edge->consumption), &(graph->params), shortenedCExpr);

		/*platform_fprintf ("\t%s->%s [taillabel=\"%s\" headlabel=\"%s\" labeldistance=%d labelangle=50];\n",
			edge->getSource()->getName(),edge->getSink()->getName(),
			shortenedPExpr,shortenedCExpr,labelDistance);*/
//		platform_fprintf (file, "\t%s->%s [taillabel=\"(%d):%s\" headlabel=\"(%d):%s\"];\n",
//			edge->source->name,
//			edge->sink->name,
//			edge->sourcePortIx,
//			shortenedPExpr,
//			edge->sinkPortIx,
//			shortenedCExpr);
		//labelDistance = 3 + labelDistance%(3*4); // Oscillating the label distance to keep visibility
		platform_fprintf (file, "\t%s->%s [taillabel=\"(%d):%s\" headlabel=\"(%d):%s\" label=\"%s\"];\n",
			edgeIt.current()->getSrc()->getName(),
			edgeIt.current()->getSnk()->getName(),
			edgeIt.current()->getSrcPortIx(),
			prodExpr,
			edgeIt.current()->getSnkPortIx(),
			consExpr,
			delayExpr);
	}

	platform_fprintf (file, "}\n");
	platform_fclose(file);
}
