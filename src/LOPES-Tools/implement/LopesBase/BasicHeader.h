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

/* $Id: BasicHeader.h,v 1.7 2006/08/01 15:03:25 bahren Exp $*/

#ifndef BASICHEADER_H
#define BASICHEADER_H

// AIPS++/CASA includes
#include <casa/Containers/Record.h>

using casa::Record;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class BasicHeader
    
    \ingroup LopesBase
    
    \brief Basic header information for a LOPES-Tools object
    
    \author Lars B&auml;hren
    
    \date 2005/07/20
    
    \test tBasicHeader.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[AIPS++] Record -- A hierarchical collection of named fields of various
                 types. The Record class is a particular type of a record class.
		 The fields in Record may be of scalar type, array type, or a
		 Record. The types are chosen to be compatible with the native
		 types of the Table system, viz: Bool, uChar, Short, Int, uInt,
		 float, double, Complex, DComplex, String. Arrays of all these
		 types are also available.<br>
		 Record uses copy-on-write semantics. This means that when a
		 Record is copied, only the pointer to the underlying RecordRep
		 object is copied. Only when the Record gets changed (i.e. when
		 a non-const Record member function is called), the RecordRep
		 object is copied. This results in a cheap copy behaviour.
    </ul>
    
    <h3>Synopsis</h3>
    
    In order to make meta data available to objects derived from BasicObject,
    we define a mechanism to store and access keyword/value pairs.
    
    For the tree as a whole, header information should be available for
    - an individual object
    - a branch of a tree 
    - the tree
    In order to keep access to header information, a HeaderExtraction class
    should wrap the actual request and extraction of a certain header information,
    regardles where it is stored in the tree.
    
    Information on the type of the stored data can be handled as flags, e.g. 
    \verbatim
    static int IS_ARRAY   = 00000001;
    static int IS_COMPLEX = 00000010;
    static int IS_STRING  = 00000100;
    \endverbatim
  */
  
  class BasicHeader {
    
    //! Record structure to store the meta data.
    Record header_p;
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor
      
      This is the default constructor.
    */
    BasicHeader ();
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~BasicHeader ();
    
    // --- Parameters ------------------------------------------------------------
    
    BasicHeader& header ();
    
    Record& headerRecord () {
      return header_p;
    }
    
    bool setHeader (const Record& header);
    
  };
  
}  // Namespace LOPES -- end

#endif
  
