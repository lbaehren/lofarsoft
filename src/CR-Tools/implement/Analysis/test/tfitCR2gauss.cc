/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id$*/

#include <Analysis/CRinvFFT.h>
#include <IO/LopesEventIn.h>
#include <Analysis/fitCR2gauss.h>

/*!
  \file tfitCR2gauss.cc

  \ingroup Analysis

  \brief A collection of test routines for fitCR2gauss
 
  \author Andreas Horneffer
 
  \date 2007/06/18
*/

using CR::CRinvFFT;
using CR::LopesEventIn;
using CR::fitCR2gauss;

/*!
  \brief Test constructors for a new CRinvFFT object

  \return nofFailedTests -- The number of failed tests.
*/
int test_fitCR2gauss ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_CRinvFFT]\n" << std::endl;

  try {

    // First we need a DataReader object. In our case we take "LopesEventIn" 
    // which is derived from the DataReader
    LopesEventIn lev;

    std::cout << "[1] Testing default constructor ..." << std::endl;
    // Need the pipeline object itself
    CRinvFFT pipeline;    
    
    std::cout << "[2] Attaching observatories Record ..." << std::endl;
    // Generate a record that points to the CalTable files 
    // Hardcoded path is not nice, but works...
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/usg/data/lopes/LOPES-CalTable");

    // Put the record into the pipeline object.
    pipeline.SetObsRecord(obsrec);

    std::cout << "[3] Opening Event ..." << std::endl;
    if (! lev.attachFile("/data/LOPES/cr/sel/2004.01.12.00:28:11.577.event") ){
      std::cout << "Failed to attach file: /data/LOPES/cr/sel/2004.01.12.00:28:11.577.event" << endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[4] Initializing the DataReader ..." << std::endl;
    // Open a new event/datafile/whatever.
    if (! pipeline.InitEvent(&lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    
    std::cout << "[5] Running the pipeline ..." << std::endl;
    Vector<Double> Times,ccBeam,xBeam,pBeam,gauss;
    Matrix<Double> TimeSeries;
    Int i,TSlen,nants;

    std::cout << "                      ... Setting direction and phase center," << std::endl;
    //pipeline.setPhaseCenter(-22.2582,15.22359);
    pipeline.setPhaseCenter(-25.44, 8.94, True);
    pipeline.setDirection(41.9898208,64.70544,1750);

    std::cout << "                      ... retrieving the data from the pipeline," << std::endl;
    lev.timeValues(Times);
    pipeline.GetTCXP(&lev, TimeSeries, ccBeam, xBeam, pBeam);

    StatisticsFilter<Double> mf(3,FilterType::MEAN);
    ccBeam = mf.filter(ccBeam);
    xBeam = mf.filter(xBeam);
    pBeam = mf.filter(pBeam);


    std::cout << "[6] Do the fitting ..." << std::endl; 
    // Now we finally have all the data from the pipeline
    // and can start using the fitter...
    fitCR2gauss newObject;
    Record fiterg;
    Vector<uInt> remoterange(2);
    
    std::cout << "                      ... initializing the fitter,";
    std::cout << newObject.setTimeAxis(Times);
    remoterange(0) = 16384; remoterange(1) = 29127;
    std::cout << newObject.setRemoteRange(remoterange);
    std::cout << newObject.setFitRangeSeconds(-2e-6,-1.7e-6);
    std::cout << endl;

    std::cout << "                      ... do the fitting," << endl;
    fiterg = newObject.Fitgauss(xBeam,ccBeam,True);

    cout << "   Fit results:" << endl;
    cout << "     XConverged: " << fiterg.asBool("Xconverged") << " CCConverged: " 
	 << fiterg.asBool("CCconverged") << endl;
    cout << "     XHeight: " << fiterg.asDouble("Xheight") << "+-"
	 <<fiterg.asDouble("Xheight_error") << endl;
    cout << "     XWidth: " << fiterg.asDouble("Xwidth") << "+-"
	 <<fiterg.asDouble("Xwidth_error") << endl;
    cout << "     XCenter: " << fiterg.asDouble("Xcenter") << "+-"
	 <<fiterg.asDouble("Xcenter_error") << endl;
    cout << "     CCHeight: " << fiterg.asDouble("CCheight") << "+-"
	 <<fiterg.asDouble("CCheight_error") << endl;
    cout << "     CCWidth: " << fiterg.asDouble("CCwidth") << "+-"
	 <<fiterg.asDouble("CCwidth_error") << endl;
    cout << "     CCCenter: " << fiterg.asDouble("CCcenter") << "+-"
	 <<fiterg.asDouble("CCcenter_error") << endl;
    gauss = fiterg.asArrayDouble("Cgaussian");


    std::cout << "[7] Generating output, ..." << std::endl;

    FILE *allout;

    allout = fopen("tfitCR2gauss_timeseries.tab","w");

    nants = TimeSeries.ncolumn();
    TSlen = TimeSeries.nrow();
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i)*1e6);//1
      fprintf(allout," \t \t %e ",xBeam(i));//2
      fprintf(allout," \t %e ",ccBeam(i));//3
      fprintf(allout," \t %e ",pBeam(i));//4
      fprintf(allout," \t %e ",gauss(i));//5
    };
    fprintf(allout,"\n");
    fclose(allout);
    

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_fitCR2gauss ();
  }

  return nofFailedTests;
}
