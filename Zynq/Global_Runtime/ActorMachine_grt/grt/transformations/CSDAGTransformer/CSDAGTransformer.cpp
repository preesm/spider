/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * A CSDAG transformer transforms a CSDAG graph into an SRDAG one. 
 * It duplicates edges and breaks the repetition patterns to generate the SRDAG graph.
 * 
 * @author mpelcat
 */
#include "CSDAGTransformer.h"
#include "../../tools/SchedulingError.h"
#include "../../tools/DotWriter.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>

/**
 Constructor
*/
CSDAGTransformer::CSDAGTransformer(){
	CSDAGExplodeVertex = NULL;
	nb_input_edges = 0;
	nbExplode = 0;
}

/**
 Destructor
*/
CSDAGTransformer::~CSDAGTransformer()
{
}

/**
 Transforms a CSDAG graph in DAG

 @param input: input CSDAG graph with pattern expressions on production and consumption
 @param output: output SRDAG graph with variable expressions on production and consumption
*/
void CSDAGTransformer::transform(CSDAGGraph* input, SRDAGGraph* output, Architecture* archi){

//	CSDAGExplodeVertex = output->getExplodeVertex();
	CSDAGExplodeVertex->setName("Explode");
	CSDAGExplodeVertex->setFunctionIndex(0);
	CSDAGExplodeVertex->addTiming(0,"1");
//	CSDAGExplodeVertex->addTiming(1,"1");
	CSDAGExplodeVertex->addAllConstraints(0,archi->getNbSlaves());
	CSDAGExplodeVertex->addConstraint(0,1);
//	CSDAGExplodeVertex->addAllConstraints(1, 1);
	CSDAGExplodeVertex->resolveArgs();
	CSDAGExplodeVertex->resolveTimings(archi);


	DotWriter dot;

	for(int i=0; i< input->getNbVertices(); i++){
		CSDAGVertex* inputVertex = input->getVertex(i);
		// Gets the number of repetitions and treats the source and sink patterns of all the input edges
		brv[i] = treatInputEdgesData(input, inputVertex);
		// Sets the number of repetitions of the current CSDAG Vertex
		inputVertex->setRepetitionNb(brv[i]);
		// Transforms one vertex of the graph in several SRDAG vertices

		int firstCreatedEdgeIndex = output->getNbEdges();

		createGraph(input, inputVertex, output);

//		char tempstr[30];
//		sprintf(tempstr, "srdag%d.gv", i);
//		dot.write(output, tempstr, 1);

		// The edges newly added are sorted in the order of their SRDAG sink. Only the edges after the given index are sorted
		output->sortEdges(firstCreatedEdgeIndex);
	}

	// Iterating the CSDAG vertices to add their corresponding repetitions to the SRDAG
//	for(int i=0; i< input->getNbVertices(); i++){ // /!\ NbVertice-1 due to the dummy explode CSDAGVertex
//		CSDAGVertex* inputVertex = input->getVertex(i);
//		// Gets the number of repetitions and treats the source and sink patterns of all the input edges
//		brv[i] = treatInputEdgesData(input, inputVertex);
//		// Sets the number of repetitions of the current CSDAG Vertex
//		inputVertex->setRepetitionNb(brv[i]);
//		// Transforms one vertex of the graph in several SRDAG vertices
//		addVertices(input, inputVertex, brv[i], output);
//
//		int firstCreatedEdgeIndex = output->getNbEdges();
//
//		for(int j=0; j< nb_input_edges; j++){
//			// Transforms one edge of the CSDAG graph in several SRDAG edges
//			createEdges(input,input->getVertexIndex(input_edges[j]->getSource()), j, i, output);
//		}
//
//		// The edges newly added are sorted in the order of their SRDAG sink. Only the edges after the given index are sorted
//		output->sortEdges(firstCreatedEdgeIndex);
//	}
}

/**
 Gets the input edges of the current vertex and their source and sink patterns. Treats the patterns to obtain the
 same global production and consumption and to generate the number of repetitions.

 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
 @param sinkVertex: input CSDAG vertex which is the sink of the edges being treated
 @return the sinkVertex repetition number
*/
int CSDAGTransformer::treatInputEdgesData(CSDAGGraph* inputGraph, CSDAGVertex* sinkVertex){
	// Getting sinkVertex input edges
	this->nb_input_edges = inputGraph->getInputEdges(sinkVertex,input_edges);

	// All the input edges must lead to the same sink repetition
	unsigned short sinkRepetition = 0;
	
	if(nb_input_edges == 0){
		// A vertex without predecessor is supposed to have a repetition factor of 1
		return 1;
	}

	// Resolving input edges source and sink patterns
	for(int input_edge_id=0; input_edge_id<nb_input_edges; input_edge_id++){
		CSDAGEdge* input_edge = input_edges[input_edge_id];
		CSDAGVertex* source = input_edge->getSource();

		unsigned short sourceRepetition = brv[inputGraph->getVertexIndex(source)]; // previously calculated repetition number of the source
		unsigned short sourcePatternSize = input_edge->getResolvedProductionNb();

		if(sourceRepetition > MAX_CSDAG_PATTERN_SIZE){
			// The edge source repetition number outsizes the maximal pattern size
			exitWithCode(1002);
		}

		int totalProducedTokens = 0;
		for(int j=0; j<sourceRepetition; j++){
			// Assigning the patterns in a loop
			int prod = input_edge->getResolvedProduction(j%sourcePatternSize);
			resolvedInputEdgesPatterns[Source][input_edge_id][j] = prod;
			totalProducedTokens += prod;
		}

		// The input patterns now contains exactly sourceRepetition non null patterns
		resolvedInputEdgesPatternsSizes[Source][input_edge_id] = sourceRepetition;

		// Duplicating the sink pattern of each input edge to match the number of tokens of the source pattern
		int totalConsumedTokens = 0;
		unsigned short localSinkRepetition = 0;
		unsigned short sinkPatternSize = input_edge->getResolvedConsumptionNb();

		if(totalConsumedTokens == 0 && totalProducedTokens==0){
			resolvedInputEdgesPatterns[Sink][input_edge_id][0] = 0;
		}

		for(int k=0; totalConsumedTokens < totalProducedTokens; k++){
			int cons = input_edge->getResolvedConsumption(k%sinkPatternSize);
			resolvedInputEdgesPatterns[Sink][input_edge_id][k] = cons;
			totalConsumedTokens += cons;
			localSinkRepetition++;
		}

		if(sinkRepetition == 0){
			sinkRepetition = localSinkRepetition;
		}else{
			if(sinkRepetition != localSinkRepetition  && !(totalConsumedTokens == 0 && totalProducedTokens == 0)){
				// Two input edges set different repetition numbers to their common sink
				exitWithCode(1003);
			}
		}

		resolvedInputEdgesPatternsSizes[Sink][input_edge_id] = sinkRepetition;

		if(sinkRepetition > MAX_CSDAG_PATTERN_SIZE){
			// The edge sink repetition number outsizes the maximal pattern size
			exitWithCode(1004);
		}

		if(totalConsumedTokens != totalProducedTokens){
			// The CSDAG to SRDAG transformation led to an unbalanced number of produced and consumed token for an edge
			exitWithCode(1005);
		}
	}

	return sinkRepetition;
}

/**
 Transforms a CSDAG vertex and its input edges in DAG vertices and edges.

 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
 @param sinkVertex: input CSDAG vertex which is the sink of the edges being treated
 @param brval: Basis repetition value of sinkVertex
 @param outputGraph: output SRDAG graph with variable expressions on production and consumption
 
 @return the repetition of the current vertex
*/
void CSDAGTransformer::addVertices(CSDAGGraph* inputGraph, CSDAGVertex* sinkVertex, int brval, SRDAGGraph* outputGraph){

	SRDAGVertex* outputVertex;

	// Adding one SRDAG vertex per repetition
	for(int j=0; j< brval; j++){
		outputVertex = outputGraph->addVertex();

		// Setting attributes to link the SRDAG graph to the DAG one
		outputVertex->setCsDagReference(sinkVertex);
		outputVertex->setReferenceIndex(j);
//		for(int i=0;i<sinkVertex->getParamNb();i++){
//			outputVertex->setParamValue(i, sinkVertex->getParamValue(j,i));
//		}
		sinkRepetitions[j] = outputVertex;
	}
}


void CSDAGTransformer::createGraph(CSDAGGraph* inputGraph, CSDAGVertex* csdag_vertex, SRDAGGraph* outputGraph){
	int csdag_nbInput = csdag_vertex->getNbInputEdge();
	int csdag_brv = brv[inputGraph->getVertexIndex(csdag_vertex)];
	int csdag_sources_brv[MAX_CSDAG_INPUT_EDGES];

	SRDAGVertex* sourceRepetitionsInput[MAX_CSDAG_INPUT_EDGES][MAX_CSDAG_VERTEX_REPETITION];

	int remainingSourceTokens[MAX_CSDAG_INPUT_EDGES];
	int remainingSinkTokens[MAX_CSDAG_INPUT_EDGES];

	int hSourceIndex[MAX_CSDAG_INPUT_EDGES];

	int toAddNb=0;
	SRDAGVertex* toAddSource[MAX_CSDAG_INPUT_EDGES];
	int toAddRate[MAX_CSDAG_INPUT_EDGES];

	SRDAGVertex* 	explode			[MAX_CSDAG_INPUT_EDGES];
	SRDAGEdge* 		lastSource		[MAX_CSDAG_INPUT_EDGES];
	int 			lastSinkId		[MAX_CSDAG_INPUT_EDGES];
	int 			curSourceSREdge	[MAX_CSDAG_INPUT_EDGES];
	int 			curSinkSREdge	[MAX_CSDAG_INPUT_EDGES];

	memset(curSourceSREdge, 0, MAX_CSDAG_INPUT_EDGES*sizeof(int));
	memset(curSinkSREdge, 0, MAX_CSDAG_INPUT_EDGES*sizeof(int));
	memset(hSourceIndex, 0, MAX_CSDAG_INPUT_EDGES*sizeof(int));


	for(int i=0; i<csdag_nbInput; i++){
		remainingSourceTokens[i] = resolvedInputEdgesPatterns[Source][i][0];
		remainingSinkTokens[i]   = resolvedInputEdgesPatterns[Sink][i][0];
		csdag_sources_brv[i]     = brv[inputGraph->getVertexIndex(csdag_vertex->getInputEdge(i)->getSource())];
		outputGraph->getVerticesFromCSDAGReference(csdag_vertex->getInputEdge(i)->getSource(),sourceRepetitionsInput[i]);
	}

	for(int srdag_vertexId=0; srdag_vertexId< csdag_brv; srdag_vertexId++){
		for(int inputId=0; inputId<csdag_nbInput; inputId++){

			if(remainingSourceTokens[inputId] == remainingSinkTokens[inputId] && curSourceSREdge[inputId] == 0 && curSinkSREdge[inputId] == 0){
				toAddSource[toAddNb] = sourceRepetitionsInput[inputId][hSourceIndex[inputId]];
				toAddRate[toAddNb] = remainingSourceTokens[inputId];
				toAddNb++;

			}else{
				if(curSourceSREdge[inputId] == 0 && curSinkSREdge[inputId] == 0){
					explode[inputId] = outputGraph->addVertex();
					explode[inputId]->setCsDagReference(CSDAGExplodeVertex);
					explode[inputId]->setReferenceIndex(nbExplode++);
				}

				while(1){
					int tokenRate = std::min(remainingSourceTokens[inputId],remainingSinkTokens[inputId]);

					if(curSourceSREdge[inputId] == 0){
						lastSource[inputId] = outputGraph->addEdge(sourceRepetitionsInput[inputId][hSourceIndex[inputId]],tokenRate,explode[inputId]);
					}else{
						lastSource[inputId]->setTokenRate(lastSource[inputId]->getTokenRate() + tokenRate);
					}

					if(curSinkSREdge[inputId] == 0){
						toAddSource[toAddNb] = explode[inputId];
						toAddRate[toAddNb] = tokenRate;
						lastSinkId[inputId] = toAddNb;
						toAddNb++;
					}else{
						toAddRate[lastSinkId[inputId]] += tokenRate;
					}

					curSourceSREdge[inputId]++;
					curSinkSREdge[inputId]++;

					remainingSourceTokens[inputId] -= tokenRate;
					remainingSinkTokens[inputId] -= tokenRate;

					if(remainingSourceTokens[inputId] == 0){
						hSourceIndex[inputId]++;
						curSourceSREdge[inputId] = 0;
						if(hSourceIndex[inputId]>=csdag_sources_brv[inputId]){
							if(remainingSinkTokens[inputId] != 0){
								// SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
								exitWithCode(1006);
							}
							break;
						}
						remainingSourceTokens[inputId] = resolvedInputEdgesPatterns[Source][inputId][hSourceIndex[inputId]]; /* /!\ */
					}else if(curSinkSREdge[inputId] > 1)
						curSourceSREdge[inputId] = 0;

					if(remainingSinkTokens[inputId] == 0){
						curSinkSREdge[inputId] = 0;
						remainingSinkTokens[inputId] = resolvedInputEdgesPatterns[Sink][inputId][srdag_vertexId];
						break;
					}else if(curSourceSREdge[inputId] > 1)
						curSinkSREdge[inputId] = 0;
				}
			}
		}

		SRDAGVertex* vertex = outputGraph->addVertex();
		vertex->setCsDagReference(csdag_vertex);
		vertex->setReferenceIndex(srdag_vertexId);

		for(int i=0; i<toAddNb; i++){
			outputGraph->addEdge(toAddSource[i],toAddRate[i],vertex);
		}
		toAddNb = 0;
	}

//	int sourcePatternSize = brv[sourceVertexIndex];
//	int sinkPatternSize = brv[sinkVertexIndex];

//	int csdagVertexInputNb = inputGraph->getVertex(sinkVertexIndex)->getNbInputEdge();

}
/**
 Adds the SRDAG edges corresponding to one edge of the input graph.They are not added yet
 because they must be added in the order of their sink.

 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
 @param sourceVertexIndex: index of the input edge source
 @param edgeIndex: index of the input edge
 @param sinkVertexIndex: index of the input edge sink
 @param outputGraph: output SRDAG graph with variable expressions on production and consumption
*/
void CSDAGTransformer::createEdges(CSDAGGraph* inputGraph, int sourceVertexIndex, int edgeIndex,
							   int sinkVertexIndex, SRDAGGraph* outputGraph){

	if((brv[sourceVertexIndex] != 0) && (brv[sinkVertexIndex] != 0)){
		// Retrieving the different repetitions of source in the correct order
		outputGraph->getVerticesFromCSDAGReference(inputGraph->getVertex(sourceVertexIndex),sourceRepetitions);
		// The sink repetitions are already in the array sinkRepetitions

		int* sourcePattern = resolvedInputEdgesPatterns[Source][edgeIndex];
		int sourcePatternSize = brv[sourceVertexIndex];
		int* sinkPattern = resolvedInputEdgesPatterns[Sink][edgeIndex];
		int sinkPatternSize = brv[sinkVertexIndex];

		int remainingSourceTokens = sourcePattern[0];
		int remainingSinkTokens = sinkPattern[0];

		int hSourceIndex = 0;
		int hSinkIndex = 0;

		SRDAGVertex* explode;
		SRDAGEdge *source, *sink;
		int curSourceSREdge = 0;
		int curSinkSREdge = 0;

		/* If prod == conso, it is a single rate edge, no explode needed */
		if(remainingSourceTokens == remainingSinkTokens){
			outputGraph->addEdge(sourceRepetitions[hSourceIndex],remainingSourceTokens, sinkRepetitions[hSinkIndex]);
			return;
		}

		while(1){
			int tokenRate = (remainingSourceTokens>remainingSinkTokens)?remainingSinkTokens:remainingSourceTokens; //min

			if(curSourceSREdge == 0 && curSinkSREdge == 0){
				explode = outputGraph->addVertex();
				explode->setCsDagReference(CSDAGExplodeVertex);
				explode->setReferenceIndex(nbExplode++);
			}

			if(curSourceSREdge == 0){
				source = outputGraph->addEdge(sourceRepetitions[hSourceIndex],tokenRate,explode);
			}else{
				source->setTokenRate(source->getTokenRate() + tokenRate);
			}

			if(curSinkSREdge == 0){
				sink = outputGraph->addEdge(explode, tokenRate, sinkRepetitions[hSinkIndex]);
			}else{
				sink->setTokenRate(sink->getTokenRate() + tokenRate);
			}

			curSourceSREdge++;
			curSinkSREdge++;

//			SRDAGEdge* newEdge = outputGraph->addEdge(sourceRepetitions[hSourceIndex],tokenRate,sinkRepetitions[hSinkIndex]);
			remainingSourceTokens -= tokenRate;
			remainingSinkTokens -= tokenRate;

			if(remainingSourceTokens == 0){
				hSourceIndex++;
				curSourceSREdge = 0;
				if(hSourceIndex>=sourcePatternSize){
					if(remainingSinkTokens != 0){
						// SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
						exitWithCode(1006);
					}
					break;
				}
				remainingSourceTokens = sourcePattern[hSourceIndex];
			}else if(curSinkSREdge > 1)
				curSourceSREdge = 0;

			if(remainingSinkTokens == 0){
				hSinkIndex++;
				curSinkSREdge = 0;
				if(hSinkIndex>=sinkPatternSize){
					// SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
					exitWithCode(1006);
					break;
				}
				remainingSinkTokens = sinkPattern[hSinkIndex];
			}else if(curSourceSREdge > 1)
				curSinkSREdge = 0;
		}
	}

}

