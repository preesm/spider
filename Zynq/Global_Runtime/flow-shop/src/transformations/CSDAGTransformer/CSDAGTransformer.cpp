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

#include <cstdio>
#include <cstdlib>

/**
 Constructor
*/
CSDAGTransformer::CSDAGTransformer(){
	nb_input_edges = 0;
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
	// Iterating the CSDAG vertices to add their corresponding repetitions to the SRDAG
	for(int i=0; i< input->getNbVertices(); i++){
		CSDAGVertex* inputVertex = input->getVertex(i);
		// Gets the number of repetitions and treats the source and sink patterns of all the input edges
		brv[i] = treatInputEdgesData(input, inputVertex);
		// Sets the number of repetitions of the current CSDAG Vertex
		inputVertex->setRepetitionNb(brv[i]);
		// Transforms one vertex of the graph in several SRDAG vertices
		addVertices(input, inputVertex, brv[i], output);

		int firstCreatedEdgeIndex = output->getNbEdges();

		for(int j=0; j< nb_input_edges; j++){
			// Transforms one edge of the CSDAG graph in several SRDAG edges
			createEdges(input,input->getVertexIndex(input_edges[j]->getSource()), j, i, output);
		}

		// The edges newly added are sorted in the order of their SRDAG sink. Only the edges after the given index are sorted
		output->sortEdges(firstCreatedEdgeIndex);
	}
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
		sinkRepetition = 1;
	}

	// Resolving input edges source and sink patterns
	for(int i=0; i<nb_input_edges; i++){
		CSDAGEdge* input_edge = input_edges[i];
		//Careful! some pattern elements can be 0 (corresponding to no vertex). -> the null elements are removed 
		// while resolving the pattern. Resolving the patterns also sets the parameters of the edge sink
		resolvedInputEdgesPatternsSizes[0][i] = input_edge->resolveSourcePattern(resolvedInputEdgesPatterns[0][i]);
		resolvedInputEdgesPatternsSizes[1][i] = input_edge->resolveSinkPattern(resolvedInputEdgesPatterns[1][i]);
		
		// Duplicating the source pattern of the current input edge to match its source repetitions (calculated when
		// this function was called on the source)

		CSDAGVertex* source = input_edge->getSource();
		int sourceIndex = inputGraph->getVertexIndex(source); // vertex index of the source
		unsigned short sourceRepetition = brv[sourceIndex]; // previously calculated repetition number of the source
		unsigned short currentSourcePatternSize = resolvedInputEdgesPatternsSizes[0][i];

		if(sourceRepetition > MAX_CSDAG_PATTERN_SIZE){
			// The edge source repetition number outsizes the maximal pattern size
			exitWithCode(1002);
		}

		int totalProducedTokens = 0;
		for(int j=0, k=0; j<sourceRepetition; j++){

			// Assigning the patterns in a loop
			resolvedInputEdgesPatterns[0][i][j] = resolvedInputEdgesPatterns[0][i][k];
			totalProducedTokens += resolvedInputEdgesPatterns[0][i][k];
			k++;
			if(k>=currentSourcePatternSize){
				k=0;
			}
		}

		// The input patterns now contains exactly sourceRepetition non null patterns
		resolvedInputEdgesPatternsSizes[0][i] = sourceRepetition;

		// Duplicating the sink pattern of each input edge to match the number of tokens of the source pattern
		int totalConsumedTokens = 0;
		unsigned short currentSinkPatternSize = resolvedInputEdgesPatternsSizes[1][i];
		unsigned short localSinkRepetitionCalc = 0;

		for(int k=0; totalConsumedTokens < totalProducedTokens;){
			resolvedInputEdgesPatterns[1][i][localSinkRepetitionCalc] = resolvedInputEdgesPatterns[1][i][k];
			totalConsumedTokens += resolvedInputEdgesPatterns[1][i][k];
			k++;
			localSinkRepetitionCalc++;
			if(k>=currentSinkPatternSize){
				k=0;
			}
		}

		if(sinkRepetition == 0){
			sinkRepetition = localSinkRepetitionCalc;
		}
		else{
			if(sinkRepetition != localSinkRepetitionCalc){
				// Two input edges set different repetition numbers to their common sink
				exitWithCode(1003);
			}
		}

		resolvedInputEdgesPatternsSizes[1][i] = sinkRepetition;

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
		for(int i=0;i<sinkVertex->getParamNb();i++){
			outputVertex->setParamValue(i, sinkVertex->getParamValue(j,i));
		}
		sinkRepetitions[j] = outputVertex;
	}
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

		int* sourcePattern = resolvedInputEdgesPatterns[0][edgeIndex];
		int sourcePatternSize = brv[sourceVertexIndex];
		int* sinkPattern = resolvedInputEdgesPatterns[1][edgeIndex];
		int sinkPatternSize = brv[sinkVertexIndex];

		int remainingSourceTokens = sourcePattern[0];
		int remainingSinkTokens = sinkPattern[0];

		int hSourceIndex = 0;
		int hSinkIndex = 0;

		while(1){
			int tokenRate = (remainingSourceTokens>remainingSinkTokens)?remainingSinkTokens:remainingSourceTokens; //min

			outputGraph->addEdge(sourceRepetitions[hSourceIndex],tokenRate,sinkRepetitions[hSinkIndex]);
			remainingSourceTokens -= tokenRate;
			remainingSinkTokens -= tokenRate;
			if(remainingSourceTokens == 0){
				hSourceIndex++;
				if(hSourceIndex>=sourcePatternSize){
					if(remainingSinkTokens != 0){
						// SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
						exitWithCode(1006);
					}
					break;
				}
				remainingSourceTokens = sourcePattern[hSourceIndex];
			}
			if(remainingSinkTokens == 0){
				hSinkIndex++;
				if(hSinkIndex>=sinkPatternSize){
					// SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
					exitWithCode(1006);
					break;
				}
				remainingSinkTokens = sinkPattern[hSinkIndex];
			}
		}
	}

}

