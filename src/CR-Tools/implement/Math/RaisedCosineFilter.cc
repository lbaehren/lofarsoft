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

#include <Math/RaisedCosineFilter.h>

const double pi = 3.1415926535897932384626433832795;

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                           RaisedCosineFilter

  /*!
    \param blocksize -- 
    \param delta     -- 
  */
  template <class T>
  RaisedCosineFilter<T>::RaisedCosineFilter (unsigned int const &blocksize,
					     int const &delta) 
    : BasicFilter<T> (blocksize)
  {
    init (blocksize, delta, delta);
  }
  
  //_____________________________________________________________________________
  //                                                           RaisedCosineFilter
  
  /*!
    \param blocksize -- 
    \param deltaRise -- 
    \param deltaFall -- 
  */
  template <class T>
  RaisedCosineFilter<T>::RaisedCosineFilter (unsigned int const &blocksize,
					     int const &deltaRise,
					     int const &deltaFall) 
    : BasicFilter<T> (blocksize)
  {
    init (blocksize, deltaRise, deltaFall);
  }
  
  //_____________________________________________________________________________
  //                                                           RaisedCosineFilter
  
  /*!
    \param blocksize          -- 
    \param transitionFraction -- (Symmetric) Fraction of the blocksize, over
           which the transition between 0 and 1 takes place.
  */
  template <class T>
  RaisedCosineFilter<T>::RaisedCosineFilter (unsigned int const &blocksize,
					     T const &transitionFraction)
    : BasicFilter<T> (blocksize)
  {
    int delta (0);
    //
    delta = int(T(blocksize)*transitionFraction);
    //
    init (blocksize, delta, delta);
  }
  
  //_____________________________________________________________________________
  //                                                           RaisedCosineFilter

  /*!
    \param blocksize      -- 
    \param transitionRise -- 
    \param transitionFall -- 
  */
  template <class T>
  RaisedCosineFilter<T>::RaisedCosineFilter (unsigned int const &blocksize,
					     T const &transitionRise,
					     T const &transitionFall)
    : BasicFilter<T> (blocksize)
  {
    int deltaRise (0);
    int deltaFall (0);
    //
    deltaRise = int(T(blocksize)*transitionRise);
    deltaFall = int(T(blocksize)*transitionFall);
    //
    init (blocksize, deltaRise, deltaFall);
  }
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  template <class T>
  RaisedCosineFilter<T>::~RaisedCosineFilter ()
  {;}
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  
  template <class T>
  void RaisedCosineFilter<T>::init (const int& blocksize,
				    const int& deltaRise,
				    const int& deltaFall)
  {
    deltaRise_p = deltaRise;
    deltaFall_p = deltaFall;
    alpha_p     = 0.0;
    // Initialize the weights & the blocksize 
    BasicFilter<T>::setWeights (std::vector<T>(blocksize,1));
    // variables
    int deltaConst (blocksize-deltaRise-deltaFall);
    /* Test error catching. */
    if (deltaRise > blocksize) {
      std::cerr << "ERROR: raise transition region exceeds filter length!"
		<< std::endl;
    } else if (deltaConst < 0) {
      std::cerr << "ERROR: the sum of the transition regions exceeds filter length!"
		<< std::endl;
    } else if (deltaFall > blocksize){
      std::cerr << "ERROR: fall transition region exceeds filter length!"
		<< std::endl;
    } else {
      setWeights();
    }
    
  }
  
  //_____________________________________________________________________________
  //                                                                   setWeights

  template <class T>
  void RaisedCosineFilter<T>::setWeights ()
  {
    int n                  = 0;
    unsigned int blocksize = BasicFilter<T>::blocksize();
    T alpha                = 0.0;
    
    Vector<T> weights (blocksize,1.0);
    
    // raising segment
    alpha = pi/(deltaRise_p);
    for (n=0; n<deltaRise_p; n++) {
      weights[n] = 0.5*(cos(alpha*(n)+pi) + 1);
    }
    // falling transition
    alpha = pi/(deltaFall_p);
    for (n=0; n<deltaFall_p; n++) {
      weights[blocksize-deltaFall_p+n] = 0.5*(cos(alpha*(n)) + 1);
    }
    
    // debugging output
    for (n=0; n<blocksize; n++) {
      std::cout << n << "\t" << weights[n] << std::endl;
    }
    
  }

}  // Namespace CR -- end
