/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
 *                                                                         *
 *   Edited by: Jaap Kroes     J.Kroes@student.science.ru.nl               *
 *              Pim Schellart  P.Schellart@student.science.ru.nl           *
 *              Lars B"ahren   bahren@astron.nl                            *
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

/* $Id: LopesEvent.h,v 1.11 2006/07/05 16:01:08 bahren Exp $ */

#ifndef _LOPESEVENT_H_
#define _LOPESEVENT_H_

#include <sstream>
#include <fstream>
#include <iostream>

#include <lopes/Data/Data.h>
#include <lopes/IO/DataReader.h>

using namespace std;

namespace LOPES {

/*!
  \class LopesEvent
  
  \ingroup Data

  \brief Class for dealing with LOPES event files
  
  \author Sven Lafebre

  \test tLopesEvent.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[LOPES-Tools] Data
    <li>[LOPES-Tools] DataReader
  </ul>
  
  <h3>Synopsis</h3>
  
  This class contains the framework for reading, writing and accessing LOPES
  event files, conforming to the LOPES data record specification version 3 as
  described under section <em>LOPES data records</em> in <em>The LOPES-Tools
  Software package Manual</em>.

  As of 2006/06/13 the LopesEvent class is derived from the DataReader class;
  the motivation for this is provide the means to perform pre-processing of
  data (such as conversion from ADC values to voltages) directly within a 
  common framework -- for more a more detailed description see the documentation
  of the DataReader class. The major changes introduced by this are
  <ul>
    <li>the <tt>blocksize</tt> no longer is stored in LopesEvent, but is in
    DataReader. Thus access to this parameter is via DataReader::blocksize() 
    and DataReader::setBlocksize() for an object, and via direct manipulation
    of the protected <tt>blocksize_p</tt> variable.
    <li>
  </ul>

  <h3>Limitations</h3>

  WARNING: Please mind that this class is only tested to work on
  Intel/lillte-endian machines. Problems due to reading/writing of memory blocks
  of non standard size could occur.

*/

class LopesEvent : public DataReader {
  
 public:
  
  enum EvType {
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

  enum Observatory {
    //! LOFAR Prototype Station
    LOPES,
    //! LOFAR at Radboud Universiteit Nijmegen
    LORUN
  };
  
 private:
  //!  The filename
  string filename_;
  
  //!  The length of the data record in bytes
  uint length_;
  
  //! Version number of the data record
  uint version_;
  
  //!  KASCADE style timestamp
  uint timeJDR_;
  
  //!  KASCADE style timestamp
  uint timeTL_;
  
  /*!
    \brief Type of data record. 
    
    LOPES-Tools currently only supports the TIM-40 type (dataType == 1)
  */
  uint dataType_;
  
  //!  Type of the recorded event.
  EvType evType_;
  
  //!  Number of samples (short) in one channel (optional)
  uint blocksize_;
  
  //!  First entry is presync for the sync signal (optional) 
  int  presync_;
  
  //!  Timestamp from menable card
  uint timeLTL_;

  //! Observatory number LOPES (empty or 0), LORUN (1), etc..
  uint observatory_;

  //! The data itself
  Data** data_;
  
  //LopesEvent readfile(fstream* handle);
  
 public:

  // --- Construction ----------------------------------------------------------
  
  /*!
    \brief Empty constructor
  */
  LopesEvent ();

  /*!
    \brief Argumented constructor

    \param name      -- Name of the file from which to read in the data
    \param blocksize -- Size of a block of data, [samples]
  */
  LopesEvent (string const &name,
	      uint const &blocksize);
  
  /*!
    \brief Argumented constructor

    \param name        -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  LopesEvent (string const &name,
	      uint const &blocksize,
	      Vector<Float> const &adc2voltage,
	      Matrix<Complex> const &fft2calfft);
  
  /*!
    \brief Argumented constructor
    
    This constructor creates a LOPES event object from a file with supplied name
    <b>name</b>.
  */
  LopesEvent (const char* name);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~LopesEvent();

  // --- Lopes event parameters and data ---------------------------------------
  
  /*!
    \brief Get the name of the data file.

    \return filename -- Name of the data file 
  */
  string  filename()
    {return filename_;}

  /*!
    \brief Get the length of the data record.

    \return length -- Length of the data record in bytes
  */
  uint&   length()
    {return length_;}

  /*!
    \brief Set/Get the length of the data record.

    \param in -- Length of the data record in bytes

    \return length -- Length of the data record in bytes
  */
  uint&   length(uint in)
    {length_ = in;    return length_;}

  /*!
    \brief Get the Version number of the data record

    \return version -- Version number of the data record
  */
  uint&   version()
    {return version_;}

  /*!
    \brief Set/Get the Version number of the data record

    \return in -- Version number of the data record

    \return version -- Version number of the data record
  */
  uint&   version(uint in)
    {version_ = in;   return version_;}

  uint&   timeJDR()
    {return timeJDR_;}

  uint&   timeJDR(uint in)
    {timeJDR_ = in;   return timeJDR_;}

  uint&   timeTL()
    {return timeTL_;}

  uint&   timeTL(uint in)
    {timeTL_ = in;    return timeTL_;}

  /*!
    \brief Get the type of the recorded event.

    \return dataType -- The type of the recorded event.
  */
  uint&   dataType()
    {return dataType_;}

  uint&   dataType(uint in)
    {dataType_ = in;  return dataType_;}

  EvType& evType()
    {return evType_;}

  EvType& evType(EvType in)
    {evType_ = in;    return evType_;}

  int&    presync()
    {return presync_;}

  int&    presync(uint in)
    {presync_ = in;   return presync_;}

  /*!
    \brief Timestamp from menable card
  */
  uint&   timeLTL()
    {return timeLTL_;}

  uint&   timeLTL(uint in)
    {timeLTL_ = in;   return timeLTL_;}

  /*!
    \brief Set/Get the observatory number
  */
  uint&   observatory()
    {return observatory_;}

  uint&   observatory(uint in)
    {observatory_ = in;   return observatory_;}

  /*!
    \brief Open a Lopes event file and read its header
  */
  void    readHeader(const char* name);

  /*!
    \brief Create a Lopes event file
  */
  void    createEvent(const char* name, fstream* handle);
           
  /*!
    \brief Open an event file
  */
  void    openEvent(const char* name, fstream* handle, int* handle_beg, int* handle_end);

  /*!
    \brief Start reading a header at a filehandle
  */
  void    readHeaderHere(fstream* handle);

  /*!
    \brief Start reading file contents at a filehandle
  */
  void    readContentsHere(fstream* handle, int* handle_beg, int* handle_end);

  /*!
    \brief Write a lopes event file
  */
  void    writeEvent(const char* name);

  /*!
    \brief Open a Lopes event file and write its header
  */
  void    writeHeader(const char* name);
  
  /*!
    \brief Start writing a header at a filehandle
  */
  void    writeHeaderHere(fstream* handle, int* handle_beg, int* handle_end);

  /*!
    \brief Write all channel data at a filehandle
  */
  void    writeContentsHere(fstream* handle);
  
  /*!
    \brief Write channel data at a filehandle
  */
  void    writeChannelHere(fstream* handle, uint c_num);
           
  Data*   dataSetById(uint id);

  Data*   dataSet(uint set);

  Data*   operator[] (uint set)
    {return dataSet(set);}

  short   operator() (uint set, uint i)
    {return dataSet(set)->data_[i];}

  // -- Functionality within the DataReader framework ---------------------------

  /*!
    \brief Get the raw time series after ADC

    For an LopesEvent object the lowest level of access to the data is slightly
    different as is the case for e.g. ITSCapture: the recorded data are small
    enough to completely fit into memory (and automatically held there after
    creation of a new LopesEvent object). Instead of initiating another read
    from the disk file, we here directly operate on the buffered data.
    
    \return fx -- Raw ADC time series, [Counts]
  */
  Matrix<Float> fx ();  

 protected:

  /*!
    \brief Connect the data streams used for reading in the data

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  Bool setStreams ();

 private:

  /*!
    \brief Initialize the internal parameters to default values 

    Special care should be taken here, that parameters used to derive e.g. array 
    sizes from do not remain unset; otherwise the program will crahs when trying
    to allocate memory or will not be able to properly handle excute the 
    destructor.
  */
  void init ();

  /*!
    \brief Initialize the internal parameters
  */
  void init (const char* name);
};

}

#endif /* _LOPESEVENT_H_ */
