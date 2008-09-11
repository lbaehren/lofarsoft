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

int main (int argc,
	  char* argv[])
{
  int result (0);

  int blocksize          = 1024;
  int nof_frames         = 1;
  std::string refcode    = "AZEL";
  std::string projection = "STG";
  
  /* Set up the list of options accepted on the command line */

  bpo::options_description desc ("Allowed options");

  desc.add_options ()
    ("help", "Show help messages")
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
  std::cout << "-- blocksize  = " << blocksize  << std::endl;
  std::cout << "-- nof_frames = " << nof_frames << std::endl;
  std::cout << "-- refcode    = " << refcode    << std::endl;
  std::cout << "-- projection = " << projection << std::endl;

  return result;
}
