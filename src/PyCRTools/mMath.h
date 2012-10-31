/**************************************************************************
 *  Header file of Math module for CR Pipeline Python bindings.           *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef CR_PIPELINE_MATH_H
#define CR_PIPELINE_MATH_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

#include <boost/math/special_functions/fpclassify.hpp>

// ========================================================================
//  Type definitions
// ========================================================================

#ifdef PYCRTOOLS_WITH_NUMPY
typedef boost::python::numeric::array ndarray;
#endif /* PYCRTOOLS_WITH_NUMPY */

// ========================================================================
//  Definitions
// ========================================================================

enum hWEIGHTS {WEIGHTS_FLAT,WEIGHTS_LINEAR,WEIGHTS_GAUSSIAN};


//========================================================================
//                           Math Functions
//========================================================================

/*
#define Between(x,lower,upper) ((x > lower) && (x < upper))
#define BetweenOrEqual(x,lower,upper) ((x >= lower) && (x <= upper))
#define Outside(x,lower,upper) ((x < lower) || (x > upper))
#define OutsideOrEqual(x,lower,upper) ((x <= lower) || (x >= upper))
*/

template <class T, class S>
  inline bool Between(T x, S lower, S upper) {return ((hfcast<S>(x) > lower) && (hfcast<S>(x) < upper));}
template <class T,class S>
  inline bool BetweenOrEqual(T x, S lower, S upper) {return ((hfcast<S>(x) >= lower) && (hfcast<S>(x) <= upper));}
template <class T,class S>
  inline bool Outside(T x, S lower, S upper) {return ((hfcast<S>(x) < lower) || (hfcast<S>(x) > upper));}
template <class T,class S>
  inline bool OutsideOrEqual(T x, S lower, S upper) {return ((hfcast<S>(x) <= lower) || (hfcast<S>(x) >= upper));}

extern vector<HNumber> hWeights(const HInteger wlen, const hWEIGHTS wtype);

extern HNumber hPhase(const HNumber frequency, const HNumber time);

extern HComplex hAmplitudePhaseToComplex(const HNumber amplitude, const HNumber phase);
extern HComplex hPhaseToComplex(const HNumber phase);
extern HNumber funcGaussian (const HNumber x, const HNumber sigma, const HNumber mu);


// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mMath.def.h"




#endif /* CR_PIPELINE_MATH_H */
