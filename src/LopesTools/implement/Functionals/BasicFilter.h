\/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (<bahren@astron.nl>)                                     *
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

/* $Id: BasicFilter.h,v 1.8 2006/04/14 11:11:57 bahren Exp $*/

#ifndef _BASICFILTER_H_
#define _BASICFILTER_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

#include <casa/namespace.h>

namespace lopestools {

/*!
  \class BasicFilter

  \ingroup Functionals

  \brief Basic characteristics and methods for filters

  \author Lars B&auml;hren

  \date 2005/11/16

  \test tBasicFilter.cc

  <h3>Prerequisite</h3>

  --/--

  <h3>Synopsis</h3>

  A simple and basic framework for filters; this class handles storage and
  application of the filter weights. The computation of the weights themselves
  then is implemented in the various derived classes (e.g. in HanningFilter).

  <h3>Example(s)</h3>

  <ol>
    <li>In the simplest case we construct a filter of length \f$ N \f$ with all weights 
    set to unity, i.e. no filtering at all:
    \verbatim
    # include <lopes/Functionals/BasicFilter.h>
    
    unsigned int blocksize (1024);
    
    BasicFilter<Float> filter_float (blocksize);
    BasicFilter<Double> filter_double (blocksize);
    BasicFilter<Complex> filter_complex (blocksize);
    \endverbatim

    <li>The Filter can be constructed as well by directly providing the filter
    weights during construction:
    \verbatim
    # include <lopes/Functionals/BasicFilter.h>
    
    unsigned int blocksize (1024);
    Vector<Float> weights (blocksize,0.5);
    
    BasicFilter<Float> filter (blocksize,weights);
    \endverbatim
  </ol>
*/
template <class T> class BasicFilter {
  
  // Number of elements in a data vector passed to the filter
  unsigned int blocksize_p;
  // Filter weights by which the incoming data are multiplied
  Vector<T> weights_p;
  
 public:
  
  // --- Construction ----------------------------------------------------------

  /*!
    \brief Default constructor

    Contains a single filter weight of value 1.
  */
  BasicFilter ();

  /*!
    \brief Constructor
    
    \param blocksize -- Number of elements in a data vector passed to the filter
  */
  BasicFilter (const unsigned int& blocksize);
  
  /*!
    \brief Constructor
    
    \param weights -- The filter weights.
  */
  BasicFilter (const Vector<T>& weights);
  
  /*!
    \brief Copy constructor
    
    \param other -- Another BasicFilter.
  */
  BasicFilter (const BasicFilter<T>& other);
  
  // --- Destruction -----------------------------------------------------------
  
  /*!
    \brief Destructor
  */
  ~BasicFilter ();

  // --- Operators -------------------------------------------------------------

  /*!
    \brief Copy operator
    
    \param other -- Another BasicFilter.
   */
  BasicFilter<T> &operator= (BasicFilter<T> const &other); 
  
  // --- Parameters ------------------------------------------------------------
  
  /*!
    \brief Get the number of elements in a vector of data.
    
    \param blocksize -- The number of elements in a vector of data.
  */
  unsigned int blocksize () {
    return blocksize_p;
  }

  /*!
    \brief Get the weights which are applied at filtering
    
    \return weights -- The filter weights.
  */
  Vector<T> weights () {
    return weights_p;
  }
  
  /*!
    \brief Set the weights which are applied at filtering
    
    \param weights -- The filter weights.
  */
  void setWeights (const Vector<T>& weights);

  /*!
    \brief Apply the filter of a block of data

    \param data -- Input data to the filter

    \retval data -- The data after application of the filter.
   */
  void filter (Vector<T>& data);

  /*!
    \brief Apply the filter of a block of data

    \param data -- Input data to the filter

    \retval filteredData -- The data after application of the filter.
   */
  void filter (Vector<T> filteredData,
	       const Vector<T>& data);

 private:
  
  /*!
    \brief Unconditional copying
  */
  void copy (BasicFilter<T> const& other);
  
  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);
  
};

}

#endif /* _BASICFILTER_H_ */
