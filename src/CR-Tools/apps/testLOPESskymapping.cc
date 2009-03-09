/*-------------------------------------------------------------------------*
 | $Id:: testLOPESskymapping.cc 2403 2009-03-05 15:03:49Z baehren        $ |
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

#include <Coordinates/SkymapCoordinate.h>
#include <Data/LopesEventIn.h>
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
  \file testLOPESskymapping.cc

  \brief Test the Skymapper with a LOPES event
 
  \author Andreas Horneffer
 
  \date 2009/03/09

  <h3>Synopsis</h3>

*/

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- LOPES data set to use as data input
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
    cout << "testLOPESskymapping::simpleImage reading in event and setting up the pipeline"  << endl;
    CR::LopesEventIn dr(infile, blocksize); 
    CR::CRinvFFT pipeline;
    Record obsrec;
    obsrec.define("LOPES","/home/horneff/usg/data/lopes/LOPES-CalTable");
    pipeline.SetObsRecord(obsrec);
    pipeline.InitEvent(&dr);


    //________________________________________________________
    // Set up the skymap coordinates and infos
     // Observation info
    cout << "testLOPESskymapping::simpleImage Setting up ObsInfo"  << endl;
    std::string telescope  = "LOPES";
    std::string observer   = "John Doe";
    casa::ObsInfo info;
    info.setTelescope(telescope);
    info.setObserver(observer);
    // Spatial coordinates
    cout << "testLOPESskymapping::simpleImage Setting up SpatialCoordinate"  << endl;
    std::string refcode    = "AZEL";
    std::string projection = "SIN";
    IPosition shape (3,30,30,10);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius, refcode,projection);
    spatial.setShape(shape);
    // Time-Frequency coordinate
    cout << "testLOPESskymapping::simpleImage Setting up TimeFreqCoordinate"  << endl;
    uint nofBlocksPerFrame = 1;
    uint nofFrames         = (dr.headerRecord().asInt("Filesize")/blocksize)/nofBlocksPerFrame;
    TimeFreqCoordinate timeFreq (blocksize, nofBlocksPerFrame, nofFrames,true);
    // Skymap coordinate
    cout << "testLOPESskymapping::simpleImage Setting up SkymapCoordinate"  << endl;
    SkymapCoordinate coord (info,
			    spatial,
			    timeFreq);
    
    //________________________________________________________
    // retrieve the antenna positions

    cout << "testLOPESskymapping::simpleImage Retrieving the antenna positions"  << endl;
    Matrix<double> antPositions;
    antPositions = pipeline.GetAntPositions(&dr);
    cout << antPositions <<endl;
    
    //________________________________________________________
    // Set up the skymapper
    
    cout << "testLOPESskymapping::simpleImage Setting up the Skymapper..."  << endl;
    Skymapper skymapper (coord,
			 antPositions,
			 outfile);
    cout << "                                                         ... done."  << endl;
    skymapper.summary();
    
    //________________________________________________________
    // process the event
     

    uint nofBlocks = nofBlocksPerFrame * nofFrames;

    Matrix<casa::DComplex> data; 

    for (uint blocknum=0; blocknum<nofBlocks; blocknum++){
      dr.setBlock(blocknum);
      data = dr.fft();
      skymapper.processData(data);
    };
    
  } catch (AipsError x) {
    cerr << "[testLOPESskymapping::simpleImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  uint nofFailedTests=0, blocksize=1024;
  std::string infile, outfile="testLOPESskymapping.img";

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cerr << "Usage: testLOPESskymapping <input.event> [<output-image>]"
	      << endl;
    return 1;
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
