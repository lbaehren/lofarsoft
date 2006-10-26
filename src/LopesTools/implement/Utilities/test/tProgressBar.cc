/***************************************************************************
 *   Copyright (C) 2004-2005                                               *
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

/* $Id: tProgressBar.cc,v 1.1 2005/12/09 01:21:26 bahren Exp $ */

#include <cmath>

#include <Utilities/ProgressBar.h>

/*!
  \file tProgressBar.cc

  \brief A collection of test routines for ProgressBar.

  \author Lars B&auml;hren

  <h3>Test results</h3>

  <img src="../figures/runningTime.png">
  <img src="../figures/totalTime.png">
  <img src="../figures/remainTime.png">
*/

// --- Global variables --------------------------------------------------------

const int nofIterations (20);

// --- Test computation of time estimate ---------------------------------------

/*!
  \brief Test computation of estimated time.
*/
void testTimer2 () {
  
  cout << "\n[tProgressBar::testTimer2]\n" << endl;
  
  clock_t start, end;
  int nofComputations (10000000);
  double diff;
  double x;
  
  start = clock();
  
  for (int i=0; i<nofIterations; i++) {
    //
    for (int j=0; j<nofComputations; j++) {
      x = std::sin(std::sin(1.0*j));
    }
    //
    end = clock();
    diff = ((double)( end - start ) / (double)CLOCKS_PER_SEC );
    cout << "\t"
	 << i << "\t"
	 << end << "\t"
	 << diff << "\t"
	 << diff*nofIterations/(i+1.0) << "\t"
	 << diff*(nofIterations/(i+1.0)-1.0) << endl;
  }
  
}

/*!
  \brief Test computation of estimated time.
*/
void testTimer () {

  cout << "\n[tProgressBar::testTimer]\n" << endl;
  
  struct timeval timeval_p;
  int nofComputations (10000000);
  int diff;
  int starttime;
  double x;

  gettimeofday(&timeval_p, NULL);
  starttime = timeval_p.tv_sec;  

  for (int i=0; i<nofIterations; i++) {
    //
    for (int j=0; j<nofComputations; j++) {
      x = std::sin(std::sin(1.0*j));
    }
    //
    gettimeofday(&timeval_p, NULL);
    diff = timeval_p.tv_sec-starttime;
    cout << "\t"
	 << i << "\t"
	 << timeval_p.tv_sec << "\t"
	 << diff << "\t"
	 << diff*nofIterations/(i+1.0) << "\t"
	 << diff*(nofIterations/(i+1.0)-1.0) << endl;
  }
  
}

// --- Display -----------------------------------------------------------------

/*!
  \brief Draw a previously constructed ProgressBar
  
  \param bar             -- A progress bar
  \param nofComputations -- The number of computation loops.
*/
void drawProgressBar (ProgressBar& bar,
		      const int& nofComputations) {

  int nofLoops;
  double x;

  nofLoops = bar.maximumValue();
  
  for (int i=0; i<nofLoops; i++) {
    for (int j=0; j<nofComputations; j++) {
      x = std::sin(std::sin(1.0*j));
    }
    bar.update(i+1);
  }
  
}

// --- Construction ------------------------------------------------------------

/*!
  \brief Test constructors

  \param loops           -- Total number of loops for which the bar is drawn.
  \param length          -- Length of the bar, in characters.
  \param nofComputations -- Number of computations to perform, when drawing
                            the bar.
*/
int test_ProgressBar (const int& loops,
		      const int& length, 
		      const int& nofComputations) 
{
  int nofFailedTests (0);

  cout << "\n[tProgressBar] Testing constructors.\n" << endl;

  cout << "[1] ProgressBar (int)" << endl;
  {
    ProgressBar bar = ProgressBar (loops);
    bar.showTime (false);
    //
    drawProgressBar (bar,nofComputations);
  }

  cout << "[2] ProgressBar (int,int)" << endl;
  {
    ProgressBar bar = ProgressBar (loops,length);
    bar.showTime (false);
    //
    drawProgressBar (bar,nofComputations);
  }

  return nofFailedTests;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main ()
{
  int nofFailedTests (0);
  int loops = 100;
  int length = 60;
  int nofComputations = 100000;


  {
    testTimer ();
    testTimer2 ();
  }

  {
    nofFailedTests += test_ProgressBar (loops, length, nofComputations);
  }

  {
    cout << "\n[tProgressBar] Testing change of bar symbol." << endl;
    //
    ProgressBar bar = ProgressBar (loops);
    //
    drawProgressBar (bar,nofComputations);
    //
    bar.setSymbol("/");
    drawProgressBar (bar,nofComputations);
    //
    bar.setSymbol("%");
    drawProgressBar (bar,nofComputations);
  }

  {
    cout << "\n[tProgressBar] Testing change of bar width." << endl;
    //
    ProgressBar bar = ProgressBar (loops);
    //
    drawProgressBar (bar,nofComputations);
    //
    bar.setBarwidth(60);
    drawProgressBar (bar,nofComputations);
    //
    bar.setBarwidth(50);
    drawProgressBar (bar,nofComputations);
  }

  {
    cout << "\n[tProgressBar] Testing display of remain time." << endl;
    //
    ProgressBar bar = ProgressBar (loops);
    //
    drawProgressBar (bar,nofComputations);
    //
    bar.showTime(true);
    //
    nofComputations *= 5;
    drawProgressBar (bar,nofComputations);
    //
    nofComputations *= 5;
    drawProgressBar (bar,nofComputations);
  }

  return 0;

}
