/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 

#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>


using namespace std;

#include "../SchedulerDimensions.h"

void toupperStr(char upper[], const char str[]);

typedef struct variable_st{
    char name[VARIABLE_NAME_LEN_MAX+1];
    int value;
} variable;

/**
 List of variables to be used in an expression parser

 @author: mpelcat
*/

class Variablelist {

    private:
		/**
		 The stored variables
		*/
        variable variables[MAX_VARIABLE_NUMBER];
		int variablesNr;

    public:
		/*
		 * Constructor
		 */
		Variablelist();

        bool exist(const char* name);
        bool add(const char* name, int value);
        bool del(const char* name);

        bool getValue(const char* name, int* value);
        bool getValue(const int id, int* value);
        int  getId(const char* name);

		variable* getVariable(const char* name);
};


#endif

