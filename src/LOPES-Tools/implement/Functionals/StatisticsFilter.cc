/***************************************************************************
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

/* $Id: StatisticsFilter.cc,v 1.7 2006/05/19 14:49:10 bahren Exp $*/

/*!
  \class StatisticsFilter
*/

#include <iostream>
#include <Functionals/StatisticsFilter.h>

namespace LOPES {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T>
  StatisticsFilter<T>::StatisticsFilter ()
    : strength_p (3)
  {
    setFilterType (FilterType::MEDIAN);
  }
  
  template <class T>
  StatisticsFilter<T>::StatisticsFilter (const unsigned int& strength)
    : strength_p (strength)
  {
    setFilterType (FilterType::MEDIAN);
  }
  
  template <class T>
  StatisticsFilter<T>::StatisticsFilter (const unsigned int& strength,
					 const FilterType::filter filterType)
    : strength_p (strength)
  {
    setFilterType (filterType);
  }
  
  template <class T>
  StatisticsFilter<T>::StatisticsFilter (const StatisticsFilter<T>& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  template <class T>
  StatisticsFilter<T>::~StatisticsFilter ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=
  
  template <class T>
  StatisticsFilter<T> &StatisticsFilter<T>::operator= (const StatisticsFilter<T>& other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy
  
  template <class T>
  void StatisticsFilter<T>::copy (const StatisticsFilter<T>& other)
  {
    filterType_p  = other.filterType_p;
    strength_p    = other.strength_p;
    blocksize_p   = other.blocksize_p;
    nofDataSets_p = other.nofDataSets_p;
  }
  
  // -------------------------------------------------------------------- destroy
  
  template <class T>
  void StatisticsFilter<T>::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
#ifdef HAVE_CASA

  // ---------------------------------------------------------------- applyFilter
  
  template <class T>
  casa::Vector<T> StatisticsFilter<T>::applyFilter (const casa::Vector<T>& data)
  {
    unsigned int i;
    casa::Vector<T> result (blocksize_p);
    casa::Matrix<T> tmp ((blocksize_p+strength_p),strength_p);
    
    //   std::cout << "[StatisticsFilter<T>::applyFilter]" << std::endl;
    //   std::cout << " - blocksize : " << blocksize_p << std::endl;
    //   std::cout << " - strength  : " << strength_p << std::endl;
    //   std::cout << " - result    : " << result.shape() << std::endl;
    //   std::cout << " - tmp       : " << tmp.shape()<< std::endl;
    
    // Preparation of the data
    for (i=0; i<strength_p; i++) {
      tmp(casa::Slice(i,blocksize_p),casa::Slice(i)) = data;
    };
    
    // This is the only section, where we actually need to distinguish between
    // the different filter types; main computation handled by ArrayMath.
    switch (filterType_p) {
    case FilterType::MAXIMUM:
      for (i=0; i<blocksize_p; i++) {
	result(i) = max(tmp.row(i+strength_p/2));
      };
      break;
    case FilterType::MEAN:
      for (i=0; i<blocksize_p; i++) {
	result(i) = mean(tmp.row(i+strength_p/2));
      };
      break;
    case FilterType::MEDIAN:
      for (i=0; i<blocksize_p; i++) {
	result(i) = median(tmp.row(i+strength_p/2));
      };
      break;
    case FilterType::MINIMUM:
      for (i=0; i<blocksize_p; i++) {
	result(i) = min(tmp.row(i+strength_p/2));
      };
      break;
    case FilterType::RMS:
      for (i=0; i<blocksize_p; i++) {
	result(i) = sqrt(
			 sum(
			     (tmp.row(i+strength_p/2))
			     *(tmp.row(i+strength_p/2))
			     )/blocksize_p
			 );
      };
      break;
    case FilterType::STDDEV:
      for (i=0; i<blocksize_p; i++) {
	result(i) = stddev(tmp.row(i+strength_p/2));
      };
      break;
    }
    
    return result;
  }
  
  // --------------------------------------------------------------------- filter
  
  template <class T>
  casa::Vector<T> StatisticsFilter<T>::filter (const casa::Vector<T>& data)
  {
    blocksize_p = data.nelements();
    
    if ((strength_p < 1) ||(strength_p > blocksize_p)) {
      strength_p = 1;
      std::cerr << "[StatisticsFilter::filter] Filter strength set to 1" << std::endl;
    }
    
    return applyFilter (data);
  }
  
  // --------------------------------------------------------------------- filter
  
  template <class T>
  casa::Matrix<T> StatisticsFilter<T>::filter (const casa::Matrix<T>& data)
  {
    casa::IPosition shape (data.shape());
    
    blocksize_p = shape(0);
    
    if ((strength_p < 1) ||(strength_p > blocksize_p)) {
      strength_p = 1;
      std::cerr << "[StatisticsFilter::filter] Filter strength set to 1"
		<< std::endl;
    }
    
    casa::Matrix<T> result (shape);
    
    try {
      for (int n(0); n<shape(1); n++) {
	result.column(n) = applyFilter (data.column(n));
      }
    } catch (casa::AipsError x) {
      std::cerr << "[StatisticsFilter::filter]" << x.getMesg()
		<< std::endl;
    }
    
    return result;
  }

#endif
  
  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================
  
  template class StatisticsFilter<float>;
  template class StatisticsFilter<double>;
 
}
