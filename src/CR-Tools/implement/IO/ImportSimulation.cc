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

/* $Id$*/

#include <IO/ImportSimulation.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  ImportSimulation::ImportSimulation ()
    : itsAzimuthAngle(0.0),
      itsElevationAngle(0.0),
      itsShowerTheta(0.0),
      itsShowerPhi(0.0),
      itsLowerWindowBoundary(1e10),
      itsUpperWindowBoundary(-1e10)
  {
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ImportSimulation::~ImportSimulation ()
  {
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool ImportSimulation::openNewSimulation (String const SimulationName,
					    String const AntListName,
					    String const path)
  {
    bool zenithOk       = false;
    bool azimuthOk      = false;
    bool algorithmOk    = false;
    bool resbaseOk      = false;
    bool antennaReadsOk = true;
    
    // read in and parse reas parameter file
    ifstream fin((path+"/"+SimulationName+".reas").c_str());
    if (not fin)							// read error
      {
	cout << "Error reading file " << path+"/"+SimulationName+".reas!" << endl;
	return false;
      }
    char lineBuf[1024];
    while ( fin.getline(lineBuf,1024) )
      {
	string currLine(lineBuf);
	// commentary line
	if (currLine.substr(0,1) == string("#")) { continue; }
	// search for "=" starting with second character
	unsigned int pos = currLine.find("=",1);
	if ( pos < currLine.size() )
	  {
	    // everything before the "="
	    string type = currLine.substr(0,pos);
	    string value;
	    // search for ";" starting with the position of the "=" found before
	    unsigned int pose = currLine.find(";",pos);
	    if ( pose < currLine.size() )
	      {
		// comment found, only use part before the ";"
		value = currLine.substr(pos+1,pose-pos-1);		// only part before the ";"
	      }
	    else
	      {
		value = currLine.substr(pos+1);				// everything after the "="
	      }
	    unsigned int tend = type.size();
	    stringstream ss;
	    ss << value;      // feed the value into the stringstream for conversion
	    
	    // now check for the individual entries
	    if (type.find("ShowerZenithAngle") < tend)
	      {
		ss >> itsShowerTheta;
		itsElevationAngle=90.0-itsShowerTheta;
		if ((itsElevationAngle >= 0.0) && (itsElevationAngle <= 90.0))
		  { zenithOk=true; }
		continue;
	      }
	    
	    if (type.find("ShowerAzimuthAngle") < tend)
	      {
		ss >> itsShowerPhi;
		while (itsShowerPhi < 0.0) { itsShowerPhi+=360.0;}		// normalize to interval [0,360]
		while (itsShowerPhi >= 360.0) { itsShowerPhi-=360.0;}		// normalize to interval [0,360]
		itsAzimuthAngle = (itsShowerPhi+180.0);				// change from "propagates to" to "comes from"
		itsAzimuthAngle = -itsAzimuthAngle;				// flip east and west
		while (itsAzimuthAngle < 0.0) { itsAzimuthAngle+=360.0; }	// renormalize to interval [0,360]
		azimuthOk=true;
		continue;
	      }
	    	    
	    if (type.find("InnerTimeAlgorithm") < tend)
	      {
		int ii;
		ss >> ii;
		if ((ii == 2) || (ii == 3))					// make sure we have equidistantly sampled data
		  { algorithmOk = true; }	
		continue;
	      }

	    if (type.find("ResolutionReductionScale") < tend)
	      {
		Double dd;
		ss >> dd;
		if (dd == 0.0)							// make sure all antennas have same sampling timebase
		  { resbaseOk = true; }	
		continue;
	      }

	  }
      }
    fin.close();
    
    // now open and read in bins-file
    ifstream bin( (path+"/"+SimulationName+"_"+AntListName+".bins").c_str() );
    if (not bin)
      {
	cout << "Error reading file " << path+"/"+SimulationName+"_"+AntListName+".bins" << endl;
	return false;
      }
    String tmpFileName;
    ThreeVector dummyVector;
    Double dummydouble;

    // only interested in the file name here
    while (bin >> tmpFileName >> dummyVector >> dummydouble)
      {
	unsigned int pos1 = tmpFileName.find("raw_",0)+4;
	unsigned int pos2 = tmpFileName.find(".dat",pos1);
	string id = tmpFileName.substr(pos1,pos2-pos1);
	ImportAntenna tmpAntenna(id,
				 (path+"/"+tmpFileName).c_str(),
				 itsShowerTheta,
				 itsShowerPhi);
	if (tmpAntenna.validData())
	  {
	    itsAntennaList.push_back(tmpAntenna);
	  }
	else
	  {
	    // error reading one of the antenna files
	    antennaReadsOk=false;
	  }
      }
    bin.close();

    // set to first antenna in list
    iNextAntennaToReturn=itsAntennaList.begin();
    
    if (not zenithOk) {
      cout << "Error: illegal zenith angle value!\n";
    }  
    if (not azimuthOk) {
      cout << "Error: illegal azimuth angle value!\n";
    }  
    if (not algorithmOk) {
      cout << "Error: illegal (non-equidistantly sampling) timing algorithm!\n";
    }  
    if (not antennaReadsOk) {
      cout << "Error: one or more antenna.dat files could not be read!\n";
    }  
    if (not resbaseOk) {
      cout << "Error: automatic resolution reduction cannot be used for reas2event!\n";
    }  

    // find common time window between all of the antennas
    Double lowestWindowBoundary = itsLowerWindowBoundary;	// start value
    Double highestWindowBoundary = itsUpperWindowBoundary;	// start value
    for (vector<ImportAntenna>::const_iterator i=itsAntennaList.begin(); i!=itsAntennaList.end(); ++i)
      {
        if (i->getSimulatedWindowStart() < lowestWindowBoundary) { lowestWindowBoundary = i->getSimulatedWindowStart(); }
	if (i->getSimulatedWindowEnd() > highestWindowBoundary) { highestWindowBoundary = i->getSimulatedWindowEnd(); }
      }
    for (vector<ImportAntenna>::iterator i=itsAntennaList.begin(); i!=itsAntennaList.end(); ++i)
      {
        i->setRequiredWindowBoundaries(lowestWindowBoundary,highestWindowBoundary);
      }
    
    return (zenithOk && azimuthOk && algorithmOk && resbaseOk && antennaReadsOk);
  }
  
  
  Bool ImportSimulation::getNextAntenna(const ImportAntenna*& newAntenna)
  {
    if (iNextAntennaToReturn == itsAntennaList.end())
      {
	newAntenna = 0;
	// no more antennas left
	return false;
      }
    else
      {
	// dereference iterator and return pointer
	newAntenna = &(*iNextAntennaToReturn);
	// iterate to next antenna
	++iNextAntennaToReturn;
	return true;
      }
  }
  
}  //  Namespace CR -- end
