//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* Util/genutil/RunningMedian.h */

#ifndef __RunningMedian_h
#define __RunningMedian_h

#include <set>         // defines multiset
#include <functional>  // defines less<>

//#define _DEBUG
//#include <iostream>

//! For use in the efficient computation of a running median
/*!
  There are quite a few different approaches to this problem, including

  1) O(sqrtN) algorithm by Soumya D. Mohanty

  2) O(logN) algorithm based on an indexable skiplist by Raymond Hettinger

  3) O(logN) algorithm based on std::multiset (TopCoder Match Editorial)

  The last of these is quite clever, and requires very little coding.

  This implementation is based on the example from the TopCoder web-site

  http://community.topcoder.com/tc?module=Static&d1=match_editorials&d2=srm310
  (accessed on 20 April 2012)
*/

template< typename T, typename Compare = std::less<T> >
class RunningMedian
{
  mutable std::multiset<T,Compare> lowerHalf, upperHalf;
  Compare lt;
  unsigned nth;

public:

  RunningMedian (unsigned n=0) { nth = n; }
  void set_nth (unsigned n) { nth = n; }
  unsigned get_nth () const { return nth; }

  //! Erases all of the elements
  void clear () { lowerHalf.clear(); upperHalf.clear(); }

  //! Insert an element
  void insert (const T& element)
  {
#ifdef _DEBUG
    std::cerr << "insert sizes lt=" << lowerHalf.size()
	      << " gt=" << upperHalf.size()
	      << " element=" << element << " low=" << *upperHalf.begin()
	      << std::endl;
#endif

    if (upperHalf.size() && lt( element, *upperHalf.begin() ))
      lowerHalf.insert( element );
    else
      upperHalf.insert( element );

#ifdef _DEBUG
    std::cerr << "insert. sizes lt=" << lowerHalf.size()
	      << " gt=" << upperHalf.size() << std::endl;
#endif
  }

  //! Erase an element
  void erase (const T& element)
  {
#ifdef _DEBUG
    std::cerr << "erase sizes lt=" << lowerHalf.size()
	      << " gt=" << upperHalf.size()
	      << " element=" << element << " low=" << *upperHalf.begin()
	      << std::endl;
#endif

    if (lt(element, *upperHalf.begin()))
      lowerHalf.erase( lowerHalf.find( element ) );
    else
      upperHalf.erase( upperHalf.find( element ) );

#ifdef _DEBUG
    std::cerr << "erase. sizes lt=" << lowerHalf.size()
	      << " gt=" << upperHalf.size() << std::endl;
#endif
  }

  //! Get the current median value
  const T& get_median () const
  {
#ifdef _DEBUG
    std::cerr << "median nth=" << nth << " sizes lt=" << lowerHalf.size()
	      << " gt=" << upperHalf.size() << " low=" << *upperHalf.begin()
	      << std::endl;
#endif

    while (upperHalf.size() > nth+1)
      lowerHalf.insert( popFirstElement( upperHalf ));

    while (lowerHalf.size() > nth)
      upperHalf.insert( popLastElement( lowerHalf ));

    return *upperHalf.begin();
  }
  
protected:

  /* helper functions */

  static T popFirstElement (std::multiset<T,Compare>& S)
  {
    T result = *(S.begin());
    S.erase( S.begin() );
    return result;
  }

  static T popLastElement (std::multiset<T,Compare>& S)
  {
    typename std::multiset<T>::iterator it = S.end();
    it--;
    T result = *it;
    S.erase( it );
    return result;
  }

};

#endif

