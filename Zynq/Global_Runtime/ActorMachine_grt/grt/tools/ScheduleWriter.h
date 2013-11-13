
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

#ifndef SCHEDULE_WRITER
#define SCHEDULE_WRITER

#include "../graphs/CSDAG/CSDAGGraph.h"
#include "../graphs/CSDAG/CSDAGVertex.h"
#include "../graphs/CSDAG/CSDAGEdge.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/SRDAG/SRDAGVertex.h"
#include "../graphs/SRDAG/SRDAGEdge.h"
#include "../scheduling/Schedule/Schedule.h"
#include "../scheduling/Schedule/BaseSchedule.h"
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
		void write(Schedule* schedule, SRDAGGraph* hGraph, Architecture* archi, const char* path);
		void write(BaseSchedule* schedule, SRDAGGraph* hGraph, Architecture* archi, const char* path);

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
