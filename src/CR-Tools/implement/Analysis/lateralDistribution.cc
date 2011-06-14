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
#include "TGraph2DErrors.h"
#include "TF1.h"
#include "TF2.h"
#include <TH1.h>
#include <TH2.h>
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
    lateralTimeCut(25e-9),
    dataColor(kBlue),  // normally blue (4), in comparison to simulation black (1)
    simColor(kBlue) // blue (4) for iron, red (2) for simulations
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
                                                      const double& fitDistance,
                                                      const bool& fitWithEta)
  {
    Record erg;
    try {
      unsigned int ant = 0;        // counting antennas with pulse information
      // define default values, in case lateral distribution does not work
      erg.define("eps",0.);
      erg.define("epsilon_0m",0.);
      erg.define("R_0",0.);
      erg.define("eta",0.);
      erg.define("sigeps",0.);
      erg.define("sigR_0",0.);
      erg.define("sigeta",0.);
      erg.define("chi2NDF",0.);
      erg.define("latMeanDist",0.);
      erg.define("latMinDist",0.);
      erg.define("latMaxDist",0.);
      erg.define("NlateralAntennas",ant);
      erg.define("fitDistance",fitDistance);

      //for the dispersion fit values
      erg.define("dispersion_RMS_perc",0.);
      erg.define("dispersion_Mean_perc",0.);

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
            cerr << "\nlateralDistribution::fitLateralDistribution: antenna IDs of data and simulation do not match. Skipping...\n"
                 << endl;
            //return Record();
            continue; //this antennas are not counted!
          }
          fieldStrSim[ant] = pulsesSim[antID[ant]].height;
          fieldStrErSim[ant] = pulsesSim[antID[ant]].heightError;
          distanceSim[ant] = pulsesSim[antID[ant]].dist;
          distanceErSim[ant] = pulsesSim[antID[ant]].disterr;
        }

        if (distance[ant] == 0.)
          cout << "\nlateralDistribution::fitLateralDistribution: WARNING: lateral distance of 0 m. \n" << endl;

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
      double maxX = maxdist*1.1;
      double maxY = fieldMax*3.;
      double minX = 0;
      double minY = 1;

      cout << "\nAntennas in the Plot/Fit (no cuts applied): " << ant << endl;
      erg.define("NlateralAntennas",ant);

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      double latMinDist = 0;
      double latMaxDist = 0;
      if (ant > 0) {
        latMeanDist = TMath::Mean(ant, distance);
        latMinDist = *min_element(distance, distance + ant);
        latMaxDist = *max_element(distance, distance + ant);
      }
      cout << "Mean distance (data): " << latMeanDist << " m." << endl;
      cout << "Min distance (data): " << latMinDist << " m." << endl;
      cout << "Max distance (data): " << latMaxDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);
      erg.define("latMinDist",latMinDist);
      erg.define("latMaxDist",latMaxDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      latPro->SetFillColor(0);
      latPro->SetLineColor(dataColor);
      latPro->SetMarkerColor(dataColor);
      latPro->SetMarkerStyle(kFullCircle);
      latPro->SetMarkerSize(1.1);
      stringstream label;
      label << "Lateral distribution - " << Gt;
      //latPro->SetTitle(label.str().c_str());
      latPro->SetTitle("");
      latPro->GetXaxis()->SetTitle("distance R [m]");
      latPro->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
      latPro->GetXaxis()->SetTitleSize(0.05);
      latPro->GetXaxis()->SetLimits(minY,maxX);
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(minY,maxY);
      latPro->GetYaxis()->SetRangeUser(minY,maxY);


      TGraphErrors *latProSim = new TGraphErrors (ant, distanceSim,fieldStrSim,distanceErSim,fieldStrErSim);
      latProSim->SetFillColor(0);
      latProSim->SetLineColor(simColor);
      latProSim->SetMarkerColor(simColor);
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
      TPaveStats *ptstats = new TPaveStats(0.54,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("Data");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetTextColor(dataColor);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      latPro->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(latPro->GetListOfFunctions());

      /* statistic box of fit SIMULATIONS */
      TPaveStats *ptstatsS = new TPaveStats(0.54,0.69,0.98,0.84,"brNDC");
      ptstatsS->SetName("Simulation");
      ptstatsS->SetBorderSize(2);
      ptstatsS->SetTextAlign(12);
      ptstatsS->SetTextColor(simColor);
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
      TPaveStats *easstats = new TPaveStats(0.37,0.84,0.54,0.99,"brNDC");
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
      string SlopeParameterName;
      if (ant >= 3) {
        // define names for statistics
        if (index1 != "") {
          epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}- " + index1 + " [#muV/m/MHz]";
          if (fitWithEta)
            SlopeParameterName = "#eta- " + index1 + " [1/m]";
          else  
            SlopeParameterName = "R_{0}- " + index1 + " [m]";
        } else {
          epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "} [#muV/m/MHz]";
          if (fitWithEta)
            SlopeParameterName = "#eta [1/m]";
          else  
            SlopeParameterName = "R_{0} [m]";
        }

        // fit exponential
        TF1 *fitfuncExp;
        string fitFunction;
        if (fitWithEta)
          fitFunction = "[0]*exp(-(x-"+boost::lexical_cast<string>(fitDistance)+")*[1])";
        else
          fitFunction = "[0]*exp(-(x-"+boost::lexical_cast<string>(fitDistance)+")/[1])";
        fitfuncExp=new TF1("fitfuncExp",fitFunction.c_str(),0.,maxX);
        //fitfuncExp=new TF1("fitfuncExp","[0]*exp(-(x-100)/[1])",50,190);
        fitfuncExp->SetParName(0,epsName.c_str());
        fitfuncExp->SetParameter(0,20);
        fitfuncExp->SetParName(1,SlopeParameterName.c_str());
        if (fitWithEta)
          fitfuncExp->SetParameter(1,0.01);
        else
          fitfuncExp->SetParameter(1,100);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);
        fitfuncExp->SetLineColor(dataColor);

        cout << "------------------------------"<<endl;
        latPro->Fit(fitfuncExp, "R");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        if (fitWithEta) {
          erg.define("eta",fitfuncExp->GetParameter(1));
          erg.define("sigeta",fitfuncExp->GetParError(1));
        } else {
          erg.define("R_0",fitfuncExp->GetParameter(1));
          erg.define("sigR_0",fitfuncExp->GetParError(1));
        }
        // error of epsilon = error of fit + 5 % calibration uncertainty due to environmental effects
        double sigmaEpsilon = sqrt(pow(fitfuncExp->GetParError(0),2)+pow((0.05*fitfuncExp->GetParameter(0)),2));
        erg.define("sigeps",sigmaEpsilon);
        erg.define("chi2NDF",fitfuncExp->GetChisquare()/double(fitfuncExp->GetNDF()));

        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps    = " << fitfuncExp->GetParameter(0) << "\t +/- " << fitfuncExp->GetParError(0) << "\t µV/m/MHz\n"
             << "total error of eps (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n";
        if (fitWithEta)     
          cout << "eta    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t 1/m\n";
        else
          cout << "R_0    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t m\n";
        cout << "Chi^2  = " << fitfuncExp->GetChisquare() << "\t NDF " << fitfuncExp->GetNDF() << "\n"
             << endl;

        if (fitSim) {
          cout << "-------- SIMULATIONS ---------"<<endl;
          // define names for statistics
          if (index2 != "") {
            epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}- " + index2 + " [#muV/m/MHz]";
            if (fitWithEta)
              SlopeParameterName = "#eta- " + index2 + " [1/m]";
            else  
              SlopeParameterName = "R_{0}- " + index2 + " [m]";
          } else {
            epsName = "#epsilon_{"+ boost::lexical_cast<string>( round(fitDistance) ) + "}" + "} [#muV/m/MHz]";
            if (fitWithEta)
              SlopeParameterName = "#eta [1/m]";
            else  
              SlopeParameterName = "R_{0} [m]";
          }
          TF1 *fitfuncExpS;
          fitfuncExpS=new TF1("fitfuncExpS",fitFunction.c_str(),0.,maxX);
          //fitfuncExpS=new TF1("fitfuncExpS","[0]*exp(-(x-100)/[1])",50,190);
          fitfuncExpS->SetParName(0,epsName.c_str());
          fitfuncExpS->SetParameter(0,20);
          fitfuncExpS->SetParName(1,SlopeParameterName.c_str());
          if (fitWithEta)
            fitfuncExpS->SetParameter(1,0.01);
          else
            fitfuncExpS->SetParameter(1,100);
          fitfuncExpS->SetFillStyle(0);
          fitfuncExpS->SetLineWidth(2);
          fitfuncExpS->SetLineColor(simColor);

          latProSim->Fit(fitfuncExpS, "R");
          ptstatsS->Draw();
          
          /* Uncertainty bands for data and simulations */
          // define shaded area
          Int_t shadedRed = TColor::GetColor(255,175,175);
          Int_t shadedBlue = TColor::GetColor(110,110,255);
          Int_t shadedGray = TColor::GetColor(175,175,175);
          TGraph *errorBand = new TGraph(6);
          float minEvaldist = maxX*0.005;
          float maxEvaldist = maxX*0.995;
          double R0forBand = 0;
          if (fitWithEta)
            R0forBand = 1/fitfuncExp->GetParameter(1);
          else  
            R0forBand = fitfuncExp->GetParameter(1);
          double epsForBand = fitfuncExp->GetParameter(0);
          
          // uncertainty band for 35% calibration uncertainty (on measured data)
          double uncertainty = 0.35;          
          errorBand->SetPoint(0,minEvaldist,fitfuncExp->Eval(minEvaldist)*(1.-uncertainty));
          // check if band touches upper left corner         
          if (fitfuncExp->Eval(minEvaldist)*(1.+uncertainty) > maxY) {
            errorBand->SetPoint(1,minEvaldist,maxY);
            errorBand->SetPoint(2,fitDistance+R0forBand*log((1.+uncertainty)*epsForBand/maxY),maxY);
          } else {
            errorBand->SetPoint(1,minEvaldist,fitfuncExp->Eval(minEvaldist)*(1.-uncertainty));
            errorBand->SetPoint(2,minEvaldist,fitfuncExp->Eval(minEvaldist)*(1.+uncertainty));
          }
          errorBand->SetPoint(3,maxEvaldist,fitfuncExp->Eval(maxEvaldist)*(1.+uncertainty));
          if (fitfuncExp->Eval(maxEvaldist)*(1.-uncertainty) < minY) {
            errorBand->SetPoint(4,maxEvaldist,minY);
            errorBand->SetPoint(5,fitDistance+R0forBand*log((1.-uncertainty)*epsForBand/minY),minY);            
          } else {
            errorBand->SetPoint(4,maxEvaldist,fitfuncExp->Eval(maxEvaldist)*(1.+uncertainty));
            errorBand->SetPoint(5,maxEvaldist,fitfuncExp->Eval(maxEvaldist)*(1.-uncertainty));
          }
          
          errorBand->SetFillColor(shadedGray);
          
          errorBand->SetTitle("");
          errorBand->GetXaxis()->SetTitle("distance R [m]"); 
          errorBand->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
          errorBand->GetXaxis()->SetTitleSize(0.05);
          errorBand->GetXaxis()->SetLimits(minX,maxX);
          errorBand->GetYaxis()->SetTitleSize(0.05);
          errorBand->GetYaxis()->SetRange(minY,maxY);
          errorBand->GetYaxis()->SetRangeUser(minY,maxY);
        
          TGraph *errorBandS = new TGraph(6);
          if (fitWithEta)
            R0forBand = 1/fitfuncExpS->GetParameter(1);
          else  
            R0forBand = fitfuncExpS->GetParameter(1);
          epsForBand = fitfuncExpS->GetParameter(0);
          
          // uncertainty band for 40% KASCADE energy uncertainty
          uncertainty = 0.4;
          errorBandS->SetPoint(0,minEvaldist,fitfuncExpS->Eval(minEvaldist)*(1.-uncertainty));
          // check if band touches upper left corner         
          if (fitfuncExpS->Eval(minEvaldist)*(1.+uncertainty) > maxY) {
            errorBandS->SetPoint(1,minEvaldist,maxY);
            errorBandS->SetPoint(2,fitDistance+R0forBand*log((1.+uncertainty)*epsForBand/maxY),maxY);
          } else {
            errorBandS->SetPoint(1,minEvaldist,fitfuncExpS->Eval(minEvaldist)*(1.-uncertainty));
            errorBandS->SetPoint(2,minEvaldist,fitfuncExpS->Eval(minEvaldist)*(1.+uncertainty));
          }
          errorBandS->SetPoint(3,maxEvaldist,fitfuncExpS->Eval(maxEvaldist)*(1.+uncertainty));
          if (fitfuncExpS->Eval(maxEvaldist)*(1.-uncertainty) < minY) {
            errorBandS->SetPoint(4,maxEvaldist,minY);
            errorBandS->SetPoint(5,fitDistance+R0forBand*log((1.-uncertainty)*epsForBand/minY),minY);            
          } else {
            errorBandS->SetPoint(4,maxEvaldist,fitfuncExpS->Eval(maxEvaldist)*(1.+uncertainty));
            errorBandS->SetPoint(5,maxEvaldist,fitfuncExpS->Eval(maxEvaldist)*(1.-uncertainty));
          }
          
          errorBandS->SetFillStyle(3344);
          if (simColor == kBlue)
            errorBandS->SetFillColor(shadedBlue); // for proton
          else  
            errorBandS->SetFillColor(shadedRed); // for iron
             
          errorBandS->SetTitle("");
          errorBandS->GetXaxis()->SetTitle("distance R [m]"); 
          errorBandS->GetYaxis()->SetTitle("field strength #epsilon [#muV/m/MHz]");
          errorBandS->GetXaxis()->SetTitleSize(0.05);
          errorBandS->GetXaxis()->SetLimits(minX,maxX);
          errorBandS->GetYaxis()->SetTitleSize(0.05);
          errorBandS->GetYaxis()->SetRange(minY,maxY);
          errorBandS->GetYaxis()->SetRangeUser(minY,maxY);   

          c1->Clear();
          latPro->Draw("AP");
          //latProSim->Draw("SAME P");
          errorBand->Draw("SAME F");
          errorBandS->Draw("SAME F");
          latPro->Draw("SAME P");
          latProSim->Draw("SAME P");
          ptstats->Draw();
          ptstatsS->Draw();
          easstats->Draw();

          // write fit results to record with other results
          erg.define("eps_sim",fitfuncExpS->GetParameter(0));
          if (fitWithEta) {
            erg.define("eta_sim",fitfuncExpS->GetParameter(1));
            erg.define("sigeta_sim",fitfuncExpS->GetParError(1));
            erg.define("R_0_sim",-1.);
            erg.define("sigR_0_sim",-1.);
          } else {
            erg.define("R_0_sim",fitfuncExpS->GetParameter(1));
            erg.define("sigR_0_sim",fitfuncExpS->GetParError(1));
            erg.define("eta_sim",-1.);
            erg.define("sigeta_sim",-1.);
          }
          // error of epsilon = error of fit + 10 % energy uncertainty
          double sigmaEpsilon_sim = sqrt(pow(fitfuncExpS->GetParError(0),2)+pow((0.10*fitfuncExpS->GetParameter(0)),2));
          erg.define("sigeps_sim",sigmaEpsilon_sim);
          erg.define("chi2NDF_sim",fitfuncExpS->GetChisquare()/double(fitfuncExpS->GetNDF()));

          cout << "Result of exponential fit eps * e^(-x/R_0):\n"
               << "eps_sim    = " << fitfuncExpS->GetParameter(0) << "\t +/- " << fitfuncExpS->GetParError(0) << "\t µV/m/MHz\n"
               << "total error of eps_sim (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n";
          if (fitWithEta)     
            cout << "eta_sim   = " << fitfuncExpS->GetParameter(1) << "\t +/- " << fitfuncExpS->GetParError(1) << "\t 1/m\n";
          else
            cout << "R_0_sim    = " << fitfuncExpS->GetParameter(1) << "\t +/- " << fitfuncExpS->GetParError(1) << "\t m\n";
          cout << "Chi^2_sim  = " << fitfuncExpS->GetChisquare() << "\t NDF " << fitfuncExpS->GetNDF() << "\n"
               << endl;
          //fitfuncExpS->Delete();
        }

        // write plot to file
        stringstream plotNameStream("");
        plotNameStream << filePrefix << Gt << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
        plotNameStream.str("");
        plotNameStream << filePrefix << Gt << ".root";
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
          latDev->SetFillColor(dataColor);
          latDev->SetLineColor(dataColor);
          latDev->SetMarkerColor(dataColor);
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
          plotNameStream.str("");
          plotNameStream << filePrefix << "dev-" << Gt << ".root";
          c1->Print(plotNameStream.str().c_str());
          latDev->Delete();
          //delete latDev;
        }


       ///put to true if you need esp_0m and dispersion % values
       ///dispersion methods to evaluete how good is the fit
        if (false) {

          ///deviation along the y axix; also the deviation in the ortogonal direction to the fit have been tried but with not so much difference
          double percentDev=0.;
          ant = 0;

          double epsilon_0m = fitfuncExp->Eval(0.0);
          erg.define("epsilon_0m",epsilon_0m);

          //hist for the dispersion:
           TCanvas *c2 = new TCanvas("c2","",0,0,600,400);
           c2->Divide(1,1);
           //TH1F *h1 = new TH1F("","",60,-10,10);
           TH1F *h1 = new TH1F("","",60,-100,100);
           h1->SetMarkerStyle(3);
           h1->SetMarkerColor(dataColor);
           h1->SetLineColor(dataColor);
           h1->SetFillColor(dataColor);
           h1->SetFillStyle(3004);
           h1->SetLineWidth(3);
           h1->SetLineStyle(11);
           //h1->GetYaxis()->SetRangeUser(0,5);
           h1->GetYaxis()->SetTitle("deviation % on y");

           /* statistic box of fit */
           TPaveStats *gaussInfo = new TPaveStats(0.62,0.84,0.98,0.99,"brNDC");
           gaussInfo->SetName("Data");
           gaussInfo->SetBorderSize(2);
           gaussInfo->SetTextAlign(12);
           gaussInfo->SetFillColor(0);
           gaussInfo->SetOptStat(0);
           gaussInfo->SetOptFit(111);
           gaussInfo->Draw();

          for (map <int, PulseProperties>::iterator it=pulsesRec.begin(); it != pulsesRec.end(); ++it)
            if ( !(*it).second.lateralCut ) { // don't use an antenna marked to cut
              (*it).second.lateralExpHeight = fitfuncExp->Eval((*it).second.dist);
              (*it).second.lateralExpHeightErr = 0;


              cout<<" x:"<<(*it).second.dist<<" eps100: "<<fitfuncExp->GetParameter(0)<<" and R0: "<<fitfuncExp->GetParameter(1)<<endl;

              ///1st method : deviation point-function only concerning y axix
              (*it).second.lateralExpDeviation = (*it).second.height - (*it).second.lateralExpHeight;
              (*it).second.lateralExpDeviationErr = 0;

              cout<<" deviation : "<<(*it).second.lateralExpDeviation<<endl;
              cout<<" weigh     : "<<(*it).second.heightError<<endl;

              //cout << (*it).second.antenna << " \t " << side1[1] << " \t "
              //    << modSide2 << " \t " << InternAngle << " \t " << (*it).second.lateralExpDeviation << " " << endl;
              ///deviation %
              percentDev = (((*it).second.height/(*it).second.lateralExpHeight) - 1.)*100;

              ++ant;
              c2->cd(1);
              h1->GetListOfFunctions()->Add(gaussInfo);
              gaussInfo->SetParent(h1->GetListOfFunctions());
              h1->Fill(percentDev);
              //h1->Draw();
              h1->DrawNormalized();
              //h1->Fit("gaus");

           }


          gaussInfo->Draw();
          stringstream plotNameStream2;
          plotNameStream2 << "eps100-ydev-perc-" << Gt << ".eps";
          c2->Print(plotNameStream2.str().c_str());

          erg.define("dispersion_RMS_perc",h1->GetRMS());
          erg.define("dispersion_Mean_perc",h1->GetMean());

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
      erg.define("latTime1D_Rcurv",0.);
      erg.define("latTime1D_sigRcurv",0.);
      erg.define("latTime1D_chi2NDF",0.);
      erg.define("latTime2D_Rcurv",0.);
      erg.define("latTime2D_sigRcurv",0.);
      erg.define("latTime2D_chi2NDF",0.);

      erg.define("latTime1D_ConeRho",0.);
      erg.define("latTime1D_sigConeRho",0.);
      erg.define("latTime1D_Conechi2NDF",0.);
      erg.define("latTime2D_ConeRho",0.);
      erg.define("latTime2D_sigConeRho",0.);
      erg.define("latTime2D_Conechi2NDF",0.);

      // create arrays for plotting and fitting
      unsigned int Nant = pulsesRec.size();
      Double_t timeVal  [Nant], distance  [Nant]; // time value at antenna position projected to shower plane
      Double_t timeValEr[Nant], distanceEr[Nant];
      Double_t timeValUnpro  [Nant], timeValUnproEr [Nant]; // time values at antenna positions
      Double_t zshower[Nant],  zshowerEr[Nant];             // z shower coordinate of antenna position
      Double_t timeValSim  [Nant], distanceSim  [Nant];
      Double_t timeValErSim[Nant], distanceErSim[Nant];
      Double_t timeValUnproSim[Nant], timeValUnproErSim [Nant];
      Double_t zshowerSim[Nant], zshowerErSim[Nant];
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
        timeVal       [ant] = (*it).second.distZ / lightspeed * 1e9,    // defined in Math/Constants.h
        timeVal       [ant] += (*it).second.geomDelay;
        timeVal       [ant] += (*it).second.time - ccCenter*1e9;
        // pulse time error: Error from shower plane (geometry) + time calibration (2 ns)
        timeValEr     [ant] = sqrt(  pow((*it).second.distZerr / lightspeed * 1e9,2)
                                  + pow((*it).second.timeError,2));
        // time values without projection to shower plane
        timeValUnpro  [ant] = (*it).second.geomDelay + (*it).second.time - ccCenter*1e9;
        timeValUnproEr[ant] = (*it).second.timeError;
        zshower       [ant] = (*it).second.distZ;
        zshowerEr     [ant] = 0.;
        distance      [ant] = (*it).second.dist;
        distanceEr    [ant] = (*it).second.disterr;
        antennaNumber [ant] = (*it).second.antenna;
        antID         [ant] = (*it).second.antennaID;

        //for simulation
        if (fitSim) {
          if (pulsesSim[antID[ant]].antennaID != antID[ant]) { // consistency check
            cerr << "\nlateralDistribution::lateralTimeDistribution: antenna IDs of data and simulation do not match. Skipping...\n" << endl;
            //return Record();
            continue; //this antennas are not counted!
          }
          timeValSim[ant] = pulsesSim[antID[ant]].time + pulsesSim[antID[ant]].distZ / lightspeed * 1e9;;
          timeValErSim[ant] = pulsesSim[antID[ant]].timeError;
          distanceSim[ant] = pulsesSim[antID[ant]].dist;
          distanceErSim[ant] = pulsesSim[antID[ant]].disterr;
          timeValUnproSim[ant] = pulsesSim[antID[ant]].time;
          timeValUnproErSim[ant] = pulsesSim[antID[ant]].timeError;
          // the 2D fit needs a defined error != 0
          if (timeValUnproErSim[ant] == 0)
            timeValUnproErSim[ant] = 1.;
          zshowerSim[ant] = pulsesSim[antID[ant]].distZ;
          zshowerErSim[ant] = pulsesSim[antID[ant]].distZerr;
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

      TGraphErrors *timePro =
        new TGraphErrors(ant, distance, timeVal, distanceEr, timeValEr);
      double offsetY = 40;
      if (fitSim) {
        //timeMin += 10;
        //timeMax -= 25;
        //timeMin += 13;
        //timeMax -= 22;
        timeMin += 13;
        timeMax -= 26;
      }
      double offsetX = 20;

      timePro->SetFillColor(0);
      timePro->SetLineColor(dataColor);
      timePro->SetMarkerColor(dataColor);
      timePro->SetMarkerStyle(kFullCircle);
      timePro->SetMarkerSize(1.1);
      stringstream label;
      label << "radius of curvature - " << Gt;
      //timePro->SetTitle(label.str().c_str());
      timePro->SetTitle("");
      timePro->GetXaxis()->SetTitle("distance R [m]");
      timePro->GetYaxis()->SetTitle("time t [ns]");
      timePro->GetXaxis()->SetTitleSize(0.05);
      timePro->GetYaxis()->SetTitleSize(0.05);
      timePro->GetYaxis()->SetRangeUser(timeMin, timeMax + offsetY);
      timePro->GetXaxis()->SetLimits(0, maxdist + offsetX);

      TGraph2DErrors *timePro2D =
        new TGraph2DErrors(ant, distance, zshower, timeValUnpro, distanceEr, zshowerEr, timeValUnproEr);
      timePro2D->SetFillColor(0);
      timePro2D->SetLineColor(dataColor);
      timePro2D->SetMarkerColor(dataColor);
      timePro2D->SetMarkerStyle(kFullCircle);
      timePro2D->SetMarkerSize(1.1);
      timePro2D->SetTitle("");
      timePro2D->GetXaxis()->SetTitle("distance R [m]");
      timePro2D->GetYaxis()->SetTitle("z_shower [m]");
      timePro2D->GetZaxis()->SetTitle("time t [ns]");
      timePro2D->GetXaxis()->SetTitleSize(0.05);
      timePro2D->GetYaxis()->SetTitleSize(0.05);
      timePro2D->GetZaxis()->SetTitleSize(0.05);
      //timePro2D->GetZaxis()->SetRangeUser(timeMin, timeMax + offsetY);
      //timePro2D->GetXaxis()->SetRangeUser(mindist - offsetX, maxdist + offsetX);

      TGraphErrors *timeProSim =
        new TGraphErrors (ant, distanceSim, timeValSim, distanceErSim, timeValErSim);
      timeProSim->SetFillColor(simColor);
      timeProSim->SetLineColor(simColor);
      timeProSim->SetMarkerColor(simColor);
      timeProSim->SetMarkerStyle(kFullSquare);
      timeProSim->SetMarkerSize(1.1);
      timeProSim->GetYaxis()->SetRangeUser(timeMin, timeMax + offsetY);
      timeProSim->GetXaxis()->SetLimits(0, maxdist + offsetX);

      TGraph2DErrors *timePro2DSim =
        new TGraph2DErrors (ant, distanceSim, zshowerSim, timeValUnproSim, distanceErSim, zshowerErSim, timeValUnproErSim);
      timePro2DSim->SetFillColor(simColor);
      timePro2DSim->SetLineColor(simColor);
      timePro2DSim->SetMarkerColor(simColor);
      timePro2DSim->SetMarkerStyle(kFullSquare);
      timePro2DSim->SetMarkerSize(1.1);

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
      timePro->Draw("AP");
      if (fitSim)
        timeProSim->Draw("SAME P");

      /* statistic box of fit */
      TPaveStats *ptstats = new TPaveStats(0.6,0.84,0.98,0.99,"brNDC");
      ptstats->SetName("stats");
      ptstats->SetBorderSize(2);
      ptstats->SetTextAlign(12);
      ptstats->SetTextColor(dataColor);
      ptstats->SetFillColor(0);
      ptstats->SetOptStat(0);
      ptstats->SetOptFit(111);
      ptstats->Draw();
      timePro->GetListOfFunctions()->Add(ptstats);
      ptstats->SetParent(timePro->GetListOfFunctions());

      /* statistic box of fit SIMULATIONS */
      TPaveStats *ptstatsS = new TPaveStats(0.6,0.69,0.98,0.84,"brNDC");
      ptstatsS->SetName("Simulation");
      ptstatsS->SetBorderSize(2);
      ptstatsS->SetTextAlign(12);
      ptstatsS->SetTextColor(simColor);
      ptstatsS->SetFillColor(0);
      ptstatsS->SetOptStat(0);
      ptstatsS->SetOptFit(111);
      if (fitSim) {
        ptstatsS->Draw();
        timeProSim->GetListOfFunctions()->Add(ptstatsS);
        ptstatsS->SetParent(timeProSim->GetListOfFunctions());
      }

      /* statistic box of EAS parameter */
      TPaveStats *easstats = new TPaveStats(0.43,0.84,0.6,0.99,"brNDC");
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
      string curvNameS = "";
      string coneName = "";
      string coneNameS = "";
      if (index1 != "") {
        curvName = "r_{c}- " + index1 + " [m]";
        coneName = "#rho- " + index1 + " [rad]";
      } else {
        curvName = "r_{c} [m]";
        coneName = "#rho [rad]";
      }
      if (index2 != "") {
        curvNameS = "r_{c}- " + index2 + " [m]";
        coneNameS = "#rho- " + index2 + " [rad]";
      } else {
        curvNameS = "r_{c} [m]";
        coneNameS = "#rho [rad]";
      }

      /* FIT */
      if (ant >= 3) {
        // spherical fit (1 and 2 dimensional)
        TF1 *fitFunc;
        TF2 *fitFunc2D;
        fitFunc=new TF1("fitFunc","3.335640952*(sqrt([0]**2+x**2)-[0])",0,1000);
        fitFunc2D=new TF2("fitFunc2D","3.335640952*(sqrt(([0]-y)**2+x**2)-[0])",0,1000,-200,200);
        fitFunc->SetParName(0,curvName.c_str());
        fitFunc2D->SetParName(0,curvName.c_str());
        //fitFunc->FixParameter(1,1e9/lightspeed); // = 3.335640952
        fitFunc2D->SetParameter(0,1000);
        fitFunc->SetFillStyle(0);
        fitFunc->SetLineWidth(2);

        cout << "------------------------------"<<endl;
        timePro2D->Fit(fitFunc2D, "", "");
        fitFunc->SetParameter(0,fitFunc2D->GetParameter(0));
        timePro->Fit(fitFunc, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("latTime1D_Rcurv",fitFunc->GetParameter(0));
        erg.define("latTime1D_sigRcurv",fitFunc->GetParError(0));
        erg.define("latTime1D_chi2NDF",fitFunc->GetChisquare()/double(fitFunc->GetNDF()));
        erg.define("latTime2D_Rcurv",fitFunc2D->GetParameter(0));
        erg.define("latTime2D_sigRcurv",fitFunc2D->GetParError(0));
        erg.define("latTime2D_chi2NDF",fitFunc2D->GetChisquare()/double(fitFunc2D->GetNDF()));
        cout << "\nResults of spherical fit (data)\n"
             << "Rcurv(1D) = " << fitFunc->GetParameter(0) << "\t +/- " << fitFunc->GetParError(0) << " m\n"
             << "Chi^2 (1D) = " << fitFunc->GetChisquare() << "\t NDF " << fitFunc->GetNDF() << "\n"
             << "Rcurv(2D) = " << fitFunc2D->GetParameter(0) << "\t +/- " << fitFunc2D->GetParError(0) << " m\n"
             << "Chi^2 (2D) = " << fitFunc2D->GetChisquare() << "\t NDF " << fitFunc2D->GetNDF() << "\n"
             << endl;

        if (fitSim) {
          cout << "-------- SIMULATIONS ---------"<<endl;
          TF1 *fitFuncS;
          TF2 *fitFuncS2D;
          fitFuncS=new TF1("fitFuncS","3.335640952*(sqrt([0]**2+x**2)-[0])",0,1000);
          fitFuncS2D=new TF2("fitFuncS2D","3.335640952*(sqrt(([0]-y)**2+x**2)-[0])",0,1000,-200,200);
          fitFuncS->SetParName(0,curvNameS.c_str());
          fitFuncS2D->SetParName(0,curvNameS.c_str());
          fitFuncS2D->SetParameter(0,1000);
          fitFuncS->SetFillStyle(0);
          fitFuncS->SetLineWidth(2);
          fitFuncS->SetLineColor(simColor);

          cout << "------------------------------"<<endl;
          timePro2DSim->Fit(fitFuncS2D, "", "");
          fitFuncS->SetParameter(1,fitFuncS2D->GetParameter(1));
          timeProSim->Fit(fitFuncS, "");
          ptstatsS->Draw();

          // write fit results to record with other results
          erg.define("latTime1D_Rcurv_sim",fitFuncS->GetParameter(0));
          erg.define("latTime1D_sigRcurv_sim",fitFuncS->GetParError(0));
          erg.define("latTime1D_chi2NDF_sim",fitFuncS->GetChisquare()/double(fitFuncS->GetNDF()));
          erg.define("latTime2D_Rcurv_sim",fitFuncS2D->GetParameter(0));
          erg.define("latTime2D_sigRcurv_sim",fitFuncS2D->GetParError(0));
          erg.define("latTime2D_chi2NDF_sim",fitFuncS2D->GetChisquare()/double(fitFuncS2D->GetNDF()));
          cout << "\nResults of spherical fit (SIMULATION)\n"
               << "Rcurv(1D) = " << fitFuncS->GetParameter(0) << "\t +/- " << fitFuncS->GetParError(0) << " m\n"
               << "Chi^2 (1D) = " << fitFuncS->GetChisquare() << "\t NDF " << fitFuncS->GetNDF() << "\n"
               << "Rcurv(2D) = " << fitFuncS2D->GetParameter(0) << "\t +/- " << fitFuncS2D->GetParError(0) << " m\n"
               << "Chi^2 (2D) = " << fitFuncS2D->GetChisquare() << "\t NDF " << fitFuncS2D->GetNDF() << "\n"
               << endl;
        }

        // write plot to file
        stringstream plotNameStream("");
        plotNameStream << filePrefix << "sphere-" << Gt <<".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());
        plotNameStream.str("");
        plotNameStream << filePrefix << "sphere-" << Gt <<".root";
        c1->Print(plotNameStream.str().c_str());
        fitFunc->Delete();

        // fit cone (one and two dimensional)
        TF1 *fitFuncCone;
        TF2 *fitFuncCone2D;
        fitFuncCone=new TF1("fitFuncCone","3.335640952*(x*sin([0]))",0,1000);
        fitFuncCone2D=new TF2("fitFuncCone2D","3.335640952*(x*sin([0])-y*cos([0]))",0,1000,-200,200);
        fitFuncCone->SetParName(0,coneName.c_str());
        fitFuncCone2D->SetParName(0,coneName.c_str());
        //fitFuncCone->SetParName(1,"offset [ns]");
        //fitFuncCone2D->SetParName(1,"offset [ns]");
        //fitFuncCone2D->FixParameter(1,0);
        //fitFunc->FixParameter(1,1e9/lightspeed); // = 3.335640952
        fitFuncCone2D->SetParameter(0,0);

        cout << "------------------------------"<<endl;
        timePro2D->Fit(fitFuncCone2D, "", "");
        fitFuncCone->SetParameter(0,fitFuncCone2D->GetParameter(0));
        timePro->Fit(fitFuncCone, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("latTime1D_ConeRho",fitFuncCone->GetParameter(0));
        erg.define("latTime1D_sigConeRho",fitFuncCone->GetParError(0));
        erg.define("latTime1D_Conechi2NDF",fitFuncCone->GetChisquare()/double(fitFuncCone->GetNDF()));
        erg.define("latTime2D_ConeRho",fitFuncCone2D->GetParameter(0));
        erg.define("latTime2D_sigConeRho",fitFuncCone2D->GetParError(0));
        erg.define("latTime2D_Conechi2NDF",fitFuncCone2D->GetChisquare()/double(fitFuncCone2D->GetNDF()));
        cout << "\nResults of cone fit (data)\n"
             << "Rho   (1D) = " << fitFuncCone->GetParameter(0)
             << "\t +/- " << fitFuncCone->GetParError(0) << " rad "
             << "\t = " << fitFuncCone->GetParameter(0)*180./3.14159
             << "\t +/- " << fitFuncCone->GetParError(0)*180./3.14159 << " °\n"
             //<< "offset(1D) = " << fitFuncCone->GetParameter(1) << "\t +/- " << fitFuncCone->GetParError(1) << " ns\n"
             << "Chi^2 (1D) = " << fitFuncCone->GetChisquare() << "\t NDF " << fitFuncCone->GetNDF() << "\n"
             << "Rho   (2D) = " << fitFuncCone2D->GetParameter(0)
             << "\t +/- " << fitFuncCone2D->GetParError(0) << " rad "
             << "\t = " << fitFuncCone2D->GetParameter(0)*180./3.14159
             << "\t +/- " << fitFuncCone2D->GetParError(0)*180./3.14159 << " °\n"
             //<< "offset(2D) = " << fitFuncCone2D->GetParameter(1) << "\t +/- " << fitFuncCone2D->GetParError(1) << " ns\n"
             << "Chi^2 (2D) = " << fitFuncCone2D->GetChisquare() << "\t NDF " << fitFuncCone2D->GetNDF() << "\n"
             << endl;

        if (fitSim) {
          cout << "-------- SIMULATIONS ---------"<<endl;
          TF1 *fitFuncConeS;
          TF2 *fitFuncConeS2D;
          // without offset
          fitFuncConeS=new TF1("fitFuncConeS","3.335640952*(x*sin([0]))",0,1000);
          fitFuncConeS2D=new TF2("fitFuncConeS2D","3.335640952*(x*sin([0])-y*cos([0]))",0,1000,-200,200);
          // with offset
          //fitFuncConeS=new TF1("fitFuncConeS","[1]+3.335640952*(x*sin([0]))",0,1000);
          //fitFuncConeS2D=new TF2("fitFuncConeS2D","[1]+3.335640952*(x*sin([0])-y*cos([0]))",0,1000,-200,200);
          fitFuncConeS->SetParName(0,coneNameS.c_str());
          fitFuncConeS2D->SetParName(0,coneNameS.c_str());
          fitFuncConeS->SetParName(1,"offset [ns]");
          fitFuncConeS2D->SetParName(1,"offset [ns]");
          fitFuncConeS2D->SetParameter(1,0);
          //fitFunc->FixParameter(1,1e9/lightspeed); // = 3.335640952
          fitFuncConeS2D->SetParameter(0,0);
          fitFuncConeS->SetFillStyle(0);
          fitFuncConeS->SetLineWidth(2);
          fitFuncConeS->SetLineColor(simColor);

          cout << "------------------------------"<<endl;
          timePro2DSim->Fit(fitFuncConeS2D, "", "");
          fitFuncConeS->SetParameter(0,fitFuncConeS2D->GetParameter(0));
            fitFuncConeS->SetParameter(1,fitFuncConeS2D->GetParameter(1));
          timeProSim->Fit(fitFuncConeS, "");
          ptstats->Draw();

          erg.define("latTime1D_ConeRho_sim",fitFuncConeS->GetParameter(0));
          erg.define("latTime1D_sigConeRho_sim",fitFuncConeS->GetParError(0));
          erg.define("latTime1D_ConeOffset_sim",fitFuncConeS->GetParameter(1));
          erg.define("latTime1D_sigConeOffset_sim",fitFuncConeS->GetParError(1));
          erg.define("latTime1D_Conechi2NDF_sim",fitFuncConeS->GetChisquare()/double(fitFuncConeS->GetNDF()));
          erg.define("latTime2D_ConeRho_sim",fitFuncConeS2D->GetParameter(0));
          erg.define("latTime2D_sigConeRho_sim",fitFuncConeS2D->GetParError(0));
          erg.define("latTime2D_ConeOffset_sim",fitFuncConeS2D->GetParameter(1));
          erg.define("latTime2D_sigConeOffset_sim",fitFuncConeS2D->GetParError(1));
          erg.define("latTime2D_Conechi2NDF_sim",fitFuncConeS2D->GetChisquare()/double(fitFuncConeS2D->GetNDF()));
          cout << "\nResults of cone fit (SIMULATION)\n"
             << "Rho   (1D) = " << fitFuncConeS->GetParameter(0)*180./3.14159
             << "\t +/- " << fitFuncConeS->GetParError(0)*180./3.14159 << " °\n"
             << "offset(1D) = " << fitFuncConeS->GetParameter(1) << "\t +/- " << fitFuncConeS->GetParError(1) << " ns\n"
             << "Chi^2 (1D) = " << fitFuncConeS->GetChisquare() << "\t NDF " << fitFuncConeS->GetNDF() << "\n"
             << "Rho   (2D) = " << fitFuncConeS2D->GetParameter(0)*180./3.14159
             << "\t +/- " << fitFuncConeS2D->GetParError(0)*180./3.14159 << " °\n"
             << "offset(2D) = " << fitFuncConeS2D->GetParameter(1) << "\t +/- " << fitFuncConeS2D->GetParError(1) << " ns\n"
             << "Chi^2 (2D) = " << fitFuncConeS2D->GetChisquare() << "\t NDF " << fitFuncConeS2D->GetNDF() << "\n"
              << endl;
        }

        // write plot to file
        plotNameStream.str("");
        plotNameStream << filePrefix << "cone-" << Gt <<".eps";
        //cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str()); // do not create plot, because it is identical to the first one
        plotNameStream.str("");
        plotNameStream << filePrefix << "cone-" << Gt <<".root";
        c1->Print(plotNameStream.str().c_str());
        fitFuncCone->Delete();
        fitFuncCone2D->Delete();
      } else {
        cout<<"No fit was done, because less than 3 antennas are 'good':\n";
      }

      easstats->Delete();
      ptstats->Delete();
      // ptstatsS->Delete();
      timePro->Delete();
      timePro2D->Delete();
      // timeProSim->Delete();
      delete c1;
    } catch (AipsError x) {
      cerr << "lateralDistribution::lateralTimeDistribution: " << x.getMesg() << endl;
    }
    return erg;
  }
} // Namespace CR -- end


#endif
