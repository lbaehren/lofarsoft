/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1964 2008-09-06 17:52:38Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef SKYMAPCOORDINATE_H
#define SKYMAPCOORDINATE_H

// Standard library header files
#include <iostream>
#include <string>

// CR-Tools header files
#include <Coordinates/SkymapQuantity.h>
#include <Coordinates/SpatialCoordinate.h>
#include <Coordinates/TimeFreqCoordinate.h>
#include <Observation/ObservationData.h>

// Namespace usage
using CR::SpatialCoordinate;
using CR::TimeFreqCoordinate;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SkymapCoordinate
    
    \ingroup CR_Coordinates
    
    \brief Container for the coordinates associated with a sky map
    
    \author Lars B&auml;hren

    \date 2009/01/06

    \test tSkymapCoordinate.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::SkymapQuantity -- Quantity for which to compute the skymap
      <li>CR::SpatialCoordinate -- Container for the spatial coordinates
      <li>CR::TimeFreqCoordinate -- Container for the time-frequency axes
    </ul>
    
    <h3>Synopsis</h3>

    This class mainly operates as a container for the coordinate objects handling
    the spatial and temporal-spectral characteristics of a sky map.
    
    <h3>Example(s)</h3>

    <ol>
      <li>Construct new object based on custom parameters:
      \code
      std::string telescope  = "LOFAR";
      std::string observer   = "Lars Baehren";
      std::string refcode    = "AZEL";
      std::string projection = "SIN";
      uint blocksize         = 1024;
      casa::Quantity sampleFreq (200,"MHz");
      uint nyquistZone (1);
      uint blocksPerFrame (1);
      uint nofFrames (10);

      ObservationData obsData (telescope,
                               observer);
      TimeFreqCoordinate timeFreq (blocksize,
	 			   sampleFreq,
		 		   nyquistZone,
			 	   blocksPerFrame,
				   nofFrames);
      
      SkymapCoordinate coord (obsData,
                              timeFreq);
      \endcode
      <li>Retrieve the values along the frequency and/or the time axis:
      \code
      Vector<double> times = coord.timeFreqCoordinate().timeValues();
      Vector<double> freq  = coord.timeFreqCoordinate().frequencyValues();
      \endcode
      Retrieve the values along the spatial axes:
      \code
      Matrix<double> positions = coord.spatialCoordinate().positionValues();
      \endcode
    </ol>
    
  */
  class SkymapCoordinate {
    
    //! Observation data (epoch, location, etc.)
    ObservationData obsData_p;
    //! Container for the spatial coordinates
    CR::SpatialCoordinate spatialCoord_p;
    //! Container for the time-frequency axes
    CR::TimeFreqCoordinate timeFreqCoord_p;
    //! Quantity for which to compute the skymap
    SkymapQuantity skymapQuantity_p;
    //! Coordinate system object to be attached to an image
    casa::CoordinateSystem csys_p;
    //! The number of coordinate axes
    unsigned int nofAxes_p;
    //! Number of elements along the coordinate axes
    casa::IPosition shape_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    SkymapCoordinate ();
    
    /*!
      \brief Argumented constructor

      \param obsData       -- Observation data (epoch, location, etc.)
      \param timeFreqCoord -- Container for the coupled time and frequency axes
    */
    SkymapCoordinate (ObservationData const &obsData,
		      TimeFreqCoordinate const &timeFreqCoord,
		      SkymapQuantity::Type const &quantity=SkymapQuantity::FREQ_POWER);

    /*!
      \brief Argumented constructor

      \param obsData       -- Observation data (epoch, location, etc.)
      \param spatialCoord  -- 
      \param timeFreqCoord -- Container for the coupled time and frequency axes
    */
    SkymapCoordinate (ObservationData const &obsData,
		      SpatialCoordinate const &spatialCoord,
		      TimeFreqCoordinate const &timeFreqCoord,
		      SkymapQuantity::Type const &quantity=SkymapQuantity::FREQ_POWER);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SkymapCoordinate object from which to create this new
             one.
    */
    SkymapCoordinate (SkymapCoordinate const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~SkymapCoordinate ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SkymapCoordinate object from which to make a copy.
    */
    SkymapCoordinate& operator= (SkymapCoordinate const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the quantity for which to compute the skymap

      \return skymapQuantity -- Quantity for which to compute the skymap
    */
    inline SkymapQuantity skymapQuantity () const {
      return skymapQuantity_p;
    }

    /*!
      \brief Set the quantity for which to compute the skymap

      \param skymapQuantity -- Quantity for which to compute the skymap
      \param adjustSettings -- Try to adjust internal settings in case internal 
             parameters do not agree with the new skymap quantity?

      \return status -- Status of the operation; returns \e false in case an error
              was encountered.
    */
    bool setSkymapQuantity (SkymapQuantity::Type const &type,
			    bool const &adjustSettings=true)
    {
      return setSkymapQuantity (SkymapQuantity(type),
				adjustSettings);
    }

    /*!
      \brief Set the quantity for which to compute the skymap

      \param skymapQuantity -- Quantity for which to compute the skymap
      \param adjustSettings -- Try to adjust internal settings in case internal 
             parameters do not agree with the new skymap quantity?

      \return status -- Status of the operation; returns \e false in case an error
              was encountered.
    */
    bool setSkymapQuantity (SkymapQuantity const &skymapQuantity,
			    bool const &adjustSettings=true);

    /*!
      \brief Get the number of coordinate axes
      
      \return nofAxes -- The number of coordinate axes.
    */
    inline unsigned int nofAxes () const {
      return nofAxes_p;
    }
    
    /*!
      \brief Get the number of casa::Coordinate object to make of this coordinate
      
      \return nofCoordinates -- The number of casa::Coordinate object to make of
             this coordinate
    */
    inline unsigned int nofCoordinates () const {
      return (spatialCoord_p.nofCoordinates()+timeFreqCoord_p.nofCoordinates());
    }

    /*!
      \brief Get the shape, i.e. the number of elements along each axis
      
      \return shape -- Vector with the number of elements per coordinate axis.
    */
    inline casa::IPosition shape () const {
      return shape_p;
    }

    /*!
      \brief Get some basic information on the observation (epoch, location, etc.)
      
      \return obsData -- Observation data (epoch, location, etc.)
    */
    inline ObservationData observationData () const {
      return obsData_p;
    }

    /*!
      \brief Set some basic information on the observation (epoch, location, etc.)
      
      \param obsData -- Observation data (epoch, location, etc.)

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setObservationData (ObservationData const &obsData);

    /*!
      \brief Get the object encapsulating the spatial coordinates
    */
    inline SpatialCoordinate spatialCoordinate () const {
      return spatialCoord_p;
    }
    
    /*!
      \brief Set the object encapsulating the spatial coordinates

      \param coord -- 

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setSpatialCoordinate (SpatialCoordinate const &coord);

    /*!
      \brief Get the object encapsulating the temporal-spectral coordinates
    */
    inline TimeFreqCoordinate timeFreqCoordinate () const {
      return timeFreqCoord_p;
    }
    
    /*!
      \brief Set the object encapsulating the temporal-spectral coordinates

      \param coord -- 

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setTimeFreqCoordinate (TimeFreqCoordinate const &coord);

    /*!
      \brief Get the coordinate system from the coordinates

      \return csys -- Coordinate system object encapsulating the coordinates for
              the sky map.
    */
    inline casa::CoordinateSystem coordinateSystem () const {
      return csys_p;
    }

    /*!
      \brief Get the names of the world axes

      \return names -- The names of the world axes.
    */
    inline Vector<String> worldAxisNames() const {
      return csys_p.worldAxisNames();
    }

    /*!
      \brief Get the units of the world axes

      \return units -- The units of the world axes.
    */
    inline Vector<String> worldAxisUnits() const {
      return csys_p.worldAxisUnits();
    }

    /*!
      \brief Get the value of the reference pixel

      \return refPixel -- The value of the reference pixel.
    */
    inline Vector<double> referencePixel() const {
      return csys_p.referencePixel();
    }

    /*!
      \brief Get the value of the coordinate increment

      \return increment -- The value of the coordinate increment.
    */
    inline Vector<double> increment() const {
      return csys_p.increment();
    }

    /*!
      \brief Get the matrix for the linear transformation
      
      \return Xform -- The matrix of the linear transformation, as retrieved
              through the <tt>casa::Coordinate::linearTransform()<tt> function.
     */
    inline Matrix<double> linearTransform() const {
      return csys_p.linearTransform();
    }

    /*!
      \brief Get the value of the reference value

      \return refValue -- The value of the reference value.
    */
    inline Vector<double> referenceValue() const {
      return csys_p.referenceValue();
    }
    
    /*!
      \brief Conversion from pixel to world coordinates
      
      \retval world -- Values in world coordinates
      \param pixel  -- Values in pixel coordinates
    */
    inline void toWorld (Vector<double> &world,
			 Vector<double> const &pixel)
      {
	csys_p.toWorld (world,pixel);
      }
    
    /*!
      \brief Conversion from world to pixel coordinates

      \retval pixel -- Values in pixel coordinates
      \param world  -- Values in world coordinates
    */
    inline void toPixel (Vector<double> &pixel,
			 const Vector<double> &world)
      {
	csys_p.toPixel (pixel,world);
      }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SkymapCoordinate.
    */
    inline std::string className () const {
      return "SkymapCoordinate";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Retrieve the world axis values of the coordinates

      \b Warning: The array of values returned by this method can grow quite big,
      thereby there is a considerable chance that memory allocation limits may
      get exceeded.
      
      \return values -- The world axis values of the coordinates
    */
    casa::Matrix<double> worldAxisValues (bool const &fastedAxisFirst=true);

  private:
    
    //! Unconditional copying
    void copy (SkymapCoordinate const &other);
    
    //! Unconditional deletion 
    void destroy(void);

    //! Initialize the object's internal parameters
    void init ();

    //! Initialize the object's internal parameters
    void init (ObservationData const &obsData,
	       SpatialCoordinate const &spatialCoord,
	       TimeFreqCoordinate const &timeFreqCoord,
	       SkymapQuantity::Type const &quantity=SkymapQuantity::FREQ_POWER);

    //! Set up the coordinate system object to be attached to an image
    void setCoordinateSystem ();

    //! Set the shape of the coordinate axes
    void setShape();

  };
  
} // Namespace CR -- end

#endif /* SKYMAPCOORDINATE_H */
  
