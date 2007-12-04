/*-------------------------------------------------------------------------*
 | $Id: analyseLOPESevent.cc,v 1.13 2007/08/03 12:27:24 horneff Exp $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <Analysis/analyseLOPESevent.h>

#define DEG2RAD (PI/180.)

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  analyseLOPESevent::analyseLOPESevent(){
    pipeline_p    = NULL;
    lev_p         = NULL;
    remoteStart_p = 1./4.;
    remoteStop_p  = 4./9.;
    fitStart_p    = -2e-6;
    fitStop_p     = -1.7e-6;
    plotStart_p   = -2.05e-6;
    plotStop_p    = -1.55e-6;

    clear();
  };
  
  void analyseLOPESevent::clear() {
    if (pipeline_p != NULL) {
      delete pipeline_p;
      pipeline_p = NULL;
    };
    if (lev_p != NULL) {
      delete lev_p;
      lev_p = NULL;
    };
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  analyseLOPESevent::~analyseLOPESevent ()
  {
    destroy();
  }
  
  void analyseLOPESevent::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void analyseLOPESevent::summary (std::ostream &os)
  {
    os << "[analyseLOPESevent] Summary of object properties" << std::endl;
    os << "-- remoteStart = " << remoteStart_p << std::endl;
    os << "-- remoteStop  = " << remoteStop_p  << std::endl;
  }
   
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------- initPipeline
  
  Bool analyseLOPESevent::initPipeline(Record ObsRecord){
    try {
      clear();
      pipeline_p= new CRinvFFT();
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::initPipeline: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  // --------------------------------------------------------------- ProcessEvent

  Record analyseLOPESevent::ProcessEvent(String evname,
					 Double Az,
					 Double El,
					 Double distance, 
					 Double XC,
					 Double YC,
					 Bool RotatePos,
					 String PlotPrefix, 
					 Bool generatePlots,
					 Vector<Int> FlaggedAntIDs, 
					 Bool verbose,
					 Bool simplexFit,
					 Double ExtraDelay,
					 int doTVcal){
    Record erg;
    try {
      Int nsamples;
      Vector<Double> Times, ccBeam, xBeam, pBeam, tmpvec;
      Matrix<Double> TimeSeries;
      Record fiterg;
      
      pipeline_p->setVerbosity(verbose);
      // initialize the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Failed to attach file: " << evname << endl;
	return Record();
      };
      if (! pipeline_p->InitEvent(lev_p)){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Failed to initialize the DataReader!" << endl;
	return Record();
      };
      
      //  Enable/disable doing the phase calibration as requested
      switch (doTVcal){
      case 0:
	pipeline_p->doPhaseCal(False);
	break;
      case 1:
	pipeline_p->doPhaseCal(True);
	break;
      default:
	if ( lev_p->header().isDefined("EventClass") &&
	     lev_p->header().asInt("EventClass") == LopesEventIn::Simulation ){
	  if (verbose){
	    cout << "Simulation event: ";
	  };
	  pipeline_p->doPhaseCal(False);
	} else {
	  pipeline_p->doPhaseCal(True);
	};
	break;
      };


      // Generate the antenna selection
      Vector <Bool> AntennaSelection;
      Int i,j,id,nants,nselants, nflagged=FlaggedAntIDs.nelements();
      AntennaSelection = pipeline_p->GetAntennaMask(lev_p);
      nants = AntennaSelection.nelements();
      Vector<Int> AntennaIDs,selAntIDs;
      lev_p->header().get("AntennaIDs",AntennaIDs);
      if (nflagged >0){
	for (i=0; i<nflagged; i++){
	  id = FlaggedAntIDs(i);
	  for (j=0; j<nants; j++){
	    if (AntennaIDs(j) == id) {
	      AntennaSelection(j) = False;
	      id = 0;
	      break;
	    };
	  };
	  if (verbose && (id !=0)){
	    cout << "analyseLOPESevent::ProcessEvent: " << "AntennaID: " << id 
		 << " not found -> no antenna flagged." << endl;
	  };
	};
      };

      //initialize the pipeline
      Times = lev_p->timeValues();
      nsamples = Times.nelements();
      if (! pipeline_p->setPhaseCenter(XC, YC, RotatePos)){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during setPhaseCenter()!" << endl;
	return Record();
      };

      //Flagg antennas
      flagger.calcWeights(pipeline_p->GetTimeSeries(lev_p));
      AntennaSelection = AntennaSelection && flagger.parameters().asArrayBool("AntennaMask");
      nselants = ntrue(AntennaSelection);


      //initialize the fitter
      Vector<uInt> remoteRange(2,0);
      remoteRange(0) = (uInt)(nsamples*remoteStart_p);
      remoteRange(1) = (uInt)(nsamples*remoteStop_p);
      fitObject.setTimeAxis(Times);
      fitObject.setRemoteRange(remoteRange);
      fitObject.setFitRangeSeconds(fitStart_p,fitStop_p);

      //perform the position fitting
      Double center;
      if (simplexFit) {
	if (verbose) { cout << "analyseLOPESevent::ProcessEvent: starting evaluateGrid()." << endl;};
	if (! evaluateGrid(Az, El, distance, AntennaSelection, &center) ){
	  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during evaluateGrid()!" << endl;
	  return Record();
	};
	if (verbose) { cout << "analyseLOPESevent::ProcessEvent: starting SimplexFit()." << endl;};
	//if (! SimplexFit2(Az, El, distance, center, AntennaSelection) ){
	//  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during SimplexFit2()!" << endl;
	//  return Record();
	//};
	if (! SimplexFit(Az, El, distance, center, AntennaSelection) ){
	  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during SimplexFit()!" << endl;
	  return Record();
	};
	pipeline_p->setVerbosity(verbose);
      };
      
      // Get the beam-formed data
      if (! pipeline_p->setDirection(Az, El, distance)){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during setDirection()!" << endl;
	return Record();
      };
      if (! pipeline_p->GetTCXP(lev_p, TimeSeries, ccBeam, xBeam, pBeam, AntennaSelection)){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during GetTCXP()!" << endl;
	return Record();
      };
      // smooth the data
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);

      Slice remoteRegion(remoteRange(0),(remoteRange(1)-remoteRange(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));

      // do the fitting
      fiterg = fitObject.Fitgauss(xBeam, ccBeam, True, center);
      if ( !fiterg.isDefined("Xconverged") || !fiterg.isDefined("CCconverged") ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during fit!" << endl;
	return Record();	
      };

      if (verbose) {
	cout << "Event \"" << evname << "\" fit results:" << endl;
	if (fiterg.asBool("Xconverged")){
	  cout << " Fit to X-Beam has converged:" << endl;
	} else {
	  cout << " Fit to X-Beam has _not_ converged:" << endl;
	};
	cout << "  XHeight: " << fiterg.asDouble("Xheight") << " +/- " 
	     <<fiterg.asDouble("Xheight_error") << endl;
	cout << "  XWidth : " << fiterg.asDouble("Xwidth") << " +/- "
	     <<fiterg.asDouble("Xwidth_error") << endl;
	cout << "  XCenter: " << fiterg.asDouble("Xcenter") << " +/- "
	     <<fiterg.asDouble("Xcenter_error") << endl;
	if (fiterg.asBool("CCconverged")){
	  cout << " Fit to CC-Beam has converged:" << endl;
	} else {
	  cout << " Fit to CC-Beam has _not_ converged:" << endl;
	};
	cout << "  CCHeight: " << fiterg.asDouble("CCheight") << " +/- " 
	     <<fiterg.asDouble("CCheight_error") << endl;
	cout << "  CCWidth : " << fiterg.asDouble("CCwidth") << " +/- "
	     <<fiterg.asDouble("CCwidth_error") << endl;
	cout << "  CCCenter: " << fiterg.asDouble("CCcenter") << " +/- "
	     <<fiterg.asDouble("CCcenter_error") << endl;
      };
      
      //copy fit results to record
      erg.mergeField(fiterg,"Xconverged", RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCconverged",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter_error",RecordInterface::OverwriteDuplicates);

      // calculate other stuff
      erg.define("rmsC",stddev(ccBeam(remoteRegion)));
      erg.define("rmsX",stddev(xBeam(remoteRegion)));
      Matrix<Double> AntPos=pipeline_p->GetAntPositions();
      AntPos = toShower(AntPos, Az, El);
      Vector<Double> distances(nants);
      for (i=0; i<nants; i++) {
	distances(i) = sqrt( square(AntPos.row(i)(0)) + square(AntPos.row(i)(1)) );
      };
      erg.define("distances",distances);
      tmpvec.resize(nselants);
      tmpvec = distances(AntennaSelection).getCompressedArray() ;
      erg.define("meandist",mean(tmpvec));
      erg.define("Azimuth",Az);
      erg.define("Elevation",El);
      erg.define("Distance",distance);

      // Generate the plots
      if (generatePlots) {
	i = ntrue(Times<plotStart_p);
	j = ntrue(Times<plotStop_p);
	Slice plotRegion(i,(j-i));
	Matrix<Double> parts((j-i),nselants);
	Vector<Double> xarr, yarr, empty, gauss;
	String plotname;
	SimplePlot plotter;
	xarr = Times(plotRegion) * 1e6;
	//plot the CC-beam
	plotname = PlotPrefix;
	plotname += "-CC.ps";
	yarr = ccBeam(plotRegion) * 1e6;
	plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			  "Time [\\(0638)seconds]", "CC-Beam [\\(0638)V/m/MHz]","",4);
	yarr = pBeam(plotRegion) *1e6;
	plotter.PlotLine(xarr,yarr,5,3);
	gauss = fiterg.asArrayDouble("Cgaussian");
	yarr = gauss(plotRegion) *1e6;
	plotter.PlotLine(xarr,yarr,6,2);
	// plot the X-Beam
	plotname = PlotPrefix;
	plotname += "-X.ps";
	yarr = xBeam(plotRegion) * 1e6;
	plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			  "Time [\\(0638)seconds]", "X-Beam [\\(0638)V/m/MHz]","",4);
	yarr = pBeam(plotRegion) *1e6;
	plotter.PlotLine(xarr,yarr,5,3);
	gauss = fiterg.asArrayDouble("Xgaussian");
	yarr = gauss(plotRegion) *1e6;
	plotter.PlotLine(xarr,yarr,6,2);
	// plot all antenna traces
	plotname = PlotPrefix;
	plotname += "-all.ps";
	Double ymax,ymin,xmax,xmin,tmpval;
	selAntIDs.resize(nselants);
	AntennaIDs(AntennaSelection).getCompressedArray(selAntIDs);
	for (i=0; i<nselants; i++){
	  parts.column(i) = TimeSeries.column(i)(plotRegion)*1e6;
	  tmpvec(i) = max(abs(parts.column(i)));
	};
	//ymax = max(parts); ymin = min(parts);
	//ymax = 2*abs(erg.asDouble("CCheight")); ymin = -ymax;
	ymax = median(tmpvec)*1.05; ymin = -ymax;
	xmin = min(xarr); xmax = max(xarr);
	tmpval = (xmax-xmin)*0.05;
	xmin -= tmpval;
	xmax += tmpval;
	plotter.InitPlot(plotname, xmin, xmax, ymin, ymax);
	plotter.AddLabels("Time [\\(0638)seconds]", "X-Beam [\\(0638)V/m/MHz]");
	Int col1,col2,col3;
	col1=col2=col3=3;
	for (i=0; i<nselants; i++){
	  if (selAntIDs(i) > 70000) { 
	    plotter.PlotLine(xarr,parts.column(i),col3++,3);
	  } else if (selAntIDs(i) > 40000) {
	    plotter.PlotLine(xarr,parts.column(i),col2++,2);
	  } else {
	    plotter.PlotLine(xarr,parts.column(i),col1++,1);
	  };
	};
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::ProcessEvent: " << x.getMesg() << endl;
      return Record();
    }; 
    return erg;
  }

  // ------------------------------------------------------------------- toShower

  Matrix<Double> analyseLOPESevent::toShower (Matrix<Double> & pos,
					      Double Az,
					      Double El){
    Matrix<Double> erg;
    try {
      if (pos.ncolumn() != 3) {
	cerr << "analyseLOPESevent:toShower: " << "invalis parameters: pos.ncolumn() != 3 " << endl;
	return Matrix<Double>();
      };
      erg.resize(pos.shape());

      Double azrad,elrad;
      azrad = Az*DEG2RAD;
      elrad = El*DEG2RAD;

      Double l1, l2, l3, m1, m2, m3, n1, n2, n3;
      
      l1 = -sin(azrad);
      l2 = cos(azrad);
      l3 = 0.;

      m1 = sin(elrad)*cos(azrad);
      m2 = sin(elrad)*sin(azrad);
      m3 = -cos(elrad);

      n1 = cos(elrad)*cos(azrad);
      n2 = cos(elrad)*sin(azrad);
      n3 = sin(elrad);
      
      erg.column(0) = pos.column(0)*l1 + pos.column(1)*l2;
      erg.column(1) = pos.column(0)*m1 + pos.column(1)*m2 + pos.column(2)*m3;
      erg.column(2) = pos.column(0)*n1 + pos.column(1)*n2 + pos.column(2)*n3;

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:toShower: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return erg;
  };
  

  Bool analyseLOPESevent::SimplexFit (Double &Az,
				      Double &El,
				      Double &distance,
				      Double &center, 
				      Vector<Bool> AntennaSelection ){    
    try {
      Int i,minpos,maxpos,niteration=0,oldminpos,nsameiter;
      Record erg;
      Vector<Double> azs(4), els(4), dists(4), height(4,0.), cents(4);
      Double newaz, newel, newdist, newheight, nnewheight, newcent, nnewcent;
      Double meanaz, meanel, meandist, vecaz, vecel, vecdist;
      Bool running=True;
      
      pipeline_p->setVerbosity(False);

      // set start values
      azs = Az; els = El; dists = distance;
      azs(1) += 1; els(2) += 1; 
      azs(3) -= 1; els(3) -= 1;      
      dists(2) -= 2000;dists(1) += 2000;dists(3) += 2000;
      cents = center;
      for (i=0; i<4; i++){
	height(i) = getHeight(azs(i), els(i), dists(i), AntennaSelection, &cents(i));
      };
      minpos=0; nsameiter=0;oldminpos=0;
      while(running){ // the big loop
	// find worst point
	for (i=0; i<4; i++){
	  if ((i!=minpos) && (height(i)<height(minpos))){
	    minpos = i;
	  };
	};
	// find the vector from minpoint to centerpoint
	meanaz = meanel = meandist = 0.;
	for (i=0; i<4; i++){
	  if (i!=minpos){
	    meanaz += azs(i); meanel += els(i); meandist += dists(i);
	  };
	};
	meanaz /= 3.; meanel /= 3.; meandist /= 3.;
	vecaz = meanaz-azs(minpos); vecel = meanel-els(minpos); vecdist = meandist-dists(minpos);
	if (minpos == oldminpos){
	  nsameiter++;
	} else {
	  nsameiter=0;
	  oldminpos = minpos;
	};
	if ( (nsameiter==8) || ((abs(vecaz)<0.03) && (abs(vecel)<0.03) && (abs(vecdist)<1))) {
	  // restart from close to best position
	  maxpos=0;
	  for (i=1; i<4; i++){
	    if (height(i)>height(maxpos)){
	      maxpos = i;
	    };
	  };
	  azs(minpos) = azs(maxpos)+0.2;
	  els(minpos) = els(maxpos)+0.2;
	  dists(minpos) = dists(maxpos)+50;
	  cents(minpos) = cents(maxpos);
	  height(minpos) = getHeight(azs(minpos),els(minpos),dists(minpos), AntennaSelection, &cents(minpos));
	  continue;
	};
	if (nsameiter==16) {
	  // We are seriously stuck. Just give up...
	  running = False;
	}
	//new point
	newaz = azs(minpos) + 2.*vecaz;	newel = els(minpos) + 2.*vecel;
	newdist = dists(minpos) + 2.*vecdist;
	newcent = cents(minpos);
	newheight = getHeight(newaz, newel, newdist, AntennaSelection, &newcent);
	if (newheight > max(height)) { //expand the simplex
	  cout << " SimplexFit: expanding simplex"<<endl;
	  newaz = azs(minpos) + 4.*vecaz; newel = els(minpos) + 4.*vecel;
	  newdist = dists(minpos) + 4.*vecdist;
	  nnewcent = cents(minpos);
	  nnewheight = getHeight(newaz, newel, newdist, AntennaSelection, &nnewcent);
	  if (newheight > nnewheight) {
	    newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	    newdist = dists(minpos) + 2.*vecdist;
	  } else {
	    newheight = nnewheight;
	    newcent = nnewcent;
	  };
	} else {
	  height(minpos) = max(height);
	  if (newheight < min(height)) { 
	    cout << " SimplexFit: reducing simplex"<<endl;
	    newaz = azs(minpos) + 1.5*vecaz; newel = els(minpos) + 1.5*vecel;
	    newdist = dists(minpos) + 1.5*vecdist;
	    nnewcent = cents(minpos);
	    nnewheight = getHeight(newaz, newel, newdist, AntennaSelection, &nnewcent);
	    if (newheight > nnewheight) {
	      newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	      newdist = dists(minpos) + 2.*vecdist;
	    } else {
	      newheight = nnewheight;
	      newcent = nnewcent;
	    };
	  };
	};
	// save the new position
	Bool clipping=True;
	while (clipping){
	  if (newel>90.) { newel=180.-newel; continue;};
	  if (newel<0.) { newel=-newel; continue;};
	  if (newdist<0.) { newdist=-newdist; continue;};
	  clipping = False;
	};
	azs(minpos) = newaz; els(minpos) = newel; dists(minpos) = newdist; cents(minpos) = newcent;
	height(minpos)=newheight;
	// generate output
	printf("## %i ------------------------------- \n",niteration);
	printf("  Azimuth:   %6.2f; %6.2f; %6.2f; %6.2f \n",azs(0),azs(1),azs(2),azs(3));
	printf("  Elevation: %6.2f; %6.2f; %6.2f; %6.2f \n",els(0),els(1),els(2),els(3));
	printf("  Distance:  %6.1f; %6.1f; %6.1f; %6.1f \n",dists(0),dists(1),dists(2),dists(3));
	printf("  Center:    %6.3f; %6.3f; %6.3f; %6.3f \n",cents(0)*1e6,cents(1)*1e6,cents(2)*1e6,cents(3)*1e6);
	printf("  Height:    %6.3f; %6.3f; %6.3f; %6.3f \n",height(0),height(1),height(2),height(3));
	// test convergence
	meanaz = max(azs)-min(azs); meanel = max(els)-min(els);
	meanaz *= cos(mean(els)*DEG2RAD);
	meanaz = abs(meanaz*meanel);
	meandist = max(dists)-min(dists);
	if ((meanaz<0.01)&&((meandist/mean(dists))<0.025)) {
	  running = False;
	};
	niteration++;
	if (niteration > 100) {
	  running = False;
	};
      }; // end of the big loop
      // calculate mean position
      meanaz = meanel = meandist = 0.;
      for (i=0; i<4; i++){
	meanaz += azs(i); meanel += els(i); meandist += dists(i);
      };
      meanaz /= 4.; meanel /= 4.; meandist /= 4.;
      Bool clipping=True;
      while (clipping){
	if (meanaz>360.) { meanaz-=360.; continue;};
	if (meanaz<0.) { meanaz+=360.; continue;};
	if (meanel>90.) { meanel=180.-meanel; continue;};
	if (meanel<0.) { meanel=-meanel; continue;};
	if (meandist<0.) { meandist=-meandist; continue;};
	clipping = False;
      };
      Az = meanaz; El = meanel; distance = meandist; center = mean(cents);

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:SimplexFit: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Bool analyseLOPESevent::evaluateGrid (Double &Az,
					Double &El,
					Double &distance,
					Vector<Bool> AntennaSelection,
					Double *centerp){
    try {
      int estep,astep,arange,erange=3;
      Double maxaz=Az,maxel=El,maxheight=0.,maxcenter=-1.8e-6;
      Double az_,el_,height_,center_;
      Double stepfactor=1.;

      pipeline_p->setVerbosity(False);

      for (estep=-erange; estep<=erange; estep++){
	el_ = El + stepfactor*estep;
	arange = erange-abs(estep);
	for (astep=-arange; astep<=arange; astep++){
	  az_ = Az+astep*stepfactor/(cos(el_*DEG2RAD)+1e-6);
	  if (el_>90.){ az_+= 180.; };
	  center_ = 2e99;
	  height_ = getHeight(az_, el_, distance, AntennaSelection, &center_);
	  if (height_ > maxheight){
	    maxaz = az_; maxel = el_; maxheight=height_; maxcenter=center_;
	  };
	};
      };
      Bool clipping=True;
      while (clipping){
	if (maxaz>360.) { maxaz-=360.; continue;};
	if (maxaz<0.) { maxaz+=360.; continue;};
	if (maxel>90.) { maxel=180.-maxel; continue;};
	if (maxel<0.) { maxel=-maxel; continue;};
	clipping = False;
      };
      Az = maxaz; El = maxel;
      if (centerp != NULL) {
	*centerp = maxcenter;
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:evaluateGrid: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
  
  Double analyseLOPESevent::getHeight (Double az,
				       Double el,
				       Double dist, 
				       Vector<Bool> AntennaSelection,
				       Double *centerp){
    Double erg;
    try {
      Vector<Double> ccb,xb,pb;
      Matrix<Double> ts;
      Record ergrec;
      Bool clipping=True;
      while (clipping){
	if (az>360.) { az-=360.; continue;};
	if (az<0.) { az+=360.; continue;};
	if (el>90.) { el=180.-el; continue;};
	if (el<0.) { el=-el; continue;};
	if (dist<0.) { dist=-dist; continue;};
	clipping = False;
      };
      pipeline_p->setDirection(az, el, dist);
      pipeline_p->GetTCXP(lev_p, ts, ccb, xb, pb, AntennaSelection);
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      ccb = mf.filter(ccb);
      xb = mf.filter(xb);
      if (centerp != NULL) {
	ergrec = fitObject.Fitgauss(xb, ccb, False, *centerp);
      } else {
	ergrec = fitObject.Fitgauss(xb, ccb, False);
      };
      erg = ergrec.asDouble("CCheight")*1e6;
      if (centerp != NULL) {
	*centerp = ergrec.asDouble("CCcenter");
      };
      if (!ergrec.asBool("CCconverged") || 
	  (ergrec.asDouble("CCcenter") < fitStart_p) || (ergrec.asDouble("CCcenter") > fitStop_p) ||
	  (ergrec.asDouble("CCwidth")>1e-7)) { 
	pb = lev_p->timeValues();
	erg = mean(ccb(Slice(ntrue(pb<-1.83e-6),5)))*1e6;
	if (centerp != NULL) {
	  *centerp = -1.8e-6;
	};
      };
      cout << "getHeight: Az:"<<az<<" El:"<<el<<" Dist:"<<dist<<" Height:"<< erg << " conv:"<<
	ergrec.asBool("Xconverged")<<ergrec.asBool("CCconverged")<< " Center:"<< ergrec.asDouble("CCcenter")<<endl;
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:getHeight: " << x.getMesg() << endl;
      return 0.;
    }; 
    return erg;
  };

  Bool analyseLOPESevent::SimplexFit2 (Double &Az,
				       Double &El,
				       Double &distance,
				       Double &center,
				       Vector<Bool> AntennaSelection ){    
    try {
      Int i(0), minpos(0), maxpos(0), niteration(0), oldminpos(0), nsameiter(0);
      Record erg;
      Vector<Double> azs(5), els(5), dists(5), cents(5), height(5,0.);
      Double newaz, newel, newdist, newcent, newheight, nnewheight;
      Double meanaz, meanel, meandist, meancent, vecaz, vecel, vecdist, veccent;
      Bool running=True;
      
      pipeline_p->setVerbosity(False);
      
      // set start values
      azs = Az; els = El; dists = distance; cents=center;
      azs(1) += 1; els(2) += 1; 
      azs(3) -= 1; els(3) -= 1;      
      azs(4) -= 1; els(4) += 1;            
      dists(2) -= 2000;dists(1) += 2000;dists(3) += 2000;dists(4) -= 2000;
      cents(1) = -1.8e-6; cents(2) = -1.85e-6; cents(3) = -1.75e-6; cents(4) = -1.8e-6; 
      for (i=0; i<5; i++){
	height(i) = getHeight2(azs(i), els(i), dists(i), cents(i), AntennaSelection);
      };
      minpos=0; nsameiter=0;oldminpos=0;
      while(running){ // the big loop
	// find worst point
	for (i=0; i<5; i++){
	  if ((i!=minpos) && (height(i)<height(minpos))){
	    minpos = i;
	  };
	};
	// find the vector from minpoint to centerpoint
	meanaz = meanel = meandist = meancent = 0.;
	for (i=0; i<5; i++){
	  if (i!=minpos){
	    meanaz += azs(i); meanel += els(i); meandist += dists(i); meancent += cents(i);
	  };
	};
	meanaz /= 4.; meanel /= 4.; meandist /= 4.; meancent /= 4.;
	vecaz = meanaz-azs(minpos); vecel = meanel-els(minpos); 
	vecdist = meandist-dists(minpos); veccent = meancent-cents(minpos);
	if (minpos == oldminpos){
	  nsameiter++;
	} else {
	  nsameiter=0;
	  oldminpos = minpos;
	};
	if ( (nsameiter==8) || ((abs(vecaz)<0.03) && (abs(vecel)<0.03) && (abs(vecdist)<1))) {
	  // restart from close to best position
	  maxpos=0;
	  for (i=1; i<4; i++){
	    if (height(i)>height(maxpos)){
	      maxpos = i;
	    };
	  };
	  azs(minpos) = azs(maxpos)+0.2;
	  els(minpos) = els(maxpos)+0.2;
	  dists(minpos) = dists(maxpos)+50;
	  cents(minpos) = cents(maxpos)+0.02e-6;
	  height(minpos) = getHeight2(azs(minpos),els(minpos),dists(minpos),cents(minpos), AntennaSelection);
	  continue;
	};
	if (nsameiter==16) {
	  // We are seriously stuck. Just give up...
	  running = False;
	}
	//new point
	newaz = azs(minpos) + 2.*vecaz;	newel = els(minpos) + 2.*vecel;
	newdist = dists(minpos) + 2.*vecdist; newcent = cents(minpos) + 2.*veccent;
	newheight = getHeight2(newaz, newel, newdist, newcent, AntennaSelection);
	if (newheight > max(height)) { //expand the simplex
	  cout << " SimplexFit2: expanding simplex"<<endl;
	  newaz = azs(minpos) + 4.*vecaz; newel = els(minpos) + 4.*vecel;
	  newdist = dists(minpos) + 4.*vecdist; newcent = cents(minpos) + 4.*veccent;
	  nnewheight = getHeight2(newaz, newel, newdist, newcent, AntennaSelection);
	  if (newheight > nnewheight) {
	    newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	    newdist = dists(minpos) + 2.*vecdist; newcent = cents(minpos) + 2.*veccent;
	  } else {
	    newheight = nnewheight;
	  };
	} else {
	  height(minpos) = max(height);
	  if (newheight < min(height)) { 
	    cout << " SimplexFit2: reducing simplex"<<endl;
	    newaz = azs(minpos) + 1.5*vecaz; newel = els(minpos) + 1.5*vecel;
	    newdist = dists(minpos) + 1.5*vecdist; newcent = cents(minpos) + 1.5*veccent;
	    nnewheight = getHeight2(newaz, newel, newdist, newcent, AntennaSelection);
	    if (newheight > nnewheight) {
	      newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	      newdist = dists(minpos) + 2.*vecdist; newcent = cents(minpos) + 2.*veccent;
	    } else {
	      newheight = nnewheight;
	    };
	  };
	};
	// save the new position
	Bool clipping=True;
	while (clipping){
	  if (newel>90.) { newel=180.-newel; continue;};
	  if (newel<0.) { newel=-newel; continue;};
	  if (newdist<0.) { newdist=-newdist; continue;};
	  clipping = False;
	};
	azs(minpos) = newaz; els(minpos) = newel; dists(minpos) = newdist; cents(minpos) = newcent;
	height(minpos)=newheight;
	// generate output
	printf("## %i ------------------------------- \n",niteration);
	printf("  Azimuth:   %6.2f; %6.2f; %6.2f; %6.2f; %6.2f \n",azs(0),azs(1),azs(2),azs(3),azs(4));
	printf("  Elevation: %6.2f; %6.2f; %6.2f; %6.2f; %6.2f \n",els(0),els(1),els(2),els(3),els(4));
	printf("  Distance:  %6.1f; %6.1f; %6.1f; %6.1f; %6.1f \n",dists(0),dists(1),dists(2),dists(3),dists(4));
	printf("  Center:    %6.3f; %6.3f; %6.3f; %6.3f; %6.3f \n",cents(0)*1e6,cents(1)*1e6,cents(2)*1e6,cents(3)*1e6,cents(4)*1e6);
	printf("  Height:    %6.3f; %6.3f; %6.3f; %6.3f; %6.3f \n",height(0)*1e6,height(1)*1e6,height(2)*1e6,height(3)*1e6,height(4)*1e6);
	// test convergence
	meanaz = max(azs)-min(azs); meanel = max(els)-min(els);
	meanaz *= cos(mean(els)*DEG2RAD);
	meanaz = abs(meanaz*meanel);
	meandist = max(dists)-min(dists);
	if ((meanaz<0.01)&&((meandist/mean(dists))<0.025)) {
	  running = False;
	};
	niteration++;
	if (niteration > 150) {
	  running = False;
	};
      }; // end of the big loop
      // calculate mean position
      meanaz = meanel = meandist = meancent = 0.;
      for (i=0; i<5; i++){
	meanaz += azs(i); meanel += els(i); meandist += dists(i); meancent += cents(i);
      };
      meanaz /= 5.; meanel /= 5.; meandist /= 5.; meancent /= 5.;
      
      Bool clipping=True;
      while (clipping){
	if (meanaz>360.) { meanaz-=360.; continue;};
	if (meanaz<0.) { meanaz+=360.; continue;};
	if (meanel>90.) { meanel=180.-meanel; continue;};
	if (meanel<0.) { meanel=-meanel; continue;};
	if (meandist<0.) { meandist=-meandist; continue;};
	clipping = False;
      };
      Az = meanaz; El = meanel; distance = meandist; center = meancent;
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:SimplexFit2: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Double analyseLOPESevent::getHeight2 (Double az,
					Double el,
					Double dist,
					Double Center,
					Vector<Bool> AntennaSelection){
    Double erg (0);
    try {
      Vector<Double> xbeam,time,CenterVec(1),ergVec(1);
      Bool clipping=True;
      while (clipping){
	if (az>360.) { az-=360.; continue;};
	if (az<0.) { az+=360.; continue;};
	if (el>90.) { el=180.-el; continue;};
	if (el<0.) { el=-el; continue;};
	if (dist<0.) { dist=-dist; continue;};
	clipping = False;
      };
      pipeline_p->setDirection(az, el, dist);
      xbeam = pipeline_p->GetXBeam(lev_p, AntennaSelection);
      StatisticsFilter<Double> mf(3,FilterType::MEAN);
      xbeam = mf.filter(xbeam);
      time = lev_p->timeValues();
      CenterVec(0) = Center;
      
      InterpolateArray1D<Double, Double>::interpolate(ergVec,CenterVec,time,xbeam,
						      InterpolateArray1D<Float,Double>::spline);
      erg = ergVec(0);

      cout << "getHeight2: Az:"<<az<<" El:"<<el<<" Dist:"<<dist<<" Center:"<<Center 
	   << " Height:"<< erg << endl;
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:getHeight2: " << x.getMesg() << endl;
      return 0.;
    }; 
    return erg;
  };


} // Namespace CR -- end
