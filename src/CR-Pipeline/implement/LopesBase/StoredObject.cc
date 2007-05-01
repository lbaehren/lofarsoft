/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: StoredObject.cc,v 1.12 2007/04/17 13:52:47 bahren Exp $*/

#include <LopesBase/StoredObject.h>

// --- Construction ------------------------------------------------------------


template <class T> StoredObject<T>::StoredObject () 
  : BasicObject()
{	
  className_p = "StoredObject";
  format_p.setFlag(0);
}

template <class T> StoredObject<T>::StoredObject (uInt format) 
  : BasicObject()
{	
  className_p = "StoredObject";
  format_p.setFlag(format);
}

template <> StoredObject<Double>::StoredObject () 
  : BasicObject()
{	
  className_p = "StoredObject";
  format_p.setFlag(EF_isNumeric);
}

template <> StoredObject<DComplex>::StoredObject () 
  : BasicObject() {	
  className_p = "StoredObject";
  format_p.setFlag(EF_isNumeric|EF_isComplex);
}

// --- Destruction -------------------------------------------------------------

template <class T> StoredObject<T>::~StoredObject () {
  cleanup();
}

// --- Parameters --------------------------------------------------------------

template<class T> Bool StoredObject<T>::get(void *ref) {
  Vector<T> *vecpoint;
  if (valid_p || updateStorage()) {
    try {
      vecpoint = (Vector<T> *)ref;
      vecpoint->reference(data_p);
      return True;
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      cerr << "get: error while referencing data, probably got wrong type as argument." << endl;
      return False;
    };
  } else {
    throw(AipsError("Cannot return invalid data"));
    return False;
  };
}

// ==============================================================================
//
// Template Instantiation
//
// ==============================================================================

template class StoredObject<Double>;
template class StoredObject<DComplex>;
