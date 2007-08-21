/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: tCRinvFFT.cc,v 1.4 2007/06/19 08:10:53 horneff Exp $*/

#include <casa/Containers/Record.h>

#include <Analysis/CRinvFFT.h>
#include <Data/LopesEventIn.h>

/*!
  \file tCRinvFFT.cc

  \ingroup Analysis

  \brief A collection of test routines for CRinvFFT
 
  \author Andreas Horneffer
 
  \date 2007/04/20
*/

// -----------------------------------------------------------------------------

using CR::LopesEventIn;
using CR::CRinvFFT;

/*!
  \brief Test constructors for a new CRinvFFT object

  \return nofFailedTests -- The number of failed tests.
*/
int test_CRinvFFT ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_CRinvFFT]\n" << std::endl;

  try {

    // First we need a DataReader object. In our case we take "LopesEventIn" 
    // which is derived from the DataReader
    LopesEventIn lev;

    std::cout << "[1] Testing default constructor ..." << std::endl;
    // Need the pipeline object itself
    CRinvFFT newObject;    
    
    std::cout << "[2] Attaching observatories Record ..." << std::endl;
    // Generate a record that points to the CalTable files 
    // Hardcoded path is not nice, but works...
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");

    // Put the record into the pipeline object.
    newObject.SetObsRecord(obsrec);

    std::cout << "[3] Opening Event ..." << std::endl;
    if (! lev.attachFile("/data/LOPES/cr/sel/2004.01.12.00:28:11.577.event") ){
      std::cout << "Failed to attach file: /data/LOPES/cr/sel/2004.01.12.00:28:11.577.event" << endl;
      nofFailedTests++;
      return nofFailedTests;
    };

    std::cout << "[4] Initializing the DataReader ..." << std::endl;
    // Open a new event/datafile/whatever.
    // This function does (nearly) all the magic.
    if (! newObject.InitEvent(&lev)){
      std::cout << "  Failed to initializze the DataReader!" << std::endl;
      nofFailedTests++;
      return nofFailedTests;
    };
    
    std::cout << "[5] Arranging storage, ..." << std::endl;
    Vector<Double> Times,ccBeam;
    Matrix<Double> TimeSeries;
    Int i,j,TSlen,nants;

    FILE *allout;

    allout = fopen("tCRinvFFT_allants_timeseries.tab","w");
    Times = lev.timeValues();
    std::cout << "                      ... Setting direction and phase center," << std::endl;
    //newObject.setPhaseCenter(-22.2582,15.22359);
    newObject.setPhaseCenter(-25.44, 8.94, True);
    newObject.setDirection(41.9898208,64.70544,1750);

    std::cout << "                      ... retrieving the data from the pipeline," << std::endl;
    TimeSeries = newObject.GetTimeSeries(&lev);
    ccBeam = newObject.GetCCBeam(&lev);

    nants = TimeSeries.ncolumn();
    TSlen = TimeSeries.nrow();
    std::cout << "                      ... and dumping the filtered data to file." << std::endl;
    for (i=0; i< TSlen; i++) {
      fprintf(allout,"\n %e ",Times(i));
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %e ",TimeSeries(i,j)*1e6);
      };
      fprintf(allout," \t \t %e ",ccBeam(i)*1e6);
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
    nofFailedTests += test_CRinvFFT ();
  }

  return nofFailedTests;
}
