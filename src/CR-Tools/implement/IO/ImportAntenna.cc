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
  const Double ImportAntenna::rad = M_PI/180.0;
  // warn if energy fraction in projected pulses falls below this limit
  const Double ImportAntenna::minimumEfficiency = 0.995;
  // conversion factor from cgs values to Volt per Meter
  const Double ImportAntenna::cgsToVpm = 2.99792458e4;  

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ImportAntenna::ImportAntenna (string parID,
				string parFileName,
				Double parShowerTheta,
				Double parShowerPhi)
    : itsID(parID),
      itsFileName(parFileName),
      itsSamplingTimeScale(0.0),
      itsSimulatedWindowStart(0.0),
      itsSimulatedWindowEnd(0.0),
      itsRequiredWindowStart(0.0),
      itsRequiredWindowEnd(0.0),
      itsPaddedWindowStart(0.0),
      itsPaddedWindowEnd(0.0),
      itsRequiredWindowValid(false),
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
	Double tmpTimeStamp;
	ThreeVector tmpDataPoint;
	while (fin >> tmpTimeStamp >> tmpDataPoint)
	  {
	    itsTimeSeries.push_back(tmpDataPoint);
	    if (firstPoint)
	      {
		itsSimulatedWindowStart = tmpTimeStamp;
		firstPoint = false;
	      }
	    itsSimulatedWindowEnd = tmpTimeStamp;
	  }
	hasData = true;
	itsSamplingTimeScale = (itsSimulatedWindowEnd-itsSimulatedWindowStart)/(itsTimeSeries.size()-1);	// calculate sampling time scale
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
    if (not itsRequiredWindowValid)
      {
       	cout << "Error: required time window was not initialized correctly for antenna "
	     << getID()
	     << "\n";
	return false;
      }
    const long NumPointsSimulated = static_cast<long>(itsTimeSeries.size()); // original number of points in time series
    const long NumPointsBeforeSimulated = static_cast<long>((itsSimulatedWindowStart-itsRequiredWindowStart)/itsSamplingTimeScale+0.5);
    const long NumPointsAfterSimulated = static_cast<long>((itsRequiredWindowEnd-itsSimulatedWindowEnd)/itsSamplingTimeScale+0.5);
    const long NumPointsRequired = NumPointsBeforeSimulated+NumPointsSimulated+NumPointsAfterSimulated;
    const long NumPointsPadded = static_cast<long>(pow(2.0,ceil(log(static_cast<Double>(NumPointsRequired))/log(2.0))+1.0));	// enlarge to second next power of 2
    const long PointsToPadBefore = (NumPointsPadded-NumPointsRequired)/2;				// pad (rounded down) half of them before
    const long PointsToPadAfter = NumPointsPadded-NumPointsRequired-PointsToPadBefore;			// pad the rest behind
    Eazimuth.resize(NumPointsPadded);
    Ezenith.resize(NumPointsPadded);
    Double fullEnergy = 0.0;
    Double projEnergy = 0.0;
    
    // pad before actual data
    for (long i=0; i<PointsToPadBefore+NumPointsBeforeSimulated; ++i)
      {
	Eazimuth(i) = 0.0;
	Ezenith(i) = 0.0;
      }
    
    // actual data
    for (long i=0; i<NumPointsSimulated; ++i)
      {
	Eazimuth(PointsToPadBefore+NumPointsBeforeSimulated+i)=cgsToVpm*itsTimeSeries.at(i).DottedWith(itsAzimuthAxis);
	Ezenith(PointsToPadBefore+NumPointsBeforeSimulated+i)=cgsToVpm*itsTimeSeries.at(i).DottedWith(itsZenithAxis);
	fullEnergy+=cgsToVpm*cgsToVpm*itsTimeSeries.at(i).GetLength()*itsTimeSeries.at(i).GetLength();
	projEnergy+=Eazimuth(PointsToPadBefore+NumPointsBeforeSimulated+i)*Eazimuth(PointsToPadBefore+NumPointsBeforeSimulated+i)+Ezenith(PointsToPadBefore+NumPointsBeforeSimulated+i)*Ezenith(PointsToPadBefore+NumPointsBeforeSimulated+i);
      }
    
    // pad after actual data
    for (long i=0; i<NumPointsAfterSimulated+PointsToPadAfter; ++i)
      {
	Eazimuth(PointsToPadBefore+NumPointsBeforeSimulated+NumPointsSimulated+i) = 0.0;
	Ezenith(PointsToPadBefore+NumPointsBeforeSimulated+NumPointsSimulated+i) = 0.0;
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
  
  bool ImportAntenna::setRequiredWindowBoundaries(Double windowStart, Double windowEnd)
  {
    if (windowStart > itsSimulatedWindowStart)		{ return false; } // window starts too late
    if (windowEnd < itsSimulatedWindowEnd)		{ return false; } // window ends too early
    if ( (fmod(windowStart,itsSamplingTimeScale)/itsSamplingTimeScale>1.e-5)
       && fabs(1.-(fmod(windowStart,itsSamplingTimeScale)/itsSamplingTimeScale))>1.e-5)
       { return false; } // not a multiple of sampling timescale
    if ( (fmod(windowEnd,itsSamplingTimeScale)/itsSamplingTimeScale>1.e-5)
       && fabs(1.-(fmod(windowEnd,itsSamplingTimeScale)/itsSamplingTimeScale))>1.e-5)
       { return false; } // not a multiple of sampling timescale
    // values are valid, now calculate and set boundaries of padded time window
    itsRequiredWindowStart = windowStart;
    itsRequiredWindowEnd = windowEnd;
    const long NumPointsSimulated = static_cast<long>(itsTimeSeries.size()); // original number of points in time series
    const long NumPointsBeforeSimulated = static_cast<long>((itsSimulatedWindowStart-itsRequiredWindowStart)/itsSamplingTimeScale+0.5);
    const long NumPointsAfterSimulated = static_cast<long>((itsRequiredWindowEnd-itsSimulatedWindowEnd)/itsSamplingTimeScale+0.5);
    const long NumPointsRequired = NumPointsBeforeSimulated+NumPointsSimulated+NumPointsAfterSimulated;
    const long NumPointsPadded = static_cast<long>(pow(2.0,ceil(log(static_cast<Double>(NumPointsRequired))/log(2.0))+1.0));	// enlarge to second next power of 2
    const long PointsToPadBefore = (NumPointsPadded-NumPointsRequired)/2;				// pad (rounded down) half of them before
    itsPaddedWindowStart = itsSimulatedWindowStart-(NumPointsBeforeSimulated+PointsToPadBefore)*itsSamplingTimeScale;	// calculate beginning of padded window
    itsPaddedWindowEnd = itsPaddedWindowStart+(NumPointsPadded-1)*itsSamplingTimeScale;		// calculate end of padded window
    itsRequiredWindowValid = true;
    return true;
  }

}  // Namespace CR -- end

