/*
 * ScheduleChecker.h
 *
 *  Created on: Jan 24, 2013
 *      Author: jheulot
 */

#ifndef SCHEDULECHECKER_H_
#define SCHEDULECHECKER_H_

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/JobSet/JobSet.h"
#include "../graphs/Schedule/Schedule.h"

class ScheduleChecker {
public:
	ScheduleChecker();
	virtual ~ScheduleChecker();

	void checkList(SRDAGGraph* srDag, Architecture* archi);
	void checkFlow(SRDAGGraph* srDag, JobSet* jobset, Architecture* archi);

	void checkSchedule(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi);
};

#endif /* SCHEDULECHECKER_H_ */
