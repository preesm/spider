/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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

class PiSDFParam : public SetElement {
public:
    /** Constructor */
    PiSDFParam(
            const char *name,
            const char *expr,
            PiSDFParamType type,
            PiSDFGraph *graph,
            std::int32_t localID);

    PiSDFParam(
            const char *name,
            const char *expr,
            PiSDFParamType type,
            PiSDFGraph *graph,
            std::int32_t localID,
            std::initializer_list<PiSDFParam *> dependencies);

    ~PiSDFParam() override;

    /** Getters */
    inline int getGlobalID() const;

    inline int getLocalID() const;

    inline const char *getName() const;

    inline PiSDFParamType getType() const;

    inline Param getStaticValue() const;

    inline int getParentId() const;

    inline Expression *getExpression();

    inline Param getValue();

//    inline Param getValue() const;

    /** Setters */
    inline void setValue(Param value);

    inline void resetEvaluation();

    inline void setParentId(int parentId);

    inline void setHeritedParemeter(PiSDFParam *heritedParameter);

    inline void setSetter(PiSDFVertex *setter, int portIx);

    inline void setExpression(Expression *expr);

private:
    /**
     * @brief Global ID of the parameter within the application.
     */
    std::int32_t globalID_;

    /**
     * @brief Local ID of the parameter within its containing graph.
     */
    std::int32_t localID_;

    /**
     * @brief Name of the parameter within its containing graph.
     */
    std::string name_;

    /**
     * @brief Literal expression of the parameter.
     */
    std::string expressionString_;

    /**
     * @brief Real value of the parameter;
     */
    Param value_;

    /**
     * @brief Containing graph of the parameter.
     */
    PiSDFGraph *graph_;

    /**
     * @brief Parameter Type (STATIC, DYNAMIC, HERITED).
     */
    PiSDFParamType type_;

    /**
    * @brief Pointer to original parameter if parameter if of type HERITED.
    */
    PiSDFParam *heritedParam_;

    /**
     * @brief Vertex setting the parameter's value if it is of type DYNAMIC.
     */
    PiSDFVertex *setter_;

    /**
     * @brief Vector of parameter dependencies (in the case of a dynamic dependent parameter)
     */
    std::vector<PiSDFParam *> dependencies_;

    /**
     * @brief Expression of the parameter (needed for dynamic dependent parameter)
     */
    Expression *expr_;

    Param evaluateExpression();

    /**
     * @brief Flag indicating that the expression has already been evaluated.
     */
    bool isEvaluated;



    int parentId_;
    int portIx_;
};

/** Inline Fcts */
/** Getters */
inline int PiSDFParam::getGlobalID() const {
    return globalID_;
}

inline int PiSDFParam::getLocalID() const {
    return localID_;
}

inline const char *PiSDFParam::getName() const {
    return name_.c_str();
}

inline PiSDFParamType PiSDFParam::getType() const {
    return type_;
}

inline Param PiSDFParam::getValue() {
    if (heritedParam_) {
        value_ = heritedParam_->getValue();
    } else if (!dependencies_.empty() && !isEvaluated) {
        isEvaluated = true;
        value_ = evaluateExpression();
    }
    return value_;
}


inline Param PiSDFParam::getStaticValue() const {
    if (type_ != PISDF_PARAM_STATIC) {
        throwSpiderException("Dynamic param [%s] used as Static param.", name_.c_str());
    }
    return value_;
}

inline int PiSDFParam::getParentId() const {
    if (type_ != PISDF_PARAM_HERITED) {
        throwSpiderException("Not Herited param [%s] used as Herited param.", name_.c_str());
    }
    return parentId_;
}

inline Expression *PiSDFParam::getExpression() {
    return expr_;
}

/** Setters */
inline void PiSDFParam::setValue(Param value) {
    value_ = value;
}

inline void PiSDFParam::resetEvaluation() {
    if (!dependencies_.empty()) {
        isEvaluated = false;
        value_ = -1;
    }
}

inline void PiSDFParam::setParentId(int parentId) {
    parentId_ = parentId;
}

inline void PiSDFParam::setSetter(PiSDFVertex *setter, int portIx) {
    if (!dependencies_.empty()) {
        throwSpiderException(
                "Dynamic parameter [%s] with configure actor can not have dependencies to other parameters.",
                name_.c_str());
    }
    setter_ = setter;
    portIx_ = portIx;
}

inline void PiSDFParam::setExpression(Expression *expr) {
    expr_ = expr;
}

inline void PiSDFParam::setHeritedParemeter(PiSDFParam *heritedParameter) {
    heritedParam_ = heritedParameter;
}


#endif/*PISDF_PARAM_H*/
