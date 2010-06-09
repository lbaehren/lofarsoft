/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Coordinates/MConversions.h>

namespace CR { // Namespace CR -- begin
  
  // ------------------------------------------------------------- ProjectionType
  
  casa::Projection::Type ProjectionType (String const &refcode)
  {
    casa::Projection prj;
    casa::Projection::Type tp (casa::Projection::STG);
    
    try {
      tp = prj.type(refcode);
    } catch (casa::AipsError x) {
      std::cerr << "[MConversions::ProjectionType] " << x.getMesg() << std::endl;
    }
    
    return tp;
  }

  // ============================================================================
  //
  //  Directions
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                               MDirectionType

  casa::MDirection::Types MDirectionType (String const &refcode)
  {
    bool ok (true);
    casa::MDirection md;
    casa::MDirection::Types tp;
    //
    try {
      ok = md.getType(tp,refcode);
    } catch (casa::AipsError x) {
      std::cerr << "[MConversions::MDirectionType] " << x.getMesg() << std::endl;
    }
    //
    return tp;
  }

  //_____________________________________________________________________________
  //                                                               MDirectionName

  casa::String MDirectionName (casa::MDirection::Types const &tp)
  {
    casa::MDirection md;

    return md.showType(tp);
  }

  //_____________________________________________________________________________
  //                                                              MDirectionNames

  std::vector<std::string> MDirectionNames ()
  {
    std::vector<std::string> refcode;
    
    refcode.push_back("AZEL");      /* topocentric Azimuth and Elevation (N through E) */
    refcode.push_back("AZELSW");    /* topocentric Azimuth and Elevation (S through W) */
    refcode.push_back("AZELNE");    /* topocentric Azimuth and Elevation (N through E) */
    refcode.push_back("AZELGEO");   /* geodetic Azimuth and Elevation (N through E)    */
    refcode.push_back("AZELSWGEO"); /* geodetic Azimuth and Elevation (S through W)    */
    refcode.push_back("AZELNEGEO"); /* geodetic Azimuth and Elevation (N through E)    */
    refcode.push_back("B1950");     /* mean epoch and ecliptic at B1950.0              */
    refcode.push_back("ECLIPTIC");  /* ecliptic for J2000 equator and equinox          */
    refcode.push_back("ICRS");      /* International Celestial reference system        */
    refcode.push_back("J2000");     /* mean equator and equinox at J2000.0 (FK5)       */
    refcode.push_back("GALACTIC");  /* galactic coordinates                            */
    
    return refcode;
  }

  // ============================================================================
  //
  //  Positions
  //
  // ============================================================================

  // -------------------------------------------------------- ObservatoryPosition

  casa::MPosition ObservatoryPosition (String const &telescope)
  {
    bool ok (true);
    casa::MPosition obsPosition;
    
    try {
      casa::MeasTable::initObservatories ();
    } catch (casa::AipsError x) {
      std::cerr << "[SkymapGrid::grid] " << x.getMesg() << std::endl;
    }
    
    ok = casa::MeasTable::Observatory(obsPosition,
				      telescope);
    
    if (!ok) {
      std::cerr << "[ObservationData] Observatory"
	   << telescope
	   << "not found in database"
		<< std::endl;
      // get the list of available observatories
      std::cerr << casa::MeasTable::Observatories() << std::endl;
    }
    
    return obsPosition;
  }

  // ============================================================================
  //
  //  Conversions
  //
  // ============================================================================

  // --------------------------------------------------------------- LOPES2MEpoch

  casa::MEpoch LOPES2MEpoch(casa::uInt JDR,
			    int TL)
  {
    double offset (3506716800.);

    return casa::MEpoch(casa::MVEpoch(Quantity(((double)JDR+((double)TL/5e6)+offset),
					       "s")),
			casa::MEpoch::Ref(casa::MEpoch::UTC));
  }
  
} // Namespace CR -- end
