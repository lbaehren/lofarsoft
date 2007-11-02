/*-------------------------------------------------------------------------*
 | $Id: template-class.cc,v 1.13 2007/06/13 09:41:37 bahren Exp          $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Observation/StationBeam.h>
 
#ifndef HAVE_CASACORE
  #warning StationBeam.cc needs casacore as CASA is broken. Not doing anything
#else

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  StationBeam::StationBeam ()
  {;}
  
  
  StationBeam::StationBeam( const Double& source_declination,
			    const Double& source_hr_angle,
			    const Double& sky_temp,
			    const Double& moon_temp,
			    const Vector<uint>& station_id,
			    const Double& freq_init,
			    const Double& bandwidth,
			    const Double& station_radii,
			    Vector<Double>& position_x,
			    Vector<Double>& position_y,
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
	 
	 Double pi(3.1416) ;	 
	 DComplex j(0, 1 );
	 DComplex Q(0.0);
	 DComplex power_x(0.0);
	 DComplex power_sum(0.0);
	 DComplex power_final(0.0);
	 
	 Double a(0.0);
	 Double b(0.0);
	 Double h1(0.0);
	 Double h2(0.0);
	 Double cos_zenith(0.0);
	 Double new_zenith(0.0);
	 Double new_azimuth(0.0);
	 Double L_cosine(0.0);
	 Double M_cosine(0.0);
	 Double sourceHrAngle = pi/180.*source_hr_angle ;
         Double sourceDeclination = pi/180.*source_declination ;
         Double hrAngle = pi/180.*hr_angle ;
         Double decAngle = pi/180.*declination ;
	 	 
	 Double power_dish(0.0);
	 Double xx(0.0);
	 Double d1(0.0);
	 Double c1(0.0);
	 Double k1(0.0);
	 Double k2(0.0);
	 Double cos_z_f_term(0.0);
	 Double cos_z_s_term(0.0);
	 Double cos_z_t_term(0.0);
	 	 
	 a = -station_radii ;
	 b = station_radii ;
	 
	 h1 = (b-a)/2 ;
	 h2 = (b+a)/2 ;
	 
         cos_z_f_term = cos(sourceHrAngle)*sin(sourceDeclination)*cos(hrAngle)*sin(decAngle);
         cos_z_s_term = sin(sourceHrAngle)*sin(sourceDeclination)*sin(hrAngle)*sin(decAngle) ;
         cos_z_t_term = cos(sourceDeclination)*cos(decAngle) ;

        cos_zenith = cos_z_f_term + cos_z_s_term + cos_z_t_term ;
	
	new_zenith = acos(cos_zenith) ;
	
	if (decAngle > sourceDeclination)
	   new_azimuth = abs(hrAngle-sourceHrAngle) ;
	   else 
	   	  if(decAngle < sourceDeclination)
		              new_azimuth = pi -abs(sourceHrAngle-hrAngle) ;
			                if ((decAngle < sourceDeclination) && (hrAngle >pi))
						   new_azimuth = pi + (sourceHrAngle - hrAngle) ;
         
	  L_cosine = sin(new_zenith)*cos(new_azimuth) ;
	  
	  M_cosine = sin(new_zenith)*sin(new_azimuth) ;
	  
        for( uint r =0; r <n_roots; r++ ){
	
	     DComplex power_x (0.0) ;
	     Double yy(0.0);
	     
	     xx = h1*legendre_root(r)+h2 ;
	    
	     d1 = station_radii*cos(pi/180.*sourceDeclination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
	     c1 = -station_radii*cos(pi/180.*sourceDeclination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
	     k1 =(d1-c1)/2. ;
	     
	     k2 =(d1+c1)/2. ;
	     
	     for( uint s=0; s <n_roots; s++ ){
	     
	         yy = k1*legendre_root(s)+k2 ;
		 
		 Q = exp(-2*pi*j*frequency/3e8*(L_cosine*xx+M_cosine*yy)) ;
		 
		 power_x = power_x + legendre_weight(s)*(Q) ;
		 
	     }
	     power_sum = power_sum + legendre_weight(r)*k1*power_x ;
	    
        }
	power_final = h1*power_sum ;    
	
       power_dish = abs(h1*power_sum/(2.*pi*(3e8/frequency)*(3e8/frequency)))*abs(h1*power_sum/(2.*pi*(3e8/frequency)*(3e8/frequency))) ;
	
       return power_dish ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::station_beam " << x.getMesg () << endl ;
      return Double ();
    }       
	       
 }
 
  Double StationBeam::tied_array( const Double& frequency,
	                          const Double& hr_angle,
		                  const Double& declination,
		                  const Double& source_declination,
		                  const Double& source_hr_angle,
		                  const Double& station_radii,
				  const Vector<uint>& station_id,
		                  Vector<Double>& position_x,
		                  Vector<Double>& position_y,
		                  const Vector<Double>& legendre_root,
		                  const Vector<Double>& legendre_weight ) 
     
 {
   try {
   
        StationBeam stbm ;
        
	Double pi(3.1416) ;
	DComplex j(0, 1 );
        DComplex sum_tied_array(0.0);
	Double integral_value (0.0) ;
	
	uint n_stations = station_id.nelements() ;
	uint n_stat = 0 ;
	
	Double observed_L(0.0) ;
	Double observed_M(0.0) ;
	Double observed_N(0.0) ;
	Double pos_new_x(0.0) ;
	Double pos_new_y(0.0) ;

	Double position_new_x(0.0);
	Double position_new_y(0.0);
	Double position_new_y1(0.0);
	Double position_new_y2(0.0);
	
        Double sourceHrAngle = pi/180.*source_hr_angle ;
        Double sourceDeclination = pi/180.*source_declination ;
        Double hrAngle = pi/180.*hr_angle ;
        Double decAngle = pi/180.*declination ;

	observed_L = sin(decAngle-sourceDeclination)*cos(hrAngle-sourceHrAngle) ;
	
	observed_M = sin(decAngle-sourceDeclination)*sin(hrAngle-sourceHrAngle) ;
		
	observed_N = cos(decAngle-sourceDeclination) ;
		
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
	     
	     position_new_x = pos_new_x*cos(sourceHrAngle)-pos_new_y*sin(sourceHrAngle) ;
	     position_new_y1 = pos_new_x*cos(pi/180.*sourceDeclination)*sin(pi/180.*sourceHrAngle) ;
	     position_new_y2 = pos_new_y*cos(pi/180.*sourceHrAngle)*cos(pi/180.*sourceDeclination) ;
	     
	     position_new_y = position_new_y1 +position_new_y2 ;
             
	    sum_tied_array = sum_tied_array +stationbeam*exp(2*pi*j*(frequency/3e8)*(position_new_x*observed_L+position_new_y*observed_M));
	  }
       
       integral_value = abs(sum_tied_array)*abs(sum_tied_array);	     	     
													
       return integral_value ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::tied_array " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }
 
 
 	
Double StationBeam::integrate_freq( const Double& hr_angle,
                                    const Double& declination,
                                    const Double& source_declination,
	                            const Double& source_hr_angle,
	                            const Double& station_radii,
	                            const Vector<uint>& station_id,
		                    const Double& freq_init,
		                    const Double& bandwidth,
                                    const Double& freq_interval,
		                    Vector<Double>& position_x,
		                    Vector<Double>& position_y,
                                    const Vector<Double>& legendre_root,
             		            const Vector<Double>& legendre_weight )   

{
   try {
          StationBeam stbm ;
         	 
          uint nroots = legendre_root.nelements() ;
          
          Double freq_final = freq_init + bandwidth ;
     
          Double power_beam(0.0) ;
          Double power_sum(0.0); 
          
      	  Double k1_freq(0.0) ;
          Double k2_freq(0.0) ;
          Double k1_f(0.0);
          Double k2_f(0.0);
          Double freq(0.0);
          Double y_outer_sum(0.0);
          Double y_inner_sum(0.0);

          while( k1_freq < freq_final ) {

	      k2_freq = k1_freq + freq_interval ;
              k1_f = (k2_freq - k1_freq)/2. ;
              k2_f = (k2_freq +k1_freq )/2. ;
             
              y_inner_sum = 0;

              for(uint ff=0; ff< nroots; ff++ ) {
  
		  freq = k1_f*legendre_root(ff)+k2_f ;
		                    
                  power_beam = stbm.tied_array( freq,
	                                        hr_angle,
		                                declination,
		                                source_declination,
		                                source_hr_angle,
		                                station_radii,
				                station_id,
		                                position_x,
		                                position_y,
		                                legendre_root,
		                                legendre_weight ) ;
       
		  y_inner_sum = y_inner_sum + legendre_weight(ff)*power_beam ;
	      }

              y_outer_sum = y_outer_sum + k1_f*y_inner_sum ;
              k1_freq = k2_freq ;
	  }
            
     return y_outer_sum ;
   }
      
   catch( AipsError x ){
   cerr << "StationBeam::integrate_freq " << x.getMesg () << endl ;
   return Double ();
  }       
}	


Double StationBeam::beamwidth_decli(  const Double& source_declination,
                                      const Double& source_hr_angle,
	                              const Double& station_radii,
	                              const Vector<uint>& station_id,
	                              const Double& freq_init,
		                      const Double& bandwidth,
                                      const Double& freq_interval,
		                      Vector<Double>& position_x,
		                      Vector<Double>& position_y,
                                      const Vector<Double>& legendre_root,
             	                      const Vector<Double>& legendre_weight )   

{
   try {
          StationBeam stbm ;
          
          Double pi = 3.1416 ;	 
          uint nroots = legendre_root.nelements() ;
          
          
       Double max_power = stbm.integrate_freq( source_hr_angle,
                                               source_declination,
                                               source_declination,
	                                       source_hr_angle,
	                                       station_radii,
	                                       station_id,
		                               freq_init,
		                               bandwidth,
                                               freq_interval,
		                               position_x,
		                               position_y,
                                               legendre_root,
             		                       legendre_weight )   ;

          Double decAngle_min = (source_declination );
          Double decAngle_max = (source_declination + 2.0) ;
  
          Double interval = 0.005;
          
          uint loop_dec = int((decAngle_max - decAngle_min)/interval) ;
          Double decAngle( 0.0 ) ;

          Double power_cal( 0.0) ;
          Vector<Double> power(loop_dec,0.0);
          power_cal = max_power ;
          Double power_compare(0.0);

          Vector<Double> power_normalized( loop_dec, 0.0);

	  Vector<Double> dec_vector( loop_dec, 0.0 );
          Vector<Double> selected_declinations( loop_dec,0.0 );
          
          Double declination_min(0.0);
          Double declination_max(0.0);
	  Double declination_width(0.0);
          dec_vector(0)=decAngle_min ;
          power(0)=power_cal;
          uint i =1 ;

          do
	   {
              power_compare = power_cal ;
    
              decAngle =  decAngle_min + (i+1)*interval ;
              
	      dec_vector(i) = decAngle ;

              power_cal = stbm.integrate_freq( source_hr_angle,
                                             decAngle,
                                             source_declination,
	                                     source_hr_angle,
	                                     station_radii,
	                                     station_id,
		                             freq_init,
		                             bandwidth,
                                             freq_interval,
		                             position_x,
		                             position_y,
                                             legendre_root,
             		                     legendre_weight ) ;
              
              power(i)= power_cal ;
      
	      i = i+1 ;
	  
	   }

	  while( power_compare > power_cal ) ;
          
	  Double power_max = max_power ;
	 
          Double power_min = power_cal ;
                           
          power_normalized = power/power_max ;
         
          Double power_norm_min = power_min/max_power ;
   
          Double power_norm_max = power_max/max_power ;
 
          Double half_power =(power_norm_max -power_norm_min)/2 ;
  
          Double power_optimum = power_norm_max - half_power ;
          
           selected_declinations.resize(ntrue( power_normalized >half_power));
         
           selected_declinations = dec_vector(power_normalized >half_power).getCompressedArray();

          uint n_elements = selected_declinations.nelements() ;
      
          declination_max = selected_declinations(n_elements-1);
          cout << "declination maximum : " << declination_max << endl ;

          declination_min = selected_declinations(0) ;
          cout << " declination minimum : " << declination_min << endl ;

//          Double d_max = 
          
          Double x_width = cos(pi/180.*source_hr_angle)*sin(pi/180.*declination_min)*cos(pi/180.*source_hr_angle)*sin(pi/180.*declination_max) ;
          Double y_width = sin(pi/180.*source_hr_angle)*sin(pi/180.*declination_min)*sin(pi/180.*source_hr_angle)*sin(pi/180.*declination_max) ;
          Double z_width = cos(pi/180.*declination_min)*cos(pi/180.*declination_max);

          declination_width = 180./pi*acos(x_width+ y_width+ z_width) ;

          return declination_width ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::beamwidth_decli " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }

 
Double StationBeam::beamwidth_hr(  const Double& source_declination,
                                      const Double& source_hr_angle,
	                              const Double& station_radii,
	                              const Vector<uint>& station_id,
	                              const Double& freq_init,
		                      const Double& bandwidth,
                                      const Double& freq_interval,
		                      Vector<Double>& position_x,
		                      Vector<Double>& position_y,
                                      const Vector<Double>& legendre_root,
             	                      const Vector<Double>& legendre_weight )   

{
   try {
          StationBeam stbm ;
          
          Double pi = 3.1416 ;	 
          uint nroots = legendre_root.nelements() ;
          
          
          Double max_power = stbm.integrate_freq( source_hr_angle,
                                               source_declination,
                                               source_declination,
	                                       source_hr_angle,
	                                       station_radii,
	                                       station_id,
		                               freq_init,
		                               bandwidth,
                                               freq_interval,
		                               position_x,
		                               position_y,
                                               legendre_root,
             		                       legendre_weight )   ;

          Double hrAngle_min = (source_hr_angle );
          Double hrAngle_max = (source_hr_angle + 15.0) ;
  
          Double interval = 0.1;
          
          uint loop_hr = int((hrAngle_max - hrAngle_min)/interval) ;
          Double hrAngle( 0.0 ) ;

          Double power_cal( 0.0) ;
          Vector<Double> power(loop_hr,0.0);
          power_cal = max_power ;
          Double power_compare(0.0);

          Vector<Double> power_normalized( loop_hr, 0.0);

	  Vector<Double> hr_vector( loop_hr, 0.0 );
          Vector<Double> selected_hrangles(loop_hr,0.0 );
          
          Double hr_min(0.0);
          Double hr_max(0.0);
	  Double hr_width(0.0);
          hr_vector(0) =hrAngle_min ;
          power(0)=power_cal;
          uint i =1 ;

          do
	   {
              power_compare = power_cal ;
              hrAngle =  hrAngle_min + (i+1)*interval ;
              
	      hr_vector(i) = hrAngle ;

              power_cal = stbm.integrate_freq( hrAngle,
                                               source_declination,
                                             source_declination,
	                                     source_hr_angle,
	                                     station_radii,
	                                     station_id,
		                             freq_init,
		                             bandwidth,
                                             freq_interval,
		                             position_x,
		                             position_y,
                                             legendre_root,
             		                     legendre_weight ) ;
              
              power(i)= power_cal ;
      
	      i = i+1 ;
	  
	   }

	  while( power_compare > power_cal ) ;
          
	  Double power_max = max_power ;
	 
          Double power_min = power_cal ;
                           
          power_normalized = power/power_max ;
         
          Double power_norm_min = power_min/max_power ;
  
          Double power_norm_max = power_max/max_power ;
    
          Double half_power =(power_norm_max -power_norm_min)/2 ;
       
          Double power_optimum = power_norm_max - half_power ;
          
           selected_hrangles.resize(ntrue( power_normalized >half_power));
         
           selected_hrangles = hr_vector(power_normalized >half_power).getCompressedArray();

          uint n_elements = selected_hrangles.nelements() ;
    
          hr_max = selected_hrangles(n_elements-1);
          cout << "hr angle  maximum : " << hr_max << endl ;

          hr_min = selected_hrangles(0) ;
          cout << " hr angle minimum : " << hr_min << endl ;

//          Double d_max = 
          
          Double x_width = cos(pi/180.*hr_max)*sin(pi/180.*source_declination)*cos(pi/180.*hr_min)*sin(pi/180.*source_declination) ;
          Double y_width = sin(pi/180.*hr_max)*sin(pi/180.*source_declination)*sin(pi/180.*hr_min)*sin(pi/180.*source_declination) ;
          Double z_width = cos(pi/180.*source_declination)*cos(pi/180.*source_declination);

          hr_width = 180./pi*acos(x_width+ y_width+ z_width) ;

          return hr_width ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::beamwidth_hr " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }         
         

Double StationBeam::power_moon( const Double& source_declination,
	                        const Double& source_hr_angle,
	                        const Double& station_radii,
	                        const Vector<uint>& station_id,
		                const Double& freq_init,
		                const Double& bandwidth,
                                const Double& freq_interval,
		                Vector<Double>& position_x,
		                Vector<Double>& position_y,
                                const Vector<Double>& legendre_root,
             		        const Vector<Double>& legendre_weight )
{

  try {
       StationBeam stbm ;

       Double power_moon(0.0);
     
       Double pi = 3.1416 ;	 
       uint nroots = legendre_root.nelements() ;

       Double max_power = stbm.integrate_freq( source_hr_angle,
                                               source_declination,
                                               source_declination,
	                                       source_hr_angle,
	                                       station_radii,
	                                       station_id,
		                               freq_init,
		                               bandwidth,
                                               freq_interval,
		                               position_x,
		                               position_y,
                                               legendre_root,
             		                       legendre_weight )   ;
       cout << " maximum power at the source location :" << max_power << endl ;
       Double declination_min = source_declination - 0.25 ;
       Double declination_max = source_declination + 0.25 ;
       
       Double declination_interval = (declination_max - declination_min)/100 ;

       Double decli_1 = declination_min ;
       Double decli_2(0.0) ;
       Double declination( 0.0);

       Double hr_1(0.0) ;
       Double hr_2(0.0) ;

       while( decli_1 < declination_max ) {
             
	   decli_2 = decli_1 + declination_interval ;
           Double decli_1x = (decli_2 - decli_1)/2 ;
           Double decli_2x = (decli_2 + decli_1)/2 ;
           Double hr_outer_sum =0.0 ;

           for( uint dd =0; dd <nroots; dd++ ) {

	      declination = decli_1x*legendre_root(dd) + decli_2x ;
        hr_2 = source_hr_angle + 1/sin(declination*pi/180.)*sqrt(0.25*0.25-(abs(declination-source_declination))*(abs(declination-source_declination)));
	hr_1 = source_hr_angle - 1/sin(declination*pi/180.)*sqrt(0.25*0.25-(abs(declination-source_declination))*(abs(declination-source_declination)));
             
              Double hr_1y = (hr_2 -hr_1)/2 ;
              Double hr_2y = (hr_2 +hr_1)/2 ;
              Double hr_inner_sum =0.0 ;
              Double sum_freq(0.0);

              for( uint hh =0; hh <nroots; hh++ ) {

		  Double hr_angle = hr_1y*legendre_root(hh)+ hr_2y ;

                  sum_freq = stbm.integrate_freq( hr_angle,
                                                  declination,
                                                  source_declination,
	                                          source_hr_angle,
	                                          station_radii,
	                                          station_id,
		                                  freq_init,
		                                  bandwidth,
                                                  freq_interval,
		                                  position_x,
		                                  position_y,
                                                  legendre_root,
             		                          legendre_weight ) ;

		  hr_inner_sum = hr_inner_sum + legendre_weight(hh)*(sum_freq/max_power)*sin(pi/180.*declination) ;
              }
	      hr_outer_sum = hr_outer_sum + legendre_weight(dd)*hr_1y*(pi/180.)*hr_inner_sum ;

	   }
           power_moon = power_moon + decli_1x*(pi/180.)* hr_outer_sum ;
          
           decli_1 = decli_2 ;   
        }
      return power_moon ;
    }
      
    catch( AipsError x ){
    cerr << "StationBeam::power_moon " << x.getMesg () << endl ;
    return Double ();
  }       
	       
 }




Double StationBeam::min_power_moon( const Double& source_declination,
	                            const Double& source_hr_angle,
	                            const Double& station_radii,
	                           const Vector<uint>& station_id,
		                   const Double& freq_init,
		                   const Double& bandwidth,
                                   const Double& freq_interval,
		                   Vector<Double>& position_x,
		                   Vector<Double>& position_y,
                                   const Vector<Double>& legendre_root,
             		           const Vector<Double>& legendre_weight )
{

  try {
       StationBeam stbm ;

       Double min_power_moon(1e20);
       Double min_d_position(0.0);
       Double min_h_position(0.0);

       Double pi = 3.1416 ;	 
       uint nroots = legendre_root.nelements() ;

       Double declination_min = source_declination - 0.25 ;
       Double declination_max = source_declination + 0.25 ;
       uint declination_loop = 20 ;
       Double declination_interval = (declination_max - declination_min)/declination_loop ;

       Double decli_1 = declination_min ;
       Double decli_2(0.0) ;
       Double declination( 0.0);

       Double hr_1(0.0) ;
       Double hr_2(0.0) ;

       Double sum_freq(0.0) ;

       for( uint dd =0; dd< declination_loop ; dd++ ) {

	  declination = declination_min +dd*declination_interval ;

          hr_2 = source_hr_angle + 1/sin(declination*pi/180.)*sqrt(0.25*0.25-(abs(declination-source_declination))*(abs(declination-source_declination)));
	  hr_1 = source_hr_angle - 1/sin(declination*pi/180.)*sqrt(0.25*0.25-(abs(declination-source_declination))*(abs(declination-source_declination)));
             
          uint hrangle_loop = 10 ;

          Double hrangle_interval = (hr_2 - hr_1)/hrangle_loop ;

	  Double hrangle(0.0) ;

          for( uint hh =0; hh < hrangle_loop; hh++ ) {

	      hrangle = hr_1 + hh*hrangle_interval ;

	          sum_freq = stbm.integrate_freq( hrangle,
                                                  declination,
                                                  source_declination,
	                                          source_hr_angle,
	                                          station_radii,
	                                          station_id,
		                                  freq_init,
		                                  bandwidth,
                                                  freq_interval,
		                                  position_x,
		                                  position_y,
                                                  legendre_root,
             		                          legendre_weight ) ;

		  if( sum_freq < min_power_moon ) {

                      min_power_moon = sum_freq ;
                      min_h_position = hrangle ;
                      min_d_position = declination ;
		  }

	  }

       }

       cout << " minimum declination position : " << min_d_position << endl ;
       cout << "minimum hr angle position :" << min_h_position << endl ;
     
      return min_power_moon ;
    }
      
    catch( AipsError x ){
    cerr << "StationBeam::power_moon " << x.getMesg () << endl ;
    return Double ();
  }       
	       
 }



 Double StationBeam::power_sky(  const Double& source_declination,
	                         const Double& source_hr_angle,
	                         const Double& station_radii,
	                         const Vector<uint>& station_id,
		                 const Double& freq_init,
		                 const Double& bandwidth,
                                 const Double& freq_interval,
		                 Vector<Double>& position_x,
		                 Vector<Double>& position_y,
                                 const Vector<Double>& legendre_root,
             	                 const Vector<Double>& legendre_weight )
{

  try {
       StationBeam stbm ;

       Double power_sky(0.0);
     
       Double pi = 3.1416 ;	 
       uint nroots = legendre_root.nelements() ;

       Double max_power = stbm.integrate_freq( source_hr_angle,
                                               source_declination,
                                               source_declination,
	                                       source_hr_angle,
	                                       station_radii,
	                                       station_id,
		                               freq_init,
		                               bandwidth,
                                               freq_interval,
		                               position_x,
		                               position_y,
                                               legendre_root,
             		                       legendre_weight )   ;
     
       Double declination_min = 0.0 ;
       Double declination_max = 90.0 ;
       
       Double declination_interval = (declination_max - declination_min)/360 ;

       Double decli_1 = declination_min ;
       Double decli_2(0.0) ;
       Double declination( 0.0);
       
       Double hrangle_min = 0.0;
       Double hrangle_max = 360.0 ;

       Double hr_angle_interval = 5.0 ;

       Double hr_1(0.0)  ;
       Double hr_2(0.0) ;

       while( decli_1 < declination_max ) {
             
	   decli_2 = decli_1 + declination_interval ;
           Double decli_1x = (decli_2 - decli_1)/2 ;
           Double decli_2x = (decli_2 + decli_1)/2 ;
           
           Double decli_inner_sum =0.0 ;

           for( uint dd =0; dd <nroots; dd++ ) {

	      declination = decli_1x*legendre_root(dd) + decli_2x ;
              Double hr_outer_sum = 0.0 ;
              
              while( hr_1< hrangle_max ) {

		  hr_2 = hr_1 + hr_angle_interval ;
                  Double hr_1y = (hr_2 - hr_1)/2 ;
                  Double hr_2y = (hr_2 + hr_1)/2 ;

		  Double hr_inner_sum = 0.0 ;
                  Double sum_freq = 0.0 ;
 
                  for( uint hh=0; hh <nroots; hh++ ) {

		      Double hr_angle = hr_1y*legendre_root(hh) + hr_2y ;

                      sum_freq = stbm.integrate_freq( hr_angle,
                                                      declination,
                                                      source_declination,
	                                              source_hr_angle,
	                                              station_radii,
	                                              station_id,
		                                      freq_init,
		                                      bandwidth,
                                                      freq_interval,
		                                      position_x,
		                                      position_y,
                                                      legendre_root,
             		                              legendre_weight ) ;

		      hr_inner_sum = hr_inner_sum + legendre_weight(hh)*(sum_freq/max_power)*sin(pi/180.*declination) ;
                }
	        hr_outer_sum = hr_outer_sum + hr_1y*(pi/180.)*hr_inner_sum ;
	        hr_1 = hr_2 ;
	     }
             hr_1 = hrangle_min ; 
             decli_inner_sum = decli_inner_sum + legendre_weight(dd)*hr_outer_sum ; 
             
          }
	  power_sky = power_sky + decli_1x*(pi/180.)*decli_inner_sum ;
          decli_1 = decli_2 ;   
//	  cout << " declination angle for the integration over sky : " <<  decli_1 << " power sky " << power_sky << endl ;           
      }
      return power_sky ;
    }
      
    catch( AipsError x ) {
    cerr << "StationBeam::power_sky " << x.getMesg () << endl ;
    return Double ();
  }    
}

	         
    

} // Namespace CR -- end

#endif
