
/*-------------------------------------------------------------------------*
 | $Id:: cccheck.cc 2009-07-02 swelles        $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen s.terveen@astro.ru.nl)  			                       *
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

#include <crtools.h>
#include <Data/LopesEventIn.h>
#include <Display/SimplePlot.h>
#include <Data/LOFAR_TBB.h>

using CR::SimplePlot;

/*!
	\file cccheck.cc
	
	\ingroup CR_Applications
	
	\brief Make a cross-correlation between 2 antennas and write out a ps-file of it, but now for HDF5 data. To be merged later with ccheck.cc

	\author Sander ter Veen
	
	\date 2009/07/02
*/

// -----------------------------------------------------------------------------

/*!
  \brief Read in a file, make the cc, and write out a ps.

  \param infile       -- first LOFAR data set to use as data input
  \param infile2      -- second LOFAR data set to use as data input 
  \param outfile      -- (path-)name of the image-file to be generated

  \return nofFailedTests --  The number of failed tests encountered within this
          function (no tests implemented).
*/
int ccmaker(string const &infile1, string const &infile2, string const &outfile, int const &blocksize, int const &offset1, int zoom=40)//, int const &offset2)
{
	int nofFailedTests     = 0;
	LOFAR_TBB *dr1;
	LOFAR_TBB *dr2;
	dr1 = new LOFAR_TBB(infile1,blocksize);
	dr2 = new LOFAR_TBB(infile2,blocksize);
	//second
	int start1 = dr1->sample_number()[0];
	dr1->setBlock(offset1);
	int start2 = dr2->sample_number()[0];
	int offset2=offset1;//-(start2-start1)/blocksize;
	int difference=start1-start2;
	dr2->setShift(difference); //If the shifts are not the same they probably are allready set.
	//if(difference > blocksize/2) { difference-=blocksize; }
	std::cout << " start1 " << start1 <<" ; start2 " << start2 << " ; offset1 " << offset1 << " ; offset2 " << offset2 << " ; difference " << difference << "\n";
	
    Matrix<DComplex> fftdata1 = dr1->calfft();
	fftdata1(0,0)=(0,0);
	std::cout << fftdata1(0,0);
	dr2->setBlock(offset2);
	Matrix<DComplex> fftdata2 = dr2->calfft();
	fftdata2(0,0)=(0,0);
	std::cout << fftdata2(0,0);
	Vector<DComplex> vecdata;
	vecdata = fftdata1.column(0) * conj(fftdata2.column(0));
	
/*	std::cout <<"length of vecdata = " << vecdata.shape() << "\t" <<
				"length of fftdata.column(0) = " << fftdata.column(0).shape() << "\t" <<
				"size of fftdata = " << fftdata.shape() << endl;
*/	
	Vector<Double> ccdata;
	
	
	ccdata = dr1->invfft(vecdata);
	Vector<Double> xvals;
	xvals = dr1->timeValues();

	Vector<Double> empty;
	int timesteps =zoom;//xvals.shape()[0];
	Vector<Double> xx(timesteps);
	Vector<Double> yy(timesteps);
	
	int startcounting = (int(ccdata.shape()[0]-timesteps)/2);
	for (int i=0; i < timesteps; i++){
		xx[i] = xvals[i+startcounting]-(offset1+0.5)*blocksize*0.000000005;
		yy[i] = ccdata[i+startcounting];
	}
	std::cout << "x_values = " << xx << std::endl;
	std::cout << "y_values_" << offset1 << " = " << yy << std::endl;
	SimplePlot myplotter;
	std::string shortinfile1(infile1,39,11);
	std::string shortinfile2(infile2,39,11);
	std::string title = "correlation between " + shortinfile1 + " and \n" + shortinfile2;
	myplotter.quickPlot(outfile, xx, yy, empty, empty, "time-displacement (s)", "correlation", title);
 /*newObject.quickPlot("tSimplePlot-line.ps", xval, yval, empty, empty,
    			"X-axis", "Y-Axis", "plotting-test with lines",
    			4, True, 1, False, False, True);
*/
	return nofFailedTests;
}


int main (int argc,
	  char *argv[])
{
  
  uint nofFailedTests=0;
  std::string infile1, infile2, outfile="ccimage3.ps";
	int blocksize=1024, offset1=1, zoom=40;
  /*
    Check if filename of the dataset is provided on the command line;
  */
  if (argc < 3) {
    std::cout << "No input dataset given. Using the example.event" << endl;
	infile1 = "/Users/STV/Astro/data/lightning/CS302C-B0T16:48:58.h5";
	infile2 = "/Users/STV/Astro/data/lightning/CS302C-B0T16:48:58.h5";
  } else {
    infile1 = argv[1];
	infile2 = argv[2];  
     if (argc > 3) {
       outfile  = argv[3];
	 }
	 if (argc > 4) {
		blocksize = atoi(argv[4]);
	  }
	  if (argc > 5) {
	   offset1 = atoi(argv[5]);
	 }
	  if (argc > 6) {
	   zoom = atoi(argv[6]);
	}
  		 
  };
	
  

	std::cout << outfile << "\n";
   	nofFailedTests += ccmaker (infile1, infile2, outfile, blocksize, offset1, zoom);//, offset2);
  
  return nofFailedTests;
}
