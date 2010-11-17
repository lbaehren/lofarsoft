/**************************************************************************
 *  Header file of Math module for CR Pipeline Python bindings.           *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
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

using namespace std;


// ========================================================================
//  Definitions
// ========================================================================

enum hWEIGHTS {WEIGHTS_FLAT,WEIGHTS_LINEAR,WEIGHTS_GAUSSIAN};


//========================================================================
//                           Math Functions
//========================================================================


#define Between(x,lower,upper) ((x > lower) && (x < upper))
#define BetweenOrEqual(x,lower,upper) ((x >= lower) && (x <= upper))
#define Outside(x,lower,upper) ((x < lower) || (x > upper))
#define OutsideOrEqual(x,lower,upper) ((x <= lower) || (x >= upper))



extern vector<HNumber> hWeights(const HInteger wlen, const hWEIGHTS wtype);

extern HNumber hPhase(const HNumber frequency, const HNumber time);

extern HComplex hAmplitudePhaseToComplex(const HNumber amplitude, const HNumber phase);
extern HComplex hPhaseToComplex(const HNumber phase);
extern HNumber funcGaussian (const HNumber x, const HNumber sigma, const HNumber mu);


// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  namespace Math { // Namespace Math -- begin

    template <class T> inline T square(T val);
    template <class T> inline T logSave(T val);

    template <class T> inline T hMedian(std::vector<T>& vec);

    template <class Iter, class S>
      void hMul2(const Iter vec1,const Iter vec1_end, S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter it=vec1;

      // Vector operation
      while (it!=vec1_end) {
        *it = hfcast<T>(*it * val);
        ++it;
      };
    }

    template <class Iter, class S>
      void hAdd2(const Iter vec1,const Iter vec1_end, S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter it=vec1;

      // Vector operation
      while (it!=vec1_end) {
        *it = hfcast<T>(*it + val);
        ++it;
      };
    }

    template <class Iter, class S>
      void hDiv2(const Iter vec1,const Iter vec1_end, S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter it=vec1;

      // Sanity check
      if (abs(val) < A_LOW_NUMBER) {
        throw PyCR::ValueError("Division by zero");
      }

      // Vector operation
      while (it!=vec1_end) {
        *it = hfcast<T>(*it / val);
        ++it;
      };
    }

    template <class Iter, class S>
      void hSub2(const Iter vec1,const Iter vec1_end, S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter it=vec1;

      // Vector operation
      while (it!=vec1_end) {
        *it = hfcast<T>(*it - val);
        ++it;
      };
    }

    template <class Iter, class Iterin, class S>
      void hMul2(const Iter vec,const Iter vec_end, const Iterin vec1,const Iterin vec1_end,const S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iterin it1=vec1;
      Iter itout=vec;

      // Vector operation
      while (itout !=vec_end) {
        *itout = hfcast<T>(*it1 * val);
        ++it1; ++itout;
        if (it1==vec1_end) it1=vec1;
      };
    }

    template <class Iter, class Iterin, class S>
      void hAdd2(const Iter vec,const Iter vec_end, const Iterin vec1,const Iterin vec1_end,const S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iterin it1=vec1;
      Iter itout=vec;

      // Vector operation
      while (itout !=vec_end) {
        *itout = hfcast<T>(*it1 + val);
        ++it1; ++itout;
        if (it1==vec1_end) it1=vec1;
      };
    }

    template <class Iter, class Iterin, class S>
      void hDiv2(const Iter vec,const Iter vec_end, const Iterin vec1,const Iterin vec1_end,const S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iterin it1=vec1;
      Iter itout=vec;

      if (abs(val) < A_LOW_NUMBER) {
        throw PyCR::ValueError("Division by zero");
      }

      // Vector operation
      while (itout !=vec_end) {
        *itout = hfcast<T>(*it1 / val);
        ++it1; ++itout;
        if (it1==vec1_end) it1=vec1;
      };
    }

    template <class Iter, class Iterin, class S>
      void hSub2(const Iter vec,const Iter vec_end, const Iterin vec1,const Iterin vec1_end,const S val)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iterin it1=vec1;
      Iter itout=vec;

      // Vector operation
      while (itout !=vec_end) {
        *itout = hfcast<T>(*it1 - val);
        ++it1; ++itout;
        if (it1==vec1_end) it1=vec1;
      };
    }


  } // Namespace Math -- end

} // Namespace PyCR -- end



// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mMath.def.h"




#endif /* CR_PIPELINE_MATH_H */
