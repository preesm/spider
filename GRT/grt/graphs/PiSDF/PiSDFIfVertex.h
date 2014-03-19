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

#ifndef PISDFIFVERTEX_H_
#define PISDFIFVERTEX_H_

#include "../Base/BaseVertex.h"

class PiSDFIfVertex: public BaseVertex {
	BaseVertex* parentVertex;
	PiSDFEdge*	parentEdge;
	UINT16 parentEdgeIndex;
	UINT8 direction;
public:

	void reset(){
		this->BaseVertex::reset();
		parentVertex = (BaseVertex *)NULL;
		parentEdge = (PiSDFEdge *)NULL;
		parentEdgeIndex = -1;
		direction = 0;
	}

    BaseVertex *getParentVertex() const
    {
        return parentVertex;
    }

    void setParentVertex(BaseVertex *parentVertex)
    {
        this->parentVertex = parentVertex;
    }

    UINT8 getDirection() const
    {
        return direction;
    }

    void setDirection(UINT8 direction)
    {
        this->direction = direction;
    }

    UINT16 getParentEdgeIndex() const
    {
        return parentEdgeIndex;
    }

    void setParentEdgeIndex(UINT16 parentEdgeIndex)
    {
        this->parentEdgeIndex = parentEdgeIndex;
    }

    PiSDFEdge* getParentEdge() const
    {
    	return this->parentEdge;
    }

    void setParentEdge(PiSDFEdge *parentEdge)
    {
        this->parentEdge = parentEdge;
    }
};

#endif /* PISDFIFVERTEX_H_ */
