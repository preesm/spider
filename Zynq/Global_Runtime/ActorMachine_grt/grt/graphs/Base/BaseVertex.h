
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

#ifndef BASEVERTEX_H_
#define BASEVERTEX_H_

#include <cstring>
#include <types.h>
#include <grt_definitions.h>
#include <tools/SchedulingError.h>
#include "../PiSDF/PiSDFParameter.h"
#include "../PiSDF/PiSDFEdge.h"


typedef enum {
	pisdf_vertex,
	config_vertex,
	input_vertex,
	join_vertex,
	broad_vertex,
	output_vertex,
	switch_vertex,
	select_vertex
}VERTEXT_TYPE;

class BaseVertex {
	UINT32 id;
	char name[MAX_VERTEX_NAME_SIZE];
	UINT64 function_index;
	VERTEXT_TYPE type;
	UINT64 nbInputEdges;
	PiSDFEdge* inputEdges[MAX_NB_INPUT_EDGES];
	UINT64 nbOutputEdges;
	PiSDFEdge* outputEdges[MAX_NB_OUTPUT_EDGES];
	UINT8 nbParameters;
	PiSDFParameter* parameters[MAX_NB_PiSDF_PARAMS];

	UINT32 nbRepetition; // Stores the number of replicas in a Sr graph.

	bool visited; // Useful for graph browsing.
	bool executable; // Says whether the vertex can be executed.
	UINT32 tempId; // Used while creating a topology matrix.
public:
	BaseVertex();
	virtual ~BaseVertex();

	void addInputEdge(PiSDFEdge* edge);

	void addOutputEdge(PiSDFEdge* edge);

	void addParameter(PiSDFParameter* param);

	PiSDFEdge* getInputEdge(UINT32 index){
		if(index >= nbInputEdges) exitWithCode(1041);
		return inputEdges[index];
	}

	PiSDFEdge* getOutputEdge(UINT32 index){
		if(index >= nbOutputEdges) exitWithCode(1041);
		return outputEdges[index];
	}

	bool getExecutable(){
		return executable;
	}

	/*
	 * Set the executable field to true if the vertex is ready, that is :
	 * 1. The parameters on which the vertex depends are solved.
	 * 2. The source vertices on its inputs can be executable.
	 * 3. The consumption is not 0 nor exceeds the initial tokens (delay).
	 * Note that for hierarchical vertices, the function is call for each vertex
	 * of the sub-graph.
	 */
	void checkForExecution();


	/*
	 * Auto-generated getters and setters.
	 */

    UINT32 getId() const
    {
        return id;
    }

    UINT64 getNbInputEdges() const
    {
        return nbInputEdges;
    }

    UINT64 getNbOutputEdges() const
    {
        return nbOutputEdges;
    }

    UINT64 getFunction_index() const
    {
        return function_index;
    }

    char* getName()
    {
        return name;
    }

    VERTEXT_TYPE getType() const
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

    bool getVisited() const
    {
        return visited;
    }

    UINT32 getTempId() const
    {
        return tempId;
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

    void setType(VERTEXT_TYPE type)
    {
    	this->type = type;
    }

    void setNbRepetition(UINT32 nbRepetition)
    {
        this->nbRepetition = nbRepetition;
    }

    void setVisited(bool visited)
    {
        this->visited = visited;
    }


    void setTempId(UINT32 tempId)
    {
        this->tempId = tempId;
    }

//    void setNbParameters(UINT8 nbParameters)
//    {
//        this->nbParameters = nbParameters;
//    }

//    void setParameter(const UINT64 index, PiSDFParameter* parameter)
//    {
//        this->parameters[index] = parameter;
//    }

};

#endif /* BASEVERTEX_H_ */
