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
#include <cmath>

#include <wcslib/lin.h>

/*!
  \file tUseWCS.cc
  
  \inroup CR_test

  \brief A simple test for external build against libwcs

  \author Lars B&auml;hren

  \date 2007/01/29

  Check if we can properly build against the WCS library.
  
  WCSLIB has an extensive test suite which also provides programming templates
  as well as demonstrations.  Test programs, with names that indicate the main
  WCSLIB routine under test, reside in ./test and each contains a brief
  description of its purpose.
*/

/*!
  \brief Testing WCSLIB linear transformation routines
  
  \return nofFailedTests -- The number of failed tests
 */
int test_wcs_lin ()
{
  int nofFailedTests (0);
  
  int i, j, k, status;
  int NAXIS  = 5;
  int NCOORD = 2;
  int NELEM  = 9;
  double CRPIX[5] =  {256.0, 256.0,  64.0, 128.0,   1.0};
  double PC[5][5] = {{  1.0,   0.5,   0.0,   0.0,   0.0},
		     {  0.5,   1.0,   0.0,   0.0,   0.0},
		     {  0.0,   0.0,   1.0,   0.0,   0.0},
		     {  0.0,   0.0,   0.0,   1.0,   0.0},
		     {  0.0,   0.0,   0.0,   0.0,   1.0}};
  double CDELT[5] =  {  1.2,   2.3,   3.4,   4.5,   5.6};
  
  double pix[2][9] = {{303.0, 265.0, 112.4, 144.5,  28.2, 0.0, 0.0, 0.0, 0.0},
		      { 19.0,  57.0,   2.0,  15.0,  42.0, 0.0, 0.0, 0.0, 0.0}};
  double img[2][9];
  
  std::cout << "[1] Testing WCSLIB linear transformation routines" << std::endl;
  try {
    double *pcij;
    struct linprm lin;
    
    for (status = 1; status <= 3; status++) {
      printf("%4d: %s.\n", status, lin_errmsg[status]);
    }
    
    lin.flag = -1;
    linini(1, NAXIS, &lin);
    
    pcij = lin.pc;
    for (i = 0; i < lin.naxis; i++) {
      lin.crpix[i] = CRPIX[i];
      
      for (j = 0; j < lin.naxis; j++) {
	*(pcij++) = PC[i][j];
      }
      
      lin.cdelt[i] = CDELT[i];
    }
    
    for (k = 0; k < NCOORD; k++) {
      printf("\nPIX %d:", k+1);
      for (j = 0; j < NAXIS; j++) {
	printf("%14.8f", pix[k][j]);
      }
    }
    printf("\n");
    
    if ((status = linp2x(&lin, NCOORD, NELEM, pix[0], img[0]))) {
      printf("linp2x ERROR %d\n", status);
      return 1;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  return nofFailedTests;
}

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  
  nofFailedTests += test_wcs_lin ();
  
  return nofFailedTests;
}
