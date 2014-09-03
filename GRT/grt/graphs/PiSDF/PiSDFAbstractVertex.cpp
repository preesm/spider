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

#include "PiSDFAbstractVertex.h"

PiSDFAbstractVertex::PiSDFAbstractVertex() {
	id=-1;
	graph = NULL;
	name[0] = '\0';
	function_index = -1;
	type=normal_vertex;
	nbParameters = 0;
	refPiSDFVertex = NULL;
	nbRepetition = 1;
	executable = undefined;
	status = VxStNoExecutable;
	scheduled = false;
	tempId=-1;
	inputEdges.reset();
	outputEdges.reset();

	for(int i=0; i<MAX_VERTEX_REPETITION; i++)
		childVertices[i].reset();

	for(int i=0; i<MAX_SLAVE_TYPES; i++){
		constraints[i] = FALSE;
	}
}

PiSDFAbstractVertex::~PiSDFAbstractVertex() {
	// TODO Auto-generated destructor stub
}

void PiSDFAbstractVertex::reset() {
	id=-1;
	graph = NULL;
	name[0] = '\0';
	function_index = -1;
	type=normal_vertex;
	nbParameters = 0;
	refPiSDFVertex = NULL;
	nbRepetition = 1;
	executable = undefined;
	status = VxStNoExecutable;
	scheduled = false;
	tempId=-1;
	inputEdges.reset();
	outputEdges.reset();
	for(int i=0; i<MAX_VERTEX_REPETITION; i++)
		childVertices[i].reset();

	for(int i=0; i<MAX_SLAVE_TYPES; i++){
		constraints[i] = FALSE;
	}
}

void PiSDFAbstractVertex::resetRefs(){
	for(int i=0; i<MAX_VERTEX_REPETITION; i++)
		childVertices[i].reset();
}

void PiSDFAbstractVertex::setInputEdge(PiSDFEdge* edge, UINT32 id){
	inputEdges.add(edge,id);
}

void PiSDFAbstractVertex::setOutputEdge(PiSDFEdge* edge, UINT32 id){
	outputEdges.add(edge,id);
}


UINT32 PiSDFAbstractVertex::getInputEdgeId(PiSDFEdge* edge){
	return inputEdges.getIdOf(edge);
}

UINT32 PiSDFAbstractVertex::getOutputEdgeId(PiSDFEdge* edge){
	return outputEdges.getIdOf(edge);
}


void PiSDFAbstractVertex::addParameter(PiSDFParameter* param)
{
	if(nbParameters > MAX_NB_PiSDF_PARAMS){
		exitWithCode(1057);
	}
	parameters[nbParameters++] = param;
}

//
//bool PiSDFAbstractVertex::getExecutable(){
//	// Checking if all parameters have been resolved.
//	for (UINT32 i = 0; i < this->nbParameters; i++) {
//		if(! this->parameters[i]->getResolved()) return false;
//	}
//
//	for (UINT32 i = 0; i < this->nbInputEdges; i++){
//		PiSDFEdge* edge = this->inputEdges[i];
//		// Updating integer consumption value.
//		int value;
//		globalParser.interpret(edge->getConsumption(), &value);
//		edge->setConsumtionInt(value);
//
//		// Updating integer delay value.
//		globalParser.interpret(edge->getDelay(), &value);
//		edge->setDelayInt(value);
//
//		/*
//		 * Checking if input edges have enough initial tokens (delays),
//		 * and consumption is not zero, so that the vertex can be executed.
//		 */
//		if((edge->getConsumptionInt() < edge->getDelayInt()) || (edge->getConsumptionInt() == 0))
//			return false;
//		else if (this->getType() == select_vertex) // For "select" vertices, only one valid edge is enough.
//			break;
//	}
//
//	for (UINT32 i = 0; i < this->nbOutputEdges; i++){
//		PiSDFEdge* edge = this->outputEdges[i];
//		// Updating integer production value.
//		int value;
//		globalParser.interpret(edge->getProduction(), &value);
//		edge->setProductionInt(value);
//	}
//
//	return true;
//}


/*
 * See the header file for comments..
 */
bool PiSDFAbstractVertex::invalidEdges(){
	UINT32 nbNulInputs, nbNulOutputs;
	nbNulInputs = 0;
	nbNulOutputs = 0;

	for (UINT32 i = 0; i < inputEdges.getNb(); i++) {
		if((inputEdges[i]->getProductionInt() == 0) &&
		   (inputEdges[i]->getConsumptionInt() == 0))
			nbNulInputs++;
	}

	for (UINT32 i = 0; i < outputEdges.getNb(); i++) {
		if((outputEdges[i]->getProductionInt() == 0) &&
		   (outputEdges[i]->getConsumptionInt() == 0))
			nbNulOutputs++;
	}

	return ((nbNulInputs == inputEdges.getNb()) && (nbNulOutputs == outputEdges.getNb()));
}

