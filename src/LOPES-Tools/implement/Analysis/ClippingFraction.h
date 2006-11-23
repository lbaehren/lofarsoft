/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron,nl)                                       *
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

/*!
  \brief Flagger testing for clipping of signals

  \ingroup Analysis
  
  \author Lars B&auml;hren

  \date 2005/06/16

  \test tClippingFraction.cc

  \todo Try creating a templated version of this class, since variables
  typically might be either <i>float</i> or <i>double</i>.

*/

#ifndef CLIPPINGFRACTION_H
#define CLIPPINGFRACTION_H

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

using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  template <class T> class ClippingFraction {
    
    //! Lower clipping threshold
    T clipLow_p;
    
    //! Upper clipping threshold
    T clipHigh_p;
    
    //! Threshold for the allowed fraction of samples in a data block to be clipped
    T clipFraction_p;
    
  public:
    
    // --- Construction ------------------------------------------------
    
    /*!
      \brief Default constructor
      
      Invoked without any arguments, the maximum range will be set to [-1;+1] with
      allowed clipping fraction of 1%.
    */
    ClippingFraction ();
    
    /*!
      \brief Constructor
      
      The clipping fraction by default will be set to 1%.
      
      \param limits -- Maximum valid signal limits; the maximum and the minimum
                       value in the provided array are considered as the lower
		       and upper value respectively at which a signal is
		       considered to be clipping, i.e.
		       \f$ x_{\rm min} \leq x \leq x_{\rm max} \f$.
    */
    ClippingFraction (const Vector<T>& limits);
    
    /*!
      \brief Constructor
      
      \param limits -- Maximum valid signal limits; the maximum and the minimum value
      in the provided array are considered as the lower and upper value
      respectively at which a signal is considered to be clipping.
      \param fraction -- Maximum fraction of values (samples) in a data block
      which are allowed to be clipping.
    */
    ClippingFraction (const Vector<T>& limits,
		      const T& fraction);
    
    // --- Destruction -------------------------------------------------
    
    ~ClippingFraction ();
    
    // --- Operators ---------------------------------------------------
    
    ClippingFraction<T> &operator=(const ClippingFraction<T> &other);
    
    // --- Flagger parameters ------------------------------------------
    
    /*!
      \brief Get the value of the clipping fraction parameter.
      
      \return fraction -- Maximum fraction of values (samples) in a data block
      which are allowed to be clipping.
    */
    T clipFraction ();
    
  /*!
    \brief Set the value of the clipping fraction parameter.

    \param fraction -- Maximum fraction of values (samples) in a data block
           which are allowed to be clipping.
  */
  void setClipFraction (const T& fraction);

  /*!
    \brief Get the values of the signal clipping limits.
    
    \return limits -- 
  */
  Vector<T> limits ();
  
  /*!
    \brief Set the signal clipping limits.

    \param clipLow  -- 
    \param clipHigh -- 
  */
  void setLimits (const T& clipLow,
		  const T& clipHigh);
  
  /*!
    \brief Set the signal clipping limits.

    \param limits -- Maximum valid signal limits; the maximum and the minimum
           value in the provided array are considered as the lower and upper
	   value respectively at which a signal is considered to be clipping.
  */
  void setLimits (const Vector<T>& limits);

  // --- Apply flagging to a block of data ---------------------------
  
  /*!
    \brief Evaluate clipping criterion for a block of data
    
    \param data -- The data.

    \retval fraction -- Fraction of entries/samples in the data block considered
            to be clipping.

    \return ok -- Is the clipping fraction criterion exceeded?
  */
  bool eval (const Vector<T>& data,
	     T& fraction);
  
};

}  // Namespace LOPES -- end

#endif
