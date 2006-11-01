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

/* $Id: MSFiller.cc,v 1.1 2006/07/13 14:11:54 bahren Exp $*/

#include <lopes/Data/MSFiller.h>
#include <lopes/Utilities/ParameterFromRecord.h>

/*!
  \class MSFiller
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

MSFiller::MSFiller ()
{;}

MSFiller::MSFiller (string const &msFilename)
{
  msFilename_p = msFilename;
}

MSFiller::MSFiller (MSFiller const &other)
{
  copy (other);
}

void MSFiller::init ()
{
  setComponents ();
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

MSFiller::~MSFiller ()
{
  destroy();
}

void MSFiller::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

MSFiller& MSFiller::operator= (MSFiller const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void MSFiller::copy (MSFiller const &other)
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void MSFiller::setComponents ()
{
  ComponentList mymodel;
  SkyComponent point (ComponentType::POINT);
  point.flux() = Flux<Double>(1.0, 0.0, 0.0, 0.0);
  point.shape().setRefDirection(MDirection(Quantity(0, "deg"),
					   Quantity(90,"deg"),
					   MDirection::AZEL));
  mymodel.add(point);
  //
  components_p = mymodel;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------------ setElevationLimit

bool MSFiller::setElevationLimit (GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  Quantity elevationLimit (10.0,"deg");

  status = QuantityFromRecord (elevationLimit,"elevationLimit",rec);

  if (status) {
    MSSimulator::setElevationLimit (elevationLimit);
  } else {
    std::cerr << "[MSFiller::setElevationLimit]" << std::endl;
  }

  return status;
}

// --------------------------------------------------------- setAutoCorrelationWt

bool MSFiller::setAutoCorrelationWt (GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  Float autoCorrWeight (1.0);
  
  if (rec.exists("autoCorrWeight")) {
    gtmp = rec.get("autoCorrWeight");
    gtmp.get(autoCorrWeight);
  } else {
    status = false;
  }

  if (status) {
      MSSimulator::setAutoCorrelationWt(autoCorrWeight);
  } else {
    std::cerr << "[MSFiller::setAutoCorrelationWt]" << std::endl;
  }

  return status;
}

// --------------------------------------------------------------------- setTimes

bool MSFiller::setTimes (GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  Quantity integrationTime (1.0, "s");
  Quantity gapTime (1.0, "s");
  Bool useHourAngle (False);
  Quantity startTime (0.0, "s");
  Quantity stopTime  (1.0, "s");
  MEpoch refTime (MVEpoch(51483.18946969),MEpoch::UTC);
  
  status = QuantityFromRecord (integrationTime, "integrationTime", rec);
  status = QuantityFromRecord (gapTime,         "gapTime",         rec);
  status = QuantityFromRecord (startTime,       "startTime",       rec);
  status = QuantityFromRecord (stopTime,        "stopTime",        rec);

  if (rec.exists("useHourAngle")) {
    gtmp = rec.get("useHourAngle");
    gtmp.get(useHourAngle);
  } else {
    status = false;
  }
  
  if (status) {
    MSSimulator::setTimes(integrationTime,
			  gapTime,
			  useHourAngle,
			  startTime,
			  stopTime,
			  refTime);
  } else {
    std::cerr << "[MSFiller::setTimes]" << std::endl;
  }

  return status;
}

// ---------------------------------------------------------------------- initAnt

bool MSFiller::initAnt (GlishRecord &rec)
{
  GlishArray gtmp;
  bool status (true);
  
  String telescope;
  Matrix<Double> antennaPositions;

  if (rec.exists("telescope")) {
    gtmp = rec.get("telescope");
    gtmp.get(telescope);
  } else {
    status = false;
  }
  
  if (rec.exists("antennaPositions")) {
    gtmp = rec.get("antennaPositions");
    gtmp.get(antennaPositions);
  } else {
    status = false;
  }
  


  return status;
}
