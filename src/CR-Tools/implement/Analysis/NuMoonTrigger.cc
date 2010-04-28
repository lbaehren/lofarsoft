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

namespace CR { // Namespace  -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  NuMoonTrigger::NuMoonTrigger()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void NuMoonTrigger::summary (std::ostream &os)
  {
    os << "[NuMoonTrigger] Summary of internal parameters." << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

#ifdef HAVE_ROOT

  //_____________________________________________________________________________
  //                                                                     fft_data

  casa::Matrix<DComplex> NuMoonTrigger::fft_data( Vector<double>&  data,
						  uint const& n_frames,
						  uint const& nyquist_zone ) 
  {
    uint dataBlockSize = 1024 ;
    uint n_freqbins    = dataBlockSize/2+1 ;
    Matrix<DComplex> fft_implemented( n_freqbins, n_frames, 0.0 ) ;
    
    try {
      FFTServer <double,DComplex> server ;
      casa::Vector<double> sliced_vector(dataBlockSize, 0.0) ;
      Vector<DComplex> FFTVector( n_freqbins, 0.0 ) ;
      int sample(0);
      
      for( uint i=0; i< n_frames; i++ ){
	
	casa::Vector<double> sliced_vector = data( Slice( sample, dataBlockSize )) ;
	
	server.fft( FFTVector, sliced_vector ) ;
	
	switch( nyquist_zone ){
	  
	case 1:  
	  fft_implemented.column(i) = FFTVector ;
	  
	  break ;
	  
	case 2:
	  for( uint channel(0); channel< n_freqbins; channel++ ){
	    
	    fft_implemented(channel, i )=conj( FFTVector( n_freqbins-channel-1)) ;
	    
	  }
	  break ;
	}	
	sample = sample +dataBlockSize ;
    	
      }
      
    }
    
    catch ( AipsError x ){
      
      cerr << " NuMoonTrigger::fft_data " << x.getMesg () << endl ;
      Matrix<DComplex>() ;
    }
    return  fft_implemented;	 
  }

  //_____________________________________________________________________________
  //                                                        zero_channel_flagging

  casa::Matrix<DComplex> NuMoonTrigger::zero_channel_flagging (Vector<double> &data,
							       uint const &n_frames,
							       uint const &nyquist_zone) 
  {
    uint dataBlockSize = 1024 ;
    uint n_freqbins    = dataBlockSize/2+1 ;
    Matrix<DComplex> fft_implemented( n_freqbins, n_frames, 0.0 ) ;
    
    try {
      
      FFTServer <double,DComplex> server ;
      
      casa::Vector<double> sliced_vector(dataBlockSize, 0.0) ;
      
      Vector<DComplex> FFTVector( n_freqbins, 0.0 ) ;
      
      int sample(0);
      
      for( uint i=0; i< n_frames; i++ ){
	
	casa::Vector<double> sliced_vector = data( Slice( sample, dataBlockSize )) ;
	
	server.fft( FFTVector, sliced_vector ) ;
	FFTVector(0) =0.0 ;
	FFTVector(n_freqbins-1) = 0.0 ;
	
	switch( nyquist_zone ){
	  
	case 1:  
	  fft_implemented.column(i) = FFTVector ;
	  
	  break ;
	  
	case 2:
	  
	  for( uint channel(0); channel< n_freqbins; channel++ ){
	    
	    fft_implemented(channel, i )=conj( FFTVector( n_freqbins-channel-1)) ;
	    
	  }
	  break ;
	}
	
	sample = sample +dataBlockSize ;
    	
      }
      
    }
    
    catch ( AipsError x ){
      
      cerr << " NuMoonTrigger::fft_data " << x.getMesg () << endl ;
      Matrix<DComplex>() ;
    }
    return  fft_implemented;	
  }

  //_____________________________________________________________________________
  //                                                                  RFI_removal

   Matrix<DComplex> NuMoonTrigger::RFI_removal( Matrix<DComplex> fft_samples ) 
   {
     
     uint n_row   = fft_samples.nrow() ;
     uint n_colum =fft_samples.ncolumn() ;
     casa::Matrix<DComplex> RFIMitigated_array( n_row, n_colum,0.0 ) ; 
     
     try {
     	   casa::Vector<double>freq_Vector = NuMoonTrigger::freq_vector(  200e6,
		   	                				 2) ;
	   
     	  uint dataBlockSize = 1024 ;
	  
	  DComplex expo(0, 1);
	   
	 // uint n_freqbins = dataBlockSize/2+1 ;
	  
	  uint n_segments = 3 ; //n_freqbins/16 ;
	  
	  casa::Matrix<double> phase_array(n_row, n_colum,0.0 ) ;
	  phase_array = phase(fft_samples);
	  
	  casa::Matrix<double> amplitude_spectra(n_row, n_colum,0.0 ) ;
	   
	 amplitude_spectra = 1.0 ; //amplitude(fft_samples) ;
	
	 casa::Matrix<DComplex> array_phases( n_row, n_colum,0.0 ) ;
	  
	  for(uint i=0; i<n_row ; i++ ){
	  
	  	for(uint j=0; j<n_colum; j++ ){
		
			double phase_value = phase_array(i,j) ;
			
			array_phases(i,j) = exp(expo*phase_value) ;
			
			}
			
		}
	  			  
	  // CR::RFIMitigation  rfi( fft_samples,
	  //		          dataBlockSize,
	  //		          n_segments ) ;
	  
	  casa::Matrix<double> RFIrejected_array( n_row, n_colum,0.0 ) ;
	  for(uint i=0;i<n_colum;i++){
	    //	    RFIrejected_array= rfi.getOptimizedSpectra( fft_samples,
	    //dataBlockSize,
	    casa::Vector<Double> rfiVec=RFIrejected_array.column(i);
	      RFIMitigation::doRFIMitigation(amplitude(fft_samples.column(i)),n_segments,1e20,5.,rfiVec) ;	
	  }
	  casa::Matrix<double> RFIrejected_absolutearray( n_row, n_colum,0.0 ) ;
	  
	  RFIrejected_absolutearray = RFIrejected_array ; //*amplitude_spectra ;
	  //RFIrejected_absolutearray = amplitude_spectra ;
	  
	 // uint n_freq = freq_Vector.nelements() ;
	
	 // uint n_RFIremoved = RFIrejected_absolutearray.ncolumn() ;
	  /*

	  double f_init = 150e+6 ;
		
	  double f_final = 180e+6 ;
	
	  for( uint nc_RFI=0; nc_RFI< n_RFIremoved; nc_RFI++ ){
	
		for( uint fr=0; fr< n_freq; fr++ ){
	
			double frequency = freq_Vector(fr) ;
		
			if( frequency >f_init && frequency < f_final ){
			
				//cout << "frequency values :" << frequency <<endl ;
			
				RFIrejected_absolutearray(fr,nc_RFI) =0.0 ;
				
				}
			}
		}	
*/ 
	  casa::Matrix<DComplex> RFIreject_array( n_row, n_colum,0.0 ) ;  ;
	  
	  //convertArray( RFIreject_array,amplitude_spectra) ;

	  convertArray( RFIreject_array, RFIrejected_absolutearray ) ;
	  	  
	  RFIMitigated_array = RFIreject_array*array_phases  ;
	  
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::RFI_removal " << x.getMesg () << endl ;
       Matrix<DComplex> ();
       }
       
      return RFIMitigated_array ;   
   } 						

  //_____________________________________________________________________________
  //                                                           Average_effect_RFI

  Matrix<DComplex> NuMoonTrigger::Average_effect_RFI( Matrix<DComplex> fft_samples,
						      Matrix<DComplex> RFI_mitigated ) 
   {
      uint n_row   = fft_samples.nrow() ;
      uint n_colum = fft_samples.ncolumn() ;
      casa::Matrix<DComplex> RFI_Average_effect( n_row,n_colum,0.0);
	    
      try {
	DComplex expo(0, 1);
	
	casa::Matrix<double> fft_phase_array( n_row, n_colum,0.0 ) ;
	
	fft_phase_array = phase( fft_samples );
	
	casa::Matrix<DComplex> fft_array_phases( n_row, n_colum,0.0 ) ;
	
	for(uint i=0; i<n_row ; i++ ){
	  
	  for(uint j=0; j<n_colum; j++ ){
	    
	    double phase_value = fft_phase_array(i,j) ;
	    
	    fft_array_phases(i,j) = exp(expo*phase_value) ;
	    
	  }
	  
	}
	
	
	casa::Matrix<double> fft_amplitude_spectra( n_row, n_colum,0.0 ) ;
	
	fft_amplitude_spectra = amplitude( fft_samples ) ;
	
	casa::Matrix<double> RFI_amplitude_spectra( n_row, n_colum,0.0 ) ;
	
	RFI_amplitude_spectra = amplitude( RFI_mitigated ) ;
	
	casa::Matrix<double> RFI_Bin_Average( n_row, n_colum, 0.0 ) ;
	
	for( uint col=0; col< n_colum; col++ ){
	  
	  casa::Vector<double> RFI_column_Vector = RFI_amplitude_spectra.column(col) ;
	  
	  for( uint row=0; row< n_row -10; row++ ){
	    
	    casa::Vector<double> sort_RFI_vector = RFI_column_Vector( Slice( row,10 )) ; 
	    
	    //double average_value = mean( sort_RFI_vector ) ;
	    
	    double standard_deviation = stddev( sort_RFI_vector ) ;
	    
	    RFI_Bin_Average( row,col )= standard_deviation ;
	  }
	}
	
	casa::Vector<double> bin_av_filled = RFI_Bin_Average.row(n_row-11) ;
	
	for( uint l_row= n_row-10; l_row< n_row; l_row++ ){
	  
	  RFI_Bin_Average.row(l_row)= bin_av_filled ;
	  
	}
	for( uint coll=0; coll< n_colum; coll++ ){
	  
	  //casa::Vector<double> RFI_column_Vector = RFI_amplitude_spectra.column(coll) ;
	  
	  //casa::Vector<double> fft_column_vector =  fft_amplitude_spectra.column(coll) ;
	  
	  for( uint roww=0; roww< n_row ; roww++ ){
	    
	    double bin_average = RFI_Bin_Average(roww, coll) ;
	    
	    if( fft_amplitude_spectra( roww,coll ) > 2*bin_average )
	      
	      { fft_amplitude_spectra( roww,coll ) =1.0 ;
		
		// cout << "fft bin content has made unity" << endl ;
		
	      }
	  }
	}
	
	casa::Matrix<DComplex> fft_complex_array( n_row, n_colum,0.0 ) ;  
	
	convertArray( fft_complex_array, fft_amplitude_spectra ) ;
	
	RFI_Average_effect = fft_complex_array*fft_array_phases ;
	
        
      }
      
      catch ( AipsError x ){
	cerr << " NuMoonTrigger::Average_effect_RFI" << x.getMesg () << endl ;
	Matrix<DComplex> ();
      }
      return RFI_Average_effect ;   
   } 
  
  //_____________________________________________________________________________
  //                                                                  freq_vector

  Vector<double> NuMoonTrigger::freq_vector (double const& sampling_rate,
					     uint const& nyquist_zone ) 
  {
    // casa::Vector<double> frequency_Vector ;
    Vector<double> frequency_Vector(513,0.0) ;
    try {
      
      double channel_width = sampling_rate/1024. ;			 
      
      Vector<uint> band_ID( 513, 0 ) ;
      
      Vector<uint> selected_channelID ;
      
      Vector<double> selected_channels ;
      
      switch( nyquist_zone ){
	
      case 1:  
	for( uint i=0; i<513 ; i++ ){
	  
	  double central_freq = i*channel_width + channel_width/2 ;
	  
	  frequency_Vector(i) = central_freq ;
	  
	  band_ID(i) = i+1 ;
	  
	}
	
	break ;
	
      case 2:
	for( uint i=0; i<513 ; i++ ){
	  
	  double central_freq = sampling_rate/2+i*channel_width + channel_width/2 ;
	  
	  frequency_Vector(i) = central_freq ;
	  
	  band_ID(i) = i+1 ;
	  
	}
	
	
	break ;
      }
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::freq_vector" << x.getMesg () << endl ;
      Vector<double>() ;
    }
    
    return frequency_Vector ;  
    
  }
  
  //_____________________________________________________________________________
  //                                                                de_dispersion

  Matrix<DComplex> NuMoonTrigger::de_dispersion (Matrix<DComplex> fft_samples,
						 double const& TEC,
						 const Vector<double> freq_Vector ) 
  {
    uint n_cloumn = fft_samples.ncolumn() ;
    uint n_row    = fft_samples.nrow() ;
    casa::Matrix<DComplex> dispersed_modified_array( n_row, n_cloumn, 0.0 ) ;
    
    try {
      
      double pi = 3.14159265358979323846 ;
      
      DComplex expo(0, 1);
      
      DComplex expr = exp(expo*0.0) ;
      
      uint dataBlockSize = 1024 ;
      
      uint n_freqbins = dataBlockSize/2+1 ;
      
      double DM = 1.34e+9*TEC*1.0 ;
      
      double t0 = 8.e+5/3.e+8 ;
      
      double modified_phase(0.0) ;
      
      casa::Matrix<double> phase_array(n_row, n_cloumn, 0.0) ;
      
      phase_array = ( phase(fft_samples));
      
      uint n_columns = phase_array.ncolumn() ;
      
      uint n_rows = phase_array.nrow() ;
      //Matrix<double> phase_array = phase(fft_samples) ;
      //  casa::Matrix<DComplex> dispersed_modified_array( n_rows, n_columns, 0.0 ) ;
      
      casa::Matrix<double> amp_array(n_row, n_cloumn, 0.0) ;
      
      amp_array = amplitude(fft_samples) ;
      
      casa::Matrix<DComplex> amp_amplified_array( n_row, n_cloumn, 0.0 ) ; 
      
      convertArray( amp_amplified_array, amp_array ) ;
      
      casa::Matrix<DComplex> phase_modified_array( n_rows, n_columns, 0.0 ) ;
      
      for( uint i=0; i< n_columns; i++ ){
	
	for( uint j=0; j< n_freqbins; j++ ){
	  
	  double inner_term = 1-2*DM/t0/((freq_Vector(j))*(freq_Vector(j))) ;
	  
	  double outer_term = 2*pi*sqrt(inner_term)*(freq_Vector(j))*t0 -2*pi*t0*freq_Vector(j) ;
	  
	  modified_phase = fmod(phase_array(j,i) +pi +outer_term,2*pi) ;
	  
	  phase_modified_array(j,i) = exp(expo*modified_phase) ;
	  
	}
	
      }
      
      dispersed_modified_array =  amp_amplified_array*phase_modified_array  ;
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::de_dispersion " << x.getMesg () << endl ;
      Matrix<DComplex> ();
    }
    return dispersed_modified_array ;	 
  } 
  
  //_____________________________________________________________________________
  //                                                                    ifft_data

  Vector<double> NuMoonTrigger::ifft_data (Matrix<DComplex> const& data,
    			    		   uint const& n_frames,
					   uint const& nyquist_zone ) 
  {
    uint dataBlockSize = 1024 ;
    
    casa::Vector<double> timeSeries( dataBlockSize*n_frames, 0.0 ) ;
    try {
      uint n_freqbins = dataBlockSize/2+1 ;
      
      casa::Vector<DComplex> FFTVector( n_freqbins,0.0) ;
      
      FFTServer <double,DComplex> server ;
      
      casa::Vector<double> IFFTVector( dataBlockSize, 0.0 );
      
      uint sample_n(0) ;
      
      for( uint i=0; i< n_frames; i++ ) {
	
	casa::Vector<DComplex> sliced_vector = data.column(i) ;
	
	switch( nyquist_zone ){
	case 1:  
	  FFTVector = sliced_vector ;
	  break ;
	case 2:
	  for( uint channel(0); channel< n_freqbins; channel++ ){
	    FFTVector(n_freqbins-channel-1)=conj( sliced_vector( channel)) ;
	  }
	  break ;
	};

	server.fft(IFFTVector, FFTVector ) ;
	
	for( uint j=0; j< dataBlockSize; j++ ) {
	  sample_n = i*dataBlockSize+j ;
	  timeSeries( sample_n )= IFFTVector(j) ;
	}
 	
      } // END -- for( uint i=0; i< n_frames; i++ )
    }
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::ifft_data " << x.getMesg () << endl ;
      Vector<Double> ();
    }
    return timeSeries ;
  }
  
  //_____________________________________________________________________________
  //                                                                  cal_AvPower
  
  Vector<double> NuMoonTrigger::cal_AvPower( Vector<double> const& beamed_Array ) 
    
  {
    uint dataBlockSize = 1024 ;
    uint n_row         = beamed_Array.nelements() ;
    uint n_frames      = n_row/dataBlockSize ;
    
    Vector<double> Average_P5( n_frames, 0.0 ) ;
    
    try {
      
      
      Vector<double> P5( dataBlockSize-5-25-1, 0.0 ) ;
      // 5 is for 5 time block integration and 25 is to neglect end effect, 
      //an extra one needs to be subtracted to define exact number of pairs
      
	  for(uint i=0; i<n_frames; i++ ){
	  
	  	double average_power(0.0) ;
			
			for( uint k=25; k< dataBlockSize-25 ; k++ ){
			
				double Power_5(0.0) ;
				
				for( uint l=k; l<(k+5); l++ ){
				
					uint count = i*1024+l ;
				
					double value = beamed_Array(count) ;
					
					//cout << " value of data array element :" << value << endl ;
				
					Power_5 += value*value ;
					
					//cout << " Power_5 : " <<Power_5 <<endl ;
					
					}
					
					//P5(k,i) = Power_5 ;
					
					average_power += Power_5 ;
				}
			
			Average_P5(i) = average_power/(1024.-50.) ;
	  		
		}	  
         
		
      }
     
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::cal_AvPower " << x.getMesg () << endl ;
       Vector<double> ();
       }
       
     return Average_P5 ;  
  }   
  
  //_____________________________________________________________________________
  //                                                                ppf_implement

   Matrix<DComplex> NuMoonTrigger::ppf_implement( Vector<double>& samples,
   						  uint const& nyquist_zone,
                                  		  const Vector<double>& ppf_coeff ) 
   {
     uint dataBlockSize = 1024 ;
     uint n_element     = samples.nelements() ;
     uint nColumns      = n_element/dataBlockSize ;
     casa::Matrix<DComplex> sorted_Matrix(513,nColumns,0.0 ) ;
     
     try { 
       Matrix<DComplex> ppf_implemented ;
       
       CR::ppfimplement ppfimpl( samples,
				 ppf_coeff ) ;
       
       ppf_implemented = ppfimpl.FFTSamples( samples,
					     ppf_coeff ) ;
       uint n_columns = ppf_implemented.ncolumn() ;
       
       casa::Vector<DComplex> column_vector(1024,0.0) ;
       
       casa::Vector<DComplex> sorted_vector(513,0.0) ;
       
       switch( nyquist_zone ){
	 
       case 1:  
	 for( uint i=0; i<n_columns ; i++ ){
	   
	   ppf_implemented.column(i) = ppf_implemented.column(i) ;
	   
	   column_vector = ppf_implemented.column(i) ;
	   
	   sorted_Matrix.column(i) = column_vector(Slice(0,513)) ;	
	 }
	 
	 break ;
	 
       case 2:
	 
	 for( uint i=0; i<n_columns ; i++ ){
	   
	   for( uint channel(513); channel< dataBlockSize; channel++ ){
	     ppf_implemented(channel, i )=( ppf_implemented(channel,i)) ;
	     //ppf_implemented(channel, i )=conj( ppf_implemented(( dataBlockSize-channel-1),i)) ;
	   }
	   
	   column_vector = ppf_implemented.column(i) ;
	   
	   sorted_Matrix.column(i) = column_vector(Slice(511,513)) ;
	 }
	 
	 break ;
       }
       /*			
				for(uint NC=0; NC< n_columns; NC++){
				
				column_vector = ppf_implemented.column(NC) ;
				
				sorted_vector = column_vector(Slice(0,513)) ;
				
				sorted_Matrix.column(NC) =sorted_vector ;
				
				}*/
       
     }
     
     catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppf_implement " << x.getMesg () << endl ;
       Matrix<DComplex> ();
     }
     return sorted_Matrix ;   
   } 
  
  //_____________________________________________________________________________
  //                                                             ppfdata_cutshort

  Matrix<DComplex> NuMoonTrigger::ppfdata_cutshort( Matrix<DComplex>& samples,
						    Vector<uint> subband_ID ) 
  {
    uint nOfelements = subband_ID.nelements() ;
    
    uint n_columns = samples.ncolumn() ;
    
    Matrix<DComplex> sortened_array( 1024, n_columns, 0.0 ) ;
    
    try {
      
      
      for( uint i=0; i< nOfelements; i++ ){
	
	uint band_ID = subband_ID(i) ;
	
	Vector<DComplex> sample_vector = samples.row( band_ID-1 ) ;
	
	sortened_array.row( band_ID-1 ) = sample_vector ;
	
      }
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::ppfdata_cutshort" << x.getMesg () << endl ;
      Matrix<DComplex> (); 
    }
    return sortened_array;
  } 
  
  //_____________________________________________________________________________
  //                                                               PPFBand_vector

  Vector<uint> NuMoonTrigger::PPFBand_vector( double const& sampling_rate,
					      uint const& nyquist_zone,
					      const Vector<double> freq_range ) 
  {
    double init          = freq_range(0) ;
    double final         = freq_range(1) ;
    double channel_width = sampling_rate/1024. ;
    int number_channels  = int((final-init)/channel_width) ;
    Vector<uint> selected_channelID( number_channels, 0) ;
    
    try {
      Vector<double> frequency_vector(513,0.0) ;
      
      Vector<uint> band_ID( 513, 0 ) ;
      
	   switch( nyquist_zone ){
	  
	  		case 1:  
				for( uint i=0; i<513 ; i++ ){
				
					double central_freq = i*channel_width + channel_width/2. ;
					
					frequency_vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
									
					}
				
				break ;
				
			case 2:
				for( uint i=0; i<513 ; i++ ){
				
					double central_freq = sampling_rate/2.+i*channel_width + channel_width/2. ;
					
					frequency_vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
					
					}
				
				
    				break ;
			}
			
		//uint n_freqch = frequency_vector.nelements() ;
			
		uint k=0; 
			
		for( uint j=0; j<513; j++ ){
		
			double compare = frequency_vector(j)   ;
			
			if( compare >init && compare <final) {
			
				uint bandid = band_ID(j) ;
				
				selected_channelID(k) = bandid ;
				
				k = k+1 ;
				
			}
		}
	 
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::PPFBandID_vector" << x.getMesg () << endl ;
       Vector<uint> () ;
	}
      
      return selected_channelID ; 
   } 
   
  //_____________________________________________________________________________
  //                                                                ppf_inversion

   Vector<double> NuMoonTrigger::ppf_inversion( const Matrix<DComplex>& FTData, 
                               			const Vector<double>& ppf_invcoeff,
						const Vector<uint> subBand_ID )  
						
   {
	Vector<Double> ppfinverted_Vector ;
  	 
     try {
     	  	   
          CR::ppfinversion ppf_Invert( FTData,
	  			       ppf_invcoeff,
				       subBand_ID ) ;
     
         ppfinverted_Vector = ppf_Invert.FIR_inversion( ppf_invcoeff,
	 	       		 		        FTData,
		       		 		        subBand_ID ) ;
        	 
	 }
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppf_inversion " << x.getMesg () << endl ;
        Vector<Double>();
	}
       
       return ppfinverted_Vector ;
   } 
   
  //_____________________________________________________________________________
  //                                                                  phase_delay

   Matrix<double> NuMoonTrigger::phase_delay( const Vector<double>& freq_Vector,
				              const double& source_latitude,
					      const double& source_longitude,
					      const Vector<double>& position_x,
		   			      const Vector<double>& position_y,
		   		              const Vector<double>& position_z ) 
 
  {
    	uint n_element = position_x.nelements() ;
         
	uint freq_channels = freq_Vector.nelements() ;
	  
	Matrix<double> phase_Delay(freq_channels, n_element, 0.0 ) ; 
	
     try { 
     	  double pi = 3.14159265358979323846 ;
	  
     	  Vector<double> geomdelay = NuMoonTrigger::geom_Delay( source_latitude,
								source_longitude,
								position_x,
								position_y,
								position_z ) ;
	  
	  for( uint antenna=0; antenna< n_element; antenna++ ){
	  
	  	for( uint freq=0; freq< freq_channels; freq++ ){
		
			double frequency = freq_Vector(freq) ;
			
			double  delay = geomdelay( antenna ) ;
		
			phase_Delay(freq, antenna)= 2*pi*frequency*delay ;
			
			}
		}
     
        
	
     } 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::phase_Delay " << x.getMesg () << endl ;
      Matrix<double>();
     }
   
     return phase_Delay ;
  } 
  
  //_____________________________________________________________________________
  //                                                                   geom_delay
  
  Vector<double> NuMoonTrigger::geom_Delay( const double& source_latitude,
			    		    const double& source_longitude,
					    const Vector<double>& position_x,
		   			    const Vector<double>& position_y,
		   		            const Vector<double>& position_z )
  {
    uint  RCU_channels = position_x.nelements() ;
    
    Vector<double> Geomdelay( RCU_channels, 0.0 ) ;
    
    try { 
      
      DComplex expo(0, 1);
      
      double speed_light = 3e8 ;
      
      double source_pos_x = cos(source_longitude)*cos(source_latitude) ;
      double source_pos_y = sin(source_longitude)*cos(source_latitude) ;
      double source_pos_z = sin(source_latitude) ; 
      
      Vector<double> ant_pos_x( RCU_channels, 0.0) ;
      Vector<double> ant_pos_y( RCU_channels, 0.0) ;
      Vector<double> ant_pos_z( RCU_channels, 0.0) ;  
      
      for( uint j=0; j< RCU_channels; j++ ){
	
	ant_pos_x(j) = position_x(j)-position_x(0) ;
	ant_pos_y(j) = position_y(j)-position_y(0) ;
	ant_pos_z(j) = position_z(j)-position_z(0) ; //+ height(j) ;
	
	Geomdelay(j) = (1./speed_light)*(source_pos_x*ant_pos_x(j)+source_pos_y*ant_pos_y(j)+source_pos_z*ant_pos_z(j))   ;
      }
      
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::geom_Delay " << x.getMesg () << endl ;
      Vector<double> ();
    }
    return Geomdelay ;
  }	
  
  //_____________________________________________________________________________
  //                                                                integer_Delay

  Vector<int> NuMoonTrigger::integer_Delay( const double& source_latitude,
			    		    const double& source_longitude,
					    const double& sampling_frequency,
					    const Vector<double>& position_x,
		   			    const Vector<double>& position_y,
		   		            const Vector<double>& position_z )
  { 
    uint  RCU_channels = position_x.nelements() ;
    
    Vector<int> integer_delay( RCU_channels,0 );
    
    try {  
      
      Vector<double> geomdelay = NuMoonTrigger::geom_Delay( source_latitude,
							    source_longitude,
							    position_x,
							    position_y,
							    position_z ) ;
      uint n_element = geomdelay.nelements() ;
      
      for( uint i=0; i< n_element; i++ ){
	
	double delay = geomdelay(i) ;
	
	int intDelay = int(delay*sampling_frequency) ;
	
	integer_delay(i) = intDelay ;
	
      }		
      
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::integer_Delay " << x.getMesg () << endl ;
      Vector<int> ();
    }
    return integer_delay ;
  }	
  
  //_____________________________________________________________________________
  //                                                               fraction_Delay

  Vector<double> NuMoonTrigger::fraction_Delay( const double& source_latitude,
			    		        const double& source_longitude,
					        const double& sampling_frequency,
			    		        const Vector<double>& position_x,
		   	    			const Vector<double>& position_y,
		   	    			const Vector<double>& position_z )
  {
    uint  RCU_channels = position_x.nelements() ;
    
    Vector<double> fraction_delay( RCU_channels,0 );
    
    try {  	
      
      Vector<double> geomdelay = NuMoonTrigger::geom_Delay( source_latitude,
							    source_longitude,
							    position_x,
							    position_y,
							    position_z ) ;
      uint n_element = geomdelay.nelements() ;
      
      for( uint i=0; i< n_element; i++ ){
	
	double delay = geomdelay(i) ;
	
	double fracDelay = delay - (1./sampling_frequency)*(int(delay*sampling_frequency)) ;
	
	fraction_delay(i) = fracDelay ;
	
      }		
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::fraction_Delay " << x.getMesg () << endl ;
      Vector<double> ();
    }
    return fraction_delay ;
    
  }		    
  
  //_____________________________________________________________________________
  //                                                              weights_applied

  Matrix<DComplex> NuMoonTrigger::weights_applied( Matrix<DComplex> const& data,
						   Matrix<double> const& phase_Delay,
						   const Vector<double>& gain_scale_factor,
						   uint const& rcu_id ) 
    
  {
    uint n_columns = data.ncolumn() ;
    
    uint n_rows = data.nrow() ;
    
    casa::Matrix<DComplex> phase_corrected_data(n_rows, n_columns, 0.0 ) ;
    
    try {
      /*Matrix<double> NuMoonTrigger::phase_delay( const Vector<uint>& RCU_id,
	const Vector<double>& freq_Vector,
	const double& source_latitude,
	const double& source_longitude ) ; */
      DComplex expo(0, 1) ;
      
      casa::Matrix<double> data_phase( n_rows, n_columns, 0.0 ) ;
      
      casa::Matrix<double> data_amplitude( n_rows, n_columns, 0.0 ) ;
      
      casa::Matrix<DComplex> data_phase_corrected(n_rows, n_columns, 0.0 ) ;
      
      casa::Matrix<DComplex> data_amplitude_corrected(n_rows, n_columns, 0.0 ) ;
      
      Vector<double> phase_vector( n_rows,0.0) ;
      
      Vector<double> data_phase_vector( n_rows,0.0 ) ;
      
      Vector<double> phase_modified_vector( n_rows, 0.0 ) ; 
      
      phase_vector = phase_Delay.column( rcu_id ) ;
      
      double gainScale = gain_scale_factor(rcu_id) ;
      
      data_phase = phase(data) ;
      
      data_amplitude = amplitude( data) ;
      
      data_amplitude = data_amplitude*gainScale ;
      
      convertArray( data_amplitude_corrected, data_amplitude ) ;
      
      for( uint i=0; i< n_columns; i++ ) {
	
	data_phase_vector = data_phase.column(i) ;
	
	phase_modified_vector = data_phase_vector+phase_vector ;
	
	DComplex weight_applied(0.0) ;
	
	for( uint j=0; j< n_rows; j++ ){
	  
	  weight_applied = exp( expo*phase_modified_vector(j)) ;
	  
	  data_phase_corrected(j,i) = weight_applied ;
	}
      }
      
      phase_corrected_data = data_amplitude_corrected*data_phase_corrected ;
      
      
    } 
    
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::weights_applied " << x.getMesg () << endl ;
      Matrix<DComplex> ();
    }
    return phase_corrected_data ;
  }

  //_____________________________________________________________________________
  //                                                                   Sim_Signal

  /*!
    \param data              -- Vector with the data to process.
    \param simTEC            -- 
    \param freq_Vector       -- 
    \param peak_height       -- 
    \param pulse_loc         -- 
    \param rnd_phase         -- 
    \param geom_phase_factor -- 
    \param nyquist_zone      -- 

    \return addedSignal -- 
  */
  Vector<double> NuMoonTrigger::Sim_Signal (Vector<double> const& data,
    		 	     		    double const& simTEC,
					    const Vector<double>& freq_Vector,
					    double const& peak_height,
					    double const& pulse_loc,
					    double const& rnd_phase,
					    Vector<double> geom_phase_factor,
					    uint const& nyquist_zone)
  {
    double pi       = 3.14159265358979323846;
    uint blocksize  = 1024 ;
    uint fftLength  = blocksize/2+1 ;
    uint n_elements = data.nelements() ;
    
    casa::Vector<double> addedSignal( n_elements,0.0 ) ;
    casa::Vector<double> amp( fftLength, 0.0 ) ;
    casa::Vector<double> phase( fftLength, 0.0 ) ;
    casa::Vector<DComplex> phased_vector( fftLength, 0.0 ) ;
    
    try {
      
      double DM, t0; 
      DComplex expo(0, 1);
      DComplex expr = exp(expo*0.0) ;
      FFTServer <Double,DComplex> server ;
      
      DM = 1.34e+9*simTEC*1.0 ;
      
      t0 = 8.e+5/3.e+8 ;
      
      for( uint i=0; i< fftLength; i++ ) {
	
	amp(i) = (peak_height/1024) ;
	
	phase(i) = rnd_phase + (1-(pulse_loc/1024.))*2*pi*i-geom_phase_factor(i);
	
	double nu = freq_Vector(i) ;
	
	phase(i) -= 2*pi*sqrt(1-2*DM/t0/(nu*nu))*nu*t0 -2*pi*nu*t0 ;
	
	phased_vector(i) = exp(expo*phase(i)) ;
	
      }
      
      casa::Vector<DComplex> amp_vector( fftLength, 0.0 )  ;
      
      convertArray( amp_vector, amp ) ;
      
      casa::Vector<DComplex> dispersed_signalVector( fftLength, 0.0 ) ;
      
      casa::Vector<DComplex> nyquist_signalVector( fftLength, 0.0 ) ;
      
      dispersed_signalVector = amp_vector*phased_vector  ;
      
      casa::Vector<double> signal_Vector ( blocksize,0.0 ) ;
      
      server.fft( signal_Vector, dispersed_signalVector ) ;
      
      double signal(0.0) ;
      
      for( uint j=0; j< blocksize; j++ ) {
	
	signal = 1024.*signal_Vector(j);   
	
	if(signal>2047) {signal=2047 ;} 
	if(signal<-2048) {signal=-2048 ;} 
	
	double data_value =data(j) ;
	
	addedSignal(j) = signal +data_value ;
	
      }
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::Sim_Signal " << x.getMesg () << endl ;
    }
    
    return addedSignal ;	  
  }
  
  //_____________________________________________________________________________
  //                                                                integer_Delay
  
  Matrix<double> NuMoonTrigger::Cleaned_data(  Matrix<double> const& data,
					       uint const& n_samples,
					       double const& simTEC,
					       uint const& nyquist_zone,
					       double const& sampling_rate,
					       double const& TEC,
					       const Vector<double> freq_range ) 
  {
    uint blocksize = 1024;
    uint n_frames  = n_samples/blocksize ;
    uint nc_rcu    = data.ncolumn() ;
    
    casa::Matrix<double> Cleaned_data(n_samples, nc_rcu,0.0 ) ;  
    
    try {
      
      CR::NuMoonTrigger moontrigger() ;
      
      casa::Vector<double> FREQvector = NuMoonTrigger::freq_vector( sampling_rate,
								    nyquist_zone ) ;

      for( uint rcu=0; rcu< nc_rcu; rcu++ ) {
	
	casa::Vector<double> RCU_data = data.column( rcu ) ;
	
	casa::Matrix<DComplex> FFTdata = NuMoonTrigger::fft_data( RCU_data,
								  n_frames,
								  nyquist_zone ) ;
	
	casa::Matrix<DComplex> RFIremoved =NuMoonTrigger::RFI_removal( FFTdata ) ;
	
	casa::Matrix<DComplex> dispersed_Array = NuMoonTrigger::de_dispersion( RFIremoved,
									       TEC,
									       FREQvector ) ;
	
	casa::Vector<double> IFFT_Vector = NuMoonTrigger::ifft_data( dispersed_Array,
								     n_frames,
								     nyquist_zone ) ;
	
	Cleaned_data.column(rcu) = IFFT_Vector ;
	
      }
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger:: Cleaned_data" << x.getMesg () << endl ;
      Matrix<double>() ;
    }
    return Cleaned_data ;	    
    
  }

  //_____________________________________________________________________________
  //                                                              BeamFormed_data

  Vector<double> NuMoonTrigger::BeamFormed_data(  Matrix<double> const& data,
						  uint const& n_samples,
						  double const& simTEC,
						  uint const& nyquist_zone,
						  double const& sampling_rate,
						  double const& TEC,
						  const Vector<double> freq_range,
						  const double& pointing_latitude,
						  const double& pointing_longitude,
						  const Vector<double>& gain_scale_factor,
						  const Vector<double>& position_x,
						  const Vector<double>& position_y,
						  const Vector<double>& position_z ) 
    
  {
    uint blocksize = 1024;
    uint fftLength = blocksize/2+1;
    uint n_row     = data.nrow() ;
    uint n_column  = data.ncolumn() ;
    uint n_frames  = n_row/blocksize ;
    
    casa::Vector<double> beamformed_Vector( n_row, 0.0) ;
    
    try {
      
      
      Matrix<double> cleaned_data = NuMoonTrigger::Cleaned_data(  data,
								  n_samples,
								  simTEC,
								  nyquist_zone,
								  sampling_rate,
								  TEC,
								  freq_range ) ;
      
      casa::Vector<double> FREQvector = NuMoonTrigger::freq_vector( sampling_rate,
								    nyquist_zone ) ;
      
      casa::Matrix<double> phases = NuMoonTrigger::phase_delay( FREQvector,
								pointing_latitude,
								pointing_longitude,
								position_x,
								position_y,
								position_z )  ;
      
      casa::Vector<double> column_CleanedVector( n_row, 0.0 );
      
      casa::Vector<double> sorted_Vector( blocksize, 0.0) ;
      
      casa::Matrix<DComplex> sorted_FFT_Vector( fftLength, 1, 0.0 ) ;
      
      casa::Vector<double> beamformed_IFFT( blocksize, 0.0) ;
      
      casa::Matrix<DComplex> beamformed_FFT_dummy( fftLength, 1, 0.0 ) ;
      
      uint init_Bin =0 ;
      
      for( uint f=0; f< n_frames; f++ ){
	
	casa::Vector<DComplex> beamformed_FFT( fftLength, 0.0 );
	
	for( uint antenna=0; antenna< n_column; antenna++ ){
	  
	  column_CleanedVector = data.column(antenna);
	  
	  //uint n_cl = column_CleanedVector.nelements() ;
	  
	  sorted_Vector = column_CleanedVector( Slice( init_Bin, blocksize )) ;
	  
	  casa::Matrix<DComplex> sorted_FFT_Vector = NuMoonTrigger::fft_data( sorted_Vector,
									      1,
									      nyquist_zone ) ;
	  
	  Matrix<DComplex> weight_Applied = NuMoonTrigger::weights_applied( sorted_FFT_Vector,
									    phases,
									    gain_scale_factor,
									    antenna ) ;
	  
	  beamformed_FFT = beamformed_FFT + weight_Applied.column(0) ;
	  
	}
	beamformed_FFT_dummy.column(0) = beamformed_FFT ;
	
	beamformed_IFFT = NuMoonTrigger::ifft_data( beamformed_FFT_dummy,
						    1,
						    nyquist_zone ) ;
	//uint n_b_IFFT = beamformed_IFFT.nelements() ;
	
	for( uint j=0; j< blocksize; j++){
	  
	  beamformed_Vector(init_Bin+j) = (beamformed_IFFT(j)) ;
	}
	init_Bin = init_Bin + blocksize ;
	
      }	
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger:: BeamFormed_data" << x.getMesg () << endl ;
      Vector<double>() ;
    }
    return beamformed_Vector ;	
  }
  
  //_____________________________________________________________________________
  //                                                             Added_SignalData

  Matrix<double> NuMoonTrigger::Added_SignalData( Matrix<double> const& data,
						  uint const& n_samples,
						  double const& simTEC,
						  uint const& nyquist_zone,
						  double const& peak_height,
						  double const& sampling_rate,
						  double const& TEC,
						  const double& source_latitude,
						  const double& source_longitude,
						  const double& pointing_latitude,
						  const double& pointing_longitude,
						  const Vector<double>& gain_scale_factor,
						  const Vector<double>& position_x,
						  const Vector<double>& position_y,
						  const Vector<double>& position_z,
						  const Vector<double> freq_range ) 
    
  {
    double pi      = 3.14159265358979323846 ;
    uint blocksize = 1024 ;
    uint nc_rcu    = data.ncolumn() ;
    uint nr_rcu    = data.nrow () ;
    uint n_frames  = n_samples/blocksize ;     
    
    casa::Matrix<double> AddedSignal_data(nr_rcu, nc_rcu, 0.0 ) ;
    casa::Vector<double> FREQvector;
    casa::Matrix<double> Cleaned_DATA;
    casa::Vector<double> Beamed_data;
    
    try {
      NuMoonTrigger numooontrigger ;
      
      
      FREQvector = NuMoonTrigger::freq_vector (sampling_rate,
					       nyquist_zone);
      
      
      Cleaned_DATA = NuMoonTrigger::Cleaned_data (data,
						  n_samples,
						  simTEC,
						  nyquist_zone,
						  sampling_rate,
						  TEC,
						  freq_range);
      
      Beamed_data = NuMoonTrigger::BeamFormed_data (Cleaned_DATA,
						    n_samples,
						    simTEC,
						    nyquist_zone,
						    sampling_rate,
						    TEC,
						    freq_range,
						    pointing_latitude,
						    pointing_longitude,
						    gain_scale_factor,
						    position_x,
						    position_y,
						    position_z) ; 
      
      Vector<double> Average_P = NuMoonTrigger::cal_AvPower( Beamed_data ) ;
      
      
      uint freq_channels = FREQvector.nelements() ;
      
      casa::Matrix<double> phaseDelay( freq_channels, nc_rcu,0.0 );
      
      casa::Vector<double> random_number( n_frames, 0.0 ) ;
      
      casa::Vector<double> pulse_LocVector( n_frames, 0.0 ) ;
      
      casa::Vector<double> rnd_PhaseVector( n_frames, 0.0 ) ;
      
      ACG gen(1, n_frames );
      
      for( uint nSample=0; nSample < n_frames; nSample++ ){
	Normal rnd(&gen, 0.0, 0.01 ); // with 0.04 generated random number lies within range of unity
	Double nextExpRand = rnd() ;
	random_number(nSample) = abs(nextExpRand) ;
	pulse_LocVector(nSample) =random_number(nSample)*512+200 ; 
	rnd_PhaseVector(nSample) =random_number(nSample)*2*pi ; 
      }
      gen.reset () ;
      
      phaseDelay = NuMoonTrigger::phase_delay( FREQvector,
					       source_latitude,
					       source_longitude,
					       position_x,
					       position_y,
					       position_z ) ;
      
      for( uint columns=0; columns< nc_rcu; columns++ ) {	  
	
	casa::Vector<double> antenna_data =  Cleaned_DATA.column( columns ) ;
	
	casa::Vector<double> geom_Weight_Vector = phaseDelay.column( columns ) ;
	
	casa::Vector<double> signal_Added( blocksize, 0.0) ;
	
	uint initin_value =0 ;
	
	double gain_factor = gain_scale_factor(columns) ;
	
	casa::Vector<double> rejoined_Vector( n_frames*blocksize,0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++ ){
	  
	  double Average_power = Average_P( frame ) ;
	  
	  casa::Vector<double> sort_vector = antenna_data( Slice(initin_value, blocksize )) ;
	  
	  double pulse_loc = pulse_LocVector(frame) ;
	  //cout << "pulse location : " << pulse_loc << endl ;
	  double random_phase = rnd_PhaseVector( frame ) ;
	  //cout << "random phase : " << random_phase <<endl ;
	  
	  double power_level = gain_factor*peak_height*sqrt( Average_power) ;
	  
	  signal_Added = numooontrigger.Sim_Signal(sort_vector,
						   simTEC,
						   FREQvector,
						   power_level,
						   pulse_loc,
						   random_phase,
						   geom_Weight_Vector ,
						   nyquist_zone ) ;
	  if( frame<1){
	    ofstream logfile2;
	    logfile2.open("Sim_signal",ios::out );
	    
	    for( uint r=0; r<blocksize; r++){
	      logfile2<< signal_Added(r) << endl ;
	    }
	    logfile2.close(); 
	  }			
	  for( uint sample_number=0; sample_number< blocksize; sample_number++ ){
	    
	    uint s_number = initin_value +sample_number ;
	    
	    rejoined_Vector( s_number ) = signal_Added( sample_number) ;
	    
	  }
	  //cout << "re joined vector :" << rejoined_Vector <<endl ;		
	  initin_value = initin_value + blocksize ;
	  
	}	
	
	AddedSignal_data.column( columns ) = rejoined_Vector ;
	
      }
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::Added_SignalData" << x.getMesg () << endl ;
      Matrix<double> ();
    }
    
    return AddedSignal_data ;
  }
  
  //___________________________________________________________________________________
  
  Vector<double> NuMoonTrigger::without_Signal(  Matrix<double> const& data,
    						 uint const& n_samples,
					         double const& simTEC,
						 double const& sampling_rate,
						 uint const& nyquist_zone,
						 uint const& time_int_bins,
  					         double const& TEC,
						 const double& source_latitude,
						 const double& source_longitude,
						 const double& pointing_latitude,
		   				 const double& pointing_longitude,
		   				 const Vector<double>& gain_scale_factor,
			    	  		 const Vector<double>& position_x,
		   	    	  		 const Vector<double>& position_y,
		   	    	  		 const Vector<double>& position_z,
						 const Vector<double>& ppf_coeff,
		   				 const Vector<double>& ppf_invcoeff,
		  				 const Vector<double> freq_range,
						 double const& peak_height ) 
  {
    casa::Matrix<double> Cleaned_DATA;
    Vector<double> beamformed_Timeseries(n_samples,0.0) ;
    casa::Vector<double> ppfCoefficients;
    casa::Vector<double> ppfCoefficientsInversion;
    uint timeIntegrationBins;
    double sourceLongitude;
    double sourceLatitude;
    double peakHeight;

    /* Avoid compiler warnings */
    timeIntegrationBins      = time_int_bins;
    ppfCoefficients          = ppf_coeff;
    ppfCoefficientsInversion = ppf_invcoeff;
    sourceLongitude          = source_latitude;
    sourceLatitude           = source_longitude;
    peakHeight               = peak_height;
    
    try{
      Cleaned_DATA = NuMoonTrigger::Cleaned_data (data,
						  n_samples,
						  simTEC,
						  nyquist_zone,
						  sampling_rate,
						  TEC,
						  freq_range) ;
      
      beamformed_Timeseries = NuMoonTrigger::BeamFormed_data( Cleaned_DATA,
							      n_samples,
							      simTEC,
							      nyquist_zone,
							      sampling_rate,
							      TEC,
							      freq_range,
							      pointing_latitude,
							      pointing_longitude,
							      gain_scale_factor,
							      position_x,
							      position_y,
							      position_z ) ; 
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::without_Signal" << x.getMesg () << endl ;
    }

    return beamformed_Timeseries ;
  }   
  
  //_____________________________________________________________________________
  //                                                                PPF_processed
  
  /*!
    \param data               -- 
    \param n_samples          -- 
    \param simTEC             -- 
    \param sampling_rate      -- 
    \param nyquist_zone       -- 
    \param time_int_bins      -- 
    \param TEC                -- 
    \param source_latitude    -- 
    \param source_longitude   -- 
    \param pointing_latitude  -- 
    \param pointing_longitude -- 
  */
  Vector<double> NuMoonTrigger::PPF_processed( Matrix<double> const& data,
					       uint const& n_samples,
					       double const& simTEC,
					       double const& sampling_rate,
					       uint const& nyquist_zone,
					       uint const& time_int_bins,
					       double const& TEC,
					       const double& source_latitude,
					       const double& source_longitude,
					       const double& pointing_latitude,
					       const double& pointing_longitude,
					       const Vector<double>& gain_scale_factor,
					       const Vector<double>& position_x,
					       const Vector<double>& position_y,
					       const Vector<double>& position_z,
					       const Vector<double>& ppf_coeff,
					       const Vector<double>& ppf_invcoeff,
					       const Vector<double> freq_range,
					       double const& peak_height ) 
    
  {
    uint blocksize = 1024;
    uint n_rcu     = data.ncolumn() ;
    casa::Vector<double> ppf_invertedData( n_samples, 0.0 ) ;
    casa::Vector<double>selected_channels( blocksize,0.0 );
    casa::Matrix<double> addedSignalData;
    casa::Vector<uint> selected_channelID;
    
    try{
      addedSignalData = NuMoonTrigger::Added_SignalData (data,
							 n_samples,
							 simTEC, 
							 nyquist_zone,
							 peak_height,
							 sampling_rate,
							 TEC,
							 source_latitude,
							 source_longitude,
							 pointing_latitude,
							 pointing_longitude,
							 gain_scale_factor,
							 position_x,
							 position_y,
							 position_z,
							 freq_range )  ;
      
      selected_channelID = NuMoonTrigger::PPFBand_vector (sampling_rate,
							  nyquist_zone,
							  freq_range);
      
      uint number_bands = selected_channelID.nelements() ;
      double bandwidth  = sampling_rate/blocksize;
      
      if (nyquist_zone==1){
	
	for(uint nband=0; nband< number_bands; nband++ ){
	  
	  selected_channels( nband ) = selected_channelID(nband)*bandwidth ;
	}
      }
      if (nyquist_zone==2){
	
	for(uint nband=0; nband< number_bands; nband++ ){
	  
	  selected_channels( nband ) = sampling_rate/2.0+selected_channelID(nband)*bandwidth ;
	}
      }
      
      
      uint n_frames = n_samples/blocksize ;
      
      casa::Matrix<double> Geometric_weights = NuMoonTrigger::phase_delay( selected_channels,
									   source_latitude,
									   source_longitude,
									   position_x,
									   position_y,
									   position_z ) ;
      
      Matrix<DComplex> Beam_formed( 513, n_frames, 0.0 ) ;
      
      for( uint antenna=0; antenna< n_rcu; antenna++ ) {
	
	casa::Vector<double> samples = addedSignalData.column( antenna ) ;
	
	casa::Matrix<DComplex> ppf_data_antenna = NuMoonTrigger::ppf_implement( samples,
										nyquist_zone,
										ppf_coeff ) ;
	
	//uint columns_ppfdata = ppf_data_antenna.ncolumn() ;
	
	casa::Matrix<DComplex> phase_corrected = NuMoonTrigger::weights_applied( ppf_data_antenna,
										 Geometric_weights,
										 gain_scale_factor,
										 antenna ) ;
	
	DComplex beamed_value(0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++ ){
	  
	  for( uint freQchannel =0 ; freQchannel< 513; freQchannel++ ){
	    
	    beamed_value = phase_corrected( freQchannel, frame);
	    
	    Beam_formed( freQchannel,frame) = Beam_formed( freQchannel,frame)+ beamed_value;
	  }
	}
	
      }
      
      //Beam_formed = Beam_formed*(1./n_rcu) ;
      
      casa::Matrix<DComplex> RFI_rejected = NuMoonTrigger::RFI_removal( Beam_formed ) ; 
      
      casa::Matrix<DComplex> de_dispersedData = NuMoonTrigger::de_dispersion( RFI_rejected,
									      TEC,
									      selected_channels ) ;
      
      
      casa::Matrix<DComplex> selected_PPFdata = NuMoonTrigger::ppfdata_cutshort( de_dispersedData,
										 selected_channelID ) ;
      
      ppf_invertedData = NuMoonTrigger::ppf_inversion( selected_PPFdata,
						       ppf_invcoeff,
						       selected_channelID )  ;
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::PPF_processed" << x.getMesg () << endl ;
      Vector<double> ();
    }
    return ppf_invertedData ;    
  }  
  
  //_____________________________________________________________________________
  //                                                                FFT_processed
  
  Vector<double> NuMoonTrigger::FFT_processed (Matrix<double> const& data,
					       uint const& n_samples,
					       double const& simTEC,
					       double const& sampling_rate,
					       uint const& nyquist_zone,
					       uint const& time_int_bins,
					       double const& TEC,
					       const double& source_latitude,
					       const double& source_longitude,
					       const double& pointing_latitude,
					       const double& pointing_longitude,
					       const Vector<double>& gain_scale_factor,
					       const Vector<double>& position_x,
					       const Vector<double>& position_y,
					       const Vector<double>& position_z,
					       const Vector<double>& ppf_coeff,
					       const Vector<double>& ppf_invcoeff,
					       const Vector<double> freq_range,
					       double const& peak_height ) 
  {
    Vector<double> timeSeries_FFT( n_samples,0.0 );
    casa::Matrix<double> addedSignalData;
    casa::Vector<double> ppfCoefficients;
    casa::Vector<double> ppfCoefficientsInversion;
    uint timeIntegrationBins;

    /* Avoid compiler warnings */
    timeIntegrationBins      = time_int_bins;
    ppfCoefficients          = ppf_coeff;
    ppfCoefficientsInversion = ppf_invcoeff;
    
    try{
      addedSignalData = NuMoonTrigger::Added_SignalData( data,
							 n_samples,
							 simTEC, 
							 nyquist_zone,
							 peak_height,
							 sampling_rate,
							 TEC,
							 source_latitude,
							 source_longitude,
							 pointing_latitude,
							 pointing_longitude,
							 gain_scale_factor,
							 position_x,
							 position_y,
							 position_z,
							 freq_range )  ;
      
      // addedSignalData = addedSignalData*512. ;
      timeSeries_FFT = NuMoonTrigger::BeamFormed_data( addedSignalData,
						       n_samples,
						       simTEC,
						       nyquist_zone,
						       sampling_rate,
						       TEC,
						       freq_range,
						       pointing_latitude,
						       pointing_longitude,
						       gain_scale_factor,
						       position_x,
						       position_y,
						       position_z ) ;
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::FFT_processed" << x.getMesg () << endl ;
      Vector<double> ();
    }
    return timeSeries_FFT ;
  }   
  
  //_____________________________________________________________________________
  //                                                                 root_ntuples
  
  void NuMoonTrigger::root_ntuples( Matrix<double> const& data,
				    uint const& n_samples,
				    double const& simTEC,
				    double const& sampling_rate,
				    uint const& nyquist_zone,
				    uint const& time_int_bins,
				    double const& TEC,
				    const double& source_latitude,
				    const double& source_longitude,
				    const double& pointing_latitude,
				    const double& pointing_longitude,
				    const Vector<double>& gain_scale_factor,
				    const Vector<double>& position_x,
				    const Vector<double>& position_y,
				    const Vector<double>& position_z,
				    const Vector<double>& ppf_coeff,
				    const Vector<double>& ppf_invcoeff,
				    const Vector<double> freq_range,
				    double const& peak_height )
    
  {
    cout << "To store data informaton in ntuples" << endl ;
    
    try {
      uint n_frames (n_samples/1024);
      casa::Matrix<double> Cleaned_DATA;
      casa::Vector<double> beamed_Array;
      casa::Vector<double> No_Signal;
      casa::Vector<double> FFT_Signal;
      casa::Vector<double> Average_P;
      
      TNtuple *eventuple_FFT = new TNtuple ("eventuple_FFT",
					    "eventuple_FFT",
					    "time:frame:Average_P5:sample_value") ;
      TNtuple *eventuple_raw = new TNtuple ("eventuple_raw",
					    "eventuple_raw",
					    "time:frame:Average_P5:sample_value") ;
      /* Clean-up the data */
      Cleaned_DATA = NuMoonTrigger::Cleaned_data(  data,
						   n_samples,
						   simTEC,
						   nyquist_zone,
						   sampling_rate,
						   TEC,
						   freq_range ) ; 
      /* Retrieve the beamformed data */
      beamed_Array = NuMoonTrigger::BeamFormed_data(  Cleaned_DATA,
						      n_samples,
						      simTEC,
						      nyquist_zone,
						      sampling_rate,
						      TEC,
						      freq_range,
						      pointing_latitude,
						      pointing_longitude,
						      gain_scale_factor,
						      position_x,
						      position_y,
						      position_z ) ;   
      
      Average_P = NuMoonTrigger::cal_AvPower( beamed_Array ) ;
            
      No_Signal = NuMoonTrigger::without_Signal(  data,
						  n_samples,
						  simTEC,
						  sampling_rate,
						  nyquist_zone,
						  time_int_bins,
						  TEC,
						  source_latitude,
						  source_longitude,
						  pointing_latitude,
						  pointing_longitude,
						  gain_scale_factor,
						  position_x,
						  position_y,
						  position_z,
						  ppf_coeff,
						  ppf_invcoeff,
						  freq_range,
						  peak_height ) ; 
      
      FFT_Signal = NuMoonTrigger::FFT_processed( data,
						 n_samples,
						 simTEC,
						 sampling_rate,
						 nyquist_zone,
						 time_int_bins,
						 TEC,
						 source_latitude,
						 source_longitude,
						 pointing_latitude,
						 pointing_longitude,
						 gain_scale_factor,
						 position_x,
						 position_y,
						 position_z,
						 ppf_coeff,
						 ppf_invcoeff,
						 freq_range,
						 peak_height ) ;     
      
      casa::Vector<double> Average_P_ws= NuMoonTrigger::cal_AvPower( FFT_Signal ) ;
      
      uint SAP (0);
      uint sample_max (0);
      uint sample_min (0);
      double Average_P5 (0);
      double time_Bin (0);
      double bin_integrated (0);
      
      for( uint frame=0; frame< n_frames; frame++ ){
	
	sample_max = SAP+1024 ;
	sample_min = SAP ;
	Average_P5 = Average_P(frame ) ;
	
	for( uint sample= sample_min+50 ; sample< sample_max-50; sample++ ){
	  
	  bin_integrated = 0;
	  
	  for (uint j=sample; j<(sample+5); j++) {
	    time_Bin = FFT_Signal(j) ;
	    bin_integrated = bin_integrated + time_Bin*time_Bin ;
	  }
	  if (bin_integrated > 3.0*Average_P5) {
	    cout << "Eventuple has been written at sample : "<< sample << endl;
	    eventuple_FFT->Fill (sample,
				 frame,
				 Average_P5,
				 bin_integrated/Average_P5);
	    sample = sample_max-50 ;
	  }
	}
	SAP = sample_max ;
      }
      
    uint s_m_e =0;
    
    for( uint fra=0; fra< n_frames; fra++ ) {
      sample_max = s_m_e+1024 ;
      sample_min = s_m_e ;
      Average_P5 = Average_P(fra ) ;
      
      for( uint samp= sample_min+50 ; samp< sample_max-50; samp++ ){
	
	double bin_int =0 ;
	
	for (uint j=samp; j<(samp+5); j++) {
	  double t_Bin = No_Signal(j) ;
	  bin_int = bin_int + t_Bin*t_Bin ;
	}
	
	if( bin_int > 3.0*Average_P5){
	  
	  eventuple_raw->Fill( samp, fra, Average_P5, bin_int/Average_P5 ) ;
	  //uint bin_1 = bin_0 ;
	  samp = sample_max-50 ;
	}
      }
      s_m_e = sample_max ;
    }
    
    eventuple_FFT->Write() ;
    eventuple_raw->Write() ;
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger:: root_ntuple" << x.getMesg () << endl ;
    }
    // return Matrix<DComplex> Geom_weights ;
  }
  
  
#endif 
  
} // Namespace  -- end
