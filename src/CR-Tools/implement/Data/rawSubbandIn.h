/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#ifndef RAWSUBBANDIN_H
#define RAWSUBBANDIN_H

// Standard library header files
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>

#include <casa/namespace.h>

#define uint32 uint32_t
#define uint16 uint16_t
#define uint8 uint8_t

namespace CR { // Namespace CR -- begin
  
  /*!
    \class rawSubbandIn
    
    \ingroup Data
    
    \brief Read in the raw dump of subband data
    
    \author Andreas Horneffer

    \date 2008/03/07

    \test trawSubbandIn.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class rawSubbandIn {

  protected:
    
    struct FileHeader {
      uint32_t   magic;        // 0x3F8304EC, also determines endianness
      uint8_t    bitsPerSample;
      uint8_t    nrPolarizations;
      uint16_t   nrBeamlets;
      uint32_t   nrSamplesPerBeamlet;
      char     station[16];
      uint32_t   dummy;
      double   sampleRate;    // 156250.0 or 195312.5
      double   subbandFrequencies[54];
      double   beamDirections[8][2];
      int16_t    beamlet2beams[54];
      uint32_t   dummy2;
    };

    struct BlockHeader {
      uint32_t   magic; // 0x2913D852
      int32_t    coarseDelayApplied[8];
      int32_t    dummy;
      double   fineDelayRemainingAtBegin[8],fineDelayRemainingAfterEnd[8];
      Int64    time[8]; // compatible with TimeStamp class.
      uint32_t   nrFlagsRanges[8];
      struct range {
	uint32_t    begin; // inclusive
	uint32_t    end;   // exclusive
      } flagsRanges[8][16];
    };

    struct FileHeader FHead;

    int numblocks;
    double firstdate,lastdate;
    Vector<Double> blockdates;   
    String OpenedFile;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    rawSubbandIn ();
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~rawSubbandIn ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, rawSubbandIn.
    */
    std::string className () const {
      return "rawSubbandIn";
    }

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
    
    /*!
      \brief Get some data from the file
      
      \param Filename -- path to the file that is to be read in.
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Currently generates one bogous "error message" when it has scanned the whole file...
    */
    Bool attachFile(String Filename);
    
    /*!
      \brief Get some data from the file
      
      \param startdate -- date/timestamp of the first sample in the data
      \param nSamples  -- number of samples to return
      \param pol       -- polarization (0 or 1)

      \return Matrix with the data
    */
    Matrix<DComplex> getData(Double startdate, int nSamples, int pol);


    /*!
      \brief Get the subband indices

      \return Vector with the indices.
    */
    Vector<Int> getSubbandIndices();
    
    
  protected:

    double ntohd(double in);

    Int64 ntohll(Int64 in);

    Bool readFileHeader(FILE *fd, FileHeader &FHead);

    Bool readBlockHeader(FILE *fd, BlockHeader &BHead);

  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* RAWSUBBANDIN_H */
  
