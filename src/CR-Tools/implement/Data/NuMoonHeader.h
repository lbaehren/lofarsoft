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

/* $Id: NuMoonHeader.h,v 1.2 2006/08/01 15:00:31 bahren Exp $*/

#ifndef NUMOONHEADER_H
#define NUMOONHEADER_H

#include <iostream>
#include <fstream>
#include <string>

using std::string;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class NuMoonHeader
    
    \ingroup CR_Data
    
    \brief Brief description for class NuMoonHeader
    
    \author Lars B&auml;hren
    
    \date 2006/02/24
    
    \test tNuMoonHeader.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    \verbatim
    HEADER    DADA
    HDR_VERSION    1.0
    HDR_SIZE    4096
    DADA_VERSION    1.0
    PIC_VERSION    1.0
    OBS_ID    MOON
    PRIMARY    unset
    SECONDARY    unset
    FILE_NAME    /data1/data
    FILE_SIZE    800000000
    FILE_NUMBER  0                   
    UTC_START    2006-02-20-03:59:40 
    MJD_START    53786.16643518519   #
    OBS_OFFSET   1600000000          
    OBS_OVERLAP    0
    SOURCE    moon
    RA    03:58:53.705
    DEC    +54:13:13.58
    TELESCOPE    WSRT
    INSTRUMENT    PuMa2
    FREQ         120                 
    BW    20
    TSAMP    0.025
    NBIT    8
    NDIM    1
    NPOL    2
    BYTES_PER_SECOND 80000000
    \endverbatim
    
    <h3>Example(s)</h3>
    
  */
  
  class NuMoonHeader {
    
    string header_p;
    int hdrVersion_p;
    int hdrSize_p;
    int dataVersion_p;
    int picVersion_p;
    string observationId_p;
    string primary_p;
    string secondary_p;
    string fileName_p;
    int fileSize_p;
    int fileNumber_p;
    /*   string UTC_START; */
    /*   string MJD_START; */
    /*   double OBS_OFFSET; */
    /*   double OBS_OVERLAP; */
    /*   string source_p; */
    
  public:
    
    enum Keyword {
      //! The type of header (check this!)
      HEADER,
      //! Header version (might change in the future)
      HDR_VERSION,
      //! Header size
      HDR_SIZE,
      //! DADA version
      DADA_VERSION,
      //! Pic version (??)
      PIC_VERSION,
      //! Observation ID
      OBS_ID,
      //! Primary (??)
      PRIMARY,
      //! Secondary (??)
      SECONDARY,
      //! Original file name
      FILE_NAME,
      //! Size of the file
      FILE_SIZE,
      //! Number of the file
      FILE_NUMBER,
      //! Observation start (UTC)
      UTC_START,
      //! Observation start (MJD)
      MJD_START,
      //! Offset w.r.t. to observation start
      OBS_OFFSET,
      //! Observation overlap (??)
      OBS_OVERLAP,
      //! Source observed
      SOURCE,
      //! RA of source position (J2000)
      RA,
      //! DEC of source position (J2000)
      DEC,
      //! Telescope with which the observation was carried out
      TELESCOPE,
      //! Instrument with which the observation was carried out
      INSTRUMENT,
      //! Observation center frequency [MHz]
      FREQ,
      //! Bandwidth
      BW,
      //! (??)
      TSAMP,
      //! Number of bits per data value
      NBIT,
      //! Number of axis in the data array
      NDIM,
      //! Number of polarizations
      NPOL,
      //! Bytes per second
      BYTES_PER_SECOND
    };
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    NuMoonHeader ();
    
    /*!
      \brief Default constructor
    */
    NuMoonHeader (const string& fileName);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another NuMoonHeader object from which to create this new
      one.
    */
    NuMoonHeader (NuMoonHeader const& other);
    
    // ---------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~NuMoonHeader ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another NuMoonHeader object from which to make a copy.
    */
    NuMoonHeader &operator= (NuMoonHeader const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the file from which the data are read
      
      \return filename -- Name of the file from which the data are read
    */
    string fileName () {
      return fileName_p;
    }
    
    void setFileName (const string& fileName);
    
    // -------------------------------------------------------------------- Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (NuMoonHeader const& other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Parse the header of the data file
    */
    void parseHeader ();
    
  };

}  // Namespace CR -- end

#endif /* _NUMOONHEADER_H_ */
