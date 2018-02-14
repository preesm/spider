/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <spider.h>

typedef enum{
	MSG_START_JOB=1,
	MSG_PARAM_VALUE=2,
	MSG_CLEAR_TIME=3,
	MSG_RESET_LRT=4,
	MSG_END_ITER=5,
	MSG_STOP_LRT=6
}CtrlMsgType;

typedef enum{
	TRACE_JOB=1,
	TRACE_SPIDER=2
}TraceMsgType;

typedef enum{
	TRACE_SPIDER_GRAPH=1,
	TRACE_SPIDER_ALLOC=2,
	TRACE_SPIDER_SCHED=3,
	TRACE_SPIDER_OPTIM=4,
	TRACE_SPIDER_TMP0,
	TRACE_SPIDER_TMP1,
	TRACE_SPIDER_TMP2,
	TRACE_SPIDER_TMP3
}TraceSpiderType;

typedef struct {
	unsigned long msgIx;
//	unsigned char msgIx:2;
//	unsigned char reserved:6;
}UndefinedMsg;

typedef struct {
//	unsigned char msgIx:2;
//	unsigned long  srdagIx:29;
//	unsigned char  specialActor:1;
//	unsigned short fctIx:16;
//	unsigned char nbInEdge:8;
//	unsigned char nbOutEdge:8;
//	unsigned char nbInParam:8;
//	unsigned char nbOutParam:8;
	unsigned long msgIx;
	unsigned long srdagIx;
	unsigned long specialActor;
	unsigned long fctIx;
	unsigned long traceEnabled;
	unsigned long nbInEdge;
	unsigned long nbOutEdge;
	unsigned long nbInParam;
	unsigned long nbOutParam;
}StartJobMsg;

typedef struct {
	unsigned long msgIx;
	unsigned long spiderTask;
	unsigned long srdagIx;
	unsigned long lrtIx;
	Time start;
	Time end;
}TraceMsg;

typedef struct {
	unsigned long id:32; // Deprecated
	unsigned long alloc:32;
	unsigned long size:32;
	unsigned long ntoken:32;
	unsigned long blkLrtIx:32;
	unsigned long blkLrtJobIx:32;
} Fifo;

typedef struct {
	unsigned long msgIx;
	unsigned long srdagIx;
//	unsigned char msgIx:2;
//	unsigned long srdagIx:30;
}ParamValueMsg;

typedef struct ClearTimeMsg{
	unsigned long msgIx;
//	unsigned char msgIx:2;
//	unsigned char reserved:6;
} ClearTimeMsg;

typedef struct {
	unsigned long msgIx;
//	unsigned char msgIx:2;
//	unsigned char reserved:6;
}ResetLrtMsg;

typedef struct {
	unsigned long msgIx;
//	unsigned char msgIx:2;
//	unsigned char reserved:6;
}EndIterMsg;

typedef struct {
	unsigned long msgIx;
//	unsigned char msgIx:2;
//	unsigned char reserved:6;
}StopLrtMsg;

#endif/*MESSAGE_H*/
