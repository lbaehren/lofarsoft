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

/* $Id: MedianFilter.h,v 1.6 2006/04/14 11:11:57 bahren Exp $*/

#ifndef _MEDIANFILTER_H_
#define _MEDIANFILTER_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

#include <casa/namespace.h>

/*!
  \class MedianFilter.h

  \ingroup Functionals

  \brief A Meadian filter

  \author Lars B&auml;hren

  \date 2005/12/21

  \test tMedianFilter.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  This essentially is an re-implementation of the code found in lopestools.cc

  <h3>Example(s)</h3>

  <ol>
    <li>Working with vector input:
    \code
    MedianFilter<Float> mf (strength);
    Vector<Float> input (nofChannels,1.0);
    Vector<Float> output (mf.filter(input));
    \endcode
    
    <li>Working with matrix input:
    \code
    MedianFilter<Float> mf (strength);
    Matrix<Float> input (nofChannels,1.0);
    IPosition shape (input.shape());
    Matrix<Float> output (shape);
    
    for (int n(0); n<shape(1); n++) {
      output.column(n) = mf.filter(input.column(n));
    }
    \endcode
    Since this type of processing is supported internally, you also simply might
    say:
    \code
    MedianFilter<Float> mf (strength);
    Matrix<Float> input (nofChannels,nofAntennas,1.0);
    Matrix<Float> output (mf.filter(input));
    \endcode
  </ol>

  Here is a short test script which can be run to compare the result from the new
  implementation to the output of the original one:
  \code
  data.setblock(n=1,blocksize=512);
  fx := data.get("f(x)");
  f := cMedFilter (fx, strength=5);
  \endcode

*/

template <class T> class MedianFilter {
  
  //! Strength of the filter, i.e. the number of bins over which to average
  int strength_p;
  //! Length of a vector of input data
  int blocksize_p;
  //! Number of data sets (when working on matrix input)
  int nofDataSets_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  MedianFilter ();

  /*!
    \brief Argumented constructor

    \param strength -- Strength of the filter, i.e. the number of bins over
                       which to average
  */
  MedianFilter (const int& strength);

  /*!
    \brief Copy constructor

    \param other -- Another MedianFilter object from which to create this new
                    one.
  */
  MedianFilter (MedianFilter<T> const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~MedianFilter ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another MedianFilter object from which to make a copy.
  */
  MedianFilter &operator= (MedianFilter<T> const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Strength of the filter

    \return strength -- Strength of the filter, i.e. the number of bins over
                        which to average
  */
  int strenght () {
    return strength_p;
  }

  /*!
    \brief Strength of the filter

    \param strength -- Strength of the filter, i.e. the number of bins over
                       which to average
  */
  void setStrength (const int& strength) {
    strength_p = strength;
  }

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Apply the filter to data, non-destructive.

    \param data --

    \return result -- Data after application of the filter
  */
  Vector<T> filter (const Vector<T>& data);

  /*!
    \brief Apply the filter to data, non-destructive.

    \param data -- Multiple data sets, [x-axis,antenna].

    \return result -- Data after application of the filter
  */
  Matrix<T> filter (const Matrix<T>& data);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (MedianFilter<T> const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Apply the filter to the data
   */
  Vector<T> applyFilter (const Vector<T>& data);
  
};

#endif /* _MEDIANFILTER_H_ */
