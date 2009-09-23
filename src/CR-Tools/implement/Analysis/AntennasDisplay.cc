#include "AntennasDisplay.h"

AntennasDisplay::AntennasDisplay()
{

   positionsGiven  = false;
   showCore        = false;
}

void AntennasDisplay::addNewAntenna(double xCoordinate, double yCoordinate, double magnitude, double time, int polarization)
{
   xCoordinates.push_back  (xCoordinate);
   yCoordinates.push_back  (yCoordinate);
   magnitudes.push_back    (magnitude);
   times.push_back         (time);
   polarizations.push_back (polarization);
}

bool AntennasDisplay::setAllAntennas(std::vector<double> xCoordinatesALL, std::vector<double> yCoordinatesALL)
{
   if(xCoordinatesALL.size() == yCoordinatesALL.size())
   {
      this->xCoordinatesALL = xCoordinatesALL;
      this->yCoordinatesALL = yCoordinatesALL;
      positionsGiven        = true;
      return true;
   }
   return false;
}

void AntennasDisplay::setCore(double xCore, double yCore, double azimuth, double elevation)
{
   this->xCore     = xCore;
   this->yCore     = yCore;
   this->azimuth   = azimuth;
   this->elevation = elevation;
   showCore        = true;
}

bool AntennasDisplay::checkActivity(double xCoordinate, double yCoordinate)
{
   for(unsigned int i=0; i<xCoordinates.size(); i++)
      if((xCoordinates[i] == xCoordinate) && (yCoordinates[i] == yCoordinate))
         return true;
   return false;
}

double AntennasDisplay::getAbsDifference(double value1, double value2)
{
   return (value1 - value2 >= 0) ? value1 - value2 : (value1 - value2) * (-1);
}

AntennasDisplay::deadAntennasPos AntennasDisplay::getDeadAntennas()
{
   deadAntennasPos output;
   std::vector<double> differenceX = getSetDifference(xCoordinatesALL, xCoordinates);
   std::vector<double> differenceY = getSetDifference(yCoordinatesALL, yCoordinates);

   // We cannot assume that all antennas have unique coordinates
   for(unsigned int i=0; i<differenceX.size(); i++)
   {
      for(unsigned int j=0; j<xCoordinatesALL.size(); j++)
         if((xCoordinatesALL[j] == differenceX[i]) && (!checkActivity(xCoordinatesALL[j], yCoordinatesALL[j])))
         {
            output.xCoordinatesDEAD.push_back(xCoordinatesALL[j]);
            output.yCoordinatesDEAD.push_back(yCoordinatesALL[j]);
         }
   }

   for(unsigned int i=0; i<differenceY.size(); i++)
   {
      for(unsigned int j=0; j<yCoordinatesALL.size(); j++)
         if((yCoordinatesALL[j] == differenceY[i]) && (!checkActivity(xCoordinatesALL[j], yCoordinatesALL[j])))
         {
            output.xCoordinatesDEAD.push_back(xCoordinatesALL[j]);
            output.yCoordinatesDEAD.push_back(yCoordinatesALL[j]);
         }
   }

   return output;
}


double AntennasDisplay::getMinimumValue (std::vector<double>& inputData)
{
   std::vector<double>::const_iterator iterator = min_element(inputData.begin(), inputData.end());
   if(iterator != inputData.end())
      return *iterator;
   return -1;
}

double AntennasDisplay::getMaximumValue (std::vector<double>& inputData)
{
   std::vector<double>::const_iterator iterator = max_element(inputData.begin(), inputData.end());
   if(iterator != inputData.end())
      return *iterator;
   return -1;
}

double AntennasDisplay::getMinimumDistance (std::vector<double>& inputData1, std::vector<double>& inputData2, borders borderValues, double plotFittingStep)
{
   double maximumRadValue = 0;
   bool   overlapped      = false;

   while(!overlapped)
   {
      for(unsigned int i=0; i<inputData1.size(); i++)
      {
         if(!overlapped)
         {
               for(unsigned int j=0; j<inputData1.size(); j++)
               {
                  if((inputData1[i] != inputData1[j]) && (inputData2[i] != inputData2[j]))
                  {
                     if(pow(inputData1[i] - inputData1[j], 2) + pow(inputData2[i] - inputData2[j], 2) <= pow(maximumRadValue, 2))
                     {
                           overlapped = true;
                           break;
                     }
                  }
               }
         }
         if
         (
         (getAbsDifference(inputData1[i], borderValues.eValue) < maximumRadValue) ||
         (getAbsDifference(inputData1[i], borderValues.wValue) < maximumRadValue) ||
         (getAbsDifference(inputData2[i], borderValues.nValue) < maximumRadValue) ||
         (getAbsDifference(inputData2[i], borderValues.sValue) < maximumRadValue)
         )
            overlapped = true;
      }
      if(!overlapped)
         maximumRadValue += plotFittingStep;
   }

   return maximumRadValue*2;
}

std::vector<double> AntennasDisplay::getSetDifference(std::vector<double> inputData1, std::vector<double> inputData2)
{
   std::vector<double>           setDifference(inputData1.size()+inputData2.size());
   std::vector<double>::iterator iterator;

   sort(inputData1.begin(), inputData1.end());
   sort(inputData2.begin(), inputData2.end());

   iterator = set_difference(inputData1.begin(), inputData1.end(), inputData2.begin(), inputData2.end(), setDifference.begin());
   setDifference.erase(iterator, setDifference.end());

   return setDifference;
}

AntennasDisplay::borders AntennasDisplay::getBorders (int plotsOffset)
{
   borders output;

   if(positionsGiven)
   {
      output.nValue = (int)(getMaximumValue(yCoordinatesALL) + plotsOffset);
      output.sValue = (int)(getMinimumValue(yCoordinatesALL) - plotsOffset);
      output.eValue = (int)(getMaximumValue(xCoordinatesALL) + plotsOffset);
      output.wValue = (int)(getMinimumValue(xCoordinatesALL) - plotsOffset);
   }
   else
   {
      output.nValue = (int)(getMinimumValue(yCoordinates) + plotsOffset);
      output.sValue = (int)(getMaximumValue(yCoordinates) - plotsOffset);
      output.eValue = (int)(getMinimumValue(xCoordinates) + plotsOffset);
      output.wValue = (int)(getMaximumValue(xCoordinates) - plotsOffset);
   }

   output.nValue = std::max((int)(getMaximumValue(yCoordinates) + plotsOffset), output.nValue);
   output.sValue = std::min((int)(getMinimumValue(yCoordinates) - plotsOffset), output.sValue);
   output.eValue = std::max((int)(getMaximumValue(xCoordinates) + plotsOffset), output.eValue);
   output.wValue = std::min((int)(getMinimumValue(xCoordinates) - plotsOffset), output.wValue);

   if(showCore)
   {
      output.nValue = std::max(int(yCore + plotsOffset), output.nValue);
      output.sValue = std::min(int(yCore - plotsOffset), output.sValue);
      output.eValue = std::max(int(xCore + plotsOffset), output.eValue);
      output.wValue = std::min(int(xCore - plotsOffset), output.wValue);
   }

   return output;
}

TCanvas* AntennasDisplay::createPlot (std::string title, unsigned int width, unsigned int height, unsigned int lineWidth, double plotFittingStep, unsigned int plotsOffset, unsigned int bckgFillColor, unsigned int deadAntColor, unsigned int deadAntMarkerSize, unsigned int numberOfColorsInLeg, unsigned int distanceOfLegColors, unsigned int firstColorOfLeg, unsigned int legBorderSize, unsigned int legFillColor, double legTextSize, double legStartPosX, double legStartPosY, double legEndPosX, double legEndPosY, double legEntrySeparation, double legMarkerSize, double elevationPlotMax, double elevationPlotMin, double elevationArrowSize)
{
   const unsigned int maxColorValue = 255; // ROOT's limitation

   // Check input data
   if
   (
      (xCoordinates.size() != yCoordinates.size())                             ||
      (xCoordinates.size() == 0)                                               ||
      (xCoordinatesALL.size() != yCoordinatesALL.size())
   )
   {
      std::cerr<<"Incorrect input data"<<std::endl;
      return NULL;
   }
   // Check input parameters
   if
   (
      (distanceOfLegColors == 0)                                               ||
      (bckgFillColor > maxColorValue)                                          ||
      (legFillColor > maxColorValue)                                           ||
      (legTextSize  < 0 || legTextSize > 1)                                    ||
      (legMarkerSize < 0)                                                      ||
      (numberOfColorsInLeg + firstColorOfLeg > maxColorValue)                  ||
      (legStartPosX < 0 || legStartPosX > 1)                                   ||
      (legStartPosY < 0 || legStartPosY > 1)                                   ||
      (legEndPosX < 0 || legEndPosX > 1)                                       ||
      (legEndPosY < 0 || legEndPosY > 1)
   )
   {
      std::cerr<<"Incorrect input parameter";
      return NULL;
   }

   // Create canvas
   TCanvas* c1 = new TCanvas("c1", "c1", width, height);
   c1->SetFillColor(bckgFillColor);

   // Create plot's frame
   borders borderValues = getBorders(plotsOffset);
   std::string plotDescription = title + "; X [m] ; Y [m]";
   TH1F* h1 = c1->DrawFrame(borderValues.wValue, borderValues.sValue, borderValues.eValue, borderValues.nValue, plotDescription.c_str());
   h1->GetXaxis()->CenterTitle();
   h1->GetYaxis()->CenterTitle();

   // Calculate time's multiplication value
   double minimumTimeValue = getMinimumValue(times);
   double maximumTimeValue = getMaximumValue(times);
   double timeMulValue     = (double)(numberOfColorsInLeg) / (maximumTimeValue - minimumTimeValue);
   double timeAddValue     = firstColorOfLeg - timeMulValue * minimumTimeValue;

   // Draw the legend
   TLegend* lg1 = new TLegend(legStartPosX, legStartPosY, legEndPosX, legEndPosY, "Time [ns]");
   lg1->SetEntrySeparation(legEntrySeparation);

   std::vector<TMarker*> vt1;

   for(unsigned int i=numberOfColorsInLeg, j=0; i>0; i-=distanceOfLegColors, j++)
   {
      vt1.push_back(new TMarker(0, 0, kFullSquare));
      vt1[j]->SetMarkerColor(i - 1 + firstColorOfLeg);
      vt1[j]->SetMarkerSize(legMarkerSize);
      if(i == numberOfColorsInLeg)
      {
         std::stringstream ss (std::stringstream::in | std::stringstream::out);
         ss << maximumTimeValue;
         lg1->AddEntry(vt1[j],ss.str().c_str(),"P");
      }
      else if((i - distanceOfLegColors) <= 0)
      {
         std::stringstream ss (std::stringstream::in | std::stringstream::out);
         ss << minimumTimeValue;
         lg1->AddEntry(vt1[j],ss.str().c_str(),"P");
      }
      else
         lg1->AddEntry(vt1[j], " ", "P");
   }


   const static int textAngle = 15;

   lg1->SetFillColor(legFillColor);
   lg1->SetTextSize(legTextSize);
   lg1->SetTextAngle(textAngle);
   lg1->Draw();
   lg1->SetBorderSize(legBorderSize);

   // Calculate magnitude's multiplication value
   double minimumMagnitudeValue = getMinimumValue(magnitudes);
   double maximumMagnitudeValue = getMaximumValue(magnitudes);

   double minimumDistance       = 0;

   if(positionsGiven)
      minimumDistance = getMinimumDistance(xCoordinatesALL, yCoordinatesALL, borderValues, plotFittingStep);
   else
      minimumDistance = getMinimumDistance(xCoordinates, yCoordinates, borderValues, plotFittingStep);

   double magnitudeMulValue     = 0;
   double magnitudeAddValue     = 0;

   if(maximumMagnitudeValue - minimumMagnitudeValue != 0)
   {
      if(minimumMagnitudeValue != 0)
      {
         double minMaxRelation = 0;

         if(maximumMagnitudeValue != 0)
            minMaxRelation = minimumMagnitudeValue / maximumMagnitudeValue;
         if(minMaxRelation < 0)
            minMaxRelation *= -1;
         magnitudeMulValue = (minimumDistance - minMaxRelation * minimumDistance)/ (maximumMagnitudeValue - minimumMagnitudeValue);
      }
      else
         magnitudeMulValue = minimumDistance / (maximumMagnitudeValue - minimumMagnitudeValue);
      magnitudeAddValue = minimumDistance - magnitudeMulValue * maximumMagnitudeValue;
   }
   else
   {
      if(maximumMagnitudeValue != 0)
         magnitudeMulValue = minimumDistance / maximumMagnitudeValue;
   }

   // Draw core direction
   if(showCore)
   {
      const double elevationMax = 0;
      const double elevationMin = 90;

      double elevationMulValue   = (elevationPlotMax - elevationPlotMin) / (elevationMax - elevationMin);
      double elevationAddValue   = elevationPlotMax;
      double elevationMagnitude  = elevation * elevationMulValue + elevationAddValue;
      azimuth                   *= M_PI / 180;

      TArrow* arr = new TArrow(xCore, yCore,
                               elevationMagnitude * sin(azimuth) + xCore,
                               elevationMagnitude * cos(azimuth) + yCore,
                               elevationArrowSize, ">");
      arr->Draw();
   }

   // Draw magnitudes
   std::vector<TLine*> vl1;

   for(unsigned int i=0; i<xCoordinates.size(); i++)
   {
      switch(polarizations[i])
      {
         case (int)NS :
            vl1.push_back(getHorizontalLine(xCoordinates[i], yCoordinates[i], magnitudes[i] * magnitudeMulValue + magnitudeAddValue, lineWidth));
         break;
         case (int)EW :
            vl1.push_back(getVerticalLine(xCoordinates[i], yCoordinates[i], magnitudes[i] * magnitudeMulValue + magnitudeAddValue, lineWidth));
            break;
      }

      int colorVal = 0;

      if(distanceOfLegColors > 1)
      {
         unsigned int mVal = (times[i] * timeMulValue + timeAddValue-firstColorOfLeg) / distanceOfLegColors;
         if(mVal >= numberOfColorsInLeg / distanceOfLegColors)
            mVal--;
         colorVal    = firstColorOfLeg + mVal * distanceOfLegColors;
      }
      else
         colorVal = (int) (times[i] * timeMulValue + timeAddValue);

      vl1[i]->SetLineColor(colorVal);
      vl1[i]->Draw();
   }

   // If all positions are given, antennas not providing signals can be identified
   if(positionsGiven)
   {
      deadAntennasPos deadAntennas = getDeadAntennas();

      std::vector<TMarker*> vm1;

      for(unsigned int i=0; i<deadAntennas.xCoordinatesDEAD.size(); i++)
      {
         vm1.push_back(new TMarker(deadAntennas.xCoordinatesDEAD[i], deadAntennas.yCoordinatesDEAD[i], 5));
         vm1[i]->SetMarkerSize(deadAntMarkerSize);
         vm1[i]->SetMarkerColor(deadAntColor);
         vm1[i]->Draw();
      }
   }
   return c1;
}

bool AntennasDisplay::createPlot(std::string filename, std::string title)
{
   TCanvas* c = createPlot(title);
   if(c != NULL)
      return savePlot(filename, c);
   return true;
}

bool AntennasDisplay::savePlot(std::string filename, TCanvas* c)
{
   std::stringstream plotNameStream(std::stringstream::in | std::stringstream::out);
   plotNameStream << filename << ".eps";
   c->Print(plotNameStream.str().c_str());
   return true;
}

TLine* AntennasDisplay::getVerticalLine (double xCenter, double yCenter, double length, unsigned int thickness)
{
   TLine* output = new TLine(xCenter, yCenter + length / 2, xCenter, yCenter - length / 2);
   output->SetLineWidth(thickness);
   return output;
}

TLine* AntennasDisplay::getHorizontalLine (double xCenter, double yCenter, double length, unsigned int thickness)
{
    TLine* output = new TLine(xCenter + length / 2, yCenter, xCenter - length / 2, yCenter);
    output->SetLineWidth(thickness);
    return output;
}
