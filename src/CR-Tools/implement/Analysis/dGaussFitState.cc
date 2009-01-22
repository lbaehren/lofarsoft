//# GaussFitState.cc : 
//# Copyright (C) 1995,1996,1999,2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <Analysis/dGaussFitState.h>

#include <casa/Arrays/Slice.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin

void dGaussFitState::setHeight(const Vector<Double> &h) 
{
    height_p.resize(h.nelements());
    height_p = h;
}

void dGaussFitState::setCenter(const Vector<Double> &c) 
{
    center_p.resize(c.nelements());
    center_p = c;
}

void dGaussFitState::setWidth(const Vector<Double> &w) 
{
    width_p.resize(w.nelements());
    width_p = w;
}

void dGaussFitState::clear_self()
{
    height_p.resize(0);
    center_p.resize(0);
    width_p.resize(0);
    hgterr_p.resize(0);
    cnterr_p.resize(0);
    widerr_p.resize(0);
    maxIter_p = fitter_p.getMaxIter();
    criteria_p = fitter_p.getCriteria();
    chisq_p = 0.;
    ndf_p = 1;
}

Bool dGaussFitState::setUpFitter()
{
    // all the parameters must have the same number of
    // non-zero elements
    if (height_p.nelements() == 0 ||
	height_p.nelements() != width_p.nelements() ||
	height_p.nelements() != center_p.nelements()) {
	return False;
    }


    AutoDiff<Double> ah(0,3,Gaussian1D<Double>::HEIGHT), 
	             ac(0,3,Gaussian1D<Double>::CENTER), 
	             aw(0,3,Gaussian1D<Double>::WIDTH);

    // Form a combination of functions.  The parameters of the function
    // is the aggregate of the parameters of every function in the sum.

    CompoundFunction<AutoDiff<Double> > sumfunc;

    for (uInt i=0;i<height_p.nelements();i++) {
	ah.value() = height_p(i);
	ac.value() = center_p(i);
	aw.value() = width_p(i);
	Gaussian1D<AutoDiff<Double> > gauss(ah, ac, aw);
	sumfunc.addFunction(gauss);
    }

    // designate the function to be fitted.
    fitter_p.setFunction(sumfunc);

    // parameters already have default values.  No need to give them
    // initial guessed values. Otherwise, need to call setFittedFuncParams(...)

    // set the errors to be zero
    hgterr_p.resize(height_p.nelements());
    hgterr_p = 0.0;
    cnterr_p.resize(center_p.nelements());
    cnterr_p = 0.0;
    widerr_p.resize(width_p.nelements());
    widerr_p = 0.0;

    fitter_p.setMaxIter(maxIter_p);
    fitter_p.setCriteria(criteria_p);
    return True;
}

void dGaussFitState::getStateFromFitter(Int NumPoints) {
    // extract the parameters
  ///    Vector<Double> parms(fitter().fittedFunction()->getAdjustParameters());
    Vector<Double> values(fitter().fittedFunction()->nparameters());
    for (uInt i=0; i<values.nelements();i++) {
      values[i] = (*(fitter().fittedFunction()))[i].value();
    };

    height_p = values(Slice(0,height_p.nelements(),3));
    center_p = values(Slice(1,center_p.nelements(),3));
    width_p = values(Slice(2,width_p.nelements(),3));

    // and the errors from the covariance matrix

    Matrix<Double> covar(fitter_p.compuCovariance());
    Vector<Double> diag(fitter_p.compuCovariance().diagonal());

    // only do this if all diag elements are >= 0
    if (allGE(diag, Double(0.0))) {
	diag = sqrt(diag);
	hgterr_p = diag(Slice(0,hgterr_p.nelements(),3));
	cnterr_p = diag(Slice(1,cnterr_p.nelements(),3));
	widerr_p = diag(Slice(2,widerr_p.nelements(),3));
    } else {
	// otherwise zero them all out
	hgterr_p = 0.0;
	cnterr_p = 0.0;
	widerr_p = 0.0;
    }
    
    chisq_p = fitter_p.chiSquare();
    ndf_p = NumPoints-(height_p.nelements()*3);
    if (ndf_p <=0) ndf_p=1;

    Double factor = sqrt( chisq_p / ndf_p);

    hgterr_p *= factor;
    cnterr_p *= factor;
    widerr_p *= factor;
    
    maxIter_p = fitter_p.getMaxIter();
    criteria_p = fitter_p.getCriteria();
}

Record dGaussFitState::state() const 
{
    Record rec;

    // see if this might work

    rec.define(String("converged"),fitter_p.converged());
    rec.define(String("currentIteration"),Int(fitter_p.currentIteration()));
    rec.define(String("maxIter"), Int(maxIter_p));
    rec.define(String("criteria"), criteria_p);
    if (height_p.nelements()) {
	rec.define(String("height"),height_p);
    }
    if (center_p.nelements()) {
	rec.define(String("center"),center_p);
    } 
    if (width_p.nelements()) {
	rec.define(String("width"),width_p);
    }
    if (hgterr_p.nelements()) {
	rec.define(String("height_error"), hgterr_p);
    }
    if (cnterr_p.nelements()) {
	rec.define(String("center_error"), cnterr_p);
    }
    if (widerr_p.nelements()) {
	rec.define(String("width_error"), widerr_p);
    } 
    if (chisq_p != 0.) {
      rec.define("chisq",chisq_p);
      rec.define("ndf",ndf_p);
    }

    return rec;
}

void dGaussFitState::setState(const Record& rec) 
{
  if (rec.isDefined("maxIter")) {
    Int imax;
    imax = rec.asInt("maxIter");
    if (imax >= 0) {
      maxIter_p = uInt(imax);
    } 
  }
  if (rec.isDefined("criteria")) {
    criteria_p = rec.asFloat("criteria");
  }
  if (rec.isDefined("height")) {
    height_p = rec.toArrayDouble("height");
  }
  if (rec.isDefined("center")) {
    center_p = rec.toArrayDouble("center");
  }
  if (rec.isDefined("width")) {
    width_p = rec.toArrayDouble("width");
  } 
}

} // Namespace CR -- end
