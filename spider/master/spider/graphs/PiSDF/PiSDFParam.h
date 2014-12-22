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

#ifndef PISDF_PARAM_H
#define PISDF_PARAM_H

#include <graphs/PiSDF/PiSDFCommon.h>

class PiSDFParam{
public:
	/** Constructor */
	PiSDFParam(
			const char* name,
			int typeIx,
			PiSDFGraph* graph,
			PiSDFParamType type);
	~PiSDFParam();

	/** Getters */
	inline int getIx() const;
	inline int getTypeIx() const;
	inline const char* getName() const;
	inline PiSDFParamType getType() const;
	inline int getStaticValue() const;
	inline int getParentId() const;

	/** Setters */
	inline void setValue(int value);
	inline void setParentId(int parentId);
	inline void setSetter(PiSDFVertex* setter, int portIx);

	int evaluate(const int * paramValues);

//	const PiSDFGraph* getGraph() const;

private:
	static int globalIx;

	int id_;
	int typeIx_;
	const char* name_;
	PiSDFGraph* graph_;
	PiSDFParamType type_;

	// STATIC
	int value_;

	// HERITED
	int parentId_;

	// DYNAMIC
	PiSDFVertex* setter_;
	int portIx_;
};

/** Inline Fcts */
/** Getters */
inline int PiSDFParam::getIx() const{
	return id_;
}
inline int PiSDFParam::getTypeIx() const{
	return typeIx_;
}
inline const char* PiSDFParam::getName() const{
	return name_;
}
inline PiSDFParamType PiSDFParam::getType() const{
	return type_;
}
inline int PiSDFParam::getStaticValue() const{
	if(type_ != PISDF_PARAM_STATIC)
		throw "Error: Not Static param used as Static param\n";
	return value_;
}
inline int PiSDFParam::getParentId() const{
	if(type_ != PISDF_PARAM_HERITED)
		throw "Error: Not Herited param used as Herited param\n";
	return value_;
}

/** Setters */
inline void PiSDFParam::setValue(int value){
	value_ = value;
}
inline void PiSDFParam::setParentId(int parentId){
	parentId_ = parentId;
}
inline void PiSDFParam::setSetter(PiSDFVertex* setter, int portIx){
	setter_ = setter;
	portIx_ = portIx;
}

#endif/*PISDF_PARAM_H*/
