/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2016) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#ifndef PISDF_PARAM_H
#define PISDF_PARAM_H

#include <tools/SetElement.h>

#include <graphs/PiSDF/PiSDFCommon.h>

class Expression;

class PiSDFParam : public SetElement{
public:
	/** Constructor */
	PiSDFParam(
			const char* name,
			int typeIx,
			PiSDFGraph* graph,
			PiSDFParamType type,
			const char* expr);
	~PiSDFParam();

	/** Getters */
	inline int getIx() const;
	inline int getTypeIx() const;
	inline const char* getName() const;
	inline PiSDFParamType getType() const;
	inline int getStaticValue() const;
	inline int getParentId() const;
	inline Expression* getExpression();

	/** Setters */
	inline void setValue(int value);
	inline void setParentId(int parentId);
	inline void setSetter(PiSDFVertex* setter, int portIx);
	inline void setExpression(Expression* expr);

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

	// DEPENDENT
	Expression* expr_;
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
		throw std::runtime_error("Error: Not Static param used as Static param\n");
	return value_;
}
inline int PiSDFParam::getParentId() const{
	if(type_ != PISDF_PARAM_HERITED)
		throw std::runtime_error("Error: Not Herited param used as Herited param\n");
	return parentId_;
}
inline Expression* PiSDFParam::getExpression(){
	return expr_;
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
inline void PiSDFParam::setExpression(Expression* expr){
	expr_ = expr;
}

#endif/*PISDF_PARAM_H*/
