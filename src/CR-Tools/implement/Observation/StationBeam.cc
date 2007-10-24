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
	 
cos_z_f_term = cos(pi/180.*source_hr_angle)*sin(pi/180.*source_declination)*cos(pi/180.*hr_angle)*sin(pi/180.*declination);
cos_z_s_term = sin(pi/180.*source_hr_angle)*sin(pi/180.*source_declination)*sin(pi/180.*hr_angle)*sin(pi/180.*declination) ;
cos_z_t_term = cos(pi/180.*source_declination)*cos(pi/180.*declination) ;

        cos_zenith = cos_z_f_term + cos_z_s_term + cos_z_t_term ;
	
	new_zenith = acos(cos_zenith) ;
	
	if (declination > source_declination)
	   new_azimuth = abs(hr_angle-source_hr_angle) ;
	   else 
	   	  if(declination < source_declination)
		              new_azimuth = 180 -abs(source_hr_angle-hr_angle) ;
			                if ((declination < source_declination) && (hr_angle >180))
						   new_azimuth = 180 + (source_hr_angle - hr_angle) ;
         
	  L_cosine = sin(new_zenith)*cos(pi/180.*new_azimuth) ;
	  
	  M_cosine = sin(new_zenith)*sin(pi/180.*new_azimuth) ;
	  
        for( uint r =0; r <n_roots; r++ ){
	
	     DComplex power_x (0.0) ;
	     Double yy(0.0);
	     
	     xx = h1*legendre_root(r)+h2 ;
	    
	     d1 = station_radii*cos(pi/180.*source_declination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
	     c1 = -station_radii*cos(pi/180.*source_declination)*sqrt(1.-(xx/station_radii)*(xx/station_radii)) ;
	     
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
	Double mul_sum(0.0) ;
	Double position_new_x(0.0);
	Double position_new_y(0.0);
	Double position_new_y1(0.0);
	Double position_new_y2(0.0);
	
	observed_L = sin(pi/180.*(declination-source_declination))*cos(pi/180.*(hr_angle-source_hr_angle)) ;
	
	observed_M = sin(pi/180.*(declination-source_declination))*sin(pi/180.*(hr_angle-source_hr_angle)) ;
		
	observed_N = cos(pi/180.*(declination-source_declination)) ;
		
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
	     
	     position_new_x = pos_new_x*cos(pi/180.*source_hr_angle)-pos_new_y*sin(pi/180.*source_hr_angle) ;
	     position_new_y1 = pos_new_x*cos(pi/180.*source_declination)*sin(pi/180.*source_hr_angle) ;
	     position_new_y2 = pos_new_y*cos(pi/180.*source_hr_angle)*cos(pi/180.*source_declination) ;
	     
	     position_new_y = position_new_y1 +position_new_y2 ;
             
	    sum_tied_array = sum_tied_array +stationbeam*exp(2*pi*j*frequency/3e8*(position_new_x*observed_L+position_new_y*observed_M));
	  }
       
       integral_value = (abs(sum_tied_array)*abs(sum_tied_array));	     	     
													
       return integral_value ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::tied_array " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }
 
 
  Double StationBeam::integrate_moon(  const Double& source_declination,
		                       const Double& source_hr_angle,
		                       const Double& station_radii,
		                       const Vector<uint>& station_id,
		                       const Double& freq_init,
		                       const Double& bandwidth,
		                       Vector<Double>& position_x,
		                       Vector<Double>& position_y,
		                       const Vector<Double>& legendre_root,
		                       const Vector<Double>& legendre_weight )   
 
{
   try {
         StationBeam stbm ;
	 
	 uint nroots = legendre_root.nelements() ;
	 
	 Double f1m (0.0) ;
	 Double f2m (0.0) ;
	 Double f1_fm(0.0) ;
	 Double f2_fm(0.0);
	 Double ffm(0.0);
	 Double freq_final(0.0);
	 f1m = freq_init ;
	 freq_final = freq_init +bandwidth ;
	 	 
	 Double h1m(0.0) ;
	 Double h2m(0.0) ;
	 Double h1_xm (0.0);
	 Double h2_xm (0.0);
	 Double xxm(0.0);
	 h1m = source_declination-0.25 ;
	 	 	 
	 Double k1m(0.0) ;
	 Double k2m(0.0) ;
	 Double k1_ym(0.0);
	 Double k2_ym(0.0);
	 Double yym(0.0);
	 
	 Double fm_outer_sum(0.0) ;
	 Double fm_inner_sum(0.0) ;
	 Double xm_outer_sum(0.0) ;
	 Double xm_inner_sum(0.0) ;
	 Double ym_outer_sum(0.0) ;
	 Double ym_inner_sum(0.0) ;     
	 
	 Double power_moon(0.0);
	
	 while( h1m < (source_declination+0.25)){
		  
		h2m= h1m+0.05 ;
		h1_xm =(h2m-h1m)/2. ;
		h2_xm =(h2m+h1m)/2. ;
		xm_inner_sum =0;
		ym_outer_sum = 0.0 ;
		       
		for( uint im=0; im<nroots; im++){
		       
		      xxm = h1_xm*legendre_root(im)+h2_xm ;
		      
        k2m = source_hr_angle +1/sin(0.0174533*xxm)*sqrt(0.25*0.25-(abs(xxm-source_declination))*(abs(xxm-source_declination)));
	k1m = source_hr_angle -1/sin(0.0174533*xxm)*sqrt(0.25*0.25-(abs(xxm-source_declination))*(abs(xxm-source_declination)));
			   
		      k1_ym = (k2m-k1m)/2. ;
		      k2_ym = (k2m+k1m)/2. ;
		      
                      ym_inner_sum = 0.0 ;
		      
		      for( uint jm =0; jm <nroots; jm++){
		      
		           fm_outer_sum =0.0;
			   yym = k1_ym* legendre_root(jm)+ k2_ym ;
			   
			   while(f1m < freq_final ){
		               
			        f2m = f1m+1e6 ;
			        f1_fm = (f2m-f1m)/2. ;	      
			        f2_fm = (f2m+f1m)/2. ;
			        fm_inner_sum =0.0;
	       
	                       for(uint gm=0; gm<nroots; gm++ ){
	      
	                           ffm = f1_fm*legendre_root(gm)+f2_fm ;
		             	 
				   power_moon = stbm.tied_array( ffm,
	                                                         yym,
		                                                 xxm,
		                                                 source_declination,
		                                                 source_hr_angle,
		                                                 station_radii,
				                                 station_id,
		                                                 position_x,
		                                                 position_y,
		                                                 legendre_root,
		                                                 legendre_weight ) ;
							     
                                 fm_inner_sum = fm_inner_sum + legendre_weight(gm)*power_moon ;
			       }
			    
			   fm_outer_sum = fm_outer_sum + f1_fm*fm_inner_sum ;	
			   f1m = f2m ;
			   }
		       
			f1m = freq_init ;
			ym_inner_sum = ym_inner_sum + legendre_weight(jm)*fm_outer_sum ;
		      }
		      
		      ym_outer_sum = ym_outer_sum + k1_ym*ym_inner_sum ;
		      xm_inner_sum = xm_inner_sum + legendre_weight(im)*ym_outer_sum ;
          }
	  xm_outer_sum = xm_outer_sum + h1_xm*xm_inner_sum ;
	  h1m = h2m ;
	}
			    
	power_moon = xm_outer_sum ;       
	
	return power_moon ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::integrate_moon " << x.getMesg () << endl ;
      return Double ();
      }       
	       
 }
 
 
 		    		    
Double StationBeam::integrate_sky(  const Double& source_declination,
		                    const Double& source_hr_angle,
		                    const Double& station_radii,
		                    const Vector<uint>& station_id,
		                    const Double& freq_init,
		                    const Double& bandwidth,
		                    Vector<Double>& position_x,
		                    Vector<Double>& position_y,
		                    const Vector<Double>& legendre_root,
		                    const Vector<Double>& legendre_weight )      

 {
   try {
         StationBeam stbm ;
	 
	 Double power_beam(0.0);
        	 	 
	 Double h1(0.0) ;
	 Double h2(0.0) ;
	 Double h1_x(0.0);
	 Double h2_x(0.0);
	 Double xx(0.0);
	 //h1 = source_declination-0.25 ;
	 	 	 
	 Double k1(0.0) ;
	 Double k2(0.0) ;
	 Double k1_y(0.0);
	 Double k2_y(0.0);
	 Double yy(0.0);
	 
	 Double f_outer_sum(0.0) ;
	 Double f_inner_sum(0.0);
	 Double x_outer_sum(0.0) ;
	 Double x_inner_sum(0.0);
	 Double y_inner_sum(0.0);
	 Double y_outer_sum(0.0);
	 
	 Double f1(0.0) ;
	 Double f2(0.0) ;
	 Double f1_f(0.0) ;
	 Double f2_f(0.0);
	 Double ff(0.0);
	 Double freq_final(0.0);
	 f1 = freq_init ;
	 freq_final = freq_init +bandwidth ;
	
	 
	 Double declination_min(0.0) ;
	 Double declination_max(90.0) ;
	 Double declination_interval(0.1);
	 	 
	 Double hrangle_min(0.0) ;
	 Double hrangle_max(360.0) ;
	 Double hrangle_interval(1.0);
	 	 
	 Double power_sky(0.0);	 
	 
	 
	 uint nroots = legendre_root.nelements() ;
	 
	 ofstream logfile1 ;
	  
	 logfile1.open("powersky", ios::out) ;
	 
	 while(h1< declination_max){
	   
	   h2 = h1 + 0.25 ;
	   cout << "declination angle :" <<h2 << endl ;
	   h1_x = (h2-h1)/2. ;
	   h2_x = (h2+h1)/2. ;
	   x_inner_sum =0.0 ;
	   
	   for( uint i =0; i<nroots; i++){
	     
	     y_outer_sum =0.0 ;
	     xx = h1_x*legendre_root(i)+h2_x ;
	     
	     while( k1 < hrangle_max){
	       
	       k2 = k1+ 5.0 ;
	       k1_y = (k2-k1)/2.0 ;
	       k2_y = (k2+k1)/2.0 ;
	       y_inner_sum = 0.0 ;
	       
	       for( uint j=0; j<nroots; j++ ){
		 
		 f_outer_sum =0.0 ;
		 yy = k1_y*legendre_root(j)+ k2_y ;
		 
		 while(f1 < freq_final ){
		   
		   f2 = f1+1e6 ;
		   f1_f = (f2-f1)/2.;
		   f2_f = (f2+f1)/2. ;
		   f_inner_sum =0.0;
		   
		   for( uint g=0; g<nroots; g++){
		     
		     ff = f1_f*legendre_root(g)+f2_f ;
		     power_beam = stbm.tied_array( ff,
						   yy,
						   xx,
						   source_declination,
						   source_hr_angle,
						   station_radii,
						   station_id,
						   position_x,
						   position_y,
						   legendre_root,
						   legendre_weight ) ;
		     
		     f_inner_sum = f_inner_sum + legendre_weight(g)*power_beam ;
		   }
		   f_outer_sum = f_outer_sum + f1_f*f_inner_sum ;
		   f1 = f2 ;
		   logfile1 << h1 << "\t" << k1 << "\t" << f_outer_sum << "\n" ;
		 }
		 f1 = freq_init ;
		 y_inner_sum = y_inner_sum + legendre_weight(j)*f_outer_sum ;
	       }
	       y_outer_sum = y_outer_sum + k1_y*y_inner_sum ;
	       k1 = k2 ;	      
	       logfile1 << "\n" ;
	     }
	     
	     k1 = hrangle_min ;
	     x_inner_sum = x_inner_sum + legendre_weight(i)*y_outer_sum ;
	   }
	   
	   x_outer_sum = x_outer_sum + h1_x*x_inner_sum ;
	   h1 = h2 ;
	   
	 }			   
	 power_sky = power_sky + f_outer_sum ;      
	 logfile1.close() ;
	 
	 return power_sky ;
   }
   
   catch( AipsError x ){
     cerr << "StationBeam::integrate_sky " << x.getMesg () << endl ;
     return Double ();
   }       
   
 }			    
  
/*
		    		     		     		     
 Double StationBeam::temp_final(  const Double& source_declination,
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
		                  const Vector<Double>& legendre_weight  )   
{
 try{
     StationBeam stbm ;
     
     Double pi = 3.1416 ;
     
     Double power_sky = stbm.integrate_sky( source_declination,
		                            source_hr_angle,
		                            station_radii,
		                            station_id,
		                            freq_init,
		                            bandwidth,
		                            position_x,
		                            position_y,
		                            legendre_root,
		                            legendre_weight )   ;
 	
     Double power_moon = stbm.integrate_moon( source_declination,
		                       source_hr_angle,
		                       station_radii,
		                       station_id,
		                       freq_init,
		                       bandwidth,
		                       position_x,
		                       position_y,
		                       legendre_root,
		                       legendre_weight ) ;
    
    				       			       					    			 
    return power_moon ;
      }
      
      catch( AipsError x ){
      cerr << "StationBeam::temp_final " << x.getMesg () << endl ;
      return Double ();
      }       
}
 */     
 
	         
    

} // Namespace CR -- end
