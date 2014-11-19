/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef SRDAG_COMMON_H
#define SRDAG_COMMON_H

#include <tools/Set.h>
#include <tools/Iterator.h>

class SRDAGEdge;
class SRDAGParam;
class SRDAGGraph;
class SRDAGVertex;

#define BROADCAST_F_IX 		0
#define IMPLODE_F_IX 		1
#define EXPLODE_F_IX 		2
#define ROUNDBUFFER_F_IX 	3
#define INIT_F_IX 			4
#define END_F_IX 			5

typedef enum SRDAGType {
	SRDAG_NORMAL,
	SRDAG_BROADCAST,
	SRDAG_JOIN,
	SRDAG_FORK,
	SRDAG_ROUNDBUFFER,
	SRDAG_INIT,
	SRDAG_END
} SRDAGType;

typedef enum SRDAGState {
	SRDAG_NEXEC,
	SRDAG_EXEC,
	SRDAG_RUN
} SRDAGState;

/** Set types */
typedef Set<SRDAGEdge*> SRDAGEdgeSet;
typedef Set<SRDAGVertex*> SRDAGVertexSet;

/** Iterators types */
typedef SetIterator<SRDAGEdge*> SRDAGEdgeIterator;
typedef SetIterator<SRDAGVertex*> SRDAGVertexIterator;

#endif/*SRDAG_COMMON_H*/
