 /*-------------------------------------------------------------------------*
 | $Id:: tcasa_measures.cc 1901 2008-08-20 13:47:09Z baehren             $ |
 *-------------------------------------------------------------------------*
/***************************************************************************
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

#include <crtools.h>

/* casacore*/
#include <scimath/Mathematics/FFTServer.h>
/* CT-Tools*/
#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Data/rawSubbandIn.h>
#include <Data/tbbctlIn.h>
/* DAL */
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>
/*!
  \file tppfinversion.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfinversion
 
  \author Kalpana Singh
 
  \date 2007/06/01
*/
using CR::ppfinversion ;
using CR::ppfimplement ;
using CR::tbbctlIn ;
using CR::rawSubbandIn ;

using namespace std;
using DAL::BeamFormed ;
using DAL::BeamGroup ;

const uint dataBlockSize (1024);
const uint nofsegmentation(16*6) ;
Vector<Double> samples( dataBlockSize*nofsegmentation, 0.0 ) ;
 
// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfinversion object

  \return nofFailedTests -- The number of failed tests.
*/

 
void show_data ( Matrix<Double> const &data )
 {
//   std::cout << " Data Shape : " << data.shape() << std::endl ;
//   std::cout << " Data ......: " 
//             << data(1,1) << ""
// 	    << data(2,1) << ""
// 	    << data(3,1)
// 	    <<std::endl ;
	    
 }

int test_ppfinversion ()
{
  cout << "\n[test_ppfinversion]\n" << endl;
  
  int nofFailedTests = 0;
  
  cout << "[1] Testing default constructor ..." << endl;
  
  try {
    
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;
    
    
    Vector<String> filenames( 2 );
    {
      filenames(0)="/mnt/lofar/ppfinversion/TBB1.cor.h5";
      filenames(1)="/mnt/lofar/ppfinversion/rw_20080604_121347_2300.dat.h5"; 
    }
    cout << "[1] Testing attaching file...." << endl ;
    
    //"making object of class BeamFormed "
    
    DAL::BeamFormed bformed (filenames(0));
    
    DAL::BeamGroup bgroup() ;
    
    cout << "Filename = " << bformed.filename() << endl;
    
    //if(!bformed.)
    
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ----------------------------------------------------------- test_ppfinversions

int test_ppfinversions (std::string const &filename)
{
  int nofFailedTests (0);
  
  try {
    Vector<Double> ppfcoeff(16384,0.0) ;
    readAsciiVector( ppfcoeff,data_ppf_coefficients.c_str()) ;
    Vector<Double> ppfcoeff_inv(16384, 0.0) ;
    readAsciiVector( ppfcoeff_inv,data_ppf_inversion.c_str()) ;
    
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;     
    
    cout << "[1] Testing attaching file...." << endl ;
    
    //"making object of class BeamFormed "
    
    DAL::BeamFormed bformed (filename);
    
    DAL::BeamGroup bgroup() ;
    
    cout << "Filename = " << bformed.filename() << endl;
    
    cout << "objects are build :" << endl ;
    
  } catch (AipsError x) {
    cerr << x.getMesg() << endl ;
    nofFailedTests++;
  }

  return nofFailedTests;
}   

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  std::string filename;

  /* Check parameter provided from the command line */
  if ( argc < 2 ) {
    cout << "[tppfinversion] Missing name of input data file!" << endl;
    return 1;
  } else {
    filename = std::string (argv[1]);
  }

  /* Run the tests */

  nofFailedTests += test_ppfinversions (filename);

  return nofFailedTests;
}
