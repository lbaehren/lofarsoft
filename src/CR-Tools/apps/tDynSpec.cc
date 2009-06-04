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
//#include <Data/TBB_Timeseries.h>
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

//_______________________________________________________________________________
//                                                                      show_data

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

//_______________________________________________________________________________
//                                                                      plot_data

void plot_data ()
{
}

//_______________________________________________________________________________
//                                                                           main

/*!
  \brief Test processing of raw TBB data to create a dynamic spectrum

  Input is taken from an HDF5 file containing raw TBB data from the individual
  dipoles, output dynamic spectrum is written to a FITS file.
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
					"-a <antenna nr>\n"
					"-z <nyquist zone>\n";					
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
	
	
	LOFAR_TBB ts2 = LOFAR_TBB(filename, fftblocksize);
	ts2.summary();
	TBB_Timeseries ts = TBB_Timeseries(filename);
	ts.summary();

	cout << "For testing, manually adjusting nyquistzone \n";

	ts2.summary();
	DynamicSpectrum Spectrum = DynamicSpectrum(ts2, nrblocks, antennanr);

	cout << "set outFilename as \n";
    Spectrum.setFilename(outfile);
	cout << "make FITS \n";
	Spectrum.toFITS();
	

	
	//cout << Spectrum.totalPower() << endl;
    return nofFailedTests;
}
