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

/* $Id$*/

#ifndef _STATISTICSFILTER_H_
#define _STATISTICSFILTER_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>

#include <casa/namespace.h>

/*!
  \class FilterType
  
  \ingroup Math

  \author Lars B&auml;hren

  \brief Enum for the various filter types
*/
class FilterType {
 public:
  //! The various filter types implemented in the StatisticsFilter class
  enum filter {
    //! Use maximum of the values
    MAXIMUM,
    //! Use median of values
    MEAN,
    //! Use mean of values
    MEDIAN,
    //! Use minimum of the values
    MINIMUM,
    //! Root mean square of the values
    RMS,
    //! Use standard deviation of values
    STDDEV
  };
};

/*!
  \class StatisticsFilter

  \brief A filter based on statistical properties of the data

  \author Lars B&auml;hren

  \date 2005/12/21

  \test tStatisticsfilter.cc

  <h3>Prerequisite</h3>

  <ul type=square>
    <li>[LOPES-Tools] FilterType
  </ul>

  <h3>Synopsis</h3>

  This essentially is an re-implementation of the code found in lopestools.cc

  <h3>Example(s)</h3>

  <ol>
    <li>Working with vector input:
    \code
    StatisticsFilter<Float> mf (strength);
    Vector<Float> input (nofChannels,1.0);
    Vector<Float> output (mf.filter(input));
    \endcode
    
    <li>Working with matrix input:
    \code
    StatisticsFilter<Float> mf (strength);
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
    StatisticsFilter<Float> mf (strength);
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

template <class T> class StatisticsFilter {

  //! Type of the filter
  FilterType::filter filterType_p;
  //! Strength of the filter, i.e. the number of bins over which to average
  unsigned int strength_p;
  //! Length of a vector of input data
  unsigned int blocksize_p;
  //! Number of data sets (when working on matrix input)
  unsigned int nofDataSets_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  StatisticsFilter ();

  /*!
    \brief Argumented constructor

    \param strength -- Strength of the filter, i.e. the number of bins over
                       which to average
  */
  StatisticsFilter (const unsigned int& strength);

  /*!
    \brief Argumented constructor

    \param strength   -- Strength of the filter, i.e. the number of bins over
                         which to average
    \param filterType -- Which type of filter to use (one of the types in
                         FilterType)
  */
  StatisticsFilter (const unsigned int& strength,
		    const FilterType::filter filterType);

  /*!
    \brief Copy constructor

    \param other -- Another StatisticsFilter object from which to create this new
                    one.
  */
  StatisticsFilter (const StatisticsFilter<T>& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~StatisticsFilter ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another StatisticsFilter object from which to make a copy.
  */
  StatisticsFilter &operator= (StatisticsFilter<T> const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Strength of the filter

    \return strength -- Strength of the filter, i.e. the number of bins over
                        which to average
  */
  unsigned int strenght () {
    return strength_p;
  }

  /*!
    \brief Strength of the filter

    \param strength -- Strength of the filter, i.e. the number of bins over
                       which to average
  */
  void setStrength (const unsigned int& strength) {
    strength_p = strength;
  }

  /*!
    \brief Get the filter type

    \return filterType -- The type of the filter (e.g. MEAN)
  */
  FilterType::filter filterType () {
    return filterType_p;
  }

  /*!
    \brief Set the filter type

    \param filterType -- The type of the filter (e.g. MEAN)
  */
  void setFilterType (const FilterType::filter type) {
    filterType_p = type;
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
  void copy (const StatisticsFilter<T>& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Apply the filter to the data
   */
  Vector<T> applyFilter (const Vector<T>& data);
  
};

#endif /* _STATISTICSFILTER_H_ */
