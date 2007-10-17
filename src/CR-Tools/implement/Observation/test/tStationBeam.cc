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
uint stationid[4]= {2, 3, 4, 5,};

Double frequency = 120e6 ;
Double bandwidth = 30e6 ;
Double station_radii = 15. ;
Vector<uint> station_id(4,0);
Vector<Double> posit_x(8,0.0) ;
Vector<Double> posit_y(8,0.0) ;
Vector<Double> legendre_root(5,0.0);
Vector<Double> legendre_weight(5,0.0);

Double posi_x[8] = {28., 36., 57., 38., 25., 27., 48., 67. };
Double posi_y[8] = { 34., 54., 67., 90., 67., 78., 78., 89. };
Double l_root[5] = { 0.9062, 0.5385, 0., -0.5385, -0.9062 };
Double l_weight[5] = { 0.2369, 0.4786, 0.5689, 0.4786, 0.2369 };

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new StationBeam object

  \return nofFailedTests -- The number of failed tests.
*/
int test_StationBeam ()
{
  int nofFailedTests (0);
  
  StationBeam stbm ;
  
  std::cout << "\n[test_StationBeam]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
  //  StationBeam newObject;
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
 
 
 for(uint p=0; p<8; p++){
     posit_x(p) = posi_x[p] ;
     }
     
 for(uint w=0; w<8; w++){
   posit_y(w) = posi_y[w];
   }
 
 for(uint lr=0; lr<5; lr++){
    legendre_root(lr)= l_root[ lr ];
    }
    
  for(uint lw=0; lw <5; lw++ ){
     legendre_weight(lw)= l_weight[lw] ;
    }
 
 for(uint stid=0; stid<4; stid++){
 
    station_id(stid)=stationid[stid];
    }
//  ofstream logfile1 ;
//  
//  logfile1.open("input",ios::out);
//  for(uint h=0; h<100; h++){
//     logfile1 << input(h) << endl ;
//     }
//  
//  logfile1.close();
//  
  Double  hr_angle = 40 ;
  Double declination = 40 ;
  
  StationBeam stbm ;
  
  Double beam_value = stbm.tied_array( frequency,
	                               hr_angle,
		                       declination,
				       source_declination,
				       source_hr_angle,
				       station_radii,
				       station_id,
				       posit_x,
				       posit_y,
				       legendre_root,
				       legendre_weight ) ;
				       
cout << " beam_value " << beam_value << endl ;				       
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
 