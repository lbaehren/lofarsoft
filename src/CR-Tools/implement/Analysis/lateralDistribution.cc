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

  void lateralDistribution::fitLateralDistribution (const string& filePrefix,
                                                    map <int, PulseProperties>& pulses,
                                                    Record& erg)
  {
    try {
      cout << "\nFitting lateral distribution..." << endl;
      cout << "Applying cuts is turned off (since 26 May 2010)." << endl;
      
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

      // create arrays for plotting and fitting
      unsigned int Nant = pulses.size();
      Double_t fieldStr[Nant],distance[Nant];
      Double_t fieldStrEr[Nant],distanceEr[Nant];
      int antennaNumber[Nant];
      int antID[Nant];

      // loop through antennas and fill the arrays
      unsigned int ant = 0;        // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulses.begin(); it != pulses.end(); ++it) {
        distance[ant] = (*it).second.dist;
        distanceEr[ant] = (*it).second.disterr;
        fieldStr[ant] = (*it).second.height;
        fieldStrEr[ant] = (*it).second.heightError;
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

      // looking for extreme value to define axis of plot
      Double_t fieldMax=0;
      Double_t fieldMin=0;
      Double_t maxdist=0;
      for (unsigned int i = 0; i < Nant; ++i) {
        /* get largest distance and min and max field strength */
        if ( distance[i] > maxdist)
          maxdist=distance[i];
        if (fieldStr[i] > fieldMax)
          fieldMax=fieldStr[i];
        if (fieldStr[i] < fieldMin)
          fieldMin=fieldStr[i];
      }

      cout << "\nAntennas in the Plot: " << ant << endl;
      
      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (ant > 0)
        latMeanDist = Mean(ant, distance);
      cout << "Mean distance of antennas: " << latMeanDist << " m." << endl;
      erg.define("latMeanDist",latMeanDist);

      TGraphErrors *latPro = new TGraphErrors (ant, distance,fieldStr,distanceEr,fieldStrEr);
      latPro->SetFillColor(1);
      latPro->SetLineColor(4);
      latPro->SetMarkerColor(4);
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
      if (ant >= 3) {
        // store number of antennas used for fit
        erg.define("NlateralAntennas",ant);

        // fit exponential
        TF1 *fitfuncExp;
        fitfuncExp=new TF1("fitfuncExp","[0]*exp(-x/[1])",50,190);
        //fitfuncExp=new TF1("fitfuncExp","[0]*exp(-(x-100)/[1])",50,190);
        fitfuncExp->SetParName(0,"#epsilon_{0}");
        //fitfuncExp->SetParName(0,"#epsilon_{100}");
        fitfuncExp->SetParName(1,"R_{0}");
        fitfuncExp->SetParameter(1,200);
        //fitfuncExp->FixParameter(1,461.8-285.8);
        fitfuncExp->GetXaxis()->SetRange(20,300);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);


        cout << "------------------------------"<<endl;
        latPro->Fit(fitfuncExp, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        erg.define("R_0",fitfuncExp->GetParameter(1));
        //erg.define("sigeps",fitfuncExp->GetParError(0));
        // for the error of epsilon add another 5 percent calibration uncertainty due to environmental effects
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


        // write plot to file
        stringstream plotNameStream;
        plotNameStream << filePrefix << GT << ".eps";
        cout << "\nCreating plot: " << plotNameStream.str() << endl;
        c1->Print(plotNameStream.str().c_str());

        // calculate residuals 
        double eps = fitfuncExp->GetParameter(0);
        double r_0 = fitfuncExp->GetParameter(1);
        for (unsigned int i=0 ; i < ant; ++i) {
          double calculatedExp = eps * exp(-distance[i]/r_0);
          double deviationExp  = 1. - fieldStr[i]/calculatedExp;
 
          pulses[antID[i]].lateralExpHeight = calculatedExp;
          pulses[antID[i]].lateralExpDeviation = deviationExp;
            /* debug output 
            cout << "Residuals for Ant " << antID[i] << ": "
                 << "Exp: " << deviationExp*100 << "  \% \t"
                 << endl;
            */
        }
      } else {
        erg.define("eps",0.);
        erg.define("R_0",0.);
        erg.define("sigeps",0.);
        erg.define("sigR_0",0.);
        erg.define("chi2NDF",0.);
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

  Record lateralDistribution::fitBothLateralDistribution (const string filePrefix,
                                                          map <int, PulseProperties> pulsesRec,
                                                          map <int, PulseProperties> pulsesSim,
                                                          int Gt, double az, double ze)
  {
    Record erg;
    try {
      cout << "\nFitting lateral distribution for simulations and data..." << endl;
      
      // create arrays for plotting and fitting LOPES data and REAS sim
      unsigned int Nant = pulsesRec.size();
      double fieldStr[Nant],distance[Nant],fieldStrSim[Nant];
      double fieldStrEr[Nant],distanceEr[Nant],fieldStrErSim[Nant],distanceErSim[Nant];
      int antennaNumber[Nant];
      int antID[Nant];

      unsigned int ant = 0;        // counting antennas with pulse information
      for (map <int, PulseProperties>::iterator it=pulsesRec.begin(); it != pulsesRec.end(); ++it) {
	if ( (*it).second.lateralCut ) //so use again the same cut applied for call_pipeline
  	  continue;
        distance[ant] = (*it).second.dist;
        distanceEr[ant] = (*it).second.disterr;
        fieldStr[ant] = (*it).second.height;
        fieldStrEr[ant] = (*it).second.heightError;
        antennaNumber[ant] = (*it).second.antenna;
        antID[ant] = (*it).first;

        //cout<<"rec  "<<fieldStr[ant]<<endl;

        //for simulation
        if (pulsesSim[antID[ant]].antennaID != antID[ant]) { // consistency check
          cerr << "\nlateralDistribution::fitLateralDistribution: antenna IDs of data and simulation do not match. Aborting...\n" << endl;
          return Record();
        }
        
        fieldStrSim[ant] = pulsesSim[antID[ant]].height;
        fieldStrErSim[ant] = pulsesSim[antID[ant]].heightError;
        distanceErSim[ant] = pulsesSim[antID[ant]].disterr;

        ++ant; // increase counter  
      }

      // consistancy check
      if (ant != Nant)
        cerr << "lateralDistribution::fitLateralDistribution: Nant != number of antenna values\n" << endl; 

      Double_t fieldMax=0;
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
        if (fieldStrSim[i]<fieldMin) {
          fieldMin=fieldStrSim[i];
        }
        if (fieldStr[i]<fieldMin) {
          fieldMin=fieldStr[i];
        }
      }

      cout << "\nAntennas in the Plot/Fit (no cuts applied): " << ant << endl;

      // calculate mean distance of the remaining antennas
      double latMeanDist = 0;
      if (ant > 0)
        latMeanDist = Mean(ant, distance);
      //cout << "Mean distance of antennas surviving the cuts: " << latMeanDist << " m." << endl;
      //erg.define("latMeanDist",latMeanDist);

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
      latPro->GetYaxis()->SetTitleSize(0.05);
      latPro->GetYaxis()->SetRange(0,100);

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
      latPro->SetMaximum((fieldMax*3));
      latPro->SetTitle("");
      latPro->SetFillColor(0);
      latPro->GetYaxis()->SetRangeUser(1,fieldMax);
      latPro->GetXaxis()->SetRangeUser(0,(maxdist*1.10));
      latPro->Draw("AP");
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
      ptstatsS->Draw();
      latProSim->GetListOfFunctions()->Add(ptstatsS);
      ptstatsS->SetParent(latProSim->GetListOfFunctions());

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

      /* FIT for data */
      // do fit only if there are at least 3 antennas (otherwise set parameters to 0)!
      if (ant >= 3) {
        // fit exponential
        TF1 *fitfuncExp;
        fitfuncExp=new TF1("fitfuncExp","[0]*exp(-x/[1])",50,190);
        //fitfuncExp=new TF1("fitfuncExp","[0]*exp(-(x-100)/[1])",50,190);
        fitfuncExp->SetParName(0,"#epsilon_{0}");
        //fitfuncExp->SetParName(0,"#epsilon_{100}");
        fitfuncExp->SetParName(1,"R_{0}");
        fitfuncExp->SetParameter(1,200);
        //fitfuncExp->FixParameter(1,461.8-285.8);
        fitfuncExp->GetXaxis()->SetRange(20,300);
        fitfuncExp->SetFillStyle(0);
        fitfuncExp->SetLineWidth(2);

        /* FIT for SIMULATIONS */
        // fit exponential
        TF1 *fitfuncExpS;
        fitfuncExpS=new TF1("fitfuncExpS","[0]*exp(-x/[1])",50,190);
        //fitfuncExpS=new TF1("fitfuncExpS","[0]*exp(-(x-100)/[1])",50,190);
        fitfuncExpS->SetParName(0,"#epsilonSim_{0}");
        //fitfuncExpS->SetParName(0,"#epsilon_{100}");
        fitfuncExpS->SetParName(1,"RSim_{0}");
        fitfuncExpS->SetParameter(1,200);
        //fitfuncExpS->FixParameter(1,461.8-285.8);
        fitfuncExpS->GetXaxis()->SetRange(20,300);
        fitfuncExpS->SetFillStyle(0);
        fitfuncExpS->SetLineWidth(2);


        cout << "------------------------------"<<endl;
        latPro->Fit(fitfuncExp, "");
        ptstats->Draw();

        // write fit results to record with other results
        erg.define("eps",fitfuncExp->GetParameter(0));
        erg.define("R_0",fitfuncExp->GetParameter(1));
        //erg.define("sigeps",fitfuncExp->GetParError(0));
        double sigmaEpsilon = sqrt(pow(fitfuncExp->GetParError(0),2)+pow((0.19*fitfuncExp->GetParameter(0)),2));
        erg.define("sigeps",sigmaEpsilon);
        erg.define("sigR_0",fitfuncExp->GetParError(1));
        erg.define("chi2NDF",fitfuncExp->GetChisquare()/double(fitfuncExp->GetNDF()));

        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps    = " << fitfuncExp->GetParameter(0) << "\t +/- " << fitfuncExp->GetParError(0) << "\t µV/m/MHz\n"
             << "total error of eps (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n"
             << "R_0    = " << fitfuncExp->GetParameter(1) << "\t +/- " << fitfuncExp->GetParError(1) << "\t m\n"
             << "Chi^2  = " << fitfuncExp->GetChisquare() << "\t NDF " << fitfuncExp->GetNDF() << "\n"
             << endl;

        cout << "-------- SIMULATIONS ---------"<<endl;
        latProSim->Fit(fitfuncExpS, "");
        ptstatsS->Draw();

        // write fit results to record with other results
        erg.define("eps_sim",fitfuncExpS->GetParameter(0));
        erg.define("R_0sim",fitfuncExpS->GetParameter(1));
        //erg.define("sigeps",fitfuncExp->GetParError(0));
        double sigmaEpsilon_sim = sqrt(pow(fitfuncExpS->GetParError(0),2)+pow((0.19*fitfuncExpS->GetParameter(0)),2)); ///????????????
        erg.define("sigeps_sim",sigmaEpsilon_sim);
        erg.define("sigR_0sim",fitfuncExpS->GetParError(1));
        erg.define("chi2NDF_sim",fitfuncExpS->GetChisquare()/double(fitfuncExpS->GetNDF()));

        cout << "Result of exponential fit eps * e^(-x/R_0):\n"
             << "eps_sim    = " << fitfuncExpS->GetParameter(0) << "\t +/- " << fitfuncExpS->GetParError(0) << "\t µV/m/MHz\n"
             << "total error of eps_sim (including calibration) = " << sigmaEpsilon << "\t µV/m/MHz\n"
             << "R_0sim    = " << fitfuncExpS->GetParameter(1) << "\t +/- " << fitfuncExpS->GetParError(1) << "\t m\n"
             << "Chi^2_sim  = " << fitfuncExpS->GetChisquare() << "\t NDF " << fitfuncExpS->GetNDF() << "\n"
             << endl;


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
