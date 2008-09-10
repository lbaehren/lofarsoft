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
#include <Imaging/Skymapper.h>

using std::cout;
using std::endl;

namespace bpo = boost::program_options;

/*!
  \file skymapper.cc

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
  </table>

  <h3>Examples</h3>

  <ul>
    <li>Show the available command line options:
    \verbatim
    skymapper --help
    \endverbatim
    <li>Use default setting for the generations of a skymap:
    \verbatim
    skymapper --infile 20080604_122217_2300.dat.h5
    \endverbatim
    <li>Generate skymap using a specific celestial reference frame:
    \verbatim
    skymapper --infile 20080604_122217_2300.dat.h5 --refcode J2000
    \endverbatim
  </ul>
*/

int main (int argc, char *argv[])
{
  int status (0);

  // -----------------------------------------------------------------
  // Parsing of the command-line parameters

  std::string infile     = "";
  std::string outfile    = "skymap.h5";
  int blocksize          = 1024;
  int blocksPerFrame     = 1;
  int nofFrames          = 1;
  std::string refcode    = "AZEL";
  std::string projection = "STG";

  bpo::options_description desc ("[skymapper] Available command line options");

  desc.add_options ()
    ("help", "Show help messages")
    ("infile", bpo::value<std::string>(),
     "Input data file from which to generate the image")
    ("outfile", bpo::value<std::string>(),
     "Output image file.")
    ("blocksize", bpo::value<int>(),
     "Size of an individual block of data read from file, [samples].")
    ("blocks_per_frame", bpo::value<int>(),
     "Number of input data blocks combined into an output time-frame.")
    ("frames", bpo::value<int>(),
     "Number of time-frames in generated image.")
    ("refcode", bpo::value<std::string>(),
     "Reference code for the celestial coordinate system.")
    ("projection", bpo::value<std::string>(),
     "Reference code for the spherical map projection.")
    ;
  
  bpo::variables_map vm;
  bpo::store (bpo::parse_command_line(argc,argv,desc), vm);
  
  if (vm.count("help")) {
    std::cout << desc << std::endl;
  }
  
  if (vm.count("blocksize")) {
    blocksize = vm["blocksize"].as<int>();
  }

  if (vm.count("frames")) {
    nofFrames = vm["frames"].as<int>();
  }

  if (vm.count("infile")) {
    infile = vm["infile"].as<std::string>();
  } else {
    infile = "UNDEFINED";
    std::cerr << "ERROR: No data input file given! Unable to proceed!" << endl;
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

  // -----------------------------------------------------------------
  // Report control parameters

  cout << "-- Input data file           = " << infile         << endl;
  cout << "-- Output image file         = " << outfile        << endl;
  cout << "-- blocksize                 = " << blocksize      << endl;
  cout << "-- Blocks per time-frame     = " << blocksPerFrame << endl;
  cout << "-- nof. time frames          = " << nofFrames      << endl;
  cout << "-- Celestial reference frame = " << refcode        << endl;
  cout << "-- Spherical map projection  = " << projection     << endl;

  return status;
}
