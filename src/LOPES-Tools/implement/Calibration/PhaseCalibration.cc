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

#include <Calibration/PhaseCalibration.h>

/*!
  \class PhaseCalibration
*/

namespace LOPES {  // Namespace LOPES -- BEGIN
  
  // ==============================================================================
  //
  //  Construction
  //
  // ==============================================================================
  
  PhaseCalibration::PhaseCalibration ()
  {;}   
  
  PhaseCalibration::PhaseCalibration (const Matrix<DComplex>& spectra,
				      const Matrix<double>& frequencyRanges,
				      const Matrix<double>& expectedPhases,
				      const Matrix<double>& phaseGradient,
				      const Vector<double>& frequencyValues,
				      const Vector<double>& sampleJumps,
				      const int& referenceAntenna,
				      const double& sampleRate,
				      const double& badnessWeight )
    
  { 
    //Matrix<DComplex> gainValues ;
  }
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  PhaseCalibration::~PhaseCalibration ()
  {;}   
  
  Vector<int> PhaseCalibration::getFrequencyIndices (const Matrix<double>& frequencyRanges,
						     const Vector<double>& frequencyValues )
  {
    
    try {
      
      int nOfRows = frequencyRanges.nrow() ;	// no of rows will depend on how many peaks have to be scanned
      
      int nOfColumns = frequencyRanges.ncolumn() ;	// no. of columns will be always 2, i.e., min. and max. 
      // frequency values to scan the power peak
      
      int nOfIndices = nOfColumns*nOfRows ;		// how many indeces have to be scanned in the vector frequencyValues
      
      Vector<int> frequencyIndices( nOfIndices );   
      
      //int nOfElements = frequencyValues.nelements () ;
      
      int k = 0;
      
      int l = 0;
      
      double frequencyValue = 0.0 ;
      
      double frequencyRange = 0.0 ;
      
      for(int i = 0; i< nOfRows; i++)
	
	{
	  for(int j = 0; j< nOfColumns; j++)
	    
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
      return Vector<int> ();
    }
  }
  
  // ------------------------------------------------------- getIndexOfPowerPeaks
  
  Matrix<int> PhaseCalibration::getIndexOfPowerPeaks( const Matrix<DComplex>& spectra,
  			       		 	      const Vector<int>& frequencyIndex,
			       		 	      const Vector<double>& frequencyValues )
    
  {
    
    try {
      
      Matrix<double> absoluteArray( amplitude( spectra) );
      
      int nOfColumns = spectra.ncolumn() ;	         // no. of columns will be always 2, i.e., min. and max. 
      // frequency values to scan the power peak
      
      //int nOfRows = spectra.nrow() ;		// no of rows will depend on how many peaks have to be scanned
      
      int nOfSegments = (frequencyIndex.nelements())/2  ; // No of segments of the input array spectra will depend on
      // the no. of elements in the vector frequencyIndex
      
      Matrix<int> indexOfPowerPeaks( nOfSegments, nOfColumns ) ;
      
      int nOfElements = frequencyIndex.nelements() ;
      
      int n = 0 ;      // Row of the matrix indexOfPowerPeak
      
      int m = 0 ;
      
      int powerPeakIndex = 0 ;
      
      for( int i =0; i< nOfElements ; i = i+2 ) {
	
	int segmentInit = frequencyIndex( i ) ;
	
	int segmentFinal = frequencyIndex( i+1 ) ;
	
	int segmentLength = (segmentFinal-segmentInit + 1) ;
	
	Matrix<double> segmentedMatrix( absoluteArray( Slice( segmentInit, segmentLength ), Slice( 0, nOfColumns ) ) ) ;
	
	int Columns = segmentedMatrix.ncolumn() ;
	
	int Rows = segmentedMatrix.nrow() ;
	
	double elementValue = 0 ;
	
	//   cout<< "Index of Power Peaks :"<< endl ;
	
	for( int k = 0; k < Columns; k++ )
	  {
	    for( int j = 0; j < Rows; j++ )
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
      return Matrix<int> ();
    }
  }
  
  
  Vector<double> PhaseCalibration::getPeakFrequencyValues( const Matrix<int>& powerIndex,
							   const Vector<double>& frequencyValues )
    
  {
    
    try {
      
      int nOfRows = powerIndex.nrow() ;
      
      Vector<double> peakFrequencyValues( nOfRows ) ;
      
      // cout<< "Peak Frequency Values :"<< endl;
      for( int i = 0; i< nOfRows ; i++ )
	{
	  int j = powerIndex( i, 0 );    // peak power frequency index will be same for all antenna so 
	  // calculated here only for one antenna
	  
	  peakFrequencyValues( i ) = frequencyValues( j );
	  
	  cout<<peakFrequencyValues( i )<< "\t" ;
	  
	}
      cout<< endl;
      return peakFrequencyValues ;
      
    }
    catch ( AipsError x ) {
      cerr << " PhaseCalibration::getPeakFrequencyValues " << x.getMesg () << endl ;
      return Vector<double> ();
    }
  }   
  
  
  Matrix<double> PhaseCalibration::getPhaseValues( const Matrix<DComplex>& spectra,
						   const Matrix<int>& powerIndex )
    
  {
    
    try { 
      
      int nOfColumns = powerIndex.ncolumn() ;
      
      int nOfRows = powerIndex.nrow() ;
      
      Matrix<DComplex> sortedArray ( nOfRows, nOfColumns ) ;
      
      int k = 0 ;
      
      	 for( int i = 0; i< nOfRows ; i++ )
	  {
	    for( int r = 0; r < nOfColumns ;  r++ )
             {
	       k = powerIndex(i, r) ;
	     	  
	      sortedArray( i, r ) = spectra( k, r)  ;
	  }
	}
	     
	 Matrix<double> phaseValue( phase(sortedArray));

	return phaseValue ;
    
    }
    
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getPhaseValues " << x.getMesg () << endl ;
    return Matrix<double> ();
   }
 }
 
 
 Matrix<double> PhaseCalibration::getPhaseDiffInDegree( const Matrix<double>& calculatedPhases,
 					               const Matrix<double>& expectedPhases,
						       const Matrix<double>& phaseGradient,
						       const int& referenceAntenna )
 	
 {
 
   try {	 
      
      int nOfColumns = calculatedPhases.ncolumn() ;
      
      int nOfRows = calculatedPhases.nrow() ;
      
      int columns = expectedPhases.ncolumn() ;
      
      Matrix<double> calculatedPhasesInDegree( nOfRows, nOfColumns ) ;   // Phases from Raw FFT
      
      Matrix<double> phaseDiff( nOfRows, nOfColumns ) ; 
 
      Matrix<double> phasesDiff( nOfRows, nOfColumns ) ;

      Matrix<double> residualPhaseGradient( nOfRows, nOfColumns ) ;

      Matrix<double> relativePhaseGradient( nOfRows, nOfColumns ) ;

      Matrix<double> calculatedPhasesDiff( nOfRows, nOfColumns ) ;

       
      if ( nOfColumns == columns ) {
	   
	    int columnNumber = referenceAntenna ;
	     
	 
             Vector<double> referencePhaseGradient = phaseGradient.column( columnNumber ) ;

	     Vector<double> referenceAntennaPhases = calculatedPhasesInDegree.column( columnNumber );

	     
	     for( int i = 0; i< nOfColumns ; i++ ) {

	         for( int j = 0; j< nOfRows ; j++ ) {

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
    return Matrix<double> ();
   }
 }
      
 

 
 Matrix<double> PhaseCalibration::getRelativeDelay( const Matrix<double>& phaseDiff,
 						   const double& sampleRate,
						   const Matrix<int>& powerPeakIndices,
						   const Vector<double>& frequencyValues ) 
 
 {
 
  try {
  
      int nOfColumns = phaseDiff.ncolumn() ;
      
       int nOfRows = phaseDiff.nrow() ; 
      
      Matrix<double> relativeDelay( nOfRows, nOfColumns );
      
      double peakFrequency = 0 ;
      
      for( int i = 0; i< nOfColumns ; i++ ) {
	         
        for( int j = 0; j< nOfRows ; j++ )  {
      
          int m = powerPeakIndices( j, i );
	  
	  peakFrequency = frequencyValues( m ) ;
		 
          relativeDelay ( j,i ) = phaseDiff(j,i)/180.0/(2.0*peakFrequency)*sampleRate ;
	   
          //relativeDelay ( j,i ) = delays ;
           }
	 }
     
     return relativeDelay ;
     
       }
    catch( AipsError x )  {
    
    cerr << "PhaseCalibration::getRelativeDelay " << x.getMesg () << endl; 
    
    return Matrix<double> () ;
    }
 }

  // ----------------------------------------------------------- getAntennaReturn
  
  Vector<bool>
  PhaseCalibration::getAntennaReturn (const Matrix<DComplex>& spectra,
				      const Matrix<double>& frequencyRanges,
				      const Matrix<double>& expectedPhases,
				      const Matrix<double>& phaseGradient,
				      const Vector<double>& frequencyValues,
				      const Vector<double>& sampleJumps,
				      const int& referenceAntenna,
				      const double& sampleRate,
				      const double& badnessWeight)
  {
    try {
      // PhaseCalibration object -> why is this needed here??
      PhaseCalibration phcl;
      // indices for the frequency bands segmentation
      Vector<int> frequencyIndex = phcl.getFrequencyIndices (frequencyRanges, 
      							     frequencyValues);
      Matrix<int> powerPeakIndices = phcl.getIndexOfPowerPeaks (spectra, 
      							        frequencyIndex,
								frequencyValues); 
      Matrix<double> calculatedPhases = phcl.getPhaseValues( spectra,
							     powerPeakIndices);
      Matrix<double> phaseDiff = phcl.getPhaseDiffInDegree( calculatedPhases,
							    expectedPhases,
							    phaseGradient,
							    referenceAntenna ) ;
      Matrix<double> relativeDelay = phcl.getRelativeDelay( phaseDiff, 
							    sampleRate,
							    powerPeakIndices,
							    frequencyValues ) ;
      Vector<double> peakFrequencyValues = phcl.getPeakFrequencyValues (powerPeakIndices, frequencyValues ) ;
      
      // book-keeping for the calculcation below
      antBoolVector antReturn;
      int nOfColumns (relativeDelay.ncolumn());
      int nOfRows (relativeDelay.nrow());
      Vector<bool> antennaReturn (nOfColumns);
      Vector<double> newRelativePhases ( nOfRows ) ;
      Vector<double> newIntRelativePhases (nOfRows);
      int jumpSteps (sampleJumps.nelements());
      double maxDelayDifference (0.0);
      Vector<double> absoluteRelativeDelay (nOfColumns);
      Vector<double> minRelativePhases (phaseDiff.nrow()); // length of a column
      Vector<double> calRelativeDelays;
      double meanValue (0.0);
      double badness (0.0);
      double minBadness (0.0);
      double minRelativePosition (0.0);
      double stepJump (0.0);
      double newRelativePhase (0.0);
      // loop counters
      int j,k;
      
      for ( int i = 0; i< nOfColumns ; i++ )  {
	// maximal difference of delays
	maxDelayDifference = max(relativeDelay.column(i)) - min(relativeDelay.column(i));
	absoluteRelativeDelay = abs( relativeDelay.column ( i ) ) ;
	meanValue = mean (absoluteRelativeDelay);
	badness = maxDelayDifference*(1-badnessWeight)+ meanValue*badnessWeight ;
	
	/*
	  Loop with comparison of maximum delay difference
	*/
	if( badness >=0.1) {
	  
	  minBadness          = badness ;
	  minRelativePosition = 0;
	  minRelativePhases   = phaseDiff.column(i);
	  
	  /*
	    for loop with discrete sample jumps
	  */
	  for(j=0; j< jumpSteps; j++ ) {
	    
	    stepJump = sampleJumps(j);
	    
	    Vector<double>  jumpPhase ;
	    
	    jumpPhase = peakFrequencyValues *360.*stepJump / sampleRate ;
	    
	    Vector<double> phaseDiffColumn = phaseDiff.column(i) ;
	    
	    newRelativePhases = phaseDiffColumn - jumpPhase ;
	    
	    int nOfElements = newRelativePhases.nelements() ;
	    
	    for (k=0; k<nOfElements; k++) {
	      
	      int integralPhase = int((newRelativePhases(k))/360 ) ;
	      
	      newRelativePhase = newRelativePhases(k) - integralPhase*360 ;
	      
	      if( newRelativePhase > 180 ) {
		
		newRelativePhase = newRelativePhase - 360 ;
		
	      }
	      
	      if( newRelativePhase < -180 ) {
		
		newRelativePhase = newRelativePhase + 360 ;
		
	      }
	    }  // -- end of for loop of comparison --
	    
	    {
	      double tmpfloat = 1/360.*sampleRate;
	      calRelativeDelays = newRelativePhase/peakFrequencyValues*tmpfloat;
	    }
	    
	    double badness = ( max( calRelativeDelays )- min( calRelativeDelays ))*( 1-badnessWeight )+
	      mean( abs( calRelativeDelays ))*badnessWeight ;
	    
	    if( minBadness > badness + 0.01 )  {
	      
	      minBadness = badness ;
	      
	      minRelativePhases = newRelativePhases ;
	      
	      minRelativePosition  = stepJump ;
	      
	    } 
	  }					// end of the for loop for discrete sample jumps
	  
	  phaseDiff.column(i) = minRelativePhases ;
          
	  {
	    double tmpfloat =1/360*sampleRate;
	    relativeDelay.column(i)=phaseDiff.column(i)/peakFrequencyValues*tmpfloat+double(minRelativePosition) ;
	  }
	  
	  if( minBadness >= 0.15 )  {
	    
	    antReturn = F ;
	    cout << "Antenna :" << i << "is False" << endl ;
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
      
      return Vector<bool> () ;
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

} // Namespace LOPES -- END
