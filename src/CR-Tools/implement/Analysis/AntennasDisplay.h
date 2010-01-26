/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Marcin Franc <marcin.franc@gmail.com>                                 *
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

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#ifdef HAVE_ROOT
#include <TArrow.h>
#include <TLine.h>
#include <TLegend.h>
#include <TMarker.h>
#include <TH1F.h>
#include <TCanvas.h>
#endif

/*!
  \class AntennasDisplay

  \brief Display the geometrical arrangement of antennas

  \ingroup Analysis

  \author Marcin Franc

  <b>Note:</b>
  
  For the sake of clear architecture of classes (MVC design pattern)
  utility methods operating on standard data types and methods strictly
  realated to drawing should be put into separate classes. This hasn't been
  done because the architecture of CR-Tools violates most of desing paradigms
  anyway.
  Therefore this note has strictly informational purpose
 */
class AntennasDisplay {

   public:

  // === Structures =============================================================

   //! Structure used to transfer the data about plot's borders
   struct borders
   {
      int nValue;
      int sValue;
      int eValue;
      int wValue;
   };

   //! Enumerator of polarization types
   enum polarizationType
   {
     //! North-South polarization
     NS = 0,
     //! East-West polarization
     EW =1
   };
   
   //! Structure used to transfer the data about positions of antennas not providing any signal
   struct deadAntennasPos
   {
      std::vector<double> xCoordinatesDEAD;
      std::vector<double> yCoordinatesDEAD;
   };

   // === Public Methods ========================================================

   //! Constructor
   AntennasDisplay     ();
   //! Function adding a new antenna
   void addNewAntenna  (double xCoordinate,
			double yCoordinate,
			double magnitude, 
			double time,
			int polarization);
   //! Function setting positions of all antennas
   bool setAllAntennas (std::vector<double> xCoordinatesALL,
			std::vector<double> yCoordinatesALL);
   //! Function setting coordinates of shower core
   void setCore (double xCore,
		 double yCore, 
		 double azimuth,
		 double elevation);

#ifdef HAVE_ROOT
   //! Either one can call createPlot setting all parameters ...
   TCanvas* createPlot (std::string title,
			unsigned int width = 1024,
			unsigned int height = 768,
			unsigned int lineWidth = 3,
			double plotFittingStep = 0.1,
			unsigned int plotsOffset = 10,
			unsigned int bckgFillColor = 10,
			unsigned int deadAntColor = 18,
			unsigned int deadAntMarkerSize = 1,
			unsigned int numberOfColorsInLeg = 48,
			unsigned int distanceOfLegColors = 1,
			unsigned int firstColorOfLeg = 52,
			unsigned int legBorderSize = 0,
			unsigned int legFillColor = 10,
			double legTextSize = 0.025,
			double legStartPosX = 0.90,
			double legStartPosY = 0.10,
			double legEndPosX = 0.98,
			double legEndPosY = 0.90,
			double legEntrySeparation = -0.01,
			double legMarkerSize = 1.25,
			double elevationPlotMax = 20,
			double elevationPlotMin = 1,
			double elevationArrowSize = 0.015);
   //! Save the output of the plot
   bool     savePlot   (std::string filename,
			TCanvas* c);
   // ... or call the function below to invoke createPlot with default parameters
   bool     createPlot (std::string filename,
			std::string title);
#endif
   
 private:

   //____________________________________________________________________________
   //                                                           Private variables

   std::vector<double> xCoordinates;
   std::vector<double> yCoordinates;
   std::vector<double> magnitudes;
   std::vector<double> times;
   std::vector<int>    polarizations;
   
   std::vector<double> xCoordinatesALL;
   std::vector<double> yCoordinatesALL;
   bool                positionsGiven;
   
   double              xCore;
   double              yCore;
   double              azimuth;
   double              elevation;
   bool                showCore;

   // === Private methods - utility functions ===================================

   // Checking the activity of the anntenna having the position indicated by xCoordinate and yCoordinate
   bool                checkActivity        (double xCoordinate,
					     double yCoordinate);
   //! Gets absolute difference between value1 and value2
   double              getAbsDifference     (double value1,
					     double value2);
   //! Gets plot borders
   borders             getBorders           (int plotsOffset);
   //! Gets the positions of dead antennas
   deadAntennasPos     getDeadAntennas      ();
   //! Gets the minimum value of the given vector
   double              getMinimumValue      (std::vector<double>& inputData);
   //! Gets the maximum value of the given vector
   double              getMaximumValue      (std::vector<double>& inputData);
   // Gets minimum distance between antennas and plot borders
   // The speed of fitting relies on the precision of plotFittingStep value
   double              getMinimumDistance   (std::vector<double>& inputData1, std::vector<double>& inputData2, borders borderValues, double plotFittingStep);
   //! Gets the difference of two vectors
   std::vector<double> getSetDifference     (std::vector<double> inputData1,
					     std::vector<double> inputData2);
#ifdef HAVE_ROOT
   //! Gets the horizontal line
   TLine*              getHorizontalLine    (double xCenter,
					     double yCenter,
					     double length,
					     unsigned int thickness);
   //! Gets the vertical line
   TLine*              getVerticalLine      (double xCenter,
					     double yCenter,
					     double length,
					     unsigned int thickness);
#endif

};
