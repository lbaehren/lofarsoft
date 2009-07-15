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


#ifdef HAVE_CFITSIO
extern "C" {
#include <fitsio.h>
}
#endif

#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>



/*
  \file ttbbctlIn.cc

  \ingroup CR_Applications

  \brief A file to make a dynamic spectrum of hdf5 data corrected for the antenna gain. (Experimental)
 
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
  std::string calfile = "";
  int fftblocksize = 1024;
  int nrblocks = 2500;
  int antennanr = 0;
  bool weight = 1;
	int startblock = 0;
	
	
	casa::Matrix<double> dynamicSpectrum_p;
	casa::LinearCoordinate timeAxis_p;
	casa::SpectralCoordinate freqAxis_p;
	std::string filename_p;
	
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help"){
			cout << "Options:\n"
					"-i <inputfile.h5>\n"
					"-o <outputfile>\n"
					"-s <fft blocksize>\n"
					"-n <nr of blocks\n"
					"-a <antenna nr>\n"
			        "-w <weight or normalize the function>\n"
					"-b <start block>\n"
			        "-c <calibration file>"
					"-cb <nr of blocks to use for callibration>";
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
		} else if(topic == "-b"){
			        argcounter++;
			        startblock = atoi(argv[argcounter]);
			        cout << "startblock " << startblock << endl;	
		} else if(topic == "-w"){
			        argcounter++;
			        weight = atoi(argv[argcounter]);
				    cout << "if " << weight << "equals 0 there is no normalization" << endl;
		} else {
			cout << "specify at least an input file with -i <inputfile.h5>";
		}
	}
	
	
	LOFAR_TBB ts2 = LOFAR_TBB(filename, fftblocksize);
	ts2.summary();
	//TBB_Timeseries ts = TBB_Timeseries(filename);
    
	// copied from the DynamicsSpectrum class. But now not as a function. 
	//DynamicSpectrum Spectrum = DynamicSpectrum(ts2, nrblocks, antennanr); //, startblock);

	
	/*
	const double alphaHanning = 0.54;
	cout << "Hanning filter set to " << alphaHanning << endl;
	ts2.setHanningFilter(alphaHanning);
	*/
	
	//calculate callibration data
	
	
	

	
	
	//set callibration
	if(weight!=0){
	Matrix< DComplex > fft2calfft; 
	
	int maxnrblocks = (int) ts2.data_length()(antennanr)/ts2.blocksize();
	if(nrblocks == 0 || nrblocks > maxnrblocks){
		nrblocks = maxnrblocks;
	}
	
	ts2.setBlock(0);
	fft2calfft = ts2.fft();
	//callibration starts at block 0 for an equal amount of the blocks from which we start.
	for(int blocknr=0; blocknr < nrblocks; blocknr++){
		ts2.setBlock(blocknr);
		fft2calfft = fft2calfft + ts2.fft();
	}
	fft2calfft =fft2calfft / nrblocks;

    
	//take the inverse of fft2calfft
	fft2calfft =conj(fft2calfft)/(abs(fft2calfft)*abs(fft2calfft));

	
	ts2.setFFT2calFFT 	(fft2calfft); 
												  }
	//Now do it again to callibrate
/*	int maxnrblocks = (int) ts2.data_length()(antennanr)/ts2.blocksize();
	if(nrblocks == 0 || nrblocks > maxnrblocks){
		nrblocks = maxnrblocks;
	}
	int fftsize = (int) ts2.fftLength();
	dynamicSpectrum_p.resize(nrblocks,fftsize);
*/
	int fftsize = (int) ts2.fftLength();
	dynamicSpectrum_p.resize(nrblocks,fftsize);
	for(int blocknr=startblock; blocknr < nrblocks+startblock; blocknr++){
		ts2.setBlock(blocknr);
		Matrix<DComplex> ft = ts2.calfft();
		Matrix<Double> absft = amplitude(ft);
		//cout << setprecision(8) << absft.column(0) << endl ;
		dynamicSpectrum_p.row(blocknr-startblock) = absft.column(antennanr);
	}
	
	
	//./apps/CalDynSpec -n 10240 -i ~/Astro/data/lightning/CS302C-B0T16\:48\:58.h5 -o ~/Astro/data/lightning/fits/CS302C-B0T16\:48\:58UnWeighted -b 18750 -n 2000 -w 0 -s 10240
    //./apps/CalDynSpec -n 10240 -i ~/Astro/data/lightning/CS302C-B0T16\:48\:58.h5 -o ~/Astro/data/lightning/fits/CS302C-B0T16\:48\:58Weighted -b 18750 -n 2000 -w 1 -s 10240

	
	
	
	
	
	
	
	
	// Create time and frequency axes.
	casa::Vector<double> freqs = ts2.frequencyValues(antennanr);
	//cout << freqs(0) << std::endl;
	double samplefreq = ts2.sample_frequency_value()(antennanr);
	string frequnit = ts2.sample_frequency_unit()(antennanr);
	double timestep = 1 / samplefreq * ts2.blocksize();
	string timeunit;
	if(frequnit == "MHz"){
		timeunit = "us";
	} else if(frequnit == "Hz"){
		timeunit = "s";
	} else if(frequnit == "kHz"){
		timeunit = "ms";
	} else{ timeunit = "1/" + frequnit;}
	
	DynamicSpectrum Spectrum = DynamicSpectrum();
	Spectrum.setFrequencyAxis( freqs(0), freqs(1)-freqs(0), "Hz");
	Spectrum.setTimeAxis( 0, timestep, timeunit);
	Spectrum.setSpectrum(dynamicSpectrum_p);
	
	
	
	
	cout << "set outFilename as \n";
    Spectrum.setFilename(outfile);
//  if(weight){
//	cout << "normalizing the spectrum \n";
//		Spectrum.normalize();
//  }
	cout << "make FITS \n";
	Spectrum.toFITS();
	

	
	//cout << Spectrum.totalPower() << endl;
    return nofFailedTests;
}
