/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include "LRTActor.h"
#include "Memory.h"



LRTActor::LRTActor(SRDAGGraph *graph, SRDAGVertex* srvertex, launcher* curLaunch){
	this->ActionID = srvertex->getReference()->getFunction_index();
	this->nbInputFifos = srvertex->getNbInputEdge();
	this->nbOutputFifos = srvertex->getNbOutputEdge();
	this->nbParams = srvertex->getReference()->getNbParameters();

	for(UINT32 i=0; i<this->nbInputFifos; i++){
		SRDAGEdge* edge = srvertex->getInputEdge(i);
		this->inFIFOs[i] = curLaunch->getFIFO(graph->getEdgeIndex(edge));
//		this->inputFifoId[i] = graph->getEdgeIndex(edge);
//		this->readDataSize[i] = edge->getTokenRate() * DEFAULT_FIFO_SIZE; // TODO: the size should come within the edge.
//		this->inputFifoAddr[i] = mem->alloc(this->readDataSize[i]);
	}

	for(UINT32 i=0; i<this->nbOutputFifos; i++){
		SRDAGEdge* edge = srvertex->getOutputEdge(i);
		this->outFIFOs[i] = curLaunch->getFIFO(graph->getEdgeIndex(edge));
//		this->outputFifoId[i] = graph->getEdgeIndex(edge);
//		this->writeDataSize[i] = edge->getTokenRate() * DEFAULT_FIFO_SIZE; // TODO: the size should come within the edge.
//		this->outputFifoAddr[i] = mem->alloc(this->writeDataSize[i]);
	}

	for(UINT32 i=0; i<this->nbParams; i++){
		this->params[i] = srvertex->getReference()->getParameter(i)->getValue();
	}
}


void LRTActor::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CREATE_TASK);
	launch->addUINT32ToSend(slave, this->ActionID);
	launch->addUINT32ToSend(slave, 0); // Not an actor machine.
	launch->addUINT32ToSend(slave, this->nbInputFifos);
	launch->addUINT32ToSend(slave, this->nbOutputFifos);
	for (UINT32 i = 0; i < this->nbInputFifos; i++) {
		launch->addUINT32ToSend(slave, this->inFIFOs[i]->id);
		launch->addUINT32ToSend(slave, this->inFIFOs[i]->addr);
		// TODO: see if the FIFO' size is required.
	}
	for (UINT32 i = 0; i < this->nbOutputFifos; i++) {
		launch->addUINT32ToSend(slave, this->outFIFOs[i]->id);
		launch->addUINT32ToSend(slave, this->outFIFOs[i]->addr);
		// TODO: see if the FIFO' size is required.
	}
	for(UINT32 i = 0; i < this->nbParams; i++)
		launch->addUINT32ToSend(slave, this->params[i]);

//	launch->addUINT32ToReceive(slave, MSG_CREATE_TASK);
}


void LRTActor::toDot(FILE* pFile, char* vertexName, UINT32 vertexId){
	fprintf (pFile, "\t%d [label=\"%s\\nFunction F%d\\n", vertexId, vertexName, this->ActionID);
	for (UINT32 i = 0; i < this->nbInputFifos; i++)
		fprintf (pFile, "Fin  %d\\n", this->inFIFOs[i]->id);

	for (UINT32 i = 0; i < this->nbOutputFifos; i++)
		fprintf (pFile, "Fout %d\\n", this->outFIFOs[i]->id);

	for(UINT32 i = 0; i < this->nbParams; i++)
		fprintf (pFile, "Param %d\\n", this->params[i]);

	fprintf (pFile, "\",shape=box];\n");
}
