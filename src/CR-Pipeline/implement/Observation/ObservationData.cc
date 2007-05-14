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

/* $Id: ObservationData.cc,v 1.9 2007/05/02 09:37:10 bahren Exp $ */

#include <Observation/ObservationData.h>
#include <Skymap/SkymapperTools.h>

namespace CR {  // -- Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData ()
  {
    ObsInfo obsInfo;
    Time startTime;
    Quantity epoch (startTime.modifiedJulianDay(), "d");
    String telescope ("WSRT");
    //
    obsInfo.setObsDate (epoch);
    obsInfo.setTelescope (telescope);
    //
    ObservationData::init (obsInfo);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const String observatory)
  {
    ObsInfo obsInfo;
    Time startTime;
    Quantity epoch (startTime.modifiedJulianDay(), "d");
    //
    obsInfo.setTelescope (observatory);
    obsInfo.setObsDate (epoch);
    //
    ObservationData::init (obsInfo);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const Quantity epoch,
				    const String observatory)
  {
    ObsInfo obsInfo;
    MEpoch obsDate (epoch);
    //
    obsInfo.setTelescope (observatory);
    obsInfo.setObsDate (obsDate);
    //
    ObservationData::init (obsInfo);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const MEpoch epoch,
				    const String observatory)
  {
    ObsInfo obsInfo;
    //
    obsInfo.setTelescope (observatory);
    obsInfo.setObsDate (epoch);
    //
    ObservationData::init (obsInfo);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const MEpoch obsDate,
				    String const &obsName,
				    Matrix<Double> const &antennaPositions,
				    String const &observer)
  {
    init (obsDate,
	  obsName,
	  antennaPositions,
	observer);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const MEpoch obsDate,
				    String const &obsName,
				    const MPosition obsPosition,
				    Matrix<Double> const &antennaPositions,
				    String const &observer)
  {
    init (obsDate,
	  obsName,
	  obsPosition,
	  antennaPositions,
	  observer);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (const ObsInfo obsInfo)
  {
    ObservationData::init (obsInfo);
  }
  
  // ------------------------------------------------------------ ObservationData
  
  ObservationData::ObservationData (ObservationData const& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  ObservationData::~ObservationData ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  // ------------------------------------------------------------------ operator=
  
  ObservationData &ObservationData::operator= (ObservationData const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy
  
  void ObservationData::copy (ObservationData const &other)
  {
    description_p = other.description_p;
    obsInfo_p     = other.obsInfo_p;
    obsPosition_p = other.obsPosition_p;
    
    antennaPositions_p.resize(other.antennaPositions_p.shape());
    antennaPositions_p = other.antennaPositions_p;
  }
  
  // -------------------------------------------------------------------- destroy
  
  void ObservationData::destroy ()
  {;}
  

// ==============================================================================
//
//  Initialization
//
// ==============================================================================

// ------------------------------------------------------------------------- init

void ObservationData::init (const ObsInfo obsInfo)
{
  setDescription ("NONE");

  setObsInfo (obsInfo);

  // default values for antenna positions
  setAntennaPositions (Matrix<Double> (3,1,0.0));
}

// ------------------------------------------------------------------------- init

void ObservationData::init (const MEpoch obsDate,
			    String const &obsName,
			    Matrix<Double> const &antennaPositions,
			    String const &observer)
{
  MPosition obsPosition = observatoryPosition (obsName);
  
  init (obsDate,
	obsName,
	obsPosition,
	antennaPositions,
	observer);
}

// ------------------------------------------------------------------------- init

void ObservationData::init (const MEpoch obsDate,
			    String const &obsName,
			    const MPosition obsPosition,
			    Matrix<Double> const &antennaPositions,
			    String const &observer)
{
  obsInfo_p.setObsDate   (obsDate);
  obsInfo_p.setObserver  (observer);
  
  setDescription ("NONE");
  
  setObservatory (obsName,
		  obsPosition);
  
  setAntennaPositions (antennaPositions);
}


// ------------------------------------------------------- setObservatoryPosition

void ObservationData::setObservatoryPosition ()
{
  String obsName (obsInfo_p.telescope());
  
  obsPosition_p = observatoryPosition (obsName);
}

// ---------------------------------------------------------- observatoryPosition

MPosition ObservationData::observatoryPosition (String const &telescope)
{
  bool ok (true);
  MPosition obsPosition;
  
  try {
    MeasTable::initObservatories ();
  } catch (AipsError x) {
    cerr << "[SkymapGrid::grid] " << x.getMesg() << endl;
  }

  ok = MeasTable::Observatory(obsPosition,telescope);
  
  if (!ok) {
    cerr << "[ObservationData] Observatory"
	 << telescope
	 << "not found in database"
	 << endl;
    // get the list of available observatories
    cerr << MeasTable::Observatories() << endl;
  }

  return obsPosition;
}

// =============================================================================
//
//  Handling of the ObsInfo object:
//   - observation epoch
//   - observatory name
//   - name of the observer
//
// =============================================================================

void ObservationData::setObsInfo (ObsInfo const &obsInfo,
				  bool const &getObsPosition)
{
  // store the ObsInfo object
  obsInfo_p = obsInfo;
  
  // Try to set the observatory position from the observatory table
  if (getObsPosition) {
    ObservationData::setObservatoryPosition ();
  }
}

// --------------------------------------------------------------------- setEpoch

void ObservationData::setEpoch (const Quantity epoch)
{
  MEpoch obsDate (epoch);
  //
  obsInfo_p.setObsDate (obsDate);
}

// --------------------------------------------------------------------- setEpoch

void ObservationData::setEpoch (const MEpoch epoch)
{
  obsInfo_p.setObsDate (epoch);
}


// --------------------------------------------------------------- setObservatory

void ObservationData::setObservatory (const String obsName)
{
  obsInfo_p.setTelescope(obsName);
  //
  ObservationData::setObservatoryPosition ();
}

// --------------------------------------------------------------- setObservatory

void ObservationData::setObservatory (const String obsName,
				      const MPosition obsPosition)
{
  obsInfo_p.setTelescope(obsName);
  obsPosition_p = obsPosition;
}


// =============================================================================
//
//  Antenna setup of the telescope/observatory
//
// =============================================================================

// ------------------------------------------------------------- antennaPositions

Matrix<Double> ObservationData::antennaPositions (bool const &coordAxisFirst)
{
  if (coordAxisFirst) {
    return antennaPositions_p;
  } else {
    IPosition shape (antennaPositions_p.shape());
    Matrix<Double> positions (shape(1),shape(0));
    // invert the position vectors
    for (int antenna (0); antenna<shape(1); antenna++) {
      for (int coord (0); coord<shape(0); coord++) {
	positions(antenna,coord) = antennaPositions_p(coord,antenna);
      }
    }
    // return the rearranged matrix
    return positions;
  }
}

// ---------------------------------------------------------- setAntennaPositions

void ObservationData::setAntennaPositions (Matrix<Double> const &antennaPositions,
					   bool const &coordAxisFirst)
{
  IPosition shape (antennaPositions.shape());
  
  if (coordAxisFirst) {
    antennaPositions_p.resize(shape);
    antennaPositions_p = antennaPositions;
  }
  else{
    antennaPositions_p.resize(shape(1),shape(0));
    // invert the position vectors
    for (int antenna (0); antenna<shape(0); antenna++) {
      for (int coord (0); coord<shape(1); coord++) {
	antennaPositions_p(coord,antenna) = antennaPositions(antenna,coord);
      }
    }
  }
}

// ------------------------------------------------------------------ nofAntennas

uint ObservationData::nofAntennas ()
{
  IPosition shape (antennaPositions_p.shape());

  return shape(1);
}

// =============================================================================
//
//  Data derived from basic information
//
// =============================================================================

MeasFrame ObservationData::getObservationFrame ()
{
  MeasFrame frame (obsInfo_p.obsDate(),
		   obsPosition_p);

  return frame;
}

// ------------------------------------------------------------- conversionEngine

MDirection::Convert ObservationData::conversionEngine (const String refcode,
						       const bool toLocal)
{
  bool ok (true);
  MeasFrame frame (obsInfo_p.obsDate(),obsPosition_p);
  MDirection azel (SkymapperTools::MDirectionType ("AZEL"));
  MDirection other (SkymapperTools::MDirectionType (refcode));

  //------------------------------------------------------------------

  MDirection MDirectionFROM;
  MDirection MDirectionTO;
  MDirection::Types tp;
  
  if (toLocal) {
    MDirectionFROM = other;
    //
    MDirectionTO = azel;
    ok = MDirectionTO.getType(tp,"AZEL");
  } else {
    MDirectionFROM = azel;
    //
    MDirectionTO = other;
    ok = MDirectionTO.getType(tp,refcode);
  }

  //------------------------------------------------------------------
  
  MDirection::Convert conv (MDirectionFROM,
 			    MDirection::Ref(tp,frame));
  
  if (conv.isNOP()) {
    cerr << "[ObservationData] Conversion engine not operational."
	 << endl;
    //
    cerr << " - Conversion to local : " << toLocal << endl;
    cerr << " - MDirection (source) : " << MDirectionFROM << endl;
    cerr << "                       : " << MDirectionFROM.getRefString() << endl;
    cerr << " - MDirection (target) : " << MDirectionTO << endl;
    cerr << "                       : " << MDirectionTO.getRefString() << endl;
  }
  
  return conv;
}

// ------------------------------------------------------------- conversionEngine

MDirection::Convert ObservationData::conversionEngine (const String refcodeTO,
						       const String refcodeFROM)
{
  bool ok (true);
  MeasFrame frame (obsInfo_p.obsDate(),obsPosition_p);
  MDirection MDirectionFROM (SkymapperTools::MDirectionType (refcodeFROM));
  MDirection MDirectionTO (SkymapperTools::MDirectionType (refcodeTO));
  MDirection::Types tpFROM;
  MDirection::Types tpTO;

  // get the type of the target MDirection
  {
    MDirection md;
    //
    ok = md.getType (tpFROM,refcodeFROM);
    MDirectionFROM = MDirection(tpFROM);
    //
    ok = md.getType (tpTO,refcodeTO);
    MDirectionTO = MDirection(tpTO);
  }
  
  MDirection::Convert conv (MDirectionFROM,
  			    MDirection::Ref(tpTO,frame));

  if (conv.isNOP()) {
    cerr << "[ObservationData] Conversion engine not operational."
	 << endl;
  }
  
  return conv;
}

// =============================================================================
//
//  Feedback
//
// =============================================================================

  
  void ObservationData::summary ()
  {
    summary (std::cout);
  }
  
  void ObservationData::summary (std::ostream &os)
  {
    os << "[ObservationData]" << std::endl;
    os << " Description          = " << description()              << std::endl;
    os << " Observer             = " << observer()                 << std::endl;
    os << " Epoch                = " << epoch()                    << std::endl;
    os << " Observatory          = " << observatory()              << std::endl;
    os << " Observatory position = " << observatoryPosition()      << std::endl;
    os << " nof. antennas        = " << nofAntennas()              << std::endl;
    os << " antenna positions    = " << antennaPositions_p.shape() << std::endl;
  }
  
}  // -- Namespace CR -- end
