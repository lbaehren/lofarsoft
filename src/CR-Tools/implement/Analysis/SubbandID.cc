/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                                     *
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

/* $Id: subbandID.cc,v 1.3 2007/08/08 15:29:50 singh Exp $*/

#include <Analysis/SubbandID.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  SubbandID::SubbandID ()
  {
    samplingFreq_p = 80e06;
    bandID_p       = 0;
    nofSubbands_p  = 256;
  }
  
  SubbandID::SubbandID (Double const &samplingFreq,
                        uint const &bandID,
                        uint const &nofsubbands)
    
  {
    samplingFreq_p = samplingFreq;
    bandID_p       = bandID;
    nofSubbands_p  = nofsubbands;
  }
  
  SubbandID::SubbandID ( const Double& samplingFreq,
                         const Double& subband_freq_1,
                         const uint& nofsubbands  ) 
  {
    samplingFreq_p = samplingFreq;
    bandID_p       = 0;
    nofSubbands_p  = nofsubbands;
  }
  
  SubbandID::SubbandID ( const Double& samplingFreq,
                         const Vector<Double>& subband_frequencies )
    
  {
    //uint nofsubbands ;
  }			 
  
  
  SubbandID::SubbandID (SubbandID const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SubbandID::~SubbandID ()
  {
    destroy();
  }
  
  void SubbandID::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  SubbandID& SubbandID::operator= (SubbandID const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void SubbandID::copy (SubbandID const &other)
  {
    samplingFreq_p = other.samplingFreq_p;
    bandID_p       = other.bandID_p;
    nofSubbands_p  = other.nofSubbands_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void SubbandID::summary ()
  {;}
  
  
  
  // ============================================================================
  //
  //  Computational Methods
  //
  // ============================================================================
  
  Vector<Double> SubbandID :: calcFrequency (const Double& sampling_Freq,
                                             const uint& subband_ID,
                                             const uint& n_subbands )
  {
    samplingFreq_p = sampling_Freq;
    bandID_p       = subband_ID;
    nofSubbands_p  = n_subbands;
    
    try {
      Vector<Double> freq_Val ( nofSubbands_p, 0.0 );
      Double freq_width = samplingFreq_p / 1024. ;        
      Double first_freq = freq_width*bandID_p ;
      
      freq_Val (0) = first_freq ;
      
      for( uint n = 1; n <nofSubbands_p; n++){
	
	freq_Val(n) = first_freq + freq_width ;
	first_freq  = freq_Val(n) ;
      }
      return freq_Val;
    }
    catch( AipsError x ){
      cerr << " SubbandID :: calcFrequency " << x.getMesg() << endl ;
      return Vector<Double> () ;
    }
  }
  
  
  Vector<uint> SubbandID :: calcSubbandID( const Double& sampling_freq,
                                           const Double& subband_freq,
                                           const uint& n_subbands )
  { 
    try {
      
      Vector<uint> subbands_IDs( n_subbands, 0 );
      
      uint subband_1 = int( subband_freq/(sampling_freq/1024.) ) ;
      
      subbands_IDs (0) = subband_1 ; 
      
      for( uint m =1; m < n_subbands ; m++ ){
	
	subbands_IDs( m ) = subband_1 + m ;
      }                                 
      return subbands_IDs ;
    }
    catch( AipsError x ){
      cerr << "SubbandID :: calcSubbandID " <<  x.getMesg() << endl ;
      return Vector<uint> () ;
    }
    
  }
  
  
  Vector<uint> SubbandID::calcbandIDVector ( const Double& sampling_freq,
					     const Vector<Double>& subband_frequencies )			       
  {
    try {
      
      uint nofsubabands = subband_frequencies.nelements() ;
      
      Double bandwidth = sampling_freq /1024. ; 
      
      Vector<uint> subband_IDs( nofsubabands, 0 ) ;
      
      for( uint ns =0; ns <nofsubabands; ns++ ){
	
	subband_IDs( ns ) = int(subband_frequencies(ns)/bandwidth) ;
      }
      return subband_IDs ;
    }
    catch( AipsError x ){
      cerr << "SubbandID :: calcbandIDVector "  << x.getMesg() << endl ;
      return Vector<uint> () ;
    }
  }
  
} // Namespace CR -- end
