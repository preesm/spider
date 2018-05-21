/* Standard headers for Papify test applications.
    This file is customized to hide Windows / Unix differences.
*/

#ifndef PAPIFY_EVENT_LIB_H
#define PAPIFY_EVENT_LIB_H

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>
#if (!defined(NO_DLFCN) && !defined(_BGL) && !defined(_BGP))
#include <dlfcn.h>
#endif

#include <errno.h>
#if !defined(__FreeBSD__) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include "papiStdEventDefs.h"

#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"
#define NORMAL "\033[0m"

static int TESTS_COLOR = 0;

/*
* Papify_action_s structure stores the information related to the monitoring of the function being monitored
*
*/

typedef struct papify_action_s {
	char *action_id;			// Name of the function being monitored
	long long *counterValues; 		// Total number of events associated to the function execution
	long long *counterValuesStart; 		// Starting point
	long long *counterValuesStop; 		// End point (required to measure events by differences)
	char *component_id;			// PAPI component associated to the PE executing the function
	char *PE_id;			// ID associated to the eventSet to be monitored. This ID needs to be different for functions executed in parallel, as the eventSets are associated to specific threads
	int num_counters;			// Number of events being monitored
	unsigned long long time_init_action;	// Starting time of the function
	unsigned long long time_end_action;	// Ending time of the function
	FILE* papify_output_file;		// File where the monitoring data will be stored
	int* papify_eventCodeSet;		// Code of the events that are being monitored
	int papify_eventSet;			// EventSet associated to the monitoring of the function
	
} papify_action_s;


/**
 * @brief
 *
 * @param someAction
 * @param eventCodeSetSize
 * @param eventCodeSet
 */
void structure_test(papify_action_s *someAction, int eventCodeSetSize, int *eventCodeSet);

/**
 * @brief Initialize PAPI library and get the init time (this function should be the called before any other monitoring function)
 *
 * @remark It also stores the value of time_zero, which should be the starting point of the program.
 */
void event_init(void);

/**
 * @brief Initialize PAPI library and multiplex functionalities
 *
 */
void event_init_multiplex(void);

/**
 * @brief Activates the multiplexing functionality when monitoring the system
 *
 * @param papify_action
 *
 */
void eventList_set_multiplex_unified(papify_action_s* papify_action);

/**
 * @brief Creates
 *
 * @param papify_action
 *
 */
void event_create_eventList_unified(papify_action_s* papify_action);

/**
 * @brief Launch the monitoring of the eventSet. This eventSet will be counting from the beginning and the actual values will
 * be computed by differences with event_start and event_stop functions
 *
 * @param papify_action
 * @param eventSet_id
 *
 *
 */
void event_launch(papify_action_s* papify_action, int eventSet_id);

/**
 * @brief Read the current values of the event counters and stores them as the starting point
 *
 * @param papify_action
 * @param eventSet_id
 */
void event_start(papify_action_s* papify_action, int eventSet_id);

/**
 * @brief Read the current values of the event counters and stores them as the ending point.
 * After that, the total value is computed by differences and stored as the total value
 *
 * @param papify_action
 * @param eventSet_id
 */
void event_stop(papify_action_s* papify_action, int eventSet_id);

/**
 * @brief Create the .csv file associated to each function and prints the header of the file
 *
 * @param papify_action
 * @param actorName
 * @param all_events_name
 */
void event_init_output_file(papify_action_s* papify_action, char* actorName, char* all_events_name);

/**
 * @brief This function translates the name of the events to be monitored to the PAPI codes associated to each event
 *
 * @param papify_action
 * @param code_set_size
 * @param all_events_name
 */
void event_init_event_code_set(papify_action_s* papify_action, int code_set_size, char* all_events_name);

/**
 * @brief This function initializes all the variables of the papify_action_s
 *
 * @param papify_action
 * @param componentName
 * @param PEName
 * @param actorName
 * @param num_events
 */
void event_init_papify_actions(papify_action_s* papify_action, char* componentName, char* PEName, char* actorName, int num_events);

/**
 * @brief This function stores the starting point of the timing monitoring using PAPI_get_real_usec() function
 *
 * @param papify_action
 */
void event_start_papify_timing(papify_action_s* papify_action);


/**
 * @brief This function stores the ending point of the timing monitoring using PAPI_get_real_usec() function
 *
 * @param papify_action
 */
void event_stop_papify_timing(papify_action_s* papify_action);

/**
 * @brief This function configures all the monitoring environment
 * (initialization, file creation, event translation, eventSet creation and launches the monitoring)
 *
 * @param papify_action
 * @param componentName
 * @param PEName
 * @param actorName
 * @param num_events
 * @param all_events_name
 * @param eventSet_id
 *
 * @remark It should be noted that, so far, Papify supports the monitoring of the same events for all the functions executed by each PE.
 */
void configure_papify(papify_action_s* papify_action, char* componentName, char* PEName, char* actorName, int num_events, char* all_events_name, int eventSet_id);

#endif