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

#ifndef PiSDFAbstractVertex_H_
#define PiSDFAbstractVertex_H_

#include <cstring>
#include <platform_types.h>
#include <grt_definitions.h>
#include <tools/SchedulingError.h>
#include <tools/Array.h>
#include "../PiSDF/PiSDFParameter.h"
#include "../PiSDF/PiSDFEdge.h"
//#include "graphs/SDF/SDFGraph.h"

typedef enum {
	pisdf_vertex,
	config_vertex,
	input_vertex,
	join_vertex,
	broad_vertex,
	output_vertex,
	switch_vertex,
	select_vertex,
	roundBuff_vertex
}VERTEX_TYPE;

typedef enum{
	undefined = -1,
	impossible = 0,
	possible = 1
}EXE_FLAG;

typedef enum{
	VxStExecutable,
	VxStExecuted,
	VxStNoExecutable
}STATUS_FLAG;

class PiSDFAbstractVertex {
	UINT32 id;
	char name[MAX_VERTEX_NAME_SIZE];
	UINT64 function_index;
	VERTEX_TYPE type;

	Array<PiSDFEdge*,MAX_NB_INPUT_EDGES> inputEdges;
	Array<PiSDFEdge*,MAX_NB_OUTPUT_EDGES> outputEdges;

	UINT8 nbParameters;
	PiSDFParameter* parameters[MAX_NB_PiSDF_PARAMS];

	PiSDFAbstractVertex* refPiSDFVertex;	// If generated from a PiSDF, this is the reference to PiSDF vertex.

	UINT32 		nbRepetition; // Stores the number of replicas in a Sr graph.

	EXE_FLAG 	executable; // Says whether the vertex can be executed.
	STATUS_FLAG status;
	bool 		scheduled; 	// Says whether the vertex has been already scheduled within the current iteration.
	UINT32 		tempId; 	// Used while creating a topology matrix.
public:
	PiSDFAbstractVertex();
	virtual ~PiSDFAbstractVertex();

	void reset();

	void setInputEdge(PiSDFEdge* edge, UINT32 id);

	void setOutputEdge(PiSDFEdge* edge, UINT32 id);

	void addParameter(PiSDFParameter* param);

	/*
	 * Marks a vertex as executable, i.e. all its parameters have been solved and
	 * all its predecessors are executable.
	 */
	void checkForExecution();

	bool checkPredecessors();

	/*
	 * Checks the input and output edges of the vertex. It returns true, if all the edges
	 * productions and consumptions are zero. It means that the vertex will not be
	 * considered in the current iteration. It is the case of vertices that depend on
	 * parameters, and such parameters' values reduce the edges' expression to zero.
	 */
	bool invalidEdges();






	/*
	 * Auto-generated getters and setters.
	 */

    UINT32 getId() const
    {
        return id;
    }

	PiSDFEdge* getInputEdge(UINT32 index){
		if(index >= inputEdges.getNb()) exitWithCode(1041);
		return inputEdges[index];
	}

	PiSDFEdge* getOutputEdge(UINT32 index){
		if(index >= outputEdges.getNb()) exitWithCode(1041);
		return outputEdges[index];
	}

	UINT32 getInputEdgeIx(PiSDFEdge* edge){
		for(UINT32 i=0; i<inputEdges.getNb(); i++){
			if(inputEdges[i] == edge)
				return i;
		}
		return -1;
	}

	UINT32 getOutputEdgeIx(PiSDFEdge* edge){
		for(UINT32 i=0; i<outputEdges.getNb(); i++){
			if(outputEdges[i] == edge)
				return i;
		}
		return -1;
	}


    UINT64 getNbInputEdges(){
        return inputEdges.getNb();
    }

    UINT64 getNbOutputEdges(){
        return outputEdges.getNb();
    }

    UINT32 getInputEdgeId(PiSDFEdge* edge);
    UINT32 getOutputEdgeId(PiSDFEdge* edge);

    UINT64 getFunction_index() const
    {
        return function_index;
    }

    char* getName()
    {
        return name;
    }

    VERTEX_TYPE getType() const
    {
    	return this->type;
    }

    UINT8 getNbParameters() const
    {
        return nbParameters;
    }

    PiSDFParameter* getParameter(const UINT64 index)
    {
        return this->parameters[index];
    }

    UINT32 getNbRepetition() const
	{
    	return nbRepetition;
	}

//    bool getVisited() const
//    {
//        return visited;
//    }

    UINT32 getTempId() const
    {
        return tempId;
    }

    STATUS_FLAG getStatus() const
    {
        return status;
    }

	EXE_FLAG getExecutable() const
	{
		return executable;
	}


    bool getScheduled() const
    {
        return scheduled;
    }

    PiSDFAbstractVertex *getRefPiSDFVertex() const
    {
        return refPiSDFVertex;
    }

    void setId(UINT32 id)
    {
        this->id = id;
    }

    void setFunction_index(const UINT64 function_index)
    {
        this->function_index = function_index;
    }

    void setName(const char *name)
    {
    	strcpy(this->name,name);
    }

    void setType(VERTEX_TYPE type)
    {
    	this->type = type;
    }

    void setNbRepetition(UINT32 nbRepetition)
    {
        this->nbRepetition = nbRepetition;
    }

//    void setVisited(bool visited)
//    {
//        this->visited = visited;
//    }


    void setTempId(UINT32 tempId)
    {
        this->tempId = tempId;
    }


    void setStatus(STATUS_FLAG status)
    {
        this->status = status;
    }


    void setExecutable(EXE_FLAG executable)
    {
    	this->executable = executable;
    }


    void setScheduled(bool scheduled)
    {
        this->scheduled = scheduled;
    }


//    void setNbParameters(UINT8 nbParameters)
//    {
//        this->nbParameters = nbParameters;
//    }

//    void setParameter(const UINT64 index, PiSDFParameter* parameter)
//    {
//        this->parameters[index] = parameter;
//    }


    void setRefPiSDFVertex(PiSDFAbstractVertex *refPiSDFVertex)
    {
        this->refPiSDFVertex = refPiSDFVertex;
    }
};

#endif /* PiSDFAbstractVertex_H_ */
