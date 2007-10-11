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

#include <IO/toLOPESgrid.h>
#include <IO/ApplyInstrumentEffects.h>
#include <scimath/Mathematics/FFTServer.h>

using CR::toLOPESgrid;  // Namespace usage

using namespace CR;

/*!
  \file ttoLOPESgrid.cc

  \ingroup IO

  \brief A collection of test routines for the toLOPESgrid class
 
  \author Andreas Horneffer
 
  \date 2007/10/10
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new toLOPESgrid object

  \return nofFailedTests -- The number of failed tests.
*/
int test_toLOPESgrid ()
{
  int nofFailedTests (0);
  
  try {
    
    std::cout << "\n[test_toLOPESgrid]\n" << std::endl;    
    std::cout << "[1] Testing default constructor ..." << std::endl;
    toLOPESgrid newtoLOPESgrid;
    
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
    ApplyInstEff.setAntennaIDs(sim2fft.getAntIDs());
    ApplyInstEff.setFreqAxis(sim2fft.getFrequency());
    Matrix<DComplex> FFT;
    FFT = ApplyInstEff.ApplyEffects(sim2fft.getfft());
    std::cout << " ok "  << endl;

    std::cout << "[6] Converting to LOPES grid ... " ;;   
    ok = newtoLOPESgrid.AddFFTData(FFT,  sim2fft.getBlocklen(), sim2fft.getFrequency());
    std::cout << " ok: " << ok << endl;

    std::cout << "[7] Generating output file ... " << endl;;
    std::cout << "   ... retrieving the data from the object," << std::endl;
    Matrix<Double> timedata;
    Vector<Double> times;
    Int i,j,blocklen_p,nants;

    timedata = newtoLOPESgrid.GetData();
    times = newtoLOPESgrid.GetTimes();
    blocklen_p = times.nelements();
    nants = timedata.ncolumn();

    std::cout << "   ... and dumping the data to file." << std::endl;
    FILE *allout;
    allout = fopen("ttoLOPESgrid_pulse.tab","w");
    for (i=0; i< blocklen_p; i++) {
      fprintf(allout,"\n %f    ",times(i)*1e6);
      for (j=0; j<nants; j++) {
	fprintf(allout,"\t %e ",timedata(i,j));
      };
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
    nofFailedTests += test_toLOPESgrid ();
  }

  return nofFailedTests;
}
