/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Kalpana Singh (<mail>)                                                *
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

#include <Analysis/NuMoonTrigger.h>

#include <crtools.h>
#include <crtools.h>
#include <Data/LOFAR_TBB.h>
#include <IO/DataReader.h>

// Namespace usage
using CR::NuMoonTrigger;

using namespace std;
/*!
  \file tNuMoonTrigger.cc

  \ingroup Analysis

  \brief A collection of test routines for the NuMoonTrigger class
 
  \author Kalpana Singh
 
  \date 2009/04/21
*/
Matrix<Double> posit_x(10,10,0.0) ;

Double posi_x[10] = { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};

//std::string const &filename =/mnt/lofar/CS1_tbb/rw_20090417_181700.h5

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new NuMoonTrigger object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors (std::string const &filename,
		       unsigned int const  &n_samples=139264)
{
  std::cout << "\n[tNuMoonTrigger::test_constructors]\n" << std::endl;
  
  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    NuMoonTrigger numooontrigger;
    
    CR::LOFAR_TBB lofar_tbb( filename, n_samples ) ;
    
    lofar_tbb.DataReader::setNyquistZone(2);
    
    casa::Matrix<double> data = lofar_tbb.fx( ) ;	
    ofstream logfile11;
    
    logfile11.open( "antenna_0.dat", ios::out );
    for( uint nSample=0; nSample < n_samples ; nSample++ ){
      
      logfile11 << data(nSample,0) << endl ;
    }
    
    logfile11.close() ;
    
    uint nofSamples =  data.nrow () ;
    uint n_columns  = data.ncolumn () ;
    
    cout << "number of samples : " << nofSamples << "number of columns :" << n_columns << endl ;
    
//     double simTEC        = 10.0 ;
    double sampling_rate = 200e6 ;
    uint nyquist_zone    = 2 ;
//     uint time_int_bins   = 5 ;
    uint n_frames        = 136 ;
//     double TEC           = 10.0 ;
//     double peak_power    = 49.0 ;
    
    casa::Vector<double> ppfcoeff(n_samples,0.0) ;
    casa::Vector<double> ppfcoeff_inv(n_samples, 0.0) ;
    casa::Vector<double> antenna_0 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_1 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_2 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_3 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_4 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_5 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_6 (n_samples, 0.0 ) ;
    casa::Vector<double> antenna_7 (n_samples, 0.0 ) ;
    
    readAsciiVector( ppfcoeff,"Coeffs16384Kaiser-quant.dat" ) ;
    
    readAsciiVector( ppfcoeff_inv,"ppf_inv.dat" ) ;
    
    casa::Vector<double> freq_range(2,0.0) ;
    
    casa::Vector<uint> RCU_id(8,0) ;
    
    freq_range(0)= 130e6 ;
    
    freq_range(1) = 180e6 ;
    
    RCU_id(0) = 0;
    RCU_id(1) = 1;
    RCU_id(2) = 2;
    RCU_id(3) = 3;
    RCU_id(4) = 4;
    RCU_id(5) = 5;
    RCU_id(6) = 6;
    RCU_id(7) = 7;
    
//     double pi          = 3.14159265358979323846 ;
//     uint dataBlockSize = 1024 ;
//     uint nOfSegments   = 5 ;
//     double peak_height = 500. ;
//     double pulse_loc   = 600. ;
//     double rnd_phase   = 0.2*pi ;

    casa::Vector<double>freq_Vector = numooontrigger.freq_vector(  sampling_rate,
								   nyquist_zone) ;
    
    readAsciiVector( antenna_0,"antenna_0.dat" ) ;
    readAsciiVector( antenna_1,"antenna_1.dat" ) ;
    readAsciiVector( antenna_2,"antenna_2.dat" ) ;
    readAsciiVector( antenna_3,"antenna_3.dat" ) ;
    readAsciiVector( antenna_4,"antenna_4.dat" ) ;
    readAsciiVector( antenna_5,"antenna_5.dat" ) ;
    readAsciiVector( antenna_6,"antenna_6.dat" ) ;
    readAsciiVector( antenna_7,"antenna_7.dat" ) ;
    
    uint n_rcu =8 ;
    
    casa::Matrix<double> raw_data(n_samples, n_rcu, 0.0 ) ;
    
    raw_data.column(0) = antenna_0 ;
    raw_data.column(1) = antenna_1 ;
    raw_data.column(2) = antenna_2 ;	
    raw_data.column(3) = antenna_3 ;
    raw_data.column(4) = antenna_4 ;
    raw_data.column(5) = antenna_5 ;
    raw_data.column(6) = antenna_6 ;
    raw_data.column(7) = antenna_7 ;
    
    uint nROW    = raw_data.nrow() ;
    uint nCOLUMN = raw_data.ncolumn();
    
	//cout << "NUmber of rows in raw data array :" <<nROW << " and number of columns :" <<nCOLUMN<< endl;
	casa::Vector<double> data_Vec = raw_data.column(0) ;
	  
// 	  double alpha (0.5);
// 	  
// 	  DataReader *dr;
// 	
//   ITS_Capture *capture = new ITS_Capture ( filename,
// 					1024 );
//   dr = capture;
//           dr->setHanningFilter (alpha);
// 	  Matrix<DComplex> fft_filtered (dr->fft());
// 	  Matrix<Double> absoluteArray(amplitude (fft_filtered));
	   
	Matrix<DComplex> FFT_data = numooontrigger.fft_data( data_Vec,
				 	                             n_frames,
	 				                            nyquist_zone ) ;
	Matrix<double>	FFT_ampl = amplitude( FFT_data )   ;
	
	Matrix<DComplex> RFI_rem =  numooontrigger.RFI_removal( FFT_data ) ;
 	
	Matrix<double>	RFI_rem_ampl = amplitude( RFI_rem )   ;
	
	casa::Matrix<DComplex> geom_Weights_factor= numooontrigger.Geom_weights(  RCU_id,
 					        				  freq_Vector )   ;
 	 cout << "geometrical weights have been calculated:" << endl ;
	 
	Matrix<DComplex> Beam_formed( 513, n_frames, 0.0 ) ;
       
        Matrix<double> Beam_formed_ampl( 513, n_frames, 0.0 ) ;
       
       casa::Matrix<double> in_phase(nROW, nCOLUMN, 0.0 ) ;
       
	for( uint antenna=0; antenna< nCOLUMN; antenna++ ) {
 	
		casa::Vector<double> data_vector = raw_data.column(antenna) ;
		
		casa::Vector<DComplex> geomWeights = geom_Weights_factor.column(antenna) ;
 	
			
		Matrix<DComplex> FFTdata = numooontrigger.fft_data( data_vector,
				 	                             n_frames,
	 				                            nyquist_zone ) ;
		
		Matrix<DComplex> RFI_removed =  numooontrigger.RFI_removal( FFTdata ) ;
 	
		Matrix<DComplex> phase_corrected = numooontrigger.weights_applied( RFI_removed,
   						             			   geom_Weights_factor,
 							     	 		   antenna ) ;
 		 cout << "geometrical weights have been applied for antenna :" << antenna << endl ;
		DComplex beamed_value(0.0) ;
												
		for( uint frame=0; frame< n_frames; frame++ ){
		
			for( uint freQchannel =0 ; freQchannel< 513; freQchannel++ ){
			
				beamed_value = phase_corrected( freQchannel, frame);
			
				Beam_formed( freQchannel,frame) = Beam_formed( freQchannel,frame)+ beamed_value;
			 	}
			}
			
		}
	
		cout << "data is beamformed" <<endl ;
	Beam_formed = Beam_formed*(1./n_rcu) ;
	
	Beam_formed_ampl = amplitude( Beam_formed ) ;
	
	Vector<double> IFFT_data = numooontrigger.ifft_data( Beam_formed,
     				                              n_frames,
	 						      nyquist_zone ) ;  
	
	cout << "IFFT has been calculated : "<< endl ;
       // in_phase.column(antenna) = IFFT_data ;
		  
	Vector<double> Power_IFFT_data( nofSamples, 0.0 );
	
	Vector<double> Power_integrated( nofSamples-5, 0.0 );
 
 	for(uint l=0; l<nofSamples; l++){
  
  		double ifft_sample = IFFT_data(l) ;
  		
  		Power_IFFT_data(l)= ifft_sample*ifft_sample ;
		//cout << "ifft_sample "  << ifft_sample << " Power IFFT data " << Power_IFFT_data(l) <<endl ;
	}
 
 Matrix<DComplex> power_FFTdata = numooontrigger.fft_data( Power_IFFT_data,
				 	                   n_frames,
	 				                    nyquist_zone ) ;
Matrix<double>	power_ampl_FFTdata = amplitude( power_FFTdata )	;    
	for(uint n=0; n<nofSamples-5; n++){
		for( uint m=0; m<5; m++) {
		
			Power_integrated(n) += Power_IFFT_data(n+m) ;
		}		
	}	
	
	casa::Vector<double> Av_P5( n_frames, 0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++){
	
		double averageP5(0.0) ;
		for( uint av= frame*1024; av< frame*1024+1024; av++ ){
	     
			 averageP5 += Power_integrated(av) ;
	 	}
	 	averageP5 = averageP5/1024. ;
		Av_P5(frame) = averageP5 ;
		
	//	cout << "number of frame : " <<frame << " has averageP5 " << averageP5 << endl ;
	 }

	 
	ofstream logfile1;
    
        logfile1.open( "raw_0.dat", ios::out );
        for( uint nSample=0; nSample < nofSamples ; nSample++ ){
		
	            logfile1 << data_Vec(nSample) << endl ;
	   }
	  
        logfile1.close() ;
	
	ofstream logfile2;
     
     logfile2.open( "FFT_data", ios::out );
        for( uint nSample=0; nSample < 513 ; nSample++ ){
		 for( uint ncol=0; ncol<n_frames ; ncol++){
	             
		     logfile2 << FFT_ampl(nSample,ncol) << "\t";
	   }
	  logfile2 <<endl ;
	}
       logfile2.close() ;
	
              
       ofstream logfile3;
        logfile3.open( "RFI_0", ios::out );
        for( uint Sample=0; Sample < 513 ; Sample++ ){
	
	   for( uint ncol=0; ncol<n_frames ; ncol++){
	   
	             logfile3 << RFI_rem_ampl(Sample,ncol) << "\t";
               }
             logfile3 <<endl ;
	  }
        logfile3.close() ;
	
	
        ofstream logfile4;
	logfile4.open( "Beam_formed", ios::out );
        for( uint nSample=0; nSample < 513  ; nSample++ ){
		for( uint ncol=0; ncol< n_frames; ncol++){
		
	        logfile4 << Beam_formed_ampl( nSample,ncol) << "\t" ;
	    }
	 logfile4 << endl;
	 } 
        logfile4.close() ;
	
	
	ofstream logfile5;
     
     logfile5.open( "IFFT_data_0", ios::out );
        for( uint nSample=0; nSample < nofSamples ; nSample++ ){
		
	            logfile5 << IFFT_data(nSample) << "\t" << Power_IFFT_data(nSample) << endl ;
	   }
	  
        logfile5.close() ; 
	
	ofstream logfile6;
     
     logfile6.open( "power_integrated", ios::out );
        for( uint nSample=0; nSample < nofSamples-5 ; nSample++ ){
		
	            logfile6 << Power_integrated(nSample) << endl ;
	   }
	  
        logfile6.close() ;
	
	ofstream logfile7;
	logfile7.open( "power_FFT", ios::out );
        for( uint nSample=0; nSample < 513  ; nSample++ ){
		for( uint ncol=0; ncol< n_frames; ncol++){
		
	        logfile7 << power_ampl_FFTdata( nSample,ncol) << "\t" ;
	    }
	 logfile7 << endl;
	 } 
        logfile7.close() ;
	
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main ( int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  
  std::string filename;
  if (argc > 1) {
    filename = std::string(argv[1]);
  } else {
    std::cerr << "Please provide a HDF5 filename.\n";
    return(DAL::FAIL);
  }

  // Test for the constructor(s)
  nofFailedTests += test_constructors (filename);

  return nofFailedTests;
}
