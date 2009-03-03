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

#include <Observation/ObservationFrame.h>

using std::cout;
using std::cerr;
using std::endl;

// =============================================================================
//
// Construction / Destruction
//
// =============================================================================

//______________________________________________________________________________
//                                                              ObservationFrame

ObservationFrame::ObservationFrame()
{
  Quantity epoch;
  String obsName;
  Time currentTime;
  
  epoch = Quantity(currentTime.modifiedJulianDay(),"d");
  obsName = "WSRT";
  
  ObservationFrame::setObservationFrame(epoch,obsName);
}

//______________________________________________________________________________
//                                                              ObservationFrame

ObservationFrame::ObservationFrame(Quantity epoch,
				   String obsName)
{
  MPosition obsPosition;

  /* check if the provided observatory name if known to the system and position
     information is available from the data repository */
  MeasTable::Observatory(obsPosition,obsName);

  ObservationFrame::setObservationFrame(epoch,obsName,obsPosition);
}

//______________________________________________________________________________
//                                                              ObservationFrame

ObservationFrame::ObservationFrame (Quantity epoch,
				    MPosition obsPosition)
{
  String obsName = "undefined";

  ObservationFrame::setObservationFrame(epoch,obsName,obsPosition);
}

//______________________________________________________________________________
//                                                              ObservationFrame

ObservationFrame::ObservationFrame(Quantity epoch,
				   String obsName,
				   MPosition obsPosition)
{
  // simply pass the input arguments to ObservationFrame::setObservationFrame
  ObservationFrame::setObservationFrame(epoch,obsName,obsPosition);
}

// =============================================================================
//
//  I/O of the observation frame member data.
//
// =============================================================================

//______________________________________________________________________________
//                                                           setObservationFrame

void ObservationFrame::setObservationFrame (Quantity epoch,
					    String obsName)
{
  // pass the input arguments to the functions setting up the individual values
  ObservationFrame::setEpoch (epoch);
  ObservationFrame::setObservatory (obsName);
}

//______________________________________________________________________________
//                                                           setObservationFrame

void ObservationFrame::setObservationFrame (Quantity epoch,
					    String obsName,
					    Matrix<double> antennaPositions)
{
  ObservationFrame::setObservationFrame (epoch,obsName);
  ObservationFrame::setAntennaPositions(antennaPositions);
}


//______________________________________________________________________________
//                                                           setObservationFrame

void ObservationFrame::setObservationFrame (Quantity epoch,
					    String obsName,
					    MPosition obsPosition)
{
  // pass the input arguments to the functions setting up the individual values
  ObservationFrame::setEpoch(epoch);
  ObservationFrame::setObservatory (obsName,obsPosition);
}

//______________________________________________________________________________
//                                                           getObservationFrame

MeasFrame ObservationFrame::getObservationFrame ()
{
  // get the observation epoch as measure
  MEpoch obsTime = MEpoch (epoch_p);
  // construct a reference frame ...
  MeasFrame frame (obsTime,observatoryPosition_p);
  // ... and pass is back
  return frame;
}

//______________________________________________________________________________
//                                                        setObservatoryPosition

void ObservationFrame::setObservatoryPosition () 
{
  String obsName;
  MPosition obsPosition;
  bool obsStatus;
  
  obsName = name_p;
  MeasTable::initObservatories ();
  obsStatus = MeasTable::Observatory(obsPosition,obsName);
  
  if (obsStatus) {
//     cout << "Found entry for " << obsName << " in database." << endl;
//     cout << obsPosition << endl;
//     cout << obsPosition.getRef() << endl;
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

//______________________________________________________________________________
//                                                                     baselines

Cube<double> ObservationFrame::baselines ()
{
  int nant,ncoords;
  Cube<double> antennaBaselines;
  
  antennaPositions_p.shape(nant,ncoords);
  antennaBaselines.resize (nant,nant,ncoords);

  for (int i=0; i<nant; ++i) {
    for (int j=0; j<nant; ++j) {
      for (int k=0; k<ncoords; ++k) {
	antennaBaselines(i,j,k) = antennaPositions_p(j,k)-antennaPositions_p(i,k);
      }
    }
  }

  return antennaBaselines;

}

//______________________________________________________________________________
//                                                                   nofAntennae

int ObservationFrame::nofAntennae (bool const &selected)
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

void ObservationFrame::summary (std::ostream& s = std::cout)
{
  IPosition shape = antennaPositions_p.shape();

  s << "\n[ObservationFrame]\n" << endl;
  s << " - Epoch of the observation    : " << epoch_p << endl;
  s << " - Name of the Observatory     : " << name_p << endl;
  s << " - Location of the Observatory : " << observatoryPosition_p << endl;
  s << " - 3D antenna positions ...... : " << shape << endl;
  for (int ant=0; ant<shape(0); ant++) {
    s << "\t" << ant << " : " <<  antennaPositions_p.row(ant) << endl;
  }
}
