/*-------------------------------------------------------------------------*
| $Id::                                                                 $ |
*-------------------------------------------------------------------------*
***************************************************************************
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

#ifndef _BASICFILTER_H_
#define _BASICFILTER_H_

#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class BasicFilter
    
    \ingroup CR_Math
    
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
      <li>In the simplest case we construct a filter of length \f$ N \f$ with all
      weights set to unity, i.e. no filtering at all:
      \code
      # include <Math/BasicFilter.h>
      
      unsigned int blocksize (1024);
      
      BasicFilter<Float> filter_float (blocksize);
      BasicFilter<Double> filter_double (blocksize);
      BasicFilter<Complex> filter_complex (blocksize);
      \endcode
      
      <li>The Filter can be constructed as well by directly providing the filter
      weights during construction:
      \code
      # include <Math/BasicFilter.h>
      
      unsigned int blocksize (1024);
      Vector<Float> weights (blocksize,0.5);
      
      BasicFilter<Float> filter (blocksize,weights);
      \endcode
    </ol>
  */
  
  template <class T> class BasicFilter {
    
  protected:
    
    //! Number of elements in a data vector passed to the filter
    unsigned int blocksize_p;
    //! Filter weights by which the incoming data are multiplied
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
    
    //! Destructor
    ~BasicFilter () {
      destroy();
    }
    
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
    inline Vector<T> weights () {
      return weights_p;
    }
    
    /*!
      \brief Set the weights which are applied at filtering
      
      \param weights -- The filter weights.
    */
    inline void setWeights (const Vector<T>& weights) {
      blocksize_p = weights.nelements();
      //
      weights_p.resize(blocksize_p);
      weights_p = weights;
    }
    
    /*!
      \brief Apply the filter of a block of data
      
      \param data -- Input data to the filter
      
      \retval data -- The data after application of the filter.
      \return status -- Returns <tt>True</tt> if the operation was successful;
              returns <tt>False</tt> in case the shape of the provided data 
	      vector does not match the shape of the array holding the weighting
	      factors.
    */
    inline bool filter (Vector<T>& data)
    {
      if (blocksize_p == data.nelements()) {
	data *= weights_p;
	return true;
      } else {
	std::cerr << "[BasicFilter::filter] Mismatching array shapes!"
		  << std::endl;
	return false;
      }
    }
    
    /*!
      \brief Apply the filter of a block of data
      
      \param data -- Input data to the filter
      
      \retval filteredData -- The data after application of the filter.
    */
    inline bool filter (Vector<T> filteredData,
			const Vector<T>& data)
    {
      if (blocksize_p == data.nelements()) {
	filteredData.resize (data.shape());
	filteredData = data;
	filter (filteredData);
	return true;
      } else {
	std::cerr << "[BasicFilter::filter] Mismatching array shapes!"
		  << std::endl;
	return false;
      }
    }
    
  private:
    
    //! Unconditional copying
    void copy (BasicFilter<T> const& other);
    //! Unconditional deletion 
    void destroy(void);
    
  };
  
}  // Namespace CR -- end

#endif /* _BASICFILTER_H_ */
