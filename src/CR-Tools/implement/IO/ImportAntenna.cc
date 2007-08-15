/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Tim Huege (<tim.huege@ik.fzk.de>)                                     *
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

/* $Id: importAntenna.cc,v 1.3 2006/10/16 13:07:45 huege Exp $*/

#include <IO/ImportAntenna.h>

namespace CR { // Namespace CR -- begin
  
  // degree to radians
  const double ImportAntenna::rad = M_PI/180.0;
  // warn if energy fraction in projected pulses falls below this limit
  const double ImportAntenna::minimumEfficiency = 0.995;
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ImportAntenna::ImportAntenna (string parID,
				string parFileName,
				double parShowerTheta,
				double parShowerPhi)
    : itsID(parID),
      itsFileName(parFileName),
      itsSamplingTimeScale(0.0),
      hasData(false),
      itsObservationAxis(-1.0*ThreeVector(cos(parShowerPhi*rad)*sin(parShowerTheta*rad),
					  sin(parShowerPhi*rad)*sin(parShowerTheta*rad),
					  -cos(parShowerTheta*rad))),
      itsAzimuthAxis((itsObservationAxis.CrossedWith(ThreeVector(-cos(parShowerPhi*rad),
								 -sin(parShowerPhi*rad),
								 0.0))).GetDirection()),
      itsZenithAxis(itsAzimuthAxis.CrossedWith(itsObservationAxis))
  {
    /*
      cout << "AziDirection: " << ThreeVector(-cos(parShowerPhi*rad),-sin(parShowerPhi*rad),0.0) << endl;
      cout << "itsObservationAxis: " << itsObservationAxis << endl;
      cout << "itsZenithAxis: " << itsZenithAxis << endl;
      cout << "itsAzimuthAxis: " << itsAzimuthAxis << endl;
    */  
    ifstream fin (itsFileName.c_str());
    if (not fin)
      {
	cout << "Error reading file " << itsFileName << "!" << endl;
      }
    else
      {	
	bool firstPoint = true;
	double tmpTimeStamp;
	double firstTimeStamp=0.0;
	ThreeVector tmpDataPoint;
	while (fin >> tmpTimeStamp >> tmpDataPoint)
	  {
	    itsTimeSeries.push_back(tmpDataPoint);
	    if (firstPoint)
	      {
		firstTimeStamp = tmpTimeStamp;
		firstPoint = false;
	      }
	  }
	hasData = true;
	itsSamplingTimeScale = (tmpTimeStamp-firstTimeStamp)/(itsTimeSeries.size()-1);	// calculate sampling time scale
      }
    fin.close();
  }
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  ImportAntenna::~ImportAntenna ()
  {
  }
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  
  Bool ImportAntenna::getTimeSeries(Vector<Double> &Eazimuth,
				    Vector<Double> &Ezenith) const
  {
    const long NumPoints = static_cast<long>(itsTimeSeries.size());				// original number of points in time series
    const long NumPointsPadded = static_cast<long>(pow(2.0,ceil(log(static_cast<double>(NumPoints))/log(2.0))+1.0));	// enlarge to second next power of 2
    const long PointsToPadBefore = (NumPointsPadded-NumPoints)/2;					// pad (rounded down) half of them before
    const long PointsToPadAfter = NumPointsPadded-NumPoints-PointsToPadBefore;			// pad the rest behind
    
    Eazimuth.resize(NumPointsPadded);
    Ezenith.resize(NumPointsPadded);
    double fullEnergy = 0.0;
    double projEnergy = 0.0;
    
    // pad before actual data
    for (long i=0; i<PointsToPadBefore; ++i)
      {
	Eazimuth(i) = 0.0;
	Ezenith(i) = 0.0;
      }
    
    // actual data
    for (long i=0; i<NumPoints; ++i)
      {
	Eazimuth(PointsToPadBefore+i)=itsTimeSeries.at(i).DottedWith(itsAzimuthAxis);
	Ezenith(PointsToPadBefore+i)=itsTimeSeries.at(i).DottedWith(itsZenithAxis);
	fullEnergy+=itsTimeSeries.at(i).GetLength()*itsTimeSeries.at(i).GetLength();
	projEnergy+=Eazimuth(PointsToPadBefore+i)*Eazimuth(PointsToPadBefore+i)+Ezenith(PointsToPadBefore+i)*Ezenith(PointsToPadBefore+i);
      }
    
    // pad after actual data
    for (long i=0; i<PointsToPadAfter; ++i)
      {
	Eazimuth(PointsToPadBefore+NumPoints+i) = 0.0;
	Ezenith(PointsToPadBefore+NumPoints+i) = 0.0;
      }
    
    if (projEnergy/fullEnergy < minimumEfficiency)
      {
	cout << "Warning: energy fraction contained in projected pulse for antenna "
	     << getID()
	     << " is only "
	     << projEnergy/fullEnergy << "!\n";
	cout << "Shower axis direction specified might be inconsistent with simulated data!\n";
	return false;
      }
    else
      {
	return true;
      }
  }
  
}  // Namespace CR -- end
