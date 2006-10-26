/***************************************************************************
 *   Copyright (C) <year>                                                  *
 *   <author> (<mail>)                                                     *
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
/* $Id: DataFlag.h,v 1.5 2006/05/04 11:46:29 bahren Exp $ */

#ifndef _DATAFLAG_H_
#define _DATAFLAG_H_

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>

#include <lopes/Analysis/ClippingFraction.h>

#include <casa/namespace.h>

/*!
  \class DataFlag

  \ingroup Data

  \brief Flags for a number of data sets.

  \author Lars B&auml;hren

  \date 2005/06/24

  \test tDataFlag.cc
 */

template <class T> class DataFlag : public ClippingFraction<T> {

  //! Threshold above which an antenna is flagged.
  T threshold_p;

  //! Flag values.
  Vector<T> flags_p;

  //! Test if signals are clipped?
  Bool testClipping_p;

 public:
  
  // --- Construction ----------------------------------------------------------

  /*!
    \brief Constructor

    \param num -- Number of data sets.
  */
  DataFlag (const Int& num);

  /*!
    \brief Constructor

    \param num       -- Number of data sets.
    \param threshold -- Threshold for the float to boolean conversion of the 
                        flag values.
  */
  DataFlag (const Int& num,
	    const T& threshold);

  /*!
    \brief Constructor

    \param num       -- Number of data sets.
    \param threshold -- Threshold for the float to boolean conversion of the 
                        flag values.
    \param limits    -- Maximum valid signal limits.
  */
  DataFlag (const Int& num,
	    const T& threshold,
	    const Vector<T>& limits);

  /*!
    \brief Constructor
    
    \param flags -- Flags for the data sets.
  */
  DataFlag (const Vector<Bool>& flags);

  /*!
    \brief Constructor
    
    \param flags -- Flags for the data sets.
  */
  DataFlag (const Vector<T>& flags);

  // --- Destruction -----------------------------------------------------------

  ~DataFlag ();

  // --- Flagging threshold ----------------------------------------------------

  /*!
    \brief Get the threshold for the float-to-boolean conversion of the flags

    \return threshold -- Threshold for the float-to-boolean conversion of the 
                         flag values.
  */
  T threshold ();

  /*!
    \brief 
    
    \param threshold -- Threshold for the float-to-boolean conversion of the 
                        flag values.
  */
  void setThreshold (const T& threshold);
  
  // --- Access to the flags ---------------------------------------------------

  /*!
    \brief Get flag for an individual dataset (as bool).
  */
  void flags (Bool& flag,
	      const uInt& num);

  /*!
    \brief Get flag for an individual dataset (as float).
  */
  void flags (T& flag,
	      const uInt& num);

  /*!
    \brief Get the flags for all datasets (as bool).
  */
  void flags (Vector<Bool>& flags);

  /*!
    \brief Get the flags for all datasets (as float).
  */
  void flags (Vector<T>& flags);

  /*!
    \brief 
  */
  void setFlags (const Vector<Bool>& flags);
  
  /*!
    \brief 
  */
  void setFlags (const Vector<T>& flags);

  /*!
    \brief 
  */
  void setFlags (const Matrix<T>& data);

  /*!
    \brief 
  */
  void setFlags (const Vector<T>& data,
		 const Int& num);
  
  /*!
    \brief 
  */
  void flagData (Vector<Bool>& flags,
		 const Matrix<T>& data);
  
  /*!
    \brief 
  */
  void flagData (Vector<T>& flags,
		 const Matrix<T>& data);

 private:

  /*!
    \brief Convert internal flag value (float) to boolean.
  */
  Bool float2bool (const T& flag);
};

#endif
