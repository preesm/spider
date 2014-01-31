
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#ifndef TOP_AM_ACTIONS_H_
#define TOP_AM_ACTIONS_H_

/* Top level. */
void readVOL();
void decodeDsply();



/** DecodeDsply level 1 **/
void inputVOL_L1();
void inputComplexity();
void inputVOLPos();
void inputImgDimensions();
void broadVOL();
void readVOP();
void decodeVOP();
void displayVOP();

#if HIERARCHY_LEVEL > 1
//	void inputVOL_L2();
//	void inputVOPData_L2();
//	void inputFrmData_L2();
//	void inputPrevImg_L2();
	void setVOPType();
	void switch_0();
	void switch_1();
	void decodeVOP_I();
	void decodeVOP_P();
	void select_0();
	void select_1();
//	void outputImg();
#else
	void decodeVOP();
#endif


#endif /* TOP_AM_ACTIONS_H_ */
