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
using namespace CR;
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
  std::string filename1 = "/Users/STV/Astro/Programming/usg/data/test/rw_20090402_165959_0100.h5";
  std::string outfile ="/Users/STV/Astro/Programming/debugging/antennatest";
  int fftblocksize = 1024;
  int nrblocks = 2500;
  int antennanr = 0, antennanr2 = 0;
  bool weight = 0;
	int startblock = 0;
	int nfiles = 1;
	int ninputfiles = 1;
	std::string filename2, filename3, filename4, filename5, filename6;
	
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help"){
			cout << "Options:\n"
					"-in <nr of input files>\n"
			        "-i <inputfile.h5, or list of files if -in command was given>\n"
					"-o <outputfile>\n"
					"-s <fft blocksize>\n"
					"-n <nr of blocks\n"
					"-a <antenna nr>\n"
			        "-w <weight or normalize the function>\n"
					"-b <start block>\n";
		} else if(topic == "-in"){
			argcounter++;
			nfiles = atoi(argv[argcounter]);
			cout << "nr of inputfiles: " << nfiles << endl;
		} else if(topic == "-i"){
				argcounter++;
				filename1 = string(argv[argcounter]);
			    int q=1;
			    ninputfiles=nfiles;
			cout << "q= " << q << "ninputfiles= " << ninputfiles << endl;
			    cout << "input 1: " << filename1 << endl;
		    if(q<ninputfiles) {
				argcounter++;
				filename2 = string(argv[argcounter]);
			    q++;
			    cout << "input 2: " << filename2 << endl;
			} 
			if(q<ninputfiles) {
				argcounter++;
				filename3 = string(argv[argcounter]);
			    q++;
			    cout << "input 3: " << filename3 << endl;
			} 
			if(q<ninputfiles) {
				argcounter++;
				filename4 = string(argv[argcounter]);
			    q++;
			    cout << "input 4: " << filename4 << endl;
			} 
			if(q<ninputfiles) {
				argcounter++;
				filename5 = string(argv[argcounter]);
			    q++;
			    cout << "input 5: " << filename5 << endl;
			} 
			if(q<ninputfiles) {
				argcounter++;
				filename6 = string(argv[argcounter]);
			    q++;
			cout << "input 6: " << filename6 << endl;
			}
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
		} else if(topic == "-in"){
			argcounter++;
			antennanr2 = atoi(argv[argcounter]);
			cout << "second antenna used " << antennanr << endl;
		} else if(topic == "-b"){
			argcounter++;
			startblock = atoi(argv[argcounter]);
			cout << "startblock " << startblock << endl;	
		} else if(topic == "-w"){
				    weight = 1;
				cout << "spectrum will be normalized " << endl;
		} else {
			cout << "specify at least an input file with -i <inputfile.h5>";
		}
	}
	
	
	LOFAR_TBB ts2 = LOFAR_TBB(filename1, fftblocksize);
	cout << "Summary fails. Using infile " << filename1 << endl;
	ts2.summary();
	//TBB_Timeseries ts = TBB_Timeseries(filename);
    int nants=ts2.fx().shape()[1];
	DynamicSpectrum Spectrum = DynamicSpectrum(ts2, nrblocks, antennanr); //, startblock);
    
	cout << "summing over "<< nants << " antennas" << endl;
	for(antennanr2=1;antennanr2<nants; antennanr2++){
	   DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
	   Spectrum.sumSpectrum(otherSpectrum);
	}
    std::string outfiletemp=outfile+"_1";
	cout << "set outFilename as \n";
    Spectrum.setFilename(outfiletemp);
	
	cout << "make FITS \n";
	Spectrum.toFITS();
	
	if(ninputfiles>=2){
		cout << "Summary fails. Using infile " << filename2 << endl;
		LOFAR_TBB ts2 = LOFAR_TBB(filename2, fftblocksize);
		nants=ts2.fx().shape()[1];
		for(antennanr2=0;antennanr2<nants; antennanr2++){
			DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
			Spectrum.sumSpectrum(otherSpectrum);
		}
	} 
	outfiletemp=outfile+"_2";
	cout << "set outFilename as \n";
    Spectrum.setFilename(outfiletemp);
	
	cout << "make FITS \n";
	Spectrum.toFITS();
	if(ninputfiles>=3){	
		cout << "Summary fails. Using infile " << filename3 << endl;
		LOFAR_TBB ts2 = LOFAR_TBB(filename3, fftblocksize);
		nants=ts2.fx().shape()[1];
		for(antennanr2=0;antennanr2<nants; antennanr2++){
			DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
			Spectrum.sumSpectrum(otherSpectrum);
		}
	} 
	if(ninputfiles>=4){
		cout << "Summary fails. Using infile " << filename4 << endl;
		LOFAR_TBB ts2 = LOFAR_TBB(filename4, fftblocksize);
		nants=ts2.fx().shape()[1];
		for(antennanr2=0;antennanr2<nants; antennanr2++){
			DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
			Spectrum.sumSpectrum(otherSpectrum);
		}
	} 
	outfiletemp=outfile+"_4";
	cout << "set outFilename as \n";
    Spectrum.setFilename(outfiletemp);
	
	cout << "make FITS \n";
	Spectrum.toFITS();
	if(ninputfiles>=5){
		cout << "Summary fails. Using infile " << filename5 << endl;
		LOFAR_TBB ts2 = LOFAR_TBB(filename5, fftblocksize);
		nants=ts2.fx().shape()[1];
		for(antennanr2=0;antennanr2<nants; antennanr2++){
			DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
			Spectrum.sumSpectrum(otherSpectrum);
		}
	} 
	if(ninputfiles>=6){		
		cout << "Summary fails. Using infile " << filename6 << endl;
		LOFAR_TBB ts2 = LOFAR_TBB(filename6, fftblocksize);
		nants=ts2.fx().shape()[1];
		for(antennanr2=0;antennanr2<nants; antennanr2++){
			DynamicSpectrum otherSpectrum = DynamicSpectrum(ts2, nrblocks, antennanr2);
			Spectrum.sumSpectrum(otherSpectrum);
		}
	}
	 
	

	cout << "set outFilename as \n";
    Spectrum.setFilename(outfile);

	cout << "make FITS \n";
	Spectrum.toFITS();
	

	
	//cout << Spectrum.totalPower() << endl;
    return nofFailedTests;
}
