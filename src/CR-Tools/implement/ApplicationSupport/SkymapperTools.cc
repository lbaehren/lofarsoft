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

/* $Id$*/

#include <ApplicationSupport/SkymapperTools.h>
#include <Utilities/MConversions.h>

using CR::ObservationData;
using CR::SkymapGrid;

// ----------------------------------------------------------------- skymapCenter

Vector<Double> SkymapperTools::skymapCenter (const Quantity epoch,
					     const String telescope,
					     const String refcodeSkymap,
					     const String refcodeInput,
					     const Vector<Double> &centerInput)
{
  Bool verbose (True);
  Vector<Double> centerSkymap(centerInput.shape());

  ObservationData obsData (epoch,telescope);
  
  // Do not go through the coordinate conversion unless required; first check
  // if the center position is given already on the reference frame of the map

  if (refcodeSkymap == refcodeInput) {
    centerSkymap = centerInput;
  } else {
    try {
      MVDirection MVDirectionFROM;
      Vector<Quantity> QDirectionTO(2);
      //
      MDirection::Convert conv = obsData.conversionEngine (refcodeSkymap,
							   refcodeInput);
      //
      MVDirectionFROM = MVDirection (Quantity(centerInput(0),"deg"),
				     Quantity(centerInput(1),"deg"));
      //
      QDirectionTO = conv(MVDirectionFROM).getValue().getRecordValue();
      //
      centerSkymap(0) = QDirectionTO(0).getValue("deg");
      centerSkymap(1) = QDirectionTO(1).getValue("deg"); 
    } catch (AipsError x) {
      cerr << "[Skymapper::adjustCenterPosition] " << x.getMesg() << endl;
    }
  }
  
  if (verbose) {
    cout << "[Skymapper::adjustCenterPosition]" << endl;
    cout << " - Observation epoch  : " << obsData.epoch() << endl;
    cout << " - Observatory name   : " << obsData.observatory() << endl;
    cout << " - Map center, input  : " << centerInput
	 << " (" << refcodeInput << ")" << endl;
    cout << " - Map center, skymap : " << centerSkymap
	 << " (" << refcodeSkymap << ")" << endl;
  }
   
  return centerSkymap;
}

// ---------------------------------------------------------------- setSkymapGrid

void SkymapperTools::setSkymapGrid (Skymap &skymap,
				    const Quantity epoch,
				    const String telescope,
				    const String refcodeSkymap,
				    const String refcodeCenter,
				    const String projection,
				    const IPosition& shape,
				    const Vector<Double>& center,
				    const Vector<Double>& increment)
{
  Vector<Double> centerSkymap (center.shape());

  // -----------------------------------------------------------------
  // Adjust the world coordiates for the center pixel

  centerSkymap = SkymapperTools::skymapCenter (epoch,
					       telescope,
					       refcodeSkymap,
					       refcodeCenter,
					       center);

  // -----------------------------------------------------------------
  // Convert epoch and telescope information to an ObsInfo object

  ObservationData obsData (epoch,telescope);
  ObsInfo obsInfo (obsData.obsInfo());

  // -----------------------------------------------------------------
  // Write the data to the SkymapGrid object embedded in the Skymap

  skymap.setSkymapGrid (obsInfo,
			refcodeSkymap,
			projection,
			shape,
			centerSkymap,
			increment);
}


// ==============================================================================
//
//  Handling of coordinates and coordinate systems
//
// ==============================================================================

// ---------------------------------------------------------------------- obsinfo

ObsInfo SkymapperTools::obsinfo (const Time &obsTime,
				 const String &telescope,
				 const String &observer)
{
  MEpoch obsDate (MVEpoch(obsTime.modifiedJulianDay()));

  return SkymapperTools::obsinfo (obsDate, telescope, observer);
}

// ---------------------------------------------------------------------- obsinfo

ObsInfo SkymapperTools::obsinfo (const Quantity &obsTime,
				 const String &telescope,
				 const String &observer)
{
  MEpoch obsDate (obsTime);

  return SkymapperTools::obsinfo (obsDate, telescope, observer);
}

// ---------------------------------------------------------------------- obsinfo

ObsInfo SkymapperTools::obsinfo (const MEpoch &obsDate,
				 const String &telescope,
				 const String &observer)
{
  ObsInfo obs;
  obs.setTelescope(telescope);
  obs.setObserver(observer);
  obs.setObsDate(obsDate);

  return obs;
}

// ------------------------------------------------------------- coordinateSystem

CoordinateSystem SkymapperTools::coordinateSystem (const String &imagefile,
						   const ObsInfo &obsinfo,
						   const Bool replaceObsInfo)
{
  CoordinateSystem csys;

  try {
    PagedImage<Float> image (imagefile);
    csys = coordinateSystem (image,
			     obsinfo,
			     replaceObsInfo);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    csys = CoordinateSystem();
  }

  return csys;
}

// ------------------------------------------------------------- coordinateSystem

CoordinateSystem SkymapperTools::coordinateSystem (const PagedImage<Float> &image,
						   const ObsInfo &obsinfo,
						   const Bool replaceObsInfo)
{
  CoordinateSystem csys = image.coordinates();

  if (replaceObsInfo) {
    csys.setObsInfo (obsinfo);
  }

  return csys;
}

// ------------------------------------------------------------- coordinateSystem

CoordinateSystem SkymapperTools::coordinateSystem(const ObsInfo &obsinfo,
						  const String &refcode,
						  const String &projection,
						  const IPosition &shape,
						  const Vector<Double> &crpix,
						  const Vector<Double> &crval,
						  const Vector<Double> &cdelt)
{
  bool status (true);
  CoordinateSystem cs;

  // assign observation info
  cs.setObsInfo(obsinfo);

  // -- set up the DirectionAxis ---------------------------
  try {
    Matrix<Double> xform(2,2);
    
    xform = 0.0;
    xform.diagonal() = 1.0;
    
    DirectionCoordinate coord (CR::MDirectionType(refcode),
			       Projection(CR::ProjectionType(projection)),
			       crval(0)*C::pi/180.0,
			       crval(1)*C::pi/180.0,
			       -2*C::pi/180.0,
			       2*C::pi/180,
			       xform,
			       0.5*shape(0),0.5*shape(1));

    cs.addCoordinate(coord);
  } catch (AipsError x) {
    cerr << "[Skymapper::setCoordinateSystem] " << x.getMesg() << endl;
    status = false;
  }

  // -- set up the distance axis ---------------------------
  try {
    Vector<String> names (1,"Distance");
    Vector<String> units (1,"m");
    Vector<Double> refPix (1,crpix(2));
    Vector<Double> refVal (1,crval(2));
    Vector<Double> inc (1,cdelt(2));
    Matrix<Double> pc (1,1,1.0);
    
    LinearCoordinate coord (names,
			    units,
			    refVal,
			    inc,
			    pc,
			    refPix);

    cs.addCoordinate(coord);
  } catch (AipsError x) {
    cerr << "[Skymapper::setCoordinateSystem] " << x.getMesg() << endl;
    status = false;
  }

  // -- set up the time axis -------------------------------

  try {
    Vector<String> names  (1,"Time");
    Vector<String> units  (1,"s");
    Vector<Double> refPix (1,crpix(3));
    Vector<Double> refVal (1,crval(3));
    Vector<Double> inc    (1,cdelt(3));
    Matrix<Double> pc     (1,1,1.0);
    
    LinearCoordinate coord (names,
			    units,
			    refVal,
			    inc,
			    pc,
			    refPix);

    cs.addCoordinate(coord);
  } catch (AipsError x) {
    cerr << "[Skymapper::setCoordinateSystem] " << x.getMesg() << endl;
    status = false;
  }

  // -- set up the frequency axis --------------------------

  try {
    SpectralCoordinate coord (MFrequency::TOPO,
			      crval(4),
			      cdelt(4),
			      crpix(4),
			      0.0);

    cs.addCoordinate(coord);
  } catch (AipsError x) {
    cerr << "[Skymapper::setCoordinateSystem] " << x.getMesg() << endl;
    status = false;
  }

  // if everything so far went ok, we can assign the coordinate system tool
  if (!status) {
    return CoordinateSystem();
  }
  
  return cs;
}

// ------------------------------------------------------------- coordinateSystem

CoordinateSystem SkymapperTools::coordinateSystem (GlishRecord const &glishRec)
{
  Record rec;
  CoordinateSystem csys;

  // convert GlishRecord to Record
  glishRec.toRecord(rec);
  CoordinateSystem *csys_tmp = CoordinateSystem::restore(rec, "");
  
  if (csys_tmp==0) {
    std::cerr << "[SkymapperTools::coordinateSystem] "
	      << "Failed to create a CoordinateSystem from this record"
	      << std::endl;
  } else {
    csys = *csys_tmp;
  }
  
  return csys;
}

