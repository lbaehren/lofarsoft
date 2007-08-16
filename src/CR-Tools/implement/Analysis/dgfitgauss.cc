// gfitgaus.cc:  a glish client for the gaussian fitter
//# Copyright (C) 1995,1996,2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office   }

//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: dgfitgauss.cc,v 1.4 2007/06/21 16:07:31 horneff Exp $
//#--------------------------------------------------------------------------

#include <Analysis/dgfitgauss.h>

namespace CR { // Namespace CR -- begin
  
  // ------------------------------------------------------------- Construction
  
  dfitgauss::dfitgauss()
  {;}
  
  // -------------------------------------------------------------- Destruction
  
  dfitgauss::~dfitgauss()
  {;}
    

  // -------------------------------------------------------------- Methods

  Record dfitgauss::fitXY(Vector<Double> &x, Vector<Double> &y, Vector<Double> sigma, Bool verbose){
    Record result;
    try {
      IPosition shape;
      
      if (x.nelements() != y.nelements()){
	cerr << "dfitgauss::fitXY: " << "invalid parameters: x.nelements() != y.nelements()" << endl;
	return Record();
      }
      
      if (sigma.nelements() != y.nelements()){
	if (verbose) {cout << "dfitgauss::fitXY: " << "Using equal weights" << endl;}
	sigma.resize(x.nelements());
	sigma = 1.0;
      };

      // set up the fitter using the state information
      if (!state_.setUpFitter()) {
	cerr << "dfitgauss::fitXY: " << "initial guesses are inconsistent" << endl;
	return Record();
      }
      
      // now perform the fit
      Vector<Double> solution = state_.fitter().fit(x, y, sigma);

      state_.fitter().setParameterValues(solution);
      
      state_.getStateFromFitter(x.nelements());
      
      // reformate the state
      result = state_.state();

    } catch (AipsError x) {
      cerr << "dfitgauss::fitXY: " << x.getMesg() << endl;
      return Record();
    }; 
    return result;
} // fit xy

//----------------------------------------------------------------------------

  Vector<Double> dfitgauss::evalGaussian(Vector <Double> x){
    Vector<Double> y;
    try {
      uInt size = x.nelements ();
      y.resize(size);
      
      // set up the fitter using the state information
      if (!state_.setUpFitter()) {
	cerr << "dfitgauss::evalGaussian: " << "parameters are inconsistent" << endl;
	return Vector<Double>();
      }
      
      // and evaluate the function in the fitter
      Function<AutoDiff<Double> > *sumfunc = state_.fitter().fittedFunction();
      for (uInt i=0;i<size;i++) y(i) = (*sumfunc)(x(i)).value();
      
    } catch (AipsError x) {
      cerr << "dfitgauss::fitXY: " << x.getMesg() << endl;
      return Vector<Double>();
    };     
    return y;
}      
                                              
//----------------------------------------------------------------------------



} // Namespace CR -- end
