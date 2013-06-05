/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef SCHEDULE_WRITER
#define SCHEDULE_WRITER

#include "../graphs/CSDAG/CSDAGGraph.h"
#include "../graphs/CSDAG/CSDAGVertex.h"
#include "../graphs/CSDAG/CSDAGEdge.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/SRDAG/SRDAGVertex.h"
#include "../graphs/SRDAG/SRDAGEdge.h"
#include "../scheduling/architecture/Architecture.h"


/**
 * Writes a schedule file from a SRDAG graph. The schedule 
 * is written in a XML file in two formats:
 * - one that can be read by the Java Gantt Chart Plotter
 * - one (in a comment tag) that generates Latex Gantt for the paper
 * 
 * @author mpelcat
 */
class ScheduleWriter {

	private :

	public : 
		/**
		 Constructor
		*/
		ScheduleWriter();


		/**
		 Destructor
		*/
		~ScheduleWriter();


		/**
		 Writes a schedule in a file

		 @param csGraph: reference CSDAG
		 @param hGraph: implemented SRDAG
		 @param archi: architecture
		 @param path: output file path
		*/
		void write(CSDAGGraph* csGraph, SRDAGGraph* hGraph, Architecture* archi, const char* path);


		/**
		 Exports the speedups to display them with matlab

		 @param csGraph: reference CSDAG
		 @param hGraph: implemented SRDAG
		 @param archi: architecture
		 @param path: output file path
		*/
		void exportSpeedups(CSDAGGraph* csGraph, SRDAGGraph* hGraph, Architecture* archi, char* speedupsPath, char* spansPath, char* worksPath);

};

#endif
