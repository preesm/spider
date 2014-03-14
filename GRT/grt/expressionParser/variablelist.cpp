/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#include "variablelist.h"
#include "../tools/SchedulingError.h"

/**
 List of variables to be used in an expression parser

 @author: mpelcat
*/

/*
 * Constructor
 */
Variablelist::Variablelist()
{
	variablesNr = 0;
	memset(variables,'\0', MAX_VARIABLE_NUMBER*sizeof(variable));
}

/*
 * Returns true if the given name already exists in the variable list
 */
bool Variablelist::exist(const char* name)
{
    return (getId(name) != -1);
}


/*
 * Add a name and value to the variable list
 */
bool Variablelist::add(const char* name, int value)
{

#ifdef DISPLAY
	//printf("varAdd: %s %d\n", name,value);
#endif

    int id = getId(name);
	variable* new_var = NULL;

    if (id == -1)
    {
		if(variablesNr == MAX_VARIABLE_NUMBER){
			// Adding a variable in an already full variable table
			exitWithCode(1012);
		}

		new_var = &variables[variablesNr];
		variablesNr++;
    }
    else
    {
        // variable already exists. overwrite it
		new_var = &variables[id];
    }
	
    strcpy(new_var->name, name);
    new_var->value = value;

    return true;
}

/*
 * Delete given variablename from the variable list
 */
bool Variablelist::del(const char* name)
{
    int id = getId(name);
    if (id != -1)
    {
		for(int i = id; i < variablesNr-1; i++){
			variable* current = &variables[i];
			variable* next = &variables[i+1];
			strcpy(current->name, next->name);
			current->value = next->value;
		}
		variablesNr--;
        return true;
    }
    return false; 
}

/*
 * Get value of variable with given name
 */
bool Variablelist::getValue(const char* name, int* value)
{
    int id = getId(name);
    if (id != -1)
    {
        *value = variables[id].value;
        return true;
    }
    return false;
}


/*
 * Get value of variable with given id
 */
bool Variablelist::getValue(const int id, int* value)
{
    if (id >=0 && id < variablesNr)
    {
        *value = variables[id].value;
        return true;
    }
    return false;
}

/*
 * Returns the id of the given name in the variable list. Returns -1 if name
 * is not present in the list. Name is case insensitive
 */
int Variablelist::getId(const char* name)
{
    for (int i = 0; i < variablesNr; i++)
    {
        if (strcmp(name,  variables[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

/*
 * Returns the variable corresponding to the current name.
 */
variable* Variablelist::getVariable(const char* name){
    for (int i = 0; i < variablesNr; i++)
    {
        if (strcmp(name,  variables[i].name) == 0)
        {
            return &variables[i];
        }
    }

	// Searching a non existing variable while creating an RPN
	exitWithCode(1027);
    return NULL;
}
