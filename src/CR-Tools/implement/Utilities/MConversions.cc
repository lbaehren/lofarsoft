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

#include <Utilities/MConversions.h>

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

  //_____________________________________________________________________________
  //                                                              ProjectionNames
  
  std::vector<std::string> ProjectionNames ()
  {
    std::vector<std::string> projection;
    
    projection.push_back("AIR");    /* Airy                     */
    projection.push_back("AIT");    /* Hammer-Aitoff            */
    projection.push_back("CAR");    /* Cartesian                */
    projection.push_back("MER");    /* Mercator                 */
    projection.push_back("MOL");    /* Molweide                 */
    projection.push_back("SIN");    /* Orthographics/synthesis  */
    projection.push_back("STG");    /* Stereographic            */
    projection.push_back("TAN");    /* Gnomonic                 */
    
    return projection;
  }
  
  // ============================================================================
  //
  //  Directions
  //
  // ============================================================================
  
  // ------------------------------------------------------------- MDirectionType
  
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

  // ------------------------------------------------------------- MDirectionName

  casa::String MDirectionName (casa::MDirection::Types const &tp)
  {
    casa::MDirection md;

    return md.showType(tp);
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
