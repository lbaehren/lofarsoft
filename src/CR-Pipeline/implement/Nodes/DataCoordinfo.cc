/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <Nodes/DataCoordinfo.h>

namespace CR {  // Namespace CR -- begin
  
  // =============================================================================
  //
  //  Construction / Destruction
  //
  // =============================================================================
  
  DataCoordinfo::DataCoordinfo ()
  {
    name_p = units_p = defAxis_p = domain_p = "UNDEFINED";
  }
  
  DataCoordinfo::~DataCoordinfo()
  {
    stored_p = false;
  }
  
  // =============================================================================
  //
  //  Access to member data
  //
  // =============================================================================
  
  String DataCoordinfo::name () {
    return name_p;
  }
  
  void DataCoordinfo::setName (String const &name) {
    name_p = name;
  }
  
  String DataCoordinfo::units () {
    return units_p;
  }
  
  void DataCoordinfo::setUnits (String const &units) {
    units_p = units;
  }
  
  String DataCoordinfo::defAxis () {
    return defAxis_p;
  }
  
  void DataCoordinfo::setDefAxis (String const &defAxis) {
    defAxis_p = defAxis;
  }
  
  String DataCoordinfo::domain () {
    return domain_p;
  }
  
  void DataCoordinfo::setDomain (String const &domain) {
    domain_p = domain;
  }
  
  bool DataCoordinfo::stored () {
    return stored_p;
  }
  
  void DataCoordinfo::setStored (const bool stored) {
    stored_p = stored;
  }
  
  String DataCoordinfo::ref () {
    return ref_p;
  }
  
  void DataCoordinfo::setRef (String const &ref) {
    ref_p = ref;
  }
  
  String DataCoordinfo::type () {
    return type_p;
  }
  
  void DataCoordinfo::setType (String const &type) {
    type_p = type;
  }
  
  String DataCoordinfo::genFunc () {
    return genFunc_p;
  }
  
  void DataCoordinfo::setGenFunc (String const &genFunc) {
    genFunc_p = genFunc;
  }
  
}  // Namespace CR -- end
