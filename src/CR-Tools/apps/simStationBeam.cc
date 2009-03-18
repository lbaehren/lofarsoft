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

#include <Display/SimplePlot.h>
#include <Analysis/StationBeam.h>

using CR::StationBeam;
using CR::SimplePlot;
/*!
  \file tStationBeam.cc

  \ingroup CR_Applications

  \brief A collection of test routines for StationBeam
 
  \author Kalpana Singh
 
  \date 2007/08/10
*/

// Dynamic variables -----------------------------

Double source_declination (1);
Double source_hr_angle (120);

Double freq_init =120e6 ; 
Double bandwidth = 30e6 ;
Double freq_interval = 10e6 ;

uint stationid[48]= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48} ;
uint n_stations = 48 ;

// Static variables -------------------------------

Double station_radii = 30. ;

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
//    SimplePlot smplt ;

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

 StationBeam stbm ;
//  SimplePlot smplt ;

// ofstream logfile1 ;
//      
// logfile1.open("power",ios::out);

 Double zenith_min = source_declination -1.0 ;
 Double zenith_max = source_declination + 1.0 ;

 Double zenith_interval = 0.05 ;

 uint zenith_loop = int((zenith_max-zenith_min)/zenith_interval) ;
 Vector<Double> zenithAngle(zenith_loop, 0.0);

 Double hrangle_min = source_hr_angle -5.0 ;
 Double hrangle_max = source_hr_angle +5.0  ;
 
 Double hrangle_interval = 0.1 ;

 uint hrangle_loop =int((hrangle_max - hrangle_min)/hrangle_interval) ;

 Vector<Double> hrAngle(hrangle_loop, 0.0);

 Matrix<Double> single_stat_beam( hrangle_loop, zenith_loop,0.0 );
 Matrix<Double> tied_array_beam( hrangle_loop,zenith_loop,0.0 );
 Matrix<Double> integrate_freq( hrangle_loop+1, zenith_loop+1,0.0 ) ;
 
/*
 stbm.plot_2dimen( source_declination,
	           source_hr_angle,
	           station_radii,
	           station_id,
		   freq_init,
		   bandwidth,
                   freq_interval,
		   posit_x,
		   posit_y,
                   legendre_root,
                   legendre_weight )  ;
		   */
    			   

     stbm.generate_statistics_table( station_radii,
	                            station_id,
	                            freq_init,
		                    bandwidth,
                                    freq_interval,
		                    posit_x,
		                    posit_y,
                                    legendre_root,
             	                    legendre_weight )   ;



/*
Double beamwidth_hrangle = stbm.beamwidth_hr(  source_declination,
                                                source_hr_angle,
	                                        station_radii,
	                                        station_id,
	                                        freq_init,
		                                bandwidth,
                                                freq_interval,
		                                posit_x,
		                                posit_y,
                                                legendre_root,
             	                                legendre_weight )   ;
												
					
Double beamwidth_decli = stbm.beamwidth_decli(  source_declination,
                                                source_hr_angle,
	                                        station_radii,
	                                        station_id,
	                                        freq_init,
		                                bandwidth,
                                                freq_interval,
		                                posit_x,
		                                posit_y,
                                                legendre_root,
             	                                legendre_weight )   ;
						*/

// Double power_sky = stbm.integrate_decli( source_declination,
// 	                                  source_hr_angle,
// 	                                 station_radii,
//                                            station_id,
//                                           freq_init,
// 		                           bandwidth,
//                                            freq_interval,
// 		                           position_x,
// 		                           position_y,
//                                            legendre_root,
// 					 legendre_weight )   ;
// 
//  cout << " power integrated over sky : " << power_sky << endl;

/*
Double min_power = stbm.min_power_moon( source_declination,
                                        source_hr_angle,
                                        station_radii,
                                        station_id,
                                        freq_init,
                                        bandwidth,
                                        freq_interval,
                                        position_x,
                                        position_y,
                                        legendre_root,
                                        legendre_weight );

cout << " min power of moon " << min_power << endl ;*/

//  ofstream logfile1 ;
     
//  logfile1.open("power",ios::out);
   
//  //  logfile1 << powermoon << endl;
//    logfile1 << power_sky << endl ;
//    logfile1 << min_power << endl;
//    logfile1.close() ;
       
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
