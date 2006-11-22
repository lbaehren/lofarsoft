/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: tPhaseCalibration.cc,v 1.11 2006/11/02 13:39:35 singh Exp $*/


/*!
  \file tPhaseCalibration.cc

  \ingroup Calibration

  \brief A collection of test routines for PhaseCalibration
 
  \author Kalpana Singh
 
  \date 2006/07/03
*/

// casa header files

#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>
#include <lopes/Calibration/PluginBase.h>
#include <lopes/Calibration/PhaseCalibration.h>

#include <lopes/Data/ITSCapture.h>
#include <lopes/Data/LopesEvent.h>
#include <lopes/IO/DataReader.h>

#include <casa/namespace.h>


// -----------------------------------------------------------------------------
// 
// Global Valriables
//
//------------------------------------------------------------------------------

uint dataBlockSize ( 32768/2 ) ;
Double alpha ( 0.5 ) ;

Vector<String> filenamesVector () 
{
  Vector<String> files( 1 );
  
  files( 0 ) = "/data/LOPES/cr/sel/2004.02.10.01:48:14.776.event"; 
  
  
  //2004.01.12.00:28:11.577.event" ;
 
  //2004.02.10.01:48:14.776.event"; 
  //2004.02.09.18:02:11.922.event"; 

  return files ;
 }
 
 Double frequencyRange[3][2] = { {62165865.6, 62223239.5},
                                {67670000.0, 67712097.0 },
				{67915000.0, 67947000.0 } };
				 
	
 Double expectedPhase[3][10] = { {0., 118., 4.8, 99.6, 172.4, -66., -113.6, 49., -138.1, 5.1},
                                 {0., 112., -143., -73., -14., -136., -151., -25., 11.4, -1.2},
				 {0., 122., -97., -42., 18.7, -113., -140., -14., 140.6, 29.9} } ;
				 
Double sampleJump[6]={ -2, 2, -1, 1, -3, 3 } ;

Double Grad[3][10] = { {-919.812673, -861.727128, -940.460636, -984.350944, -1459.81119, -1476.3772, -589.239534, -695.093165, -631.581104, -1188.81975},
                      {-1000.96359, -937.753418, -1023.43323, -1071.19578, -1588.60373, -1606.63129, -641.225475, -756.418093, -687.30265, -1293.70394},
                      {-1004.61041, -941.169949, -1027.16192, -1075.09848, -1594.39151, -1612.48476, -643.561662,-759.173963, -689.806711, -1298.41731} };
			 

Vector<Double> frequencyValues(dataBlockSize+1, 0.0);

Matrix<Double> frequencyRanges( 3, 2, 0.0 ) ;

Matrix<Double> expectedPhases( 3, 10, 0.0 );

Matrix<Double> phaseGradient( 3, 10, 0.0 ) ;

Vector<Double> sampleJumps(6, 0);
   
 /*!
  \brief Test constructors for a new PhaseCalibration object

  \return nofFailedTests -- The number of failed tests.
*/

Int test_PhaseCalibrations ()
{
  Int nofFailedTests (0);
  
  try {
  
  PhaseCalibration phcl ; 
  
  } catch ( AipsError x) {
  
  cerr<< "test_PhaseCalibration :- Testing default constructor ..." << x.getMesg () << endl;
  }
    return nofFailedTests;
}

Bool test_PhaseCalibration ()
{
 Bool ok ( True ) ;
 
 try {
 
cout << " - getting filenames" << endl; 

Vector<String> filenames = filenamesVector();

cout << " - setting up DataReader ..." << endl;
cout << " -- filename  = " << filenames(0) << endl; 
cout << " -- blocksize = " << dataBlockSize <<endl;

DataReader *dr ;
LopesEvent *anEvent = new LopesEvent( filenames(0), 
 			               dataBlockSize );

dr = anEvent ;
dr->setHanningFilter (alpha );

cout << " - getting FFT data" << endl; 

Matrix<DComplex> spectra (dr->fft());
 
cout << " - extracting phases ..." << endl;

Matrix<Double> phaseArray( phase( spectra ) ) ;
 
uInt row = phaseArray.nrow () ;
 
uInt column = phaseArray.ncolumn () ;
 
cout << "Number of Rows : " << row << "\n"
      << "Number of Columns : " << column << endl;
      
ofstream logfile1p ;
 
logfile1p.open( "rawdata", ios::out ) ;
 
 for( uInt sample(0); sample< row ; sample++ ) {
    for( uInt antenna(0); antenna< column; antenna++ ) {
       logfile1p << phaseArray( sample, antenna )
                 << "\t" ;
     }
    logfile1p << endl ;
  }
logfile1p.close();
 
 PhaseCalibration phcl ;
 
 for( Int i=0; i< 3; i++ ) {
   for( Int j =0; j<2; j++ ) {
      frequencyRanges(i,j) = frequencyRange[i][j] ;
   }
  }
  
 for( Int m =0; m<3; m++ ) {
     for( Int n =0; n<10; n++ ){
	 expectedPhases(m,n) = expectedPhase[m][n];
     }
 }
       
 for( Int k=0; k<3; k++){
     for( Int l=0; l<10; l++){
	 phaseGradient(k,l) = Grad[k][l];
     }
 }

Double interval = 0.0 ;
 
for( Int k =0; k< row ; k++ ){

   interval = (80-40)*1000000/row ;

    frequencyValues(k)= 40*1000000 + interval*k ;

    }
//cout<< " Frequency Values :" <<frequencyValues<< endl;

 for( Int k=0; k<6; k++){
   	 sampleJumps(k) = sampleJump[k];
     }
 

Double sampleRate = 80000000 ;

 Int referenceAntenna = 0 ;

Double badnessWeight = 0.5 ;

//Double badnessParameter = 0.1 ;
 
 /* 

Vector<Int> frequencyIndex( phcl.getFrequencyIndices ( frequencyRanges,
                                                        frequencyValues ));

Matrix<Int> powerPeakIndices( phcl.getIndexOfPowerPeaks( spectra,
  			       		 	          frequencyIndex,
                                                          frequencyValues ) ); 

Vector<Double> peakFrequencyValues( phcl.getPeakFrequencyValues( powerPeakIndices,
								 frequencyValues ) );		        

Matrix<Double> phaseValues ( phcl.getPhaseValues( spectra,
						  powerPeakIndices )) ;


/*
Matrix<Double> residualPhases ( phcl.getResidualPhasesInDegree ( phaseValues,
							        expectedPhases,
								phaseGradient,
							        referenceAntenna ) ) ;

 Int Rows = phaseValues.nrow();
cout<< "Number of Rows in the residual matrix array:"<< Rows<< endl;
 Int Columns = phaseValues.ncolumn() ;
cout<<"NUmber of columns in the residual matrix array:"<<Columns<< endl;

cout<< "Phase Value of the raw data in degrees:"<< endl; 
for(Int i=0; i<Columns; i++){
     for(Int j=0; j<Rows; j++){

    Double phaseValue = phaseValues(j,i)*180/3.1416 ;

    cout<< phaseValue <<"\t"<<"\t" ;
     }
     cout<< endl ;
 }


Matrix<Double> relativeDelay( phcl.getRelativeDelay(  residualPhases,
 						      sampleRate,
						      powerPeakIndices,
						      frequencyValues ) ) ;

*/
Vector<Bool> AntennaReturn =(phcl.getAntennaReturn( spectra,
  		     				    frequencyRanges,
		     				    expectedPhases,
						    phaseGradient,
		     			  	    frequencyValues,
		     				    sampleJumps,
		     				    referenceAntenna,
		     				    sampleRate,
						    badnessWeight ) ); 

/*
    Vector<Int> frequencyIndex = phcl.getFrequencyIndices( frequencyRanges, 
      							     frequencyValues );
     cout<< "Frequency Indices: " << frequencyIndex << endl ;
     
     
     
   Matrix<Int> powerPeakIndices = phcl.getIndexOfPowerPeaks( spectra, 
      							        frequencyIndex,
								frequencyValues ) ; 
  
   Vector<Double> peakFrequencyValues = phcl.getPeakFrequencyValues( powerPeakIndices,
								 frequencyValues ) ;	

  cout<< " Peak Frequency Values :" << peakFrequencyValues << endl ;	        

    Matrix<Double> calculatedPhases = phcl.getPhaseValues( spectra,
      							    powerPeakIndices ) ;
  
  //    Matrix<Double> residualPhasesInDegree = phcl.getResidualPhasesInDegree( calculatedPhases,
  //   						            	             expectedPhases,
//									     phaseGradient,
//							                     referenceAntenna ) ;
          
      Int nOfColumns = calculatedPhases.ncolumn() ;
      
      Int nOfRows = calculatedPhases.nrow() ;
      
      Int columns = expectedPhases.ncolumn() ;
      
     Matrix<Double> calculatedPhasesInDegree( nOfRows, nOfColumns ) ;   // Phases from Raw FFT
      
      Matrix<Double> residualPhasesInDegree( nOfRows, nOfColumns ) ; 
 
      Matrix<Double> phaseDiff( nOfRows, nOfColumns ) ;

      Matrix<Double> residualPhaseGradient( nOfRows, nOfColumns ) ;
    
     Matrix<Double> relativePhaseGradient (nOfRows, nOfColumns);
     Matrix<Double> calculatedPhasesDiff(nOfRows, nOfColumns);
       
      if ( nOfColumns == columns ) {
	   
	    Int columnNumber = referenceAntenna ;
	     
	    Vector<Double> referencePhaseGradient = phaseGradient.column( columnNumber ) ;
	    
	    Vector<Double> referenceAntennaPhases = calculatedPhasesInDegree.column( columnNumber ) ;

                
	     for( Int i = 0; i< nOfColumns ; i++ )
	       {
	       cout<< "Antenna  :" <<i <<"\n"<< endl;
	       
	         for( Int j = 0; j< nOfRows ; j++ )
		 {
		 
		
	       calculatedPhasesInDegree(j,i) = calculatedPhases(j, i )*(180/3.1416) ;
               cout<<"Phases"<<calculatedPhasesInDegree(j,i)<< "\t";
	       
	        calculatedPhasesDiff(j,i)= calculatedPhasesInDegree(j,i) - referenceAntennaPhases(j);
		cout<<"Phase Diff"<<i<<"\t"<<calculatedPhasesDiff(j,i)<< "\t";
		
	       relativePhaseGradient(j,i) = phaseGradient(j,i ) - referencePhaseGradient(j)  ;

               residualPhaseGradient( j,i ) = expectedPhases ( j,i ) - relativePhaseGradient(j,i) ;
	      cout<<"NEP "<<residualPhaseGradient( j,i )<<"\t";
	      
               phaseDiff(j,i) = calculatedPhasesDiff(j,i) - residualPhaseGradient(j,i ) ;
             //  cout<< " Final Phase Diff :" << phaseDiff(j, i) << "for antenna "<<i<<"\n"<< endl ;
           
	      if( phaseDiff( j, i) > 180.0 ) {
		 
		    phaseDiff( j, i) = phaseDiff( j, i) - 360  ;
		    
		   }
		    
		 if( phaseDiff( j, i) < -180.0 ) {
		 
		      phaseDiff( j, i) = phaseDiff( j, i) + 360  ;
		    
		     }
		   if( phaseDiff( j, i) > 180.0 ) {
		 
		    phaseDiff( j, i) = phaseDiff( j, i) - 360  ;
		    
		   }
		    
		 if( phaseDiff( j, i) < -180.0 ) {
		 
		      phaseDiff( j, i) = phaseDiff( j, i) + 360  ;
		    
		     }
         cout<< " Final Phase Diff :" << phaseDiff(j, i) <<"\t" ;
	 //   cout<< " Relative Phases :" << phaseDiff(j, i) << "for antenna "<<i<<"\n"<< "\n"<<endl ;

		 }  	// end of the inner for lk
		 cout<<"\n"<<"\n";
		}
	   }
      else
	    {
	       cerr << "[ Error: PhaseCalibration::getResidualPhases ]"
	            << " Arguments of two phase matrices are of incompatible dimensions."
	            << endl;
	    }
	    
	    //return residualPhasesInDegree ;
      //  Matrix<Double> residualDegree = phcl.getResidualDegree( residualPhases ) ;
      
      Matrix<Double> relativeDelay = phcl.getRelativeDelay( phaseDiff, 
      							   sampleRate,
							   powerPeakIndices,
							   frequencyValues ) ;
	Int nOfColumn = relativeDelay.ncolumn() ;
	
	Int nOfRow = relativeDelay.nrow() ;
	
	for(Int h=0; h<nOfRow; h++){
	  for(Int g=0; g<nOfColumn; g++) {
	  
	  cout<< "Relative Delay: "<< relativeDelay(h,g)<<"\n";
	  }
	  cout<< "\n"<<endl;
	 }
	 
     
     
//     Vector<Double> peakFrequencyValues = phcl.getPeakFrequencyValues( powerPeakIndices, frequencyValues ) ;
   
   //   Vector<Bool> antennaReturn ( nOfColumn ) ;

      Vector<Double> newRelativePhases ( nOfRow ) ;
      
      Vector<Int> newIntRelativePhases ( nOfRow ) ;
      
      Int jumpSteps = sampleJumps.nelements() ;
      
      
      for ( Int i = 0; i< nOfColumn ; i++ )  {
      
      Double maxDelayDifference = max( relativeDelay.column(i) ) - min( relativeDelay.column(i) ) ;

      Vector<Double> absoluteRelativeDelay = abs( relativeDelay.column ( i ) ) ;
       
       Double meanValue = mean( absoluteRelativeDelay ) ;
       
       Double badness = maxDelayDifference*(1-badnessWeight)+ meanValue*badnessWeight ;
       
       cout<< "Badness parameter for if loop :" << badness<< endl;
       
       if( badness >=0.1) {                               // loop with comparison of maximum delay difference
       
       Double minBadness = badness ;
                

       Vector<Double> minRelativePhases = phaseDiff.column( i ) ;

       Double minRelativePosition = 0 ;
    
       for( Int j = 0; j< jumpSteps; j++ ) {       // for loop with discrete sample jumps
	   
	   Double stepJump = sampleJumps( j ) ;
	   
	   cout<< "Jump Step: "<<stepJump<< endl;
	   
	   Vector<Double>  jumpPhase;
	    
	   jumpPhase = peakFrequencyValues*360.*stepJump/sampleRate  ;
	   
	   cout<<"Jump in phase value : "<< jumpPhase<< peakFrequencyValues << stepJump << sampleRate<< endl ;
	    
	   
	   Vector<Double> phaseDiffColumn = phaseDiff.column(i ) ;
	   
	   newRelativePhases = phaseDiffColumn - jumpPhase ;
	    
	   cout<<"New Relative Phases as Double :"<< newRelativePhases<< endl ;
	   
	    
	   	    
	    Int nOfElements = newRelativePhases.nelements () ;
	    
	    for( Int k = 0; k< nOfElements; k++ ) {		//for loop for comparison 
	    
	    
	    Int integralPhase = int((newRelativePhases(k))/360);
	    
	    cout<< "Integral Phase Value: " <<integralPhase << endl ;
	    
	   Int newRelativePhase = newRelativePhases(k) - integralPhase*360 ;
	    
	     
	    if( newRelativePhase > 180 ) {
	    
	    newRelativePhase = newRelativePhase - 360 ;
	    
	    }
	    
	    if( newRelativePhase < -180 ) {
	    
	    newRelativePhase = newRelativePhase + 360 ;
	    
	    }
	     cout<< "New relative Phases with jump steps : "<< newRelativePhase << "\n"<<endl ;
	   }	
	  // cout<< "New relative Phases with jump steps : "<< newRelativePhases<< "\n"<<endl ;
	    
	    	    
            Vector<Double> calRelativeDelays;
	    {
		Double tmpfloat = 1/360.*sampleRate ;
		calRelativeDelays = newRelativePhases/peakFrequencyValues*tmpfloat;
		
		cout<< " New Relative delays after implimenting step jumps: "<<calRelativeDelays<< endl;
	    }
	    
	    Double badness = ( max( calRelativeDelays )- min( calRelativeDelays ))*( 1-badnessWeight )
	    				+ mean( abs( calRelativeDelays ))*badnessWeight ;
	   
	    cout<< "Badness after jump steps : "<< badness<< endl;
	    
	    if( minBadness > badness + 0.01 )  {
	    
	       minBadness = badness ;
	       
	       minRelativePhases = newRelativePhases ;
	       	
	       minRelativePosition = stepJump ;
	       
	       } 
	   }					// end of the for loop for discrete sample jumps
	   
	  phaseDiff.column(i) = minRelativePhases ;
           	    
	   {
	       Double tmpfloat =1/360*sampleRate;
	 relativeDelay.column(i)=phaseDiff.column(i)/peakFrequencyValues*tmpfloat+Double(minRelativePosition) ;
	   }
	   
	   cout<<" Minimum badness :" <<minBadness<< "\n"<<endl;
	
	  if( minBadness >= 0.1 )  {
	  
//	  antReturn = F ;
	  cout<<"Antenna :"<<i<< " is False"<< "\n"<<"\n"<<endl ;
	  }
   
         else {
	 
//	  antReturn = T ;
	  cout<<"Antenna :"<<i<< " is True"<< "\n"<<"\n"<< endl ;

	  }
      
        }	  
     
	cout<<"\n"<<"\n"<< endl ;
       }
*/	
}

 catch ( AipsError x ) {
 
 cerr<< " Bool test_PhaseCalibration " << x.getMesg() << endl ;
 ok =False ;
 }
 
 return ok ;
 }
 
 
 
 
// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_PhaseCalibration ();
  }

  return nofFailedTests;
}
