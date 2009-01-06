/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#ifndef CR_TIMEFREQSKYMAP_H
#define CR_TIMEFREQSKYMAP_H

// Standard library header files
#include <iostream>
#include <string>

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>

#include <crtools.h>
#include <Coordinates/TimeFreq.h>
#include <Coordinates/CoordinateType.h>

using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TimeFreqCoordinate
    
    \ingroup CR_Coordinates
    
    \brief Container for the time-frequency domain parameters of a skymap
    
    \author Lars B&auml;hren

    \date 2008/09/06

    \test tTimeFreqCoordinate.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::TimeFreq
      <li>CR::CoordinateDomain
      <li>CR::SkymapCoordinate
    </ul>
    
    <h3>Synopsis</h3>

    This class expands the book-keeping implemented in CR::TimeFreq to include
    the additional parameters relevant when creating a skymap. In addition to the
    basic parameters -- \b blocksize, \b sampleFrequency and \b nyquistZone -- 
    we need to add:

    <table border="0">
      <tr>
        <td class="indexkey">Variable</td>
	<td class="indexkey">Symbol</td>
	<td class="indexkey">Units</td>
	<td class="indexkey">Decription</td>
      </tr>
      <tr>
       <td>\b blocksPerFrame</td>
       <td>\f$ N_{\rm bpf} \f$</td>
       <td>---</td>
       <td>The number of input data blocks combined into a single time-frame of
       the resulting skymap.</td>
      </tr>
      <tr>
       <td>\b nofFrames</td>
       <td>\f$ N_{\rm Frames} \f$</td>
       <td>---</td>
       <td>The total number of time frames to generate for the resulting
       skymap.</td>
      </tr>
      <tr>
       <td>\b beamDomain</td>
       <td>---</td>
       <td>---</td>
       <td>The target domain for which the data are processed by the
       Beamformer</td>
      </tr>
    </table>

    Based on the above variables we get different values for some of the quantities
    already listed in CR::TimeFreq

    <table border="0">
      <tr>
        <td class="indexkey">Quantity</td>
	<td class="indexkey">Units</td>
	<td class="indexkey">Symbol</td>
	<td class="indexkey">Derivation from base variables</td>
      </tr>
      <tr>
       <td>Frequency increment</td>
       <td>Hz</td>
       <td>\f$ \delta\nu \f$</td>
       <td>\f$ \left\{ \begin{array}{lll}
       \nu_{\rm Sample}/2 & & \hbox{Domain=Time} \\[2mm]
       \nu_{\rm Sample} \cdot N_{\rm Blocksize}^{-1} & & \hbox{Domain=Freq}
       \end{array} \right. \f$</td>
      </tr>
      <tr>
       <td>Time increment</td>
       <td>s</td>
       <td>\f$ \delta t \f$</td>
       <td>\f$ \left\{ \begin{array}{lll} 
       \nu_{\rm Sample}^{-1} & & \hbox{Domain=Time} \\[2mm]
       (\nu_{\rm Sample})^{-1} N_{\rm Blocksize} N_{\rm bpf} N_{\rm Frames}
       & & \hbox{Domain=Freq}
       \end{array} \right. \f$</td>
      </tr>
      <tr>
       <td>Coordinate axes shape</td>
       <td>[Samples;Samples]</td>
       <td>\f$ [ N_t ; N_\nu ] \f$</td>
       <td>\f$ \left\{ \begin{array}{lll} 
       [ N_{\rm Blocksize} N_{\rm Frames} ; 1 ] & & \hbox{Domain=Time} \\[2mm]
       [ N_{\rm Frames} ; N_{\rm FFT} ] & & \hbox{Domain=Freq}
       \end{array} \right. \f$</td>
      </tr>
    </table>

    Given the fact that we are working with more but a single object of type
    casa::Coordinate we need to wrap the calls to function handling either
    allowing access to internal parameters or carrying out coordinate conversions:
    <ul>
      <li>TimeFreqCoordinate::worldAxisNames
      <li>TimeFreqCoordinate::worldAxisUnits
      <li>TimeFreqCoordinate::referencePixel
      <li>TimeFreqCoordinate::referenceValue
      <li>TimeFreqCoordinate::linearTransform
      <li>TimeFreqCoordinate::increment
      <li>TimeFreqCoordinate::toWorld
      <li>TimeFreqCoordinate::toPixel
    </ul>

    In order to keep the internal data of this object consistent, the following
    methods of the base class need to overloaded:
    - TimeFreq::setBlocksize
    - TimeFreq::setSampleFrequency
    - TimeFreq::setNyquistZone
    - TimeFreq::setReferenceTime
    
    <h3>Example(s)</h3>
    
    <ul>
      <li>Store data inside a new TimeFreqCoordinate object:
      \code
      uint blocksize (1024);
      casa::Quantity sampleFreq (200,"MHz");
      uint nyquistZone (1);
      uint blocksPerFrame (10);
      uint frames (100);
      
      TimeFreqCoordinate tf (blocksize,
                         sampleFreq,
		         nyquistZone,
			 blocksPerFrame,
			 frames);
      \endcode
      <li>Create new object, making use of already existing data stored in a
      TimeFreq object:
      \code
      TimeFreq tf;
      uint blocksPerFrame (10);
      uint frames (100);
      
      TimeFreqCoordinate tf_skymap (tf,
			        blocksPerFrame,
			        frames);
      \endcode
      <li>Get the internal object storing the coordinate domain information:
      \code 
      TimeFreqCoordinate tf;

      CoordinateDomain domain = tf.beamDomain();
      \endcode
      The actual coordinate domain \e type and \e name can be exctracted just as
      easily:
      \code
      CoordinateDomain::Types domainType = tf.beamDomain().type();
      std::string domainName             = tf.beamDomain().name();
      \endcode
    </ul>
    
  */  
  class TimeFreqCoordinate : public TimeFreq {

    //! The target domain for which the data are processed by the Beamformer
    CR::CoordinateType coordType_p;
    //! Linear coordinate for the time axis
    casa::LinearCoordinate coordTime_p;
    //! Spectral coordinate for the frequency axis
    casa::SpectralCoordinate coordFrequency_p;
    
  protected:

    //! The number of data blocks added up within a single time frame
    uint blocksPerFrame_p;
    //! The number of time frames, \f$ N_{\rm Frames} \f$
    uint nofFrames_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TimeFreqCoordinate ();
    
    /*!
      \brief Argumented constructor

      \param blocksPerFrame -- The number of data blocks added up within a single
             time frame
      \param nofFrames      -- The number of frames
    */
    TimeFreqCoordinate (uint const &blocksPerFrame,
			uint const &nofFrames);
    
    /*!
      \brief Argumented constructor

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC step, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
      \param blocksPerFrame  -- The number of data blocks added up within a single
             time frame
      \param nofFrames       -- The number of frames
    */
    TimeFreqCoordinate (uint const &blocksize,
			double const &sampleFrequency,
			uint const &nyquistZone=1,
			uint const &blocksPerFrame=1,
			uint const &nofFrames=1);
    
    /*!
      \brief Argumented constructor

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC step.
      \param nyquistZone     -- Nyquist zone,  [1]
      \param blocksPerFrame  -- The number of data blocks added up within a single
             time frame
      \param nofFrames       -- The number of frames
    */
    TimeFreqCoordinate (uint const &blocksize,
			casa::Quantity const &sampleFrequency,
			uint const &nyquistZone=1,
			uint const &blocksPerFrame=1,
			uint const &nofFrames=1);
    
    /*!
      \brief Argumented constructor

      \param timeFreq       -- CR::TimeFreq object acting as container for
             information based on blocksize, sample frequency and Nyquist zone.
      \param blocksPerFrame -- The number of data blocks added up within a single
             time frame
      \param nofFrames      -- The number of frames
    */
    TimeFreqCoordinate (TimeFreq const &timeFreq,
			uint const &blocksPerFrame=1,
			uint const &nofFrames=1);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TimeFreqCoordinate object from which to create this new
      one.
    */
    TimeFreqCoordinate (TimeFreqCoordinate const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TimeFreqCoordinate ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TimeFreqCoordinate object from which to make a copy.
    */
    TimeFreqCoordinate& operator= (TimeFreqCoordinate const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TimeFreqCoordinate.
    */
    inline std::string className () const {
      return "TimeFreqCoordinate";
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

    /*!
      \brief Get the target domain for the beamforming

      \return type -- The target domain for the beamforming.
    */
    inline CoordinateType beamCoordDomain () const {
      return coordType_p;
    }
    
    /*!
      \brief Get the target domain for the beamforming

      \param domain -- The target domain, as type, for the beamforming.

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setBeamCoordDomain (CR::CoordinateType::Types const &domain);
    
    /*!
      \brief Get the target domain for the beamforming

      \param domain -- The target domain, as name, for the beamforming.

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setBeamCoordDomain (CR::CoordinateType const &type);
    
    /*!
      \brief Get the number of blocks per time frame

      \return blocksPerFrame -- The number of input data blocks combined into a
              single time-frame of the resulting skymap.
    */
    inline uint blocksPerFrame () const {
      return blocksPerFrame_p;
    }

    /*!
      \brief Set the number of blocks per time frame

      \param blocksPerFrame -- The number of input data blocks combined into a
             single time-frame of the resulting skymap; keep in mind that for
	     time as target domain for the Beamformer \f$ N_{\rm bpf} \equiv 1 \f$.
      \param adjustDomain -- Adjust the domain in case the provided value does
             agree with the current choice of the target domain for which the
	     data are processed by the Beamformer

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setBlocksPerFrame (uint const &blocksPerFrame,
			    bool const &adjustDomain=true);
    
    /*!
      \brief Get the number of time frames

      \return nofFrames -- The total number of time frames to generate for the
              resulting skymap.
    */
    inline uint nofFrames () const {
      return nofFrames_p;
    }

    /*!
      \brief Set the number of time frames

      \param nofFrames -- The total number of time frames to generate for the
             resulting skymap.
    */
    void setNofFrames (uint const &nofFrames);

    /*!
      \brief Get the number of blocks to be read in order to create the image

      \return nofBlocks -- The number of blocks, \f$ N_{\rm Blocks} =
      N_{\rm Frames} \cdot N_{\rm bpf} \f$, to be read in order to create the image
    */
    inline uint nofBlocks () const {
      return nofFrames_p*blocksPerFrame_p;
    }
    
    /*!
      \brief Create a coordinate object from the internally stored parameters

      \return coord -- casa::LinearCoordinate object wrapping the
              characteristics of the time axis.
    */
    inline casa::LinearCoordinate timeAxisCoordinate () const {
      return coordTime_p;
    };

    /*!
      \brief Create a coordinate object from the internally stored parameters

      \return coord -- casa::SpectralCoordinate object wrapping the
              characteristics of the frequency axis.
    */
    inline casa::SpectralCoordinate frequencyAxisCoordinate () const {
      return coordFrequency_p;
    }

    // --------------------------- Overloading of methods inherited from TimeFreq
    
    /*!
      \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$
      
      \param blocksize -- Blocksize, [samples]
    */
    virtual void setBlocksize (uint const &blocksize);
    
    /*!
      \brief Set the sample frequency, \f$ \nu_{\rm Sample} \f$

      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
    */
    void setSampleFrequency (double const &sampleFrequency);

    /*!
      \brief Set the Nyquist zone, \f$ N_{\rm Nyquist} \f$

      \param nyquistZone -- Nyquist zone,  [1]
    */
    void setNyquistZone (uint const &nyquistZone);

    /*!
      \brief Set the reference time, i.e. the start of the time axis

      \param referenceTime -- The reference time, \f$ t_0 \f$, marking the
             start of the time axis
    */
    void setReferenceTime (double const &referenceTime);
    
    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the shape, i.e. the number of elements along each axis
      
      \return shape -- [time,freq] = \f$ [ N_t , N_\nu] \f$ The number of elements
              along each of the two coupled axes.
    */
    virtual casa::IPosition shape () const;
    
    /*!
      \brief Get the reference value for the world coordinates of the axes

      \param nFrame -- The number of the frame for which to retrieve the
             reference values; keep in mind that the reference value for the
	     time axis indeed is dependent on the frame number.

      \return refValue -- [time,freq] = \f$ [ t_{\rm CRVAL}, \nu_{\rm CRVAL} ] \f$
    */
    Vector<double> referenceValue () const;

    // -------------------------------------------------------------- WCS methods

    /*!
      \brief Get the names of the world axes

      \return names -- The names of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisNames()<tt> function.
    */
    Vector<casa::String> worldAxisNames () const;
    
    /*!
      \brief Get the units of the world axes

      \return units -- The units of the world axes, as retrieved through the
              <tt>casa::Coordinate::worldAxisUnits()<tt> function.
    */
    Vector<casa::String> worldAxisUnits () const;

    /*!
      \brief Get the value of the reference pixel

      \return refPixel -- The value of the reference pixel, as retrieved through
              the <tt>casa::Coordinate::referencePixel()<tt> function.
    */
    Vector<double> referencePixel() const;

    /*!
      \brief Get the matrix for the linear transformation

      \return Xform -- The matrix of the linear transformation, as retrieved
              through the <tt>casa::Coordinate::linearTransform()<tt> function.
     */
    Matrix<double> linearTransform();

    /*!
      \brief Get the increment between subsequent values along the axes

      \return increment -- [time,freq] = \f$ [ \delta_t, \delta_\nu ] \f$
    */
    Vector<double> increment () const;

    /*!
      \brief Conversion from pixel to world coordinates

      \retval world -- Values in world coordinates
      \param pixel  -- Values in pixel coordinates
    */
    void toWorld (Vector<double> &world,
		  Vector<double> const &pixel);

    /*!
      \brief Conversion from world to pixel coordinates

      \retval pixel -- Values in pixel coordinates
      \param world  -- Values in world coordinates
    */
    void toPixel (Vector<double> &pixel,
		  Vector<double> const &world);

    /*!
      \brief Add the coordinates to a coordinate system object

      \retval csys  -- Coordinate system object collecting the individual
              coordinate
      \param append -- Append the coordinates to the existing coordinate system
             object. If <tt>append=false</tt> then any already existing
	     coordinate objects will be removed from the coordinate system first.
    */
    void toCoordinateSystem (casa::CoordinateSystem &csys,
			     bool const &append=true);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TimeFreqCoordinate const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize the internal settings of the object
    */
    void init (CoordinateType const &coordType,
	       uint const &blocksPerFrame,
	       uint const &nofFrames);

    //! Set the coordinate objects
    void setCoordinates (int const &nFrame=0);
    
  };
  
} // Namespace CR -- end

#endif /* TIMEFREQSKYMAP_H */
  
