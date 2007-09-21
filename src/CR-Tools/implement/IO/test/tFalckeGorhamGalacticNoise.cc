/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Tim Huege (tim.huege@ik.fzk.de)                                       *
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

#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <casa/Arrays.h>
#include <casa/aips.h>
#include <casa/BasicMath/Random.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <IO/FrequencyNoise.h>
#include <IO/FalckeGorhamGalacticNoise.h>
#include <scimath/Mathematics/FFTServer.h>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using CR::FalckeGorhamGalacticNoise;

/*!
  \file tFalckeGorhamGalacticNoise.cc
  
  \ingroup IO
  
  \brief A collection of test routines for FalckeGorhamNoise.

         If you want to test the normalisation of FalckeGorhamGalacticNoise,
	 please set the noise temperature in FalckeGorhamGalacticNoise.cc to
	 a constant value, e.g. 2500 Kelvin.
  
  \author Tim Huege
  
  \date 2007/09/20
  
*/

// -----------------------------------------------------------------------------

int main ()
{
  FalckeGorhamGalacticNoise Test;

  const long numpoints= 137;
  const double freqstep = 3.6e4;
  const double maxfreq = (numpoints-1)*freqstep;
  const double timestep = 2./maxfreq;
  const double noisetemp = 2500;	// 2500 Kelvin
  const double impedance = 50.0;	// 50 Ohm
  const double kB = 1.3806503e-23;

  Vector<Double> frequencies(numpoints);
  Vector<Double> timeseries;
  Vector<DComplex> noise;

  for (long i=0; i<numpoints; ++i) { frequencies(i)=i*freqstep; }
  Test.getNoise(frequencies,noise);
  
  for (long i=0; i<noise.nelements(); ++i)
  {
    cout << frequencies(i) << "\t" << noise(i) << "\n";
  }

  FFTServer<Double,DComplex> server(IPosition(1,2*noise.nelements()-2),FFTEnums::REALTOCOMPLEX);
  server.fft(timeseries,noise);
  cout << endl;

  double timeintegral = 0.0;
  for (long i=0; i<timeseries.nelements(); ++i)
  {
    cout << timeseries(i) << "\n";
    timeintegral+=timeseries(i)*timeseries(i)/impedance*timestep;
  }
  
  double theoreticalintegral = kB*noisetemp*maxfreq;
  
  cout << "\nTheoretical frequency integral (kB*T*bandwidth for fixed T): " << theoreticalintegral
       << "\nCalculated time integral: " << timeintegral << "\t which is factor " << timeintegral/theoreticalintegral << " of expected value."
       << endl;

  
  return 0;
}
