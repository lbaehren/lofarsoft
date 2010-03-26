/*-------------------------------------------------------------------------*
 | $Id:: lateralDistribution.cc 4511 2010-03-24 15:24:48Z baehren         $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Frank Schroeder (<mail>)                                              *
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

  void lateralDistribution::fitLateralDistribution (const string& filePrefix,
                                                    map <int, PulseProperties>& pulses,
                                                    Record& erg,
                                                    bool fitPowerLaw)
  {
    try {
      cout << "\nFitting lateral distribution..." << endl;
      
      // for investigation of suspicious antenna (mark it in green)
      int greenAnt = 0;  // set it to zero if now antenna should be green
      int greenValue = -1; // is set to array index which contains point information

      // get shower properties
      unsigned int GT = erg.asuInt("Date");
      double az = erg.asDouble("Azimuth");
      double el = erg.asDouble("Elevation");
      
      // for debugging: mark antenna 4 for the dual polarization setup in green
      // if (GT > 1165574694)
      //   greenAnt = 4;

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");

      // create arrays for plotting and fitting
      unsigned int Nant = pulses.size();
      Double_t fieldStr[Nant],distance[Nant],distanceClean[Nant],fieldStrClean[Nant];
      Double_t fieldStrEr[Nant],distanceEr[Nant],distanceCleanEr[Nant],fieldStrCleanEr[Nant];
      Double_t noiseBgr[Nant], timePos[Nant];
      int antennaNumber[Nant];
      int antID[Nant], antIDclean[Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;        // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulses.begin(); it != pulses.end(); ++it) {
        distance[ant] = (*it).second.dist;
        distanceEr[ant] = (*it).second.disterr;
        fieldStr[ant] = (*it).second.envelopeMaximum;
        noiseBgr[ant] = (*it).second.noise;
        timePos[ant] = (*it).second.envelopeTime;
        antennaNumber[ant] = (*it).second.antenna;
        antID[ant] = (*it).first;
        // look if this antenna shall be marked in green in the plot
        if (antennaNumber[ant] == greenAnt)
          greenValue = ant;
        ++ant; // increase counter  
      }

      // consistancy check
      if (ant != Nant)
        cerr << "lateralDistribution::fitLateralDistribution: Nant != number of antenna values\n" << endl; 

      Double_t fieldMax=0;
      Double_t fieldMin=0;
      Double_t maxdist=0;

      // for cuts (counters)
      unsigned int CutCloseToCore = 0;
      unsigned int CutSmallSignal = 0;
      unsigned int CutBadTiming = 0;
      unsigned int CutSNR = 0;
      double ccCenter = erg.asDouble("CCcenter");
      //double xCenter = erg.asDouble("Xcenter");

      // calculate errors and count number of clean (good) values
      unsigned int clean = 0;
      cout << "\nApplying quality cuts..." << endl;
      for (unsigned int i = 0; i < Nant; ++i) {
        /* error of field strength = 19% + noise */
        fieldStrEr[i]=fieldStr[i]*0.19+noiseBgr[i];
        //distanceEr[i]=15;                // should probably be calculated instead

        /* get largest distance and min and max field strength */
        if ( distance[i] > maxdist)
          maxdist=distance[i];
        if (fieldStr[i] > fieldMax)
          fieldMax=fieldStr[i];
        if (fieldStr[i] < fieldMin)
          fieldMin=fieldStr[i];

        /* New Cuts */
        // pulse time correct? (default: pulse at cc-beam center +/- 25ns)
        if (abs(timePos[i]*1e-9 - ccCenter) > lateralTimeCut) {
          CutBadTiming++;
          cout << "lateralDistribution::fitLateralDistribution: Antenna " << antennaNumber[i] << " cut because of bad timing: "
               << "CCcenter = " << ccCenter*1e9 << " , pulse time = " << timePos[i]
               << endl;
          pulses[antID[i]].lateralCut = true;
          continue;
        }
        // SNR >= 1 ?
        if ( abs(fieldStr[i]/noiseBgr[i]) < lateralSNRcut) {
          CutSNR++;
          cout << "lateralDistribution::fitLateralDistribution: Antenna " << antennaNumber[i] 
               << " cut because SNR lower than " << lateralSNRcut
               << endl;
          pulses[antID[i]].lateralCut = true;
          continue;
        }

        // store value as good value if it passed all the cuts
        distanceClean[clean]  = distance[i];
        distanceCleanEr[clean]= distanceEr[i];
        fieldStrClean[clean]  = fieldStr[i];
        fieldStrCleanEr[clean]= fieldStrEr[i];
        antIDclean[clean]     = antID[i];
        clean++;
      }

      // store number of cut antennas
      erg.define("CutCloseToCore",CutCloseToCore);
      erg.define("CutSmallSignal",CutSmallSignal);
      erg.define("CutBadTiming",CutBadTiming);
      erg.define("CutSNR",CutSNR);

      cout << "\nAntennas in the Plot: " << ant << endl;
      cout << "Entries for fit: " << clean << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (clean > 0)
        latMeanDist = Mean(clean, distanceClean);
      cout << "Mean distance of antennas surviving the cuts: " << latMeanDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      TGraphErrors *latProClean = new TGraphErrors (clean, distanceClean,fieldStrClean,distanceCleanEr,fieldStrCleanEr);
      latPro->SetFillColor(1);
      latPro->SetLineColor(2);
      latPro->SetMarkerColor(2);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Lateral distribution - " << GT;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(0,100);

      latProClean->SetLineColor(4);
      latProClean->SetMarkerColor(4);
      latProClean->SetMarkerStyle(20);
      latProClean->SetMarkerSize(1.1);


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
      latPro->SetMaximum((fieldMax*3));
      latPro->SetTitle("");
      //    latPro->SetTitle("Lateral 1160026193");
      latPro->SetFillColor(0);
      latPro->GetYaxis()->SetRangeUser(1,fieldMax);
      latPro->GetXaxis()->SetRangeUser(0,(maxdist*1.10));
      latPro->Draw("AP");
      latProClean->Draw("same p");

      // mark point of one antenna in green (for debugging)
      if (greenValue > 0) {
        TGraph *latProGreen = new TGraph(1, &distance[greenValue], &fieldStr[greenValue]);
        latProGreen->SetMarkerColor(3);
        latProGreen->SetMarkerStyle(20);
        latProGreen->SetMarkerSize(1.1);
        latProGreen->Draw("same p");
      }  


      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latProClean->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latProClean->GetListOfFunctions());

      /* statistic box of EAS parameter */
      TPaveStats *easstats = new TPaveStats(0.45,0.84,0.62,0.99,"brNDC");
      easstats->SetBorderSize(2);
      easstats->SetTextAlign(12);
      easstats->SetFillColor(0);
      easstats->SetOptStat(0);
      easstats->SetOptFit(0);
      easstats->SetName("stats");

      // create labels for the plot
      label.str("");
      label << "GT " << GT;
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
      label << 90-el << "^{o}";
      text = easstats->AddText(label.str().c_str());
      easstats->Draw();

      /* FIT */
      // do fit only if there are at least 3 good antennas left (otherwise set parameters to 0)!
      if (clean >= 3) {
        // store number of antennas used for fit
        erg.define("NlateralAntennas",clean);

        // fit exponential
        TF1 *fitfuncExp;
        fitfuncExp=new TF1("fitfuncExp","[0]*exp(-x/[1])",50,190);
        fitfuncExp->SetParName(0,"#epsilon_{0}");
        fitfuncExp->SetParName(1,"R_{0}");
        fitfuncExp->SetParameter(1,200);
        fitfuncExp->GetXaxis()->SetRange(20,300);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        latProClean->Fit(fitfuncExp, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        erg.define("R_0",fitfuncExp->GetParameter(1));
        erg.define("sigeps",fitfuncExp->GetParError(0));
        erg.define("sigR_0",fitfuncExp->GetParError(1));
        erg.define("chi2NDF",fitfuncExp->GetChisquare()/double(fitfuncExp->GetNDF()));
        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps    = " << fitfuncExp->GetParameter(0) << "\t +/- " << fitfuncExp->GetParError(0) << "\t µV/m/MHz\n"
             << "R_0    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t m\n"
             << "Chi^2  = " << fitfuncExp->GetChisquare() << "\t NDF " << fitfuncExp->GetNDF() << "\n"
             << endl;

        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << GT << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());

        // calculate residuals 
        double eps = fitfuncExp->GetParameter(0);
        double r_0 = fitfuncExp->GetParameter(1);
        for (unsigned int i=0 ; i < clean; ++i) {
          double calculatedExp = eps * exp(-distanceClean[i]/r_0);
          double deviationExp  = 1. - fieldStrClean[i]/calculatedExp;
 
          pulses[antIDclean[i]].lateralExpHeight = calculatedExp;
          pulses[antIDclean[i]].lateralExpDeviation = deviationExp;
            /* debug output 
            cout << "Residuals for Ant " << antIDclean[i] << ": "
                 << "Exp: " << deviationExp*100 << "  \% \t"
                 << endl;
            */
        }

        // fit power law
        if (fitPowerLaw) {
          TF1 *fitfuncPow;
          fitfuncPow=new TF1("fitfuncPow","[0]*x^[1]");
          fitfuncPow->SetParName(0,"#epsilon_{0}");
          fitfuncPow->SetParName(1,"k");
          fitfuncPow->GetXaxis()->SetRange(20,300);
          fitfuncPow->SetFillStyle(0);
          fitfuncPow->SetLineWidth(2);
          //fitfuncPow->SetLineColor(3);

          cout << "------------------------------"<<endl;
          latProClean->Fit(fitfuncPow, "");
          ptstats->Draw();

          // write fit results to record with other results
          erg.define("epsPow",fitfuncPow->GetParameter(0));
          erg.define("kPow",fitfuncPow->GetParameter(1));
          erg.define("sigepsPow",fitfuncPow->GetParError(0));
          erg.define("sigkPow",fitfuncPow->GetParError(1));
          erg.define("chi2NDFPow",fitfuncPow->GetChisquare()/double(fitfuncPow->GetNDF()));
          cout << "Result of power law fit epsPow * x ^ kPow:\n"
               << "epsPow = " << fitfuncPow->GetParameter(0) << "\t +/- " << fitfuncPow->GetParError(0) << "\t µV/m/MHz\n"
               << "kPow   = " << fitfuncPow->GetParameter(1) << "\t +/- " << fitfuncPow->GetParError(1) << "\t m\n"
               << "Chi^2  = " << fitfuncPow->GetChisquare() << "\t NDF " << fitfuncPow->GetNDF() << "\n"
               << endl;

          // write plot to file
          plotNameStream.str("");
          plotNameStream << filePrefix << GT << "_pow.eps";
          cout << "\nCreating plot: " << plotNameStream.str() << endl;
          c1->Print(plotNameStream.str().c_str());

          // calculate residuals 
          double epsPow = fitfuncPow->GetParameter(0);
          double kPow = fitfuncPow->GetParameter(1);
          for (unsigned int i=0 ; i < clean; ++i) {
            double calculatedPow = epsPow * pow(distanceClean[i], kPow);
            double deviationPow  = 1. - fieldStrClean[i]/calculatedPow;

            pulses[antIDclean[i]].lateralPowHeight = calculatedPow;
            pulses[antIDclean[i]].lateralPowDeviation = deviationPow;
            /* debug output 
            cout << "Residuals for Ant " << antIDclean[i] << ": "
                 << "Exp: " << deviationExp*100 << "  \% \t"
                 << "Pow: " << deviationPow*100 << "  \%"
                 << endl;
            */
          }  
        }
      } else {
        erg.define("eps",0.);
        erg.define("R_0",0.);
        erg.define("sigeps",0.);
        erg.define("sigR_0",0.);
        erg.define("chi2NDF",0.);
        erg.define("epsPow",0.);
        erg.define("kPow",0.);
        erg.define("sigepsPow",0.);
        erg.define("sigkPow",0.);
        erg.define("chi2NDFPow",0.);
        erg.define("NlateralAntennas",0);
        cout << "No fit was done, because less than 3 antennas are 'good':\n"
             << "eps = " << 0 << "\t +/- " << 0 << "\t µV/m/MHz\n"
             << "R_0 = " << 0 << "\t +/- " << 0 << "\t m\n"
             << endl;
      }

    } catch (AipsError x) {
      cerr << "lateralDistribution::fitLateralDistribution: " << x.getMesg() << endl;
    }
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
      //double       xCenter  = erg.asDouble("Xcenter");

      // get antenna positions and distances in shower coordinates
      Vector <double> distances = erg.asArrayDouble("distances");

      // create arrays for plotting and fitting
      unsigned int Nant = pulses.size();
      Double_t timeVal  [Nant], distance  [Nant], distanceClean  [Nant], timeValClean  [Nant];
      Double_t timeValEr[Nant], distanceEr[Nant], distanceCleanEr[Nant], timeValCleanEr[Nant];
      Double_t noiseBgr [Nant], fieldStr  [Nant];
      int antennaNumber [Nant];
      int antID         [Nant], antIDclean[Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;  // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulses.begin(); it != pulses.end(); ++it) {
        // Pulse time relative to shower plane and relative to CC time
        // = geom. delay from beam forming (as it is substracted during analysis) + 
        // time of pulse in trace (- CC beam time, to get an average of 0).
        timeVal      [ant] = (*it).second.distZ / lightspeed * 1e9,    // defined in Math/Constants.h
        timeVal      [ant] += (*it).second.geomDelay;
        timeVal      [ant] += (*it).second.envelopeTime - ccCenter*1e9;
        // pulse time error: Error from shower plane (geometry) + time calibration (2 ns)
        timeValEr    [ant] = sqrt(  pow((*it).second.distZerr / lightspeed * 1e9,2)
                                  + pow(2.,2));
        distance     [ant] = (*it).second.dist;
        distanceEr   [ant] = (*it).second.disterr;
        fieldStr     [ant] = (*it).second.envelopeMaximum;
        noiseBgr     [ant] = (*it).second.noise;
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

      // for cuts (counters)
      // unsigned int CutBadTiming   = 0;
      unsigned int CutSNR         = 0;

      // calculate errors and count number of clean (good) values
      unsigned int clean = 0;
      cout << "\nApplying quality cuts..." << endl;
      for (unsigned int i = 0; i < Nant; ++i) {
        /* Cuts taken from fitLateralDitribution */
        // pulse time correct? (default: pulse at cc-beam center +/- 25ns)
        /*if (abs(timeVal[i]*1e-9 - ccCenter) > lateralTimeCut) {
          CutBadTiming++;
          cout << "lateralDistribution::fitTimeDistribution: Antenna " << antennaNumber[i] << " cut because of bad timing: "
               << "CCcenter = " << ccCenter*1e9 << " , pulse time = " << timeVal[i]
               << endl;
          continue;
        }*/
        // SNR >= 1 ?
        if ( abs(fieldStr[i]/noiseBgr[i]) < lateralSNRcut) {
          CutSNR++;
          cout << "lateralDistribution::fitTimeDistribution: Antenna " << antennaNumber[i] 
               << " cut because SNR lower than " << lateralSNRcut
               << endl;
          continue;
        }

        // store value as good value if it passed all the cuts
        distanceClean[clean]   = distance[i];
        distanceCleanEr[clean] = distanceEr[i];
        timeValClean[clean]    = timeVal[i];
        timeValCleanEr[clean]  = timeValEr[i];
        antIDclean[clean]      = antID[i];
        clean++;
      }

      cout << "\nAntennas in the Plot: " << ant << endl;
      cout << "Entries for fit: " << clean << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (clean > 0)
        latMeanDist = Mean(clean, distanceClean);
      cout << "Mean distance of antennas surviving the cuts: " << latMeanDist << " m." << endl;
      // erg.define("latMeanDist",latMeanDist);
      // TODO: Define it differently or use the values which were previously calculated    

      TGraphErrors *latPro      = new TGraphErrors (ant, distance, timeVal, distanceEr, timeValEr);
      double offsetY = 100;
      double offsetX = 50;
      TGraphErrors *latProClean = new TGraphErrors (clean, distanceClean,timeValClean,distanceCleanEr,timeValCleanEr);
      latPro->SetFillColor(1);
      latPro->SetLineColor(2);
      latPro->SetMarkerColor(2);
      latPro->SetMarkerStyle(20);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Time vs distance distribution - " << GT;
      latPro->SetTitle(label.str().c_str());
      latPro->GetXaxis()->SetTitle("distance R [m]"); 
      latPro->GetYaxis()->SetTitle("time T [ns]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRangeUser(timeMin - offsetY, timeMax + offsetY);
      latPro->GetXaxis()->SetRangeUser(mindist - offsetX, maxdist + offsetX);

      latProClean->SetLineColor(4);
      latProClean->SetMarkerColor(4);
      latProClean->SetMarkerStyle(20);
      latProClean->SetMarkerSize(1.1);
      // TODO: change the names of latPro and latProClean

      /* Canvas and Plotting */
      TCanvas *c1 = new TCanvas("c1","time vs distance distribution");
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
      latProClean->Draw("same p");

      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latProClean->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latProClean->GetListOfFunctions());

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
      if (clean >= 3) {
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
        latProClean->Fit(fitFunc, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("FTD_offset",fitFunc->GetParameter(0));
        erg.define("FTD_R_curv",fitFunc->GetParameter(2));
        erg.define("FTD_sigoffset",fitFunc->GetParError(0));
        erg.define("FTD_sigR_curv",fitFunc->GetParError(2));
        erg.define("FTD_chi2NDF",fitFunc->GetChisquare()/double(fitFunc->GetNDF()));
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
