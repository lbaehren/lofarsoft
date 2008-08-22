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

/* $Id: ionoCalibration.cc,v 1.3 2007/08/08 15:29:50 singh Exp $*/

#include <Analysis/ionoCalibration.h>
#include <Analysis/ppfinversion.h>
#include <Analysis/SubbandID.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ionoCalibration::ionoCalibration ()
  {;}

  ionoCalibration :: ionoCalibration( const Matrix<DComplex>& FFT_data,
                                      const Double& hrAngle,
                                      const Double& declinationAngle,
                                      const Double& geomagLatitude,
				      const Double& height_ionosphere,
				      const Double& TEC_value,
                                      const Double& samplingFreq,
                                      const uint& subband_ID,
				      const uint& n_subbands )
  {;}
  
  ionoCalibration :: ionoCalibration( const Matrix<DComplex>& FFT_data,
                                      const Double& hrAngle,
                                      const Double& declinationAngle,
                                      const Double& geomagLatitude,
                                      const Double& height_ionosphere,
				      const Double& TEC_value,
				      const Double& samplingFreq,
				      const Vector<Double>& subband_frequencies )
  {;}
  
  ionoCalibration::ionoCalibration (ionoCalibration const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ionoCalibration::~ionoCalibration ()
  {
    destroy();
  }
  
  void ionoCalibration::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  ionoCalibration& ionoCalibration::operator= (ionoCalibration const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void ionoCalibration::copy (ionoCalibration const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void ionoCalibration::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Calculation Methods
  //
  // ============================================================================
  
  
   Double ionoCalibration::calcElevationAngle ( const Double& hrAngle,
                   	    		        const Double& declinationAngle,
                   	    		        const Double& geomagLatitude ) 
					     
   {
     try{
          Double elevationAngle( 0.0 ) ;
	  
	  elevationAngle=sin(geomagLatitude)*sin(declinationAngle)+ cos(geomagLatitude)*cos(declinationAngle)*cos(hrAngle);
	    
         return elevationAngle ;
       }
      
     catch( AipsError x ){
        cerr << " ionoCalibration::calcElevationAngle " << x.getMesg() << endl ;
        return Double () ;
      }
  }					     
    
  
  
  Double ionoCalibration::calcSTECvalue ( const Double& height_ionosphere,
		          		  const Double& TEC_value,
			  		  const Double& elevationAngle ) 
					  
  {
   try {
         Double R_earth = 6371e+3 ;
	 
	 Double dem_sqr = (cos(elevationAngle))*(cos(elevationAngle));
	 
	 Double correction_factor = (R_earth/(R_earth + height_ionosphere))*(R_earth/(R_earth + height_ionosphere));
	 
	 Double mapping_fun = 1./sqrt( 1-correction_factor*dem_sqr ) ;	
	 
	 Double STEC_value = mapping_fun*TEC_value ;
	 
	return  STEC_value;
      }
      
     catch( AipsError x ){
        cerr << " ionoCalibration:: calcSTECvalue" << x.getMesg() << endl ;
        return Double () ;
      }
  }	
  
  
  
  Vector<Double> ionoCalibration::calRelativePhaseDiff( const Double& STEC_value, 
                                                        const Vector<Double>& subband_frequencies ) 
     
 {
   try{
   
        Double multiplier = 1.34e-7 ;
	
	Double pi = 3.1416 ;
	
	//Double subband_width = samplingFreq/1024.;
	
	uint nofelement = subband_frequencies.nelements() ;
	
	Vector<Double> relativePhaseDiff( nofelement, 0.0 );
	
	for( uint i=1; i< nofelement; i++ ){
	     
            relativePhaseDiff(i)=2.*pi*multiplier*STEC_value*(1./subband_frequencies(i)-1./subband_frequencies(i-1)) ;
	     
            }
	return  relativePhaseDiff;
      }
      
     catch( AipsError x ){
        cerr << " ionoCalibration:: calRelativePhaseDiff" << x.getMesg() << endl ;
        return Vector<Double> () ;
      }
  }  
  
  
  Vector<Double> ionoCalibration::calRelativeDelay( const Double& STEC_value, 
                                                    const Vector<Double>& subband_frequencies ) 
   
  {
    try{
    
        Double multiplier = 1.34e-7 ;
	
	Double pi = 3.1416 ;
	
	uint nofelements = subband_frequencies.nelements() ;
	
        Vector<Double> relativeDelay( nofelements, 0.0 ) ;
	
	Double subband_width_diff = 0.0;
	
	for( uint i=1; i< nofelements; i++ ){
	
 subband_width_diff
 =1./(subband_frequencies(i))/(subband_frequencies(i))-1./(subband_frequencies(i-1))/(subband_frequencies(i-1)) ;
	   
	   relativeDelay(i) = 2.*pi*multiplier*STEC_value*subband_width_diff ;
	      
	   }
	      
      return  relativeDelay ;
      }
      
     catch( AipsError x ){
        cerr << " ionoCalibration:: calRelativePhaseDelay" << x.getMesg() << endl ;
        return Vector<Double> () ;
      }
  }
  
  Matrix<DComplex> ionoCalibration::SubbandGenerator( const Matrix<DComplex>& FTData,
						      const Double& samplingFreq,
				     		      const Vector<Double>& subband_frequencies ) 
    
  {
    try {
      
      SubbandID sbID;
      Vector<uint> subband_IDs = sbID.calcbandIDVector( samplingFreq,
							subband_frequencies ) ;
      
      uint nofelement = subband_IDs.nelements ();
      
      uint Rows = FTData.nrow() ;
      uint Columns = FTData.ncolumn() ;
      
      Matrix<DComplex> conjugateArray( Rows, Columns, 0.0 ) ;
      
      Matrix<DComplex> generatedArray( 1024, Columns, 0.0 ) ;
      
      conjugateArray = conj( FTData ) ;
      
      if ( nofelement == Rows ) {
	
	for( uint r =0; r < nofelement ; r++ ){
	  
	  uint num = subband_IDs(r) ;
	  
	  generatedArray.row(num) = FTData.row(r) ;
          
	  if( r <512 ){		   
	    generatedArray.row( 1024-num ) = conjugateArray.row(r) ;
	  }
	}
      }
      return generatedArray ;
    }
    
    catch( AipsError x ){
      cerr<< "ionoCalibration::SubbandGenerator " << x.getMesg() << endl ;
      return Matrix<DComplex> () ;
    }
  }
  
  
  Matrix<DComplex> ionoCalibration::phaseCorrection( const Matrix<DComplex>& FFT_data,
				    		     const Double& hrAngle,
				    		     const Double& declinationAngle,
				    		     const Double& geomagLatitude,
				    		     const Double& height_ionosphere,
				    		     const Double& TEC_value,
				    		     const Double& samplingFreq,
				    		     const Vector<Double>& subband_frequencies )
  {
    
    try {
      
      ionoCalibration ionoCal ;
	 
	 DComplex j(0, 1);
	 
	 Double elevationAngle = ionoCal.calcElevationAngle ( hrAngle,
                   	    		                      declinationAngle,
                   	    		                      geomagLatitude )  ;
      
         Double STEC_value = ionoCal.calcSTECvalue ( height_ionosphere,
		          		             TEC_value,
			  		             elevationAngle )  ;  
						     
         Vector<Double> relativePhase_diff = ionoCal.calRelativePhaseDiff( STEC_value, 
                                                                           subband_frequencies ) ;
									   
// 	 uint n_elements = relativePhase_diff.nelements() ;
	 
	 uint nofrows = FFT_data.nrow() ;
	 
	 uint nofcolumns = FFT_data.ncolumn() ;
	 
	 Matrix<Double> FFT_phases( nofrows, nofcolumns, 0.0 );
	 
	 Matrix<Double> magnitude_FFT_data( nofrows, nofcolumns, 0.0 );
	  
	 Matrix<Double> phase_corrected( nofrows, nofcolumns, 0.0 ) ;
	 
	 Matrix<DComplex> phase_multiplied( nofrows, nofcolumns, 0.0 ) ;
	 
	 Matrix<DComplex> FFT_phase_corrected( nofrows, nofcolumns, 0.0 ) ;
	 
	 magnitude_FFT_data = amplitude( FFT_data ) ;
	 
	 FFT_phases = phase(FFT_data) ;
	 
	 uint nofelements = relativePhase_diff.nelements() ;
	 
	 if( nofrows == nofelements ) {
	 
	      for( uint r=0; r < nofrows; r++ ){
	      
	              for( uint s=0; s< nofcolumns; s++ ){
	                         
	                   phase_corrected(r,s) = FFT_phases(r,s) - relativePhase_diff(r) ;
			   
			   phase_multiplied(r,s)= exp(j*(phase_corrected(r,s))) ;
			   
			   FFT_phase_corrected(r,s) = (magnitude_FFT_data(r,s))*(phase_multiplied(r,s)) ;
			   
		          }
		      
		     }
  
	 }  
	 
	/* Vector<uint> subband_IDs = sbID.calcbandIDVector( samplingFreq,
							   subband_frequencies ) ;
							   
	 Matrix<DComplex> generated_DFTdata = ionoCal.SubbandGenerator( FFT_phase_corrected,
                                     		                        samplingFreq,
                  		       		                        subband_frequencies )  ;		     
	*/	     
	 return FFT_phase_corrected ;
      }
      
     catch( AipsError x ){
        cerr << " ionoCalibration:: phaseCorrection" << x.getMesg() << endl ;
        return Matrix<DComplex> () ;
      }
  } 

  
   Matrix<DComplex> ionoCalibration::phaseRECorrection( const Matrix<DComplex>& FFT_data,
                                                        const Double& hrAngle,
                                                        const Double& declinationAngle,
                                                        const Double& geomagLatitude,
                                                        const Double& height_ionosphere,
		                                        const Double& TEC_value,
		                                        const Double& samplingFreq,
						        const Vector<Double>& subband_frequencies ) 
 
 {
   
    try {
             
	 ionoCalibration ionoCal ;
	 
	 DComplex j(0, 1);
	 
	 Double elevationAngle = ionoCal.calcElevationAngle ( hrAngle,
                   	    		                      declinationAngle,
                   	    		                      geomagLatitude )  ;
      
         Double STEC_value = ionoCal.calcSTECvalue ( height_ionosphere,
		          		             TEC_value,
			  		             elevationAngle )  ;  
						     
         Vector<Double> relativePhase_diff = ionoCal.calRelativePhaseDiff( STEC_value, 
                                                                           subband_frequencies ) ;
									   
	 
	 uint nofrows = FFT_data.nrow() ;
	 
	 uint nofcolumns = FFT_data.ncolumn() ;
	 
	 Matrix<Double> FFT_phases( nofrows, nofcolumns, 0.0 );
	 
	 Matrix<Double> magnitude_FFT_data( nofrows, nofcolumns, 0.0 );
	  
	 Matrix<Double> phase_corrected( nofrows, nofcolumns, 0.0 ) ;
	 
	 Matrix<DComplex> phase_multiplied( nofrows, nofcolumns, 0.0 ) ;
	 
	 Matrix<DComplex> FFT_phase_corrected( nofrows, nofcolumns, 0.0 ) ;
	 
	 magnitude_FFT_data = amplitude( FFT_data ) ;
	 
	 FFT_phases = phase(FFT_data) ;
	 
	 uint nofelements = relativePhase_diff.nelements() ;
	 
	 if( nofrows == nofelements ) {
	 
	      for( uint r=0; r < nofrows; r++ ){
	      
	              for( uint s=0; s< nofcolumns; s++ ){
	                         
	                   phase_corrected(r,s) = FFT_phases(r,s) + relativePhase_diff(r) ;
			   
			   phase_multiplied(r,s)= exp(j*(phase_corrected(r,s))) ;
			   
			   FFT_phase_corrected(r,s) = (magnitude_FFT_data(r,s))*(phase_multiplied(r,s)) ;
			   
		          }
		      
		     }
  
	 }  
// 	 Matrix<DComplex> generated_DFTdata = ionoCal.SubbandGenerator( FFT_phase_corrected,
//                                      		                        samplingFreq,
//                   		       		                        subband_frequencies )  ;	
// 	
	 return FFT_phase_corrected ;
      }
      
     catch( AipsError x ){
        cerr << " ionoCalibration:: phaseRECorrection" << x.getMesg() << endl ;
        return Matrix<DComplex> () ;
      }
  } 

} // Namespace CR -- end
