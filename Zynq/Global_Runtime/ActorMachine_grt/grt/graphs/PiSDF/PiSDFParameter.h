
/********************************************************************************
 * Copyright or � or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-Fran�ois Nezan, Jean-Christophe Prevotet			*
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

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <SchedulerDimensions.h>
#include <expressionParser/ReversePolishNotationGenerator.h>
#include <types.h>

class PiSDFParameter {
	char name[MAX_PARAM_NAME_SIZE];
	variable* var;
	bool resolved;
	// Expression defining the parameter's value (in abstract_syntax_elt)
//	abstract_syntax_elt production[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
public:
	PiSDFParameter(){resolved = false;};

	void setValue(UINT32 value){
		this->var->value = value;
		setResolved(true);
	}

	UINT32 getValue(){
		return this->var->value;
	}

	/*
	 * Auto-generated setters and getters.
	 */
    char* getName()
    {
        return name;
    }

    variable *getVariable() const
    {
        return var;
    }

    bool getResolved() const
    {
        return resolved;
    }

    void setName(const char* name)
    {
    	strcpy(this->name, name);
    }

    void setVariable(variable *var)
    {
        this->var = var;
    }

    void setResolved(bool resolved)
    {
        this->resolved = resolved;
    }
};

#endif /* PARAMETER_H_ */