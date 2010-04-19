/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
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

/*!
  \file rmPolSynth.cpp

  \ingroup RM

  \brief RM forward synthesizer to create polarized emission from Faraday emission
 
  \author Sven Duscha
 
  \date 14.10.09.

  <h3>Synopsis</h3>

  RMPolSynthesis command line tool that performs the forward transform to create
  polarized intensity observational data from simulated Faraday emission. Noise
  can be added to the data.
*/

#include <iostream>

#include <rm.h>			// RM Synthesis class
#include <rmIO.h>		// rmio functions
#include <rmCube.h>		// rmCube object
#include <rmNoise.h>		// noise generator functions

using namespace std;			

#define _debug	

//_______________________________________________________________________________
//                                                                          usage

/*!
  \brief Show usage of command line arguments
*/
void usage(char * const argv[])
{
  cout << "usage: " << argv[0] << " <options>" << endl;
  cout << "-d <faradaydepths>" << endl;
  cout << "-e simulated Faraday emission vector" << endl;
  cout << "-s <simulation>	containing both Faraday depths and simulated emission" << endl;
  cout << "-f <frequencies>" << endl;
  cout << "-l Frequencies are actually already lambda squareds" << endl;	
  cout << "-w <weights> (optional)" << endl;
  cout << "-a <min> (Minimum FaradayDepth)" << endl;
  cout << "-b <max> (Maximum Faraday Depth)" << endl;
  cout << "-c <step> (Faraday Depth step)" << endl;	
  cout << "-n <sigma> (optional noise to be added in sigma)" << endl;
  cout << "-o <outputfilename>" << endl;
  cout << "-p <totalpowerOutputfilename>" << endl;
  cout << "-h shows this usage help info" << endl;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc, char * const argv[])
{
  char c;
  
  unsigned int nfaradays (0);	  // number of Faraday depths that were probed for
  unsigned int nfreqs (0);	  // number of frequency channels
  double minFaradayDepth (0.0);
  double maxFaradayDepth (0.0);
  double stepFaradayDepth (0.0);
//   double sigma (0.0);             // standard deviation of polarized emission
  double noiselevel (0.0);        // noise to be added to polarized intensities
  bool lambdaSq (false);          // frequencies are actually given as lambda squareds
  
  string filenameFaradayDepths;		// filename of file containing FaradayDepths in simulated emission
  string filenameFaradayEmission;		// 
  string filenameSimulation;		// complete Faraday simulation
  string filenameFrequencies;		// filename of file containing Frequencies
  string filenameWeights;			// filename for output file for polarized intensities
  string filenameOutput;			// filename for complex polarization output
  string filenamePower;			// filename for total power output
  
  
  // TEST: forwardFourier RM-Synthesis
  vector<double> faradayDepths(nfaradays);
  vector<double> deltaFaradayDepths(nfaradays);
  vector<double> simulatedEmission(nfaradays);
  vector<complex<double> > polintensities(nfreqs);
  vector<double> powerIntensities(nfreqs);
  // 	vector<double> intensities(nfreqs);
  vector<double> weights(nfreqs);			// weights
  vector<double> frequencies(nfreqs);
  vector<double> deltaFrequencies(nfreqs);
  vector<double> freq_low(nfreqs);
  vector<double> freq_high(nfreqs);
  vector<double> lambdaSquareds(nfreqs);
  vector<double> deltaLambdaSquareds(nfreqs);
  
  vector<complex<double> > rmsf(nfaradays);	// RMSF for tests
  // 	vector<complex<double> > rmpol(nfaradays);	// polarized intensity at Faraday depth probed for
  // 	vector<double> rmpolQ(nfaradays);		// Q intensity (derived through Forward Transform)
  // 	vector<double> rmpolU(nfaradays);		// U intensity (derived through Forward Transform)
  
  // Create a rmCube object for providing algorithms, io etc.
  RM::rmCube RM;
  // Noise object for noise creation algorithm
  RM::rmNoise noise;
  
  
  try{
    
    if(argc<3) {
      usage(argv);
      exit(0);
    }
    
    // Parse command line arguments:
    // -d <faradaydepths>
    // -s <simulation>	containing both Faraday depths and simulated emission
    // -f <frequencies>
    // -l Frequencies are actually already lambda squareds
    // -w <weights> (optional)
    // -a <min> (Minimum FaradayDepth)
    // -b <max> (Maximum Faraday Depth)
    // -c <step> (Faraday Depth step)
    // -n <sigma> (optional noise to be added in sigma)
    // -o <output>
    while ((c = getopt (argc, argv, "d:e:f:l:w:o:p:a:b:c:n:h")) != -1)
      {
	switch (c)
	  {
	  case 'd':		 	// Faraday depths to probe for
	    filenameFaradayDepths = optarg;
	    break;
	  case 's':			// we have a simulation file that contains both Faraday depths (1st column) and Faraday emission (2nd column)
	    filenameSimulation = optarg;
	    break;
	  case 'e':			// simulated Faraday emission vector
	    filenameFaradayEmission = optarg;
	    break;
	  case 'f':		  	// File containing frequencies
	    filenameFrequencies = optarg;
	    break;
	  case 'l':			
	    lambdaSq = true;		// "Frequencies file" contains lambda Squareds instead
	    break;
	  case 'w':			// File containing weights (optional)
	    filenameWeights=optarg;
	    break;
	  case 'o':  				// output filename
	    filenameOutput=optarg;
	    break;
	  case 'p':  				// total Power output filename
	    filenamePower=optarg;
	    break;
	  case 'a':				// minimum Faraday depth if computed from arguments
	    minFaradayDepth=atof(optarg);
	    break;
	  case 'b':				// maximum Faraday depth if computed from arguments
	    maxFaradayDepth=atof(optarg);
	    break;
	  case 'c':			// step Faraday depth if computed from arguments
	    stepFaradayDepth=atof(optarg);
	    break;
	  case 'n':			// add n-sigma noise to polarized Intensities
	    noiselevel=atof(optarg);
	    break;
	  case 'h':
	    usage(argv);
	    exit(0);
	    break;
	  case '?':
	    if (optopt=='d' || optopt=='f'  || optopt=='l' || optopt=='p' || optopt=='w' || optopt=='o' || optopt=='a' || optopt=='b' || optopt=='c' || optopt=='n' )
	      fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	    else if (isprint (optopt))
	      fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	    else
	      fprintf (stderr,
		       "Unknown option character `\\x%x'.\n",
		       optopt);
	    return 1;
	  default:
	    abort ();
	  }
      }
    
    //**********************************************************************************
    //
    // Read simulation and parameters from files (with filenames provided by command
    // line arguments - if none is given, use default)
    //
    //**********************************************************************************
    
    if(filenameSimulation!="")	// If a complete Simulation file was given
      {
	RM.readFaradaySimulationFromFile(faradayDepths, deltaFaradayDepths, filenameSimulation);
      }
    else	// load vectors individually...
      {
	if(filenameFaradayDepths!="")	// if a Faraday Depths file is given...
	  RM.readVectorFromFile(faradayDepths, filenameFaradayDepths);
	else {
	  // consistency check
	  if(minFaradayDepth==maxFaradayDepth && minFaradayDepth!=0)
	    throw "RMPolSynthesizer: minFaradayDepth equals maxFaradayDepth";
	  if(minFaradayDepth > maxFaradayDepth)
	    throw "RMPolSynthesizer: minFaradayDepth > maxFaradayDepth";
	  if(stepFaradayDepth==0)
	    throw "RMPolSynthesizer: step equals 0";
	  
	  // compute Faraday depths from min/max and step
	  for(unsigned int i=minFaradayDepth; i < maxFaradayDepth;) {
	    faradayDepths[i]=minFaradayDepth+i*stepFaradayDepth;
	    i+=stepFaradayDepth;
	  }
	}
      }
    
    if(filenameFrequencies!="")	// if a frequency file is given...
      {
	RM.readVectorFromFile(frequencies, filenameFrequencies);
	
	if(lambdaSq==false)		// if these are frequencies, convert them to lambda Squareds
	  lambdaSquareds=RM.freqToLambdaSq(frequencies);
	else  			// ... otherwise just copy them
	  lambdaSquareds=frequencies;
      }
    else
      cout << "RMPolSynthesizer: No filename for frequencies given!" << endl;
    
    if(filenameWeights!="")		// if a file for the weights is given...
      RM.readVectorFromFile(weights, filenameWeights);
    else				// set to default weight=1 for all weights
      {
	weights.resize(frequencies.size());
	for(unsigned int i=0; i < frequencies.size(); i++) {
	  weights[i]=1;
	}
      }
    
    // Read in Faraday emission simulation from file
    if(filenameFaradayEmission!="") {
      RM.readVectorFromFile(simulatedEmission, filenameFaradayEmission);	
    }
    else {
      cout << "RMPolSynthesizer: No filename for Faraday emission simulation given"
	   << endl;
    }
    
    
    
    //*******************************************************************************************
    nfaradays=faradayDepths.size();
    deltaFaradayDepths.resize(nfaradays);
    // Faradayd depths to probe for
    for(unsigned int i=0; i<nfaradays; i++) {
      // 		faradayDepths[i]=i-150.0;				// faraday depths from -150 to +150
      deltaFaradayDepths[i]=1; 				// equidistant faraday depths
    }
    
    // compute polarized intensities from Faraday emission intensities
    polintensities = RM.forwardFourier (lambdaSquareds,
					simulatedEmission,
					faradayDepths,
					weights,
					deltaFaradayDepths,
					0);
    
    
    if(noiselevel!=0)		// if an additive noise was given
      {
	// 	sigma=noise.computeSigma(polintensities);
	/* Add noise to data */
	// 	noise.addNoise(polintensities, noiselevel*sigma, "white");
      }
    
    
    if(filenameOutput=="") {
      throw "RMPolSyntesizer: outputfilename is not given";
    }
    else {
      RM.writeRMtoFile(lambdaSquareds, polintensities, filenameOutput);
    }
    if(filenamePower!="") {
      RM.complexPower(polintensities, powerIntensities);	
      RM.writeIntToFile(lambdaSquareds, powerIntensities, filenamePower);
    }
  }
  
  catch (const char *s) {
    cout << s << endl;
  }
  
  return 0;
}


