/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/* $Id: SkymapCoordinates.h,v 1.13 2007/04/19 14:22:46 horneff Exp $*/

#ifndef SKYMAPCOORDINATES_H
#define SKYMAPCOORDINATES_H

// Standard library header files
#include <string>

// CASA header files
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <measures/Measures/MDirection.h>

// Custom header files
#include <lopes/Coordinates/TimeFreq.h>
#include <lopes/IO/DataReader.h>
#include <lopes/Observation/ObservationData.h>

using casa::CoordinateSystem;
using casa::DirectionCoordinate;
using casa::Double;
using casa::IPosition;
using casa::LinearCoordinate;
using casa::Matrix;
using casa::SpectralCoordinate;
using casa::Vector;

using LOPES::TimeFreq;

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \class SkymapCoordinates
    
    \ingroup Skymap
    
    \brief Handling of the various coordinates involved in the creation of a skymap
    
    \author Lars B&auml;hren
    
    \date 2007/03/13

    \test tSkymapCoordinates.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>TimeFreq -- Basic parameters in the time-frequency domain.
      <li>DataReader
      <li>ObservationData
      <li><a href="http://casa.nrao.edu/docs/doxygen/group__Coordinates.html">CASA
          coordinates module</a>
    </ul>
    
    <h3>Synopsis</h3>

    This class encapsulates the functionality required to convert all the
    Skymapper control parameters -- such as the number of samples per block
    (\f$ N_{\rm Blocksize} \f$), the spherical map projection (e.g. STG, TAN,
    etc.) -- to a coordinate system, which (a) can be used as input to the
    Beamformer and (b) will be attached to the created image.

    <ol>
      <li><b>Image shape.</b><br>
      The shape of the image data array
      \verbatim
      pixels = pixels [lon,lat,dist,time,freq]
      \endverbatim
      is defined through the following numbers:
      <table border="0">
        <tr valign="top">
	  <td>\f$ (N_\theta, N_\varphi) \f$</td> 
	  <td>Number of pixels in the image plane, i.e. the shape of the region
	  covered on the sky.</td>
	</tr>
	<tr valign="top">
	  <td>\f$ N_{\rm Dist} \f$</td> 
	  <td>Number of steps along the distance axis; this is 1 for standard
	  astronomical far-field beamforming, &gt; 1 for stepped focusing
	  on a sphere of finite distance to the antenna array.</td>
	</tr>
	<tr valign="top">
	  <td>\f$ N_{\rm Time} \f$</td> 
	  <td>Number of time steps; the size of this axis is derived from a number
	  control parameters (such as <tt>blocksize</tt>,
	  <tt>blocksPerIntegration</tt>) -- further details are given below.</td>
	</tr>
	<tr valign="top">
	  <td>\f$ N_{\rm Freq} \f$</td> 
	  <td>Number of frequency channels.</td>
	</tr>
      </table>
      <li><b>WCS parameters for direction coordinates.</b><br>
      A number of the input parameters need to be converted to the corresponding 
      Quanta or Measures objects; unfortunately not all the CASA classes support a 
      straight forward construction/configuration from the strings or floating point
      number, such that a few intermediate steps are required.
      <ol>
        <li>SkymapCoordinates::ProjectionType
	<li>SkymapCoordinates::MDirectionType
      </ol>
      [description of f.o.v. parameters missing]
      <li><b>WCS parameters for Time and Frequency axis.</b><br>
      The SkymapCoordinates::MapQuantity not only is responsible for setting up
      the Beamformer, but also provides information required for the proper
      setup of the Time and Frequency axis of coordinate system attached to the
      image. The basic information stored in the TimeFreq object, combined with
      the number of processed data blocks (\f$N_{\rm Blocks}\f$) is used to 
      construct WCS parameters required for the construction of the coordinate
      axes afterwards inserted into the coordinate system tool.
      <table>
        <tr>
	  <td class="indexkey">Domain</td>
	  <td class="indexkey">cdelt[3] = \f$ \delta t  \f$</td>
	  <td class="indexkey">shape[3] = \f$ N_{t}     \f$</td>
	  <td class="indexkey">cdelt[4] = \f$ \delta\nu \f$</td>
	  <td class="indexkey">shape[4] = \f$ N_{\nu}   \f$</td>
	</tr>
        <tr>
	  <td class="indexkey">Time</td>
	  <td>\f$ \delta_t = 1/\nu_{\rm Sample} \f$</td>
	  <td>\f$ N_t = N_{\rm Blocksize} \cdot N_{\rm Blocks} \f$</td>
	  <td>\f$ \delta_\nu = \frac{1}{2} \nu_{\rm Sample} \f$</td>
	  <td>\f$ N_\nu = 1 \f$</td>
	</tr>
        <tr>
	  <td class="indexkey">Frequency</td>
	  <td>\f$ \delta_t = \frac{N_{\rm Blocksize}}{\nu_{\rm Sample}} \f$</td>
	  <td>\f$ N_t = N_{\rm Blocks} \f$</td>
	  <td>\f$ \delta_\nu = \frac{\nu_{\rm Sample}}{N_{\rm Blocksize}} \f$</td>
	  <td>\f$ N_\nu = \frac{N_{\rm Blocksize}}{2} + 1 \f$</td>
	</tr>
      </table>
    </ol>
    
    <h3>Example(s)</h3>

    <ol>
      <li>Default constructor:
      \code
      SkymapCoordinates coord;  // construct new object
      coord.summary();          // display object properties
      \endcode
      The default constructor configure the internal parameters as required for
      an 120x120 pixel -- at 2deg resolution -- all-sky image in AZEL coordinates
      using a STG projection; this setup corresponds to the internal settings 
      when using the default constructor for a Skymapper object.
      <li>
      Argumented constructor:
      \code
      TimeFreq timeFreq;
      timeFreq.setBlocksize (4096);
      timeFreq.setSampleFrequency (80e06);
      timeFreq.setNyquistZone (2);
      
      ObservationData obsData
      obsData.setTelescope ("LOFAR");
      obsData.setObserver ("Lars Baehren");
      
      uint nofBlocks (10);

      SkymapCoordinates coord (timeFreq,
                               obsData,
			       nofBlocks);
      \endcode
    </ol>
    
  */  
  class SkymapCoordinates {

  public:

    // ==========================================================================
    // Enums for the various map properties

    /*!
      \brief Enum for the different coordinates in the created image

      Keep in mind, that the number of coordinates is different from the number 
      of coordinate axis, as a DirectionCoordinate can include several coordinate
      axes.
    */
    typedef enum {
      //! Direction towards a point on the celestial sphere
      Direction,
      //! Distance towards a point on the sphere (i.e. radius of the sphere), [m]
      Distance,
      //! Time, [s]
      Time,
      //! Frequency coordinate, [MHz]
      Frequency
    } MapAxes;
    
    /*!
      \brief Orientation of the generated sky map
      
      The various sky map come with "natural orientation", which is given by the
      type of spherical map projection.
    */
    typedef enum {
      //! North is to the top & East is to the left
      NORTH_EAST,
      //! North is to the top & West is to the left
      NORTH_WEST,
      //! South is to the top & East is to the left
      SOUTH_EAST,
      //! South is to the top & West is to the left
      SOUTH_WEST
    } MapOrientation;
    
    /*!
      \brief Electrical quantities and/or corresponding beam types

      Depending on the major domain, for which the skymap is created -- Time or
      Frequency -- the WCS parameters for the time and frequency axes need to be
      modified.
    */
    typedef enum {
      //! Electric field strength as function of frequency
      FREQ_FIELD,
      //! Power of the electric field as function of frequency
      FREQ_POWER,
      //! Electric field strength as function of time (sample resolution)
      TIME_FIELD,
      //! Power of the electric field as function of time
      TIME_POWER,
      //! Cross-correlation beam (cc-beam)
      TIME_CC,
      //! Excess-beam
      TIME_X
    } MapQuantity ;
    
    // ==========================================================================
    // Private data members

  private:

    //! Number of data blocks included into the map
    uint nofBlocks_p;

    // Basic container for the time-frequency domain settings
    TimeFreq timeFreq_p;
    
    //! Observation data (epoch, location, etc.)
    ObservationData obsData_p;
    
    //! Orientation of the generated sky map
    MapOrientation mapOrientation_p;

    //! Electrical quantities and/or corresponding beam types
    MapQuantity mapQuantity_p;
    
    //! Shape of the image
    IPosition shape_p;

    //! Coordinate system attached to the image
    CoordinateSystem csys_p;
    
  public:
    
    // ==========================================================================
    //                                                               Construction
    
    /*!
      \brief Default constructor
    */
    SkymapCoordinates ();
    
    /*!
      \brief Argumented constructor
      
      \param timeFreq  -- Time-frequency domain settings (blocksize, sample
                          frequency, Nyquist zone).
      \param obsData   -- Observation data (epoch, location, etc.)
      \param nofBlocks -- The number of subsequent data blocks to be processed
    */
    SkymapCoordinates (TimeFreq const &timeFreq,
		       ObservationData const &obsData,
		       uint const &nofBlocks=1);
    
    /*!
      \brief Argumented constructor
      
      \param timeFreq       -- Time-frequency domain settings (blocksize, sample
                               frequency, Nyquist zone).
      \param obsData        -- Observation data (epoch, location, etc.)
      \param mapOrientation -- Orientation of the generated sky map
      \param mapQuantity    -- Electrical quantities and/or corresponding beam
                               types
    */
    SkymapCoordinates (TimeFreq const &timeFreq,
		       ObservationData const &obsData,
		       SkymapCoordinates::MapOrientation mapOrientation,
		       SkymapCoordinates::MapQuantity mapQuantity);
    /*!
      \brief Argumented constructor
      
      \param timeFreq       -- Time-frequency domain settings (blocksize, sample
                               frequency, Nyquist zone).
      \param obsData        -- Observation data (epoch, location, etc.)
      \param nofBlocks      -- The number of subsequent data blocks to be
                               processed
      \param mapOrientation -- Orientation of the generated sky map
      \param mapQuantity    -- Electrical quantities and/or corresponding beam
                               types
    */
    SkymapCoordinates (TimeFreq const &timeFreq,
		       ObservationData const &obsData,
		       uint const &nofBlocks,
		       SkymapCoordinates::MapOrientation mapOrientation,
		       SkymapCoordinates::MapQuantity mapQuantity);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SkymapCoordinates object from which to create this
                      new one.
    */
    SkymapCoordinates (SkymapCoordinates const &other);
    
    // ==========================================================================
    //                                                                Destruction

    /*!
      \brief Destructor
    */
    ~SkymapCoordinates ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SkymapCoordinates object from which to make a copy.
    */
    SkymapCoordinates& operator= (SkymapCoordinates const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the orientation of the generated sky map

      \return mapOrientation -- The orientation of the generated sky map
    */
    inline SkymapCoordinates::MapOrientation mapOrientation () {
      return mapOrientation_p;
    }
    
    /*!
      \brief Set the orientation of the generated sky map

      \param mapOrientation -- The orientation of the generated sky map

      \return status -- Status of the operation.
    */
    bool setMapOrientation (SkymapCoordinates::MapOrientation const &mapOrientation);
    
    /*!
      \brief Get the electrical quantity and corresponding beam type

      \return mapQuantity -- The electrical quantity and corresponding beam type
    */
    inline SkymapCoordinates::MapQuantity mapQuantity () {
      return mapQuantity_p;
    }
    
    /*!
      \brief Get the electrical quantity and corresponding beam type

      \retval domain   -- Domain in which the quantity is defined, <tt>TIME</tt>
                          or <tt>FREQ</tt>
      \retval quantity -- Electrical quantity (and the corresponding beam type)

      \return status -- Status of the operation.
    */
    bool mapQuantity (std::string &domain,
		      std::string &quantity);

    /*!
      \brief Set the electrical quantity and corresponding beam type

      \param mapQuantity -- The electrical quantity and corresponding beam type

      \return status -- Status of the operation.
    */
    bool setMapQuantity (SkymapCoordinates::MapQuantity const &mapQuantity);

    /*!
      \brief Set the electrical quantity and corresponding beam type

      \param domain   -- Domain in which the quantity is defined, <tt>TIME</tt>
                         or <tt>FREQ</tt>
      \param quantity -- Electrical quantity (and the corresponding beam type)

      \return status -- Status of the operation.
    */
    bool setMapQuantity (std::string const &domain,
			 std::string const &quantity);

    // ==========================================================================
    // Coordinates

    /*!
      \brief Get the coordinate system tool build up from the coordinates

      \return csys -- CASA CoordinateSystem object, encapsulating information on
                      observation epoch and location, as well as containing the 
		      coordinate axes associated with an image.
     */
    inline CoordinateSystem coordinateSystem () {
      return csys_p;
    }

    /*!
      \brief Get the pixel array shape of the created image
      
      \return shape -- Shape of the image's pixel array
    */
    inline IPosition shape () {
      return shape_p;
    }

    // ----------------------------------------------------------- Direction axis

    /*!
      \brief Get the coordinate handling the direction axes

      \return directionAxis -- The coordinate handling the direction axis
    */
    inline DirectionCoordinate directionAxis () {
      return csys_p.directionCoordinate(SkymapCoordinates::Direction);
    }
    
    /*!
      \brief Set the shape of the image's direction plain

      \param lonPixels -- Number of pixels along the longitudinal axis of the
                          direction plain
      \param latPixels -- Number of pixels along the latitudinal axis of the
                          direction plain

      \return status -- Status of the operation;p returns <i>false</i> if an 
                        error was encountered
    */
    inline bool setDirectionShape (uint const &lonPixels,
				   uint const &latPixels) {
      shape_p(0) = lonPixels;
      shape_p(1) = latPixels;
      return true;
    }
    
    // ------------------------------------------------------------ Distance axis

    /*!
      \brief Get the coordinate handling the distance axes

      \return distanceAxis -- The coordinate handling the distance axis
    */
    inline LinearCoordinate distanceAxis () {
      return csys_p.linearCoordinate(SkymapCoordinates::Distance);
    }
    
    /*!
      \brief Set the distance axis coordinate
      
      \param refPixel  -- Reference pixel
      \param refValue  -- Reference value, i.e. the world coordinate associated
                          with the reference pixel
      \param increment -- Coordinate increment along the axis
    */
    bool setDistanceAxis (Double const &refPixel,
			  Double const &refValue,
			  Double const &increment);
    
    /*!
      \brief Set the number of steps taken along the distance axis

      \param lonPixels -- Number of steps along the distance axis

      \return status -- Status of the operation;p returns <i>false</i> if an 
                        error was encountered
    */
    inline bool setDistanceShape (uint const &nofSteps) {
      shape_p(2) = nofSteps;
      return true;
    }
    
    /*!
      \brief Get the coordinate values along the distance axis

      \return distanceValues -- The coordinate values along the distance axis
    */
    Vector<Double> distanceAxisValues ();

    /*!
      \brief Get the coordinate values along the distance axis

      \param pixelValues -- Pixel coordinates for which to return the distance
                            values

      \return distanceValues -- The coordinate values along the distance axis
    */
    Vector<Double> distanceAxisValues (Vector<Double> const &pixelValues);

    // ---------------------------------------------------------------- Time axis

    /*!
      \brief Get the coordinate handling the time axes

      \return timeAxis -- The coordinate handling the time axis
    */
    inline LinearCoordinate timeAxis () {
      return csys_p.linearCoordinate(SkymapCoordinates::Time);
    }
    
    /*!
      \brief Get the coordinate values along the time axis

      \param pixelValues -- Pixel coordinates for which to return the time
                            values

      \return timeValues -- The coordinate values along the time axis
    */
    Vector<Double> timeAxisValues (Vector<Double> const &pixelValues);

    // ----------------------------------------------------------- Frequency axis

    /*!
      \brief Get the coordinate handling the frequency axes

      \return frequencyAxis -- The coordinate handling the frequency axis
    */
    inline SpectralCoordinate frequencyAxis () {
      return csys_p.spectralCoordinate(SkymapCoordinates::Frequency);
    }

    /*!
      \brief Get the coordinate values along the frequency axis

      \param pixelValues -- Pixel coordinates for which to return the frequency
                            values

      \return frequencyValues -- The coordinate values along the frequency axis
    */
    Vector<Double> frequencyAxisValues (Vector<Double> const &pixelValues);

    // ==========================================================================
    // Feedback
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SkymapCoordinates.
    */
    std::string className () const {
      return "SkymapCoordinates";
    }

    /*!
      \brief Provide a summary of the internal status

      Output will be written to <tt>cout</tt>.
      \verbatim
      -- TimeFreq object:
       Blocksize      [samples] = 1
       Sample frequency    [Hz] = 8e+07
       Nyquist zone             = 1
       Reference time     [sec] = 0
       FFT length    [channels] = 1
       Sample interval      [s] = 1.25e-08
       Frequency increment [Hz] = 8e+07
       Frequency band      [Hz] = 0 .. 4e+07
      -- ObservationData object:
       Description              = NONE
       Observer                 = UNKNOWN
       Epoch                    = Epoch: 54173::19:47:27.8420
       Observatory              = WSRT
       Observatory position     = Position: [2.99486, 0.346737, 3.98881]
       nof. antennas            = 1
       antenna positions        = [3, 1]
      -- Image properties:
       nof. processed blocks    = 1
       Skymap orientation       = 0
       Skymap quantity          = 1 [FREQ,POWER]
       Number of coordinates    = 4
       Shape of the pixel array = [120, 120, 1, 1, 1]
       World axis names         = [Longitude, Latitude, Distance, Time, Frequency]
       World axis units         = [rad, rad, m, s, Hz]
       Reference pixel  (CRPIX) = [60, 60, 0, 0, 1]
       Reference value  (CRVAL) = [0, 1.5708, -1, 0, 0]
       Increment        (CDELT) = [-0.0349066, 0.0349066, 0, 2.5e-08, 39062.5]
      \endverbatim
    */
    void summary () {
      summary (std::cout);
    }
    
    /*!
      \brief Provide a summary of the internal status
      
      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);
    
    // --------------------------------------------------------- Time & Frequency

    /*!
      \brief Get the container for the time-frequency domain settings

      \return timeFreq -- Basic container for the time-frequency domain settings
    */
    inline TimeFreq timeFreq () {
      return timeFreq_p;
    }

    /*!
      \brief Set the parameters defining the time and frequency axis

      \param timeFreq --

      \return status -- Status of the operation
    */
    bool setTimeFreq (TimeFreq const &timeFreq);
    
    /*!
      \brief Set the parameters defining the time and frequency axis

      \param dr -- A DataReader object, which is derived from TimeFreq.

      \return status -- Status of the operation
    */
    bool setTimeFreq (DataReader const &dr);
    
    /*!
      \brief Set the parameters defining the time and frequency axis

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
      \param referenceTime   -- Reference time, \f$ t_0 \f$

      \return status -- Status of the operation
    */
    bool setTimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone,
		      double const &referenceTime);

    /*!
      \brief Get the number of data blocks included into the map

      \return nofBlocks -- Number of data blocks included into the map
    */
    inline uint nofBlocks () {
      return nofBlocks_p;
    }
    
    /*!
      \brief Get the number of data blocks included into the map

      \param nofBlocks -- Number of data blocks included into the map

      \return status -- Status of the operation, returns <tt>false</tt> if an
                        error was encountered
    */
    bool setNofBlocks (uint const &nofBlocks);

    // ---------------------------------------------------- Direction coordinates

    /*!
      \brief Get some basic information on the observation (epoch, location, etc.)
      
      \return observationData -- Observation data (epoch, location, etc.)
    */
    inline ObservationData observationData () {
      return obsData_p;
    }

    /*!
      \brief Set the parameters on the observation (epoch, location, etc.)

      \param ObservationData --

      \return status -- Status of the operation
    */
    bool setObservationData (ObservationData const &obsData);

    /*!
      \brief Get a direction coordinate based on its basic parameters
      
      \param refcode    -- Reference code for the celestial coordinate frame
      \param projection -- Reference code for the sphercial map projection
      \param refValue   -- Reference value, CRVAL
      \param increment  -- Coordinate increment, CDELT
      \param pixels     -- Number of pixels in the celestial plane
    */
    DirectionCoordinate directionCoordinate (const String& refcode,
					     const String& projection,
					     const Vector<Double>& refValue,
					     const Vector<Double>& increment,
					     const IPosition& pixels);
    
    // ------------------------------------------------------ Conversion routines

    /*!
      \brief Convert direction reference code to MDirection::Types

      \param refcode -- Reference code

      \return type -- MDirection::Types required to construct MDirection object
     */
    static MDirection::Types MDirectionType (std::string const &refcode);

    /*!
      \brief Convert the projection reference code to Projection::Type

      \param refcode -- Reference code for the projection, e.g. TAN, STG, etc.

      \return type -- 
    */
    static Projection::Type ProjectionType (std::string const &refcode);

  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (SkymapCoordinates const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize the internal parameters

      \param timeFreq  -- Time-frequency domain settings (blocksize, sample
                          frequency, Nyquist zone).
      \param obsData   -- Observation data (epoch, location, etc.)
      \param nofBlocks -- The number of subsequent data blocks to be processed
    */
    bool init (TimeFreq const &timeFreq,
	       ObservationData const &obsData,
	       uint const &nofBlocks);
  
    /*!
      \brief Initialize the internal parameters

      \param timeFreq  -- Time-frequency domain settings (blocksize, sample
                          frequency, Nyquist zone).
      \param obsData   -- Observation data (epoch, location, etc.)
      \param nofBlocks -- The number of subsequent data blocks to be processed
    */
    bool init (TimeFreq const &timeFreq,
	       ObservationData const &obsData,
	       uint const &nofBlocks,
	       SkymapCoordinates::MapOrientation mapOrientation,
	       SkymapCoordinates::MapQuantity mapQuantity);
    
    /*!
      \brief Default shape of the image pixel array
      
      \return shape -- Shape of the image pixel array
    */
    bool setShape ();
    
    // ---------------------------------------------------------- Coordinate axes

    /*!
      \brief Construct the default coordinate system
      
      \return status -- Status of the operation; returns <i>false</i> if an error
                        was encountered.
    */
    bool defaultCoordinateSystem ();
    /*!
      \brief Default coordinate for the direction axes
    */
    DirectionCoordinate defaultDirectionAxis ();
    /*!
      \brief Default coordinate for the distance axis
    */
    LinearCoordinate defaultDistanceAxis ();
    /*!
      \brief Default coordinate for the frequency axis
    */
    SpectralCoordinate defaultFrequencyAxis ();
    /*!
      \brief Default coordinate for the time axis
    */
    LinearCoordinate defaultTimeAxis ();
    /*!
      \brief Get the default coordinate for the time axis
      
      \return status -- Status of the operation -- returns <tt>false</tt> if an
                        error was encountered.
    */
    bool setTimeAxis ();
    
  };
  
} // Namespace LOPES -- end

#endif /* SKYMAPCOORDINATES_H */
  
