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

#include <Observation/StationBeam.h>
#include <casa/aips.h>
#include <fstream.h>
#include <iostream.h>

#include <casa/Arrays.h>
#include <casa/ArrayMath.h>

//using CR::StationBeam;

/*!
  \file tStationBeam.cc

  \ingroup Observation

  \brief A collection of test routines for StationBeam
 
  \author Kalpana Singh
 
  \date 2007/08/10
*/
Double pi = 3.1416 ;

Double Diameter = 200. ;

Double vel_light = 3e8 ;

Double freq_obs = 150e6 ;

Vector<Double> input( 100, 0.0 );

Vector<Double> input_inv( 100, 0.0 );

Vector<Double> output( 100, 0.0 );
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
 
 input(0)=-1.0;
 
  for( uint r=0; r<99; r++){
   input(r+1) = -1.+(r+1)*(2./100.);
  }
 
 ofstream logfile1 ;
 
 logfile1.open("input",ios::out);
 for(uint h=0; h<100; h++){
    logfile1 << input(h) << endl ;
    }
 
 logfile1.close();
 
 Double wave_length = vel_light/ freq_obs ;
 
 Double mult_factor = pi*Diameter/ wave_length ;
 
 for( uint s=0; s<100; s++ ){
 
      arguement = mult_factor*input(s);
      
      expression = (sin(arguement)/arguement-cos(arguement))/arguement ;
 
      output(s) = ( 2*expression/arguement )*(2*expression/arguement);
    }
  ofstream logfile2 ;
 
 logfile2.open("power",ios::out);
 for(uint h=0; h<100; h++){
    logfile2 << input(h) << endl ;
    }
 
 logfile2.close();
 }
 catch( AipsError x ){
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
 