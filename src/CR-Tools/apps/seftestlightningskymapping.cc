/*-------------------------------------------------------------------------*
 | $Id:: seftestlightningskymapping.cc 2009-07-13 swelles        $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
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

#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>
#include <casa/OS/Directory.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <images/Images/PagedImage.h>

using casa::AipsError;
using casa::Array;
using casa::IPosition;
using casa::ObsInfo;
using casa::PagedImage;
using casa::TiledShape;

#include <crtools.h>
#include <Coordinates/SkymapCoordinate.h>
//#include <Data/LopesEventIn.h>
#include <Data/LOFAR_TBB.h>
#include <Analysis/CRinvFFT.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>

using CR::Beamformer;
using CR::ProgressBar;
using CR::SkymapCoordinate;
using CR::Skymapper;
using CR::TimeFreq;

/*!
  \file seftestlightningskymapping.cc

  \ingroup CR_Applications

  \brief Test the Skymapper with a lightning event
 
  \author Sef Welles
 
  \date 2009/07/13

  <h3>Synopsis</h3>

*/

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- lightning data set to use as data input
  \param outfile      -- (path-)name of the image-file to be generated
  \param blocksize    -- Number of samples per block of data.

  \return nofFailedTests --  The number of failed tests encountered within this
          fucntion.
*/
int  simpleImage(string const &infile,
		 string const &outfile,
		 uint const &blocksize=1024)
{
  int nofFailedTests     = 0;
  try {    
    
    //________________________________________________________
    cout << "testlightningskymapping::simpleImage reading in event and setting up the pipeline"  << endl;
    CR::LOFAR_TBB dr(infile, blocksize); 
	cout << "shape of dr = " << dr.shape() << endl;
    CR::CRinvFFT pipeline;
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);
    pipeline.SetObsRecord(obsrec);
    pipeline.InitEvent(&dr);

	  
	// correct for the offset relative to antenna 0. Be aware that reading from the beginning of the file may give problems.
	// casa::Vector< uint > offset = dr.sample_number();
    // offset = -1*offset + offset[0];
	// dr.setShift( (std::vector<int>) offset);
	  
	  

    //________________________________________________________
    // Set up the skymap coordinates and infos
     // Observation info
    cout << "testlightningskymapping::simpleImage Setting up ObsInfo"  << endl;
    std::string telescope  = "LOFAR CS302";
    std::string observer   = "John Doe";
    casa::ObsInfo info;
    info.setTelescope(telescope);
    info.setObserver(observer);
    // Spatial coordinates
    cout << "testlightningskymapping::simpleImage Setting up SpatialCoordinate"  << endl;
    std::string refcode    = "AZEL";
    std::string projection = "STG";
    IPosition shape (3,30,30,3);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius, refcode,projection);
    spatial.setShape(shape);
    //set reference pixel, reference value, and coord increment
    Vector<double> tmpvec;
    tmpvec = spatial.referencePixel();
    tmpvec(0) = 14.5; tmpvec(1)=14.5;  tmpvec(2)=0.;
    spatial.setReferencePixel(tmpvec);
    tmpvec = spatial.referenceValue();
    tmpvec(0) = 180.; tmpvec(1)=90.;  tmpvec(2)=1000.;
    spatial.setReferenceValue(tmpvec,true);
    tmpvec = spatial.increment();
    tmpvec(0) = 7.7; tmpvec(1)=7.7;  tmpvec(2)=10000.;
    spatial.setIncrement(tmpvec,true);
    // Time-Frequency coordinate
    cout << "testlightningskymapping::simpleImage Setting up TimeFreqCoordinate"  << endl;
    uint nofBlocksPerFrame = 8 ;
    uint nofFrames         = (dr.headerRecord().asInt("Filesize")/blocksize)/nofBlocksPerFrame;
    TimeFreqCoordinate timeFreq (blocksize, nofBlocksPerFrame, nofFrames,true);
    timeFreq.setNyquistZone(2);
    // Skymap coordinate
    cout << "testlightningskymapping::simpleImage Setting up SkymapCoordinate"  << endl;
    SkymapCoordinate coord (info,
			    spatial,
			    timeFreq);
    
    //________________________________________________________
    // retrieve the antenna positions

    cout << "testlightningskymapping::simpleImage Retrieving the antenna positions"  << endl;
    Matrix<double> antPositions;
    antPositions = 3827946.312, 459792.315, 5063989.756;
    cout << "the antanna positions are: " << antPositions <<endl;
    /*
	 0 and 1: 3827944.292,459792.072,5063991.294
	 2: 3827942.397,459789.467,5063992.951
	 3: 3827946.675,459782.252,5063990.392
	 4: 3827956.998,459784.497,5063982.442
	 5: 3827894.790,459786.107,5064028.98
	*/

    
	/* If only a few antennas are selected:
    Matrix<double> subantPositions;
	IPosition start1 (2,4,0), length1 (2,2,3), stride1 (2,3,1);
    Slicer slicer1 (start1, length1, stride1);
    subantPositions = antPositions(slicer1);
    cout << subantPositions <<endl;		// change for number of antennas
    */
    //________________________________________________________
    // Set up the skymapper 
    
    cout << "testlightningskymapping::simpleImage Setting up the Skymapper..."  << endl;
	
	Skymapper skymapper (coord,
			 antPositions,		// change for number of antennas
			 outfile,
			 Skymapper::HDF5Image);

    cout << "                                                         ... done."  << endl;
    skymapper.summary();
    
    //________________________________________________________
    // process the event
	
     

    uint nofBlocks = nofBlocksPerFrame * nofFrames;

    Matrix<casa::DComplex> data; 
    //Matrix<casa::DComplex> subdata; 
	//IPosition start (2,0,4), length (2,513,2), stride (2,1,3);
    //Slicer slicer (start, length, stride);
	  
	/*
 	 start and end of lightnign
	 lightning in 16_48 from 963 ms till 1060 ms and some more spikes till 1200 ms  (block 188090, 207030, 234380)
			   in 17_23 from 1158 ms till the end (block 22617 till nofBlocks)
	           in 17_46 from 446 till 820 ms (block 87109 till 160160 )
	*/ 
	  
    for (uint blocknum=0; blocknum<nofBlocks; blocknum++){
      cout << "testlightningskymapping::simpleImage Processing block: " << blocknum << " out of: " 
	   << nofBlocks << endl;
      dr.setBlock(blocknum);
	  cout<<"so far so good..." << endl;
      data = dr.fft();
      //subdata = data (slicer);
      skymapper.processData(data);	// change for number of antennas
    };
	
   cout << "datashape = " << data.shape() << endl;
   //cout << "subdatashape = " << subdata.shape() << endl;

  } catch (AipsError x) {
    cerr << "[testlightningskymapping::simpleImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  cout<<"Dit is de door Sef aangepaste versie van testLOPESskymapping"<<endl;

  uint nofFailedTests=0, blocksize=1024;
  std::string infile, outfile="seftestlightningskymapping.img";

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cout << "Usage: testlightningskymapping <input.event> [<output-image>]. Now using lightning_17_23.h5" << endl;
	infile = "/mnt/lofar/lightning/lightning_17_23.h5"; //contains 6 dipoles, other files are _16_48.h5 and _17_46.h5
  } else {
    infile = argv[1];
     if (argc > 2) {
       outfile  = argv[2];
     };
  }
  
  nofFailedTests += simpleImage (infile,
				 outfile,
				 blocksize);
  
  return nofFailedTests;
}

