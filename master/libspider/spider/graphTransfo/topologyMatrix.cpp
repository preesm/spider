/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2015)
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
#include "topologyMatrix.h"
#include "GraphTransfo.h"

#include <tools/Rational.h>

static inline int compute_gcd(int a, int b) {
    int t;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static inline int compute_lcm(int a, int b) {
    if (a * b == 0) return 1;
    return abs(a * b) / compute_gcd(a, b);
}

int nullSpace(int *topo_matrix, int *brv, int nbEdges, int nbVertices) {
    Rational *ratioMatrix = CREATE_MUL(TRANSFO_STACK, nbVertices * nbEdges, Rational);
    Rational *ratioResult = CREATE_MUL(TRANSFO_STACK, nbVertices, Rational);

	printf("Topo Matrix:\n");
	for(int i=0; i<nbEdges; i++){
		for(int j=0; j<nbVertices; j++){
			printf("%d : ", topo_matrix[i*nbVertices+j]);
		}
//		printf("\033[2D");
		printf("\n");
	}

    /* Copy matrix into ratioMatrix */
    for (int i = 0; i < nbEdges * nbVertices; i++) {
        ratioMatrix[i] = topo_matrix[i];
    }

	printf("Topo Matrix: Rational\n");
	for(int i=0; i<nbEdges; i++){
		for(int j=0; j<nbVertices; j++){
			printf("%d,%d : ", ratioMatrix[i*nbVertices+j].getNominator(), ratioMatrix[i*nbVertices+j].getDenominator());
		}
//		printf("\033[2D");
		printf("\n");
	}

    for (int i = 0; i < nbEdges; i++) {
        Rational pivotMax = ratioMatrix[i * nbVertices + i].getAbs();
        int maxIndex = i;

        for (int t = i + 1; t < nbEdges; t++) {
            Rational newPivot = ratioMatrix[t * nbVertices + i].getAbs();
            if (newPivot > pivotMax) {
                maxIndex = t;
                pivotMax = newPivot;
            }
        }

        if (pivotMax != 0 && maxIndex != i) {
            /* Switch Rows */
            Rational tmp;
            for (int t = 0; t < nbVertices; t++) {
                tmp = ratioMatrix[maxIndex * nbVertices + t];
                ratioMatrix[maxIndex * nbVertices + t] = ratioMatrix[i * nbVertices + t];
                ratioMatrix[i * nbVertices + t] = tmp;
            }
        } else if (maxIndex == i && (pivotMax != 0)) {
            /* Do nothing */
        } else {
            break;
        }

        Rational odlPivot = ratioMatrix[i * nbVertices + i];
        for (int t = i; t < nbVertices; t++) {
            ratioMatrix[i * nbVertices + t] = ratioMatrix[i * nbVertices + t] / odlPivot;
        }

        for (int j = i + 1; j < nbEdges; j++) {
            if (ratioMatrix[j * nbVertices + i] != 0) {
                Rational oldji = ratioMatrix[j * nbVertices + i];

                for (int k = 0; k < nbVertices; k++) {
                    ratioMatrix[j * nbVertices + k] =
                            ratioMatrix[j * nbVertices + k] - (oldji * ratioMatrix[i * nbVertices + k]);
                }
            }
        }
    }

    for (int i = 0; i < nbVertices; i++) {
        ratioResult[i] = 1;
    }

    for (int i = nbEdges - 1; i >= 0; i--) {
        Rational val = 0;

        for (int k = i + 1; k < nbVertices; k++) {
            val = val + (ratioMatrix[i * nbVertices + k] * ratioResult[k]);
        }
        if (val != 0) {
            if (ratioMatrix[i * nbVertices + i] == 0) {
                throw std::runtime_error("elt diagonal zero\n");
            }
            ratioResult[i] = val.getAbs() / ratioMatrix[i * nbVertices + i];
        }
    }

    int lcm = 1;
    for (int i = 0; i < nbVertices; i++) {
        lcm = compute_lcm(lcm, ratioResult[i].getDenominator());
    }
    for (int i = 0; i < nbVertices; i++) {
        brv[i] = abs(ratioResult[i].getNominator() * lcm / ratioResult[i].getDenominator());
    }

    StackMonitor::free(TRANSFO_STACK, ratioMatrix);
    StackMonitor::free(TRANSFO_STACK, ratioResult);

    return 0;
}
