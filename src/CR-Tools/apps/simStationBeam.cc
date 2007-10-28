/*-------------------------------------------------------------------------*
 | $Id: template-tclass.cc,v 1.9 2007/06/13 09:41:37 bahren Exp          $ |
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

#include <fstream>
#include <iostream>

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/namespace.h>

#include <Observation/StationBeam.h>

using CR::StationBeam;

/*!
  \file tStationBeam.cc

  \ingroup Observation

  \brief A collection of test routines for StationBeam
 
  \author Kalpana Singh
 
  \date 2007/08/10
*/

Double source_declination (30);
Double source_hr_angle (30) ;
Double freq_init =120e6 ;
Double bandwidth = 30e6 ;
Double station_radii = 15. ;

uint n_stations = 2 ;

uint stationid[12]= { 29, 30, };

Vector<Double> position_x(n_stations,0.0);
Vector<Double> position_y(n_stations,0.0);

Vector<Double> posit_x(48,0.0) ;
Vector<Double> posit_y(48,0.0) ;

Double posi_x[48] = { -37.39, -47.53, -88.10, -74.43, 25.96, 161., 51., 175.61, -246.7, -155.48, 277.36, 310.75, -406.28, -281.68, 270.69, 304.08, -635.31, -544.09, -767.31, -733.92, -706.7, -615.48, 817.7, 941.61, -188.95, -64.35, 64.16, 10.13, -131.49, -89.96, 32.82, -16.47, 752.02, 843.24, 688.87, 722.08, -179.36, -88.14, -84.69, 6.52, 175.67, 300.27, -303.97, -270.59, -1023.62, -899.01, -297.31, -263.92 };
Double posi_y[48] = { -72.14, -117.04, -261.38, -217.43, -190.53, -114.0, 264.67, 231.28, 67.74, 158.96, -421.42, -546.03, -256.51, -289.9, -897.42, -1022.03, 696.92, 788.13, 196.59, 71.99, -527.59, -436.37, -393.99, -427.38, -1212.51, -1245.9, -138.09, -132.57, -148.34, -159.47, -255.05, -232.96, -4.42, 86.8, 718.58, 593.97, -645.59, -554.37, -484.92, -393.71, -263.33, -296.72, -68.76, -193.36, -282.51, -315.9, 1137.26, 1012.65 };

Vector<Double> legendre_root(5,0.0);
Vector<Double> legendre_weight(5,0.0);

Double l_root[5] = { 0.9064798459, 0.5384693101, 0.000000000, -0.5384693101, -0.9061798459 };

Double l_weight[5] = { 0.2369268850, 0.4786286705, 0.568888889, 0.4786286705, 0.236968850 };

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StationBeam object

  \return nofFailedTests -- The number of failed tests.
*/
int test_StationBeam ()
{
  int nofFailedTests (0);
   
  std::cout << "\n[test_StationBeam]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
   StationBeam stbm ;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

Bool test_StationBeams ()
{

 Bool ok(true) ;
 
 try{
 
 Vector<uint> station_id( n_stations,0);

 for(uint stid=0; stid< n_stations; stid++){
   
   station_id(stid)=stationid[stid];
 }
 
 for(uint p=0; p< 48; p++){
   
   posit_x(p) = posi_x[p] ;
 
   posit_y(p) = posi_y[p];
 }
 
uint stationID(0) ;

 for(uint id =0 ; id < n_stations; id++){

     stationID = station_id(id) ;
     position_x(id) = posit_x(stationID) ;
     position_y(id) = posit_y(stationID) ;
 }

     
 
for(uint lr=0; lr<5; lr++){
   legendre_root(lr)= l_root[ lr ];

   legendre_weight(lr)= l_weight[lr] ;
 }

 
 Double frequency = 120e6 ;
 Double sky_temp = 300 ;
 Double moon_temp = 300 ;

 StationBeam stbm ;
 
 ofstream logfile1 ;
     
 logfile1.open("power",ios::out);
 
 for (uint i=0; i<90; i++){
 
     for( uint j=0; j<360; j++){
 
         Double sing_sta_beam = stbm.station_beam( frequency,
  				                  j,
  			       	                  i,
  				                  source_declination,
        	       		                  source_hr_angle,
  			                          station_radii,
  			                          legendre_root,
  			                          legendre_weight )  ;
  	
 
  Double beam_value = stbm.tied_array( frequency,
     j,
     i,
     source_declination,
     source_hr_angle,
     station_radii,
     station_id,
     position_x,
     position_y,
     legendre_root,
     legendre_weight ) ;
     
    
     logfile1 << j << "\t" << i << "\t" << sing_sta_beam << "\n" ;
     }
     logfile1 << "\n" ;
     }
     
  logfile1.close();

/*

  Double beam_moon = stbm.beam_width(   source_declination,
	                               source_hr_angle,
	                               station_radii,
	                               station_id,
		                       freq_init,
		                       bandwidth,
		                       position_x,
		                       position_y,
                                       legendre_root,
             		               legendre_weight )   ;
*/
/* 
  Double beam_moon = stbm.integrate_moon( source_declination,
 		                         source_hr_angle,
 		                         station_radii,
 		                         station_id,
 		                         freq_init,
 		                         bandwidth,
 		                         position_x,
 		                         position_y,
 		                         legendre_root,
 		                         legendre_weight ) ;
 
  cout << " integrated power of  moon :" << beam_moon << endl ;					      			
 
  Double beam_sky_1 = stbm.integrate_sky( source_declination,
 				       source_hr_angle,
 				       station_radii,
 				       station_id,
 				       freq_init,
 				       bandwidth,
 				       position_x,
 				       position_y,
 				       legendre_root,
 				       legendre_weight ) ;	
 
  cout << " integrated power in one part of the  sky :" << beam_sky_1 << endl ;
*/
 
//  Double beam_sky_2 = stbm.integrate_sky_otherside( source_declination,
// 				       source_hr_angle,
// 				       station_radii,
// 				       station_id,
// 				       freq_init,
// 				       bandwidth,
// 				       position_x,
// 				       position_y,
// 				       legendre_root,
// 				       legendre_weight ) ;	
 
//  cout << " integrated power in other part of  sky :" << beam_sky_2 << endl ;	


 
//  Double beam_moonsky = stbm.integrate_moon_sky( source_declination,
// 				       source_hr_angle,
// 				       station_radii,
// 				       station_id,
// 				       freq_init,
// 				       bandwidth,
// 				       position_x,
// 				       position_y,
// 				       legendre_root,
// 				       legendre_weight ) ;	
 
//  cout << " integrated power over moon angle in  sky :" << beam_moonsky << endl ;		
		     			       	
		     			       
 //   ofstream logfile2 ;
 //  
 //  logfile2.open("power",ios::out);
 //  for(uint h=0; h<100; h++){
 //     logfile2 << input(h) << endl ;
 //     }
 //  
 //  logfile2.close();
 } catch( AipsError x ){
   cerr <<x.getMesg() << endl ;
   ok = False;
 }
 return ok ;
}
// -----------------------------------------------------------------------------

int main ()
{
  Bool ok (True) ;
  Int retval(0);
  
  if(ok){
    ok = test_StationBeams () ;
    
    if(!ok){
      retval = 1;
      cout << "Error ...........early exit "
	   <<endl ;
    }
  }
  return retval ;
}
