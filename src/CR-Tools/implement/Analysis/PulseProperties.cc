/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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

// Don't forget to run the following commands in implement/Analysis, if something has changed:
/*
 rm RootDict.*
 rootcint RootDict.C -c PulseProperties.h
*/

#include <Analysis/PulseProperties.h>
#include <cstring>

// namespace declaration causes problems with root dictionary
// should be solved in ROOT 5.20
//namespace CR { // Namespace CR -- begin

  // Make class available in ROOT dictionary
  ClassImp(PulseProperties)

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
 PulseProperties::PulseProperties ():
    height(0.),
    heightError(0.),
    time(0.),
    timeError(0.),
    antennaID(0),
    antenna(0),
    maximum(0.),
    envelopeMaximum(0.),
    minimum(0.),
    maximumTime(0.),
    envelopeTime(0.),
    minimumTime(0.),
    halfheightTime(0.),
    geomDelay(0.),
    fwhm(0.),
    distX(-1.),
    distXerr(0.),
    distY(-1.),
    distYerr(0.),
    distZ(-1.),
    distZerr(0.),
    dist(-1.),
    disterr(0.),
    angleToCore(-99.),
    angleToCoreerr(-99.),
    noise(0.),
    lateralExpHeight(0.),
    lateralExpHeightErr(0.),
    lateralExpDeviation(0.),
    lateralExpDeviationErr(0.),
    lateralCut(false),
    polarization(""),
    minMaxSign(0),
    envSign(0)
  {}

  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================

  PulseProperties::~PulseProperties ()
  {}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  void PulseProperties::summary (std::ostream &os)
  {
    os << "[PulseProperties] Summary of internal parameters." << std::endl;
  }


  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // converts stored pulse information into struct
  struct pulseStruct PulseProperties::convert2struct () {
    struct pulseStruct pulse;
    
    pulse.height=height;
    pulse.heightError=heightError;
    pulse.time=time;
    pulse.timeError=timeError;
    pulse.antennaID=antennaID;
    pulse.antenna=antenna;
    pulse.maximum=maximum;
    pulse.envelopeMaximum=envelopeMaximum;
    pulse.minimum=minimum;
    pulse.maximumTime=maximumTime;
    pulse.envelopeTime=envelopeTime;
    pulse.minimumTime=minimumTime;
    pulse.halfheightTime=halfheightTime;
    pulse.geomDelay=geomDelay;
    pulse.fwhm=fwhm;
    pulse.distX=distX;
    pulse.distXerr=distXerr;
    pulse.distY=distY;
    pulse.distYerr=distYerr;
    pulse.distZ=distZ;
    pulse.distZerr=distZerr;
    pulse.dist=dist;
    pulse.disterr=disterr;
    pulse.angleToCore=angleToCore;
    pulse.angleToCoreerr=angleToCoreerr;
    pulse.noise=noise;
    pulse.lateralExpHeight=lateralExpHeight;
    pulse.lateralExpHeightErr=lateralExpHeightErr;
    pulse.lateralExpDeviation=lateralExpDeviation;
    pulse.lateralExpDeviationErr=lateralExpDeviationErr;
    pulse.lateralCut=lateralCut;
    // check size of sting
    if (polarization.size()<31)
      strcpy (pulse.polarization, polarization.c_str());
    else  
      strcpy (pulse.polarization, "ConversionError");    
    pulse.minMaxSign=minMaxSign;
    pulse.envSign=envSign;   
    
    return pulse;
  }

//} // Namespace CR -- end
