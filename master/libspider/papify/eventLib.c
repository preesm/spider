#include "eventLib.h"
#include "papi.h"

#include <time.h>

pthread_mutex_t lock;

unsigned long long time_zero;

int papify_eventSet_PEs[500];
int papify_eventSet_PEs_launched[500];

/* 
* Print the structure to check whether it is configured correctly
*/
void structure_test(papify_action_s *someAction, int eventCodeSetSize, int *eventCodeSet) {
    int i;
    printf("Action name: %s\n", someAction->action_id);
    printf("Event Code Set:\n");
    for(i=0; i<eventCodeSetSize; i++){
        printf("\t-%d\n", eventCodeSet[i]);
    }
}

/* 
* Test function to check where the monitoring is failing
*/
static void test_fail( char *file, int line, char *call, int retval ) {

    int line_pad;
    char buf[128];

    line_pad = (int) (50 - strlen(file));
    if (line_pad < 0) {
        line_pad = 0;
    }

    memset(buf, '\0', sizeof(buf));

    if (TESTS_COLOR) {
        fprintf(stdout, "%-*s %sFAILED%s\nLine # %d\n", line_pad, file, RED,
                NORMAL, line);
    } else {
        fprintf(stdout, "%-*s FAILED\nLine # %d\n", line_pad, file, line);
    }

    if (retval == PAPI_ESYS) {
        sprintf(buf, "System error in %s", call);
        perror(buf);
    } else if (retval > 0) {
        fprintf(stdout, "Error: %s\n", call);
    } else if (retval == 0) {
    #if defined(sgi)
        fprintf( stdout, "SGI requires root permissions for this test\n" );
    #else
        fprintf(stdout, "Error: %s\n", call);
    #endif
    } else {
        fprintf(stdout, "Error in %s: %s\n", call, PAPI_strerror(retval));
    }

    fprintf(stdout, "\n");

    exit(1);
}

/* 
* Initialize multiplex functionalities
*/

static void init_multiplex( void ) {

    int retval;
    const PAPI_hw_info_t *hw_info, *hw_info_register32;
    const PAPI_component_info_t *cmpinfo, *cmpinfo_register32;

    /* Initialize the library */

    /* for now, assume multiplexing on CPU compnent only */
    cmpinfo = PAPI_get_component_info(0);
    if (cmpinfo == NULL) {
        test_fail(__FILE__, __LINE__, "PAPI_get_component_info", 2);
    }

    hw_info = PAPI_get_hardware_info();
    if (hw_info == NULL) {
        test_fail(__FILE__, __LINE__, "PAPI_get_hardware_info", 2);
    }

    if ((strstr(cmpinfo->name, "perfctr.c")) && (hw_info != NULL )
            && strcmp(hw_info->model_string, "POWER6") == 0) {
        retval = PAPI_set_domain(PAPI_DOM_ALL);
        if (retval != PAPI_OK) {
            test_fail(__FILE__, __LINE__, "PAPI_set_domain", retval);
        }
    }
    retval = PAPI_multiplex_init();
    if (retval != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI multiplex init fail\n", retval);
    }
}



void event_init(void) {

    int retval;

    // library initialization
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT )
        test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

    // place for initialization in case one makes use of threads
    retval = PAPI_thread_init((unsigned long (*)(void))(pthread_self));
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_thread_init", retval );

    printf("event_init done \n");
    time_zero = PAPI_get_real_usec();

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
    }
}


void event_init_multiplex(void) {

    int retval;
    int i;

    for(i = 0; i < 50; i++){
	papify_eventSet_PEs[i] = PAPI_NULL;
	papify_eventSet_PEs_launched[i] = 0;
    }

    // library initialization
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT )
        test_fail( __FILE__, __LINE__, "PAPI_library_init", retval );

    // place for initialization in case one makes use of threads
    retval = PAPI_thread_init((unsigned long (*)(void))(pthread_self));
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_thread_init", retval );

    // multiplex initialization
    init_multiplex(  );

    printf("event_init done \n");
    time_zero = PAPI_get_real_usec();

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
    }
}


void eventList_set_multiplex_unified(papify_action_s* papify_action) {
	int retval;

    retval = PAPI_set_multiplex( papify_action[0].papify_eventSet );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_set_multiplex", retval );
}

void event_create_eventList_unified(papify_action_s* papify_action) {

    int retval, i, maxNumberHwCounters, eventCodeSetMaxSize;
    PAPI_event_info_t info;
    unsigned long threadId;

    maxNumberHwCounters = PAPI_get_opt( PAPI_MAX_HWCTRS, NULL );
    eventCodeSetMaxSize = PAPI_get_opt( PAPI_MAX_MPX_CTRS, NULL );

    if ( eventCodeSetMaxSize < papify_action[0].num_counters)
        test_fail( __FILE__, __LINE__, "eventCodeSetMaxSize < eventCodeSetSize, too many performance events defined! ", retval );

    threadId = PAPI_register_thread();
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_register_thread", retval );

    retval = PAPI_create_eventset( &papify_action->papify_eventSet );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_create_eventset", retval );

    retval = PAPI_assign_eventset_component( papify_action[0].papify_eventSet, PAPI_get_component_index(papify_action[0].component_id));
    if ( retval == PAPI_ENOCMP )
   	retval = PAPI_assign_eventset_component( papify_action[0].papify_eventSet, 0 );

    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_assign_eventset_component", retval );

    eventList_set_multiplex_unified(papify_action);
    retval = PAPI_attach( papify_action[0].papify_eventSet, threadId );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_attach_thread", retval );
    for (i = 0; i < papify_action[0].num_counters; i++) {
        retval = PAPI_get_event_info(papify_action[0].papify_eventCodeSet[i], &info);
        if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_get_event_info", retval );
        retval = PAPI_add_event( papify_action[0].papify_eventSet, info.event_code);
        if ( retval != PAPI_OK )
            test_fail( __FILE__, __LINE__, "PAPI_add_event", retval );
    }
}


void event_launch(papify_action_s* papify_action, int eventSet_id) {

    int retval;
    papify_eventSet_PEs[eventSet_id] = papify_action[0].papify_eventSet;
    retval = PAPI_start( papify_eventSet_PEs[eventSet_id] );
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_start",retval );
}


void event_start(papify_action_s* papify_action, int eventSet_id){

    int retval;
    if(papify_action[0].num_counters != 0){
    	retval = PAPI_read( papify_eventSet_PEs[eventSet_id], papify_action->counterValuesStart );
    }
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_read",retval );
}


void event_stop(papify_action_s* papify_action, int eventSet_id) {

    int retval, i;
    if(papify_action[0].num_counters != 0){
    	retval = PAPI_read( papify_eventSet_PEs[eventSet_id], papify_action->counterValuesStop );
	for(i = 0; i < papify_action[0].num_counters; i++){
	    papify_action[0].counterValues[i] = papify_action[0].counterValuesStop[i] - papify_action[0].counterValuesStart[i];
	}
    }
    if ( retval != PAPI_OK )
        test_fail( __FILE__, __LINE__, "PAPI_read", retval );

}

void event_init_output_file(papify_action_s* papify_action, char* actorName, char* all_events_name) {
	
	char file_name[256];
	snprintf(file_name, sizeof file_name, "%s%s%s", "papify-output/papify_output_", actorName, ".csv");

	char output_string[1024];
	snprintf(output_string, sizeof output_string, "%s%s%s", "Actor,Action,tini,tend,", all_events_name, "\n");

	papify_action->papify_output_file = fopen(file_name,"w");
	fprintf(papify_action->papify_output_file,output_string);
	fclose(papify_action->papify_output_file);
}


void event_init_event_code_set(papify_action_s* papify_action, int code_set_size, char* all_events_name) {
	
	int i = 0;
	int event_code = 0;
	int retval;
	char all_events_name_local[250];

	snprintf(all_events_name_local, sizeof(all_events_name_local), "%s", all_events_name);
	char* event_name;
	char codeName_aux[250];

	event_name = strtok(all_events_name_local, ",");

	for(i = 0; i < code_set_size; i++){
		
		retval = PAPI_event_name_to_code(event_name, &event_code);
		if ( retval != PAPI_OK ) {
	  		test_fail( __FILE__, __LINE__, 
		      "Translation of event not found\n", retval );
		}

        	event_name = strtok(NULL, ",");

		papify_action[0].papify_eventCodeSet[i] = event_code;
	}
}


void event_init_papify_actions(papify_action_s* papify_action, char* componentName, char* PEName, char* actorName, int num_events) {

	papify_action->counterValues = malloc(sizeof(long long) * num_events);
	papify_action->counterValuesStart = malloc(sizeof(long long) * num_events);
	papify_action->counterValuesStop = malloc(sizeof(long long) * num_events);

	papify_action[0].action_id = malloc(strlen(actorName)+1);
	snprintf(papify_action[0].action_id, (strlen(actorName)+1) * sizeof(char), "%s", actorName);

	papify_action[0].component_id = malloc(strlen(componentName)+1);
	snprintf(papify_action[0].component_id, (strlen(componentName)+1) * sizeof(char), "%s", componentName);

	papify_action[0].PE_id = malloc(strlen(PEName)+1);
	snprintf(papify_action[0].PE_id, (strlen(PEName)+1) * sizeof(char), "%s", PEName);

	papify_action[0].num_counters = num_events;

	papify_action->papify_eventCodeSet = malloc(sizeof(int) * num_events);
	papify_action->papify_eventSet = malloc(sizeof(int) * 1);
	papify_action->papify_eventSet = PAPI_NULL;
	papify_action->papify_output_file = malloc(sizeof(FILE) * 1);
}

void event_start_papify_timing(papify_action_s* papify_action) {
	
	papify_action[0].time_init_action = PAPI_get_real_usec() - time_zero;
}


void event_stop_papify_timing(papify_action_s* papify_action) {
	
	papify_action[0].time_end_action = PAPI_get_real_usec() - time_zero;
}

/* 
* This function configures all the monitoring environment (initialization, file creation, event translation, eventSet creation and launches the monitoring)
* It should be noted that, so far, Papify supports the monitoring of the same events for all the functions executed by each PE. 
*/
void configure_papify(papify_action_s* papify_action, char* componentName, char* PEName, char* actorName, int num_events, char* all_events_name, int eventSet_id) {

	event_init_papify_actions(papify_action, componentName, PEName, actorName, num_events);
	event_init_output_file(papify_action, actorName, all_events_name);

	pthread_mutex_lock(&lock);
    	if(papify_eventSet_PEs_launched[eventSet_id] == 0 && papify_action[0].num_counters != 0){
		event_init_event_code_set(papify_action, num_events, all_events_name);
	
		event_create_eventList_unified(papify_action);
		event_launch(papify_action, eventSet_id);
		papify_eventSet_PEs_launched[eventSet_id] = 1;
	}
	pthread_mutex_unlock(&lock);
}

/* 
* This function writes all the monitoring information as a new line of the .csv file
*/
void event_write_file(papify_action_s* papify_action){

	char output_file_name[250];
	int i;

	snprintf(output_file_name, sizeof output_file_name, "%s%s%s", "papify-output/papify_output_", papify_action[0].action_id, ".csv");

	papify_action[0].papify_output_file = fopen(output_file_name,"a+");

	fprintf(papify_action[0].papify_output_file, "%s,%s,%llu,%llu", papify_action[0].PE_id, papify_action[0].action_id, papify_action[0].time_init_action, papify_action[0].time_end_action);

	for(i = 0; i < papify_action[0].num_counters; i++){
		fprintf(papify_action[0].papify_output_file, ",%lu", papify_action[0].counterValues[i]);
	}
	fprintf(papify_action[0].papify_output_file, "\n");

	fclose(papify_action[0].papify_output_file);
}

