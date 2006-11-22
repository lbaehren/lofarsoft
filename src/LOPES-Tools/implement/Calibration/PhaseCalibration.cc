/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (< k.singh@astro.ru.nl >)                               *
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

/* $Id: PhaseCalibration.cc,v 1.10 2006/11/02 13:39:26 singh Exp $*/

#include <lopes/Calibration/PhaseCalibration.h>

/*!
  \class PhaseCalibration
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

// PhaseCalibration::PhaseCalibration ()
// {;}
// 
// PhaseCalibration::PhaseCalibration (PhaseCalibration const &other)
// {
//   copy (other);
// }
PhaseCalibration :: PhaseCalibration ()
{;}   

PhaseCalibration :: ~PhaseCalibration ()
{;}   

//Argumented Consturctor


PhaseCalibration :: PhaseCalibration( const Matrix<DComplex>& spectra,
  		     		      const Matrix<Double>& frequencyRanges,
		                      const Matrix<Double>& expectedPhases,
				      const Matrix<Double>& phaseGradient,
		                      const Vector<Double>& frequencyValues,
		                      const Vector<Double>& sampleJumps,
		                      const Int& referenceAntenna,
		     		      const Double& sampleRate,
		                      const Double& badnessWeight )
				
{ 
 //Matrix<DComplex> gainValues ;
}


  Vector<Int> PhaseCalibration::getFrequencyIndices( const Matrix<Double>& frequencyRanges,
						     const Vector<Double>& frequencyValues )
 {
 
 try {
 
  Int nOfRows = frequencyRanges.nrow() ;	// no of rows will depend on how many peaks have to be scanned
  
  Int nOfColumns = frequencyRanges.ncolumn() ;	// no. of columns will be always 2, i.e., min. and max. 
  						// frequency values to scan the power peak
  
  Int nOfIndices = nOfColumns*nOfRows ;		// how many indeces have to be scanned in the vector frequencyValues
  
  Vector<Int> frequencyIndices( nOfIndices );   
  
  //Int nOfElements = frequencyValues.nelements () ;
  
  Int k = 0;
  
  Int l = 0;

  Double frequencyValue = 0.0 ;
 
  Double frequencyRange = 0.0 ;
  
  for(Int i = 0; i< nOfRows; i++)
    
    {
    for(Int j = 0; j< nOfColumns; j++)
          
	  {
	  
	  do{
	  
	    frequencyValue = frequencyValues (k);
	    // cout<<" Frequency Value" << frequencyValue<< "\t";
            frequencyRange = frequencyRanges (i,j);
	    //  cout << " FRequency Range" << frequencyRange << "\t"<<"\t" ;
	    k = k+1 ;

	      } while ( frequencyValue < frequencyRange ) ;
	  cout<<endl;
	        frequencyIndices( l ) = k+1  ;
	         
		  l = l+1 ;
	     }	
	}
	
   return frequencyIndices ;
    
    }
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getFrequencyIndices " << x.getMesg () << endl ;
    return Vector<Int> ();
   }
 }
 
   
   
  Matrix<Int> PhaseCalibration::getIndexOfPowerPeaks( const Matrix<DComplex>& spectra,
  			       		 	      const Vector<Int>& frequencyIndex,
			       		 	      const Vector<Double>& frequencyValues )
						 
 {
 
  try {
  
  Matrix<Double> absoluteArray( amplitude( spectra) );
  
  Int nOfColumns = spectra.ncolumn() ;	         // no. of columns will be always 2, i.e., min. and max. 
  						// frequency values to scan the power peak
  
  //Int nOfRows = spectra.nrow() ;		// no of rows will depend on how many peaks have to be scanned
  
  Int nOfSegments = (frequencyIndex.nelements())/2  ; // No of segments of the input array spectra will depend on
  						      // the no. of elements in the vector frequencyIndex
    
  Matrix<Int> indexOfPowerPeaks( nOfSegments, nOfColumns ) ;
  
  Int nOfElements = frequencyIndex.nelements() ;
  
  Int n = 0 ;      // Row of the matrix indexOfPowerPeak
  
  Int m = 0 ;
  
  Int powerPeakIndex = 0 ;
   
   for( Int i =0; i< nOfElements ; i = i+2 )
    {
    
     Int segmentInit = frequencyIndex( i ) ;
     
     Int segmentFinal = frequencyIndex( i+1 ) ;
      
     Int segmentLength = (segmentFinal-segmentInit + 1) ;
      
      Matrix<Double> segmentedMatrix( absoluteArray( Slice( segmentInit, segmentLength ), Slice( 0, nOfColumns ) ) ) ;
      
      Int Columns = segmentedMatrix.ncolumn() ;
      
      Int Rows = segmentedMatrix.nrow() ;
      
      Double elementValue = 0 ;
      
   //   cout<< "Index of Power Peaks :"<< endl ;

      for( Int k = 0; k < Columns; k++ )
        {
	 for( Int j = 0; j < Rows; j++ )
	     {
	      if ( elementValue < segmentedMatrix( j, k) )
	        {
	        elementValue = segmentedMatrix( j, k ) ;
		
		m = j ;
		
		}
		
	       powerPeakIndex = segmentInit + m ;
	     }
	   
	indexOfPowerPeaks ( n, k ) = powerPeakIndex ;	

	cout<<indexOfPowerPeaks( n,k )<<"\t";
		
	}
	n = n+1 ;	
     }
   cout<< endl;

    return indexOfPowerPeaks ;
    
    }
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getIndexOfPowerPeaks " << x.getMesg () << endl ;
    return Matrix<Int> ();
   }
 }
   
 
 Vector<Double> PhaseCalibration::getPeakFrequencyValues( const Matrix<Int>& powerIndex,
 					                 const Vector<Double>& frequencyValues )

 {
 
  try {
      
    Int nOfRows = powerIndex.nrow() ;
    
    Vector<Double> peakFrequencyValues( nOfRows ) ;
    
   // cout<< "Peak Frequency Values :"<< endl;
    for( Int i = 0; i< nOfRows ; i++ )
      {
        Int j = powerIndex( i, 0 );    // peak power frequency index will be same for all antenna so 
					// calculated here only for one antenna
					
	peakFrequencyValues( i ) = frequencyValues( j );
    
	cout<<peakFrequencyValues( i )<< "\t" ;
	
	}
    cout<< endl;
    return peakFrequencyValues ;
    
    }
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getPeakFrequencyValues " << x.getMesg () << endl ;
    return Vector<Double> ();
   }
 }   
      
 
 Matrix<Double> PhaseCalibration::getPhaseValues( const Matrix<DComplex>& spectra,
 					         const Matrix<Int>& powerIndex )
					  
 {
 
  try { 
  
      Int nOfColumns = powerIndex.ncolumn() ;
      
      Int nOfRows = powerIndex.nrow() ;
      
      Matrix<DComplex> sortedArray ( nOfRows, nOfColumns ) ;
      
      Int k = 0 ;
      
      	 for( Int i = 0; i< nOfRows ; i++ )
	  {
	    for( Int r = 0; r < nOfColumns ;  r++ )
             {
	       k = powerIndex(i, r) ;
	     	  
	      sortedArray( i, r ) = spectra( k, r)  ;
	  }
	}
	     
	 Matrix<Double> phaseValue( phase(sortedArray));

	return phaseValue ;
    
    }
    
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getPhaseValues " << x.getMesg () << endl ;
    return Matrix<Double> ();
   }
 }
 
 
 Matrix<Double> PhaseCalibration::getPhaseDiffInDegree( const Matrix<Double>& calculatedPhases,
 					               const Matrix<Double>& expectedPhases,
						       const Matrix<Double>& phaseGradient,
						       const Int& referenceAntenna )
 	
 {
 
   try {	 
      
      Int nOfColumns = calculatedPhases.ncolumn() ;
      
      Int nOfRows = calculatedPhases.nrow() ;
      
      Int columns = expectedPhases.ncolumn() ;
      
      Matrix<Double> calculatedPhasesInDegree( nOfRows, nOfColumns ) ;   // Phases from Raw FFT
      
      Matrix<Double> phaseDiff( nOfRows, nOfColumns ) ; 
 
      Matrix<Double> phasesDiff( nOfRows, nOfColumns ) ;

      Matrix<Double> residualPhaseGradient( nOfRows, nOfColumns ) ;

      Matrix<Double> relativePhaseGradient( nOfRows, nOfColumns ) ;

      Matrix<Double> calculatedPhasesDiff( nOfRows, nOfColumns ) ;

       
      if ( nOfColumns == columns ) {
	   
	    Int columnNumber = referenceAntenna ;
	     
	 
             Vector<Double> referencePhaseGradient = phaseGradient.column( columnNumber ) ;

	     Vector<Double> referenceAntennaPhases = calculatedPhasesInDegree.column( columnNumber );

	     
	     for( Int i = 0; i< nOfColumns ; i++ ) {

	         for( Int j = 0; j< nOfRows ; j++ ) {

		calculatedPhasesInDegree(j,i) = calculatedPhases( j,i )*(180/3.1416);

                calculatedPhasesDiff(j,i) = calculatedPhasesInDegree(j,i) - referenceAntennaPhases(j) ;
    
		  relativePhaseGradient(j,i) = phaseGradient( j,i ) - referencePhaseGradient ( j ) ;

           	residualPhaseGradient( j,i ) = expectedPhases ( j, i ) -  relativePhaseGradient(j,i) ;
		
              phasesDiff( j,i ) = calculatedPhasesDiff( j,i ) -  residualPhaseGradient(j,i ) ;

            
                  if( phaseDiff( j, i) > 180.0 ) {
		 
		    phaseDiff( j, i) = phaseDiff( j, i) - 360  ;
		    
		   }
		    
		 if( phaseDiff( j, i) < -180.0 ) {
		 
		      phaseDiff( j, i) = phaseDiff( j, i) + 360  ;
		    
		     }

		 }  	// end of the inner for lk
		}
	   }
      else
	    {
	       cerr << "[ Error: PhaseCalibration::getResidualPhases ]"
	            << " Arguments of two phase matrices are of incompatible dimensions."
	            << endl;
	    }

         
	 return phaseDiff  ;
    
    }
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getResidualPhases " << x.getMesg () << endl ;
    return Matrix<Double> ();
   }
 }
      
 

 
 Matrix<Double> PhaseCalibration::getRelativeDelay( const Matrix<Double>& phaseDiff,
 						   const Double& sampleRate,
						   const Matrix<Int>& powerPeakIndices,
						   const Vector<Double>& frequencyValues ) 
 
 {
 
  try {
  
      Int nOfColumns = phaseDiff.ncolumn() ;
      
       Int nOfRows = phaseDiff.nrow() ; 
      
      Matrix<Double> relativeDelay( nOfRows, nOfColumns );
      
      Double peakFrequency = 0 ;
      
      for( Int i = 0; i< nOfColumns ; i++ ) {
	         
        for( Int j = 0; j< nOfRows ; j++ )  {
      
          Int m = powerPeakIndices( j, i );
	  
	  peakFrequency = frequencyValues( m ) ;
		 
          relativeDelay ( j,i ) = phaseDiff(j,i)/180.0/(2.0*peakFrequency)*sampleRate ;
	   
          //relativeDelay ( j,i ) = delays ;
           }
	 }
     
     return relativeDelay ;
     
       }
    catch( AipsError x )  {
    
    cerr << "PhaseCalibration::getRelativeDelay " << x.getMesg () << endl; 
    
    return Matrix<Double> () ;
    }
  }
  
  
  Vector<Bool> PhaseCalibration::getAntennaReturn ( const Matrix<DComplex>& spectra,
  		     				    const Matrix<Double>& frequencyRanges,
		     				    const Matrix<Double>& expectedPhases,
						    const Matrix<Double>& phaseGradient,
		     			  	    const Vector<Double>& frequencyValues,
		     				    const Vector<Double>& sampleJumps,
		     				    const Int& referenceAntenna,
		     				    const Double& sampleRate,
		     				    const Double& badnessWeight )
						    
						    
						  
 {
   try {
   
      PhaseCalibration phcl ;
      
      Vector<Int> frequencyIndex = phcl.getFrequencyIndices( frequencyRanges, 
      							     frequencyValues );
      
      Matrix<Int> powerPeakIndices = phcl.getIndexOfPowerPeaks( spectra, 
      							        frequencyIndex,
								frequencyValues ) ; 
      
      Matrix<Double> calculatedPhases = phcl.getPhaseValues( spectra,
      							    powerPeakIndices ) ;
      
      Matrix<Double> phaseDiff = phcl.getPhaseDiffInDegree( calculatedPhases,
      						            	             expectedPhases,
									     phaseGradient,
							                     referenceAntenna ) ;
      
      //  Matrix<Double> residualDegree = phcl.getResidualDegree( residualPhases ) ;
      
      Matrix<Double> relativeDelay = phcl.getRelativeDelay( phaseDiff, 
      							   sampleRate,
							   powerPeakIndices,
							   frequencyValues ) ;
		
     Vector<Double> peakFrequencyValues = phcl.getPeakFrequencyValues( powerPeakIndices, frequencyValues ) ;
   
      antBoolVector antReturn ;
      
      Int nOfColumns = relativeDelay.ncolumn() ;
      
      Vector<Bool> antennaReturn ( nOfColumns ) ;

      Int nOfRows = relativeDelay.nrow() ;
      
      Vector<Double> newRelativePhases ( nOfRows ) ;
      
      Vector<Double> newIntRelativePhases ( nOfRows ) ;
      
      Int jumpSteps = sampleJumps.nelements() ;
      
      for ( Int i = 0; i< nOfColumns ; i++ )  {
      
       Double maxDelayDifference = max( relativeDelay.column(i) ) - min( relativeDelay.column(i) ) ;

      Vector<Double> absoluteRelativeDelay = abs( relativeDelay.column ( i ) ) ;
       
       Double meanValue = mean( absoluteRelativeDelay ) ;
       
       Double badness = maxDelayDifference*(1-badnessWeight)+ meanValue*badnessWeight ;
       
       if( badness >=0.1) {                               // loop with comparison of maximum delay difference
       
       Double minBadness = badness ;

       Vector<Double> minRelativePhases = phaseDiff.column( i ) ;

       Double minRelativePosition = 0 ;
    
	   for( Int j = 0; j< jumpSteps; j++ ) {       // for loop with discrete sample jumps
	   
	   Double stepJump = sampleJumps( j ) ;
	   
	   Vector<Double>  jumpPhase ;
	   
	    jumpPhase = peakFrequencyValues *360.*stepJump / sampleRate ;
	   
	    Vector<Double> phaseDiffColumn = phaseDiff.column(i) ;
	    
	    newRelativePhases = phaseDiffColumn - jumpPhase ;
	    
	    Int nOfElements = newRelativePhases.nelements() ;
	    
	    Double newRelativePhase ;
	    
	    for( Int k=0; k<nOfElements; k++) {
	    
	    Int integralPhase = int((newRelativePhases(k))/360 ) ;
	    
	    newRelativePhase = newRelativePhases(k) - integralPhase*360 ;
	    
	    if( newRelativePhase > 180 ) {
	    
	    newRelativePhase = newRelativePhase - 360 ;
	    
	    }
	    
	    if( newRelativePhase < -180 ) {
	    
	    newRelativePhase = newRelativePhase + 360 ;
	    
	    }
	   }						// end of for loop of comaprison
	    
	    Vector<Double> calRelativeDelays;
	    {
		Double tmpfloat = 1/360.*sampleRate;
		calRelativeDelays = newRelativePhase/peakFrequencyValues*tmpfloat;
	    }
	    
	    Double badness = ( max( calRelativeDelays )- min( calRelativeDelays ))*( 1-badnessWeight )+
	    				 mean( abs( calRelativeDelays ))*badnessWeight ;
	   
	    if( minBadness > badness + 0.01 )  {
	    
	       minBadness = badness ;
	       
	       minRelativePhases = newRelativePhases ;
	       	
	       minRelativePosition  = stepJump ;
	       
	       } 
	   }					// end of the for loop for discrete sample jumps
	   
	   phaseDiff.column(i) = minRelativePhases ;
           	    
	   {
	       Double tmpfloat =1/360*sampleRate;
	 relativeDelay.column(i)=phaseDiff.column(i)/peakFrequencyValues*tmpfloat+Double(minRelativePosition) ;
	   }
	   
	  if( minBadness >= 0.15 )  {
	  
	  antReturn = F ;
	  cout<<"Antenna :"<<i<< "is False"<< endl ;
	 }
	  else {
	  
	  antReturn = T ;
	  cout<<"Antenna :"<<i<< "is True"<< endl ;
	   }

	}	  
     }
	
    return antennaReturn ;
     
  }
    catch( AipsError x )  {
    
    cerr << "PhaseCalibration::getAntennaReturn " << x.getMesg () << endl; 
    
    return Vector<Bool> () ;
    }
  }
  
        
// ==============================================================================
//
//  Destruction
//
// ==============================================================================

// PhaseCalibration::~PhaseCalibration ()
// {
//   destroy();
// }
// 
// void PhaseCalibration::destroy ()
// {;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

// PhaseCalibration& PhaseCalibration::operator= (PhaseCalibration const &other)
// {
//   if (this != &other) {
//     destroy ();
//     copy (other);
//   }
//   return *this;
// }
// 
// void PhaseCalibration::copy (PhaseCalibration const &other)
// {;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================



// ==============================================================================
//
//  Methods
//
// ==============================================================================

