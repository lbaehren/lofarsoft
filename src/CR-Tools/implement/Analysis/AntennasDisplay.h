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

// (C) Marcin Franc [marcin.franc@gmail.com]

/*!
  <b>Note:</b>
  
  For the sake of clear architecture of classes (MVC design pattern)
  utility methods operating on standard data types and methods strictly
  realated to drawing should be put into separate classes. This hasn't been
  done because the architecture of CR-Tools violates most of desing paradigms
  anyway.
  Therefore this note has strictly informational purpose
 */
class AntennasDisplay
{
   public:

   // === Necessary structures ===
   // Structure used to transfer the data about plot's borders
   struct borders
   {
      int nValue;
      int sValue;
      int eValue;
      int wValue;
   };

   // Enumerator of polarization types
   enum polarizationType
   {
      NS = 0, EW =1
   };

   // Structure used to transfer the data about positions of antennas not providing any signal
   struct deadAntennasPos
   {
      std::vector<double> xCoordinatesDEAD;
      std::vector<double> yCoordinatesDEAD;
   };
   // ============================

   // === Public methods ===
   // Constructor
   AntennasDisplay     ();
   // Function adding a new antenna
   void addNewAntenna  (double xCoordinate, double yCoordinate, double magnitude, double time, int polarization);
   // Function setting positions of all antennas
   bool setAllAntennas (std::vector<double> xCoordinatesALL, std::vector<double> yCoordinatesALL);
   // Function setting coordinates of shower core
   void setCore (double xCore, double yCore, double azimuth, double elevation);
   // ======================

   //____________________________________________________________________________
   //                                        Default values for creating the plot

   // Width of resulting canvas
   const static int    _width               =  1024;
   // Height of resulting canvas
   const static int    _height              =  768;
   // Width of lines representing singal's magnitudes
   const static int    _lineWidth           =  3;
   // Fitting parameter - the smaller it is, the more accurate maximum length of signal value depicted on the plot one can achieve
   const static double _plotFittingStep     =  0.1;
   // Plots offset
   const static int    _plotsOffset         =  10;
   // Canvas' fill color
   const static int    _bckgFillColor       =  10;
   // Color in which dead antennas are drawn
   const static int    _deadAntColor        =  18;
   // Size of marker indicating dead antenna
   const static int    _deadAntMarkerSize   =  1;
   // Number of colors in the legend
   const static int    _numberOfColorsInLeg =  48;
   // Distance between those colors
   // Please note that if you want to use the color range [x,y] and only n colors from it where n < abs(x-y)
   // you should make the distance be multiple of number of colors in the legend
   const static int    _distanceOfLegColors =  1;
   // First color of the legend
   const static int    _firstColorOfLeg     =  52;
   // Size of bordder surrounding the legend
   const static int    _legBorderSize       =  0;
   // Fill color of legend's field
   const static int    _legFillColor        =  10;
   // Size of the text in the legend field
   const static double _legTextSize         =  0.025;
   // Legend positions (x1, y1, x2, y2)
   const static double _legStartPosX        =  0.90;
   const static double _legStartPosY        =  0.10;
   const static double _legEndPosX          =  0.98;
   const static double _legEndPosY          =  0.90;
   // Distance between legend fields
   const static double _legEntrySeparation  = -0.01;
   // Size of each field in the legend
   const static double _legMarkerSize       =  1.25;
   // Max elevation's line length
   const static double _elevationPlotMax    =  20;
   // Max elevation's line length
   const static double _elevationPlotMin    =  1;
   // Size of elevation's arrow
   const static double _elevationArrowSize  =  0.015;

   //____________________________________________________________________________
   //                                                                     Methods

#ifdef HAVE_ROOT
   // Either one can call createPlot setting all parameters ...
   TCanvas* createPlot (std::string title,
			unsigned int width = _width,
			unsigned int height = _height,
			unsigned int lineWidth = _lineWidth,
			double plotFittingStep = _plotFittingStep,
			unsigned int plotsOffset = _plotsOffset,
			unsigned int bckgFillColor = _bckgFillColor,
			unsigned int deadAntColor = _deadAntColor,
			unsigned int deadAntMarkerSize = _deadAntMarkerSize,
			unsigned int numberOfColorsInLeg = _numberOfColorsInLeg,
			unsigned int distanceOfLegColors = _distanceOfLegColors,
			unsigned int firstColorOfLeg = _firstColorOfLeg,
			unsigned int legBorderSize = _legBorderSize,
			unsigned int legFillColor = _legFillColor,
			double legTextSize = _legTextSize,
			double legStartPosX = _legStartPosX,
			double legStartPosY = _legStartPosY,
			double legEndPosX = _legEndPosX,
			double legEndPosY = _legEndPosY,
			double legEntrySeparation = _legEntrySeparation,
			double legMarkerSize = _legMarkerSize,
			double elevationPlotMax = _elevationPlotMax,
			double elevationPlotMin = _elevationPlotMin,
			double elevationArrowSize = _elevationArrowSize);
   // ... and then save the output ...
   bool     savePlot   (std::string filename, TCanvas* c);
   // ... or call the function below to invoke createPlot with default parameters
   bool     createPlot (std::string filename, std::string title);
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

   //____________________________________________________________________________
   //                                         Private methods - utility functions

   // Checking the activity of the anntenna having the position indicated by xCoordinate and yCoordinate
   bool                checkActivity        (double xCoordinate,
					     double yCoordinate);
   // Gets absolute difference between value1 and value2
   double              getAbsDifference     (double value1,
					     double value2);
   // Gets plot borders
   borders             getBorders           (int plotsOffset);
   // Gets the positions of dead antennas
   deadAntennasPos     getDeadAntennas      ();
   // Gets the horizontal line
#ifdef HAVE_ROOT
   TLine*              getHorizontalLine    (double xCenter,
					     double yCenter,
					     double length,
					     unsigned int thickness);
#endif
   // Gets the minimum value of the given vector
   double              getMinimumValue      (std::vector<double>& inputData);
   // Gets the maximum value of the given vector
   double              getMaximumValue      (std::vector<double>& inputData);
   // Gets minimum distance between antennas and plot borders
   // The speed of fitting relies on the precision of plotFittingStep value
   double              getMinimumDistance   (std::vector<double>& inputData1, std::vector<double>& inputData2, borders borderValues, double plotFittingStep);
   // Gets the difference of two vectors
   std::vector<double> getSetDifference     (std::vector<double> inputData1,
					     std::vector<double> inputData2);
   // Gets the vertical line
#ifdef HAVE_ROOT
   TLine*              getVerticalLine      (double xCenter,
					     double yCenter,
					     double length,
					     unsigned int thickness);
#endif
   // ===========================================

};
