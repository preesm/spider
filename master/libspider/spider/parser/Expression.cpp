#include <cmath>

#include <cmath>

/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
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
#include <cmath>
#include <graphTransfo/GraphTransfo.h>
#include <parser/Expression.h>
#include "Expression.h"


#define MAX_NVAR_ELEMENTS 100
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 100
#define EXPR_LEN_MAX 1000

static int precedence[7] = {
        2, //OP_ADD
        2, //OP_SUB
        3, //OP_MUL
        3, //OP_DIV
        4, //OP_POW
        4, //OP_FLOOR
        4  //OP_CEIL
};

static const char *operatorSign[5] = {
        "+", //OP_ADD
        "-", //OP_SUB
        "*", //OP_MUL
        "/", //OP_DIV,
        "^" //OP_POW
};

Expression::Expression(
        const char *expr,
        const PiSDFParam *const *params, int nParam) {
    stringExpr_ = std::string(expr);
    isStatic_ = true;

    nElt_ = evaluateNTokens(expr);
    stack_ = CREATE_MUL(PISDF_STACK, nElt_, Token);

    Token *output = stack_;
    Token stack[MAX_NVAR_ELEMENTS];
    int ixOutput = 0;
    int ixStack = 0;

    const char *ptr = expr;
    Token t;

    while (getNextToken(&t, &ptr, params, nParam)) {
        switch (t.type) {
            case VALUE:
            case PARAMETER:
                output[ixOutput++] = t;
                if (t.param && t.param->getType() == PISDF_PARAM_DYNAMIC) {
                    isStatic_ = false;
                }
                break;
            case OPERATOR:
                while (ixStack > 0
                       && stack[ixStack - 1].type == OPERATOR
                       && precedence[t.opType] <= precedence[stack[ixStack - 1].opType]) {
                    ixStack--;
                    output[ixOutput++] = stack[ixStack];
                }
                stack[ixStack++] = t;
                break;
            case LEFT_PAR:
                stack[ixStack++] = t;
                break;
            case RIGHT_PAR:
                while (ixStack > 0 && stack[ixStack - 1].type != LEFT_PAR) {
                    ixStack--;
                    output[ixOutput++] = stack[ixStack];
                }
                if (ixStack == 0) {
                    throwSpiderException("Missing left parenthesis.");
                }
                ixStack--;
                break;
            default:
                throwSpiderException("Error while parsing expression.");
        }
    }

    while (ixStack > 0) {
        ixStack--;
        output[ixOutput++] = stack[ixStack];
    }
    nElt_ = ixOutput;
    if (isStatic_) {
        firstEval_ = true;
        value_ = evaluate();
        firstEval_ = false;
    }
}

Expression::~Expression() {
    StackMonitor::free(PISDF_STACK, stack_);
}

Param Expression::evaluate(const PiSDFParam *const *paramList, transfoJob *job, bool *ok) const {
    float stack[MAX_NVAR_ELEMENTS];
    float *stackPtr = stack;
    const Token *inputPtr = stack_;

    while (stack_ + nElt_ > inputPtr) {
        switch (inputPtr->type) {
            case OPERATOR:
                switch (inputPtr->opType) {
                    case ADD:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) += *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case SUB:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) -= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case MUL:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) *= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case DIV:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) /= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case POW:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) = static_cast<float>(pow((double) *(stackPtr - 1), *stackPtr));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case FLOOR:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::floor(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case CEIL:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::ceil(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                }
                break;
            case VALUE:
                *stackPtr = inputPtr->value;
                stackPtr++;
                break;
            case PARAMETER:
                *stackPtr = job->paramValues[paramList[inputPtr->paramIx]->getLocalID()];
                stackPtr++;
                break;
            default:
                throwSpiderException("Parenthesis in evaluated var.");
        }
        inputPtr++;
    }
    if (ok) *ok = true;
    return (Param) stack[0];
}

Param Expression::evaluate() const {
    if (isStatic_ && !firstEval_) {
        return value_;
    }
    float stack[MAX_NVAR_ELEMENTS];
    float *stackPtr = stack;
    const Token *inputPtr = stack_;

    while (stack_ + nElt_ > inputPtr) {
        switch (inputPtr->type) {
            case OPERATOR:
                switch (inputPtr->opType) {
                    case ADD:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) += *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case SUB:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) -= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case MUL:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) *= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case DIV:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) /= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case POW:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) = static_cast<float>(pow((double) *(stackPtr - 1), *stackPtr));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case FLOOR:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::floor(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case CEIL:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::ceil(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                }
                break;
            case VALUE:
                *stackPtr = inputPtr->value;
                stackPtr++;
                break;
            case PARAMETER:
                *stackPtr = inputPtr->param->getValue();
                if (*stackPtr == -1)
                    return -1; // Not resolved TODO handle better not resolved dependent params
                stackPtr++;
                break;
            default:
                throwSpiderException("Parenthesis in evaluated var.");
        }
        inputPtr++;
    }
//    if (isStatic_) {
//        value_ = static_cast<Param>(stack[0]);
//    }
//    fprintf(stderr, "INFO: value %lu\n", static_cast<Param>(stack[0]));
    return static_cast<Param>(stack[0]);
}

Param Expression::evaluate(const Param *vertexParamValues, int nParam) const {
    float stack[MAX_NVAR_ELEMENTS];
    float *stackPtr = stack;
    const Token *inputPtr = stack_;

    while (stack_ + nElt_ > inputPtr) {
        switch (inputPtr->type) {
            case OPERATOR:
                switch (inputPtr->opType) {
                    case ADD:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) += *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case SUB:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) -= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case MUL:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) *= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case DIV:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) /= *stackPtr;
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case POW:
                        if (stackPtr - stack >= 2) {
                            stackPtr--;
                            *(stackPtr - 1) = static_cast<float>(pow((double) *(stackPtr - 1), *stackPtr));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case FLOOR:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::floor(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                    case CEIL:
                        if (stackPtr - stack >= 1) {
                            *(stackPtr - 1) = std::ceil(*(stackPtr - 1));
                        } else {
                            throwSpiderException("Invalid operator in expression.");
                        }
                        break;
                }
                break;
            case VALUE:
                *stackPtr = inputPtr->value;
                stackPtr++;
                break;
            case PARAMETER:
                if (inputPtr->paramIx < 0 || inputPtr->paramIx >= nParam) {
                    throwSpiderException("Invalid parameter id.");
                }
                *stackPtr = vertexParamValues[inputPtr->paramIx];
                if (*stackPtr == -1)
                    return -1; // Not resolved TODO handle better not resolved dependent params
                stackPtr++;
                break;
            default:
                throwSpiderException("Parenthesis in evaluated var.");
        }
        inputPtr++;
    }
    return static_cast<Param>(stack[0]);
}

void Expression::toString(
        const PiSDFParam *const *params, int nParam,
        char *out, size_t outSizeMax) {
    static char outputStack[REVERSE_POLISH_STACK_MAX_ELEMENTS + 1][EXPR_LEN_MAX];
    int outputStackSize = 0;
//	Token* varStackPtr = stack_;

    //While there are input tokens left
    int i;
    for (i = 0; i < nElt_; i++) {
        switch (stack_[i].type) {
            case OPERATOR:
                // pop out 2
                snprintf(out, outSizeMax, "( %s %s %s )",
                         outputStack[outputStackSize - 1],
                         operatorSign[stack_[i].opType],
                         outputStack[outputStackSize - 2]);
                strncpy(outputStack[outputStackSize - 2], out, EXPR_LEN_MAX);
                outputStackSize--;
                break;
            case VALUE:
                // Push value to output stack
                snprintf(outputStack[outputStackSize++], EXPR_LEN_MAX, "%d", stack_[i].value);
                break;
            case PARAMETER:
                if (stack_[i].paramIx < 0 || stack_[i].paramIx >= nParam) {
                    throwSpiderException("Invalid parameter id.");
                }
                // Push parameter name to output stack
                snprintf(outputStack[outputStackSize++], EXPR_LEN_MAX, "%s", params[stack_[i].paramIx]->getName());
                break;
            default:
                throwSpiderException("Parenthesis in evaluated var.");
        }
    }
    strncpy(out, outputStack[0], outSizeMax);
}

int Expression::evaluateNTokens(const char *expr) {
    const char **ptr = &expr;
    int i = 0;
    while (getNextToken(nullptr, ptr, nullptr, 0)) {
        i++;
    }
    return i;
}

bool Expression::getNextToken(
        Token *token,
        const char **ptr,
        const PiSDFParam *const *params, int nParam) {
    // skip over whitespaces
    while (**ptr == ' ' || **ptr == '\t')
        (*ptr)++;


    // check for end of expression
    if (**ptr == '\0')
        return false;

    // check for minus
    if (**ptr == '-') {
        if (token != nullptr) {
            token->type = OPERATOR;
            token->opType = SUB;
        }
        (*ptr)++;
        return true;
    }

    // check for plus
    if (**ptr == '+') {
        if (token != nullptr) {
            token->type = OPERATOR;
            token->opType = ADD;
        }
        (*ptr)++;
        return true;
    }

    // check for mult
    if (**ptr == '*') {
        if (token != nullptr) {
            token->type = OPERATOR;
            token->opType = MUL;
        }
        (*ptr)++;
        return true;
    }

    // check for power
    if (**ptr == '^') {
        if (token != nullptr) {
            token->type = OPERATOR;
            token->opType = POW;
        }
        (*ptr)++;
        return true;
    }

    // check for div
    if (**ptr == '/') {
        if (token != nullptr) {
            token->type = OPERATOR;
            token->opType = DIV;
        }
        (*ptr)++;
        return true;
    }

    // check for parentheses
    if (**ptr == '(') {
        if (token != nullptr) {
            token->type = LEFT_PAR;
        }
        (*ptr)++;
        return true;
    }
    if (**ptr == ')') {
        if (token != nullptr) {
            token->type = RIGHT_PAR;
        }
        (*ptr)++;
        return true;
    }

    // check for a value
    if (isdigit(**ptr)) {
        int value = 0;
        while (isdigit(**ptr)) {
            value *= 10;
            value += **ptr - '0';
            (*ptr)++;
        }
        if (token != nullptr) {
            token->type = VALUE;
            token->value = value;
        }
        return true;
    }

    // check for param
    if (isalnum(**ptr) || (**ptr == '_')) {
        const char *name = *ptr;
        size_t nb = 0;
        while (isalnum(**ptr) || (**ptr == '_')) {
            nb++;
            (*ptr)++;
        }

        if (token != nullptr) {
            // Check if it is an OPERATOR
            const char *operatorStrings[2] = {"floor", "ceil"};
            OpType operatorTypes[2] = {FLOOR, CEIL};
            for (int i = 0; i < 2; ++i) {
                if (nb == strlen(operatorStrings[i]) &&
                    strncasecmp(operatorStrings[i], name, nb) == 0) {
                    token->opType = operatorTypes[i];
                    token->type = OPERATOR;
                    return true;
                }
            }
            // Check if it is a PARAMETER
            for (int i = 0; i < nParam; i++) {
                token->type = PARAMETER;
                if (nb == strlen(params[i]->getName())
                    && strncmp(params[i]->getName(), name, nb) == 0) {
                    token->paramIx = i/*params[i]->getLocalID()*/;
                    token->param = (PiSDFParam *) params[i];
                    return true;
                }
            }
            throwSpiderException("Failed to parse expression: %s.", name);
        } else {
            return true;
        }
    }
    throwSpiderException("Failed parsing..");
}



