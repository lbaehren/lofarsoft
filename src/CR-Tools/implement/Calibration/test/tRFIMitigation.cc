/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
 *   maaijke mevius (<mail>)                                                     *
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

#include <Calibration/RFIMitigation.h>

// Namespace usage
using CR::RFIMitigation;

/*!
  \file tRFIMitigation.cc

  \ingroup Calibration

  \brief A collection of test routines for the RFIMitigation class
 
  \author maaijke mevius
 
  \date 2010/04/23
*/

//_______________________________________________________________________________
//                                                              test_constructors

/*!
  \brief Test constructors for a new RFIMitigation object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tRFIMitigation::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    RFIMitigation newObject;
    //
    newObject.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}


//_______________________________________________________________________________
//                                                              test_RFIMitigation

/*!
  \brief Test RFIMitigation

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_RFIMitigation ()
{
  std::cout << "\n[tRFIMitigation::test_RFIMitigation]\n" << std::endl;

  int nofFailedTests (0);
  
  try {
    uint blocksize=512;
    uint nr_samples=30;
    Vector<Double> spectrum(blocksize);
    double p[8];
    p[0]=10.;p[1]=-0.261917;p[2]=0.00349004;p[3]=-1.17348e-05;p[4]=-4.09638e-08;p[5]=3.53223e-10;p[6]=-7.86983e-13;p[7]=5.81604e-16;
    //fill with arbitrary data
    for(uint i=0;i<blocksize;i++)
      {
	double random= rand()/RAND_MAX+0.1;
	spectrum(i)=0;
	for(int poli=7;poli>=0;poli--)
	  {
	    spectrum(i)*=i;
	    spectrum(i)+=p[poli];
	  }
	spectrum(i)*=random;
	
	//add some RFI
	if(rand()<(0.02*RAND_MAX))
	  spectrum(i)+=100.*rand()/RAND_MAX;
	
      }
    double rmsThresholdValue = 100.;
    double flagThresholdValue = 5;
    Vector<Double> amplVec(nr_samples);
    Vector<Double> rmsVec(nr_samples);
    CR::RFIMitigation::doDownsampling(spectrum,
			nr_samples,
			amplVec,
			rmsVec);

    Vector<Double> fitVec(blocksize);
    
    CR::RFIMitigation::doBaselineFitting(amplVec,
					rmsVec,
					rmsThresholdValue,
					fitVec);

    Vector<Int> flagVec(blocksize);
    CR::RFIMitigation::doRFIFlagging(spectrum,
				    fitVec,
				    flagThresholdValue,
				    flagVec);
    

    Vector<Double> mitigatedSpectrumVec(blocksize);
    CR::RFIMitigation::doRFIMitigation(spectrum,
				      fitVec,
				      flagVec,
				      mitigatedSpectrumVec);

    //or all in one go
    CR::RFIMitigation::doRFIMitigation(spectrum, 
				      nr_samples,
				      rmsThresholdValue,
				      flagThresholdValue,
				      mitigatedSpectrumVec);
    
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors ();
  nofFailedTests += test_RFIMitigation ();

  return nofFailedTests;
}
