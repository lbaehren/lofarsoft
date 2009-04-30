/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                  *
 *   Kalpana Singh (<mail>)                                                     *
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
    		   		uint const& n_frames,
				double const& simTEC,
		   		double const& sampling_rate,
		   		uint const& nyquist_zone,
		   		double const& TEC,
		   		const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		   		const Vector<uint> freq_range,
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
  
    Matrix<double> NuMoonTrigger::reading_data( std::string const &filename,
    			       			uint const& n_frames ) 
   {
   	Matrix<double> data ;
     try {
     	  DAL::TBB_Timeseries timeseries() ;
	  
	 // uint nofSamples = n_frames*1024 ;
	  
	 // uint start =0 ;
	  
	//  data = timeseries.fx( start,
	 // 		       nofSamples ) ;
	  
         }
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::reading_data " << x.getMesg () << endl ;
        }
      return data ;
   }  
       					
//____________________________________________________________________________________________
		     
    casa::Matrix<DComplex> NuMoonTrigger::fft_data( Vector<double>&  data,
    		 	     		            uint const& n_frames,
					            uint const& nyquist_zone ) 
    {
    	  uint dataBlockSize = 1024 ;
	  
	  uint n_freqbins = dataBlockSize/2+1 ;
     
          Matrix<DComplex> fft_implemented( n_freqbins, n_frames, 0.0 ) ;
        	  
     try {
     
     	  FFTServer <double,DComplex> server ;
	  
	//  casa::Vector<double> sliced_vector(dataBlockSize, 0.0) ;
    
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
        }
      return  fft_implemented;
   }
        
//_____________________________________________________________________________________________	      

   Matrix<DComplex> NuMoonTrigger::RFI_removal( Matrix<DComplex> fft_samples ) 
   {
     casa::Matrix<DComplex> RFIMitigated_array ;
     
     try {
     
     	  uint dataBlockSize = 1024 ;
	  
	  DComplex expo(0, 1);
	   
	  uint n_freqbins = dataBlockSize/2+1 ;
	  
	  uint n_segments = n_freqbins/32 ;
	  
	  casa::Matrix<double> phase_array( phase(fft_samples));
	  
	  uint n_columns = phase_array.ncolumn() ;
	  //casa::Matrix<double> phase_array = phase( fft_samples ) ;
	  
	  casa::Matrix<DComplex> array_phases( n_freqbins, n_columns, 0.0 ) ;
	  
	  for(uint i=0; i<n_freqbins ; i++ ){
	  
	  	for(uint j=0; j<n_columns; j++ ){
		
			double phase_value = phase_array(i,j) ;
			
			array_phases(i,j) = exp(expo*phase_value) ;
			
			}
			
		}
			  
	  CR::RFIMitigation  rfi( fft_samples,
	  		          dataBlockSize,
			          n_segments ) ;
	  
	  casa::Matrix<Double> RFIrejected_absolutearray = rfi.getOptimizedSpectra( fft_samples,
					                                           dataBlockSize,
							               		   n_segments  ) ;	
	 
	  casa::Matrix<DComplex> RFIreject_array  ;
	  
	  convertArray( RFIreject_array, RFIrejected_absolutearray ) ;
	       
	  RFIMitigated_array = RFIreject_array*array_phases  ;
	     
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::RFI_removal " << x.getMesg () << endl ;
        }
      return RFIMitigated_array ;
   } 						

   
//______________________________________________________________________________________

 Vector<double> NuMoonTrigger::freq_vector( double const& sampling_rate,
		   	      		    uint const& nyquist_zone ) 
 {
    casa::Vector<double> frequency_Vector ;
	
   try {
   
   	  double channel_width = sampling_rate/1024. ;			 
	   
	  // Vector<double> frequency_vector(512,0.0) ;
	   
	   Vector<uint> band_ID( 512, 0 ) ;
	   
	   Vector<uint> selected_channelID ;
	   
	   Vector<double> selected_channels ;
	  
	   switch( nyquist_zone ){
	  
	  		case 1:  
				for( uint i=0; i<512 ; i++ ){
				
					double central_freq = i*channel_width + channel_width/2 ;
					
					frequency_Vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
									
					}
				
				break ;
				
			case 2:
				for( uint i=0; i<512 ; i++ ){
				
					double central_freq = sampling_rate/2+i*channel_width + channel_width/2 ;
					
					frequency_Vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
					
					}
				
				
    				break ;
			}
	   
 	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::freq_vector" << x.getMesg () << endl ;
        }
      return frequency_Vector ;
   }
   
    //_______________________________________________________________________________
		     
   Matrix<DComplex> NuMoonTrigger::de_dispersion( Matrix<DComplex> fft_samples,
		      		  		  double const& TEC,
						  const Vector<double> freq_Vector ) 
   {
   	Matrix<DComplex> dispersed_modified_array ;
	
     try {
     
     	  double pi = 3.14159265358979323846 ;
	  
	  DComplex expo(0, 1);
	  
	  DComplex expr = exp(expo*0.0) ;
     	  
	  uint dataBlockSize = 1024 ;
	  
	  uint n_freqbins = dataBlockSize/2+1 ;
	  
	  double DM = 1.34e+9*TEC*1.0 ;
	  
	  double t0 = 8.e+5/3.e+8 ;
	  
	  double modified_phase(0.0) ;
	  
	  casa::Matrix<double> phase_array( phase(fft_samples));
	  
	  uint n_columns = phase_array.ncolumn() ;
	  //Matrix<double> phase_array = phase(fft_samples) ;
	  
	  Matrix<double> amp_array = amplitude(fft_samples) ;
	  
	  Matrix<DComplex> amp_amplified_array ;
	  
	  convertArray( amp_amplified_array, amp_array ) ;
	  
	  Matrix<DComplex> phase_modified_array ;
	  
	  for( uint i=0; i< n_columns; i++ ){
	  
	  	  for( uint j=0; j< n_freqbins; j++ ){
		  
		  	double inner_term = 1-2*DM/t0/((freq_Vector(j))*(freq_Vector(j))) ;
			
			double outer_term = 2*pi*sqrt(inner_term)*(freq_Vector(j))*t0 -2*pi*t0*freq_Vector(j) ;
		  
		    	modified_phase = fmod(phase_array(i,j) +pi +outer_term,2*pi) ;
			
			phase_modified_array(i,j) = exp(expo*modified_phase) ;
			
			}
			
		}
	
	   dispersed_modified_array =  amp_amplified_array*phase_modified_array  ;
			  
	  		
         }
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::de_dispersion " << x.getMesg () << endl ;
        }
      return dispersed_modified_array ;
   } 
   
     
//_____________________________________________________________________________   

   Vector<double> NuMoonTrigger::ifft_data( Matrix<DComplex> const& data,
    			    		   uint const& n_frames ) 
   {
   	Vector<Double> timeSeries ;
     try {
     	   uint dataBlockSize = 1024 ;
	  
	   uint n_freqbins = dataBlockSize/2+1 ;
     	  
	   Vector<DComplex> FFTVector( n_freqbins,0.0) ;
	   
	   FFTServer <Double,DComplex> server ;
     	    
	   Vector<Double> IFFTVector( dataBlockSize, 0.0 );
     
     	    //Vector<Double> timeSeries( dataBlockSize*n_frames, 0.0 ) ;
     
     	    for( uint i=0; i< n_frames; i++ ) {
//     
           	 FFTVector = data.column(i) ;
// 	  
 	  	 server.fft(IFFTVector, FFTVector ) ;
// 	  
 	  	 for( uint j=0; j< dataBlockSize; j++ ) {
// 	  
 	       		timeSeries(i*dataBlockSize+j)= IFFTVector(j) ;
// 	       
 	          }
// 	       
            }
         
	 }
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ifft_data " << x.getMesg () << endl ;
        }
      return timeSeries ;
   }
   
   
//__________________________________________________________________________________

       
  Matrix<double> NuMoonTrigger::cal_AvPower( Matrix<double> const& data_array,
  		              	   	     uint const& n_frames ) 
				     
  {
  	Matrix<double> Average_power ;
	
     try {
     
       	  uint dataBlockSize = 1024 ;
	  
	  uint n_column = data_array.ncolumn() ;
	  
	  Matrix<double> P5( dataBlockSize*n_frames-5, n_column, 0.0 ) ;
	  
	//  Matrix<double> Average_power( n_frames,n_column, 0.0 ) ;
	  
	  for(uint i=0; i<n_column; i++ ){
	  
	  	for( uint j=0; j<n_frames; j++ ){
		
			uint dataBlock = j*dataBlockSize ;
		
			double average_power(0.0) ;
			
			for( uint k=dataBlock; k< (j+1)*dataBlockSize-5 ; k++ ){
			
				double Power_5(0.0) ;
				
				for( uint l=k; l<(k+5); l++ ){
				
					Power_5 += (data_array(l,i))*(data_array(l,i)) ;
					
					}
					
					P5(k,i) = Power_5 ;
					
					average_power += Power_5 ;
				}
			
			Average_power(j,i) = average_power/1024. ;
	  
		}	  
         }
	
      }
     
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::cal_AvPower " << x.getMesg () << endl ;
        }
	return Average_power ;
  }   
     
     
//_____________________________________________________________________________________

   Matrix<DComplex> NuMoonTrigger::ppf_implement( Vector<double>& samples,
   						  uint const& nyquist_zone,
                                  		  const Vector<double>& ppf_coeff ) 
   {
     Matrix<DComplex> ppf_implemented ;
     try { 
     	   uint dataBlockSize = 1024 ;
     		
           uint n_freqbins = dataBlockSize/2+1 ;
	  
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
           
     	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppf_implement " << x.getMesg () << endl ;
        }
      return ppf_implemented ;
   } 
    
   
 //________________________________________________________________________________________
 
 Matrix<DComplex> NuMoonTrigger::ppfdata_cutshort( Matrix<DComplex>& samples,
   						   Vector<uint> subband_ID ) 
    {
       Matrix<DComplex> sortened_array ;
       
     try {
	 
     	  uint nOfelements = subband_ID.nelements() ;
	  
	  //uint n_columns = samples.ncolumn() ;
	  
      	 // Matrix<DComplex> sortened_array( 1024, n_columns, 0.0 ) ;
	  
	  for( uint i=0; i< nOfelements; i++ ){
	  
	  	uint band_ID = subband_ID(i) ;
	  
	  	Vector<DComplex> sample_vector = samples.row( band_ID-1 ) ;
		
		sortened_array.row( band_ID-1 ) = sample_vector ;
		
		}
	
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::ppfdata_cutshort" << x.getMesg () << endl ;
        }
      return sortened_array;
   } 
    
 
 
 //________________________________________________________________________________________
 
   void NuMoonTrigger::PPFBand_vector( double const& sampling_rate,
   				       uint const& nyquist_zone,
				       const Vector<uint> freq_range ) 
   {
     
     try {
   	   double channel_width = sampling_rate/1024. ;			 
	   
	   Vector<double> frequency_vector(512,0.0) ;
	   
	   Vector<uint> band_ID( 512, 0 ) ;
	   
	   Vector<uint> selected_channelID ;
	   
	   Vector<double> selected_channels ;
	  
	   switch( nyquist_zone ){
	  
	  		case 1:  
				for( uint i=0; i<512 ; i++ ){
				
					double central_freq = i*channel_width + channel_width/2 ;
					
					frequency_vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
									
					}
				
				break ;
				
			case 2:
				for( uint i=0; i<512 ; i++ ){
				
					double central_freq = sampling_rate/2+i*channel_width + channel_width/2 ;
					
					frequency_vector(i) = central_freq ;
					
					band_ID(i) = i+1 ;
					
					}
				
				
    				break ;
			}
		double init = freq_range(0) ;
		
		double final = freq_range(1) ;
		
		uint k=0; 
		
		for( uint j=0; j<512; j++ ){
		
			double compare = frequency_vector(j)   ;
			
			if( compare > init && compare < final ) {
			
				selected_channelID(k) = band_ID(j) ;
				
				selected_channels(k) = compare ;
				
				k = k+1 ;
				}
		        }
	  	
	}
	
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::PPFBandID_vector" << x.getMesg () << endl ;
        }
      return ;
   } 
   
//____________________________________________________________________________________________
 
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
        }
      return ppfinverted_Vector ;
   } 
   
//______________________________________________________________________________________
   
  Matrix<DComplex> NuMoonTrigger::Geom_weights( const Vector<uint>& RCU_id,
					       const Vector<double>& freq_Vector ) 
    {

     Matrix<DComplex> weight_factors ;
      
     try {
      
     	  double pi = 3.14159265358979323846 ;
	  
	  DComplex expo(0, 1);
	  
	  //uint dataBlockSize = 1024 ;
	  
	  uint freq_channels = freq_Vector.nelements() ;
	  
	  uint nOfelements = RCU_id.nelements() ;
	   
     	  double long_deg = 6.0 ;
	  double long_min = 52.0 ;
	  
	  double radius = 6371.0 ;
	  
	  Vector<double> long_sec( nOfelements, 0.0 ) ;
	 
	  long_sec(0) = 12.61461 ;
	  long_sec(1) = 12.62020 ;
	  long_sec(2) = 12.34627 ;
	  long_sec(3) = 12.34070 ;
	  
	  double lat_deg = 52.0 ;
	  double lat_min = 54.0 ;
	 
	  Vector<double> lat_sec( nOfelements, 0.0 ) ;
	 
	  lat_sec(0) = 47.90154 ;
	  lat_sec(1) = 48.06707 ;
	  lat_sec(2) = 48.07043 ;
	  lat_sec(3) = 47.90490 ;
	  
	  Vector<double> latitude( nOfelements, 0.0) ;
	  Vector<double> longitude( nOfelements, 0.0 ) ;
	  
	  for( uint i=0; i< nOfelements; i++ ){
	  
	  	latitude(i) = (pi/180)*(lat_deg +(1/60)*lat_min +(1/60)*lat_sec(i)) ;
		longitude(i) = (pi/180)*(long_deg +(1/60)*long_min +(1/60)*long_sec(i)) ;
		
		}
	  Vector<double> lat_diff( nOfelements, 0.0 ) ;
	  Vector<double> long_diff( nOfelements, 0.0 ) ;	
	  
	  Vector<double> baseline( nOfelements, 0.0 ) ;
	  	  
	  Vector<double> Geom_delay( nOfelements, 0.0 ) ;
	  
	  double a, x, y, c ;
	  
	  for( uint j=0; j< nOfelements; j++ ){
	  
	  	lat_diff(j) = latitude(j)- latitude(0) ;
		long_diff(j) = longitude(j)- longitude(0) ;
		
  a=(sin(0.5*lat_diff(j)))*(sin(0.5*lat_diff(j)))+cos(latitude(j+1))*cos(latitude(j))*(sin(0.5*long_diff(j)))*(sin(0.5*long_diff(j))) ;

	 	y = sqrt(a) ;
	 	x = sqrt(1.-a) ;
	 
	 	c =2*atan2(y,x) ;
	 
	 	baseline(j) = radius*c ;
	 
	 	Geom_delay(j) =(1./c)*(sqrt(1+(baseline(j)*baseline(j)-2*baseline(j)))-1.) ;
	 
	 }
	 
	// Matrix<DComplex> weight_factors( freq_channels, nOfelements,0.0  )  ;
	 
	 for( uint g=0; g< nOfelements; g++ ){
	 
	 	for( uint f=0; f< freq_channels; f++ ){
		
			double freq_val = freq_Vector(f) ;
			
			double phase_factor = 2*pi*freq_val*Geom_delay(g) ;
		
			weight_factors(f,g) = exp( expo*phase_factor ) ;
			
			}
		}	
	 
	  
  
       } 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::Geom_weights " << x.getMesg () << endl ;
     }
   return weight_factors ;
  }
  
  //____________________________________________________________________________________

    Matrix<DComplex> NuMoonTrigger::weights_applied( Matrix<DComplex> const& data,
  					             Matrix<DComplex> const& geom_weights,
						     uint const& rcu_id ) 
  
    {
    	casa::Matrix<DComplex> data_phase_corrected ;
	
     try {
     
     	   uint n_columns = data.ncolumn() ;
	   
	   uint n_rows = data.nrow() ;
	   
	   Vector<DComplex> geom_vector = geom_weights.column( rcu_id ) ;
	   
	   Vector<DComplex> data_vector;
	   
	   Vector<DComplex> weight_applied_vector ;
	   	   
	   for( uint i=0; i< n_columns; i++ ) {
	   
	   	data_vector = data.column(i) ;
		
		weight_applied_vector = data_vector*geom_vector ;
		
		DComplex weight_applied(0.0);
		
		for( uint j=0; j< n_rows; j++ ){
		
			weight_applied = weight_applied_vector(j) ;
			
			data_phase_corrected(j,i) = weight_applied ;
		}
	   }
     
     	} 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::weights_applied " << x.getMesg () << endl ;
        }
   return data_phase_corrected ;
  }
  //_________________________________________________________________
  
   Vector<double> NuMoonTrigger::Sim_Signal( Vector<double> const& data,
    		 	     		    double const& simTEC,
					    const Vector<double>& freq_Vector,
					    double const& peak_power,
					    double const& pulse_loc,
					    double const& rnd_phase,
					    Vector<DComplex> geom_Weights_factor  ) 
   {
   	Vector<double>  addedSignal ;
	
     try {
     	
     	  double pi = 3.14159265358979323846 ;
	  
	  uint dataBlockSize = 1024 ;
	  
	  uint n_freqbins = dataBlockSize/2 +1 ;
	  
     	  double DM, t0, peak_height ;
	  
	  peak_height = sqrt(peak_power ) ;
	  
	  DComplex expo(0, 1);
	  
	  DComplex expr = exp(expo*0.0) ;
	  
	  casa::Vector<double> amp( n_freqbins, 0.0 ) ;
	  
	  casa::Vector<double> phase( n_freqbins, 0.0 ) ;
	  
	  casa::Vector<DComplex> phased_vector( n_freqbins, 0.0 ) ;
	  
	  FFTServer <Double,DComplex> server ;
     	 
	  DM = 1.34e+9*simTEC*1.0 ;
	  
	  t0 = 8.e+5/3.e+8 ;
	  
	  DComplex GeomWeights ;
	  
	  double geom_phase ;   
	   
	  for( uint i=0; i< n_freqbins; i++ ) {
	  
	  	GeomWeights = geom_Weights_factor( i ) ;
		
		geom_phase = phase( geom_phase ) ;
//     
		amp(i) = (peak_height/dataBlockSize) ;
		
		phase(i) = rnd_phase + (1-(pulse_loc/1024.))*2*pi*i - geom_phase;
		
		double nu = freq_Vector(i) ;
		
		phase(i) -= 2.*pi*sqrt(1-2.*DM/t0/(nu*nu))*nu*t0 -2.*pi*nu*t0 ;
		
		phased_vector(i) = exp(expo*phase(i)) ;
			
		}
		
	 casa::Vector<DComplex> amp_vector  ;
	 
	 convertArray( amp_vector, amp ) ;
	 
	 casa::Vector<DComplex> dispersed_signalVector = amp_vector*phased_vector  ;
		
	casa::Vector<double> signal_Vector ( dataBlockSize,0.0 ) ;
	
	server.fft( signal_Vector, dispersed_signalVector ) ;
	
	double signal(0.0) ;
	
  	for( uint j=0; j< dataBlockSize; j++ ) {
	  
 	        signal = signal_Vector(j);   
		
		signal = 2.0*int(signal/2.0) ;
		
		if(signal>2047) {signal=2047 ;} 
		if(signal<-2048) {signal=-2048 ;} 
		
		double data_value =data(j) ;
		
		addedSignal(j) = signal +data_value ;
              }
	 	  
         }
	       
       catch ( AipsError x ){
       cerr << " NuMoonTrigger::AddSignal " << x.getMesg () << endl ;
        }
     return addedSignal ;
        
   }
      
  

  
//_____________________________________________________________________________________
  
 
    Matrix<double> NuMoonTrigger::Added_SignalData( std::string const &filename,
    						    uint const& n_frames,
					            double const& simTEC,
						    uint const& nyquist_zone,
  					            Matrix<DComplex> const& geom_weights,
						    double const& peak_power,
						    const Vector<uint>& RCU_id,
						    double const& sampling_rate,
						    double const& TEC,
						    const Vector<uint> freq_range ) 
   {
   
   	casa::Matrix< double > AddedSignal_data ;
     try {
     	  
	  double pi = 3.14159265358979323846 ;
     	  
	  uint dataBlockSize = 1024 ;
	  
	  CR::LOFAR_TBB lofar_tbb( filename, n_frames ) ;
	   
	  casa::Matrix<double> data = lofar_tbb.fx( ) ;	
	 
	  CR::NuMoonTrigger moontrigger() ;
	 
	  uint nc_rcu = data.ncolumn() ;
	  
	  uint nr_rcu = data.nrow() ;
	  
	  Matrix<double> Cleaned_data( nr_rcu, nc_rcu, 0.0 ) ;
	 
	  casa::Vector<double> FREQvector = NuMoonTrigger::freq_vector( sampling_rate,
									nyquist_zone	) ;

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
									     n_frames ) ;
	 
	 	Cleaned_data.column(rcu) = IFFT_Vector ;
											
	       }
	  
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
          	
	  for( uint columns=0; columns< nc_rcu; columns++ ) {	  
	 
	 	casa::Vector<double> antenna_data = Cleaned_data.column( columns ) ;
		
		casa::Vector<DComplex> geom_Weight_Vector = geomWeights.column( columns ) ;
		
		uint init =0 ;
		
		casa::Vector<double> rejoined_Vector ;
		     
		for( uint frame=0; frame< n_frames; frame++ ){
		
			double Average_power = Average_P( frame, columns ) ;
		
			casa::Vector<double> sort_vector = antenna_data( Slice(init, dataBlockSize )) ;
		
			double pulse_loc = pulse_LocVector(frame) ;
			
			double random_phase = rnd_PhaseVector( frame ) ;
			
			double power_level = Average_power* peak_power ;
			
			Vector<double> signal_Added = NuMoonTrigger::Sim_Signal( sort_vector,
										simTEC,
										FREQvector,
										power_level,
										pulse_loc,
										random_phase,
										geom_Weight_Vector ) ;
			
			for( uint sample_number= 0; sample_number< dataBlockSize; sample_number ){
			
				rejoined_Vector( init*dataBlockSize +sample_number ) = signal_Added( sample_number) ;
				
				}
					
			init = init+1024 ;
		
		}	
		
	   AddedSignal_data.column( columns ) = rejoined_Vector ;
	  }
      } 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger::beamForming" << x.getMesg () << endl ;
        }
   return AddedSignal_data ;
  }
  

//______________________________________________________________________________________

   
    
//_______________________________________________________________________________________

  #ifdef HAVE_ROOT
  
    void NuMoonTrigger::root_ntuples( Matrix<double> const& data,
    		     			uint const& n_frames,
		     			uint const& nyquist_zone )
    
  {
    
    cout << "To store data informaton in ntuples" << endl ;
    
    try {
//       CR::NuMoonTrigger numoonTrigger( filename,
// 				       quantity,
// 				       group,
// 				       samplingRate_p,
// 				       nyquistZone_p ) ;
      } 
       catch ( AipsError x ){
       cerr << "  NuMoonTrigger:: root_ntuple" << x.getMesg () << endl ;
     }
  // return Matrix<DComplex> Geom_weights ;
  }
  
  
  #endif
  
     
  
  } // Namespace  -- end