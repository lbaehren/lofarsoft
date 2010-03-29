/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

/* $Id$*/

#include <Analysis/SubbandID.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  SubbandID::SubbandID ()
  {
  }
  
  SubbandID::SubbandID (Double const &sampleFrequency,
                        uint const &bandID,
                        uint const &nofSubbands)
    
  {
    sampleFrequency_p = sampleFrequency;
    bandID_p          = bandID;
    nofSubbands_p     = nofSubbands;
  }
  
  SubbandID::SubbandID (SubbandID const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  SubbandID& SubbandID::operator= (SubbandID const &other)
  {
    if (this != &other) {
      copy (other);
    }
    return *this;
  }
  
  void SubbandID::copy (SubbandID const &other)
  {
    sampleFrequency_p = other.sampleFrequency_p;
    bandID_p       = other.bandID_p;
    nofSubbands_p  = other.nofSubbands_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void SubbandID::summary (std::ostream &os)
  {
  }
  
  // ============================================================================
  //
  //  Computational Methods
  //
  // ============================================================================
  
  Vector<Double> SubbandID::calcFrequency (const Double& sampleFrequency,
					   const uint& subband_ID,
					   const uint& nofSubbands )
  {
    sampleFrequency_p = sampleFrequency;
    bandID_p          = subband_ID;
    nofSubbands_p     = nofSubbands;
    
    try {
      Vector<Double> frequencyValues ( nofSubbands, 0.0 );
      Double freq_width = sampleFrequency / 1024. ;        
      Double first_freq = freq_width*subband_ID ;
      
      frequencyValues (0) = first_freq ;
      
      for( uint n = 1; n <nofSubbands; n++){
	
	frequencyValues(n) = first_freq + freq_width ;
	first_freq  = frequencyValues(n) ;
      }
      return frequencyValues;
    }
    catch( AipsError x ){
      cerr << " SubbandID::calcFrequency " << x.getMesg() << endl ;
      return Vector<Double> () ;
    }
  }

  
  Vector<Double> SubbandID::calcFreqVector ( const Double& sampleFrequency,
                                             const Vector<uint>& subband_ID ) 
   {
    try {
    
         uint nOfSubbands = subband_ID.nelements() ;
	 
	 Vector<Double> frequencyValues ( nOfSubbands, 0.0 );
	 Double freq_width = sampleFrequency/1024. ;        
           
	for( uint n = 0; n <nOfSubbands; n++){
	    uint subbandID = subband_ID(n) ;
	    frequencyValues(n) = (subbandID-1)*freq_width ;
	}	 
	 
    
     return frequencyValues;
    }
    catch( AipsError x ){
      cerr << " SubbandID::calcFreqVector " << x.getMesg() << endl ;
      return Vector<Double> () ;
    }
  }
    
  Vector<uint> SubbandID::calcSubbandID (const Double& sampleFrequency,
					 const Double& subband_freq,
					 const uint& nofSubbands)
  { 
    try {
      
      Vector<uint> subbands_IDs( nofSubbands, 0 );
      uint subband_1 = int( subband_freq/(sampleFrequency/1024.) ) ;
      
      subbands_IDs (0) = subband_1 ; 
      
      for( uint m =1; m < nofSubbands ; m++ ){
	
	subbands_IDs( m ) = subband_1 + m ;
      }                                 
      return subbands_IDs ;
    }
    catch( AipsError x ){
      cerr << "SubbandID::calcSubbandID " <<  x.getMesg() << endl ;
      return Vector<uint> () ;
    }
    
  }
  
  
  Vector<uint> SubbandID::calcbandIDVector ( const Double& sampleFrequency,
					     const Vector<Double>& subband_frequencies )			       
  {
    try {
      
      uint nofsubabands = subband_frequencies.nelements() ;
      
      Double bandwidth = sampleFrequency /1024. ; 
      
      Vector<uint> subband_IDs( nofsubabands, 0 ) ;
      
      for( uint ns =0; ns <nofsubabands; ns++ ){
	
	subband_IDs( ns ) = int(subband_frequencies(ns)/bandwidth) ;
      }
      return subband_IDs ;
    }
    catch( AipsError x ){
      cerr << "SubbandID::calcbandIDVector "  << x.getMesg() << endl ;
      return Vector<uint> () ;
    }
  }
  
} // Namespace CR -- end
