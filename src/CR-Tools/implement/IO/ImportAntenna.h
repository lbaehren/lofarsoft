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

/* $Id: importAntenna.h,v 1.5 2007/03/12 08:41:07 huege Exp $*/

#ifndef IMPORTANTENNA_H
#define IMPORTANTENNA_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <casa/string.h>
#include <casa/Arrays.h>

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

#include <Math/ThreeVector.h>

using std::cout;
using std::string;
using std::ifstream;
using std::vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class ImportAntenna
    
    \ingroup IO
    
    \brief Brief description for class ImportAntenna
    
    \author Tim Huege
    
    \date 2006/10/16
    
    \test tImportAntenna.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>none
    </ul>
    
    <h3>Synopsis</h3>
    
    Reads in and provides access to an individual observer (antenna) simulated
    with REAS.
    
    <h3>Example(s)</h3>
    
    \verbatim
    ImportAntenna myAntenna("10101", "/here/raw_10101.dat", 30.0, 180.0)
    \endverbatim
    
    Opens the specified antenna file and interprets it as the result of a
    simulation with 30 degree zenith and 180 degree azimuth angles (in REAS
    convention!).
    
  */
  
  class ImportAntenna {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      \param parID -- antenna ID
      \param parFileName -- file name of the simulation file to be read in
      \param parShowerTheta -- shower zenith angle - in REAS convention!
      \param parAzimuthAngle -- shower azimuth angle - in REAS convention!
      
      Angles are in REAS convention! Zenith angle is angle to the zenith direction.
      Azimuth angle is specified as the direction into which the shower propagates.
      0 degree is to north, 90 degree is to west.
    */
    ImportAntenna (string parID,
		   string parFileName,
		   double parShowerTheta,
		   double parShowerPhi);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~ImportAntenna ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    
    
    // ------------------------------------------------------------------ Methods
    
  /*!
    \brief Get the two zero-padded, equidistantly sampled time series arrays

    Get the two zero-padded, equidistantly sampled time series arrays containing
    the efield data in cgs units.
    
    \param Eazimuth -- returns time series projected on "azimuth aligned"
                       component (lies in x-y-plane)
    \param Ezenith  -- returns time series projected on "zenith aligned" component
                       (lies in plane defined by shower axis and z-axis)
    
    \result Returns true if energy fraction in projected pulses is essentially 1,
    false otherwise.
  */
    Bool getTimeSeries(Vector<Double> &Eazimuth,
		       Vector<Double> &Ezenith) const;
    
    /*!
      \brief returns if ImportAntenna object has valid data
    */
    inline Bool validData() const { return hasData; }
    
    /*!
      \brief returns the time scale of the equidistantly sampled data (in seconds)
    */
    inline Double getSamplingTimeScale() const { return itsSamplingTimeScale; }
    
    /*!
      \brief returns antenna ID string
    */
    inline String getID() const { return itsID; }
    
  private:
    
    string itsID;
    string itsFileName;
    double itsSamplingTimeScale;
    bool hasData;
    vector<ThreeVector> itsTimeSeries;
    /*!
      vectorial direction into which observer looks - in REAS coordinates!
    */
    ThreeVector itsObservationAxis;
    /*!
      vectorial direction of "horizontal-aligned" component of e-field - in REAS
      coordinates!
    */
    ThreeVector itsAzimuthAxis;
    /*!
      vectorial direction of "vertical-aligned" component of e-field - in REAS
      coordinates!	
    */
    ThreeVector itsZenithAxis;
    
    static const double rad;
    static const double minimumEfficiency;

  };
  
}  // Namespace CR -- end

#endif /* IMPORTANTENNA_H */

