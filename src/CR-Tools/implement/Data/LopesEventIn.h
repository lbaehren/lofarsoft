/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#ifndef LOPESEVENTIN_H
#define LOPESEVENTIN_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>

#include <casa/namespace.h>

#include <IO/DataReader.h>
#include <Data/lopesevent_def.h>

using CR::DataReader;

namespace CR { // Namespace CR -- begin
  
  // Not more than 30 antennas available (yet).
#define MAX_NUM_ANTENNAS 31
  // LOPES et al. have a (fixed) sampletare of 80 MSPS
#define LOPES_SAMPLERATE 80e6 
  // LOPES and LORUN work in the 2nd Nyquist zone.
#define LOPES_NYQUIST_ZONE 2
  
  /*!
    \class LopesEventIn
    
    \ingroup CR_Data
    
    \brief Read in LOPES event files via the DataReader
    
    \author Andreas Horneffer
    
    \date 2006/12/14
    
    \test tLopesEventIn.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>DataReader
    </ul>
    
    <h3>Synopsis</h3>
    I'm unhappy with the existing <tt>LopesEvent</tt> class as it is IMHO a mess.
    
    <h3>Example(s)</h3>
    
  */  
  class LopesEventIn : public DataReader {
    
  public:
    
    // ------------------------------------------------------------- Data
    
    //! Type of event contained in the dataset
    enum EvClass {
      //! Unspecified event
      Unspecified,
      //! Cosmic ray
      Cosmic,
      //! Simulation output
      Simulation,
      //! Test event
      Test,
      //! Solar flare event
      SolarFlare,
      //! Other, not further specified event
      Other
    };

    //! Name of the observatory
    enum Observatory {
      //! LOFAR Prototype Station
      LOPES,
      //! LOFAR at Radboud Universiteit Nijmegen
      LORUN
    };
    

  private:

    //! filename (incl. path) of the file to be read
    String filename_p;
    //! Is this object attached to a file?
    Bool attached_p;
    //! Pointer to the (header-)data
    lopesevent *headerpoint_p;
    //! Number of antennas in the event
    Int NumAntennas_p;
    //! Matrix with the data itself
    Matrix<short> channeldata_p;
    //! Vector with the antenna IDs
    Vector<Int> AntennaIDs_p;
    
  public:

    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    LopesEventIn();
    
    /*!
      \brief Augmented constructor
      
      \param filename -- name (incl. path) of the lopes-eventfile to be read.
    */
    LopesEventIn (String const &filename);
    
    /*!
      \brief Augmented constructor
      
      \param filename -- name (incl. path) of the lopes-eventfile to be read.
      \param blocksize   -- Size of a block of data, [samples]
    */
    LopesEventIn (String const &filename,
		  uint const &blocksize);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LopesEventIn();
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Attach to a (another) lopes-eventfile
      
      \param filename -- name (incl. path) of the lopes-eventfile to be read.

      \return ok -- True if successfull
    */
    Bool attachFile(String filename);

    /*!
      \brief return the raw ADC time series, in ADC-counts
      
      \return Matrix with the data
    */
    Matrix<Double> fx();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the data file.
      
      \return filename -- Name of the data file 
    */
    inline String  filename() const { return filename_p; }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LopesEventIn.
    */
    virtual std::string className () const {
      return "LopesEventIn";
    }
    
    // ---------------------------------------------------------------- Header data
    
    /*!
      \brief Get the LOPES-Event version
      
      \return version -- The LOPES-Event version
    */
    inline unsigned int version () {
      return headerpoint_p->version;
    }
    
    /*!
      \brief Get the length of the data set (in bytes)
      
      \return length -- The length of the data set (in bytes)
    */
    inline unsigned int length () {
      return headerpoint_p->length;
    }
    
    /*!
      \brief Get the size of one channel of data
      
      \return Filesize -- Size of one channel (number of shorts)
    */
    inline unsigned int Filesize () {
      return headerpoint_p->blocksize;
    }
    
    /*!
      \brief Get the presync of the first entry w.r.t. the sync signal 
      
      \return presync -- First entry is presync before the sync signal
    */
    inline int presync () {
      return headerpoint_p->presync;
    }
    
    /*!
      \brief Get the type of data stored in the file
      
      \return evclass -- The type of data stored in the file (currently only TIM-40
                         supported)
    */
    inline unsigned int dataType () {
      return headerpoint_p->type;
    }
    
    /*!
      \brief Get the type of the stored event
      
      \return evclass -- The type of the stored event 
    */
    inline unsigned int eventClass () {
      return headerpoint_p->evclass;
    }
    
    /*!
      \brief Get the ID of the observatory
      
      \return observatory -- The ID of the observatory
    */
    inline unsigned int observatory () {
      return headerpoint_p->observatory;
    }
    
    /*!
      \brief KASCADE-style timestamp (JDR)
      
      \return JDR -- KASCADE-style timestamp (JDR)
    */
    inline unsigned int timestampJDR () {
      return headerpoint_p->JDR;
    }
    
    /*!
      \brief KASCADE-style timestamp (TL)
      
      \return TL -- KASCADE-style timestamp (TL)
    */
    inline unsigned int timestampTL () {
      return headerpoint_p->TL;
    }
    
    /*!
      \brief Get the 40MHz timestamp from menable card
      
      \return LTL -- 40MHz timestamp from menable card 
    */
    inline unsigned int timestampLTL () {
      return headerpoint_p->LTL;
  }
    
  protected:
    /*!
      \brief Connect the data streams used for reading in the data
      
      \return status -- Status of the operation; returns <i>true</i> if everything
                        went fine.
    */
    bool setStreams();
    
    /*!
      \brief Fill the header information from the lopesevent header into a header record.
      
      \return status -- Status of the operation; returns <i>true</i> if everything
                        went fine.
    */
    bool setHeaderRecord ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (LopesEventIn const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialization 
    */
    void init ();
    
  };
  
} // Namespace CR -- end

#endif /* LOPESEVENTIN_H */
  
