/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <IO/TSmatrix2Event.h>
#include <IO/toLOPESgrid.h>
#include <IO/ApplyInstrumentEffects.h>

using CR::TSmatrix2Event;  // Namespace usage

using namespace CR;

/*!
  \file tTSmatrix2Event.cc

  \ingroup IO

  \brief A collection of test routines for the TSmatrix2Event class
 
  \author Andreas Horneffer
 
  \date 2007/10/12
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TSmatrix2Event object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TSmatrix2Event ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_TSmatrix2Event]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    TSmatrix2Event newTSmatrix2Event;

    std::cout << "[2] Opening Simulation ... " ;
    Simulation2fftMatrix sim2fft;
    ApplyInstrumentEffects ApplyInstEff;
    ImportSimulation Sim;
    bool ok = Sim.openNewSimulation("30deg1e17_shift",
				    "lopes30",
				    "/home/horneff/testreas");
    std::cout << " ok: " << ok << endl;

    std::cout << "[3] Generating and attaching CalTableReader object ... " << endl;;
    CalTableReader CTread("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    sim2fft.setCalTable(&CTread);
    ApplyInstEff.setCalTable(&CTread);
    sim2fft.setObsDate(1104580800);
    ApplyInstEff.setObsDate(1104580800);


    std::cout << "[4] Importing simulation ... " ;;     
    ok =  sim2fft.SimulationImport(&Sim);
    std::cout << " ok: " << ok << endl;

    std::cout << "Simulation time steps:"<< Sim.getSamplingTimeScale() << "s." << endl;

    std::cout << "[5] Applying Instrument Effects ... " ;;   
    Vector<int> AntIDs;
    AntIDs = sim2fft.getAntIDs();
    ApplyInstEff.setAntennaIDs(AntIDs);
    ApplyInstEff.setFreqAxis(sim2fft.getFrequency());
    Matrix<DComplex> FFT;
    FFT = ApplyInstEff.ApplyEffects(sim2fft.getfft());
    std::cout << " ok "  << endl;

    std::cout << "[6] Converting to LOPES grid ... " ;;   
    toLOPESgrid toGrid;
    ok = toGrid.AddFFTData(FFT,  sim2fft.getBlocklen(), sim2fft.getFrequency());
    std::cout << " ok: " << ok << endl;
    Matrix<Double> timedata;
    timedata = toGrid.GetData();

    std::cout << "[7] Inserting data into event-object ... " <<endl; 
    ok = newTSmatrix2Event.SetData(timedata,AntIDs,32768);
    ok = ok && newTSmatrix2Event.SetDate(1104580800.);
    std::cout << "    ... and writing file ..." << endl;
    ok = ok && newTSmatrix2Event.WriteEvent("tTSmatrix2Event_output.event");
    std::cout << "    ok: " << ok << endl;

    //
    newTSmatrix2Event.summary(); 
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
    nofFailedTests += test_TSmatrix2Event ();
  }

  return nofFailedTests;
}
