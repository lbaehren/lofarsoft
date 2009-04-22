/*-------------------------------------------------------------------------*
 | $Id:: tDynSpec.cc                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (s.terveen@astro.ru.nl)                               *
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
#include <Data/LOFAR_TBB.h>
#include <Display/SimplePlot.h>
#include <Analysis/DynamicSpectrum.h>
#include <cstdlib>
#define PLOT_DATA_PS 0
/*
  \file ttbbctlIn.cc

  \ingroup CR_Applications

  \brief A file to make a dynamic spectrum of hdf5 data
 
  \author Sander ter Veen
 
  \date 2009/03/30
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the data array

  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
}
/*!
  \brief Test programming to process an antenna hfd5 file to a dynamic spectrum in FITS format 
 */
int main (int argc, char *argv[])
{
  int nofFailedTests (0);
  
  
  //default values
  std::string filename = "/Users/STV/Astro/Programming/usg/data/test/rw_20090402_165959_0100.h5";
  std::string outfile ="/Users/STV/Astro/Programming/debugging/antennatest";
  int fftblocksize = 1024;
  int nrblocks = 2500;
  int antennanr = 0;
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help"){
			cout << "Options:\n"
					"-i <inputfile.h5>\n"
					"-o <outputfile>\n"
					"-s <fft blocksize>\n"
					"-n <nr of blocks\n"
					"-a <antenna nr>\n";					
		} else if(topic == "-i"){
	                argcounter++;
					filename = string(argv[argcounter]);
					cout << "input: " << filename << endl;
		} else if(topic == "-o"){
					argcounter++;
					outfile = string(argv[argcounter]);
					cout << "output: " << outfile << endl;
		} else if(topic == "-s"){
	                argcounter++;
					fftblocksize = atoi(argv[argcounter]);
					cout << "blocksize " << fftblocksize << endl;
		} else if(topic == "-n"){
	                argcounter++;
					nrblocks = atoi(argv[argcounter]);
					cout << "nrblocks " << nrblocks << endl;
		} else if(topic == "-a"){
	                argcounter++;
					antennanr = atoi(argv[argcounter]);
					cout << "antenna used " << antennanr << endl;
		} else {
			cout << "specify at least an input file.";
		}
	}
		
	 
	 
 // std::string filename = "/Users/STV/Astro/Programming/usg/data/test/rw_20090402_165959_0100.h5";
  
  
  //std::string filename = "/Users/STV/Astro/Programming/usg/data/test/rw_testfile.h5";

	// Test for the constructor(s)
//   ofstream outfile;
   //TBB_Timeseries ts = TBB_Timeseries(filename);
//	int fftblocksize = 40240;
//	int nrblocks = 2500;
//	int antennanr = 0;
	
	
	
    LOFAR_TBB ts2 = LOFAR_TBB(filename, fftblocksize);

	DynamicSpectrum Spectrum = DynamicSpectrum(ts2, nrblocks, antennanr);
	

	cout << "set outFilename \n";
    Spectrum.setFilename(outfile);
	cout << "make FITS \n";
	Spectrum.toFITS();
	
	casa::Vector<double> SumFreq = log(Spectrum.averageSpectrum());
	casa::Vector<double> SumTime = log(Spectrum.totalPower());
	casa::Vector<double> xTime = ts2.frequencyValues(antennanr);
	//cout << "shapes: " << xFreq.shape() << SumFreq.shape() << SumTime.shape() << endl;
	double samplefreq = ts2.sample_frequency_value()(antennanr);
	double timestep = 1 / samplefreq * ts2.blocksize();
	double time = 0;
	casa::Vector<double> xFreq; //(SumFreq.shape);
	xFreq.resize(SumFreq.shape());
	for(uint in(0); in< xFreq.shape(); in++){
		xFreq(in) = time;
		time += timestep;
	}

	
    SimplePlot plotTT = SimplePlot();
	//std::string psfile = outfile + "_FreqPower.ps";
	plotTT.CR::SimplePlot::InitPlot 	("/Users/STV/Astro/Programming/debugging/DynSpecT.ps", min(xFreq), max(xFreq), min(SumFreq), max(SumFreq), 0, 0, 1, 1, 1);
	plotTT.CR::SimplePlot::PlotLine 	(	xFreq,SumFreq, 10, 1);
	SimplePlot plotF = SimplePlot();
	plotF.CR::SimplePlot::InitPlot 	("/Users/STV/Astro/Programming/debugging/DynSpecF.ps", min(xTime), max(xTime), min(SumTime)-1, max(SumTime)+1, 0, 0, 1, 1, 1);
    plotF.CR::SimplePlot::PlotLine 	(	xTime, SumTime, 10, 1);
	
	//cout << Spectrum.totalPower() << endl;
    return nofFailedTests;
}
