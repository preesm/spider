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

#include "topologyMatrix.h"
#include "GraphTransfo.h"

#include <tools/Stack.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct{
	int nom;
	int den;
} Rational;

static inline int compute_gcd(int a, int b){
	int t;
    while (b != 0){
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static inline int compute_lcm(int a, int b){
	if(a*b == 0) return 1;
	return abs(a*b)/compute_gcd(a,b);
}

static inline void reduce(Rational* r){
	int gcd = compute_gcd(r->nom, r->den);
	r->nom /= gcd;
	r->den /= gcd;
}

static inline Rational Rational_Add(Rational a, Rational b){
	Rational res;
	int lcm = compute_lcm(a.den, b.den);
	res.den = lcm;
	res.nom = lcm*a.nom/a.den + lcm*b.nom/b.den;
	reduce(&res);
	return res;
}

static inline Rational Rational_Sub(Rational a, Rational b){
	Rational res;
	int lcm = compute_lcm(a.den, b.den);
	res.den = lcm;
	res.nom = lcm*a.nom/a.den - lcm*b.nom/b.den;
	reduce(&res);
	return res;
}

static inline Rational Rational_Mul(Rational a, Rational b){
	Rational res;
	res.nom = a.nom*b.nom;
	res.den = a.den*b.den;
	reduce(&res);
	return res;
}

static inline Rational Rational_Div(Rational a, Rational b){
	Rational res;
	res.nom = a.nom*b.den;
	res.den = a.den*b.nom;
	reduce(&res);
	return res;
}

static inline float Rational_getVal(Rational a){
	return ((float)a.nom)/a.den;
}

static inline Rational Rational_getAbs(Rational a){
	Rational r;
	r.nom = abs(a.nom);
	r.den = abs(a.den);
	return r;
}

static inline float Rational_compSup(Rational a, Rational b){
	return Rational_getVal(Rational_Sub(a, b));
}

static inline bool Rational_isNull(Rational a){
	return a.nom == 0;
}

int nullSpace(int* topo_matrix, int* brv, int nbEdges, int nbVertices, Stack *stack){
	Rational* ratioMatrix = sAlloc(stack, nbVertices*nbEdges, Rational);
	Rational* ratioResult = sAlloc(stack, nbVertices, Rational);
	int i;

	/* Copy matrix into ratioMatrix */
	for(i=0; i<nbEdges*nbVertices; i++){
		ratioMatrix[i].nom = topo_matrix[i];
		ratioMatrix[i].den = 1;
	}

	for (i=0; i < nbVertices; i++) {
		int t;
		Rational pivotMax = {0,1};
		int maxIndex = i;
		for (t = i; t < nbEdges; t++) {
			Rational abs = Rational_getAbs(ratioMatrix[i*nbVertices+t]);
			if (Rational_compSup(abs, pivotMax)) {
				maxIndex = t;
				pivotMax = abs;
			}
		}

		if (!Rational_isNull(pivotMax) && maxIndex != i) {
			/* Switch Rows */
			Rational tmp;
			for (t = 0; t < nbVertices; t++) {
				tmp = ratioMatrix[maxIndex*nbVertices+t];
				ratioMatrix[maxIndex*nbVertices+t] = ratioMatrix[i*nbVertices+t];
				ratioMatrix[i*nbVertices+t] = tmp;
			}
		} else if (maxIndex == i && !Rational_isNull(pivotMax)) {
			/* Do nothing */
		} else {
			break;
		}

		Rational odlPivot = ratioMatrix[i*nbVertices+i];
		for (t = i; t < nbVertices; t++) {
			ratioMatrix[i*nbVertices+t] = Rational_Div(ratioMatrix[i*nbVertices+t],odlPivot);
		}

		int j;
		for (j = i + 1; j < nbEdges; j++) {
			if (!Rational_isNull(ratioMatrix[j*nbVertices+i])) {
				Rational oldji = ratioMatrix[j*nbVertices+i];
				int k;
				for (k = 0; k < nbVertices; k++) {
					ratioMatrix[j*nbVertices+k] =
							Rational_Sub(ratioMatrix[j*nbVertices+k],
									Rational_Mul(oldji,ratioMatrix[i*nbVertices+k]));
				}
			}
		}
	}

	for (i = 0; i < nbVertices; i++) {
		ratioResult[i].nom=1;
		ratioResult[i].den=1;
	}

	for(i = nbEdges-1; i >= 0; i--){
		int k;
		Rational val = {0,1};
		for (k = i + 1; k < nbVertices; k++) {
			val = Rational_Add(val, Rational_Mul(ratioMatrix[i*nbVertices+k], ratioResult[k]));
		}
		if (!Rational_isNull(val)) {
			if(Rational_isNull(ratioMatrix[i*nbVertices+i])){
				printf("elt diagonal zero\n");
				return 1;
			}
			ratioResult[i] = Rational_Div(Rational_getAbs(val), ratioMatrix[i*nbVertices+i]);
		}
	}

	int lcm = 1;
	for(i=0; i<nbVertices; i++){
		lcm = compute_lcm(lcm, ratioResult[i].den);
	}
	for(i=0; i<nbVertices; i++){
		brv[i] = abs(ratioResult[i].nom * lcm / ratioResult[i].den);
	}
	return 0;
}
