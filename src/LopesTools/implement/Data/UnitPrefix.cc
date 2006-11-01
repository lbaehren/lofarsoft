/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: template-class.cc,v 1.10 2006/10/31 19:19:57 bahren Exp $*/

#include <iostream>
#include <Data/UnitPrefix.h>

/*!
  \class UnitPrefix
*/

namespace LOPES { // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  UnitPrefix::UnitPrefix ()
  {
    init ();
  }
  
  UnitPrefix::UnitPrefix (UnitPrefix const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  UnitPrefix::~UnitPrefix ()
  {
    destroy();
  }
  
  void UnitPrefix::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  UnitPrefix& UnitPrefix::operator= (UnitPrefix const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void UnitPrefix::copy (UnitPrefix const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void UnitPrefix::init ()
  {
    nofPrefixes_p = 8;
    
    blitz::Array <std::string,1> tmp (nofPrefixes_p);

    {
      tmp = "none","nano","micro","milli","Kilo","Mega","Giga","Tera";
      names_p = tmp;
    }
//     symbols_p = "1","n","mu","m","K","M","G","T";

    for (int n(0); n<nofPrefixes_p; n++) {
      std::cout << n << "\t" << std::endl;
    }
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace LOPES -- end
