/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * A DAG graph
 * 
 * @author mpelcat
 */

#include "CSDAGGraph.h"
#include "CSDAGVertex.h"
#include "CSDAGEdge.h"
#include "../../tools/SchedulingError.h"

/**
 Constructor
*/
CSDAGGraph::CSDAGGraph()
{
	// There is no dynamic allocation of graph members
	nbVertices = 0;
	memset(vertices,'\0',MAX_CSDAG_VERTICES*sizeof(CSDAGVertex));
	nbEdges = 0;
	memset(edges,'\0',MAX_CSDAG_EDGES*sizeof(CSDAGEdge));

//	patternsTableSize = 0;
}

/**
 Destructor
*/
CSDAGGraph::~CSDAGGraph()
{
}

/**
 Adding a vertex to the graph
 
 @param vertexName: the name of the new vertex

 @return the new vertex
*/
CSDAGVertex* CSDAGGraph::addVertex(const char* vertexName){
	CSDAGVertex* vertex = NULL;
	if(nbVertices < MAX_CSDAG_VERTICES){
		vertex = &vertices[nbVertices];
		vertex->setBase(this);
		vertex->setName(vertexName);
		nbVertices++;
	}
	else{
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
	return vertex;
}

/**
 Adding an edge to the graph
 
 @param source: The source vertex of the edge
 @param production: number of tokens (chars) produced by the source
 @param sink: The sink vertex of the edge
 @param consumption: number of tokens (chars) consumed by the sink
 @return the created edge
*/
CSDAGEdge* CSDAGGraph::addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption){
	CSDAGEdge* edge = NULL;
	if(nbEdges < MAX_CSDAG_EDGES){
		edge = &edges[nbEdges];
		edge->setBase(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		sink->addInputEdge(edge);
		nbEdges++;
	}
	else{
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return edge;
}

/**
 Resolves the timings of each vertex in the CSDAG graph

 @param archi the current architecture
*/
void CSDAGGraph::resolveTimings(Architecture* archi){

	// iterating the input edges to calculate the one that gives the highest t-level to vertex;
	for(int i=0; i<nbVertices; i++){
		vertices[i].resolveTimings(archi);
	}
}

void CSDAGGraph::resolveEdgeTokenRates(){
	for(int i=0; i<nbEdges; i++){
		edges[i].resolveProdCons();
	}
}

void CSDAGGraph::resolveVerticesParams(){
	for(int i=0; i<nbVertices; i++){
		vertices[i].resolveArgs();
	}
}
