/*-------------------------------------------------------------------------*
 | $Id:: TimeFreqSkymap.cc 1967 2008-09-09 13:16:22Z baehren             $ |
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

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/detail/cmdline.hpp>

namespace bpo = boost::program_options;

// ------------------------------------------------------------------------------

/*!
  \file boostProgramOptions.cc

  \ingroup contrib

  \brief Collection of tests for working with Boost program_options library

  <h3>Usage</h3>

  <ol>
    <li>Get a list of the available command line optins which can be provided to
    the application:
    \verbatim
    boostProgramOptions --help
    \endverbatim
    Currently the following command line options are available:
    \verbatim
    Allowed options:
    --help                Show help messages
    --blocksize arg       Number of samples per block of data
    --nof_frames arg      Number of frames
    --refcode arg         Reference code for celestial coordinates
    --projection arg      Spherical map projection
    \endverbatim
    <li>User-specified values for the blocksize and the reference code for the 
    celestial reference frame:
    \verbatim
    boostProgramOptions --blocksize 2048 --refcode B1950
    \endverbatim
  </ol>
*/

// ------------------------------------------------------------------------------

int main (int argc,
	  char* argv[])
{
  int result (0);

  std::string infile;
  std::string outfile    = "skymap.h5";
  int blocksize          = 1024;
  int nof_frames         = 1;
  std::string refcode    = "AZEL";
  std::string projection = "STG";
  
  /* Set up the list of options accepted on the command line */

  bpo::options_description desc ("Supported command-line options");

  desc.add_options ()
    ("help", "Show help messages")
    ("infile", bpo::value<std::string>(), "Input data set to process")
    ("outfile", bpo::value<std::string>(), "Output file with the processing results")
    ("blocksize", bpo::value<int>(), "Number of samples per block of data")
    ("nof_frames", bpo::value<int>(), "Number of frames")
    ("refcode", bpo::value<std::string>(), "Reference code for celestial coordinates")
    ("projection", bpo::value<std::string>(), "Spherical map projection")
    ;

  /* Parse the command line parameters and store them */

  bpo::variables_map vm;
  bpo::store (bpo::parse_command_line(argc,argv,desc), vm);

  /* Retrieve the values provided on the command line */

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return result;
  }
  
  if (vm.count("infile")) {
    infile = vm["infile"].as<std::string>();
  } else {
    std::cerr << "ERROR: Missing name of input file!" << std::endl;
    std::cerr << desc << std::endl;
    return result;
  }

  if (vm.count("outfile")) {
    outfile = vm["outfile"].as<std::string>();
  }

  if (vm.count("blocksize")) {
    blocksize = vm["blocksize"].as<int>();
  }

  if (vm.count("nof_frames")) {
    nof_frames = vm["nof_frames"].as<int>();
  }

  if (vm.count("refcode")) {
    refcode = vm["refcode"].as<std::string>();
  }

  if (vm.count("projection")) {
    projection = vm["projection"].as<std::string>();
  }

  /* Report variable values */
  
  std::cout << "[boostProgramOptions]" << std::endl;
  std::cout << "-- infile     = " << infile     << std::endl;
  std::cout << "-- outfile    = " << outfile    << std::endl;
  std::cout << "-- blocksize  = " << blocksize  << std::endl;
  std::cout << "-- nof_frames = " << nof_frames << std::endl;
  std::cout << "-- refcode    = " << refcode    << std::endl;
  std::cout << "-- projection = " << projection << std::endl;

  return result;
}
