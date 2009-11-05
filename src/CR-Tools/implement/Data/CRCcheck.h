/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                  *
 *   Arthur Corstanje (<mail>)                                                     *
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

#ifndef CRCCHECK_H
#define CRCCHECK_H

// Standard library header files
#include <iostream>
//#include <string>
#include <Data/tbbctlIn.h>
#include <stdint.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CRCcheck
    
    \ingroup CR_Data
    
    \brief Class to check CRCs of raw TBB data. 
    
    \author Arthur Corstanje

    \date 2009/04/27

    \test ttbbctlIn.cc, which is the test class for the TBB raw data DataReader.
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>DAL::DataReader
    </ul>
    
    <h3>Synopsis</h3>
    
		This class checks CRCs of TBB data as read from the tbbctl --readpage command. It can (and should) also 
	  be used to do a CRC check on incoming TBB data at CEP. However currently the algorithm, as ported from 
	  a Python script by Astron, is not very fast - about 7 MB/s on a 2.4 GHz CPU... 
	  The generic CRC methods use 16-bit data buffers as input. This is not the usual char-buffer, but it prevents
	  problems with endian-ness when working on 16-bits data.
	 
    <h3>Example(s)</h3>
    
  */  
  class CRCcheck {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    CRCcheck ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CRCcheck object from which to create this new
             one.
    */
    CRCcheck (CRCcheck const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~CRCcheck ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CRCcheck object from which to make a copy.
    */
    CRCcheck& operator= (CRCcheck const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CRCcheck.
    */
    inline std::string className () const {
      return "CRCcheck";
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
		 \brief Generic CRC16 method, working on 16-bits input
		 
		 \param buffer -- unsigned 16-bits input
		 \param length -- buffer length
		 \return CRC16 -- CRC16 of buffer
		*/
		uint16_t CRC16(uint16_t * buffer, uint32_t length);
		/*! 
		 \brief Generic CRC32 method, working on 16-bits input
		 
		 \param buffer -- unsigned 16-bits input
		 \param length -- buffer length
		 \return CRC32 -- CRC32 of buffer
		 */
    
		uint32_t CRC32(uint16_t * buffer, uint32_t length);
		/*! 
		 \brief Check CRC16 of TBB raw data header record.
		 
		 \param header -- TBB header record
		 \return CRC16 -- CRC16 of header - should be zero if OK.
		 */		
    uint16_t headerCRC(tbbctl_head* header);
		/*! 
		 \brief Check CRC32 of TBB raw data payload (the ADC data).
		 
		 \param buffer -- buffer of ADC data + payload CRC32, as 16-bit short (signed).
		 \return CRC32 -- CRC32 of data - should be zero if OK.
		 */		
		uint32_t CRC32_tbbdata(short * buffer, uint32_t length);
    
  private:
	    //! Unconditional copying
    void copy (CRCcheck const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class CRCcheck -- end
  
} // Namespace CR -- end

#endif /* CRCCHECK_H */
  
