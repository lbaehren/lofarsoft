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

  Record lateralDistribution::fitLateralDistribution (const string filePrefix,
                                                      map <int, PulseProperties> pulsesRec,
                                                      map <int, PulseProperties> pulsesSim,
                                                      int Gt, double az, double ze,
                                                      const string& index1,
                                                      const string& index2)
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
      
      cout << "\nFitting lateral distribution for simulations and data..." << endl;
      
      // create arrays for plotting and fitting LOPES data and REAS sim
      unsigned int Nant = pulsesRec.size();
      double fieldStr[Nant],distance[Nant],fieldStrSim[Nant];
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
            cerr << "\nlateralDistribution::fitLateralDistribution: antenna IDs of data and simulation do not match. Aborting...\n" << endl;
            //return Record();
            continue; //this antennas are not counted!
          }         
          fieldStrSim[ant] = pulsesSim[antID[ant]].height;
          fieldStrErSim[ant] = pulsesSim[antID[ant]].heightError;
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
        if ( distance[i] > maxdist) {
          maxdist=distance[i];
        }
        if (fieldStrSim[i]>fieldMax) {
          fieldMax=fieldStrSim[i];
        }
        if (fieldStr[i]>fieldMax) {
          fieldMax=fieldStr[i];
        }
        if (fitSim) {
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
      cout << "Mean distance: " << latMeanDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      latPro->SetFillColor(4);
      latPro->SetLineColor(4);
      latPro->SetMarkerColor(4);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Lateral distribution - " << Gt;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetXaxis()->SetRange(0,maxdist*1.1);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(0,fieldMax*3);

      TGraphErrors *latProSim = new TGraphErrors (ant, distance,fieldStrSim,distanceErSim,fieldStrErSim);
      latProSim->SetFillColor(2);
      latProSim->SetLineColor(2);
      latProSim->SetMarkerColor(2);
      latProSim->SetMarkerStyle(20);
      latProSim->SetMarkerSize(1.1);
 
      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","lateral distribution");
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
      latPro->SetMinimum(1);
      latPro->SetMaximum(fieldMax*3);
      latPro->SetTitle("");
      latPro->SetFillColor(0);
      latPro->GetYaxis()->SetRangeUser(1,fieldMax*3);
      latPro->GetXaxis()->SetRangeUser(0,maxdist*1.1);
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
          epsName = "#epsilon_{0}- " + index1;
          R0Name = "R_{0}- " + index1;
        } else {
          epsName = "#epsilon_{0}";
          R0Name = "R_{0}";
        }
      
        // fit exponential
        TF1 *fitfuncExp;
        fitfuncExp=new TF1("fitfuncExp","[0]*exp(-x/[1])",0.,maxdist*1.1);
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
            epsName = "#epsilon_{0}- " + index2;
            R0Name = "R_{0}- " + index2;
          } else {
            epsName = "#epsilon_{0}";
            R0Name = "R_{0}";
          }
          TF1 *fitfuncExpS;
          fitfuncExpS=new TF1("fitfuncExpS","[0]*exp(-x/[1])",0.,maxdist*1.1);
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
        }

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << Gt << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
      }  
    } catch (AipsError x) {
      cerr << "lateralDistribution::fitLateralDistribution: " << x.getMesg() << endl;
    }
    return erg;
  }


  void lateralDistribution::lateralTimeDistribution(const string& filePrefix,
                                                    map <int, PulseProperties>& pulses,
                                                    Record& erg)
  {
    try {
      cout << "\nPlotting time vs distance" << endl;

      // Get shower properties
      unsigned int GT       = erg.asuInt  ("Date");
      double       az       = erg.asDouble("Azimuth");
      double       el       = erg.asDouble("Elevation");
      double       ccCenter = erg.asDouble("CCcenter");

      // create arrays for plotting and fitting
      unsigned int Nant = pulses.size();
      Double_t timeVal  [Nant], distance  [Nant];
      Double_t timeValEr[Nant], distanceEr[Nant];
      int antennaNumber [Nant];
      int antID         [Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;  // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulses.begin(); it != pulses.end(); ++it) {
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
        ++ant;
      }

      // consistancy check
      if (ant != Nant)
        cerr << "lateralDistribution::fitTimeDistribution: Nant != number of antenna values\n" << endl; 

      Double_t timeMax=0;
      Double_t timeMin=0;
      Double_t maxdist=0;
      Double_t mindist=0;

      timeMin = *min_element(timeVal, timeVal + Nant);
      timeMax = *max_element(timeVal, timeVal + Nant);
      mindist = *min_element(distance, distance + Nant);
      maxdist = *max_element(distance, distance + Nant);

      cout << "\nAntennas in the Plot/Fit (no cuts appplied): " << ant << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (ant > 0)
        latMeanDist = Mean(ant, distance);
      cout << "Mean distance of antennas: " << latMeanDist << " m." << endl;
      // erg.define("latMeanDist",latMeanDist);
      // TODO: Define it differently or use the values which were previously calculated    

      TGraphErrors *latPro      = new TGraphErrors (ant, distance, timeVal, distanceEr, timeValEr);
      double offsetY = 100;
      double offsetX = 50;
      latPro->SetFillColor(1);
      latPro->SetLineColor(4);
      latPro->SetMarkerColor(4);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "radius of curvature - " << GT;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("time T [ns]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRangeUser(timeMin - offsetY, timeMax + offsetY);
      latPro->GetXaxis()->SetRangeUser(mindist - offsetX, maxdist + offsetX);

      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","radius of curvature");
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
      latPro->SetFillColor(0);
      latPro->Draw("AP");

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
      label << "GT " << GT;
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
      label << 90-el << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      /* FIT */
      if (ant >= 3) {
        // fit polynomial
        TF1 *fitFunc;
        fitFunc=new TF1("fitFunc","[0]+[1]*(sqrt([2]**2+x**2)-[2])",50,190);
        fitFunc->SetParName(0,"offset");
        fitFunc->SetParName(1,"1/c");
        fitFunc->SetParName(2,"R_curv");
        fitFunc->SetParameter(0,0);
        fitFunc->FixParameter(1,1e9/lightspeed);
        fitFunc->SetParameter(2,1000);
        fitFunc->GetXaxis()->SetRange(20,300);
        fitFunc->SetFillStyle(0);
        fitFunc->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latPro->Fit(fitFunc, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("latTime_offset",fitFunc->GetParameter(0));
        erg.define("latTime_R_curv",fitFunc->GetParameter(2));
        erg.define("latTime_sigoffset",fitFunc->GetParError(0));
        erg.define("latTime_sigR_curv",fitFunc->GetParError(2));
        erg.define("latTime_chi2NDF",fitFunc->GetChisquare()/double(fitFunc->GetNDF()));
        cout << "Results of fit"
             << "offset = " << fitFunc->GetParameter(0) << "\t +/- " << fitFunc->GetParError(0) << "\t ns\n"
             << "R_curv = " << fitFunc->GetParameter(2) << "\t +/- " << fitFunc->GetParError(2) << "\t m\n"
             << "Chi^2  = " << fitFunc->GetChisquare() << "\t NDF " << fitFunc->GetNDF() << "\n"
             << "Curvature radius of CC-beam (to compare) = " << erg.asDouble("Distance") << " m\n"
             << endl;

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << GT <<".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
      } else {
        cout<<"No fit was done, because less than 3 antennas are 'good':\n";
      }
    } catch (AipsError x) {
      cerr << "lateralDistribution::lateralTimeDistribution: " << x.getMesg() << endl;
    }
  }
} // Namespace CR -- end


#endif
