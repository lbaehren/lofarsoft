/*-------------------------------------------------------------------------*
 | $Id: template-tclass.cc,v 1.9 2007/06/13 09:41:37 bahren Exp $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                                     *
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

Double source_declination = 30 ;
Double source_hr_angle = 30 ;
uint stationid[64]= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64};


Double bandwidth = 30e6 ;
Double station_radii = 15. ;
Vector<uint> station_id(64,0);
Vector<Double> posit_x(64,0.0) ;
Vector<Double> posit_y(64,0.0) ;
Vector<Double> legendre_root(5,0.0);
Vector<Double> legendre_weight(5,0.0);

Double posi_x[64] = {120.,  270.,   -91.,    59.,  -171.,   -21.,  -95.,    55.,   20.,   170.,   62.,   212.,   -29.,   121.,   -51.,      99.,    97.,   247.,   -29.,   121.,   226.,   376.,   126.,   276.,  -189.,   -39.,  -246.,   -96.,   217., 367.,   291.,   441.,   499.,   649.,   521.,   671.,    15.,   165.,  -354.,  -204.,  -462.,  -312.,  -322.,  -172.,  -284.,  -134.,   403.,   553.,   683.,   833.,   821.,   971.,   712.,   862.,   374.,   524.,  -172.,   -22., -555.,  -405.,  -793.,  -643.,  -634.,  -484.  };
Double posi_y[64] = { -597.,  -597.,  -50.,   -50.,  -198.,  -198.,  -148.,  -148.,  -140.,  -140.,  -70.,   -70.,  -246.,  -246.,  -314.,  -314.,  -236.,  -236.,  -412.,  -412.,  -214.,  -214.,   259.,   259.,   153.,   153.,  -108.,  -108.,  -710., -710.,  -412.,  -412.,   -60.,   -60.,   270.,   270.,   514.,   514.,   500.,   500.,   104.,   104.,  -241.,  -241.,  -892.,  -892.,  -962.,  -962.,  -525.,  -525.,    44.,    44.,   640.,   640.,   921.,   921.,   817.,   817., 787.,   787.,   123.,   123.,  -480.,  -480.  };

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
 
 
 for(uint p=0; p<64; p++){
     posit_x(p) = posi_x[p] ;
     }
     
 for(uint w=0; w<64; w++){
   posit_y(w) = posi_y[w];
   }
 
 for(uint lr=0; lr<5; lr++){
    legendre_root(lr)= l_root[ lr ];
    }
    
  for(uint lw=0; lw <5; lw++ ){
     legendre_weight(lw)= l_weight[lw] ;
    }
 
for(uint stid=0; stid<64; stid++){
 
    station_id(stid)=stationid[stid];
    }
  
  Double frequency = 120e6 ;
  Double sky_temp = 300 ;
  Double moon_temp = 300 ;
  Double freq_init =120e6 ;
  StationBeam stbm ;
 
/*  ofstream logfile1 ;
   
  logfile1.open("power",ios::out);

  for (uint i=0; i<90; i++){
      
      for( uint j=0; j<360; j++){*/
       
//         Double sing_sta_beam = stbm.station_beam( frequency,
// 				                  j,
// 			       	                  i,
// 				                  source_declination,
//       	       		                  source_hr_angle,
// 			                          station_radii,
// 			                          legendre_root,
// 			                          legendre_weight )  ;
// 	
      						  					  
/*  Double beam_value = stbm.tied_array( frequency,
	                               j,
		                       i,
				       source_declination,
				       source_hr_angle,
				       station_radii,
				       station_id,
				       posit_x,
				       posit_y,
				       legendre_root,
				       legendre_weight ) ;

				       
       logfile1 << j << "\t" << i << "\t" << beam_value << "\n" ;
      }
      logfile1 << "\n" ;
   }
   
  logfile1.close();*/
   
				       
 Double beam_moon = stbm.integrate_moon( source_declination,
		                         source_hr_angle,
		                         station_radii,
		                         station_id,
		                         freq_init,
		                         bandwidth,
		                         posit_x,
		                         posit_y,
		                         legendre_root,
		                         legendre_weight ) ;

cout << " integrated power of moon :" << beam_moon << endl ;					      			

Double beam_sky = stbm.integrate_sky( source_declination,
		                      source_hr_angle,
		                      station_radii,
		                      station_id,
		                      freq_init,
		                      bandwidth,
		                      posit_x,
		                      posit_y,
		                      legendre_root,
		                      legendre_weight ) ;	
				     
 cout << " integrated power whole sky :" << beam_sky << endl ;				     			       
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
 