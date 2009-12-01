/*-------------------------------------------------------------------------*
 | $id:: cccheck.cc 2009-07-02 swelles                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sef Welles swelles@science.ru.nl)  		                   *
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

#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <lattices/Lattices/LatticeExprNode.h>

using casa::Array;
using casa::conj;

/* CR-Tools header files */
#include <crtools.h>
#include <Data/LopesEventIn.h>
#include <Utilities/SimplePlot.h>

using CR::SimplePlot;

/*!
	\file cccheck.cc
	
	\ingroup CR_Applications
	
	\brief Make a cross-correlation between 2 antennas and write out a ps-file of it

	\author Sef Welles
	
	\date 2009/07/02
*/

// -----------------------------------------------------------------------------

/*!
  \brief Read in a file, make the cc, and write out a ps.

  \param infile       -- LOPES data set to use as data input
  \param outfile      -- (path-)name of the image-file to be generated

  \return nofFailedTests --  The number of failed tests encountered within this
          function (no tests implemented).
*/
int ccmaker (string const &infile,
	     string const &outfile)
{
  int nofFailedTests     = 0;
  CR::LopesEventIn *dr;
  dr = new CR::LopesEventIn(infile);
  Matrix<DComplex> fftdata = dr->calfft();
  
  
  Vector<DComplex> vecdata;
  vecdata = fftdata.column(7) * conj(fftdata.column(4));
  
/*	std::cout <<"length of vecdata = " << vecdata.shape() << "\t" <<
				"length of fftdata.column(0) = " << fftdata.column(0).shape() << "\t" <<
				"size of fftdata = " << fftdata.shape() << endl;
*/	
  Vector<Double> ccdata;
  
  
  ccdata = dr->invfft(vecdata);
  Vector<Double> xvals;
  xvals = dr->timeValues();
  
  Vector<Double> empty;
  int timesteps = 100;
  Vector<Double> xx(timesteps);
  Vector<Double> yy(timesteps);
  
  int startcounting = (int(ccdata.shape()[0]-timesteps)/2);
  for (int i=0; i < timesteps; i++){
    xx[i] = xvals[i+startcounting];
    yy[i] = ccdata[i+startcounting];
  }
  
  SimplePlot myplotter;
  myplotter.quickPlot(outfile, xx, yy, empty, empty,
		      "time-displacement (s)", "correlation", "correlation between antennas 8 and 5 in example.event");
  /*newObject.quickPlot("tSimplePlot-line.ps", xval, yval, empty, empty,
    			"X-axis", "Y-Axis", "plotting-test with lines",
    			4, True, 1, False, False, True);
*/
	return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
	  char *argv[])
{
  
  uint nofFailedTests=0;
  std::string infile, outfile="ccimage.ps";

  /*
    Check if filename of the dataset is provided on the command line;
  */
  if (argc < 2) {
    std::cout << "No input dataset given. Using the example.event" << endl;
	infile = "../../../data/lopes/example.event";
  } else {
    infile = argv[1];
     if (argc > 2) {
       outfile  = argv[2];
     };
  }
  
  nofFailedTests += ccmaker (infile, outfile);
  
  return nofFailedTests;
}
