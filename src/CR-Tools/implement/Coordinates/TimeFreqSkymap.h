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
#include <Coordinates/CoordinateDomain.h>
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
    </table>

    <h3>Example(s)</h3>
    
  */  
  class TimeFreqSkymap : public TimeFreq {
    
  protected:

    /*! The target domain for which the data are processed by the Beamformer */
    CoordinateDomain beamDomain_p;
    /*! The number of data blocks added up within a single time frame */
    uint blocksPerFrame_p;
    /*! The number of frames */
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
      \param nofFrames      -- The numbe of frames
    */
    TimeFreqSkymap (uint const &blocksPerFrame,
		    uint const &nofFrames);
    
    /*!
      \brief Argumented constructor

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC step, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
      \param blocksPerFrame -- The number of data blocks added up within a single
             time frame
      \param nofFrames      -- The numbe of frames
    */
    TimeFreqSkymap (uint const &blocksize,
		    double const &sampleFrequency,
		    uint const &nyquistZone,
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

      \return domain -- The target domain for the beamforming.
    */
    inline CoordinateDomain beamDomain () const {
      return beamDomain_p;
    }
    
    bool setBeamDomain (CR::CoordinateDomain::Types const &type);
    
    bool setBeamDomain (CR::CoordinateDomain const &domain);
    
    inline CoordinateDomain::Types beamDomainType () const {
      return beamDomain_p.type();
    }
    
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
             single time-frame of the resulting skymap.
    */
    inline void setBlocksPerFrame (uint const &blocksPerFrame) {
      blocksPerFrame_p = blocksPerFrame;
    }

    /*!
      \brief Get the number of time frames

      \return nofFrames -- The total number of time frames to generate for the
              resulting skymap.
    */
    inline uint nofFrames () const {
      return nofFrames_p;
    }

    inline void setNofFrames (uint const &nofFrames) {
      nofFrames_p = nofFrames;
    }
    
    /*!
      \brief Get the shape, i.e. the number of elements along each axis
      
      \return shape -- [time,freq] The number of elements along each of the 
              two coupled axes.
    */
#ifdef HAVE_CASA
    virtual casa::IPosition axisShape () const;
#else 
    virtual vector<int> axisShape () const;
#endif
    
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
      \brief Create a coordinate object from the internally stored parameters

      \return coord 
    */
    casa::SpectralCoordinate frequencyAxisCoordinate ();

    /*!
      \brief Create a coordinate object from the internally stored parameters

      \return coord 
    */
    casa::LinearCoordinate timeAxisCoordinate ();

    /*!
      \brief Create a coordinate objects from the internally stored parameters

      \retval freq -- 
      \retval time -- 
    */
    void coordinates (casa::SpectralCoordinate &freq,
		      casa::LinearCoordinate &time);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TimeFreqSkymap const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TIMEFREQSKYMAP_H */
  
