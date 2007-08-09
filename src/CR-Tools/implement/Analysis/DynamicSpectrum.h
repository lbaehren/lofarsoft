/***************************************************************************
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

/* $Id: DynamicSpectrum.h,v 1.2 2006/04/13 13:54:15 bahren Exp $*/

#ifndef _DYNAMICSPECTRUM_H_
#define _DYNAMICSPECTRUM_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>

#include <casa/namespace.h>

/*!
  \class DynamicSpectrum

  \ingroup Analysis
  
  \brief A framework for handling dynamic spectra

  \author Lars B&auml;hren

  \date 2006/02/22

  \test tDynamicSpectrum.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++/CASA]  Module <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates.html">Coordinates</a>
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class DynamicSpectrum {

  //! Array storing the pixel values of the dynamic spectrum, [freq,time]
  Matrix<Complex> dynamicSpectrum_p;

  //! Observation information
  ObsInfo observationInfo_p;
  //! Time axis of the dynamic spectrum
  LinearCoordinate timeAxis_p;
  //! Frequency axis of the dynamic spectrum 
  SpectralCoordinate freqAxis_p;

 public:

  /*!
    \brief Type of dynamic spectrum
  */
  enum ds_type {
    //! Beamformed dynamic spectrum
    BEAM,
    //! Total power dynamic spectrum
    POWER
  };

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  DynamicSpectrum ();

  /*!
    \brief Argumented constructor

    \param crval -- 
    \param cdelt -- 
    \param units -- 
  */
  DynamicSpectrum (const Vector<Double>& crval,
		   const Vector<Double>& cdelt,
		   const Vector<String>& units);

  /*!
    \brief Copy constructor

    \param other -- Another DynamicSpectrum object from which to create this new
                    one.
  */
  DynamicSpectrum (DynamicSpectrum const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~DynamicSpectrum ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another DynamicSpectrum object from which to make a copy.
  */
  DynamicSpectrum &operator= (DynamicSpectrum const &other); 

  // ----------------------------------------------------------------- Parameters

  void setDynamicSpectrum ();

  /*!
    \brief Set the parameters defining the time axis

    \param crval -- Reference value for the time axis; this is the time
                    corresponding to the first pixel in the dynamic spectrum.
    \param cdelt -- Increment along the time axis.
    \param unit  -- Unit within which the values are given.
  */
  void setTimeAxis (const Double& crval,
		    const Double& cdelt,
		    const String& unit);

  /*!
    \brief Set the parameters defining the time axis

    \param crval -- Reference value for the time axis; this is the time
                    corresponding to the first pixel in the dynamic spectrum.
    \param cdelt -- Increment along the time axis.
   */
  void setTimeAxis (const Quantum<Double>& crval,
		    const Quantum<Double>& cdelt);

  /*!
    \brief Set the parameters defining the frequency axis

    \param crval -- Reference value for the frequency axis; this is the time
                    corresponding to the first pixel in the dynamic spectrum.
    \param cdelt -- Increment along the frequency axis.
    \param unit  -- Unit within which the values are given.
  */
  void setFrequencyAxis (const Double& crval,
			 const Double& cdelt,
			 const String& unit);
  
  /*!
    \brief Set the parameters defining the frequency axis

    \param crval -- Reference value for the frequency axis; this is the frequency
                    corresponding to the first pixel in the dynamic spectrum.
    \param cdelt -- Increment along the frequency axis.
   */
  void setFrequencyAxis (const Quantum<Double>& crval,
			 const Quantum<Double>& cdelt);
  
  /*!
    \brief Retrieve the coordinate system attached to the dynamic spectrum

    \return coordinateSystem -- An AIPS++ CoordinateSystem object
  */
  CoordinateSystem coordinateSystem ();

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Retrieve the average spctrum

    \return averageSpectrum -- Average spectrum; obtained from the dynamic
                               spectrum by collapsing parallel to the time
			       axis.
  */
  Vector<Float> averageSpectrum ();

  /*!
    \brief Retrieve the total power as function of time

    \return totalPower -- Total power as function of time; obtained from the
                          dynamic spectrum by collapsing parallel to the
			  frequency axis.
  */
  Vector<Float> totalPower ();

  /*!
    \brief Export the dynamic spectrum to FITS

    \todo Still needs to be implemented
  */
  void toFITS (const string& outfile);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (DynamicSpectrum const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

};

#endif /* _DYNAMICSPECTRUM_H_ */
