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

/* $Id: importSimulation.h,v 1.3 2006/10/16 13:07:45 huege Exp $*/

#ifndef IMPORTSIMULATION_H
#define IMPORTSIMULATION_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>
#include <casa/string.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <IO/ImportAntenna.h>

using std::cout;
using std::endl;
using std::ostream;
using std::ifstream;
using std::stringstream;
using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class ImportSimulation
    
    \ingroup IO
    
    \brief Brief description for class ImportSimulation
    
    \author Tim Huege
    
    \date 2006/10/16
    
    \test tImportSimulation.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>none
    </ul>
    
    <h3>Synopsis</h3>
    
    Reads in a REAS simulation and provides access to the observers simulated
    therein through ImportAntenna objects.
    
    <h3>Example(s)</h3>
    
    \code
    importSimulation mySimulation;
    mySimulation.openNewSimulation("vertical","lopes30","/here");
    \endcode
    
    This opens the simulation generated from "vertical.reas" and "lopes30.list"
    situated in the directory "/here".
    
  */
  
  class ImportSimulation {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    ImportSimulation ();
    
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~ImportSimulation ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    
    
    // ------------------------------------------------------------------ Methods
    
    
    /*!
      \brief Open a new simulation
      
      \param SimulationName -- name of the REAS parameter file (without extension
                               <tt>.reas</tt>)
      \param AntListName -- name of the antenna list file (without extension
                            <tt>.list</tt>)
      \param path -- directory in which the data lie (without <tt>/</tt> at end),
                     set to "." as default
      
      \result returns true if read was successful
    */
    Bool openNewSimulation (String const SimulationName,
			    String const AntListName,
			    String const path=".");
    
    /*!
      \brief Get the ImportAntenna object for the next antenna
      
      \param ImportAntenna* -- gives back pointer to the next antenna object
      
      \result returns true if there was another antenna, false if there were no more
    */
    Bool getNextAntenna (const ImportAntenna*& newAntenna);
    
    /*!
      \brief Get the shower direction of the simulated event.
      
      \param azimuth -- azimuth angle in LOPES convention
      \param elevation -- elevation angle in LOPES convention
      
      Azimuth and elevation are from the view point of the antenna, in degrees. 
      Azimuth: North: 0deg East: 90deg; Elevation: 0deg horizon, 90deg zenith
      E.g. azimuth 0deg and elevation 45deg is a shower that comes from the north.
    */
    void getDirection(Double &azimuth,
		      Double &elevation) const {
      azimuth = itsAzimuthAngle; elevation = itsElevationAngle;
    }
    

    /*!
      \brief returns the sampling time scale of the equidistantly sampled data (in seconds)
    */
    inline Double getSamplingTimeScale() const { return itsAntennaList.front().getSamplingTimeScale(); } // take value from first antenna, is constant for all of them

    /*!
      \brief returns the time stamp of the first element of the sampled data (in seconds)
    */
    inline Double getSamplingWindowStart() const { return itsAntennaList.front().getWindowStart(); } 	// take value from first antenna, is constant for all of them

    /*!
      \brief set the minimal start and stop boundaries for the window (in seconds)

      \param LowerBoundary - maximal value for the lower window boundary, the actual boundary might be lower (in seconds)
      \param UpperBoundary - minimal value for the upper window boundary, the actual boundary might be larger (in seconds)

      The default values are dummy values. Use <tt>setWindowBoundaries()</tt> to reset a previously defined window.
    */
    void setWindowBoundaries(Double LowerBoundary=1e10, Double UpperBoundary=-1e10) {
      itsLowerWindowBoundary = LowerBoundary; itsUpperWindowBoundary = UpperBoundary;
    }

  private:
    
    Double itsAzimuthAngle;
    Double itsElevationAngle;
    Double itsShowerTheta;
    Double itsShowerPhi;
    Double itsLowerWindowBoundary;
    Double itsUpperWindowBoundary;
    vector<ImportAntenna> itsAntennaList;
    vector<ImportAntenna>::const_iterator iNextAntennaToReturn;
  };
  
}  // Namespace CR -- end

#endif /* IMPORTSIMULATION_H */
  
