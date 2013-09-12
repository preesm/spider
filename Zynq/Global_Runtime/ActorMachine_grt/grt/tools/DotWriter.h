/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef DOT_WRITER
#define DOT_WRITER

#include "../graphs/CSDAG/CSDAGGraph.h"
#include "../graphs/CSDAG/CSDAGVertex.h"
#include "../graphs/CSDAG/CSDAGEdge.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/SRDAG/SRDAGVertex.h"
#include "../graphs/SRDAG/SRDAGEdge.h"


/**
 * Writes a dot file from a graph of a given type
 * 
 * @author mpelcat
 */
class DotWriter {

	private :
	public : 
		/**
		 Constructor
		*/
		DotWriter();


		/**
		 Destructor
		*/
		~DotWriter();


		/**
		 Writes a SRDAGGraph in a file

		 @param graph: written graph
		 @param path: output file path
		*/
		void write(SRDAGGraph* graph, const char* path, char displayNames);


		/**
		 Writes a CSDAGGraph in a file

		 @param graph: written graph
		 @param path: output file path
		*/
		void write(CSDAGGraph* graph, const char* path, char displayResolvedValues);
};

#endif
