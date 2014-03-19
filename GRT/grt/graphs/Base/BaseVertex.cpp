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

#include "BaseVertex.h"

BaseVertex::BaseVertex() {
	id=-1;
	name[0] = '\0';
	function_index = -1;
	type=pisdf_vertex;
	nbInputEdges = nbOutputEdges = nbParameters = 0;
	refPiSDFVertex = NULL;
	nbRepetition = 1;
	executable = undefined;
	status = VxStNoExecutable;
	scheduled = false;
	tempId=-1;
}

BaseVertex::~BaseVertex() {
	// TODO Auto-generated destructor stub
}

void BaseVertex::reset() {
	id=-1;
	name[0] = '\0';
	function_index = -1;
	type=pisdf_vertex;
	nbInputEdges = nbOutputEdges = nbParameters = 0;
	refPiSDFVertex = NULL;
	nbRepetition = 1;
	executable = undefined;
	status = VxStNoExecutable;
	scheduled = false;
	tempId=-1;
}

void BaseVertex::addInputEdge(PiSDFEdge *edge)
{
	if(nbInputEdges > MAX_NB_INPUT_EDGES){
		exitWithCode(1047);
	}
	inputEdges[nbInputEdges++] = edge;
}


void BaseVertex::addOutputEdge(PiSDFEdge *edge)
{
	if(nbOutputEdges > MAX_NB_OUTPUT_EDGES){
		exitWithCode(1048);
	}
	outputEdges[nbOutputEdges++] = edge;
}


void BaseVertex::addParameter(PiSDFParameter* param)
{
	if(nbParameters > MAX_NB_PiSDF_PARAMS){
		exitWithCode(1057);
	}
	parameters[nbParameters++] = param;
}

//
//bool BaseVertex::getExecutable(){
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


void BaseVertex::checkForExecution(){
	// Checking if all parameters have been resolved.
//	for (UINT32 i = 0; i < this->nbParameters; i++){
//		if(! this->parameters[i]->getResolved()){
////			executable = impossible;
//			return;
//		}
//	}

	// Checking if all predecessors are executable.
	for (UINT32 i = 0; i < this->nbInputEdges; i++)
	{
		BaseVertex* predVertex = inputEdges[i]->getSource();
		if((predVertex != this) && (predVertex->getType() != input_vertex)){
			if(predVertex->getExecutable() != possible)
				// Exiting since at least one predecessor has not been marked as possible.
				return;
		}
	}

	executable = possible; //The vertex can be executed.
}


bool BaseVertex::checkPredecessors(){
	// Checking if all predecessors are executable.
	for (UINT32 i = 0; i < this->nbInputEdges; i++)
	{
		BaseVertex* predVertex = inputEdges[i]->getSource();
		if((predVertex != this) && (predVertex->getType() != input_vertex)){
			if((predVertex->getStatus() != VxStExecutable)&&
				(predVertex->getType() != roundBuff_vertex))
				// Exiting since at least one predecessor has not been marked as possible.
				return false;
		}
	}
	return true;
}


/*
 * See the header file for comments..
 */
bool BaseVertex::invalidEdges(){
	UINT32 nbNulInputs, nbNulOutputs;
	nbNulInputs = 0;
	nbNulOutputs = 0;

	for (UINT32 i = 0; i < nbInputEdges; i++) {
		if((inputEdges[i]->getProductionInt() == 0) &&
		   (inputEdges[i]->getConsumptionInt() == 0))
			nbNulInputs++;
	}

	for (UINT32 i = 0; i < nbOutputEdges; i++) {
		if((outputEdges[i]->getProductionInt() == 0) &&
		   (outputEdges[i]->getConsumptionInt() == 0))
			nbNulOutputs++;
	}

	return ((nbNulInputs == nbInputEdges) && (nbNulOutputs == nbOutputEdges));
}


//
//
///*
// * If the function executes until the end, it marks the vertex as executable.
// */
//void BaseVertex::checkForExecution(SDFGraph* outSDF){
//	// Checking if all parameters have been resolved.
//	for (UINT32 i = 0; i < this->nbParameters; i++)
//		if(! this->parameters[i]->getResolved()) return;
//
//	// Resolving parameter-depending expressions.
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
//		// Checking if input edges have enough initial tokens (delays).
//		// TODO: check (edge->getConsumptionInt() > 0)) and (edge->getConsumptionInt() < edge->getDelayInt())
//
//		if ((this->getType() == select_vertex)&&(this->getType() == switch_vertex)){
//			// TODO: Special treatment for "select" and "switch" vertices...
//			break;
//		}
//		else{
//			// Checking if all predecessors can be executed.
//			BaseVertex* predVertex = inputEdges[i]->getSource();
//			// Call this function on each predecessor.
//			if((predVertex != this) && (predVertex->getType() != input_vertex))
//				if(predVertex->getExecutable() != possible) // Exits if at least one predecessor has not been marked as possible.
//					return;
//		}
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
//
////	// Checking if all inputs can be executed.
////	for (UINT32 i = 0; i < this->nbInputEdges; i++) {
////		BaseVertex* input = inputEdges[i]->getSource();
////		// Call this function on each predecessor.
////		if(!input->getVisited()){ // Exists if at least one predecessor has not been visited yet.
////			visited = false;
////			return;
//////			input->checkForExecution(outSDF);
////		}
////		if(!input->getExecutable()) return; // Exists if at least one predecessor can't be executed yet.
////	}
//
////	// Treating hierarchical vertices.
////	if(type == pisdf_vertex)
////	{
////		PiSDFGraph* subGraph = ((PiSDFVertex*)this)->getSubGraph();
////		if(subGraph != (PiSDFGraph*)0)
////			for (UINT32 i = 0; i < subGraph->getnb; i++) {
////
////			}
////	}
////	else{
////	}
//	executable = possible; //The vertex can be executed.
//}
