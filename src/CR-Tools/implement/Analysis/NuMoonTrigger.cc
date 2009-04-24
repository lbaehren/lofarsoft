/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Kalpana Singh (<k.singh@rug.nl>)                                      *
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
  
  NuMoonTrigger::NuMoonTrigger () {}

  NuMoonTrigger::NuMoonTrigger ( std::string const & filename_t,
		                 SkymapQuantity const &quantity,
		                 std::string const & group,
  				 double samplingRate_p, 
		                 uint nyquistZone_p  )  
	
  {
   // observingTime_p = 0;
   // samplingRate_p  = 0;
    mean_p          = 0;
    variance_p      = 0;
  }
  
  NuMoonTrigger::NuMoonTrigger (NuMoonTrigger const &other)
  {
    copy (other);
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
      copy (other);
    }
    return *this;
  }
  
  void NuMoonTrigger::copy (NuMoonTrigger const &other)
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
  

  Matrix<Double> NuMoonTrigger::timeSeries_data( std::string const & filename_t,
						 std::string const & group,
						 uint start_p,
						 uint nofSamples_p  )
  {

   cout << "To read time series data from buffer boards" << endl ;
   try {
     
        DAL::TBB_StationGroup stationgroup( filename_t, 
					     group ) ;

        cout << "Retrieve data without channel selection" << endl;
   
       //casa::Vector<casa::MDirection> beamDirection = timeseries.beam_direction() ;

	casa::Vector<double> sampling_frequency = stationgroup.sample_frequency_value() ;

	//casa::Vector<uint> nyq_zone = dipoledataset.nyquist_zone() ;

       // uint length_data = dipoledataset.data_length() ;

	casa::Matrix<double> timeSeriesData = stationgroup.fx( start_p,
								nofSamples_p ) ;
	
	return timeSeriesData ;
	      
        }
        catch( AipsError x ){
	cerr << " NuMoonTrigger::timeSeries_data" << x.getMesg () << endl ;
	return Matrix<Double> () ;
	}
   }

  //_____________________________________________________________________________
  //                                                             antenna_position

   Matrix<Double> NuMoonTrigger::antenna_position( std::string const & filename_t,
						   std::string const & group )
  {

   cout << "To read the antenna positions for all buffer boards" << endl ;
   try {
     
        DAL::TBB_StationGroup stationgroup( filename_t,
					     group ) ;

        casa::Matrix<double> antenna_Positions = stationgroup.antenna_position_value() ;

	//casa::Vector<casa::MPosition> antenna_MPositions = stationgroup.antenna_position() ;
      
	return antenna_Positions ;
        
	}

        catch( AipsError x ){
	cerr << " NuMoonTrigger::antenna_position" << x.getMesg () << endl ;
	return Matrix<Double> () ;
	}
   }

  //_____________________________________________________________________________

  string NuMoonTrigger::beamAngle_ra( std::string const & filename )
  {
    
    cout << "To read the RA of the beam of filtered data" << endl ;
    string beamAngle_ra;
    try {
      
      DAL::BeamFormed bf( filename ) ;
      
      beamAngle_ra = bf.point_ra();	
      
    }
    
    catch( AipsError x ){
      cerr << " NuMoonTrigger::beamAngle_ra" << x.getMesg () << endl ;
      return string();
    }
    
    return beamAngle_ra;
  }
  
  //_____________________________________________________________________________

 string NuMoonTrigger::beamAngle_dec( std::string const & filename )

 {

   cout << "To read the RA of the beam of filtered data" << endl ;
   
   string beamAngle_dec;
   
   try {
     
     DAL::BeamFormed bf( filename ) ;
     
     beamAngle_dec = bf.point_dec();	
     
   }
   
   catch( AipsError x ){
     cerr << " NuMoonTrigger::beamAngle_dec" << x.getMesg () << endl ;
     return string() ;
   }
   
   return beamAngle_dec;
 }
  
  
  //_____________________________________________________________________________


  casa::Vector<uint> NuMoonTrigger::obs_time( std::string const & filename )

 {

   cout << "To read the time of observation" << endl ;
   Vector<uint> obsTime;
   
   try {
     DAL::TBB_Timeseries TBB_time( filename ) ;
     obsTime = TBB_time.time();	
   }
   
   catch( AipsError x ){
     cerr << " NuMoonTrigger::obs_time" << x.getMesg () << endl ;
     obsTime = Vector<uint> ();
   }
   
   return obsTime;
 }
  
  //_____________________________________________________________________________

  Vector<Double> NuMoonTrigger::Direction_Conversion( double const &ra,
						      double const &dec,
						      double const &obs_epoch )
 {
   Vector<Double> azel;

   try {
        MDirection dir;
        dir = MDirection( Quantity(ra,"deg"),
			  Quantity(dec,"deg"),
			  MDirection::Ref(MDirection::J2000)) ;
        
 	MDirection j2000 (dir) ;
	cout << j2000 << endl ;
	
	cout << casa::MDirection::Convert ( j2000,
					    MDirection::Ref( MDirection::B1950 ))() <<endl ;
	
	cout << casa::MDirection::Convert ( j2000,
					    MDirection::Ref( MDirection::GALACTIC ))() <<endl ;
	// set up a model for the input (default reference is UTC)
	
	casa::MEpoch epoch( casa::Quantity( obs_epoch,"d")) ;
	
	double radius =1 ;
	casa::MVPosition mvp ( Quantity( radius,"m"),
			       Quantity( ra,"deg"),
			       Quantity( dec,"deg"));
	MPosition obs( mvp,
		       MPosition::Ref(MPosition::WGS84));
	//create reference frame
	casa::MeasFrame frame( epoch,obs ) ;
	//create conversion engine
	MDirection mdFROM (MDirection::J2000);
	MDirection::Convert conv (mdFROM,
				  MDirection::Ref(MDirection::AZEL,frame));

	//convert direction from J2000 to AZEL
	MVDirection mvd = MVDirection (Quantity(ra,"deg"),Quantity(dec,"deg"));
	cout <<"AZEL = " <<conv(MVDirection (mvd)) << endl ;
	
   }
   catch( AipsError x ){
     cerr << " NuMoonTrigger::Direction_Conversion" << x.getMesg () << endl ;
     azel = Vector<double>();
   }
   
   return azel;
 }
  
  //_____________________________________________________________________________
  //                                                                 Beam_forming

  casa::Vector<Double> NuMoonTrigger::Beam_forming( std::string const &filename,
						    SkymapQuantity const &quantity,
						    std::string const &group,
						    double samplingRate_p, 
						    uint nyquistZone_p )
    
  {

   cout << "To do beamforming of TBB data" << endl ;
   casa::Matrix<double> beam ;

   try {

	uint nofSamples_p = 1024 ;

	DAL::TBB_Timeseries tbb_data ( filename) ;

	CR::TimeFreq time_freq( nofSamples_p,
				samplingRate_p,
				nyquistZone_p ) ;	

	DAL::TBB_StationGroup stationgroup( filename, 
					     group ) ;

	CR::NuMoonTrigger numoonTrigger( filename,
					 quantity,
					 group,
					samplingRate_p,
					nyquistZone_p ) ;
        cout << "Retrieve data without channel selection" << endl;
   
       //casa::Vector<casa::MDirection> beamDirection = timeseries.beam_direction() ;

	casa::Vector<double> sampling_frequency = stationgroup.sample_frequency_value() ;

	casa::Vector< uint > ny_Zone = stationgroup.nyquist_zone() ;   	

        casa::Vector< uint > dataLength = stationgroup.data_length() ; 

	uint n_dipoles = stationgroup.nofDipoleDatasets() ;	
	
	//DAL::DataReader dr() ;
     
	Matrix<double> antennaPositions = numoonTrigger.antenna_position( filename,
					       	                            group ) ;
	
	string ra_s = numoonTrigger.beamAngle_ra( filename ) ;

	double ra ;

	ra = atof(ra_s.c_str()) ;

	string dec_s = numoonTrigger.beamAngle_dec( filename ) ;
	
	double dec ;

	dec =atof(dec_s.c_str()) ;

        casa::Vector<uint> epoch = numoonTrigger.obs_time( filename ) ;

	casa::Vector<double> sky_positions = numoonTrigger.Direction_Conversion( ra,
						      				  dec,
									          epoch(0) ) ;
			
	bool anglesInDegrees(false) ;
	bool farfield(true) ;
	bool bufferDelays(false) ;
	bool bufferphases(false) ;
	bool bufferweights(false) ;
	
	Matrix<double> sky_Position ;

	sky_Position.row(0) = sky_positions ;

	Matrix<double> Phase_corrected( dataLength(0),n_dipoles,0.0) ;
	Matrix<DComplex> out ( time_freq.fftLength(),
			       n_dipoles );
	
	int n_frames = (1/nofSamples_p)*dataLength(0) ;
	
	int start = 0 ;

	for( int frame=0; frame < n_frames; frame++ ){

		Matrix<double>tbb_frame = tbb_data.fx( start,
							nofSamples_p ) ;		

		for( uint n_Antenna =0; n_Antenna < n_dipoles; n_Antenna++ ) {

			Vector<double> inColumn = tbb_frame.column( n_Antenna ) ;

			time_freq.setSampleFrequency( sampling_frequency( n_Antenna ) ) ;
	
			time_freq.setNyquistZone( ny_Zone(n_Antenna) ) ;

			casa::Vector<double> freqVector = time_freq.frequencyValues() ;

	  		IPosition shape (out.shape());			

  		//	Vector <Double> inColumn;
		
			Vector<DComplex> outColumn;
	
			FFTServer<Double,DComplex> server(IPosition(1,nofSamples_p),
					            FFTEnums::REALTOCOMPLEX);
	
	//	for (int antenna(0); antenna<shape(1); antenna++) {

		//inColumn = tbb_frame.column(antenna);
	       
		server.fft(outColumn,inColumn);	

		switch (nyquistZone_p) {
		       	case 1:
	                case 3:
		              out.column(n_Antenna) = outColumn;
		        break;
	                case 2:
			for (int channel (0); channel<shape(0); channel++) {
	  			out(channel,n_Antenna) = conj(outColumn(shape(0)-channel-1));
				}
		        break;
	      	       }
		}
			
		casa::Vector<double> frequencies = time_freq.frequencyValues() ;

		CR::GeomWeight gm_weight( antennaPositions,
					  CR::CoordinateType::Cartesian,
					  sky_Position,
					  CR::CoordinateType::AzElRadius,
					  anglesInDegrees,
					  farfield,
					  bufferDelays,
					  frequencies ,
					  bufferphases ,
					  bufferweights ) ;

  		CR::Beamformer bf( gm_weight ) ;

		bf.setSkymapType( SkymapQuantity::TIME_CC ) ;

//		bf.resize(bf.shapeBeam()) ;				

		bf.processData( beam,
				out ) ;

/*		casa::Matrix<double> IFFT( nofSamples_p, n_dipoles, 0.0 ) ;

		for( uint nAntenna =0; nAntenna < n_dipoles; nAntenna++ ) {

		uint fftLength_p = (nofSamples_p/2)+1 ;
	//	for( timesampled_Freq =0 ; timesampled_Freq < n_frames; timesampled_Freq++ ) {

			Vector<DComplex> inColumnFFT(fftLength_p) ;

			Vector<DComplex> fftData( fftLength_p) ;

			inColumnFFT = out.column(nAntenna) ;

			Vector<Double> invFFT (nofSamples_p,0.);	
 	
			FFTServer<Double,DComplex> server(IPosition(1,nofSamples_p),
							  FFTEnums::REALTOCOMPLEX);
		      	switch (nyquistZone_p) {
		      	case 1:
 	     		case 3:
			fftData = inColumnFFT;
			break;
      			case 2:
			#ifdef DEBUGGING_MESSAGES
			//cout << "DataReader::invfft Nyquist Zone == 2, inverting FFT." << endl;
			#endif
			for (uint channel (0); channel<fftLength_p; channel++) {
		  		fftData(channel) = conj(inColumnFFT(fftLength_p-channel-1));
				}
			break;
      			}
      			// inv-FFT the data block for the current antenna 
      			server.fft(invFFT,fftData);	
			
			IFFT.column(nAntenna) = invFFT ;
		     }			
     */        	
	   for( uint filling=0; filling < nofSamples_p ; filling++ ) {

// 		uint fill = start+ filling ;

		for( uint antennas=0; antennas < n_dipoles; antennas++ ) {

// 		  Phase_corrected( fill, antennas ) = IFFT(filling, antennas) ;
  	       }
	   }
          
        start = (frame+1)*nofSamples_p ;
	}
     }
        catch( AipsError x ){
	cerr << " NuMoonTrigger::Beam_forming" << x.getMesg () << endl ;
	return Vector<double> () ;
	}
   
   return beam.column(0);
  }
  
  //_____________________________________________________________________________
  //                                                                  root_ntuple
  
#ifdef HAVE_ROOT
  void NuMoonTrigger::root_ntuple( std::string const &filename,
				   SkymapQuantity const &quantity,
				   std::string const &group,
				   double const &samplingRate_p, 
				   uint const &nyquistZone_p )
    
  {
    
    cout << "To store data informaton in ntuples" << endl ;
    
    try {
//       CR::NuMoonTrigger numoonTrigger( filename,
// 				       quantity,
// 				       group,
// 				       samplingRate_p,
// 				       nyquistZone_p ) ;
      
      casa::Vector<Double> signal = Beam_forming( filename,
						  quantity,
						  group,
						  samplingRate_p, 
						  nyquistZone_p );	
      
      TNtuple *eventuple = new TNtuple("eventuple","eventuple","signal");
      
    }
    catch( AipsError x ){
      cerr << " NuMoonTrigger::root_ntuple" << x.getMesg () << endl ;
      return void () ;
    }
  }
#endif
  
  
  
} // Namespace  -- end
