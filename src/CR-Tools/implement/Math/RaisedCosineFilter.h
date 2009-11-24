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

/* $Id$*/

#ifndef _RAISEDCOSINEFILTER_H_
#define _RAISEDCOSINEFILTER_H_

// AIPS++ header files
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

// LOPES-Tools header files
#include <Math/BasicFilter.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class RaisedCosineFilter
    
    \ingroup CR_Math
    
    \brief Brief description for class RaisedCosineFilter
    
    \author Lars B&auml;hren
    
    \date 2005/11/17
    
    \test tRaisedCosineFilter.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[CR] BasicFilter
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  template <class T> class RaisedCosineFilter : public BasicFilter<T> {

    //! Control parameter for the rising slope of the filter shape
    int deltaRise_p;
    //! Control parameter for the falling slope of the filter shape
    int deltaFall_p;
    //! Control parameter for the middle part of the filter shape
    T alpha_p;
    
  public:
    
    // --- Construction -----------------------------------------------------------
    
    //! Argumented constructor
    RaisedCosineFilter (unsigned int const &blocksize,
			int const &delta);
    //! Argumented constructor
    RaisedCosineFilter (unsigned int const &blocksize,
			int const &deltaRise,
			int const &deltaFall);
    //! Argumented constructor
    RaisedCosineFilter (unsigned int const &blocksize,
			T const &transitionFraction);
    //! Argumented constructor
    RaisedCosineFilter (unsigned int const &blocksize,
			T const &transitionRise,
			T const &transitionFall);
    
    // --- Destruction ------------------------------------------------------------
    
    ~RaisedCosineFilter ();
    
    // --- Methods ----------------------------------------------------------------
    
    vector<int> delta ();
    
    vector<T> transitionFraction ();
    
  private:
    
    //! Initialization of the internal filter parameters
    void init (const int& blocksize,
	       const int& deltaRise,
	       const int& deltaFall);
    //! Computation of the filter weights
    void setWeights ();
  
  };
  
}  // Namespace CR -- end

#endif /* _RAISEDCOSINEFILTER_H_ */
