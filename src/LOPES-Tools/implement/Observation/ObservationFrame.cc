/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: ObservationFrame.cc,v 1.1 2005/06/29 14:34:05 bahren Exp $ */

#include <Observation/ObservationFrame.h>

using std::cout;
using std::cerr;
using std::endl;

namespace LOPES {  // namespace LOPES -- begin
  
  // ============================================================================
  //
  // Construction / Destruction
  //
  // ============================================================================

  // ----------------------------------------------------------- ObservationFrame
  
  ObservationFrame::ObservationFrame()
  {
    Quantity epoch;
    String obsName;
    Time currentTime;
    
    epoch = Quantity(currentTime.modifiedJulianDay(),"d");
    obsName = "WSRT";
    
    ObservationFrame::setObservationFrame(epoch,obsName);
  }
  
  // ----------------------------------------------------------- ObservationFrame
  
  ObservationFrame::ObservationFrame(Quantity epoch,
				     String obsName)
  {
    MPosition obsPosition;
    
    /* check if the provided observatory name if known to the system and position
       information is available from the data repository */
    MeasTable::Observatory(obsPosition,obsName);
    
    ObservationFrame::setObservationFrame(epoch,obsName,obsPosition);
  }
  
  // ----------------------------------------------------------- ObservationFrame
  
  ObservationFrame::ObservationFrame (Quantity epoch,
				      MPosition obsPosition)
  {
    String obsName = "undefined";
    
    ObservationFrame::setObservationFrame(epoch,
					  obsName,
					  obsPosition);
  }
  
  // ----------------------------------------------------------- ObservationFrame
  
  ObservationFrame::ObservationFrame (Quantity epoch,
				      String obsName,
				      MPosition obsPosition)
  {
    // simply pass the input arguments to ObservationFrame::setObservationFrame
    ObservationFrame::setObservationFrame(epoch,
					  obsName,
					  obsPosition);
  }
  
  // ============================================================================
  //
  //  I/O of the observation frame member data.
  //
  // ============================================================================
  
  // -------------------------------------------------------- setObservationFrame

  void ObservationFrame::setObservationFrame (Quantity epoch,
					      String obsName)
  {
    // pass the input arguments to the functions setting up the individual values
    ObservationFrame::setEpoch (epoch);
    ObservationFrame::setObservatory (obsName);
  }
  
  // -------------------------------------------------------- setObservationFrame

  void ObservationFrame::setObservationFrame (Quantity epoch,
					      String obsName,
					      Matrix<double> antennaPositions)
  {
    ObservationFrame::setObservationFrame (epoch,obsName);
    ObservationFrame::setAntennaPositions(antennaPositions);
  }

  // -------------------------------------------------------- setObservationFrame

  void ObservationFrame::setObservationFrame (Quantity epoch,
					      String obsName,
					      MPosition obsPosition)
  {
    // pass the input arguments to the functions setting up the individual values
    ObservationFrame::setEpoch(epoch);
    ObservationFrame::setObservatory (obsName,obsPosition);
  }
  
  // -------------------------------------------------------- setObservationFrame

  void ObservationFrame::setObservationFrame (GlishRecord& record)
  {
    GlishArray gtmp;
    String s;
    
    // Variables of the Observation frame
    if (record.exists("Epoch")) {
      GlishRecord rec = record.get("Epoch");
      double epochValue;
      String epochUnit;
      gtmp = rec.get("value");
      gtmp.get(epochValue);
      gtmp = rec.get("unit");
      gtmp.get(epochUnit);
      Quantity epoch = Quantity (epochValue,epochUnit);
      ObservationFrame::setEpoch (epoch);
    }
    if (record.exists("ObservatoryName")) {
      gtmp = record.get("ObservatoryName");
      gtmp.get(s);
      ObservationFrame::setObservatory (s);
    }
    if (record.exists("ObservatoryPosition")) {
      cerr << "[ObservationFrame::setObservationFrame] Warning!" << endl;
      cerr << "Assignment of ObservatoryPosition from record not yet supported" << endl;
    }
    if (record.exists("AntennaPositions")) {
      Matrix<double> AntennaPositions;
      gtmp = record.get("AntennaPositions");
      gtmp.get(AntennaPositions);
      ObservationFrame::setAntennaPositions (AntennaPositions);
    }
    
  }
  
  // -------------------------------------------------------- getObservationFrame

  void ObservationFrame::getObservationFrame (Quantity& epoch,
					      MPosition& obsPosition)
  {
    ObservationFrame::epoch (epoch);
    ObservationFrame::getObservatory (obsPosition);
  }
  
  // -------------------------------------------------------- getObservationFrame

  MeasFrame ObservationFrame::getObservationFrame ()
  {
    // get the observation epoch as measure
    MEpoch obsTime = MEpoch (epoch_p);
    // construct a reference frame ...
    MeasFrame frame (obsTime,observatoryPosition_p);
    // ... and pass is back
    return frame;
  }
  
  // ============================================================================
  //
  //  Epoch of the observation
  //
  // ============================================================================

  // ------------------------------------------------------------------- setEpoch
  
  void ObservationFrame::setEpoch (const Quantity epoch) {
    epoch_p = epoch;
  }
  
  // ------------------------------------------------------------------- setEpoch
  
  void ObservationFrame::setEpoch (const double value,
				   const String unit)
  {
    epoch_p = Quantity (value,unit);
  }
  
  // ---------------------------------------------------------------------- epoch
  
  void ObservationFrame::epoch (Quantity& epoch) {
    epoch = epoch_p;
  }
  
  // ---------------------------------------------------------------------- epoch

  void ObservationFrame::epoch (MEpoch& epoch) {
    epoch = MEpoch(epoch_p);
  }
  
  // =============================================================================
  //
  //  Name and Position of the Observatory
  //
  // =============================================================================
  
  void ObservationFrame::setObservatory (const String observatoryName) {
    observatoryName_p = observatoryName;
    ObservationFrame::setObservatoryPosition ();
  }
  
  void ObservationFrame::setObservatory (MPosition obsPosition) {
    observatoryPosition_p = obsPosition;
  }
  
  void ObservationFrame::setObservatory(String obsName,
				      MPosition obsPosition)
  {
    ObservationFrame::setObservatory (obsName);
    ObservationFrame::setObservatory (obsPosition);
  }
  
  void ObservationFrame::setObservatoryPosition () 
  {
    String obsName;
    MPosition obsPosition;
    bool obsStatus;
    
    obsName = observatoryName_p;
    MeasTable::initObservatories ();
    obsStatus = MeasTable::Observatory(obsPosition,obsName);
    
    if (obsStatus) {
      observatoryPosition_p = obsPosition;
    } else {
      cerr << "ObservationFrame::setObservatoryPosition" << endl;
      cerr << "\tWarning, unable to find position information for " << obsName;
      cerr << " in the data respository." << endl;
      // get the list of available observatories
      Vector<String> observatories = MeasTable::Observatories();
      cout << observatories << endl;
    }
  }
  
  String ObservationFrame::getObservatoryName () {
    return observatoryName_p;
  }
  
  void ObservationFrame::getObservatory (MPosition& obsPosition) {
    obsPosition = observatoryPosition_p;
  }
  
  void ObservationFrame::getObservatory (String& obsName,
					 MPosition& obsPosition) 
  {
    obsName = observatoryName_p;
    obsPosition = observatoryPosition_p;
  }
  
  // =============================================================================
  //
  //  Array antennae
  //
  // =============================================================================
  
  void ObservationFrame::setAntennaPositions (const Matrix<double> antennaPositions)
  {
    antennaPositions_p.resize(antennaPositions.shape());
    antennaPositions_p = antennaPositions;
  }
  
  Matrix<double> ObservationFrame::antennaPositions ()
  {
    return antennaPositions_p;
  }
  
  Cube<double> ObservationFrame::baselines ()
  {
    int i (0);
    int j (0);
    int k (0);
    int nant (0);
    int ncoords (0);
    Cube<double> antennaBaselines;
    
    antennaPositions_p.shape(nant,ncoords);
    antennaBaselines.resize (nant,nant,ncoords);
    
    for (i=0; i<nant; ++i) {
      for (j=0; j<nant; ++j) {
	for (k=0; k<ncoords; ++k) {
	  antennaBaselines(i,j,k) = antennaPositions_p(j,k)-antennaPositions_p(i,k);
	}
      }
    }
    
    return antennaBaselines;
    
  }
  
  int ObservationFrame::nofAntennae () {
    return antennaPositions_p.nrow();
  }
  
  int ObservationFrame::nofAntennae (bool selected)
  {
    int sum (0);
    int nofAntennae (antennaPositions_p.nrow());
    
    for (int ant=0; ant<nofAntennae; ++ant) {
      if (antennaSelection_p(ant)) {
	++sum;
      }
    }
    if (selected) {
      return sum;
    }
    else {
      return nofAntennae-sum;
    }
  }
  
  // =============================================================================
  //
  //  Debugging and information
  //
  // =============================================================================
  
  void ObservationFrame::show (std::ostream& s = std::cout)
  {
    IPosition shape = antennaPositions_p.shape();
    
    s << "\n[ObservationFrame]\n" << endl;
    s << " - Epoch of the observation    : " << epoch_p << endl;
    s << " - Name of the Observatory     : " << observatoryName_p << endl;
    s << " - Location of the Observatory : " << observatoryPosition_p << endl;
    s << " - 3D antenna positions ...... : " << shape << endl;
    for (int ant=0; ant<shape(0); ant++) {
      s << "\t" << ant << " : " <<  antennaPositions_p.row(ant) << endl;
    }
  }
  
}  // namespace LOPES -- end
