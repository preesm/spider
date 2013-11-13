
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef DOT_WRITER
#define DOT_WRITER

//#include "../graphs/CSDAG/CSDAGGraph.h"
//#include "../graphs/CSDAG/CSDAGVertex.h"
//#include "../graphs/CSDAG/CSDAGEdge.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/SRDAG/SRDAGVertex.h"
#include "../graphs/SRDAG/SRDAGEdge.h"
//#include "../graphs/PiCSDF/PiCSDFGraph.h"
//#include "../graphs/PiCSDF/PiCSDFEdge.h"
#include "../graphs/PiSDF/PiSDFGraph.h"
#include "../graphs/SDF/SDFGraph.h"

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
//		void write(CSDAGGraph* graph, const char* path, char displayNames);


		/**
		 Writes a PiSDFGraph in a file

		 @param graph: written graph
		 @param path: output file path
		*/
		void write(PiSDFGraph* graph, const char* path, char displayNames);

		void write(BaseVertex** schedulableVertices, UINT32 nbSchedulabeVertices, const char* path, char displayNames);

		void write(SDFGraph* sdf, const char* path, char displayNames);
};

#endif
