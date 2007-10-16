/*-------------------------------------------------------------------------*
 | $Id: template-class.cc,v 1.13 2007/06/13 09:41:37 bahren Exp          $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                                 *
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

#include <Observation/StationBeam.h>
 
namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  StationBeam::StationBeam ()
  {;}
  
  
    StationBeam( const Double& source_declination,
                 const Double& source_hr_angle,
		 const Vector<uint>& station_id,
                 const Double& init_freq,
		 const Double& bandwidth,
		 const Double& station_radii,
		 const Vector<Double>& position_x,
		 const Vector<Double>& position_y,
		 const Vector<Double>& legendre_root,
		 const Vector<Double>& legendre_weight ) 

 { ; }   
 
  StationBeam::StationBeam (StationBeam const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  StationBeam::~StationBeam ()
  {
    destroy();
  }
  
  void StationBeam::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  StationBeam& StationBeam::operator= (StationBeam const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void StationBeam::copy (StationBeam const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void StationBeam::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  Double StationBeam::station_beam( const Double& frequency,
				    const Double& hr_angle,
			       	    const Double& declination,
				    const Double& source_declination,
      	       		            const Double& source_hr_angle,
			            const Double& station_radii,
			            const Vector<Double>& legendre_root,
			            const Vector<Double>& legendre_weight ) 

  {
    try {
        
         uint n_roots = legendre_root.nelements() ;
	 uint n_weights = legendre_weight.nelements();
	 
	 DComplex j(0, 1 );
	 DComplex Q(0.0);
	 uint r(0);
	 uint s(0);
	 
	 Double a(0.0);
	 Double b(0.0);
	 Double h1(0.0);
	 Double h2(0.0);
	 Double cos_zenith(0.0);
	 Double new_zenith(0.0);
	 Double new_azimuth(0.0);
	 Double L_cosine(0.0);
	 Double M_cosine(0.0);
	 Double power_sum(0.0);
	 Double power_dish(0.0);
	 Double xx(0.0);
	 Double d1(0.0);
	 Double c1(0.0);
	 Double k1(0.0);
	 Double k2(0.0);
	 
	 	 
	 a = -station_radii ;
	 b = station_radii ;
	 
	 h1 = (b-a)/2 ;
	 h2 = (b+a)/2 ;
	 
	 cos_zenith = cos(0.0174533*source_hr_angle)*sin(0.0174533*source_declination)*cos(0.0174533*hr_angle)*sin(0.0174533*declination)+
	              sin(0.0174533*source_hr_angle)*sin(0.0174533*source_declination)*sin(0.0174533*hr_angle)*sin(0.0174533*declination)+
		      cos(0.0174533*source_declination)*cos(0.0174533*declination) ;
		      
	new_zenith = acos(cos_zenith) ;
	
	if (declination > source_declination)
	  { new_azimuth = abs(hr_angle-source_hr_angle)} ;
        else 
         { new_azimuth = 180 -abs(hr_angle-source_hr_angle)} ;

        if (declination < source_declination+90)
	  { new_azimuth = 180 + (source_hr_angle - hr_angle)} ;

	  L_cosine = sin(0.0174533*new_zenith)*cos(0.0174533*new_azimuth) ;
	  M_cosine = sin(0.0174533*new_zenith)*sin(0.0174533*new_azimuth) ;
	  
        for( uint r =0; r <n_roots; r++ ){
	
	     Double power_x (0.0) ;
	     Double yy(0.0);
	     
	     xx = h1*legendre_root(r)+h2 ;
	     d1 = station_radii*cos(0.0174533*source_declination)*sqrt(1-(xx/station_radii)^2) ;
	     c1 = -station_radii*cos(0.0174533*source_declination)*sqrt(1-(xx/station_radii)^2) ;
	     k1 =(d1-c1)/2 ;
	     k2 =(d1+c1)/2 ;
	     
	     for( uint s=0; s <n_roots; s++ ){
	     
	         yy = k1*legendre_root(s)+k2 ;
		 Q = exp(-2*3.1416*j*frequency/3e8*(L_cosine*xx+M_cosine*yy)) ;
		 power_x = power_x + legendre_weight(s)*Q ;
	     }
	     power_sum = power_sum + legendre_weight(r)*k1*power_x ;
        }
	     
	power_dish = abs(h1*power_sum)/(2*3.1416*(3e8/frequency)^2) ;
	
       return power_dish ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::station_beam " << x.getMesg () << endl ;
      return Matrix<Double> ();
    }       
	       
 }
 
  Double StationBeam::tied_array( const Double& frequency,
	                          const Double& hr_angle,
		                  const Double& declination,
		                  const Double& source_declination,
		                  const Double& source_hr_angle,
		                  const Double& station_radii,
				  const Vector<uint>& station_id,
		                  const Vector<Double>& position_x,
		                  const Vector<Double>& position_y,
		                  const Vector<Double>& legendre_root,
		                  const Vector<Double>& legendre_weight ) 
     
 {
   try {
   
        StationBeam stbm ;
        
	DComplex j(0, 1 );
        DComplex sum_tied_array(0.0);
	Double integral_value (0.0) ;
	
	uint nofcoeff = Legendre_coeff.nelements();
	uint nofroots = Legendre_root.nelements();
	uint n_stations = station_id.nelements() ;
	int n_stat = 0 ;
	
	Double observed_L(0.0) ;
	Double observed_M(0.0) ;
	Double observed_N(0.0) ;
	Double pos_new_x(0.0) ;
	Double pos_new_y(0.0) ;
	
	observed_L = sin(0.0174533*(declination-source_declination))*cos(0.0174533*(hr_angle-source_hr_angle)) ;
	observed_M = sin(0.0174533*(declination-source_declination))*sin(0.0174533*(hr_angle-source_hr_angle)) ;
	observed_N = cos(0.0174533*(declination-source_declination)) ;
	
	Double stationbeam = stbm.station_beam( frequency,
				                hr_angle,
			       	                declination,
				                source_declination,
      	       		                        source_hr_angle,
			                        station_radii,
			                        legendre_root,
			                        legendre_weight ) ;
	
						
        
        for( uint ns=0; ns < n_stations; ns++ ){
	
	     n_stat = station_id(ns) ;
	     pos_new_x = position_x(n_stat) ;
	     pos_new_y = position_y(n_stat) ;
	     
	     position_x = cos(0.0174533*source_hr_angle)*pos_new_x-sin(0.0174533*source_hr_angle)*pos_new_y ;
	     position_y = cos(0.0174533*source_declination)*sin(0.0174533*source_hr_angle)*pos_new_x+
	                  cos(0.0174533*source_hr_angle)*cos(0.0174533*source_declination)*pos_new_y ;
            
            sum_tied_array = sum_tied_array +stationbeam*exp(2*3.1416*j*freq/3e8*(position_x*observed_L+ position_y*observed_M)) ;
	  }
       
       integral_value = abs(sum_tied_array);	     	     
													
       return integral_value ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::gaussian_quad " << x.getMesg () << endl ;
      return Double ();    }       
	       
 }
	

 
  
	         
    

} // Namespace CR -- end
