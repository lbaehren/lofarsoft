/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: CalTableInterpolater.h,v 1.6 2006/11/10 16:53:36 horneff Exp $*/

#ifndef CALTABLEINTERPOLATER_H
#define CALTABLEINTERPOLATER_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <scimath/Mathematics/InterpolateArray1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>
#include <scimath/Functionals/ArraySampledFunctional.h>
#include <casa/namespace.h>

#include <Calibration/CalTableReader.h>

/*!
  \class CalTableInterpolater

  \ingroup Calibration

  \brief Interpolate a CalTable field along one or more axes.

  \author Andreas Horneffer

  \date 2006/10/13

  \test tCalTableInterpolater.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li> CalTableReader
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

#define MAX_NUM_AXIS 5
#define MAX_NUM_ANTENNAS 31

template <class T> class CalTableInterpolater {

 protected:

  Bool isKeyword;
  
  /*!
    \brief Pointer to the assigned CalTableReader
  */
  CalTableReader *reader_p;

  /*!
    \brief Name of the field that is to be interpolated
  */
  String FieldName_p;
  
  /*!
    \brief Names of the fields that contain the axis values over wich the interpolation is done.
  */
  Vector<String> AxisNames_p;

  /*!
    \brief Number of axes over which is interpolated.
  */
  Int NumAxes_p;


  /*!
    \brief Axis values for which the interpolated values are to be found.
  */
  Vector<Double> AxisValues_p[MAX_NUM_AXIS];

  /*!
    \brief Has the axis or its values been changed since the last call?
  */
  Bool AxisChanged_p[MAX_NUM_AXIS];

  /*!
    \brief The Cached data. (What is cached depends on the number of axes.)
  */
  Array<T> CachedData_p[MAX_NUM_ANTENNAS];

  /*!
    \brief Date of the cached data;
  */
  uInt CachedTime_p[MAX_NUM_ANTENNAS];

  /*!
    \brief The AntennaIDs of the antennas for which data is cached.
  */
  Int CachedAntennaIDs_p[MAX_NUM_ANTENNAS];

  /*!
    \brief Index of the next empty cache position.
  */
  int NextEmptyCache;


 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  CalTableInterpolater();


  /*!
    \brief Augmented constructor
  */
  CalTableInterpolater(CalTableReader *readerObject);


  /*!
    \brief Augmented constructor

    \param readerObject -- 
    \param FieldName    -- 
    \param AxisNames    -- 

    <b>Warning:</b> This does not perform any testing of the values (yet).
  */
  CalTableInterpolater(CalTableReader *readerObject,
		       const String &FieldName, 
		       const Vector<String> &AxisNames);
  

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~CalTableInterpolater ();

  // ------------------------------------------------------------------ Operators

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Set the CalTableReader

    \param readerObject -- Pointer to the CalTableReader object
    
    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  Bool AttatchReader(CalTableReader *readerObject);

  /*!
    \brief Set the Field that is to be imterpolated

    \param FieldName -- Name of the field
    
    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  Bool SetField(const String &FieldName);

  /*!
    \brief Set or add the name of a field that contains the axis values over 
            wich the interpolation is done.

    \param AxisName -- Name of the axis field
    \param AxisNum=0 -- number of the axis, 0 == add a new axis
    
    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  Bool SetAxis(const String &AxisName, Int AxisNum=0);

  // -------------------------------------------------------------------- Methods

 
  /*!
    \brief Set the values onto which the data is to be interpolated.

    \param AxisNum -- number of the axis (1 <= AxisNum <= NumAxes_p)
    \param values -- The values for this axis
    
    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  Bool SetAxisValue(Int AxisNum, Vector<Double> values);
  
  /*!
    \brief Get the interpolated values

    \param date -- date for which the data is requested
    \param AntID -- ID of the antenna for which the data is requested
    \param *result -- Place where the returned data can be stored

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.

    <B>Note: The result will be a reference to internal data of this object. So the
    contents of it may change when GetValues is called the next time for any AntID.</B>
    I'll try to do some caching...
  */

  Bool GetValues(uInt const date, Int const AntID, Array<T> *result);


 private:

  /*!
    \brief Unconditional deletion 
  */
  void destroy();
  
  /*!
    \brief Initialize the object
    
    \return ok -- Was operation successful? Returns \c True if yes.
  */
  void init();

  int IndexFromAntID(const Int AntID);

};

#endif /* CALTABLEINTERPOLATER_H */
