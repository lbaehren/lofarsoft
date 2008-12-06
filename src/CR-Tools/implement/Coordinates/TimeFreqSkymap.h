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

#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>

#include <crtools.h>
#include <Coordinates/TimeFreq.h>
#include <Coordinates/CoordinateType.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TimeFreqSkymap
    
    \ingroup CR_Coordinates
    
    \brief Container for the time-frequency domain parameters of a skymap
    
    \author Lars B&auml;hren

    \date 2008/09/06

    \test tTimeFreqSkymap.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::TimeFreq
      <li>CR::CoordinateDomain
      <li>CR::SkymapCoordinates
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

    Base on the above variables we get different values for some of the quantities
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


    <h3>Example(s)</h3>
    
    <ul>
      <li>Store data inside a new TimeFreqSkymap object:
      \code
      uint blocksize (1024);
      casa::Quantity sampleFreq (200,"MHz");
      uint nyquistZone (1);
      uint blocksPerFrame (10);
      uint frames (100);
      
      TimeFreqSkymap tf (blocksize,
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
      
      TimeFreqSkymap tf_skymap (tf,
			        blocksPerFrame,
			        frames);
      \endcode
      <li>Get the internal object storing the coordinate domain information:
      \code 
      TimeFreqSkymap tf;

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
  class TimeFreqSkymap : public TimeFreq {
    
  protected:

    //! The target domain for which the data are processed by the Beamformer
    CoordinateType coordType_p;
    //! The number of data blocks added up within a single time frame
    uint blocksPerFrame_p;
    //! The number of time frames, \f$ N_{\rm Frames} \f$
    uint nofFrames_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TimeFreqSkymap ();
    
    /*!
      \brief Argumented constructor

      \param blocksPerFrame -- The number of data blocks added up within a single
             time frame
      \param nofFrames      -- The number of frames
    */
    TimeFreqSkymap (uint const &blocksPerFrame,
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
    TimeFreqSkymap (uint const &blocksize,
		    double const &sampleFrequency,
		    uint const &nyquistZone,
		    uint const &blocksPerFrame,
		    uint const &nofFrames);
    
    /*!
      \brief Argumented constructor

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC step.
      \param nyquistZone     -- Nyquist zone,  [1]
      \param blocksPerFrame  -- The number of data blocks added up within a single
             time frame
      \param nofFrames       -- The number of frames
    */
    TimeFreqSkymap (uint const &blocksize,
		    casa::Quantity const &sampleFrequency,
		    uint const &nyquistZone,
		    uint const &blocksPerFrame,
		    uint const &nofFrames);
    
    /*!
      \brief Argumented constructor

      \param timeFreq       -- CR::TimeFreq object acting as container for
             information based on blocksize, sample frequency and Nyquist zone.
      \param blocksPerFrame -- The number of data blocks added up within a single
             time frame
      \param nofFrames      -- The number of frames
    */
    TimeFreqSkymap (TimeFreq const &timeFreq,
		    uint const &blocksPerFrame,
		    uint const &nofFrames);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TimeFreqSkymap object from which to create this new
      one.
    */
    TimeFreqSkymap (TimeFreqSkymap const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TimeFreqSkymap ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TimeFreqSkymap object from which to make a copy.
    */
    TimeFreqSkymap& operator= (TimeFreqSkymap const &other); 
    
    // --------------------------------------------------------------- Parameters

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
    inline void setNofFrames (uint const &nofFrames) {
      nofFrames_p = nofFrames;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TimeFreqSkymap.
    */
    inline std::string className () const {
      return "TimeFreqSkymap";
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
      \brief Get the shape, i.e. the number of elements along each axis
      
      \return shape -- [time,freq] = \f$ [ N_t , N_\nu] \f$ The number of elements
              along each of the two coupled axes.
    */
#ifdef HAVE_CASA
    virtual casa::IPosition shape () const;
#else 
    virtual vector<int> shape () const;
#endif
    
    /*!
      \brief Get the reference value for the world coordinates of the axes

      \param nFrame -- The number of the frame for which to retrieve the
             reference values; keep in mind that the reference value for the
	     time axis indeed is dependent on the frame number.

      \return refValue -- [time,freq] = \f$ [ t_{\rm CRVAL}, \nu_{\rm CRVAL} ] \f$
    */
#ifdef HAVE_CASA
    casa::Vector<double> referenceValue (int const &nFrame=0) const;
#else 
    vector<double> referenceValue (int const &nFrame=0) const;
#endif

    /*!
      \brief Get the increment between subsequent values along the axes

      \return increment -- [time,freq] = \f$ [ \delta_t, \delta_\nu ] \f$
    */
#ifdef HAVE_CASA
    casa::Vector<double> increment () const;
#else 
    vector<double> increment () const;
#endif

    // --------------------------------------------------------- Optional methods

#ifdef HAVE_CASA

    /*!
      \brief Create a coordinate object from the internally stored parameters

      \return coord -- casa::LinearCoordinate object wrapping the
              characteristics of the time axis.
    */
    casa::LinearCoordinate timeAxisCoordinate ();

    /*!
      \brief Create a coordinate object from the internally stored parameters

      \return coord -- casa::SpectralCoordinate object wrapping the
              characteristics of the frequency axis.
    */
    casa::SpectralCoordinate frequencyAxisCoordinate ();

    /*!
      \brief Create a coordinate objects from the internally stored parameters
      
      \retval coord -- casa::LinearCoordinate object wrapping the
              characteristics of the time axis.
      \retval coord -- casa::SpectralCoordinate object wrapping the
              characteristics of the frequency axis.
      
      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool coordinates (casa::LinearCoordinate &time,
		      casa::SpectralCoordinate &freq);

#endif
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TimeFreqSkymap const &other);
    
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
    
  };
  
} // Namespace CR -- end

#endif /* TIMEFREQSKYMAP_H */
  
