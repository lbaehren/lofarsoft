/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <IO/toLOPESgrid.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  toLOPESgrid::toLOPESgrid (){
    starttime_p = -0.4096e-3;
    stoptime_p  =  0.4096e-3;
    samplingfreq_p = 80e6;
    reset();
  };
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  toLOPESgrid::~toLOPESgrid ()
  {
    destroy();
  }
  
  void toLOPESgrid::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void toLOPESgrid::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool toLOPESgrid::reset(Int nAntennas){
    try {
      Int i;
      output_length_p = ifloor((stoptime_p-starttime_p)*samplingfreq_p+0.5);
      //reset the data matrix
      data_p.resize(output_length_p,nAntennas);
      data_p = 0.;
      //recalculate the times-vector
      times_p.resize(output_length_p);
      for (i=0; i<output_length_p; i++){
	times_p(i) = starttime_p+i/samplingfreq_p;
      }
    } catch (AipsError x) {
      cerr << "toLOPESgrid::reset: " << x.getMesg() << endl;
      return False;
    }; 
    return True; 
  };
  
  Bool toLOPESgrid::AddFFTData(Matrix<DComplex> inFFT, Int blocksize, Vector<Double> frequency,
			       Bool Centered, Double begintime){
    try {
      FFTServer<Double,DComplex> fftserver(IPosition(1,blocksize),
					FFTEnums::REALTOCOMPLEX);
      Int i,ant;
      Int niAnts, niFreq;
      Vector<Double> itimes,tmpTimeSeries,toAdd,tmpvec;
      Double timestep;
      
      niAnts = inFFT.ncolumn();
      niFreq = inFFT.nrow();
      if (niAnts != data_p.ncolumn()){
	cerr << "toLOPESgrid::AddFFTData: " << "Different number of channels in input and stored data!" 
	     << endl;
	return False;
      };
      if (niFreq != frequency.nelements()){
	cerr << "toLOPESgrid::AddFFTData: " << "Inconsistent data: inFFT.nrow() != len(frequency)" 
	     << endl;
	return False;
      };
      //calculate the input time axis
      timestep = 1./(max(frequency)*2.);
      if (Centered) { begintime = -0.5*timestep*blocksize; };
      itimes.resize(blocksize);
      for (i=0; i<blocksize; i++){
	itimes(i) = begintime+i*timestep;
      };
      tmpTimeSeries.resize(blocksize);
      for (ant=0; ant<niAnts; ant++){
	fftserver.fft(tmpTimeSeries,inFFT.column(ant));
	InterpolateArray1D<Double, Double>::interpolate(toAdd, times_p, itimes, tmpTimeSeries,
							InterpolateArray1D<Float,Double>::nearestNeighbour);
	tmpvec.reference(data_p.column(ant));
	tmpvec += toAdd;
      };
	
    } catch (AipsError x) {
      cerr << "toLOPESgrid::AddFFTData: " << x.getMesg() << endl;
      return False;
    }; 
    return True; 
    
  };

} // Namespace CR -- end
