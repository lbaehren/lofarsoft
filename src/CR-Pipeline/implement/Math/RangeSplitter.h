/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#ifndef _RANGESPLITTER_H_
#define _RANGESPLITTER_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

#include <casa/namespace.h>

/*!
  \class RangeSplitter

  \ingroup Math

  \brief Split a range of input values into subsets

  \author Lars B&auml;hren

  \date 2006/03/27

  \test tRangeSplitter.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[start filling in your text here]
  </ul>

  <h3>Synopsis</h3>

  This is a common problem: we need to split a range of values -- be it a
  frequency band, distance zones or simple array indices -- in into segments.

  <h3>Example(s)</h3>

*/

template <class T> class RangeSplitter {

  //! Which segmentation scheme to apply?
  String method_p;
  //! The number of segments into which the range of values is split
  Int nofSegments_p;
  //! Limits of the generated segments
  Vector<T> rangeLimits_p;
  
 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  RangeSplitter ();

  /*!
    \brief Argumented constructor

    \param method -- 
  */
  RangeSplitter (const String& method);
  
  /*!
    \brief Argumented constructor

    \param nofSegments -- 
  */
  RangeSplitter (const Int& nofSegments);
  
  /*!
    \brief Argumented constructor

    \param nofSegments -- 
    \patam method      -- 
  */
  RangeSplitter (const Int& nofSegments,
		 const String& method);
  
  /*!
    \brief Copy constructor

    \param other -- Another RangeSplitter object from which to create this new
                    one.
  */
  RangeSplitter (const RangeSplitter& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~RangeSplitter ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another RangeSplitter object from which to make a copy.
  */
  RangeSplitter<T>& operator= (const RangeSplitter& other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the segmentation scheme applied to the range

    \return method -- The segmentation scheme applied to the range
  */
  String method () {
    return method_p;
  }

  /*!
    \brief Get the number of segments into which the range of values is split
    
    \return nofSegments -- The number of segments into which the range of values
                           is split
  */
  Int nofSegments () {
    return nofSegments_p;
  }

  /*!
    \brief Set the number of segments into which the range of values is split
    
    \param nofSegments -- The number of segments into which the range of values
                          is split
  */
  void setNofSegments (const Int& nofSegments) {
    nofSegments_p = nofSegments;
  }

  /*!
    \brief Get the limits of the range segments

    \return rangeLimits -- 
  */
  Vector<T> rangeLimits () {
    return rangeLimits_p;
  }
  
  // -------------------------------------------------------------------- Methods
  
  /*!
    \brief Split the range of values

    \param input -- Vector of input values defining the range to be divided into
                    segments; the provided vector can have arbitrary number of 
		    elements, as the value range is derived internally as minimum
		    and maximum value.

    \return status -- Returns <i>True</i> if everything went fine
  */
  Bool split (const Vector<T>& input);

  /*!
    \brief Split the range of values

    \param minValue -- Minimum value of the range.
    \param maxValue -- Maximum value of the range.

    \return status -- Returns <i>True</i> if everything went fine
  */
  Bool split (const T& minValue,
	      const T& maxValue);
  
  
  /*!
    \brief Split the range of values

    \param input -- Vector of input values defining the range to be divided into
                    segments; the provided vector can have arbitrary number of 
		    elements, as the value range is derived internally as minimum
		    and maximum value.
    \param nofSegments -- number of segments into which the range if subdivided

    \return status -- Returns <i>True</i> if everything went fine
  */
  Bool split (const Vector<T>& input,
	      const Int& nofSegments);
  
  /*!
    \brief Split the range of values

    \param input -- Vector of input values defining the range to be divided into
                    segments; the provided vector can have arbitrary number of 
		    elements, as the value range is derived internally as minimum
		    and maximum value.
    \param nofSegments -- number of segments into which the range if subdivided
    \param method      -- Segmentation method (lin, log)

    \return status -- Returns <i>True</i> if everything went fine
  */
  Bool split (const Vector<T>& input,
	      const Int& nofSegments,
	      const String& method="lin");
  
 private:

  /*!
    \brief Unconditional copying
  */
  void copy (const RangeSplitter& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Split the given range into segments

    This is the core function actually carrying out the segmentation of the 
    range of values,

    \param min -- Minimum of the range of values
    \param max -- Maximum of the range of values
   */
  Bool splitRange (const T& min,
		   const T& max);

};

#endif /* _RANGESPLITTER_H_ */
