/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
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

#ifndef HANNINGFILTER_H
#define HANNINGFILTER_H

// Standard header files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

// AIPS++ header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

#include <Math/BasicFilter.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class HanningFilter
    
    \ingroup CR_Math
    
    \brief A generalized Hanning filter 
    
    \author Lars B&auml;hren
    
    \date January 26, 2005
    
    \test tHanningFilter.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[CR] BasicFilter
    </ul>
    
    <h3>Synopsis</h3>
    
    A simple Hanning filter for taping of data:
    \f[
      h[n] = \alpha - (1-\alpha) \cdot \cos \left( \frac{2 \pi n}{N-1} \right)
    \f]
    where \f$ \alpha \f$ is a parameter controlling the slope of the Hanning 
    filter.
    
    <img src="../figures/tHanningFilter.png">
  */
  
  template<class T> class HanningFilter : public BasicFilter<T> {
    
    //!  Parameter for the slope of the Hanning filter.
    T alpha_p;
    
    //! Width of the plateau
    uint beta_p;
    
    //! Width of the raising slope
    uint betaRise_p;
    
    //! Width of the 
    uint betaFall_p;
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Default constructor 
    */
    HanningFilter ();
    
    /*!
      \brief Argumented constructor.
      
      Default value for the slope parameter: \f$ \alpha = 0.5 \f$.
      
      \param blocksize -- Number of frequency channels in a block of Fourier
                          transformed data.
    */
    HanningFilter (unsigned int const &blocksize);
    
    /*!
      \brief Argumented constructor.
      
      \param blocksize -- Number of frequency channels in a block of Fourier
                          transformed data.
      \param alpha     -- Parameter for the slope of the Hanning filter.
    */
    HanningFilter (unsigned int const &blocksize,
		   T const &alpha);
    
    /*!
      \brief Argumented constructor.
      
      \param blocksize -- Number of frequency channels in a block of Fourier
      transformed data.
      \param alpha     -- Parameter for the slope of the Hanning filter.
      \param beta      -- Width of the plateau, where the window function is
      unity
    */
    HanningFilter (uint const &blocksize,
		   T const &alpha,
		   uint const &beta);
    
    /*!
      \brief Argumented constructor.
    
      \param blocksize -- Number of frequency channels in a block of Fourier
      transformed data.
      \param alpha     -- Parameter for the slope of the Hanning filter.
      \param beta      -- Width of the plateau, where the window function is
      unity.
      \param betaRise  -- Width before plateau, where the window function is 
      smaller then unity.
      \param betaFall  -- Width after plateau, where the window function is also 
      smaller then unity. The sum of the three beta-parameters must equal the 
      blocksize.
    */
    HanningFilter (uint const &blocksize,
		   T const &alpha,
		   uint const &beta,
		   uint const &betaRise,
		   uint const &betaFall);
    /*!
      \brief Copy constructor
    */
    HanningFilter (const HanningFilter<T>& other);
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor.
    */
    ~HanningFilter ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another HanningFilter object from which to make a copy.
    */
    HanningFilter<T> &operator= (HanningFilter<T> const &other); 
    
    // --- Methods ---------------------------------------------------------------  
    
    /*!
      \brief Get the slope parameter 
      
      \return alpha -- Parameter for the slope of the Hanning filter.
    */
    inline T alpha () const {
      return alpha_p;
    }
    
    /*!
      \brief Set the slope parameter 
      
      \param alpha -- Parameter for the slope of the Hanning filter.
    */
    void setAlpha (const T& alpha);
    
    /*!
      \brief Get the width of the plateau, [samples/channels]
      
      \return beta -- Width of the plateau, [samples/channels]
    */
    inline uint beta () const {
      return beta_p;
    }
    
    /*!
      \brief Set the width of the plateau, [samples/channels]
      
      \param beta -- Width of the plateau, [samples/channels]
    */
    void setBeta (uint const &beta);
    
    /*!
      \brief Set the width of the plateau, [samples/channels]
      
      \param beta -- Width of the plateau, [samples/channels]
    */
    void setBeta (uint const &beta,
		  uint const &betaRise,
		  uint const &betaFall);
    
    /*!
      \brief For an unsymmetrical filter, get the parameter for the rising slope
      
      \return betaRise -- 
    */
    inline uint betaRise () const {
      return betaRise_p;
    }
    
    /*!
      \brief For an unsymmetrical filter, get the parameter for the falling slope
      
      \return betaFall -- 
    */
    inline uint betaFall () const {
      return betaFall_p;
    }
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (HanningFilter<T> const& other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Set up the filter weights
    */
    void setWeights ();  
    
  };

}  // Namespace CR -- end

#endif
