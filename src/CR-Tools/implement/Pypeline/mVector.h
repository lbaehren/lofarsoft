/**************************************************************************
 *  Header file of Vector module for CR Pipeline Python bindings          *
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

#ifndef CR_PIPELINE_VECTOR_H
#define CR_PIPELINE_VECTOR_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

using namespace std;

// ========================================================================
//
//  Generic templates for wrapper functionality
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  namespace Vector { // Namespace Vector -- begin


    template <class Iter, class IterIn>
      void hCopy(const Iter vecout, const Iter vecout_end,
                 const IterIn vecin, const IterIn vecin_end)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter itout(vecout);
      IterIn itin(vecin);

      // Sanity check - but how should this error happen in a Python code ...?
      if ((vecout > vecout_end) || (vecin > vecin_end)) {
        throw PyCR::ValueError("Negative size of vector.");
        return;
      }

      /* None of this is needed! Since it is an intended feature that the two vectors need not be of the same length!

      HInteger lenIn = std::distance(vecin,vecin_end);Vec
      HInteger lenOut = std::distance(vecout,vecout_end);


      //Why do you call this an error? This is a feature ...!
      if (lenIn > lenOut) {
        cout << "Warning: Input vector is smaller than output vector: looping over input vector." << endl;
	//throw PyCR::ValueError("Input vector is larger than output vector.");
        //return;
      }
      if (lenIn < lenOut) {
        cout << "Warning: Input vector is smaller than output vector: looping over input vector." << endl;
      }
      */

      // Copying of data
      while (itout != vecout_end) {
        *itout=hfcast<T>(*itin);
        ++itin;
        ++itout;
        if (itin==vecin_end)
          itin=vecin;
      };
    }

    template <class Iter>
      void hRedistribute(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger offset, HInteger stride)
      {
	// Variables
	Iter it(vec+offset);
	Iter itin(invec);
	// Sanity check
	if (stride < 0) {
	  throw PyCR::ValueError("Negative value for stride parameter is not allowed");
	}
	// Redistribute
	while ((itin != invec_end) && (it < vec_end)) {
	  *it = *itin;
	  ++itin;
	  it += stride;
	};
      }

    template <class Iter>
      void hTranspose(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger ncolumns)
      {
	// Variables
	Iter it(invec),itend(it+ncolumns);
	HInteger nrows=(invec_end-invec)/ncolumns;
	HInteger i(0);
	while (itend<=invec_end) {
	  PyCR::Vector::hRedistribute(vec,vec_end,it,itend,i,nrows);
	  ++i; it=itend; itend+=ncolumns;
	};
      }

    template <class Iter>
      void hTranspose(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger nrows, HInteger ncolumns)
      {
	// Variables
	HInteger size=(invec_end-invec)/(ncolumns*nrows),block=ncolumns*size;
	Iter start(invec);
	Iter iti(start),itiend(iti+size);
	Iter ito(vec),itoend(ito+size);
	while (itoend<=vec_end) {
	  hCopy(ito,itoend,iti,itiend);
	  ito=itoend; itoend+=size;
	  iti+=block; if (iti >= invec_end) {start+=size; iti=start;};
	  itiend=iti+size;
	};
      }

    template <class Iter, class Iterin, class IterI>
      void hCopy(const Iter vecout, const Iter vecout_end,
                 const Iterin vecin, const Iterin vecin_end,
                 const IterI index, const IterI index_end,
                 const HInteger number_of_elements)
    {
      // Declaration of variables
      typedef IterValueType T;
      Iter itout(vecout);
      IterI itidx(index);
      Iterin itin;
      HInteger count(number_of_elements);

      // Sanity check
      if ((index >= index_end) || (count==0))
        return;
      if (vecin >= vecin_end)
        return;
      if ((count<0) || (count > (vecout_end-vecout)))
        count=(vecout_end-vecout);

      // Copying of data
      while ((itout != vecout_end) && (count > 0)) {
        itin = vecin + *itidx;
        if ((itin >= vecin) && (itin < vecin_end))
          *itout=hfcast<T>(*itin);
        --count;
        ++itidx;
        ++itout;
        if (itidx==index_end)
          itidx=index;
      };
    }
  } // Namespace Vector -- end
} // Namespace PyCR -- end

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mVector.def.h"

#endif /* CR_PIPELINE_VECTOR_H */
