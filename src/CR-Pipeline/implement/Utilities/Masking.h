/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: Masking.h,v 1.8 2007/03/19 17:40:33 bahren Exp $ */

#if !defined(MASKING_H)
#define MASKING_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/IPosition.h>

#include <casa/namespace.h>

namespace CR {

  /*!
    \ingroup Utilities
    
    \brief Collection of methods for masking of data arrays.
    
    \author Lars B&auml;hren
    
    \date 2005/02/03
    
    \test tMasking.cc
    
    
    <h3>Prerequisite</h3>
    
    <ul>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
    -- A templated N-D Array class with zero origin. 
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Vector.html">Vector</a>,
    <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Matrix.html">Matrix</a>,
    and <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Cube.html">Cube</a>
    are one, two, and three dimensional specializations of Array.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">Mathematical
    operations for Arrays</a>.
    
    <li>[AIPS++]
    <a href="http://aips2.nrao.edu/docs/casa/implement/Arrays/MaskedArray.html">MaskedArray</a>
    -- Class for masking an Array for operations on that Array
    </ul>    
  */
  
  /*!
    \brief Merge two masking arrays.
    
    \param array1 -- 
    \param array2 -- 
    \param logic  -- Boolean logic by which to combine the two input masks:
                     - AND
		     - NAND
		     - OR
		     - NOR
  */
  Array<Bool> mergeMasks (const Array<Bool>& array1,
			  const Array<Bool>& array2,
			  const String logic);
  
  /*!
    \brief Creat mask from subset of entries of another mask.
    
    Given two masking arrays, \f$ M_1 \equiv \{ m_1, m_2, ..., m_{N_1} \} \f$ and
    \f$ M_2 \equiv \{ m_1, m_2, ..., m_{N_2} \} \f$, where \f$ N_1 > N_2 \f$, we
    set the values of \f$ M_2 \f$ by aligning its elements to \f$ M_1 \f$ via a
    position vector \f$ \vec n \f$: \f$ M_2 (k) = M_1 (n_k) \f$
    
    \param outMask - Boolean array \f$ M_2 \f$, to be set up.
    \param inMask  - Boolean array \f$ M_1 \f$.
    \param inIndex - Position vector \f$ \vec n \f$, used to map the values from
    \f$ M_1 \f$ onto \f$ M_2 \f$.
  */
  void setSubmask (Vector<Bool>& outMask, 
		   Vector<Bool> const &inMask,
		   Vector<Int> const &inIndex);
  
  /*!
    \brief Create a mask for a range of values in a data vector
    
    \param range  - Value range/interval within which elements of the data
                    array are considered selected.
    \param values - Array of values.
    
    \return mask  - Boolean array with the mask
  */
  template <class T>
    Vector<Bool> maskFromRange (Vector<T> const &range,
				Vector<T> const &values);
  
  /*!
    \brief Get the number of valid (i.e. selected) elements in a mask.
    
    \param mask - Boolean masking array
    
    \return validElements - The number of valid elements in the selection mask.
  */
  uInt validElements (const Vector<Bool>& mask);
  
  /*!
    \brief Get the number of invalid (i.e. deselected) elements in a mask.
    
    \param mask - Boolean masking array
    
    \return invalidElements - The number of invalid elements in the selection
    mask.
  */
  uInt invalidElements (const Vector<Bool>& mask);
  
}

#endif
