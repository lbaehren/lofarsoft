/*-------------------------------------------------------------------------*
 | $Id:: CRCcheck.cc 1964 2008-09-06 17:52:38Z baehren                   $ |
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

#include <Data/CRCcheck.h>


namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
	
  CRCcheck::CRCcheck ()
  {

	}
  
  CRCcheck::CRCcheck (CRCcheck const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CRCcheck::~CRCcheck ()
  {
    destroy();
  }
  
  void CRCcheck::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  CRCcheck& CRCcheck::operator= (CRCcheck const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void CRCcheck::copy (CRCcheck const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CRCcheck::summary (std::ostream &os)
  {
    os << "[CRCcheck] Summary of internal parameters." << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
	uint16_t CRCcheck::CRC16(uint16_t * buffer, uint32_t length)
  {
    uint16_t CRC = 0;
    const uint32_t CRC_poly = 0x18005;
    const uint16_t bits = 16;
    uint32_t data = 0;
    const uint32_t CRC_div = (CRC_poly & 0x7fffffff) << 15;
  
		data = (buffer[0] & 0x7fffffff) << 16;
	  for(uint32_t i=1; i<length; i++)
    {
      data += buffer[i];
      for(uint16_t j=0; j<bits; j++)
      {
        if ((data & 0x80000000) != 0)
        { data ^= CRC_div; }
        data &= 0x7fffffff;
        data <<= 1;
      }
    }
    CRC = data >> 16;
    return CRC;
  }

	uint32_t CRCcheck::CRC32(uint16_t * buffer, uint32_t length)
	{
		const uint64_t andmask = 0x7fffffffffffLLU; // to prevent typos with all those f's; LLU = unsigned long long
    uint32_t CRC = 0;
		const uint64_t CRC_poly = 0x104c11db7LLU; // CRC polynomial IEEE 802.3 (kind of standard, used by Zip etc.)
		const uint16_t bits = 16;
		register uint64_t data = 0;
		const uint64_t CRC_div = (CRC_poly & andmask) << 15;
		
		data = buffer[0] & andmask;
		data <<= 16;
		data += buffer[1];
		data &= andmask;
		data <<= 16;
		
		for(uint32_t i=2; i<length; i++)
		{
			data += buffer[i];
			for(uint16_t j=0; j<bits; j++)
			{
				if ((data & 0x800000000000LLU) != 0)
				{ 
					data ^= CRC_div; 
				}
				data &= andmask;
				data <<= 1;
			}
		}
		CRC = uint32_t(data >> 16);
		return CRC;
	}
	
  uint16_t CRCcheck::headerCRC(tbbctl_head* header)
  {
		unsigned int seqnr = header->SeqNr; // temporary; we need to zero this out for CRC check
		header->SeqNr = 0;

    uint16_t * headerBuf = reinterpret_cast<uint16_t*> (header); 
		uint16_t CRC = this->CRC16(headerBuf, sizeof(tbbctl_head) / sizeof(uint16_t));
		header->SeqNr = seqnr; // and set it back again

    return CRC;
  }
  
 	
	uint32_t CRCcheck::CRC32_tbbdata(short * buffer, uint32_t length)
	{ // needs conversion to 12-bit unsigned before CRC-ing...
		uint16_t * convertedBuffer = new uint16_t[length];
		for(uint i=0; i<length - 2; i++)
		{
			convertedBuffer[i]=((uint16_t) buffer[i]) & 0x0fff; 
		}
		convertedBuffer[length-2] = (uint16_t) buffer[length - 1]; // have to swap shortint order of payload-CRC...
		convertedBuffer[length-1] = (uint16_t) buffer[length - 2];
		
		uint32_t CRCtoReturn = this->CRC32(convertedBuffer, length);
		
		delete convertedBuffer;
		return CRCtoReturn;
	}
	

} // Namespace CR -- end
