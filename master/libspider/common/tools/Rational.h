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
/*
 * Rational.h
 *
 *  Created on: Apr 10, 2014
 *      Author: jheulot
 */

#ifndef RATIONAL_H_
#define RATIONAL_H_

#include <stdexcept>

class Rational {
private:
    long long nominator;
    long long denominator;

    inline void reduce() {
        long gcd = compute_gcd(this->nominator, this->denominator);
        nominator /= gcd;
        denominator /= gcd;
        if (denominator < 0) {
            nominator = -nominator;
            denominator = -denominator;
        }
    }

public:
    inline Rational() {
        nominator = 0;
        denominator = 1;
    }

    inline Rational(long i) {
        nominator = i;
        denominator = 1;
    }

    inline Rational(long nom, long den) {
        nominator = nom;
        denominator = den;
    }

    static inline long compute_gcd(long a, long b) {
        long t;
        while (b != 0) {
            t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    static inline long compute_lcm(long a, long b) {
        return labs(a * b) / compute_gcd(a, b);
    }

    inline Rational operator+(const Rational b) const {
        Rational res;
        long lcm = compute_lcm(this->denominator, b.denominator);
        res.denominator = lcm;
        res.nominator = lcm * this->nominator / this->denominator
                        + lcm * b.nominator / b.denominator;
        res.reduce();
        return res;
    }

    inline Rational operator-(const Rational b) const {
        Rational res;
        long lcm = compute_lcm(this->denominator, b.denominator);
        res.denominator = lcm;
        res.nominator = this->nominator * lcm / this->denominator
                        - b.nominator * lcm / b.denominator;
        res.reduce();
        return res;
    }

    inline Rational operator*(const Rational b) const {
        Rational res;
        res.nominator = this->nominator * b.nominator;
        res.denominator = this->denominator * b.denominator;
        res.reduce();
        return res;
    }

    inline Rational operator/(const Rational b) const {
        Rational res;
        res.nominator = this->nominator * b.denominator;
        res.denominator = this->denominator * b.nominator;
        res.reduce();
        return res;
    }

    inline bool operator==(const Rational b) const {
        return nominator == b.nominator && denominator == b.denominator;
    }

    inline bool operator!=(const Rational b) const {
        return nominator != b.nominator || denominator != b.denominator;
    }

    inline bool operator>(const Rational b) const {
        long lcm = compute_lcm(denominator, b.denominator);
        return nominator * (denominator / lcm) > b.nominator * (b.denominator / lcm);
    }

    inline bool operator<(const Rational b) const {
        long lcm = compute_lcm(denominator, b.denominator);
        return nominator * (denominator / lcm) < b.nominator * (b.denominator / lcm);
    }

    inline bool operator>=(const Rational b) const {
        long lcm = compute_lcm(denominator, b.denominator);
        return nominator * (denominator / lcm) >= b.nominator * (b.denominator / lcm);
    }

    inline bool operator<=(const Rational b) const {
        long lcm = compute_lcm(denominator, b.denominator);
        return nominator * (denominator / lcm) <= b.nominator * (b.denominator / lcm);
    }

    inline Rational getAbs() const {
        Rational res;
        res.nominator = labs(nominator);
        res.denominator = labs(denominator);
        return res;
    }

    inline long toInt() const {
        if (denominator == 1)
            return nominator;
        throw std::runtime_error("Error conv Rational to int without denominator = 1\n");
    }

    inline long getDenominator() const {
        return denominator;
    }

    inline long getNominator() const {
        return nominator;
    }
};

#endif /* RATIONAL_H_ */
