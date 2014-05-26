/*
 * Rational.h
 *
 *  Created on: Apr 10, 2014
 *      Author: jheulot
 */

#ifndef RATIONAL_H_
#define RATIONAL_H_

#include <stdlib.h>

class Rational {
private:
	int nominator;
	int denominator;

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
		return abs(a*b)/compute_gcd(a,b);
	}

	inline void reduce(){
		int gcd = compute_gcd(this->nominator, this->denominator);
		nominator /= gcd;
		denominator /= gcd;
		if(denominator<0){
			nominator = -nominator;
			denominator = -denominator;
		}
	}
public:
	inline Rational(){
		nominator=0;
		denominator=1;
	}
	inline Rational(int i){
		nominator=i;
		denominator=1;
	}
	inline Rational(int nom, int den){
		nominator = nom;
		denominator = den;
	}

	inline Rational operator+(Rational b){
		Rational res;
		int lcm = compute_lcm(this->denominator, b.denominator);
		res.denominator = lcm;
		res.nominator = lcm*this->nominator/this->denominator
				+ lcm*b.nominator/b.denominator;
		res.reduce();
		return res;
	}

	inline Rational operator-(Rational b){
		Rational res;
		int gcd = compute_gcd(this->denominator, b.denominator);
		res.denominator = gcd;
		res.nominator = this->denominator/gcd*this->nominator
				- b.denominator/gcd*b.nominator;
		res.reduce();
		return res;
	}

	inline Rational operator*(Rational b){
		Rational res;
		res.nominator = this->nominator * b.nominator;
		res.denominator = this->denominator * b.denominator;
		res.reduce();
		return res;
	}

	inline Rational operator/(Rational b){
		Rational res;
		res.nominator = this->nominator * b.denominator;
		res.denominator = this->denominator * b.nominator;
		res.reduce();
		return res;
	}

	inline bool operator==(Rational b){
		return nominator==b.nominator && denominator==b.denominator;
	}

	inline bool operator!=(Rational b){
		return nominator!=b.nominator || denominator!=b.denominator;
	}

	inline bool operator>(Rational b){
		int lcm = compute_lcm(denominator,b.denominator);
		return nominator*(denominator/lcm) > b.nominator*(b.denominator/lcm);
	}

	inline bool operator<(Rational b){
		int lcm = compute_lcm(denominator,b.denominator);
		return nominator*(denominator/lcm) < b.nominator*(b.denominator/lcm);
	}

	inline bool operator>=(Rational b){
		int lcm = compute_lcm(denominator,b.denominator);
		return nominator*(denominator/lcm) >= b.nominator*(b.denominator/lcm);
	}

	inline bool operator<=(Rational b){
		int lcm = compute_lcm(denominator,b.denominator);
		return nominator*(denominator/lcm) <= b.nominator*(b.denominator/lcm);
	}

	inline Rational getAbs(){
		Rational res;
		res.nominator = abs(nominator);
		res.denominator = abs(denominator);
		return res;
	}

	inline int toInt(){
		if(denominator == 1)
			return nominator;
		printf("Error conv Rational to int without denominator = 1\n");
		abort();
		return 0;
	}

	inline int getDenominator() const {
		return denominator;
	}

	inline int getNominator() const {
		return nominator;
	}
};

#endif /* RATIONAL_H_ */
