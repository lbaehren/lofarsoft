/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *   Andreas Horneffer (a.horneffer@astro.ru.nl)                           *
 *   Sven Lafebre (s.lafebre@astro.ru.nl)                                  *
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

#ifndef DATACOORDINFO_H
#define DATACOORDINFO_H

// C++ Standard library
#include <ostream>
#include <fstream>
#include <sstream>
using namespace std;

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>

using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin

  /*!
    \class DataCoordinfo
    
    \ingroup LopesLegacy
    
    \brief Coordinate information for the fields for the TIM40 system
    
    \author Lars B&auml;hren
    
    <h3>Synopsis</h3>
    
    This class implements the basic building block of the data structure created
    in <tt>TIM40coordinforec()</tt>.
    
    \verbatim
    ci      := [=];
    ci.Pos  := [Units='Sample', DefAxis='x', Domain='T', Stored=F, 
                Ref='-', Type='real', GenFunc=symbol_value('TIM40genPos')];
    ci.Time := [Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
                Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TIM40convPos2Time')];
    \endverbatim
  */
  
  class DataCoordinfo {
    
  private:
    
    String name_p;
    String units_p;
    String defAxis_p;
    String domain_p;
    bool stored_p;
    String ref_p;
    String type_p;
    String genFunc_p;
    
  public:
    
    //! Empty constructor
    DataCoordinfo ();
    
    //! Destructor
    ~DataCoordinfo();
    
    //! Get the name of the coordinate
    inline String name () { 
      return name_p;
    }
    
    //! Get the name of the coordinate
    void setName (const String);
    
    //! Get the units of the coordinate
    inline String units () {
      return units_p;
    }
    
    //! Set the units of the coordinate
    void setUnits (const String);
    
    //! Get the name of the definition domain axis
    String defAxis ();
    
    //! Set the name of the definition domain axis
    void setDefAxis (const String);
    
    //! Get the domain to which the coordinate belongs to.
    String domain ();
    
    //! Set the domain to which the coordinate belongs to.
    void setDomain (const String);
    
    //! Are the data values for this coordinate stored/cached?
    bool stored ();
    
    //! Enable/Disable storage/caching of data values for this coordinate.
    void setStored (const bool);
    
    String ref ();
    
    void setRef (const String);
    
    String type ();
    
    void setType (const String);
    
    String genFunc ();
    
    void setGenFunc (const String);
  };

}  // Namespace CR -- end

#endif
