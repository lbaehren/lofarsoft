/*-------------------------------------------------------------------------*
 | $Id: fitCR2gauss.cc,v 1.4 2007/08/03 12:27:24 horneff Exp $ |
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

#include <Analysis/fitCR2gauss.h>
#include <Analysis/dgfitgauss.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  fitCR2gauss::fitCR2gauss (){
    fitRange_p.resize(2);
    fitRange_p = 0;
    remoteRange_p.resize(2);
    remoteRange_p = 0;
    defCenter_p = -1.8e-6;
    defWidth_p = 1e-7;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  fitCR2gauss::~fitCR2gauss ()
  {
    destroy();
  }
  
  void fitCR2gauss::destroy ()
  {;}
  
  void fitCR2gauss::summary (std::ostream &os)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  Bool fitCR2gauss::setTimeAxis(Vector<Double> newTimeAxis){
    timeAxis_p.resize(newTimeAxis.shape());
    timeAxis_p = newTimeAxis;
    return True;
  };
  
  Bool fitCR2gauss::setFitRange(Vector<uInt> newrange){
    if (newrange.nelements()!=2){
      cerr << "fitCR2gauss::setFitRange: " << "Input needed as: [startsample,stopsample] !" << endl;
      return False;
    };
    if ((newrange(0)<0)||(newrange(1)<=newrange(0))||(newrange(1)>timeAxis_p.nelements())){
      cerr << "fitCR2gauss::setFitRange: " << "Parameter out of range!" << endl;
      return False;
    };
    // All clear, so set the values
    fitRange_p = newrange;
    return True;
  };

  Bool fitCR2gauss::setFitRangeSeconds(Double starttime, Double stoptime){
    try {
      uInt start,stop;
      start = ntrue(timeAxis_p<starttime);
      stop = ntrue(timeAxis_p<=stoptime);
      if (!(start<stop)){
	cerr << "fitCR2gauss::setFitRangeSeconds: " << "Invalid parameters: start>=stop" << endl;
	return False;	
      };
      // All clear, so set the values      
      fitRange_p.resize(2);
      fitRange_p(0) = start; fitRange_p(1) = stop; 
    } catch (AipsError x) {
      cerr << "fitCR2gauss::setFitRangeSeconds: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };
  
  Bool fitCR2gauss::setRemoteRange(Vector<uInt> newrange){
    if (newrange.nelements()!=2){
      cerr << "fitCR2gauss::setRemoteRange: " << "Input needed as: [startsample,stopsample] !" << endl;
      return False;
    };
    if ((newrange(0)<0)||(newrange(1)<=newrange(0))||(newrange(1)>timeAxis_p.nelements())){
      cerr << "fitCR2gauss::setRemoteRange: " << "Parameter out of range!" << endl;
      return False;
    };
    // All clear, so set the values
    remoteRange_p = newrange;
    return True;
  };

  Bool fitCR2gauss::setRemoteRangeSeconds(Double starttime, Double stoptime){
    try {
      uInt start,stop;
      start = ntrue(timeAxis_p<starttime);
      stop = ntrue(timeAxis_p<=stoptime);
      if (!(start<stop)){
	cerr << "fitCR2gauss::setRemoteRangeSeconds: " << "Invalid parameters: start>=stop" << endl;
	return False;	
      };
      // All clear, so set the values      
      remoteRange_p.resize(2);
      remoteRange_p(0) = start; remoteRange_p(1) = stop; 
    } catch (AipsError x) {
      cerr << "fitCR2gauss::setRemoteRangeSeconds: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };


  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  
  Record fitCR2gauss::Fitgauss(Vector<Double> xBeam, Vector<Double> ccBeam, Bool EvalGauss, Double center){
    Record erg;
    try {
      if (timeAxis_p.nelements()<=0) {
	cerr << "fitCR2gauss::Fitgauss: " << "Timeaxis not initialized!" << endl;
	return Record();
      };
      if ((fitRange_p(1)-fitRange_p(0))<2) {
	cerr << "fitCR2gauss::Fitgauss: " << "FitRange too small! Not initialized?" << endl;
	return Record();
      };
      if ((timeAxis_p.nelements()!=xBeam.nelements()) || 
	  (timeAxis_p.nelements()!=ccBeam.nelements())) {
	cerr << "fitCR2gauss::Fitgauss: " << "Bad data: length != timeaxis!" << endl;
	return Record();
      };

      dfitgauss fitter;
      Slice fitregion(fitRange_p(0),(fitRange_p(1)-fitRange_p(0)));
      Slice remoteregion(remoteRange_p(0),(remoteRange_p(1)-remoteRange_p(0)));
      Vector<Double> xval,yval,tmp;
      Double meanval;
      Record staterec;

      xval = timeAxis_p(fitregion);
      meanval = mean(xBeam(remoteregion));
      yval = xBeam(fitregion)-meanval;	
      // set the initial state
      staterec.define("height",max(yval));
      if (!(center < 1e99)) {
	staterec.define("center",defCenter_p);
      } else {
	staterec.define("center",center);
      };
      staterec.define("width",defWidth_p);
      fitter.setState(staterec);
      // fit the X-beam
      staterec = fitter.fitXY(xval, yval);
      // transfer values into results vector
      erg.define("Xconverged",staterec.asBool("converged"));
      tmp = staterec.asArrayDouble("height"); erg.define("Xheight",tmp(0));
      tmp = staterec.asArrayDouble("center"); erg.define("Xcenter",tmp(0));
      tmp = staterec.asArrayDouble("width");erg.define("Xwidth",tmp(0));
      tmp = staterec.asArrayDouble("height_error");erg.define("Xheight_error",tmp(0));
      tmp = staterec.asArrayDouble("center_error");erg.define("Xcenter_error",tmp(0));
      tmp = staterec.asArrayDouble("width_error");erg.define("Xwidth_error",tmp(0));
      if (EvalGauss) {
	erg.define("Xgaussian",fitter.evalGaussian(timeAxis_p));
      };
      
      // cut the Detector noise from the fit.
      tmp.resize(0);
      if (staterec.asBool("converged")) {
	uInt newfitstop,newfitlen;
	tmp = staterec.asArrayDouble("center");
	newfitstop = ntrue(timeAxis_p<=(tmp(0)+3.75e-8));
	newfitlen = newfitstop-fitRange_p(0);
	if ((newfitlen>5) && (newfitlen<(fitregion.length()-1))){
	  fitregion = Slice(fitRange_p(0),newfitlen);
#ifdef DEBUGGING_MESSAGES   
	} else {
	  cout << "fitCR2gauss::Fitgauss: " << "fitregion not changed because newfitlen=" << newfitlen << endl;
#endif
	};
      };

      // fit the CC-beam
      xval.resize(0); yval.resize(0);
      xval = timeAxis_p(fitregion);
      meanval = mean(ccBeam(remoteregion));
      yval = ccBeam(fitregion)-meanval;
      fitter.setState(staterec);
      staterec = fitter.fitXY(xval, yval);
      // transfer values into results vector
      erg.define("CCconverged",staterec.asBool("converged"));
      tmp = staterec.asArrayDouble("height"); erg.define("CCheight",tmp(0));
      tmp = staterec.asArrayDouble("center"); erg.define("CCcenter",tmp(0));
      tmp = staterec.asArrayDouble("width");erg.define("CCwidth",tmp(0));
      tmp = staterec.asArrayDouble("height_error");erg.define("CCheight_error",tmp(0));
      tmp = staterec.asArrayDouble("center_error");erg.define("CCcenter_error",tmp(0));
      tmp = staterec.asArrayDouble("width_error");erg.define("CCwidth_error",tmp(0));
      if (EvalGauss) {
	erg.define("Cgaussian",fitter.evalGaussian(timeAxis_p));
      };

    } catch (AipsError x) {
      cerr << "fitCR2gauss::Fitgauss: " << x.getMesg() << endl;
      return Record();
    }; 
    return erg;    
  };





} // Namespace CR -- end
