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

/* $Id: Skymapper.h,v 1.11 2006/11/11 17:44:25 bahren Exp $*/

#ifndef _SKYMAPPER_H_
#define _SKYMAPPER_H_

// C++ Standard library
#include <stdio.h>
#include <fstream>
using namespace std;

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>
#include <tasking/Glish.h>
#include <Glish/Client.h>

// LOPES-Tools header files
#include <lopes/IO/DataReaderTools.h>
#include <lopes/Observation/ObservationData.h>
#include <lopes/Skymap/Skymap.h>
#include <lopes/Skymap/SkymapQuantity.h>

// CASA namespace
#include <casa/namespace.h>

/*!
  \class Skymapper

  \ingroup Skymap

  \brief Brief description for class Skymapper

  \author Lars B&auml;hren

  \date 2006/01/23

  \test tSkymapper.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/images/implement/Images.html">Module Images</a>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/CoordinateSystem.html">CoordinateSystem</a>  -- Interconvert pixel and world coordinates.

    <li>[LOPES-Tools] Beamformer
    <li>[LOPES-Tools] DataReader
    <li>[LOPES-Tools] DataReaderTools
    <li>[LOPES-Tools] ObservationData
    <li>[LOPES-Tools] Skymap
    <li>[LOPES-Tools] SkymapQuantity
    <li>[LOPES-Tools] SkymapperTools
  </ul>

  <h3>Synopsis</h3>

  By default -- i.e. with the default constructor -- we set up everything for
  a [120,120] pixel all-sky map in STG projection, centered on (0,90) in AZEL
  coordinates; the various ingredients for the created PagedImage are delivered
  by the functions:
  <ul type="square">
    <li>Skymapper::defaultImageShape
    <li>Skymapper::defaultCoordsys
  </ul>

  <h4>Parameters for the image shape</h4>

  The shape of the image data array is defined through the following numbers:
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

  <h4>Parameters for the coordinate system axes</h4>

  <ul type="square">
    <li>increment on the time axis:
    \f[ \delta t = (\nu_{\rm Sampling})^{-1} \qquad \hbox{domain = TIME} \f]
    \f[ \delta t = (\nu_{\rm Sampling})^{-1} \cdot N_{\rm Blocksize} \cdot
    N_{\rm Integration} \qquad \hbox{domain = FREQ} \f]
    <li>increment for the frequency axis
    \f[ \delta \nu = \nu_{\rm max} - \nu_{\rm min} \qquad \hbox{domain = TIME} \f]
    \f[ \delta \nu = \frac{\nu_{\rm High} - \nu_{\rm Low}}{N_{\rm Channels}-1} 
    \qquad \hbox{domain = FREQ} \f]
  </ul>

  <h3>Example(s)</h3>

  <ol>
    <li>The example below will create a sky map of shape [20,20,1,1,1] of 0.5 deg
    spatial resolution, centered on [20,20] in J200 coordinates. The data are read
    in using the DataReader, which in this case is set up from the contents of
    Glish record (typical scenario on case of a Glish client application).
    \code
    // Data input
    DataReader *dr = DataReaderTools::assignFromRecord ();
    
    // Observation info
    ObservationData obsData ("LOFAR-ITS");
    obsData.setObserver ("Lars Baehren");
    
    // Coordinate settings
    IPosition shape (5,20,20,1,1,1);
    String refcode ("J2000");
    String projection ("SIN");
    Vector<Double> crval (2);
    Vector<Double> cdelt (2);
    
    crval(0) = 178.0;
    crval(1) =  28.0;
    cdelt(0) =  -0.5;
    cdelt(1) =  +0.5;
  
    Skymapper skymapper (dr,
                         obsData,
			 shape,
			 refcode,
			 projection,
			 crval,
			 cdelt);
    \endcode
  </ol>

*/

class Skymapper {

 public:
  
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
  } orientation;
  
 private:
  
  //! Switch to enable/disable verbose mode
  Bool verbose_p;
  
  //! Track the status of the Skymapper
  Bool isOperational_p;

  //! Name of the image created on disk
  String filename_p;

  //! Shape of the skymap
  IPosition shape_p;

  //! Electrical quantity, for which the sky map is generated
  SkymapQuantity quantity_p;

  //! Coordinate system attached to the image
  CoordinateSystem csys_p;

  //! Number of blocks added per integration step
  uint blocksPerTimeframe_p;

  //! A DataReader object for reading in the data from disk
  DataReader *dr_p;

  //! Observation data (epoch, location, etc.)
  ObservationData obsData_p;

  //! Skymap handler
  Skymap skymap_p;

  //! Pointer to the function handling the retrival of new data
  void (Skymapper::*pFunction) (Matrix<DComplex> &data); 

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor

    \f$ 120 \times 120 \f$ pixel grid of \f$ ( 2^\circ, 2^\circ ) \f$ deg
    resolution (AZEL, STG) map centered on \f$ ( 0, 90 ) \f$.
  */
  Skymapper ();

  /*!
    \brief Simplest argumented constructor

    Create a simple skymap from the data the DataReader is connected to:
    \f$ 120 \times 120 \f$ pixel grid of \f$ ( 2^\circ, 2^\circ ) \f$ deg
    resolution (AZEL, STG) map centered on \f$ ( 0, 90 ) \f$.

    \param dr      -- DataReader object to handle the input of data from disk
    \param obsData -- ObservationData object, wrapping various data related to
                      the observation (epoch, telescope, observer, antenna
		      positions)
  */
  Skymapper (DataReader *dr,
	     ObservationData const &obsData);

  /*!
    \brief Argumented constructor

    \param dr         -- DataReader object to handle the input of data from disk
    \param obsData    -- ObservationData object, wrapping various data related to
                         the observation (epoch, telescope, observer, antenna
		         positions)
    \param shape      -- 
    \param refcode    -- Reference code identifying the coordinate reference
                         frame, e.g. <i>AZEL</i>
    \param projection -- Reference code identifying the spherical map projection,
                         e.g. <i>AIT</i>
    \param crval      -- Reference value at the position of the sky map
    \param cdelt      -- Coordinate increment
  */
  Skymapper (DataReader *dr,
	     ObservationData const &obsData,
	     const IPosition& shape,
	     const String& refcode,
	     const String& projection,
	     const Vector<Double>& crval,
	     const Vector<Double>& cdelt);

  /*!
    \brief Copy constructor

    \param other -- Another Skymapper object from which to create this new
                    one.
  */
  Skymapper (Skymapper const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~Skymapper ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another Skymapper object from which to make a copy.
  */
  Skymapper &operator= (Skymapper const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the name of (or path to) the created image file
    
    \return filename -- Name of (or path to) the created image file
  */
  String filename () const {
    return filename_p;
  }

  /*!
    \brief Set the name of (or path to) the created image file
    
    \param  filename -- Name of (or path to) the created image file
  */
  void setFilename (String const &filename) {
    filename_p = filename;
  }

  /*!
    \brief Set the DataReader used for reading in the data from disk

    \brief dr -- A DataReader object for reading in the data from disk

    \return status -- Status of the operation; returns <tt>False</tt> if an error
                      occured
   */
  Bool setDataReader (DataReader *dr);

  /*!
    \brief Set the DataReader used for reading in the data from disk

    \brief rec -- A GlishRecord, containing all the parameters to set up a 
                  working DataReader object

    \return status -- Status of the operation; returns <tt>False</tt> if an error
                      occured
   */
  Bool setDataReader (GlishRecord &rec);

  /*!
    \brief Get the data describing the observation

    \return obsData -- Information on the observation (epoch, telescope,
                       observer, telescope position, antenna positions) wrapped
		       in an ObservationData object.
  */
  ObservationData observationData () const {
    return obsData_p;
  }
  
  /*!
    \brief Set the data describing the observation

    \param obsData    -- Information on the observation (epoch, telescope,
                         observer, telescope position, antenna positions) wrapped
			 in an ObservationData object.
    \param updateCsys -- Update the CoordinateSystem object based on the data
                         enclosed in <tt>obsData</tt>
  */
  void setObservationData (ObservationData const &obsData,
			   Bool const &updateCsys=True);
  
  /*!
    \brief Shape of the image data array

    \return shape -- Shape of the image data array
   */
  IPosition shape () const {
    return shape_p;
  }

  /*!
    \brief Set the shape of the skymap

    \param shape -- The shape of the skymap image data array,
                    [az,el,distance,time,frequency]
    \param updateRefPixel -- Update the value of the reference pixel?

    \todo We might need to update the reference pixel for the direction 
    axis; this could be taken care of by an additional boolean parameter
    passed to the function.
  */
  void setImageShape (IPosition const &shape,
		      Bool const &updateRefPixel=False);

  /*!
    \brief Get the number of data blocks whihc are combined into a timeframe

    \return  blocksPerTimeframe -- The number of blocks, which
                                   combined/integrated for a single timeframe
				   in the final image
  */
  uint blocksPerTimeframe () const {
    return blocksPerTimeframe_p;
  }

  /*!
    \brief Set the number of data blocks whihc are combined into a timeframe

    \param blocksPerTimeframe -- The number of blocks, which
                                 combined/integrated for a single timeframe
				 in the final image
  */
  void setBlocksPerTimeframe (uint const &blocksPerTimeframe) {
    blocksPerTimeframe_p = blocksPerTimeframe;
  }

  /*!
    \brief Set the electrical quantity, for which the sky map is generated

    \brief quantity -- Electrical quantity, for which the sky map is generated
  */
  void setSkymapQuantity (SkymapQuantity::Type quantity);

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Set the source (function) for reading the data during the imaging
   */
  Bool setDataSource ();

  /*!
    \brief Process the data and create a skymap

    This is the central function responsible for the generation of the Skymap,
    to be exported as AIPS++ image tool.

    \return status -- Status of the operation; returns <i>False</i> if an error
                      was encountered.

    \todo Where do the antenna positions come from, which need to be fed to the 
          Beamformer? Are they buffered somewhere? The original implementation
	  (re-)extracted the information from a Glish record each time the imaging
	  function was called.
  */
  Bool createImage ();

  // ---------------------------------------------------------- Coordinate system

  /*!
    \brief Get the coordinate system associated with the image

    \return cs -- The coordinate system
   */
  CoordinateSystem coordinateSystem () const {
    return csys_p;
  }

  /*!
    \brief Set the coordinate system from a Glish record of another one
    
    \param csys -- Another CoordinateSystem object
  */
  void setCoordinateSystem (CoordinateSystem const &csys);

  /*!
    \brief Set the coordinate system from the coordinate axis parameters

    This is a wrapper for SkymapperTools::coordinateSystem, which is performing
    the actual creation of the CoordinateSystem object.
    
    \param obsinfo        -- 
    \param refcode        -- Reference code identifying the coordinate reference
                             frame, e.g. <i>AZEL</i>
    \param projection     -- Reference code identifying the spherical map
                             projection, e.g. <i>AIT</i>
    \param crpix          -- Reference pixel
    \param crval          -- Reference value
    \param cdelt          -- Coordinate increment
  */
  bool setCoordinateSystem (const ObsInfo &obsinfo,
			    const String &refcode,
			    const String &projection,
			    const Vector<Double> &crpix,
			    const Vector<Double> &crval,
			    const Vector<Double> &cdelt);
  
  /*!
    \brief Set the coordinate system from a Glish record of another one
    
    \param csys -- Another coordinate system object embedded in a GlishRecord
  */
  void setCoordinateSystem (GlishRecord const &csys);

  /*!
    \brief Set the direction axis of the coordinate system
 
    \param refcode    -- Reference code identifying the coordinate reference
                         frame, e.g. <i>AZEL</i>
    \param projection -- Reference code identifying the spherical map projection,
                         e.g. <i>AIT</i>
    \param crval      -- Reverence value at the position of the sky map
    \param cdelt      -- Coordinate increment
  */
  Bool setDirectionAxis (String const &refcode,
			 String const &projection,
			 const Vector<Double> &crval,
			 const Vector<Double>& cdelt);    

  /*!
    \brief Set the time axis of the coordinate system

    \param referenceValue     -- Reference value, i.e. the real world value 
                                 associated with the first timeframe; [sec]
    \param nofTimeframes      -- The number of time frames, i.e. the length of
                                 time axis of the created image; the value is 
				 stored in <tt>shape[3]</tt>
    \param blocksPerTimeframe -- The number of block added/averaged for a single
                                 time frame in the final image

    \return status -- Return status of the <tt>replaceCoordinate</tt> function

    This replace the current time axis of the coordinate system object, using
    the functionality of the <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/CoordinateSystem.html">CoordinateSystem</a> class:
    \code
    // Extraction of the time axis
    LinearCoordinate axis (csys_p.linearCoordinate(axisNumber));
    // storage of the modified time axis
    Bool replaceCoordinate(const Coordinate &newCoordinate, uInt whichCoordinate)
    \endcode
  */
  Bool setTimeAxis (const double &referenceValue=0.0,
		    const int &nofTimeframes=1,
		    const int &blocksPerTimeframe=1);

  // ------------------------------------------------------------------- feedback

  /*!
    \brief Provide a summary of the internal parameters to standard output
   */
  void summary ();

  /*!
    \brief Provide a summary of the internal parameters

    \param os -- Output stream, to which the summary is written

    The output typically will look something these lines:
    \verbatim
    [Skymapper] Summary of the internal parameters
    - Observation ........  
    -- telescope          = WSRT
    -- observer           = LOFAR/CR KSP
    -- observation date   = Epoch: 53955::17:14:33.4210
    - Data I/O ..........   
    -- blocksize          = 1
    -- sampling rate [Hz] = 8e+07
    -- Nyquist zone       = 1
    - Coordinates ........  
    -- nof. coordinates   = 4
    -- names              = [Longitude, Latitude, Distance, Time, Frequency]
    -- units              = [rad, rad, m, s, Hz]
    -- ref. pixel         = [0, 0, 0, 0, 0]
    -- ref. value         = [0, 1.5708, -1, 0, 0]
    -- coord. increment   = [-0.0349066, 0.0349066, 0, 1, 1]
    - Image ..............  
    -- filename           = skymap.img
    -- image shape        = [120, 120, 1, 1, 1]
    -- blocks per frame   = 1
    \endverbatim
   */
  void summary (std::ostream &os);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (Skymapper const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Initialization internal parameters
    
    \param dr      -- DataReader object to handle the input of data from disk
    \param obsData -- Collection of information on the observation
    \param shape   -- Shape of the created image
  */
  void init (DataReader *dr,
	     ObservationData const &obsData,
	     IPosition const &shape);

  /*!
    \brief Initialization internal parameters
   */
  void init (IPosition const &shape,
	     CoordinateSystem const &csys);

  /*!
    \brief Default function for retrival of data during the imging process

    Unless redirected to an external function, the Skymapper will be using the
    embedded DataReader object to retrieve the data itself.

    \return fft -- Spectral data, [freq,ant]
  */
  void getData (Matrix<DComplex> &data);

  /*!
    \brief Default direction coordinates axis
  */
  DirectionCoordinate defaultDirectionAxis ();

  /*!
    \brief Default settings for the distance axis

    If not specified otherwise, we assume generation of a "standard astrononomical
    sky map", i.e. beamforming in the far-field of the telescope.
   */
  LinearCoordinate defaultDistanceAxis ();

  /*!
    \brief Default settings for the time axis
  */
  LinearCoordinate defaultTimeAxis ();

  /*!
    \brief Get the time axis increment 

    Takes into account wether the calculated quantity displayed in the sky map is
    in the time of frequency domain:
    \f[
      \delta t = \left\{
        \begin{array}{lll}
	  T_{\rm Sample} & @ & domain = TIME \\
	  T_{\rm Sample} \cdot N_{\rm Blocksize} \cdot N_{\rm b.p.f.} & @ &
	    domain = FREQ
	\end{array}
      \right.
    \f]
    where \f$ N_{\rm b.p.f.} \f$ is the number of blocks added up per timeframe.
  */
  Double timeAxisIncrement ();
  
  /*!
    \brief Default frequency axis
  */
  SpectralCoordinate defaultFrequencyAxis ();

  /*!
    \brief Create default coordinate system object
    
    Default coordinate system object for an all-sky map in STG projection.
  */
  void setCoordinateSystem ();

  /*!
    \brief Synchronize the information kept in the obsData and csys objects

    \param csysIsMaster -- Is the CoordinateSystem object considered as the
                           master source of the information? Is <i>False</i>
			   then <tt>csys_p</tt> is updated based on the data
			   in <tt>obsData_p</tt>.
  */
  void syncCoordinateSystem (Bool const &csysIsMaster=False);
  
  /*!
    \brief Get the default shape of an image

    \return shape -- Default shape of an image created with the Skymapper
  */
  IPosition defaultShape () {
    return IPosition (5,120,120,1,1,1);
  }

  /*!
    \brief Set the coordinates of the center of the skymap
   */
  bool setSkymapCenter ();

};

#endif /* _SKYMAPPER_H_ */
