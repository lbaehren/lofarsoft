/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
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

#ifndef TBBCTLIN_H
#define TBBCTLIN_H

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>

#include <casa/namespace.h>

//CR-tools header files
#include <IO/DataReader.h>

#define TBBCTL_BLOCK_SIZE 2140

typedef struct {
  unsigned char station;
  unsigned char RSP;
  unsigned char RCU;
  unsigned char sampleFreq;
  unsigned int SeqNr;   
  unsigned int Date;
  unsigned int SampleNr;
  unsigned short NoSamples;
  unsigned short NoFreqBands; // should be zero, as we are looking at sample data
  unsigned int bandsel[16];
  unsigned short Spare;
  unsigned short HeaderCRC; 
} tbbctl_head;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class tbbctlIn
    
    \ingroup Data
    
    \brief Class to read in 
    
    \author Andreas Horneffer

    \date 2007/11/07

    \test ttbbctlIn.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class tbbctlIn : public DataReader{
    
  private:

    //! filename (incl. path) of the file to be read
    String filename_p;

    //! Is this object attached to a file?
    Bool attached_p;
    
    //! Pointer to the header-data
    tbbctl_head *headerpoint_p;

    //! Number of antennas in the event
    Int NumAntennas_p;
    
    //! Matrix with the data itself
    Matrix<short> channeldata_p;

    //! Vector with the antenna IDs
    Vector<Int> AntennaIDs_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    tbbctlIn ();
    
    /*!
      \brief Augmented constructor
      
      \param filename -- name (incl. path) of the lopes-eventfile to be read.
    */
    tbbctlIn (String const &filename);

    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~tbbctlIn ();
    
    // ---------------------------------------------------------------- Operators
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, tbbctlIn.
    */
    std::string className () const {
      return "tbbctlIn";
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
    
    
  private:
    
    
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

#endif /* TBBCTLIN_H */
  
