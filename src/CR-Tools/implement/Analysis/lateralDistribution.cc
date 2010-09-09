/*-------------------------------------------------------------------------*
 | $Id:: lateralDistribution.cc 4511 2010-03-24 15:24:48Z baehren         $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Frank Schroeder (<mail>)   , Nunzia Palmieri                          *
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

#include <Analysis/lateralDistribution.h>

#include <boost/lexical_cast.hpp>

#include "TROOT.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TAxis.h"

#ifdef HAVE_STARTOOLS

namespace CR { // Namespace CR -- begin

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  lateralDistribution::lateralDistribution () :
    lateralSNRcut(1.0),
    lateralTimeCut(25e-9)
  {;}

  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================

  lateralDistribution::~lateralDistribution ()
  {
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  void lateralDistribution::summary (std::ostream &os)
  {
    os << "[lateralDistribution] Summary of internal parameters" << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  Record lateralDistribution::fitLateralDistribution (const string& filePrefix,
                                                      map <int, PulseProperties> pulsesRec,
                                                      map <int, PulseProperties> pulsesSim,
                                                      int Gt, double az, double ze,
                                                      const string& index1,
                                                      const string& index2,
                                                      const double& fitDistance)
  {
    Record erg;
    try {
      unsigned int ant = 0;        // counting antennas with pulse information
      // define default values, in case lateral distribution does not work
      erg.define("eps",0.);
      erg.define("R_0",0.);
      erg.define("sigeps",0.);
      erg.define("sigR_0",0.);
      erg.define("chi2NDF",0.);
      erg.define("latMeanDist",0.);
      erg.define("NlateralAntennas",ant);
      erg.define("fitDistance",fitDistance);
      
      cout << "\nFitting lateral distribution for data (and simulation)..." << endl;
      
      // create arrays for plotting and fitting LOPES data and REAS sim
      unsigned int Nant = pulsesRec.size();
      double fieldStr[Nant],distance[Nant],fieldStrSim[Nant],distanceSim[Nant];
      double fieldStrEr[Nant],distanceEr[Nant],fieldStrErSim[Nant],distanceErSim[Nant];
      int antennaNumber[Nant];
      int antID[Nant];
      
      // fit simulation only, if values are submitted
      bool fitSim = false;
      if (pulsesSim.size() > 0)
        fitSim = true;

      for (map <int, PulseProperties>::iterator it=pulsesRec.begin(); it != pulsesRec.end(); ++it) {
	if ( (*it).second.lateralCut ) // don't use an antenna marked to cut
  	  continue;
        distance[ant] = (*it).second.dist;
        distanceEr[ant] = (*it).second.disterr;
        fieldStr[ant] = (*it).second.height;
        fieldStrEr[ant] = (*it).second.heightError;
        antennaNumber[ant] = (*it).second.antenna;
        antID[ant] = (*it).first;

        //for simulation
        if (fitSim) {
          if (pulsesSim[antID[ant]].antennaID != antID[ant]) { // consistency check
            cerr << "\nlateralDistribution::fitLateralDistribution: antenna IDs of data and simulation do not match. Skipping...\n" << endl;
            //return Record();
            continue; //this antennas are not counted!
          }         
          fieldStrSim[ant] = pulsesSim[antID[ant]].height;
          fieldStrErSim[ant] = pulsesSim[antID[ant]].heightError;
          distanceSim[ant] = pulsesSim[antID[ant]].dist;
          distanceErSim[ant] = pulsesSim[antID[ant]].disterr;
        }

        ++ant; // increase counter  
      }

      // consistancy check
      if (ant != Nant)
        cerr << "lateralDistribution::fitLateralDistribution: Nant != number of antenna values\n" << endl; 

      Double_t fieldMax=3.5;
      Double_t fieldMin=0;
      Double_t maxdist=0;

      // looking for extreme values for axes
      for (unsigned int i = 0; i < Nant; ++i) {
        /* get largest distance and min and max field strength */
        if (distance[i] > maxdist) {
          maxdist=distance[i];
        }
        if (fieldStrSim[i]>fieldMax) {
          fieldMax=fieldStrSim[i];
        }
        if (fieldStr[i]>fieldMax) {
          fieldMax=fieldStr[i];
        }
        if (fitSim) {
          if (distanceSim[i] > maxdist) {
            maxdist=distanceSim[i];
          }
          if (fieldStrSim[i]<fieldMin) {
            fieldMin=fieldStrSim[i];
          }
          if (fieldStr[i]<fieldMin) {
            fieldMin=fieldStr[i];
          }
        }
      }

      cout << "\nAntennas in the Plot/Fit (no cuts applied): " << ant << endl;
      erg.define("NlateralAntennas",ant);
      
      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (ant > 0)
        latMeanDist = Mean(ant, distance);
      cout << "Mean distance (data): " << latMeanDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      latPro->SetFillColor(0);
      latPro->SetLineColor(4);
      latPro->SetMarkerColor(4);
      latPro->SetMarkerStyle(kFullCircle);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Lateral distribution - " << Gt;
      //latPro->SetTitle(label.str().c_str());
      latPro->SetTitle("");
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetXaxis()->SetLimits(0,maxdist*1.1);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(1,fieldMax*3);
      latPro->GetYaxis()->SetRangeUser(1,fieldMax*3);
      

      TGraphErrors *latProSim = new TGraphErrors (ant, distanceSim,fieldStrSim,distanceErSim,fieldStrErSim);
      latProSim->SetFillColor(2);
      latProSim->SetLineColor(2);
      latProSim->SetMarkerColor(2);
      latProSim->SetMarkerStyle(kFullSquare);
      latProSim->SetMarkerSize(1.1);
 
      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","");
      c1->Range(-18.4356,-0.31111,195.528,2.19048);
      c1->SetFillColor(0);
      c1->SetBorderMode(0);
      c1->SetBorderSize(2);

      c1->SetLeftMargin(0.127768);
      c1->SetRightMargin(0.0715503);
      c1->SetTopMargin(0.0761421);
      c1->SetBottomMargin(0.124365);
      c1->SetFrameLineWidth(2);
      c1->SetFrameBorderMode(0);

      /* lateral plot */
      c1->SetLogy();
      latPro->Draw("AP");
      if (fitSim)
        latProSim->Draw("SAME P");

      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("Data");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latPro->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latPro->GetListOfFunctions());

      /* statistic box of fit SIMULATIONS */
      TPaveStats *ptstatsS = new TPaveStats(0.62,0.69,0.98,0.84,"brNDC");
      ptstatsS->SetName("Simulation");
      ptstatsS->SetBorderSize(2);
      ptstatsS->SetTextAlign(12);
      ptstatsS->SetFillColor(0);
      ptstatsS->SetOptStat(0);
      ptstatsS->SetOptFit(111);
      if (fitSim) {
        ptstatsS->Draw();
        latProSim->GetListOfFunctions()->Add(ptstatsS);
        ptstatsS->SetParent(latProSim->GetListOfFunctions());
      }

      /* statistic box of EAS parameter */
      // TPaveStats *easstats = new TPaveStats(0.45,0.78,0.62,0.99,"brNDC");
      TPaveStats *easstats = new TPaveStats(0.45,0.84,0.62,0.99,"brNDC");
      easstats->SetBorderSize(2);
      easstats->SetTextAlign(12);
      easstats->SetFillColor(0);
      easstats->SetOptStat(0);
      easstats->SetOptFit(0);
      easstats->SetName("stats");

      // create labels for the plot
      label.str("");
      label << "GT " << Gt;
      TText *text = easstats->AddText(label.str().c_str());
      text->SetTextSize(0.04);
      // label.str("");
      // label << "E_{g}  = " << energy;
      // text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#phi   = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << az << "^{o}";
      text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#theta = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << ze << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      /* Fit exponential decrease */
      // do fit only if there are at least 3 antennas (otherwise set parameters to 0)!
      string epsName;
      string R0Name;
      if (ant >= 3) {
        // define names for statistics
        if (index1 != "") {
          epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}- " + index1;
          R0Name = "R_{0}- " + index1;
        } else {
          epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}";
          R0Name = "R_{0}";
        }
      
        // fit exponential
        TF1 *fitfuncExp;
        string fitFunction = "[0]*exp(-(x-"+boost::lexical_cast<string>(fitDistance)+")/[1])";
        fitfuncExp=new TF1("fitfuncExp",fitFunction.c_str(),0.,maxdist*1.1);
        //fitfuncExp=new TF1("fitfuncExp","[0]*exp(-(x-100)/[1])",50,190);
        fitfuncExp->SetParName(0,epsName.c_str());
        fitfuncExp->SetParameter(0,20);
        fitfuncExp->SetParName(1,R0Name.c_str());
        fitfuncExp->SetParameter(1,100);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latPro->Fit(fitfuncExp, "R");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        erg.define("R_0",fitfuncExp->GetParameter(1));
        // error of epsilon = error of fit + 5 % calibration uncertainty due to environmental effects
        double sigmaEpsilon = sqrt(pow(fitfuncExp->GetParError(0),2)+pow((0.05*fitfuncExp->GetParameter(0)),2));
        erg.define("sigeps",sigmaEpsilon);
        erg.define("sigR_0",fitfuncExp->GetParError(1));
        erg.define("chi2NDF",fitfuncExp->GetChisquare()/double(fitfuncExp->GetNDF()));

        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps    = " << fitfuncExp->GetParameter(0) << "\t +/- " << fitfuncExp->GetParError(0) << "\t µV/m/MHz\n"
             << "total error of eps (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n"
             << "R_0    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t m\n"
             << "Chi^2  = " << fitfuncExp->GetChisquare() << "\t NDF " << fitfuncExp->GetNDF() << "\n"
             << endl;
         
        if (fitSim) {        
          cout << "-------- SIMULATIONS ---------"<<endl;
          // define names for statistics
          if (index2 != "") {
            epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}- " + index2;
            R0Name = "R_{0}- " + index2;
          } else {
            epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}";
            R0Name = "R_{0}";
          }
          TF1 *fitfuncExpS;
          fitfuncExpS=new TF1("fitfuncExpS",fitFunction.c_str(),0.,maxdist*1.1);
          //fitfuncExpS=new TF1("fitfuncExpS","[0]*exp(-(x-100)/[1])",50,190);
          fitfuncExpS->SetParName(0,epsName.c_str());
          fitfuncExpS->SetParameter(0,20);
          fitfuncExpS->SetParName(1,R0Name.c_str());
          fitfuncExpS->SetParameter(1,100);
          fitfuncExpS->SetFillStyle(0);
          fitfuncExpS->SetLineWidth(2);

          latProSim->Fit(fitfuncExpS, "R");
          ptstatsS->Draw();

          // write fit results to record with other results
          erg.define("eps_sim",fitfuncExpS->GetParameter(0));
          erg.define("R_0_sim",fitfuncExpS->GetParameter(1));
          // error of epsilon = error of fit + 10 % energy uncertainty
          double sigmaEpsilon_sim = sqrt(pow(fitfuncExpS->GetParError(0),2)+pow((0.10*fitfuncExpS->GetParameter(0)),2));
          erg.define("sigeps_sim",sigmaEpsilon_sim);
          erg.define("sigR_0_sim",fitfuncExpS->GetParError(1));
          erg.define("chi2NDF_sim",fitfuncExpS->GetChisquare()/double(fitfuncExpS->GetNDF()));

          cout << "Result of exponential fit eps * e^(-x/R_0):\n"
              << "eps_sim    = " << fitfuncExpS->GetParameter(0) << "\t +/- " << fitfuncExpS->GetParError(0) << "\t µV/m/MHz\n"
              << "total error of eps_sim (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n"
              << "R_0sim    = " << fitfuncExpS->GetParameter(1) << "\t +/- " << fitfuncExpS->GetParError(1) << "\t m\n"
              << "Chi^2_sim  = " << fitfuncExpS->GetChisquare() << "\t NDF " << fitfuncExpS->GetNDF() << "\n"
              << endl;
          delete fitfuncExpS;
        }

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << Gt << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
        
        // calculate expectet value from fitted function and relative deviation for each point.residual  
        if (false) {
          double angleToCore[Nant],angleToCoreErr[Nant],deviation[Nant],deviationErr[Nant];           
          ant = 0;
          cout << "Antenna\t distance \t expected \t measured \t deviation" << endl;
          for (map <int, PulseProperties>::iterator it=pulsesRec.begin(); it != pulsesRec.end(); ++it)
            if ( !(*it).second.lateralCut ) { // don't use an antenna marked to cut
              (*it).second.lateralExpHeight = fitfuncExp->Eval((*it).second.dist);
              (*it).second.lateralExpHeightErr = 0;
              (*it).second.lateralExpDeviation = (*it).second.height/(*it).second.lateralExpHeight - 1.;
              (*it).second.lateralExpDeviationErr = 0;
              cout << (*it).second.antenna << " \t " << (*it).second.dist << " \t "
                  << (*it).second.lateralExpHeight << " \t " << (*it).second.height << " \t "
                  << (*it).second.lateralExpDeviation*100 << " %" << endl;
              // fill arrays for plot
              angleToCore[ant] = (*it).second.angleToCore;
              angleToCoreErr[ant] = (*it).second.angleToCoreerr;
              deviation[ant] = (*it).second.lateralExpDeviation * 100; // in percent
              deviationErr[ant] = (*it).second.lateralExpDeviationErr * 100; // in percent
              ++ant;
              // cuts
              if (deviation[ant] > 100)
                deviation[ant] = 100;
              if (deviation[ant] < -100)
                deviation[ant] = -100;
            }
            
          // make plot of deviation
          TGraphErrors *latDev = new TGraphErrors (ant, angleToCore,deviation,angleToCoreErr,deviationErr);
          latDev->SetFillColor(4);
          latDev->SetLineColor(4);
          latDev->SetMarkerColor(4);
          latDev->SetMarkerStyle(20);
          latDev->SetMarkerSize(1.1);
          stringstream label;
          label << "Lateral deviations - " << Gt;
          latDev->SetTitle(label.str().c_str());
          latDev->GetXaxis()->SetTitle("angle in radians"); 
          latDev->GetYaxis()->SetTitle("deviation in percent");
          latDev->GetXaxis()->SetTitleSize(0.05);
          latDev->GetYaxis()->SetRange(-105,105);
          latDev->GetYaxis()->SetTitleSize(0.05);
          latDev->GetXaxis()->SetRange(-3.2,3.2);
          
          c1->Clear();
          c1->SetLogy(0);
          latDev->SetMinimum(-105);
          latDev->SetMaximum(105);
          latDev->SetTitle("");
          latDev->SetFillColor(0);
          latDev->GetXaxis()->SetRangeUser(-3.2,3.2);
          latDev->GetYaxis()->SetRangeUser(-105,105);
          latDev->Draw("AP");
          
          plotNameStream.str("");
          plotNameStream << filePrefix << "dev-" << Gt << ".eps";
          cout << "\nCreating plot: " << plotNameStream.str() << endl;
          c1->Print(plotNameStream.str().c_str());
          latDev->Delete();
          //delete latDev;
        }
        fitfuncExp->Delete();
        //delete fitfuncExp;
      }  
      
      ptstats->Delete();
      ptstatsS->Delete();
      easstats->Delete();
      latPro->Delete();
      latProSim->Delete();
      //c1->Delete();
      //delete latPro;
      //delete latProSim;
      delete c1;
      //delete ptstats;
      //delete ptstatsS;
      //delete easstats;
    } catch (AipsError x) {
      cerr << "lateralDistribution::fitLateralDistribution: " << x.getMesg() << endl;
    }
    return erg;
  }


  Record lateralDistribution::lateralTimeDistribution(const string& filePrefix,
                                                      map <int, PulseProperties> pulsesRec,
                                                      map <int, PulseProperties> pulsesSim,
                                                      int Gt, double az, double ze,
                                                      double ccCenter,
                                                      const string& index1,
                                                      const string& index2)
  {
    Record erg;
    try {
      cout << "\nPlotting time vs distance" << endl;
      
      // predefine fields for fit results
      erg.define("latTime_offset",0.);
      erg.define("latTime_R_curv",0.);
      erg.define("latTime_latOffset",0.);
      erg.define("latTime_sigoffset",0.);
      erg.define("latTime_sigR_curv",0.);
      erg.define("latTime_siglatOffset",0.);
      erg.define("latTime_chi2NDF",0.);
      erg.define("latTimePar_offset",0.);
      erg.define("latTimePar_R_curv",0.);
      erg.define("latTimePar_latOffset",0.);
      erg.define("latTimePar_sigoffset",0.);
      erg.define("latTimePar_sigR_curv",0.);
      erg.define("latTimePar_siglatOffset",0.);
      erg.define("latTimePar_chi2NDF",0.);

      // create arrays for plotting and fitting
      unsigned int Nant = pulsesRec.size();
      Double_t timeVal  [Nant], distance  [Nant];
      Double_t timeValEr[Nant], distanceEr[Nant];
      Double_t timeValSim  [Nant], distanceSim  [Nant];
      Double_t timeValErSim[Nant], distanceErSim[Nant];
      int antennaNumber [Nant];
      int antID         [Nant];

      
      // fit simulation only, if values are submitted
      bool fitSim = false;
      if (pulsesSim.size() > 0)
        fitSim = true;
        
      // loop through antennas and fill the arrays
      unsigned int ant = 0;  // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulsesRec.begin(); it != pulsesRec.end(); ++it) {
        // Pulse time relative to shower plane and relative to CC time
        // = geom. delay from beam forming (as it is substracted during analysis) + 
        // time of pulse in trace (- CC beam time, to get an average of 0).
        timeVal      [ant] = (*it).second.distZ / lightspeed * 1e9,    // defined in Math/Constants.h
        timeVal      [ant] += (*it).second.geomDelay;
        timeVal      [ant] += (*it).second.time - ccCenter*1e9;
        // pulse time error: Error from shower plane (geometry) + time calibration (2 ns)
        timeValEr    [ant] = sqrt(  pow((*it).second.distZerr / lightspeed * 1e9,2)
                                  + pow((*it).second.timeError,2));
        distance     [ant] = (*it).second.dist;
        distanceEr   [ant] = (*it).second.disterr;
        antennaNumber[ant] = (*it).second.antenna;
        antID        [ant] = (*it).second.antennaID;
        
        //for simulation
        if (fitSim) {
          if (pulsesSim[antID[ant]].antennaID != antID[ant]) { // consistency check
            cerr << "\nlateralDistribution::lateralTimeDistribution: antenna IDs of data and simulation do not match. Skipping...\n" << endl;
            //return Record();
            continue; //this antennas are not counted!
          }         
          timeValSim[ant] = pulsesSim[antID[ant]].time + (*it).second.distZ / lightspeed * 1e9;
          timeValErSim[ant] = pulsesSim[antID[ant]].timeError;
          distanceSim[ant] = pulsesSim[antID[ant]].dist;
          distanceErSim[ant] = pulsesSim[antID[ant]].disterr;
        }
        
        ++ant;
      }

      // consistancy check
      if (ant != Nant)
        cerr << "lateralDistribution::lateralTimeDistribution: Nant != number of antenna values\n" << endl; 

      // find plot/fit range
      Double_t timeMax=0;
      Double_t timeMin=0;
      Double_t maxdist=0;
      Double_t mindist=0;      
      timeMin = *min_element(timeVal, timeVal + Nant);
      timeMax = *max_element(timeVal, timeVal + Nant);
      mindist = *min_element(distance, distance + Nant);
      maxdist = *max_element(distance, distance + Nant);
      if (fitSim) {
        Double_t timeMaxSim=0;
        Double_t timeMinSim=0;
        Double_t maxdistSim=0;
        Double_t mindistSim=0;
        timeMinSim = *min_element(timeValSim, timeValSim + Nant);
        timeMaxSim = *max_element(timeValSim, timeValSim + Nant);
        mindistSim = *min_element(distanceSim, distanceSim + Nant);
        maxdistSim = *max_element(distanceSim, distanceSim + Nant);
        if (timeMinSim < timeMin)
          timeMin = timeMinSim;
        if (timeMaxSim > timeMax)
          timeMin = timeMaxSim;
        if (mindistSim < mindist)
          mindist = mindistSim;
        if (maxdistSim > maxdist)
          maxdist = maxdistSim;
      }
      // add error to neccessary plot range
      if (timeMin>0)
        timeMin = 0;      
      timeMin -= *max_element(timeValEr, timeValEr + Nant)+10.;
      timeMax += *max_element(timeValEr, timeValEr + Nant);

      cout << "\nAntennas in the Plot/Fit (no cuts appplied): " << ant << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (ant > 0)
        latMeanDist = Mean(ant, distance);
      cout << "Mean distance of antennas: " << latMeanDist << " m." << endl;
      // erg.define("latMeanDist",latMeanDist);
      // TODO: Define it differently or use the values which were previously calculated    

      TGraphErrors *latPro      = new TGraphErrors (ant, distance, timeVal, distanceEr, timeValEr);
      double offsetY = 40;
      if (fitSim)
        offsetY += 20;
      //double offsetX = 50;
      latPro->SetFillColor(0);
      latPro->SetLineColor(4);
      latPro->SetMarkerColor(4);
      latPro->SetMarkerStyle(kFullCircle);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "radius of curvature - " << Gt;
      //latPro->SetTitle(label.str().c_str());
      latPro->SetTitle("");
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("time T [ns]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRangeUser(timeMin, timeMax + offsetY);
      //latPro->GetXaxis()->SetRangeUser(mindist - offsetX, maxdist + offsetX);
      
      TGraphErrors *latProSim   = new TGraphErrors (ant, distanceSim, timeValSim, distanceErSim, timeValErSim);
      latProSim->SetFillColor(1);
      latProSim->SetLineColor(2);
      latProSim->SetMarkerColor(2);
      latProSim->SetMarkerStyle(kFullSquare);
      latProSim->SetMarkerSize(1.1);


      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","");
      c1->Range(-18.4356,-0.31111,195.528,2.19048);
      c1->SetFillColor(0);
      c1->SetBorderMode(0);
      c1->SetBorderSize(2);
      c1->SetLeftMargin(0.127768);
      c1->SetRightMargin(0.0715503);
      c1->SetTopMargin(0.0761421);
      c1->SetBottomMargin(0.124365);
      c1->SetFrameLineWidth(2);
      c1->SetFrameBorderMode(0);

      /* time vs distance plot */
      latPro->Draw("AP");
      if (fitSim)
        latProSim->Draw("SAME P");

      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latPro->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latPro->GetListOfFunctions());

      /* statistic box of fit SIMULATIONS */
      TPaveStats *ptstatsS = new TPaveStats(0.62,0.69,0.98,0.84,"brNDC");
      ptstatsS->SetName("Simulation");
      ptstatsS->SetBorderSize(2);
      ptstatsS->SetTextAlign(12);
      ptstatsS->SetFillColor(0);
      ptstatsS->SetOptStat(0);
      ptstatsS->SetOptFit(111);
      if (fitSim) {
        ptstatsS->Draw();
        latProSim->GetListOfFunctions()->Add(ptstatsS);
        ptstatsS->SetParent(latProSim->GetListOfFunctions());
      }

      /* statistic box of EAS parameter */
      TPaveStats *easstats = new TPaveStats(0.45,0.84,0.62,0.99,"brNDC");
      easstats->SetBorderSize(2);
      easstats->SetTextAlign(12);
      easstats->SetFillColor(0);
      easstats->SetOptStat(0);
      easstats->SetOptFit(0);
      easstats->SetName("stats");

      /* create labels for the plot */
      label.str("");
      label << "GT " << Gt;
      TText *text = easstats->AddText(label.str().c_str());
      text->SetTextSize(0.04);
      label.str("");
      label << "#phi   = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << az << "^{o}";
      text = easstats->AddText(label.str().c_str());
      label.str("");
      label << "#theta = ";
      label.setf(ios::showpoint);
      label.precision(4);
      label.width(5);
      label << ze << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      // define names for statistics
      string curvName = "";
      string offsetName = "";
      string curvNameS = "";
      string offsetNameS = "";
      if (index1 != "") {
        curvName = "r_{curv}- " + index1;
        offsetName = "offset- " + index1;
      } else {
        curvName = "r_{curv}";
        offsetName = "offset";
      }
      if (index2 != "") {
        curvNameS = "r_{curv}- " + index2;
        offsetNameS = "offset- " + index2;
      } else {
        curvNameS = "r_{curv}";
        offsetNameS = "offset";
      }
      
      /* FIT */
      if (ant >= 3) {           
        // spherical fit
        TF1 *fitFunc;
        //fitFunc=new TF1("fitFunc","[0]+[1]*(sqrt([2]**2+(x-[3])**2)-[2])",0,1000);
        fitFunc=new TF1("fitFunc","[0]+3.335640952*(sqrt([1]**2+x**2)-[1])",0,1000);
        fitFunc->SetParName(0,offsetName.c_str());
        //fitFunc->SetParName(1,"1/c");
        fitFunc->SetParName(1,curvName.c_str());
        //fitFunc->SetParName(3,"lat_offset");
        fitFunc->SetParameter(0,0);
        //fitFunc->FixParameter(1,1e9/lightspeed); // = 3.335640952
        fitFunc->SetParameter(1,1000);
        //fitFunc->SetParameter(3,0);
        //fitFunc->GetXaxis()->SetRange(0,1000);
        fitFunc->SetFillStyle(0);
        fitFunc->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latPro->Fit(fitFunc, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("latTime_offset",fitFunc->GetParameter(0));
        erg.define("latTime_R_curv",fitFunc->GetParameter(1));
        erg.define("latTime_latOffset",0.);
        erg.define("latTime_sigoffset",fitFunc->GetParError(0));
        erg.define("latTime_sigR_curv",fitFunc->GetParError(1));
        erg.define("latTime_siglatOffset",0.);
        erg.define("latTime_chi2NDF",fitFunc->GetChisquare()/double(fitFunc->GetNDF()));
        cout << "Results of spherical fit (data)"
             << "R_curv     = " << fitFunc->GetParameter(1) << "\t +/- " << fitFunc->GetParError(1) << "\t m\n"
             << "t offset   = " << fitFunc->GetParameter(0) << "\t +/- " << fitFunc->GetParError(0) << "\t ns\n"
             // << "lat offset = " << fitFunc->GetParameter(3) << "\t +/- " << fitFunc->GetParError(3) << "\t m\n"
             << "Chi^2  = " << fitFunc->GetChisquare() << "\t NDF " << fitFunc->GetNDF() << "\n"
             //<< "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
             << endl;
             
        if (fitSim) {
          cout << "-------- SIMULATIONS ---------"<<endl;          
          TF1 *fitFuncS;
          //fitFuncS=new TF1("fitFuncS","[0]+[1]*(sqrt([2]**2+(x-[3])**2)-[2])",0,1000);
          fitFuncS=new TF1("fitFuncS","[0]+3.335640952*(sqrt([1]**2+x**2)-[1])",0,1000);
          fitFuncS->SetParName(0,offsetNameS.c_str());
          //fitFuncS->SetParName(1,"1/c");
          fitFuncS->SetParName(1,curvNameS.c_str());
          //fitFuncS->SetParName(3,"lat_offset");
          fitFuncS->SetParameter(0,0);
          //fitFuncS->FixParameter(1,1e9/lightspeed); // = 3.335640952
          fitFuncS->SetParameter(1,1000);
          //fitFuncS->SetParameter(3,0);
          //fitFuncS->GetXaxis()->SetRange(0,1000);
          fitFuncS->SetFillStyle(0);
          fitFuncS->SetLineWidth(2);

          cout << "------------------------------"<<endl;
          latProSim->Fit(fitFuncS, "");
          ptstatsS->Draw();

          // write fit results to record with other results
          erg.define("latTime_offset_sim",fitFuncS->GetParameter(0));
          erg.define("latTime_R_curv_sim",fitFuncS->GetParameter(1));
          erg.define("latTime_latOffset_sim",0.);
          erg.define("latTime_sigoffset_sim",fitFuncS->GetParError(0));
          erg.define("latTime_sigR_curv_sim",fitFuncS->GetParError(1));
          erg.define("latTime_siglatOffset_sim",0.);
          erg.define("latTime_chi2NDF_sim",fitFuncS->GetChisquare()/double(fitFuncS->GetNDF()));
          cout << "Results of spherical fit (SIMULATION)"
              << "R_curv     = " << fitFuncS->GetParameter(1) << "\t +/- " << fitFuncS->GetParError(1) << "\t m\n"
              << "t offset   = " << fitFuncS->GetParameter(0) << "\t +/- " << fitFuncS->GetParError(0) << "\t ns\n"
              // << "lat offset = " << fitFuncS->GetParameter(3) << "\t +/- " << fitFuncS->GetParError(3) << "\t m\n"
              << "Chi^2  = " << fitFuncS->GetChisquare() << "\t NDF " << fitFuncS->GetNDF() << "\n"
              //<< "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
              << endl;
        }

        // write plot to file
        stringstream plotNameStream("");
        plotNameStream << filePrefix << "sphere-" << Gt <<".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
        fitFunc->Delete();
        
        // fit parabola
        TF1 *fitFunc2;
        //fitFunc2=new TF1("fitFunc2","[0]+[1]*((x-[3])**2/(2*[2]))",0,1000);
        fitFunc2=new TF1("fitFunc2","[0]+3.335640952*(x**2/(2*[1]))",0,1000);
        fitFunc2->SetParName(0,offsetName.c_str());
        //fitFunc2->SetParName(1,"1/c");
        fitFunc2->SetParName(1,curvName.c_str());
        //fitFunc2->SetParName(3,"lat_offset");
        fitFunc2->SetParameter(0,0);
        //fitFunc2->FixParameter(1,1e9/lightspeed); // = 3.335640952
        fitFunc2->SetParameter(1,1000);
        //fitFunc2->SetParameter(3,0);
        //fitFunc2->GetXaxis()->SetRange(0,1000);
        fitFunc2->SetFillStyle(0);
        fitFunc2->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latPro->Fit(fitFunc2, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("latTimePar_offset",fitFunc2->GetParameter(0));
        erg.define("latTimePar_R_curv",fitFunc2->GetParameter(1));
        erg.define("latTimePar_latOffset",0.);
        erg.define("latTimePar_sigoffset",fitFunc2->GetParError(0));
        erg.define("latTimePar_sigR_curv",fitFunc2->GetParError(1));
        erg.define("latTimePar_siglatOffset",0.);
        erg.define("latTimePar_chi2NDF",fitFunc2->GetChisquare()/double(fitFunc2->GetNDF()));
        cout << "Results of parabolic fit (data)"
             << "R_curv     = " << fitFunc2->GetParameter(1) << "\t +/- " << fitFunc2->GetParError(1) << "\t m\n"
             << "t offset   = " << fitFunc2->GetParameter(0) << "\t +/- " << fitFunc2->GetParError(0) << "\t ns\n"
             // << "lat offset = " << fitFunc2->GetParameter(3) << "\t +/- " << fitFunc2->GetParError(3) << "\t m\n"
             << "Chi^2  = " << fitFunc2->GetChisquare() << "\t NDF " << fitFunc2->GetNDF() << "\n"
             //<< "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
             << endl;
             
        if (fitSim) {
          cout << "-------- SIMULATIONS ---------"<<endl;
          TF1 *fitFunc2S;
          //fitFunc2S=new TF1("fitFunc2S","[0]+[1]*((x-[3])**2/(2*[2]))",0,1000);
          fitFunc2S=new TF1("fitFunc2S","[0]+3.335640952*(x**2/(2*[1]))",0,1000);
          fitFunc2S->SetParName(0,offsetNameS.c_str());
          //fitFunc2S->SetParName(1,"1/c");
          fitFunc2S->SetParName(1,curvNameS.c_str());
          //fitFunc2S->SetParName(3,"lat_offset");
          fitFunc2S->SetParameter(0,0);
          //fitFunc2S->FixParameter(1,1e9/lightspeed); // = 3.335640952
          fitFunc2S->SetParameter(1,1000);
          //fitFunc2S->SetParameter(3,0);
          //fitFunc2S->GetXaxis()->SetRange(0,1000);
          fitFunc2S->SetFillStyle(0);
          fitFunc2S->SetLineWidth(2);

          cout << "------------------------------"<<endl;
          latProSim->Fit(fitFunc2S, "");
          ptstatsS->Draw();

          // write fit results to record with other results
          erg.define("latTimePar_offset_sim",fitFunc2S->GetParameter(0));
          erg.define("latTimePar_R_curv_sim",fitFunc2S->GetParameter(1));
          erg.define("latTimePar_latOffset_sim",0.);
          erg.define("latTimePar_sigoffset_sim",fitFunc2S->GetParError(0));
          erg.define("latTimePar_sigR_curv_sim",fitFunc2S->GetParError(1));
          erg.define("latTimePar_siglatOffset_sim",0.);
          erg.define("latTimePar_chi2NDF_sim",fitFunc2S->GetChisquare()/double(fitFunc2S->GetNDF()));
          cout << "Results of parabolic fit (SIMULATION)"
              << "R_curv     = " << fitFunc2S->GetParameter(1) << "\t +/- " << fitFunc2S->GetParError(1) << "\t m\n"
              << "t offset   = " << fitFunc2S->GetParameter(0) << "\t +/- " << fitFunc2S->GetParError(0) << "\t ns\n"
              // << "lat offset = " << fitFunc2S->GetParameter(3) << "\t +/- " << fitFunc2S->GetParError(3) << "\t m\n"
              << "Chi^2  = " << fitFunc2S->GetChisquare() << "\t NDF " << fitFunc2S->GetNDF() << "\n"
              //<< "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
              << endl;
        }

        // write plot to file
        plotNameStream.str("");
        plotNameStream << filePrefix << "parabo-" << Gt <<".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
        fitFunc2->Delete();
      } else {
        cout<<"No fit was done, because less than 3 antennas are 'good':\n";
      }
      
      easstats->Delete();
      ptstats->Delete();
      // ptstatsS->Delete();
      latPro->Delete();
      // latProSim->Delete();
      delete c1;
    } catch (AipsError x) {
      cerr << "lateralDistribution::lateralTimeDistribution: " << x.getMesg() << endl;
    }
    return erg;
  }
} // Namespace CR -- end


#endif
