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


/* $Id$*/

#include <Calibration/PhaseCalibration.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

PhaseCalibration::PhaseCalibration ()
{;}   

PhaseCalibration::PhaseCalibration( const Matrix<DComplex>& spectra,
				    const Matrix<Double>& frequencyRanges,
				    const Matrix<Double>& expectedPhases,
				    const Matrix<Double>& phaseGrad,
				    const Vector<Double>& frequencyValues,
				    const Vector<Double>& sampleJumps,
				    const Int& referenceAntenna,
				    const Double& sampleRate,
				    const Double& badnessWeight,
				    const Double& badnessThreshold)
{ 
  ;//Matrix<DComplex> gainValues ;
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

PhaseCalibration :: ~PhaseCalibration ()
{;}   


// ==============================================================================
//
//  Methods
//
// ==============================================================================


Vector<Int> PhaseCalibration::getFrequencyIndices( const Matrix<Double>& frequencyRanges,
				                   const Vector<Double>& frequencyValues )
{
  
  try {
    // no of rows will depend on how many peaks have to be scanned
    uint nOfRows = frequencyRanges.nrow() ;
    // no. of columns will be always 2, i.e., min. and max. 
    // frequency values to scan the power peak
    uint nOfColumns = frequencyRanges.ncolumn() ;
    // how many indeces have to be scanned in the vector frequencyValues
    uint nOfIndices = nOfColumns*nOfRows ;
    
    Vector<Int> frequencyIndices( nOfIndices );   
    
    //Int nOfElements = frequencyValues.nelements () ;
    
    uint k = 0;
    uint l = 0;
    Float frequencyValue = 0.0 ;
    Float frequencyRange = 0.0 ;
    
    for(uint i=0; i< nOfRows; i++) {
      for(uint j=0; j< nOfColumns; j++) {
	do{
	  
	  frequencyValue = frequencyValues (k);
	  // cout<<" Frequency Value" << frequencyValue<< "\t";
	  frequencyRange = frequencyRanges (i,j);
	  //  cout << " FRequency Range" << frequencyRange << "\t"<<"\t" ;
	  k = k+1 ;
	  
	} while ( frequencyValue < frequencyRange ) ;
	//  cout<<endl;
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
					            const Vector<Double>& frequencyValues)
 {
   Int Columns (0);
   Int Rows (0);
   Int segmentInit (0);
   Int segmentFinal (0);
   Int segmentLength (0);
   Double elementValue (0.0);
 
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
   
   for(int i=0; i< nOfElements ; i = i+2 ) {
     
     segmentInit   = frequencyIndex( i );
     segmentFinal  = frequencyIndex( i+1 ) ;
     segmentLength = (segmentFinal-segmentInit + 1) ;
     
      Matrix<Double> segmentedMatrix( absoluteArray( Slice( segmentInit, segmentLength ), Slice( 0, nOfColumns ) ) ) ;
      
      Columns      = segmentedMatrix.ncolumn();
      Rows         = segmentedMatrix.nrow() ;
      elementValue = 0.0 ;
      
      
   // cout<< "Index of Power Peaks :"<< endl ;

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
	   
	indexOfPowerPeaks ( n, k ) = powerPeakIndex  ;	

//	cout<<"indexOfPowerPeaks  :"<<indexOfPowerPeaks( n,k )<<"\t";
		
	}
	n++ ;	
     }
  

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
  Int i(0), j(0);
  Int nOfRows = powerIndex.nrow() ;
  
  try {
      
   // cout << " Number of rows in power index matrix :" << nOfRows << endl ;
    
    Vector<Double> peakFrequencyValues( nOfRows ) ;
    
     for( i = 0; i< nOfRows ; i++ )
      {
       j = powerIndex( i, 0 );    // peak power frequency index will be same for all antenna so 
					// calculated here only for one antenna
					
	peakFrequencyValues( i ) = frequencyValues( j );
    
// 	cout<< " Peak frequency values : " << peakFrequencyValues( i )<< "\t" ;
	
	}
    // cout<< endl;
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
    
    // cout << " number of columns in power index : " << nOfColumns << endl ;
    Int nOfRows = powerIndex.nrow() ;
    
    Matrix<DComplex> sortedArray ( nOfRows, nOfColumns ) ;
    
    Int k = 0 ;
    
    //	cout << " element of sorted array :"<< endl ;
    
    for( Int i = 0; i< nOfRows ; i++ )
      {
	for( Int r = 0; r < nOfColumns ;  r++ )
	  {
	    k = powerIndex(i, r) ;
	    
	    sortedArray( i, r ) = spectra( k, r)  ;
	   } 
	}
	     
	 Matrix<Double> phaseValue( phase(sortedArray));
      
//    cout << "Phase values at max. amplitudes : " <<phaseValue << endl;
	
      return phaseValue ;
    }
    
  catch ( AipsError x ) {
    cerr << " PhaseCalibration::getPhaseValues " << x.getMesg () << endl ;
    return Matrix<Double> ();
  }
 }
 
 
 Matrix<Double> PhaseCalibration::getPhaseDiffInDegree( const Matrix<Double>& calculatedPhases,
 					                const Matrix<Double>& expectedPhases,
							const Matrix<Double>& phaseGrad,
						        const Int& referenceAntenna )
{
   try {	 
      
      Int nOfColumns = calculatedPhases.ncolumn() ;
         
      Int nOfRows = calculatedPhases.nrow() ;
    
      Int columns = expectedPhases.ncolumn() ;
    
      Matrix<Double> calculatedPhasesInDegree( nOfRows, nOfColumns ) ;   // Phases from Raw FFT
       
      Matrix<Double> phaseDiff( nOfRows, nOfColumns ) ; 

      Matrix<Double> phasesDiff( nOfRows, nOfColumns );
 
      Matrix<Double> residualPhaseGradient( nOfRows, nOfColumns ) ;
     
     Matrix<Double> relativePhaseGradient( nOfRows, nOfColumns ) ;
     
     Matrix<Double> calculatedPhasesDiff( nOfRows, nOfColumns ) ;

     //Matrix<Double> calculatedPhaseDiff( nOfRows, nOfColumns ) ;
     
      if ( nOfColumns == columns ) {
	   
	    Int columnNumber = referenceAntenna ;
	    
	    Vector<Double> referencePhaseGradient = phaseGrad.column(columnNumber) ;
	    
	    
	     
	   Vector<Double> referenceAntennaPhases = calculatedPhasesInDegree.column( columnNumber );

	     
	     for( Int i = 0; i< nOfColumns ; i++ ) {

	        for( Int j = 0; j< nOfRows ; j++ ) {
		
		calculatedPhasesInDegree(j,i) = calculatedPhases(j,i)*(180/3.1416) ;		
		
// 	       cout<< "Calculated phases in degree :	" <<calculatedPhasesInDegree(j,i)<< endl;
             
              calculatedPhasesDiff(j,i) = calculatedPhasesInDegree(j,i)-referenceAntennaPhases(j);
 
              relativePhaseGradient(j,i) =  phaseGrad(j,i) - referencePhaseGradient(j);

               residualPhaseGradient(j,i) = expectedPhases(j,i) - relativePhaseGradient(j,i);

              phasesDiff(j,i) = calculatedPhasesDiff(j,i) - residualPhaseGradient(j,i);
		
		if( phasesDiff(j,i) > 180.0 ){
		
		phasesDiff(j,i) = phasesDiff(j,i) - 360 ;
		
		}
		if( phasesDiff(j,i) < -180.0){
		
		phasesDiff(j,i) = phasesDiff(j,i) + 360 ;
		
		}
               }		
             }
	   }
      else
	    {
	       cerr << "[ Error: PhaseCalibration::getPhaseDiffInDegree ]"
	            << " Arguments of two phase matrices are of incompatible dimensions."
	            << endl;
	    }

         
	 return phasesDiff  ;
    
    }
    catch ( AipsError x ) {
    cerr << " PhaseCalibration::getPhaseDiffInDegree " << x.getMesg () << endl ;
    return Matrix<Double> ();
   }
 }
      
 

 
Matrix<Double>
PhaseCalibration::getRelativeDelay (const Matrix<Double>& phaseDiff,
				    const Double& sampleRate,
				    const Matrix<Int>& powerPeakIndices,
				    const Vector<Double>& frequencyValues ) 
 {
   try {
    
    Int nOfColumns = phaseDiff.ncolumn() ;
    
    Int nOfRows = phaseDiff.nrow() ; 
    
    Matrix<Double> relativeDelay( nOfRows, nOfColumns );
    
    Double peakFrequency = 0 ;
    
    for(int i=0; i< nOfColumns ; i++ ) {
      
      for(int j=0; j< nOfRows ; j++ )  {
	
	Int m = powerPeakIndices( j, i );
	
	peakFrequency = frequencyValues( m ) ;
	
	relativeDelay ( j,i ) = phaseDiff(j,i)/180.0/(2.0*peakFrequency)*sampleRate ;
     }
   }
    
    return relativeDelay ;
    
  }
  catch( AipsError x )  {
    
    cerr << "PhaseCalibration::getRelativeDelay " << x.getMesg () << endl; 
    
    return Matrix<Double> () ;
  }
 }


 
Vector<Bool> PhaseCalibration::getAntennaReturn (  Matrix<DComplex> const &spectra,
						   Matrix<Double> const &frequencyRanges,
						   Matrix<Double> const &expectedPhases,
						   Matrix<Double> const &phaseGrad,
						   Vector<Double> const &frequencyValues,
						   Vector<Double> const &sampleJumps,
						   Int const &referenceAntenna,
						   Double const &sampleRate,
						   Double const &badnessWeight,
						   Double const &badnessThreshold, 
						   Vector<Double> *newCorrDelays )
  
						    
						  
 {
   try {
   
      PhaseCalibration phcl ;
      
      Vector<Int> frequencyIndex = phcl.getFrequencyIndices( frequencyRanges, 
      							     frequencyValues );
      
      cout << "frequencyIndex :" <<frequencyIndex << endl;
      Matrix<Int> powerPeakIndices = phcl.getIndexOfPowerPeaks( spectra, 
      							        frequencyIndex,
								frequencyValues ) ; 
       cout << "powerPeakIndices :" <<powerPeakIndices << endl;
      Matrix<Double> calculatedPhases = phcl.getPhaseValues( spectra,
      							    powerPeakIndices ) ;
      cout << "Phases :" <<calculatedPhases << endl;
     Matrix<Double> residualPhasesInDegree = phcl.getPhaseDiffInDegree( calculatedPhases,
      						                        expectedPhases,
							                phaseGrad,
                                                                        referenceAntenna ) ;
      cout << "residualPhasesInDegree :" <<residualPhasesInDegree << endl;
      
        
      Matrix<Double> relativeDelay = phcl.getRelativeDelay( residualPhasesInDegree, 
      							    sampleRate,
							    powerPeakIndices,
							    frequencyValues ) ;
      cout << " actual delays " << relativeDelay << endl ;
      
   Vector<Double> peakFrequencyValues = phcl.getPeakFrequencyValues( powerPeakIndices, frequencyValues ) ;
    
    antBoolVector antReturn ;
    
    Int nOfColumns = relativeDelay.ncolumn() ;
    
    Vector<Bool> antennaReturn ( nOfColumns ) ;
    
    Int nOfRows = relativeDelay.nrow() ;
    
    Matrix<Double> newResidualPhasesInDegree (nOfRows, nOfColumns, 0.0 );
    
    Matrix<Double> newRelativeDelay (nOfRows, nOfColumns, 0.0 );
    
    Vector<Double> newRelativePhases ( nOfRows ) ;
    
    Vector<Double> newIntRelativePhases ( nOfRows ) ;
    
    Int jumpSteps = sampleJumps.nelements() ;
     
     for (int i=0; i< nOfColumns ; i++ )  {
      
     Double maxDelayDifference = max( relativeDelay.column(i) ) - min( relativeDelay.column(i) ) ;
      
     Vector<Double> absoluteRelativeDelay = abs( relativeDelay.column ( i ) ) ;
    
     Double meanValue = mean( absoluteRelativeDelay ) ;
      
     Double badness = maxDelayDifference*(1-badnessWeight)+ meanValue*badnessWeight ;
      
    if( badness >= 0.1 ){
      
     Double minBadness = badness ;
       
     Vector<Double> minRelativePhases = residualPhasesInDegree.column(i) ;
     
     Double minRelativePosition = 0. ;
    
    for( Int j = 0; j< jumpSteps; j++ ) {       // for loop with discrete sample jumps
	 
	 Double stepJump = sampleJumps( j ) ;
	 
	 Vector<Double>  jumpPhase ;
	 
	 jumpPhase = peakFrequencyValues *360.*stepJump / sampleRate ;
	 
         Vector<Double> phaseDiffColumn = residualPhasesInDegree.column(i) ;
	 
	 newRelativePhases = phaseDiffColumn - jumpPhase ;
	 
	 Int nOfElements = newRelativePhases.nelements() ;
	 
	 Vector<Double> newRelativePhase(nOfElements) ;
	 
	 for(int k=0; k<nOfElements; k++) {
	   
	   Int integralPhase = int((newRelativePhases(k))/360. ) ;
	   
	  newRelativePhase(k) = newRelativePhases(k) - integralPhase*360 ;
	    
	   if ( newRelativePhase(k) >180 ){
	      
	      newRelativePhase(k) = newRelativePhase(k) - 360 ;
	   }
	   
	    if (newRelativePhase(k) < - 180){
	      
	      newRelativePhase(k) = newRelativePhase(k) + 360 ;
	   }
	  
           }    
	  
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
	   }
	
	   newResidualPhasesInDegree.column(i) = minRelativePhases ;
       	  
	  { 
	  Double tmpfloat =1/360.*sampleRate;
	 
        newRelativeDelay.column(i)= newResidualPhasesInDegree.column(i)/peakFrequencyValues*tmpfloat+Double(minRelativePosition) ;
// 	 cout << "relative delay : " <<  newRelativeDelay.column(i) << " for antenna " << i+1 << endl ;
	   }
	  
	  if( minBadness >= badnessThreshold )  {
	  
	  antReturn = F ;
	  cout<<"Antenna :"<<i+1<< "is False"<< endl ;
	 }
	  else {
	  
	  antReturn = T ;
	  cout<<"Antenna :"<<i+1<< "is True"<< endl ;
	   }

	}
	else{
	cout << "Antenna : " << i+1 << " is True " << endl ;
	antReturn = T ;
	}
     }

      uint NOFCOLUMNS = newResidualPhasesInDegree.ncolumn() ;
      
      uint NOFROWS = newResidualPhasesInDegree.nrow() ;
       
       Matrix<Double> Change(NOFROWS, NOFCOLUMNS,0.0);
       
       Vector <Double> meanChange(NOFCOLUMNS, 0.0 ) ;
       
       Vector<Double> actualCorr(NOFCOLUMNS);
       
       actualCorr(0) = 0.00 ;
       actualCorr(1) = 0.19 ;
       actualCorr(2) = -0.11 ;
       actualCorr(3) = -0.23 ;
       actualCorr(4) = -1.95 ;
       actualCorr(5) = -2.00 ;
       actualCorr(6) = 1.14 ;
       actualCorr(7) = 0.74 ;
       actualCorr(8) = 1.02 ;
       actualCorr(9)= -0.99 ;
       
       Vector<Double> newCorrection (NOFCOLUMNS, 0.0 ) ;
       
       for(uint r = 0; r < NOFCOLUMNS; r++ ){
       
       for( uint s = 0; s < NOFROWS; s++ ){
       
      Change(s,r)  =  relativeDelay(s,r) - newRelativeDelay(s,r) ;
      
       }
      
      meanChange(r) = mean(Change.column(r));
      
      newCorrection(r) = actualCorr(r) -  meanChange(r) ;
      }
      
           
      newCorrDelays = &newCorrection ;
      cout << " Change in sample Delays " << *newCorrDelays << endl ;
    
     return antennaReturn ;
     
     }
  catch( AipsError x )  {
    
    cerr << "PhaseCalibration::getAntennaReturn " << x.getMesg () << endl; 
    
    return Vector<Bool> () ;
  }
}

//=============================================================
    
