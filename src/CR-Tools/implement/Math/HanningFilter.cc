/*-------------------------------------------------------------------------*
 | $Id:: SkymapCoordinates.h 396 2007-06-15 13:19:42Z baehren            $ |
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

#include <Math/Constants.h>
#include <Math/HanningFilter.h>

namespace CR {  // Namespace CR -- begin
  
  // =============================================================================
  //
  //  Construction
  //
  // =============================================================================
  
  template <class T> 
  HanningFilter<T>::HanningFilter ()
    : BasicFilter<T> (),
      alpha_p (0.5)
  {
    beta_p = 0;
  }
  
  template <class T> 
  HanningFilter<T>::HanningFilter (unsigned int const &channels)
    : BasicFilter<T> (channels),
      alpha_p (0.5)
  {
    beta_p = 0;
  }
  
  template <class T> 
  HanningFilter<T>::HanningFilter (unsigned int const &channels,
				   T const &alpha)
    : BasicFilter<T> (channels),
      alpha_p (alpha)
  {
    beta_p = 0;
  }
  
  template <class T> 
  HanningFilter<T>::HanningFilter (unsigned int const &channels,
				   T const &alpha,
				   uint const &beta)
    : BasicFilter<T> (channels),
      alpha_p (alpha),
      beta_p (beta)
  {
    setBeta (beta);
  }
  
  template <class T> 
  HanningFilter<T>::HanningFilter (unsigned int const &channels,
				   T const &alpha,
				   uint const &beta,
				   uint const &betaRise,
				   uint const &betaFall)
    : BasicFilter<T> (channels),
      alpha_p (alpha),
      beta_p (beta),
      betaRise_p (betaRise),
      betaFall_p (betaFall)
  {
    setBeta (beta,
	     betaRise,
	     betaFall);
  } 

  template <class T> 
  HanningFilter<T>::HanningFilter (HanningFilter<T> const &other)
  {
    copy (other);
  }
  
  // =============================================================================
  //
  //  Destruction
  //
  // =============================================================================
  
  template <class T> 
  HanningFilter<T>::~HanningFilter ()
  {;}
  
  // =============================================================================
  //
  //  Operators
  //
  // =============================================================================
  
  template <class T> 
  HanningFilter<T> &HanningFilter<T>::operator= (HanningFilter<T> const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
}
  
  template <class T> 
  void HanningFilter<T>::copy (HanningFilter<T> const& other)
  {
    // make a copy of the base class
    this->BasicFilter<T>::operator=(other);
    // copy of elements of this class
    alpha_p    = other.alpha_p;
    beta_p     = other.beta_p;
    betaRise_p = other.betaRise_p;
    betaFall_p = other.betaFall_p;
  }
  
  template <class T> 
  void HanningFilter<T>::destroy(void)
  {;}
  
  // =============================================================================
  //
  //  Methods
  //
  // =============================================================================
  
  // ------------------------------------------------------ HanningFilter::setAlpha
  
  template <class T> 
  void HanningFilter<T>::setAlpha (const T& alpha)
  {
    alpha_p = alpha;
    setWeights ();
  }
  
  // ------------------------------------------------------- HanningFilter::setBeta
  
  template <class T> 
  void HanningFilter<T>::setBeta (uint const &beta)
  {
    unsigned int blocksize (BasicFilter<T>::blocksize());
    
    if (beta == 0) {
      beta_p     = beta;
      betaRise_p = 0;
      betaFall_p = 0;
      //
      setWeights ();
    } else {
      if (beta<blocksize) {
	uint betaRise (BasicFilter<T>::blocksize()-beta);
	uint betaFall (BasicFilter<T>::blocksize()-beta);
	//
	betaRise /= 2;
	betaFall /= 2;
	//
	setBeta (beta,
		 betaRise,
		 betaFall);
      }
    }
    
  }
  
  template <class T> 
  void HanningFilter<T>::setBeta (uint const &beta,
				  uint const &betaRise,
				  uint const &betaFall)
  {
    unsigned int blocksize (BasicFilter<T>::blocksize());
    uint width (beta+betaRise+betaFall);
    
    /*
      We always need to check that the sum of the elements in the segments does not
      exceed the total number of filter points
    */
    
    if (width>blocksize) {
      cerr << "Sum of segments exceeds blocksize!" << endl;
      if (beta>blocksize) {
	setBeta (0);
      } else {
	setBeta (beta);
      }
    } else {  
      beta_p     = beta;
      betaRise_p = betaRise;
      betaFall_p = betaFall;
      //
      setWeights ();  
    }
  }
  
  // ---------------------------------------------------- HanningFilter::setWeights
  
  template <class T> 
  void HanningFilter<T>::setWeights ()
  {
    unsigned int blocksize (BasicFilter<T>::blocksize());
    Vector<T> weights (blocksize,T(1));
    T factor (0);
    T x (0);
    
    if (beta_p == 0) {
      factor = 2*CR::pi/(blocksize-1);
      
      for (unsigned int n(0); n<blocksize; n++) {
	x = (T)n*factor;
	weights(n) = alpha_p - (T(1)-alpha_p)*cos(x);
      }
    }
    else {
      uint n(0);
      
      // raising segment
      factor = CR::pi/(betaRise_p-1);
      for (n=0; n<betaRise_p; n++) {
	x = (T)n*factor;
	weights(n) = alpha_p - (T(1)-alpha_p)*cos(x);
      }
      
      // falling segment
      factor = CR::pi/(betaFall_p-1);
      for (n=0; n<betaFall_p; n++) {
	x = (T)n*factor+T(CR::pi);
	weights(blocksize-betaFall_p+n) = alpha_p - (T(1)-alpha_p)*cos(x);
      }
    }
    
    /* store the computed values */
    BasicFilter<T>::setWeights(weights);
  }
  
  // =============================================================================
  //
  //  Template instantiation
  //
  // =============================================================================
  
  template class HanningFilter<casa::Float>;
  template class HanningFilter<casa::Double>;
  template class HanningFilter<casa::Complex>;
  template class HanningFilter<casa::DComplex>;
  
}  // Namespace CR -- end
