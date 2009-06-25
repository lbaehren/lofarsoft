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
  
  NuMoonTrigger::NuMoonTrigger( std::string const &filename,
    		   		uint const& n_samples,
				double const& simTEC,
		   		double const& sampling_rate,
		   		uint const& nyquist_zone,
				uint const& time_int_bins,
		   		double const& TEC,
		   		const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		   		const Vector<double> freq_range,
				double const& peak_power,
				const Vector<uint>& RCU_id ) 
  			
 {
  //n_frames = 0 ;
  }
  
  NuMoonTrigger::NuMoonTrigger ( NuMoonTrigger const &other)
  {
  //  copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  NuMoonTrigger::~NuMoonTrigger ()
  {
    destroy();
  }
  
  void NuMoonTrigger::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  NuMoonTrigger& NuMoonTrigger::operator= (NuMoonTrigger const &other)
  {
    if (this != &other) {
      destroy ();
    //  copy (other);
    }
    return *this;
  }
  
//   void NuMoonTrigger::copy (NuMoonTrigger const &other)
//   {;}

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

  //_____________________________________________________________________________
  //                                                                 reading_data
  
  Matrix<double> NuMoonTrigger::reading_data ( std::string const &filename,
					       uint const& n_frames ) 
  {
    Matrix<double> data ;
    uint blocksize (1024);
    uint nofSamples (n_frames*blocksize);
    uint start (0);
    
    try {
      /* Create object to connect to the data file*/
      DAL::TBB_Timeseries timeseries;
      /* Read a block of data */
      data = timeseries.fx( start,
			    nofSamples);
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::reading_data " << x.getMesg () << endl ;
    }
    
    return data ;
  }  
  
  //_____________________________________________________________________________
  
    casa::Matrix<DComplex> NuMoonTrigger::fft_data( Vector<double>&  data,
    		 	     		            uint const& n_frames,
					            uint const& nyquist_zone ) 
    {
      uint dataBlockSize = 1024 ;
      uint n_freqbins    = dataBlockSize/2+1 ;
      Matrix<DComplex> fft_implemented( n_freqbins, n_frames, 0.0 );
      
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
	fft_implemented = Matrix<DComplex>() ;
      }
      
      return fft_implemented;	
    }
  
  //_____________________________________________________________________________
  //                                                                  RFI_removal
  
  Matrix<DComplex> NuMoonTrigger::RFI_removal( Matrix<DComplex> fft_samples ) 
  {
    
    
    try {
      uint n_row = fft_samples.nrow() ;
	   
	   uint n_colum =fft_samples.ncolumn() ;
	   
     	  uint dataBlockSize = 1024 ;
	  
	  DComplex expo(0, 1);
	   
	  uint n_segments = 3 ; //n_freqbins/16 ;
	  
	  casa::Matrix<double> phase_array(n_row, n_colum,0.0 ) ;
	  phase_array = phase(fft_samples);
	  
	  casa::Matrix<double> amplitude_spectra(n_row, n_colum,0.0 ) ;
	   
	 amplitude_spectra = amplitude(fft_samples) ;
	  //uint n_columns = phase_array.ncolumn() ;
	  //casa::Matrix<double> phase_array = phase( fft_samples ) ;
	  
	  casa::Matrix<DComplex> array_phases( n_row, n_colum,0.0 ) ;
	  
	  for(uint i=0; i<n_row ; i++ ){
	  
	  	for(uint j=0; j<n_colum; j++ ){
		
			double phase_value = phase_array(i,j) ;
			
			array_phases(i,j) = exp(expo*phase_value) ;
			
			}
			
		}
			  
	  CR::RFIMitigation  rfi( fft_samples,
	  		          dataBlockSize,
			          n_segments ) ;
	  
	  casa::Matrix<double> RFIrejected_array( n_row, n_colum,0.0 ) ;
	   
	  RFIrejected_array= rfi.getOptimizedSpectra( fft_samples,
	 	                                      dataBlockSize,
						      n_segments  ) ;	
	 
	  casa::Matrix<double> RFIrejected_absolutearray( n_row, n_colum,0.0 ) ;
	  
	  RFIrejected_absolutearray = RFIrejected_array*amplitude_spectra ;
	  	      
	  casa::Matrix<DComplex> RFIreject_array( n_row, n_colum,0.0 ) ;  ;
	  
	  convertArray( RFIreject_array, RFIrejected_absolutearray ) ;
	  
	  casa::Matrix<DComplex> RFIMitigated_array( n_row, n_colum,0.0 ) ;   
	  
	  RFIMitigated_array = RFIreject_array*array_phases  ;
	  
	  return RFIMitigated_array ;   
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::RFI_removal " << x.getMesg () << endl ;
       return Matrix<DComplex> ();
       }
      
   } 						

   
//______________________________________________________________________________________

 Vector<double> NuMoonTrigger::freq_vector( double const& sampling_rate,
		   	      		    uint const& nyquist_zone ) 
 {
   // casa::Vector<double> frequency_Vector ;
	
   try {
   
   	  double channel_width = sampling_rate/1024. ;			 
	   
	   Vector<double> frequency_Vector(513,0.0) ;
	  
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
		
	  return frequency_Vector ;   
 	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::freq_vector" << x.getMesg () << endl ;
       return Vector<double>() ;
        }
    
   }
   
    //_______________________________________________________________________________
		     
   Matrix<DComplex> NuMoonTrigger::de_dispersion( Matrix<DComplex> fft_samples,
		      		  		  double const& TEC,
						  const Vector<double> freq_Vector ) 
   {
   	
	
     try {
     		
     	  double pi = 3.14159265358979323846 ;
	  
	  DComplex expo(0, 1);
	  
	  DComplex expr = exp(expo*0.0) ;
     	  
	  uint dataBlockSize = 1024 ;
	  
	  uint n_freqbins = dataBlockSize/2+1 ;
	  
	  uint ncloumn = fft_samples.ncolumn() ;
	  
	  uint nrow = fft_samples.nrow() ;
	  
	  double DM = 1.34e+9*TEC*1.0 ;
	  
	  double t0 = 8.e+5/3.e+8 ;
	  
	  double modified_phase(0.0) ;
	  
	  casa::Matrix<double> phase_array(nrow, ncloumn, 0.0) ;
	  
	  phase_array = ( phase(fft_samples));
	  
	  uint n_columns = phase_array.ncolumn() ;
	  
	  uint n_rows = phase_array.nrow() ;
	  //Matrix<double> phase_array = phase(fft_samples) ;
	  casa::Matrix<DComplex> dispersed_modified_array( n_rows, n_columns, 0.0 ) ;
	  	  
	  casa::Matrix<double> amp_array(nrow, ncloumn, 0.0) ;
	  
	  amp_array = amplitude(fft_samples) ;
	  	 
	  casa::Matrix<DComplex> amp_amplified_array( nrow, ncloumn, 0.0 ) ; 
	  
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
	  
	  return dispersed_modified_array ;		
     }
     
     catch ( AipsError x ){
       cerr << " NuMoonTrigger::de_dispersion " << x.getMesg () << endl ;
       return Matrix<DComplex> ();
     }
     
   } 
  
  
  //_____________________________________________________________________________   
  
   Vector<double> NuMoonTrigger::ifft_data( Matrix<DComplex> const& data,
    			    		   uint const& n_frames,
					   uint const& nyquist_zone ) 
   {
   	
     try {
     	   uint dataBlockSize = 1024 ;
	  
	   uint n_freqbins = dataBlockSize/2+1 ;
     	  
	   casa::Vector<DComplex> FFTVector( n_freqbins,0.0) ;
	   
	   FFTServer <double,DComplex> server ;
     	  
	   casa::Vector<double> IFFTVector( dataBlockSize, 0.0 );
     
     	   casa::Vector<double> timeSeries( dataBlockSize*n_frames, 0.0 ) ;
           
	   uint sample_n(0) ;
	   
	    for( uint i=0; i< n_frames; i++ ){
      
     	 	 casa::Vector<DComplex> sliced_vector = data.column(i) ;
      
      	//	 server.fft( FFTVector, sliced_vector ) ;
      
		  switch( nyquist_zone ){
	  
	  		case 1:  
      		 		FFTVector = sliced_vector ;
				
				break ;
				
			case 2:
				for( uint channel(0); channel< n_freqbins; channel++ ){
				
					FFTVector(n_freqbins-channel-1)=conj( sliced_vector( channel)) ;
					
					}
    				break ;
			}
		server.fft(IFFTVector, FFTVector ) ;
			
    		 for( uint j=0; j< dataBlockSize; j++ ) {
 	  
			sample_n = i*dataBlockSize+j ;
 	       		timeSeries( sample_n )= IFFTVector(j) ;
 	       
 	          }
 	       
            }
	    
	 // cout << "total sample number :" << sample_n << "number of frames : " << n_frames <<endl ;
         return timeSeries ;
	 }
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ifft_data " << x.getMesg () << endl ;
        }
     return Vector<Double> ();
   }
   
   
//__________________________________________________________________________________

       
  Matrix<double> NuMoonTrigger::cal_AvPower( Matrix<double> const& data_array,
  		              	   	     uint const& n_frames ) 
				     
  {
  	
	
     try {
     
       	  uint dataBlockSize = 1024 ;
	  
	  uint n_column = data_array.ncolumn() ;
	  
	  Matrix<double> P5( dataBlockSize*n_frames-5, n_column, 0.0 ) ;
	  
	  Matrix<double> Average_power( n_frames,n_column, 0.0 ) ;
	  
	  for(uint i=0; i<n_column; i++ ){
	  
	  	for( uint j=0; j<n_frames; j++ ){
		
			uint dataBlock = j*dataBlockSize ;
		
			double average_power(0.0) ;
			
			for( uint k=dataBlock; k< (j+1)*dataBlockSize-5 ; k++ ){
			
				double Power_5(0.0) ;
				
				for( uint l=k; l<(k+5); l++ ){
				
					double value = data_array(l,i) ;
					
					//cout << " value of data array element :" << value << endl ;
				
					Power_5 += value*value ;
					
					//cout << " Power_5 : " <<Power_5 <<endl ;
					
					}
					
					P5(k,i) = Power_5 ;
					
					average_power += Power_5 ;
				}
			
			Average_power(j,i) = average_power/1024. ;
	  
		}	  
         }
	return Average_power ;
		
      }
     
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::cal_AvPower " << x.getMesg () << endl ;
       return Matrix<double> ();
       }
	
  }   
     
     
//_____________________________________________________________________________________

   Matrix<DComplex> NuMoonTrigger::ppf_implement( Vector<double>& samples,
   						  uint const& nyquist_zone,
                                  		  const Vector<double>& ppf_coeff ) 
   {
     
     try { 
     	   uint dataBlockSize = 1024 ;
     		
           uint n_freqbins = dataBlockSize/2+1 ;
	   
	   Matrix<DComplex> ppf_implemented ;
     	   
	   CR::ppfimplement ppfimpl( samples,
	   			     ppf_coeff ) ;
	   
	  ppf_implemented = ppfimpl.FFTSamples( samples,
                               			 ppf_coeff ) ;
						 
          uint n_columns = ppf_implemented.ncolumn() ;
	  
	  switch( nyquist_zone ){
	  
	  		case 1:  
				for( uint i=0; i<n_columns ; i++ ){
				
					ppf_implemented.column(i) = ppf_implemented.column(i) ;
					
					}
				
				break ;
				
			case 2:
			
				for( uint i=0; i<n_columns ; i++ ){
				
					for( uint channel(0); channel< n_freqbins; channel++ ){
				
						ppf_implemented(channel, i )=conj( ppf_implemented(( n_freqbins-channel-1),i)) ;
					
					}
				}
    				break ;
			}	
        return ppf_implemented ;   
     	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppf_implement " << x.getMesg () << endl ;
       return Matrix<DComplex> ();
       }
      
   } 
    
   
 //________________________________________________________________________________________
 
 Matrix<DComplex> NuMoonTrigger::ppfdata_cutshort( Matrix<DComplex>& samples,
   						   Vector<uint> subband_ID ) 
    {
              
     try {
	 
     	  uint nOfelements = subband_ID.nelements() ;
	  
	  uint n_columns = samples.ncolumn() ;
	  
      	  Matrix<DComplex> sortened_array( 1024, n_columns, 0.0 ) ;
	  
	  for( uint i=0; i< nOfelements; i++ ){
	  
	  	uint band_ID = subband_ID(i) ;
	  
	  	Vector<DComplex> sample_vector = samples.row( band_ID-1 ) ;
		
		sortened_array.row( band_ID-1 ) = sample_vector ;
		
		}
	 return sortened_array;
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppfdata_cutshort" << x.getMesg () << endl ;
       return Matrix<DComplex> (); 
	}
     
   } 
    
 
 
 //________________________________________________________________________________________
 
   Vector<uint> NuMoonTrigger::PPFBand_vector( double const& sampling_rate,
   				       	       uint const& nyquist_zone,
				               const Vector<double> freq_range ) 
   {
     
     try {
   	   double channel_width = sampling_rate/1024. ;			 
	   
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
			
		double init = freq_range(0) ;
		
		double final = freq_range(1) ;
		
		uint k=0; 
		
		int number_channels = int((final-init)/channel_width) ;
		 
		Vector<uint> selected_channelID( number_channels, 0) ;
		
		for( uint j=0; j<513; j++ ){
		
			double compare = frequency_vector(j)   ;
			
			if( compare >init && compare <final) {
			
				uint bandid = band_ID(j) ;
				
				selected_channelID(k) = bandid ;
				
				k = k+1 ;
				
			}
		}
	 return selected_channelID; 	
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::PPFBandID_vector" << x.getMesg () << endl ;
       return Vector<uint> () ;
	}
      
   } 
   
//____________________________________________________________________________________________
 
  Vector<double> NuMoonTrigger::ppf_inversion( const Matrix<DComplex>& FTData, 
					       const Vector<double>& ppf_invcoeff,
					       const Vector<uint> subBand_ID )  
    
  {
    
    
    try {
      Vector<Double> ppfinverted_Vector ;
      
      CR::ppfinversion ppf_Invert( FTData,
				   ppf_invcoeff,
				   subBand_ID ) ;
      
      ppfinverted_Vector = ppf_Invert.FIR_inversion( ppf_invcoeff,
						     FTData,
						     subBand_ID ) ;
      return ppfinverted_Vector ;
      
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger::ppf_inversion " << x.getMesg () << endl ;
      return Vector<Double>();
    }
    
  } 
  
  //______________________________________________________________________________________
   
  Matrix<DComplex> NuMoonTrigger::Geom_weights( const Vector<uint>& RCU_id,
					       const Vector<double>& freq_Vector ) 
    {
         
     try {
      
     	  double pi = 3.14159265358979323846 ;
	  
	  DComplex expo(0, 1);
	  
	  uint freq_channels = freq_Vector.nelements() ;
	  
	  uint nOfelements = RCU_id.nelements() ;
	  
	  double speed_light = 3e8 ;
	  
	  Matrix<DComplex> weight_factors( freq_channels, nOfelements, 0.0 ) ; 
	  
     	  double long_deg = 6.0 ;
	  double long_min = 52.0 ;
	  
	  double radius = 6371e3 ;
	  
	  Vector<double> long_sec( 8, 0.0 ) ;
	 
	  long_sec(0) = 12.61461 ;
	  long_sec(1) = 12.61461 ;
	  long_sec(2) = 12.62020 ;
	  long_sec(3) = 12.62020 ;
	  long_sec(4) = 12.34627 ;
	  long_sec(5) = 12.34627 ;
	  long_sec(6) = 12.34070 ;
	  long_sec(7) = 12.34070 ;	
	    
	  double lat_deg = 52.0 ;
	  double lat_min = 54.0 ;
	 
	  Vector<double> lat_sec( 8, 0.0 ) ;
	 
	  lat_sec(0) = 47.90154 ;
	  lat_sec(1) = 47.90154 ;
	  lat_sec(2) = 48.06707 ;
	  lat_sec(3) = 48.06707 ;
	  lat_sec(4) = 48.07043 ;
	  lat_sec(5) = 48.07043 ;
	  lat_sec(6) = 47.90490 ;
	  lat_sec(7) = 47.90490 ;
	  
	  Vector<double> height( 8, 0.0 ) ;
	  height(0)= 51.163 ;
	  height(1)= 51.163 ;
	  height(2)= 51.020 ;
	  height(3)= 51.020 ;
	  height(4)= 51.020 ;
	  height(5)= 51.020 ;
	  height(6)= 51.163 ;
	  height(7)= 51.163 ;
	  
	  Vector<double> height_diff( 8, 0.0 ) ;
	  
	  for( uint h =0; h<8; h++ ) {
	  
	  	  height_diff(h)= height(h) -height(0) ;
	    }
	  
	  Vector<double> latitude( nOfelements, 0.0) ;
	  Vector<double> longitude( nOfelements, 0.0 ) ;
	  
	  for( uint i=0; i< nOfelements; i++ ){
	  
	      // uint rcu_number = RCU_id(i) ;
	       
	  	latitude(i) = (pi/180.)*(lat_deg +(1./60.)*lat_min +(1./60.)*lat_sec(i)) ;
		longitude(i) = (pi/180.)*(long_deg +(1./60.)*long_min +(1./60.)*long_sec(i)) ;
		
		}
	  Vector<double> lat_diff( nOfelements, 0.0 ) ;
	  Vector<double> long_diff( nOfelements, 0.0 ) ;	
	  
	  Vector<double> baseline( nOfelements, 0.0 ) ;
	  
	  Vector<double> source_distance( nOfelements, 0.0 ) ;
	  
	  Vector<double> Geom_delay( nOfelements, 0.0 ) ;
	  
	  double a, x, y, c ;
	  
	  for( uint j=0; j< nOfelements; j++ ){
	  
	  	//uint rcu_n = RCU_id(j) ;
	  	
		lat_diff(j) = latitude(j)- latitude(0) ;
		long_diff(j) = longitude(j)- longitude(0) ;
		
 a=(sin(0.5*lat_diff(j)))*(sin(0.5*lat_diff(j)))+cos(latitude(j))*cos(latitude(j))*(sin(0.5*long_diff(j)))*(sin(0.5*long_diff(j))) ;

  		y = sqrt(a) ;
	 	x = sqrt(1.-a) ;
	 
	 	c =2*atan2(y,x) ;
	 
	 	baseline(j) = radius*c ;
		
		source_distance(j)= sqrt((1.+height_diff(j))*(1.+height_diff(j))+baseline(j)*baseline(j)) ;
		
		double baseline_square = baseline(j)*baseline(j) ;
		
		double source_dis_square = source_distance(j)*source_distance(j) ;
			        
Geom_delay(j) =(source_distance(j)/speed_light)*(sqrt(1+((baseline_square-2*source_distance(j)*baseline(j))/(source_dis_square)))-1.) ;
	 
		
	}
	cout << " geom_0 :" << Geom_delay(0) << "\n" << " geom_1 :" << Geom_delay(1) << "\n" << " geom_2 :" << Geom_delay(2) << "\n" << " geom_3 :" << Geom_delay(3) << "\n" << " geom_4 :" << Geom_delay(4) << "\n" << " geom_5 :" << Geom_delay(5) << "\n" << " geom_6 :" << Geom_delay(6) << "\n" << " geom_7 :" << Geom_delay(7) << endl;
	
	//Geom_delay(1) = Geom_delay(0)-(pi/2.)/sampling_rate ;
	
	 for( uint g=0; g< nOfelements; g++ ){

	 	for( uint f=0; f< freq_channels; f++ ){
		
			double freq_val = freq_Vector(f) ;
			
			double phase_factor = 2.*pi*freq_val*Geom_delay(g) ;
			
// 			if(g==1){
//  				phase_factor =phase_factor + pi ;
//  				}
// 			if(g==2){
// 				phase_factor =phase_factor - pi/3. ;
//  				}
// 			if(g==4){
// 				phase_factor =phase_factor - 2.*pi/3. ;
//  				}
// 			if(g==5){
// 				phase_factor =phase_factor + pi/3. ;
//  				}	
// 			if(g==6){
// 				phase_factor =phase_factor + pi ;
//  				}	
// 			if(g==7){
// 				phase_factor =phase_factor + 2.*pi/3. ;
//  				}
			weight_factors(f,g) = DComplex(cos(phase_factor),sin(phase_factor)) ;
			
			}
		}	
	 
	  
  	return weight_factors ;
	
       } 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::Geom_weights " << x.getMesg () << endl ;
       return Matrix<DComplex> ();
     }
   
  }
  
  //____________________________________________________________________________________

    Matrix<DComplex> NuMoonTrigger::weights_applied( Matrix<DComplex> const& data,
  					             Matrix<DComplex> const& geom_weights,
						     uint const& rcu_id ) 
  
    {
    	
     try {
     
     	   uint n_columns = data.ncolumn() ;
	   
	   uint n_rows = data.nrow() ;
	  
	   casa::Matrix<DComplex> data_phase_corrected(n_rows, n_columns, 0.0 ) ;
	   
	   uint r_gmw = geom_weights.nrow() ;
	     
	   Vector<DComplex> geom_vector(r_gmw,0.0) ;
	    
	   geom_vector = geom_weights.column( rcu_id ) ;
	   
	   Vector<DComplex> data_vector( n_rows, 0.0 );
	   
	   Vector<DComplex> weight_applied_vector( n_rows, 0.0 ) ;
	   	   
	   for( uint i=0; i< n_columns; i++ ) {
	   
	   	data_vector = data.column(i) ;
		
		weight_applied_vector = data_vector*geom_vector ;
		
		DComplex weight_applied(0.0);
		
		for( uint j=0; j< n_rows; j++ ){
		
			weight_applied = weight_applied_vector(j) ;
			
			data_phase_corrected(j,i) = weight_applied ;
		}
	   }
        return data_phase_corrected ;
     	} 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::weights_applied " << x.getMesg () << endl ;
       return Matrix<DComplex> ();
	}
   
  }
  //_________________________________________________________________
  
   Vector<double> NuMoonTrigger::Sim_Signal( Vector<double> const& data,
    		 	     		    double const& simTEC,
					    const Vector<double>& freq_Vector,
					    double const& peak_height,
					    double const& pulse_loc,
					    double const& rnd_phase,
					    Vector<DComplex> geom_Weights_factor  ) 
   {
   		
     try {
     	
     	  double pi = 3.14159265358979323846 ;
	  
	  uint dataBlockSize = 1024 ;
	  
	  uint n_freqbins = dataBlockSize/2 +1 ;
	  
     	  double DM, t0; // peak_height(0.0) ;
	  
	 // peak_height = sqrt(peak_power ) ;
	  uint n_elements = data.nelements() ;
	  cout << "data elements used for simulating signal in real data :" << n_elements <<endl ;
	  
	  Vector<double>  addedSignal( n_elements,0.0 ) ;
	  
	  DComplex expo(0, 1);
	  
	  DComplex expr = exp(expo*0.0) ;
	  
	  casa::Vector<double> amp( n_freqbins, 0.0 ) ;
	  
	  casa::Vector<double> phase_s( n_freqbins, 0.0 ) ;
	  
	  casa::Vector<DComplex> phased_vector( n_freqbins, 0.0 ) ;
	  
	  FFTServer <Double,DComplex> server ;
     	 
	  DM = 1.34e+9*simTEC*1.0 ;
	  
	  t0 = 8.e+5/3.e+8 ;
	  
	  casa::Vector<double> geom_phase = phase(geom_Weights_factor) ;
	  
	 // double geom_phase ;   
	   
	  for( uint i=0; i< n_freqbins; i++ ) {
	  
	  //	cout << "peak height of the pulse that has to be added : "<< peak_height <<"\t" ;
		
		amp(i) = (peak_height/1024.) ;
		
	//	cout << "Amplitude of signal in a bin : " << amp(i) << endl ;
		
		phase_s(i) = rnd_phase + (1.-(pulse_loc/1024.))*2.*pi*i - geom_phase(i);
		
	//	cout << "Phase of signal that has been added : " << phase_s(i) <<endl ;
		
		double nu = freq_Vector(i) ;
		
		phase_s(i) -= 2.*pi*sqrt(1.-2.*DM/t0/(nu*nu))*nu*t0 -2.*pi*nu*t0 ;
		
		phased_vector(i) = exp(expo*phase_s(i)) ;
			
		}
		
	 casa::Vector<DComplex> amp_vector( n_freqbins, 0.0 )  ;
	 
	 convertArray( amp_vector, amp ) ;
	 
	 casa::Vector<DComplex> dispersed_signalVector( n_freqbins, 0.0 ) ;
	  
	 dispersed_signalVector = amp_vector*phased_vector  ;
	
	casa::Vector<double> signal_Vector ( dataBlockSize,0.0 ) ;
	
	server.fft( signal_Vector, dispersed_signalVector ) ;
	
	double signal(0.0) ;
	
  	for( uint j=0; j< dataBlockSize; j++ ) {
	  
 	        signal = 1024.*signal_Vector(j);   
		
		signal = 2.0*int(signal/2.0) ;
		
		if(signal>2047) {signal=2047 ;} 
		if(signal<-2048) {signal=-2048 ;} 
		
		double data_value =data(j) ;
		
		addedSignal(j) = signal +data_value ;
		
              }
	      
	
	
	return addedSignal ;	  
     }
     
     catch ( AipsError x ){
       cerr << " NuMoonTrigger::Sim_Signal " << x.getMesg () << endl ;
       return Vector<double>() ;
     }
     
     
   }
  
  
  //_____________________________________________________________________________________
  
  Matrix<double> NuMoonTrigger::Cleaned_data(  std::string const &filename,
					       uint const& n_samples,
					       double const& simTEC,
					       uint const& nyquist_zone,
					       double const& peak_power,
					       const Vector<uint>& RCU_id,
					       double const& sampling_rate,
					       double const& TEC,
					       const Vector<double> freq_range ) 
  {
    
    
    try {
      
      CR::LOFAR_TBB lofar_tbb( filename, n_samples ) ;
      
      casa::Matrix<double> data = lofar_tbb.fx( ) ;	
      
      CR::NuMoonTrigger moontrigger() ;
      
      uint nc_rcu = data.ncolumn() ;
      
      uint n_frames = n_samples/1024 ;
      
      casa::Vector<double> FREQvector = NuMoonTrigger::freq_vector( sampling_rate,
								    nyquist_zone ) ;
      casa::Matrix<double> Cleaned_data(n_samples, nc_rcu,0.0 ) ;  
      
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
      
      
      return Cleaned_data ;	       
    }
    
    catch ( AipsError x ){
      cerr << " NuMoonTrigger:: Cleaned_data" << x.getMesg () << endl ;
      return Matrix<double>() ;
    }
    
    
  }
  
  //_____________________________________________________________________________
  //                                                             Added_SignalData
  
  Matrix<double> NuMoonTrigger::Added_SignalData( std::string const &filename,
						  uint const& n_samples,
						  double const& simTEC,
						  uint const& nyquist_zone,
						  double const& peak_power,
						  const Vector<uint>& RCU_id,
						  double const& sampling_rate,
						  double const& TEC,
						  const Vector<double> freq_range ) 
  {
    double pi = 3.14159265358979323846 ;
    uint dataBlockSize = 1024 ;
    uint n_frames = n_samples/dataBlockSize ;     
    
    try {
      casa::Vector<double> FREQvector = NuMoonTrigger::freq_vector( sampling_rate,
								    nyquist_zone);
      
      Matrix<double> Cleaned_data = NuMoonTrigger::Cleaned_data(  filename,
								  n_samples,
								  simTEC,
								  nyquist_zone,
								  peak_power,
								  RCU_id,
								  sampling_rate,
								  TEC,
								  freq_range ) ;
      
      uint nc_rcu = Cleaned_data.ncolumn() ;
      uint nr_rcu = Cleaned_data.nrow () ;
      
      casa::Matrix<double> Average_P = NuMoonTrigger::cal_AvPower( Cleaned_data,
								   n_frames )  ;
      casa::Vector<double> random_number( n_frames, 0.0 ) ;
      
      casa::Vector<double> pulse_LocVector( n_frames, 0.0 ) ;
      
      casa::Vector<double> rnd_PhaseVector( n_frames, 0.0 ) ;
      
      ACG gen(1, n_frames );
      
      for( uint nSample=0; nSample < n_frames; nSample++ ){
	Normal rnd(&gen, 0.0, 0.04 );
	Double nextExpRand = rnd() ;
	random_number(nSample) = abs(nextExpRand) ;
	pulse_LocVector(nSample) =random_number(nSample)*1000 ;
	rnd_PhaseVector(nSample) =random_number(nSample)*2*pi ; 
      }
      // Resetting the generator, should get the same numbers.
      gen.reset () ;
      
      casa::Matrix<DComplex> geomWeights  = NuMoonTrigger::Geom_weights( RCU_id,
									 FREQvector ) ;
      
      casa::Matrix<double> AddedSignal_data(nr_rcu, nc_rcu, 0.0 ) ;
      
      for( uint columns=0; columns< nc_rcu; columns++ ) {	  
	
	casa::Vector<double> antenna_data = Cleaned_data.column( columns ) ;
	
	casa::Vector<DComplex> geom_Weight_Vector = geomWeights.column( columns ) ;
	
	casa::Vector<double> signal_Added( dataBlockSize, 0.0) ;
	
	uint initin_value =0 ;
	
	casa::Vector<double> rejoined_Vector( n_frames*dataBlockSize,0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++ ){
	  
	  double Average_power = Average_P( frame, columns ) ;
	  
	  casa::Vector<double> sort_vector = antenna_data( Slice(initin_value, dataBlockSize )) ;
	  
	  double pulse_loc = pulse_LocVector(frame) ;
	  
	  double random_phase = rnd_PhaseVector( frame ) ;
	  
	  double power_level =sqrt( Average_power*peak_power) ;
	  
	  Vector<double> signal_Added = NuMoonTrigger::Sim_Signal( sort_vector,
								   simTEC,
								   FREQvector,
								   power_level,
								   pulse_loc,
								   random_phase,
								   geom_Weight_Vector ) ;
	  
	  for( uint sample_number=0; sample_number< dataBlockSize; sample_number++ ){
	    
	    uint s_number = initin_value*dataBlockSize +sample_number ;
	    
	    rejoined_Vector( s_number ) = signal_Added( sample_number) ;
	    
	  }
	  
	  initin_value = initin_value +1 ;
	  
	}	
	
	AddedSignal_data.column( columns ) = rejoined_Vector ;
	
      }
      return AddedSignal_data ;
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::Added_SignalData" << x.getMesg () << endl ;
      return Matrix<double> ();
    }
    
  }
  
  //_____________________________________________________________________________
  //                                                               without_Signal
  
  Vector<double> NuMoonTrigger::without_Signal(  std::string const &filename,
    						 uint const& n_samples,
					         double const& simTEC,
						 double const& sampling_rate,
						 uint const& nyquist_zone,
						 uint const& time_int_bins,
  					         double const& TEC,
						 const Vector<double>& ppf_coeff,
		   				 const Vector<double>& ppf_invcoeff,
		  				 const Vector<double> freq_range,
						 double const& peak_power,
						 const Vector<uint>& RCU_id ) 
    {
    	
	
     try{
    
     	Matrix<double> Cleaned_data = NuMoonTrigger::Cleaned_data(  filename,
    						    		    n_samples,
					             		    simTEC,
						     		    nyquist_zone,
						     		    peak_power,
						     		    RCU_id,
						     		    sampling_rate,
						     		    TEC,
						     		    freq_range ) ;
     	
	
	   uint n_rcu = RCU_id.nelements() ;
	   
	   uint n_frames = n_samples/1024 ;
	   
	   casa::Vector<double> FREQ_vector = NuMoonTrigger::freq_vector( sampling_rate,
		   	               					  nyquist_zone ) ;
	   
	   casa::Matrix<DComplex> Geometric_weights = NuMoonTrigger::Geom_weights( RCU_id,
					       					   FREQ_vector ) ;
										
           Matrix<DComplex> Beam_formed( 513, n_frames, 0.0 ) ;
       
	   for( uint antenna=0; antenna< n_rcu; antenna++ ) {
	   
	   	casa::Vector<double> samples = Cleaned_data.column(antenna ) ;
		
		casa::Matrix<DComplex> FFT_data = NuMoonTrigger::fft_data( samples,
    		 	     						   n_frames,
			     						   nyquist_zone ) ;
				
		casa::Matrix<DComplex> phase_corrected = NuMoonTrigger::weights_applied( FFT_data,
  					             					 Geometric_weights,
						      					 antenna ) ;
		
		DComplex beamed_value(0.0) ;
												
		for( uint frame=0; frame< n_frames; frame++ ){
		
			for( uint freQchannel =0 ; freQchannel< 513; freQchannel++ ){
			
				beamed_value = phase_corrected( freQchannel, frame);
			
				Beam_formed( freQchannel,frame) = Beam_formed( freQchannel,frame)+ beamed_value;
			 	}
			}
			
		}
		
	    Beam_formed = Beam_formed*(1./n_rcu) ;
	    
	    casa::Matrix<DComplex> RFI_rejected = NuMoonTrigger::RFI_removal( Beam_formed ) ; 
	   
	    casa::Matrix<DComplex> de_dispersedData = NuMoonTrigger::de_dispersion( RFI_rejected,
		      		  						    TEC,
				  						    FREQ_vector ) ;
	    
	    casa::Vector<double> IFFT_data = NuMoonTrigger::ifft_data( de_dispersedData,
    			    						n_frames,
									nyquist_zone ) ;
																		    uint n_samples =  IFFT_data.nelements() ;
	    
	    casa::Vector<double> time_integrated( n_samples-time_int_bins, 0.0 ) ;
	    
	    for( uint nsample=0; nsample< n_samples-time_int_bins; nsample++ ){
	    
	    	for( uint t_bins=nsample; t_bins< (nsample+time_int_bins); t_bins++ ){
		
			time_integrated(nsample) += IFFT_data(t_bins)* IFFT_data(t_bins);
			
		}
	    }
	    
	    return time_integrated ;
     } 
     catch ( AipsError x ){
       cerr << "  NuMoonTrigger::without_Signal" << x.getMesg () << endl ;
       return Vector<double> ();
     }
     
    }   
  
  //______________________________________________________________________________________
  
  Vector<double> NuMoonTrigger::PPF_processed( std::string const &filename,
					       uint const& n_samples,
					       double const& simTEC,
					       double const& sampling_rate,
					       uint const& nyquist_zone,
					       uint const& time_int_bins,
					       double const& TEC,
					       const Vector<double>& ppf_coeff,
					       const Vector<double>& ppf_invcoeff,
					       const Vector<double> freq_range,
					       double const& peak_power,
					       const Vector<uint>& RCU_id ) 
    
  {
    casa::Vector<double> time_integrated;    
    casa::Matrix<double> Added_Signa_data;
    casa::Vector<uint> selected_channelID;
    unsigned int blocksize (1024);

    try{
      Added_Signa_data = NuMoonTrigger::Added_SignalData( filename,
							  n_samples,
							  simTEC, 
							  nyquist_zone,
							  peak_power, 
							  RCU_id, 
							  sampling_rate,
							  TEC,
							  freq_range )  ;
      uint n_rcu = RCU_id.nelements() ;
      
      selected_channelID = NuMoonTrigger::PPFBand_vector( sampling_rate,
							  nyquist_zone,
							  freq_range ) ;
      
      uint number_bands = selected_channelID.nelements() ;
      
      casa::Vector<double>selected_channels( blocksize,0.0 ) ;
      
      double bandwidth = sampling_rate/blocksize ;
      
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
      
      
      uint n_frames = n_samples/1024 ;
      
      casa::Matrix<DComplex> Geometric_weights = NuMoonTrigger::Geom_weights( RCU_id,
									      selected_channels ) ;
      
      Matrix<DComplex> Beam_formed( 513, n_frames, 0.0 ) ;
      
      for( uint antenna=0; antenna< n_rcu; antenna++ ) {
	
	casa::Vector<double> samples = Added_Signa_data.column( antenna ) ;
	
	casa::Matrix<DComplex> ppf_data_antenna = NuMoonTrigger::ppf_implement( samples,
										nyquist_zone,
										ppf_coeff ) ;
	
	casa::Matrix<DComplex> phase_corrected = NuMoonTrigger::weights_applied( ppf_data_antenna,
										 Geometric_weights,
										 antenna ) ;
	
	
	DComplex beamed_value(0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++ ){
	  
	  for( uint freQchannel =0 ; freQchannel< 513; freQchannel++ ){
	    
	    beamed_value = phase_corrected( freQchannel, frame);
	    
	    Beam_formed( freQchannel,frame) = Beam_formed( freQchannel,frame)+ beamed_value;
	  }
	}
	
      }
      
      Beam_formed = Beam_formed*(1./n_rcu) ;
      
      casa::Matrix<DComplex> RFI_rejected = NuMoonTrigger::RFI_removal( Beam_formed ) ; 
      
      casa::Matrix<DComplex> de_dispersedData = NuMoonTrigger::de_dispersion( RFI_rejected,
									      TEC,
									      selected_channels ) ;
      
      
      casa::Matrix<DComplex> selected_PPFdata = NuMoonTrigger::ppfdata_cutshort( de_dispersedData,
										 selected_channelID ) ;
      
      
      
      casa::Vector<double> ppf_invertedData = NuMoonTrigger::ppf_inversion( selected_PPFdata,
									    ppf_invcoeff,
									    selected_channelID )  ;
      
      
      uint n_samples =  ppf_invertedData.nelements() ;
      
      time_integrated.resize( n_samples-time_int_bins);
      
      for( uint nsample=0; nsample< n_samples-time_int_bins; nsample++ ){
	
	for( uint t_bins=nsample; t_bins< (nsample+time_int_bins); t_bins++ ){
	  
	  time_integrated(nsample) += ppf_invertedData(t_bins)*ppf_invertedData(t_bins) ;
	  
	}
      }
      
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::PPF_processed" << x.getMesg () << endl ;
      time_integrated = Vector<double> ();
    }
    
    return time_integrated;              
  }  
  
  //_______________________________________________________________________________________
  
  Vector<double> NuMoonTrigger::FFT_processed( std::string const &filename,
					       uint const& n_samples,
					       double const& simTEC,
					       double const& sampling_rate,
					       uint const& nyquist_zone,
					       uint const& time_int_bins,
					       double const& TEC,
					       const Vector<double>& ppf_coeff,
					       const Vector<double>& ppf_invcoeff,
					       const Vector<double> freq_range,
					       double const& peak_power,
					       const Vector<uint>& RCU_id ) 
  {
    casa::Matrix<double> Added_Signa_data;
    
    try{
      
      Added_Signa_data = NuMoonTrigger::Added_SignalData( filename,
							  n_samples,
							  simTEC, 
							  nyquist_zone,
							  peak_power,
							  RCU_id, 
							  sampling_rate,
							  TEC,
							  freq_range )  ;
      
      uint n_rcu = RCU_id.nelements() ;
      
      uint n_frames = n_samples/1024 ;
      
      casa::Vector<double> FREQ_vector = NuMoonTrigger::freq_vector( sampling_rate,
								     nyquist_zone ) ;
      
      casa::Matrix<DComplex> Geometric_weights = NuMoonTrigger::Geom_weights( RCU_id,
									      FREQ_vector ) ;
      
      Matrix<DComplex> Beam_formed( 513, n_frames, 0.0 ) ;
      
      for( uint antenna=0; antenna< n_rcu; antenna++ ) {
	
	casa::Vector<double> samples = Added_Signa_data.column(antenna ) ;
	
	casa::Matrix<DComplex> FFT_data = NuMoonTrigger::fft_data( samples,
								   n_frames,
								   nyquist_zone ) ;
	
	casa::Matrix<DComplex> phase_corrected = NuMoonTrigger::weights_applied( FFT_data,
										 Geometric_weights,
										 antenna ) ;
	
	DComplex beamed_value(0.0) ;
	
	for( uint frame=0; frame< n_frames; frame++ ){
	  
	  for( uint freQchannel =0 ; freQchannel< 513; freQchannel++ ){
	    
	    beamed_value = phase_corrected( freQchannel, frame);
	    
	    Beam_formed( freQchannel,frame) = Beam_formed( freQchannel,frame)+ beamed_value;
	  }
	}
	
      }
      
      Beam_formed = Beam_formed*(1./n_rcu) ;
      
      casa::Matrix<DComplex> RFI_rejected = NuMoonTrigger::RFI_removal( Beam_formed ) ; 
      
      casa::Matrix<DComplex> de_dispersedData = NuMoonTrigger::de_dispersion( RFI_rejected,
									      TEC,
									      FREQ_vector ) ;
      
      casa::Vector<double> IFFT_data = NuMoonTrigger::ifft_data( de_dispersedData,
								 n_frames,
								 nyquist_zone ) ;
      uint n_samples =  IFFT_data.nelements() ;
      
      casa::Vector<double> time_integrated( n_samples-time_int_bins, 0.0 ) ;
      
      for( uint nsample=0; nsample< n_samples-time_int_bins; nsample++ ){
	
	for( uint t_bins=nsample; t_bins< (nsample+time_int_bins); t_bins++ ){
	  
	  time_integrated(nsample) += IFFT_data(t_bins)* IFFT_data(t_bins);
	  
	}
      }
      
      return time_integrated ;
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger::FFT_processed" << x.getMesg () << endl ;
      return Vector<double> ();
    }
    
  }   
  
  //_____________________________________________________________________________
  //                                                                 root_ntuples
  
#ifdef HAVE_ROOT
    
  void NuMoonTrigger::root_ntuples( std::string const &filename,
				    uint const& n_samples,
				    double const& simTEC,
				    double const& sampling_rate,
				    uint const& nyquist_zone,
				    uint const& time_int_bins,
				    double const& TEC,
				    const Vector<double>& ppf_coeff,
				    const Vector<double>& ppf_invcoeff,
				    const Vector<double> freq_range,
				    double const& peak_power,
				    const Vector<uint>& RCU_id )
    
  {
    
    cout << "To store data informaton in ntuples" << endl ;
    
    try {
      
//       std::string outFileNameR = "pulse_data.root" ;
//       TFile *myOutput = new TFile( outFileNameR.c_str(), "RECREATE");
      
      //  TNtuple *eventuple_FFT = new TNtuple("eventuple_FFT","eventuple_FFT","time:frame:Average_P5:sample_value") ;
      
      //  TNtuple *eventuple_raw = new TNtuple("eventuple_raw","eventuple_raw","time:frame:Average_P5:sample_value") ;
      
      TNtuple *eventuple_PPF = new TNtuple("eventuple_PPF",
					   "eventuple_PPF",
					   "time:frame:Average_P5:sample_value");
      
      Matrix<double> Cleaned_data = NuMoonTrigger::Cleaned_data(  filename,
    						    		  n_samples,
					             		  simTEC,
						     		  nyquist_zone,
						     		  peak_power,
						     		  RCU_id,
						     		  sampling_rate,
						     		  TEC,
						     		  freq_range ) ;
      
      uint n_frames = n_samples/1024 ;
      
      casa::Matrix<double> Average_P = NuMoonTrigger::cal_AvPower( Cleaned_data,
								   n_frames )  ;
      
      
      
      uint Av_columns = Average_P.ncolumn() ;
      
      uint Av_rows = Average_P.nrow() ;
      
      casa::Vector<double> Average_P5( Av_rows, 0.0 ) ;
      
      for( uint row=0; row< Av_rows; row++ ){
	
    	for( uint col=0; col< Av_columns; col++ ){
	  
	  Average_P5( row) = Average_P5( row)+ Average_P( row, col) ;
	  
	}
	Average_P5( row) = Average_P5( row)*(1./Av_columns) ;
      }
      
      /*casa::Vector<double> time_integrated_WOSignal = NuMoonTrigger::without_Signal(  filename,
	n_samples,
	simTEC,
	sampling_rate,
	nyquist_zone,
	time_int_bins,
	TEC,
	ppf_coeff,
	ppf_invcoeff,
	freq_range,
	peak_power,
	RCU_id )     ;
	
	casa::Vector<double> time_integrated_FFT =NuMoonTrigger::FFT_processed( filename,
	n_samples,
	simTEC,
	sampling_rate,
	nyquist_zone,
	time_int_bins,
	TEC,
	ppf_coeff,
	ppf_invcoeff,
	freq_range,
	peak_power,
	RCU_id ) ;
      */									    
      casa::Vector<double> time_integrated_PPF = NuMoonTrigger::PPF_processed( filename,
									       n_samples,
									       simTEC,
									       sampling_rate,
									       nyquist_zone,
									       time_int_bins,
									       TEC,
									       ppf_coeff,
									       ppf_invcoeff,
									       freq_range,
									       peak_power,
									       RCU_id ) ;
      
      
      uint n_samples = time_integrated_PPF.nelements() ;
      
      for( uint timebin=0; timebin< n_samples; timebin++ ){
	
    	int frame_n = 1+ int(timebin/1024) ;
	
	//    	double sample_value_FFT = time_integrated_FFT(timebin) ;
	
	double sample_value_PPF = time_integrated_PPF(timebin) ;
	
	//	double sample_value_raw = time_integrated_WOSignal(timebin) ;
	
	double Average_value = Average_P5(frame_n ) ;
	//	cout << "Average_value :" << Average_value << "sample value PPF :" << sample_value_PPF << endl ;
	//	if( sample_value_FFT > 3.0*Average_value){
	
	//   eventuple_FFT->Fill( timebin, frame_n, Average_value, sample_value_FFT ) ;
	//  cout << "eventuple has been filled " <<endl ;
	//     }
	if( sample_value_PPF > 1.0*Average_value){
	  
	  eventuple_PPF->Fill( timebin, frame_n, Average_value, sample_value_PPF ) ;
	  
	}
	
	//	if( sample_value_raw > 3.0*Average_value){
	
	//eventuple_raw->Fill( timebin, frame_n, Average_value, sample_value_raw ) ;
	// }     
      }
      
      //	eventuple_FFT->Write() ;
      eventuple_PPF->Write() ;
      //	eventuple_raw->Write() ;
    } 
    catch ( AipsError x ){
      cerr << "  NuMoonTrigger:: root_ntuple" << x.getMesg () << endl ;
    }
    // return Matrix<DComplex> Geom_weights ;
  }
  
#endif 
  
} // Namespace  -- end
