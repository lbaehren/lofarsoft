/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#ifdef HAVE_CFITSIO
extern "C" {
#include <cfitsio/fitsio.h>
}
#endif

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>

using casa::Quantum;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class DynamicSpectrum
    
    \ingroup Analysis
    
    \brief A framework for handling dynamic spectra
    
    \author Lars B&auml;hren
    
    \date 2006/02/22
    
    \test tDynamicSpectrum.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[casacore]  Module Coordinates
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  
  class DynamicSpectrum {
    
    //! Array storing the pixel values of the dynamic spectrum, [freq,time]
    casa::Matrix<double> dynamicSpectrum_p;

    //! Base name of the file to which the data can be exported
    std::string filename_p;
    
    //! Observation information
    casa::ObsInfo obsInfo_p;
    //! Time axis of the dynamic spectrum
    casa::LinearCoordinate timeAxis_p;
    //! Frequency axis of the dynamic spectrum 
    casa::SpectralCoordinate freqAxis_p;
    
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
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    DynamicSpectrum ();
    
    /*!
      \brief Argumented constructor
      
      \param crval -- [freq,time] 
      \param cdelt -- [freq,time] 
      \param units -- [freq,time] 
    */
    DynamicSpectrum (const Vector<double>& crval,
		     const Vector<double>& cdelt,
		     const Vector<String>& units);
    
    /*!
      \brief Argumented constructor

      \param obsInfo  -- 
      \param timeAxis -- 
      \param freqAxis -- 
    */
    DynamicSpectrum (casa::ObsInfo obsInfo,
		     casa::LinearCoordinate timeAxis,
		     casa::SpectralCoordinate freqAxis);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another DynamicSpectrum object from which to create this
             new one.
    */
    DynamicSpectrum (DynamicSpectrum const& other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~DynamicSpectrum ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another DynamicSpectrum object from which to make a copy.
    */
    DynamicSpectrum &operator= (DynamicSpectrum const &other); 
    
    // --------------------------------------------------------------- Parameters

    inline std::string filename () {
      return filename_p;
    }

    inline bool setFilename (std::string const &filename) {
      filename_p = filename;
    }
    
    void setDynamicSpectrum ();
    
    /*!
      \brief Set the parameters defining the time axis
      
      \param crval -- Reference value for the time axis; this is the time
             corresponding to the first pixel in the dynamic spectrum.
      \param cdelt -- Increment along the time axis.
      \param unit  -- Unit within which the values are given.
    */
    void setTimeAxis (const double& crval,
		      const double& cdelt,
		      const String& unit);
    
    /*!
      \brief Set the parameters defining the time axis
      
      \param crval -- Reference value for the time axis; this is the time
             corresponding to the first pixel in the dynamic spectrum.
      \param cdelt -- Increment along the time axis.
    */
    void setTimeAxis (const Quantum<double>& crval,
		      const Quantum<double>& cdelt);
    
    /*!
      \brief Set the parameters defining the frequency axis
      
      \param crval -- Reference value for the frequency axis; this is the time
             corresponding to the first pixel in the dynamic spectrum.
      \param cdelt -- Increment along the frequency axis.
      \param unit  -- Unit within which the values are given.
    */
    void setFrequencyAxis (const double& crval,
			   const double& cdelt,
			   const String& unit);
    
    /*!
      \brief Set the parameters defining the frequency axis
      
      \param crval -- Reference value for the frequency axis; this is the frequency
             corresponding to the first pixel in the dynamic spectrum.
      \param cdelt -- Increment along the frequency axis.
    */
    void setFrequencyAxis (const Quantum<double>& crval,
			   const Quantum<double>& cdelt);
    
    /*!
      \brief Retrieve the coordinate system attached to the dynamic spectrum
      
      \return coordinateSystem -- An AIPS++ CoordinateSystem object
    */
    casa::CoordinateSystem coordinateSystem ();
    
    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    bool processData (casa::Matrix<casa::DComplex> const &data);
    
    /*!
      \brief Retrieve the average spctrum
      
      \return averageSpectrum -- Average spectrum; obtained from the dynamic
              spectrum by collapsing parallel to the time
      axis.
    */
    Vector<float> averageSpectrum ();
    
    /*!
      \brief Retrieve the total power as function of time
      
      \return totalPower -- Total power as function of time; obtained from the
              dynamic spectrum by collapsing parallel to the
      frequency axis.
    */
    Vector<float> totalPower ();
    
    /*!
      \brief Export the dynamic spectrum to AIPS++ image
      
      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool toImage ();

#ifdef HAVE_CFITSIO
    /*!
      \brief Export the dynamic spectrum to FITS

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool toFITS ();
#endif
    
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
  
}  // Namespace CR -- end

#endif /* _DYNAMICSPECTRUM_H_ */
