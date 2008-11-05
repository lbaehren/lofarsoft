 /*-------------------------------------------------------------------------*
  | $Id:: tcasa_measures.cc 1901 2008-08-20 13:47:09Z baehren             $ |
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

#include <crtools.h>

#include <dal/dalCommon.h>
#include <dal/dalDataset.h>
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Data/rawSubbandIn.h>
#include <Data/tbbctlIn.h>

using CR::ppfinversion ;
using CR::ppfimplement ;
using CR::tbbctlIn ;
using CR::rawSubbandIn ;

using namespace std;

// -----------------------------------------------------------------------------

/*!
  \file tppfinversion.cc

  \ingroup Analysis

  \brief A collection of test routines for ppfinversion
 
  \author Kalpana Singh
 
  \date 2007/06/01

  <h3>Usage</h3>

  \verbatim
  tppfinversion <filename>
  \endverbatim

  - \e filename is a dataset containing beamformed data in HDF5 format.
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new ppfinversion object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_ppfinversion (std::string const &filename)
{
  cout << "\n[test_ppfinversion]\n" << endl;
  
  int nofFailedTests = 0;
  
  cout << "[1] Testing default constructor ..." << endl;
    try {
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;
    
  /*  
    Vector<String> filenames( 2 );
    {
      filenames(0)="/mnt/lofar/ppfinversion/TBB.corr.h5";
      filenames(1)="/mnt/lofar/ppfinversion/rw_20080604_121347_2300.dat.h5"; 
    }
    cout << "[1] Testing attaching file...." << endl ;
    */
    DAL::BeamFormed bf (filename );
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ----------------------------------------------------------- test_ppfinversions

int test_inversions (std::string const &filename)
{
  std::cout << "\n[tppfinversion::test_inversions]\n" << std::endl;

  int nofFailedTests (0);
  Vector<Double> ppfcoeff(16384,0.0) ;
  readAsciiVector( ppfcoeff,data_ppf_coefficients.c_str()) ;
  Vector<Double> ppfcoeff_inv(16384, 0.0) ;
  readAsciiVector( ppfcoeff_inv,data_ppf_inversion.c_str()) ;

  std::cout << "[1] Test creation of DAL::BeamFormed object ..." << std::endl;
  try {
    ppfinversion ppf_inv ;
    ppfimplement ppf_imp ;
    tbbctlIn newtbbctlIn ;
    rawSubbandIn newrawSubbandIn ;     
    
    DAL::BeamFormed bf (filename);
    bf.summary();
    
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
// std::string fileBeamformed;
// int test_ppfinversion (std::string const &filename) 
  std::string filename;
  if (argc > 1) {
    filename = std::string(argv[1]);
  } else {
    std::cerr << "Please provide a HDF5 filename.\n";
    return(DAL::FAIL);
  }
  
   nofFailedTests += test_ppfinversion(filename);

//   if (nofFailedTests == 0) {
//     nofFailedTests += test_attributes (filename);
//     nofFailedTests += test_getData (filename);
//   }
  /* Check parameter provided from the command line */
//   if ( argc < 2 ) {
//     std::cerr << "[tppfinversion] Missing name of input data file!" << endl;
//     std::cerr << endl;
//     std::cerr << "  tppfinversion <filename>" << endl;
//     std::cerr << endl;
//     return 1;
//   } else {
//     fileBeamformed = std::string (argv[1]);
//   }
  
  /* Run the tests */
  
//  nofFailedTests += test_inversions (fileBeamformed);
  
  return nofFailedTests;
}
