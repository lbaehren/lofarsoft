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
	    
	     d1 = station_radii*cos(sourceDeclination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
	     c1 = -station_radii*cos(sourceDeclination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
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
 

    
Double StationBeam::incoherent_beam( const Double& frequency,
	                             const Double& hr_angle,
		                     const Double& declination,
		                     const Double& source_declination,
		                     const Double& source_hr_angle,
		                     const Double& station_radii,
		                     const Vector<uint>& station_id,
		                     const Vector<Double>& legendre_root,
		                     const Vector<Double>& legendre_weight ) 
{
   try{

        StationBeam stbm ;
        
	Double pi(3.1416) ;
	DComplex j(0, 1 );
        Double sum_incoherent(0.0);
	Double integral_value (0.0) ;
	
	uint n_stations = station_id.nelements() ;
	uint n_stat = 0 ;

	
	Double stationbeam = stbm.station_beam( frequency,
				                hr_angle,
			       	                declination,
				                source_declination,
      	       		                        source_hr_angle,
			                        station_radii,
			                        legendre_root,
			                        legendre_weight ) ;
						
        for( uint ns=0; ns < n_stations; ns++ ){
	            
	    sum_incoherent = sum_incoherent +stationbeam ;
	  }
       
       integral_value = sum_incoherent;	     	     
       cout << " power sum for incoherent addition : " << integral_value << endl ;
												
       return integral_value ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::incoherent_beam " << x.getMesg () << endl ;
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
	     position_new_y1 = pos_new_x*cos(sourceDeclination)*sin(sourceHrAngle) ;
	     position_new_y2 = pos_new_y*cos(sourceHrAngle)*cos(sourceDeclination) ;
	     
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
		                    
		  power_beam = stbm. tied_array( freq,
                                                 hr_angle,
		                                 declination,
		                                 source_declination,
		                                 source_hr_angle,
		                                 station_radii,
				                 station_id,
		                                 position_x,
		                                 position_y,
		                                 legendre_root,
		                                 legendre_weight);

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


 	
Double StationBeam::incoherent_beaming( const Double& hr_angle,
                                        const Double& declination,
                                        const Double& source_declination,
	                                const Double& source_hr_angle,
	                                const Double& station_radii,
	                                const Vector<uint>& station_id,
		                        const Double& frequency,
		                        Vector<Double>& position_x,
		                        Vector<Double>& position_y,
                                        const Vector<Double>& legendre_root,
             		               const Vector<Double>& legendre_weight )   

{
   try {
      
        StationBeam stbm ;
        
	Double pi(3.1416) ;
	DComplex j(0, 1 );
        Double sum_tied_array(0.0);
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
	
	      sum_tied_array = sum_tied_array +stationbeam ;
	  }
       
       integral_value = sum_tied_array ;	     	     
													
       return integral_value ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::sky_mapping " << x.getMesg () << endl ;
      return Double ();
      }       

}	


	
Double StationBeam::integrate_phi( const Double& declination,
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
          
          Double phi_final(360.0);
          Double phi_init(0.0);
          Double phi_interval(1.0);

          Double power_beam(0.0) ;
          Double power_sum(0.0); 
          Double hrangle(0.0);

      	  Double k1_phi(0.0) ;
          Double k2_phi(0.0) ;
          Double k1phi(0.0);
          Double k2phi(0.0);
          Double phi(0.0);
          Double y_outer_sum(0.0);
          Double y_inner_sum(0.0);

          while( k1_phi < phi_final ) {

	      k2_phi = k1_phi + phi_interval ;
              k1phi = (k2_phi - k1_phi)/2. ;
              k2phi = (k2_phi +k1_phi )/2. ;
             
              y_inner_sum = 0;

              for(uint phi=0; phi< nroots; phi++ ) {
  
		  hrangle = k1phi*legendre_root(phi)+k2phi ;
		  
                power_beam = stbm.integrate_freq( hrangle,
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
       
		  y_inner_sum = y_inner_sum + legendre_weight(phi)*power_beam ;
	      }

              y_outer_sum = y_outer_sum + k1phi*y_inner_sum ;
              k1_phi = k2_phi ;
	  }
            
     return y_outer_sum ;
   }
      
   catch( AipsError x ){
   cerr << "StationBeam::integrate_phi " << x.getMesg () << endl ;
   return Double ();
  }       
}	


	
Double StationBeam::integrate_decli( const Double& source_declination,
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
          Double pi = 3.1416 ;

          Double decli_final(90.0);
          Double decli_init(0.0);
          Double decli_interval(0.25);
          Double declination(0.0);

          Double power_beam(0.0) ;
          Double power_sum(0.0); 
          
      	  Double k1_decli(0.0) ;
          Double k2_decli(0.0) ;
          Double k1decli(0.0);
          Double k2decli(0.0);
          Double decli(0.0);
          Double y_outer_sum(0.0);
          Double y_inner_sum(0.0);

          while( k1_decli < decli_final ) {

	      k2_decli = k1_decli + decli_interval ;
              k1decli = (k2_decli - k1_decli)/2. ;
              k2decli = (k2_decli +k1_decli )/2. ;
             
              cout << " declination andgle " << k1_decli << endl ;
              y_inner_sum = 0;

              for(uint decli=0; decli< nroots; decli++ ) {
  
		  declination = k1decli*legendre_root(decli)+ k2decli ;
			
                  power_beam = stbm.integrate_phi( declination,
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

		  y_inner_sum = y_inner_sum + legendre_weight(decli)*power_beam*sin(pi/180*declination) ;
	      }

              y_outer_sum = y_outer_sum + k1decli*y_inner_sum ;
              k1_decli = k2_decli ;
	  }
            
     return y_outer_sum ;
   }
      
   catch( AipsError x ){
   cerr << "StationBeam::integrate_decli " << x.getMesg () << endl ;
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
          Double decAngle_max = (source_declination + 5.0) ;
  
          Double interval = 0.001;
          
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
          dec_vector(0)= decAngle_min ;
          power(0)= power_cal;
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
          
          declination_min = selected_declinations(0) ;
         
Double x_width = cos(pi/180.*source_hr_angle)*sin(pi/180.*declination_min)*cos(pi/180.*source_hr_angle)*sin(pi/180.*declination_max) ;
Double y_width = sin(pi/180.*source_hr_angle)*sin(pi/180.*declination_min)*sin(pi/180.*source_hr_angle)*sin(pi/180.*declination_max) ;
Double z_width = cos(pi/180.*declination_min)*cos(pi/180.*declination_max);

          declination_width = 2*180./pi*acos(x_width+ y_width+ z_width) ;
	  cout << " declination width : " <<  declination_width << endl ;
	  
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
          Double hrAngle_max = (source_hr_angle + 20.0) ;
  
          Double interval = 0.005 ;
          
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
    
          hr_min = selected_hrangles(0) ;
          
          Double x_width = cos(pi/180.*hr_max)*sin(pi/180.*source_declination)*cos(pi/180.*hr_min)*sin(pi/180.*source_declination) ;
          Double y_width = sin(pi/180.*hr_max)*sin(pi/180.*source_declination)*sin(pi/180.*hr_min)*sin(pi/180.*source_declination) ;
          Double z_width = cos(pi/180.*source_declination)*cos(pi/180.*source_declination);

          hr_width = 2*180./pi*acos(x_width+ y_width+ z_width)*sin(source_declination*pi/180.) ;
          cout << " hr angle width : " <<  hr_width << endl ;
	  
          return hr_width ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::beamwidth_hr " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }         
         

void StationBeam::generate_statistics_table( const Double& station_radii,
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
       
       ofstream logfile1 ;
       int n_stations = station_id.nelements() ;
       
       logfile1.open("beamStatistics",ios::out);
       
       logfile1 << " Number of stations used are : " << n_stations << endl ;
       
       logfile1 << " Stations which are used in tied array beaming : [ " << "\t" ;
       
       for( uint ns =0; ns< n_stations; ns++ ) {
       
            uint station = station_id(ns) ;
	    
	    logfile1 << station << "," << "\t " ;
	    
       }
        logfile1 << "]" << "\n" << endl ;
	
	logfile1 << " Frequency :" << freq_init << " MHz  and the bandwidth is " << bandwidth << " MHz " << endl ;
       
       logfile1 << "Tabulation for beamwidths, row belongs to same zenith angle while column belongs to same hrangle" <<endl;
       
       logfile1 << "\t" << "\t" 
                << "hrangle = 120" << "\t" << "\t" 
                << "hrangle =150 "<< "\t" << "\t" 
                << "hrangle = 180" << "\t" << "\t" 
		<< "hrangle =210" << "\t" << "\t" 
		<< "hrangle = 240 " << endl ;
       
// 	 Double source_declination = 2 ;   
// 	 Double source_hr_angle = 120 ;
// 	 Double zenith_width = stbm.beamwidth_decli( source_declination,
//                                                            source_hr_angle,
// 	                                                   station_radii,
// 	                                                   station_id,
// 	                                                   freq_init,
// 		                                           bandwidth,
//                                                            freq_interval,
// 		                                           position_x,
// 		                                           position_y,
//                                                            legendre_root,
//              	                                           legendre_weight )    ;
// 					   
//          Double hr_width = stbm.beamwidth_hr( source_declination,
//                                                    source_hr_angle,
// 	                                           station_radii,
// 	                                           station_id,
// 	                                           freq_init,
// 		                                   bandwidth,
//                                                    freq_interval,
// 		                                   position_x,
// 		                                   position_y,
//                                                    legendre_root,
//              	                                   legendre_weight )   ;
//              logfile1.precision(3);
// 	     logfile1 << " If the source is at zemith then the beamwidth is : " << "\t" ;
//              logfile1 << "(" << zenith_width << ", " <<  hr_width << ")" << endl; ;
	 
       for( uint z=0; z<5 ; z++ ) {
       
           Double source_declination = 15 + z*15. ;   
	   // loop starts with minimum zenith angle of 15 degree and will increase by 15 degree in every loop
	   logfile1 << "\n" << "z_angle = " << source_declination << "\t" ;
	     
	   for( uint h=0; h<5; h++ ) {
	   
	       Double source_hr_angle = 120 +h*30 ;
	       //loop starts with minimum hr angle of 30 degree and will incease by 30 degree in every loop
	       
	       Double zenith_width = stbm.beamwidth_decli( source_declination,
                                                           source_hr_angle,
	                                                   station_radii,
	                                                   station_id,
	                                                   freq_init,
		                                           bandwidth,
                                                           freq_interval,
		                                           position_x,
		                                           position_y,
                                                           legendre_root,
             	                                           legendre_weight )    ;
					   
              Double hr_width = stbm.beamwidth_hr( source_declination,
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
             logfile1.precision(2);
             logfile1 << "(" << zenith_width << ", " <<  hr_width << ")" << "\t"  ;
					   
         }
	 logfile1 << "\n" ;
       }
       
       logfile1 << endl;
       
       uint freq = int(freq_init) ;
       
       Double Tsys(0.0) ;
       Double Aeff(0.0);
       Double freq_ratio(0.0);
       Double power_factor(0.0);
       Double Tsky(0.0);
       
       switch(freq)
         {
	  case 120000000:
	      
	      Tsys = 600. ;
	      Aeff = 24.*25. ; //effective area of 24 tiles 
	      freq_ratio = 12./15. ;
	      power_factor = pow(freq_ratio, -2.6) ;
	      Tsky = 200.*power_factor ; //calculated as 200*(freq/150)^(-2.6)
	      break ;
	      
	  case 150000000:
	      
	      Tsys = 400. ;
	      Aeff = 24.*20. ; //effective area of 24 tiles 
	      freq_ratio = 15./15. ;
	      power_factor = pow(freq_ratio, -2.6) ;
	      Tsky = 200.*power_factor ; //calculated as 200*(freq/150)^(-2.6)
	      break ;
	      
         case 180000000:
	      
	      Tsys = 350. ;
	      Aeff = 24.*16. ; //effective area of 24 tiles 
	      freq_ratio = 18./15. ;
	      power_factor = pow(freq_ratio, -2.6) ;
	      Tsky = 200*power_factor ; //calculated as 200*(freq/150)^(-2.6)
	      break ;
	      
        }
	
	logfile1 << " Area effective = " << Aeff << " m sq " << "\n"<< " System Temp = " << Tsys << " K " << "\n "<< " Sky Temp = " <<  Tsky << " K " << endl ;
	
	Double stations_sq = (n_stations-1)*n_stations ;
	
	Double station_factor = sqrt(stations_sq);
		
	Double noise_level = (Tsys+Tsky)*(2.76e3)/(Aeff*station_factor) ;
	
        logfile1 << "\n" << "Noise flux density : " << noise_level << " Jy " << endl;
	
	Double trigger_level = 49.*noise_level ;
	logfile1 << "\n" << " If triggering is performed at 7 sigma then Trigger level = "<< trigger_level << " Jy" << endl ;
	
	Double frequency = freq_init/1e9 ;
	
	Double frequency_ratio = frequency/2.5 ;
	
	Double power_fac = pow(frequency_ratio, 1.44 ) ;
		
	Double freq_dependence = square(frequency_ratio/(1+power_fac)) ;
		
	Double p_energy = sqrt((100e6/bandwidth)*(trigger_level/3.9e4)*(1./freq_dependence)) ;
		
	Double primary_energy = (1e20)*p_energy ;
	
	logfile1 << " We will trigger at energy : " << primary_energy << " eV " << endl ;
	
	Double gaussian_prob = exp(-49./2.) ;
	
	Double sampling_freq = 200e6 ;
	
	Double trigger_count = gaussian_prob*sampling_freq*n_stations ;
	
	logfile1 << " We will trigger : " << trigger_count << " every sec" << " or " << trigger_count*60. << " every minute " << endl ;
	
	logfile1 << "\n" << "\n" << " But if we trigger on 8 sigma level "  << endl ;
	
	trigger_level = 64.*noise_level ;
	
	logfile1 << " then Trigger level = "<< trigger_level <<  " Jy" << endl ;
	
	frequency = freq_init/1e9 ;
	
	frequency_ratio = frequency/2.5 ;
	
	power_fac = pow(frequency_ratio, 1.44 ) ;
	
	freq_dependence = square(frequency_ratio/(1+power_fac)) ;
		
	p_energy = sqrt((100e6/bandwidth)*(trigger_level/3.9e4)*(1./freq_dependence)) ;
	
	primary_energy = (1e20)*p_energy ;
	logfile1 << " We will trigger : " << primary_energy <<  " eV " << endl ;
	gaussian_prob = exp(-64./2.) ;
	
	sampling_freq = 200e6 ;
	
	trigger_count = gaussian_prob*sampling_freq ;
	
	logfile1 << " Then we will trigger " << trigger_count << " every sec" << " or " << trigger_count*60. << " every minute " 
	        << " or " << trigger_count*60.*60.*10. << " every 10 hr " << endl ;
	logfile1.close() ;
     
    }
     
    catch( AipsError x ){
    cerr << "StationBeam::generate_table_halfwidth" << x.getMesg () << endl ;
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

		  hr_inner_sum = hr_inner_sum + legendre_weight(hh)*(sum_freq)*sin(pi/180.*declination) ;
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


 void StationBeam::plot_2dimen(  const Double& source_declination,
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
        SimplePlot smplt ;

	Double pi = 3.1416 ;

        Double R = 5 ;
        Double y_rho =0.0 ;

        Double x_range_min = -0.5 ;
        Double x_range_max = 0.5 ;
        Double x_range_interval = 0.001 ;
        uint x_loop =int((x_range_max-x_range_min)/x_range_interval)+1 ;
       
        Vector<Double> x_direction( x_loop, 0.0 ) ;

        Double y_range_min = -0.5 ;
        Double y_range_max = 0.5 ;
        Double y_range_interval = 0.001 ;
        
	uint y_loop = int((y_range_max- y_range_min)/y_range_interval)+1 ;
        
	Vector<Double> y_direction( y_loop, 0.0 ) ;
        Double zenith(0.0);

        Matrix<Double> sum_freq( x_loop, y_loop, 0.0 );
        Double summed_freq(0.0);

        for( uint x=0; x< x_loop; x++ ) {

	    Double x_range = x_range_min + x*x_range_interval ;
            
            x_direction(x) = x_range ;

            for( uint y=0; y< y_loop; y++ ) {

		Double y_range = y_range_min +y*y_range_interval ;
		//  cout << " y loop " << y << endl;
                y_direction(y) = y_range ;

                Double rho = sqrt(x_range*x_range +y_range*y_range ) ;
                
                Double c = asin(rho/R) ; 

                if ((y_range==0.0) && (rho == 0.0)) {
                    
		  y_range = 0.00000001 ;
		  rho = 0.00000001 ;
		  }

        Double arguement = x_range*sin(c)/(rho*cos(pi/180.*source_hr_angle)*cos(c)-y_range*sin(pi/180.*source_hr_angle)*sin(c)) ;
		
	Double hr_angle = 180.-(180./pi)*asin(cos(c)*sin(pi/180.*source_hr_angle)+y_range/rho*sin(c)*cos(pi/180.*source_hr_angle)) ;
		
	        Double zenith = source_declination +(180./pi)*atan(arguement) ;
        
	        Double del_hr_angle = (source_hr_angle-hr_angle)/sin(pi/180*zenith) ;
		
		Double scaled_hr_angle = source_hr_angle - del_hr_angle ;
	
                summed_freq = stbm.integrate_freq( scaled_hr_angle,
                                                   zenith,
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
/*        
	       Double del_hr = (source_hr_angle - hr_angle)/sin(pi/180.*zenith) ;
	       Double del_zenith = (source_declination - zenith );
	       Double del_alpha = sqrt(del_hr*del_hr +del_zenith*del_zenith) ;
	       
	       if (del_alpha<1.00 ) 
	       
	       summed_freq = 1.00 ;
	       
	       else
	       
	       summed_freq = 0.00 ;*/
	       
	       sum_freq(x,y)= summed_freq ;

	    }

	}
	Int axis ;
	Int just ;
	Int col ;
	Int cheight ;
	Int linewidth ;
	
	smplt.InitPlot( "beam_sky.ps", x_range_min, x_range_max, y_range_min, y_range_max, axis=0, just=1, col=1,
		         cheight=1, linewidth=1);

        smplt.quick2Dplot("beam_sky.ps", sum_freq, x_range_min, x_range_max, y_range_min, y_range_max,	     
			  "X-axis", "Y-Axis", "2d plotting-test",False, 30);
	smplt.addContourLines(sum_freq, x_range_min, x_range_max, y_range_min, y_range_max, 3);     
    }
      
    catch( AipsError x ){
    cerr << "StationBeam::plot_2dimen " << x.getMesg () << endl ;
   }       
	       
 }


} // Namespace CR -- end

#endif
