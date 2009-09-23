/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

/* Boost header files */
#include <boost/program_options.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/detail/cmdline.hpp>

/* Namespace usage */
namespace bpo = boost::program_options;

//_______________________________________________________________________________
//                                                           test_program_options

int test_programm_options ()
{
  int status (1);
  
  bpo::options_description desc ("[TestBoost] Available command line options");
  
  desc.add_options ()
    ("help,H", "Show help messages")
    ("infile,I", bpo::value<std::string>(),
     "Input data file from which to generate the image")
    ("blocksize,B", bpo::value<unsigned int>(),
     "Size of an individual block of data read from file, [samples].")
    ("blocks_per_frame", bpo::value<int>(),
     "Number of input data blocks combined into an output time-frame. (1)")
    ;
  
//   bpo::variables_map vm;
//   bpo::store (bpo::parse_command_line(argc,argv,desc), vm);
  
  return status;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
	  char *argv[])
{
  int result (1);
  std::string message;
  
  /* Test module for handling command line options */
  result =  test_programm_options ();
  
  return result;
}
