/*-------------------------------------------------------------------------*
 | $Id:: tUseFFTW3.cc 613 2007-08-23 09:22:15Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <cmath>
#include <iostream>
#include <string>

#include <cpgplot.h>

/*!
  \file tUsePGPLOT.cc

  \ingroup CR-Pipeline

  \brief A simple test for working with the PGPlot library

  \author Lars B&auml;hren

  \date 2007/09/06
*/

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main (int argc,
	  char *argv[]) 
{
  int nofFailedTests (0);

  char *devname = "?";
//   int DIV (100);
//   int i;
//   float x[DIV], y[DIV];
  /* if(cpgbeg(0, "?", 1, 1) != 1)
     exit(EXIT_FAILURE); */
  cpgask(1);

  if (argc == 2) {
    devname = argv[1];
  }
  if(cpgopen(devname) != 1) {
    exit(-1);
  }

  // --------------------------------------------------------------------- Test 1
  
  std::cout << "[1] " << std::endl;
  try {
    cpgpap(6, 0.75);      /*  6inch, aspect ratio =1.0 (aquare)     */
//     cpgscr(0, 1, 1, 1);   /*  0: background (default is black)      */
//     cpgscr(1, 0, 0, 0);   /*  0: foreground (default is white)      */
//     cpgscf(1);            /*  1:normal, 2:roman, 3:italic, 4:script */
//     cpgsch(1.2);          /*                                        */
//     cpgslw(2);            /*  unit in 0.005 inch = 0.13 mm          */
//     cpgenv(-2*M_PI, 2*M_PI, -1, 1, 0, 0);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  // --------------------------------------------------------------------- Test 2
  
//   std::cout << "[2] PGPLOT :: Special functions :: Exponential" << std::endl;
//   try {
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }
  
  // --------------------------------------------------------------------- Test 3
  
//   try {
//   } catch (std::string message) {
//     std::cerr << message << std::endl;
//     nofFailedTests++;
//   }
  
  return nofFailedTests;
}
