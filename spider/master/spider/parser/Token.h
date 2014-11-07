/*
 * Token.h
 *
 *  Created on: Nov 7, 2014
 *      Author: jheulot
 */

#ifndef PARSER_TOKEN_H
#define PARSER_TOKEN_H

namespace Parser{

class Token {
public:
	typedef enum{
		OPERATOR,
		VALUE,
		PARAMETER,
		LEFT_PAR,
		RIGHT_PAR
	} Type;

	typedef enum{
		ADD,
		SUB,
		MUL,
		DIV
	} OpType;

	Token();
	virtual ~Token();

	inline Type getType() const;
	inline OpType getOpType() const;
	inline int getValue() const;
	inline int getParamIx() const;

	inline void setType(Type type);
	inline void setOpType(OpType opType);
	inline void setValue(int value);
	inline void setParamIx(int paramIx);

private:

	Type type_;

	OpType opType_;
	int value_;
	int paramIx_;

};

inline Token::Type 	Token::getType() const{
	return type_;
}
inline Token::OpType Token::getOpType() const{
	return opType_;
}
inline int 			Token::getValue() const{
	return value_;
}
inline int 			Token::getParamIx() const{
	return paramIx_;
}

inline void Token::setType(Token::Type type){
	type_ = type;
}
inline void Token::setOpType(Token::OpType opType){
	opType_ = opType;
}
inline void Token::setValue(int value){
	value_ = value;
}
inline void Token::setParamIx(int paramIx){
	paramIx_ = paramIx;
}

}

#endif/*PARSER_TOKEN_H*/
