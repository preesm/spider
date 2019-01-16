/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFParam.h>
#include <parser/Expression.h>
#include "PiSDFParam.h"


/** Static Var def */
static std::int32_t globalID = 0;

PiSDFParam::PiSDFParam(
        const char *name,
        const char *expr,
        PiSDFParamType type,
        PiSDFGraph *graph,
        std::int32_t localID) {
    name_ = name ? std::string(name) : std::string();
    expressionString_ = expr ? std::string(expr) : std::string();
    globalID_ = globalID++;
    localID_ = localID;
    graph_ = graph;
    type_ = type;
    inheritedParam_ = nullptr;
    setter_ = nullptr;
    value_ = -1;
}


PiSDFParam::PiSDFParam(
        const char *name,
        const char *expr,
        PiSDFParamType type,
        PiSDFGraph *graph,
        std::int32_t localID,
        std::initializer_list<PiSDFParam *> dependencies) : dependencies_(dependencies) {
    name_ = name ? std::string(name) : std::string();
    expressionString_ = expr ? std::string(expr) : std::string();
    globalID_ = globalID++;
    localID_ = localID;
    graph_ = graph;
    type_ = type;
    setter_ = nullptr;
    value_ = -1;
//    using ParamSymbolTable = exprtk::symbol_table<double>;
//    using ParamExpression = exprtk::expression<double>;
//    using ParamParser = exprtk::parser<double>;
//
//    ParamSymbolTable symbolTable;
//    ParamExpression expression;
//    ParamParser parser;
//
//    // 0. Build the symbol table
//    for (auto p : dependencies_) {
//        symbolTable.add_variable(p->getName(), p->getValue());
//    }
//    symbolTable.add_constants();
//
//    // 1. Register the symbols
//    expression.register_symbol_table(symbolTable);
//
//    // 2. Compile the expression
//    parser.compile(expressionString_, expression);
    expr_ = CREATE(PISDF_STACK, Expression)(expressionString_.c_str(), dependencies_.data(), dependencies.size());
    // If parameter is static, it can be resolved now
    if (type == PISDF_PARAM_STATIC) {
        // 3. Resolve expression
        value_ = expr_->evaluate();
        // 4. Clear dependencies, we won't need them anymore
        dependencies_.clear();
        // 5. Delete the expression, we won't need it anymore
        expr_->~Expression();
        StackMonitor::free(PISDF_STACK, expr_);
        expr_ = nullptr;
    }
}


Param PiSDFParam::evaluateExpression() {
    if (expr_) {
        return expr_->evaluate();
    }
    return value_;
}

PiSDFParam::~PiSDFParam() {
    if (expr_) {
        expr_->~Expression();
        StackMonitor::free(PISDF_STACK, expr_);
    }
    dependencies_.clear();
}
