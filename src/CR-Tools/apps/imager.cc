/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/detail/cmdline.hpp>

#include <crtools.h>
#include <Data/LOFAR_TBB.h>
#include <IO/LopesEventIn.h>
#include <Imaging/Skymapper.h>
#include <IO/DataReader.h>

using std::cerr;
using std::cout;
using std::endl;

namespace bpo = boost::program_options;

/*!
  \file imager.cc

  \ingroup CR_Applications
  
  \brief Application front-end to the Skymapper class

  \author Lars B&auml;hren

  \date 2008/09/08

  <h3>Prerequisite</h3>
  
  <ul>
    <li>CR::Skymapper
    <li>CR::Beamformer
  </ul>

  <h3>Synopsis</h3>
  
  While the main functionality is implemented through the classes collected in
  \e Coordinates and \e Imaging, this is the executable expected to be run by
  the prospective user. 

  <h3>Usage</h3>
  
  Available command line options:
  
  <table border="0">
    <tr>
      <td class="indexkey">Option</td>
      <td class="indexkey">Default</td>
      <td class="indexkey">Decription</td>
    </tr>
    <tr>
      <td>help</td>
      <td></td>
      <td>Show help messages.</td>
    </tr>
    <tr>
      <td>infile</td>
      <td>--- (mandatory)</td>
      <td>Input data file from which to generate the image.</td>
    </tr>
    <tr>
      <td>outfile</td>
      <td>skymap.h5</td>
      <td>Output image file.</td>
    </tr>
    <tr>
      <td>blocksize</td>
      <td>1024</td>
      <td>Size of an individual block of data read from file, [samples].</td>
    </tr>
    <tr>
      <td>blocks_per_frame</td>
      <td>1</td>
      <td>Number of input data blocks combined into an output time-frame.</td>
    </tr>
    <tr>
      <td>frames</td>
      <td>1</td>
      <td>Number of time-frames in generated image.</td>
    </tr>
    <tr>
      <td>refcode</td>
      <td>AZEL</td>
      <td>Reference code for the celestial coordinate system.</td>
    </tr>
    <tr>
      <td>projection</td>
      <td>STG</td>
      <td>Reference code for the spherical map projection.</td>
    </tr>
    <tr>
      <td>beamtype</td>
      <td>FREQ_POWER</td>
      <td>Type of beam used to create the skymap.</td>
    </tr>
    <tr>
      <td>coord_type</td>
      <td>DirectionRadius</td>
      <td>Type of spatial coordinate.</td>
    </tr>
  </table>

  <h3>Examples</h3>

  <ul>
    <li>Show the available command line options:
    \verbatim
    imager --help
    \endverbatim
    <li>Use default setting for the generations of a skymap:
    \verbatim
    imager --infile 20080604_122217_2300.dat.h5
    \endverbatim
    or via the short form
    \verbatim
    imager -I 20080604_122217_2300.dat.h5
    \endverbatim
    <li>Generate skymap using a specific celestial reference frame and spherical
    map projection:
    \verbatim
    imager -I 20080604_122217_2300.dat.h5 --refcode J2000 --projection SIN
    \endverbatim
  </ul>
*/

//_______________________________________________________________________________
//                                                                  setDataReader

/*!
  \brief Set up a DataReader object to read in the data

  This function will attempt to determine the type of input data from which to
  create the image. 

  \retval dr       -- Pointer to a DataReader object, allowing access to the 
          contents of the dataset.
  \param infile    -- Input dataset from which to create the skymap.
  \param blocksize -- Size of an individual block of data read from file,
         [samples].

  \return status -- Status of the operation; returns \e true in case the input
          dataset could be opened successfully. In case there was an error
	  opening the input dataset -- either because of unknown type or due to
	  corrupted data -- \e false will be returned.
*/
bool setDataReader (CR::DataReader *dr,
		    std::string const &infile,
		    uint const &blocksize)
{
  bool status (true);

  /* Try to open data-set using LopesEventIn */
  try {
    cout << "[imager] Try to open data-set using LopesEventIn ..." << endl;
    CR::LopesEventIn *data = new CR::LopesEventIn (infile,blocksize);
    dr = data;
  } catch (std::string message) {
    cerr << "[imager] Failed to open data-set as LOPES-Event." << endl;
    status = false;
  }
  
  /* Try to open data-set using LOFAR_TBB */
  if (status == false) {
    status = true;
    try {
      cout << "[imager] Try to open data-set using LOFAR-TBB ..." << endl;
      CR::LOFAR_TBB *data = new CR::LOFAR_TBB (infile,blocksize);
      dr = data;
    } catch (std::string message) {
      cerr << "[imager] Failed to open data-set as LOFAR-TBB." << endl;
      status = false;
    }
  }

  cout << "-- blocksize  dial = " << dr->blocksize() << endl;
  cout << "-- FFT length = " << dr->fftLength() << endl;
  
  return status;
}

//_______________________________________________________________________________
//                                                                  main function

int main (int argc, char *argv[])
{
  int status (0);

  //__________________________________________________________________
  // Parsing of the command-line parameters

  std::string infile     = "";
  std::string outfile    = "skymap.h5";
  uint blocksize         = 1024;
  int blocksPerFrame     = 1;
  int nofFrames          = 1;
  std::string refcode    = "AZEL";
  std::string projection = "STG";
  std::string beamType   = "FREQ_POWER";
  std::string coordType  = "DirectionRadius";

  bpo::options_description desc ("[imager] Available command line options");

  desc.add_options ()
    ("help,H", "Show help messages")
    ("infile,I", bpo::value<std::string>(),
     "Input data file from which to generate the image")
    ("outfile,O", bpo::value<std::string>(),
     "Output image file.")
    ("blocksize,B", bpo::value<uint>(),
     "Size of an individual block of data read from file, [samples].")
    ("blocks_per_frame", bpo::value<int>(),
     "Number of input data blocks combined into an output time-frame. (1)")
    ("frames,F", bpo::value<int>(),
     "Number of time-frames in generated image. (1)")
    ("refcode,R", bpo::value<std::string>(),
     "Reference code for the celestial coordinate system. (AZEL)")
    ("projection,P", bpo::value<std::string>(),
     "Reference code for the spherical map projection. (STG)")
    ("beamtype", bpo::value<std::string>(),
     "Type of beam to create the skymap with. (FREQ_POWER)")
    ("coord_type", bpo::value<std::string>(),
     "Type of spatial coordinate used in the skymap. (DirectionRadius)")
    ;
  
  bpo::variables_map vm;
  bpo::store (bpo::parse_command_line(argc,argv,desc), vm);
  
  if (vm.count("help") || argc == 1) {
    cout << desc << endl;
    return 0;
  }
  
  if (vm.count("blocksize")) {
    blocksize = vm["blocksize"].as<uint>();
  }
  
  if (vm.count("blocks_per_frame")) {
    blocksPerFrame = vm["blocks_per_frame"].as<int>();
  }
  
  if (vm.count("frames")) {
    nofFrames = vm["frames"].as<int>();
  }
  
  if (vm.count("infile")) {
    infile = vm["infile"].as<std::string>();
  } else {
    std::cerr << "ERROR: No data input file given! Unable to proceed!" << endl;
    cout << desc << endl;
    return 1;
  }

  if (vm.count("outfile")) {
    outfile = vm["outfile"].as<std::string>();
  }

  if (vm.count("refcode")) {
    refcode = vm["refcode"].as<std::string>();
  }

  if (vm.count("projection")) {
    projection = vm["projection"].as<std::string>();
  }

  if (vm.count("beamtype")) {
    beamType = vm["beamtype"].as<std::string>();
  }

  if (vm.count("coord_type")) {
    coordType = vm["coord_type"].as<std::string>();
  }

  //__________________________________________________________________
  // Attempt to open the input data-set

  CR::DataReader *dr = new CR::DataReader;

  if (setDataReader(dr,infile,blocksize)) {
    cout << "[imager] Successfully opened data-set into DataReader." << endl;
    cout << "-- blocksize = " << dr->blocksize() << endl;
  } else {
    cerr << "[imager] Failed to open data-set!" << endl;
    return 1;
  }

  // -----------------------------------------------------------------
  // Report control parameters

#ifdef DEBUGGING_MESSAGES
  cout << "-- Input data file           = " << infile         << endl;
  cout << "-- Output image file         = " << outfile        << endl;
  cout << "-- blocksize                 = " << blocksize      << endl;
  cout << "-- Blocks per time-frame     = " << blocksPerFrame << endl;
  cout << "-- nof. time frames          = " << nofFrames      << endl;
  cout << "-- Celestial reference frame = " << refcode        << endl;
  cout << "-- Spherical map projection  = " << projection     << endl;
  cout << "-- Beamtype for imaging      = " << beamType       << endl;
#endif

  return status;
}
