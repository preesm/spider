/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifndef LRT_H
#define LRT_H

#include <tools/Stack.h>
#include <platform.h>

#define NB_MAX_ACTOR (200)

typedef void (*lrtFct)(
		void* inputFIFOs[],
		void* outputFIFOs[],
		Param inParams[],
		Param outParams[]);

class LRT{
public:
	LRT(int ix);
	virtual ~LRT();

	void setFctTbl(const lrtFct fct[], int nFct);

	int  runOneJob();
	void runUntilNoMoreJobs();
	void runInfinitly();

	inline void setIdle(bool idle);
	inline void setJobIx(int jobIx);
	inline bool isIdle();

	inline void rstJobIx();

	inline int getIx() const;
	inline int getJobIx() const;

protected:
	void sendTrace(int srdagIx, Time start, Time end);

private:
	int ix_;
	int nFct_;
	const lrtFct* fcts_;
	bool run_;
	bool idle_;
	int jobIx_;
	int jobIxTotal_;

	int tabBlkLrtIx[NB_MAX_ACTOR];
	int tabBlkLrtJobIx[NB_MAX_ACTOR];

#ifdef VERBOSE_TIME
	Time time_waiting_job;
	Time time_waiting_prev_actor;
	Time time_waiting_input_comm;
	Time time_compute;
	Time time_waiting_output_comm;
	Time time_global;

	Time time_alloc_data;

	Time time_other;

	Time start_waiting_job;

	int nb_iter;
#endif
};

inline int LRT::getIx() const{
	return ix_;
}

inline int LRT::getJobIx() const{
	return jobIx_;
}

inline void LRT::setIdle(bool idle){
	idle_ = idle;
}

inline bool LRT::isIdle(){
	return idle_;
}

inline void LRT::setJobIx(int jobIx){
	jobIx_ = jobIx;
}

inline void LRT::rstJobIx(){
	jobIx_ = 0;
}

#endif/*LRT_H*/
