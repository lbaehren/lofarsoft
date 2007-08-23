/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <fftw3.h>

/*!
  \file tUseFFTW3.cc

  \ingroup CR-Pipeline

  \brief A simple test for external build against libfftw3

  \author Lars B&auml;hren

  \date 2007/01/29

  
*/

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  
  // --------------------------------------------------------------------- Test 1
  
  std::cout << "[1] Complex One-Dimensional DFT" << std::endl;
  try {
    int N (256);
    fftw_complex *in;
    fftw_complex *out;
    fftw_plan p;
    
    in   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  // --------------------------------------------------------------------- Test 2
  
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
