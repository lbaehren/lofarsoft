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

/* $Id: BasicObject.cc,v 1.14 2007/04/17 13:52:47 bahren Exp $*/

#include <LopesBase/BasicObject.h>

using std::cout;
using std::endl;

namespace CR {  // Namespace CR -- begin
  
  // --- Construction ------------------------------------------------------------
  
  BasicObject::BasicObject ()
    : className_p("BasicObject"),
      title_p("UNDEFINED")
  {
    init();
  }
  
  // --- Destruction -------------------------------------------------------------
  
  BasicObject::~BasicObject () {
    cleanup();
  }
  
  
  // --- Parameters --------------------------------------------------------------
  
  bool BasicObject::invalidate () {
    uInt i (0);
    
    if (valid_p) {
      valid_p = false;
      try {
	for (i = 0; i < nChild_p; i ++){
	  child_p[i]->invalidate();
	};
      } catch (casa::AipsError x) {
	std::cerr << x.getMesg() << std::endl;
	return false;
      };
    };
    return true;  
  }
  
  // --- Functions -------------------------------------------------------------
  
  bool BasicObject::init() {
    className_p = "BasicObject";
    valid_p     = false;
    
    parent_p    = NULL;
    nParent_p   = 0;
    
    child_p     = NULL;
    nChild_p    = 0;
    
    return true;
  }
  
  bool BasicObject::cleanup () {
    if (parent_p != NULL)
      delete [] parent_p;
    
    if (child_p  != NULL)
      delete [] child_p;
    
    return true;
  }
  
  bool BasicObject::addChild (BasicObject* obj) {
    BasicObject** tmparr;
    
    tmparr = new BasicObject* [nChild_p+1];
    if (child_p != NULL) {
      for (uInt i = 0; i < nChild_p; i ++)
	tmparr[i] = child_p[i];
      delete [] child_p;
    }
    tmparr[nChild_p++] = obj;
    child_p = tmparr;
    
    return true;
  }
  
  bool BasicObject::putParent(uInt num, BasicObject* obj) {
    if (num >= nParent_p) {
      throw(casa::AipsError("putParent: Index exceeds number of parents."));
      return false;
    };
    
    if (parent_p[num] != NULL) {
      throw(casa::AipsError("putParent: Parent was already set."));
      return false;
    };
    parent_p[num] = obj;
    obj->addChild(this);
    
    return true;
  }
  
  // ----------------------------------------------------------------- listChildren
  
  Vector<String> BasicObject::listChildren(){
    Vector<String> erg;
    return erg;
  }
  
  // -------------------------------------------------------------------listParents
  
  Vector<String> BasicObject::listParents(){
    Vector<String> erg;
    return erg;
  }
  
  // ---------------------------------------------------------------------- summary
  
  void BasicObject::summary ()
  {
    cout << "-- Class name            = " << className()    << endl;
    cout << "-- Object title          = " << title()        << endl;
    cout << "-- nof. parents          = " << nParent_p      << endl;
    cout << "-- Is this a valid object? " << valid_p        << endl;
    cout << "-- nof. children         = " << nChild_p       << endl;
    cout << "-- Parent objects        = " << listParents()  << endl;
    cout << "-- Child objects         = " << listChildren() << endl;
  }
  
}  // Namespace CR -- end

// Assorted junk below

/*bool BasicObject::removeNode (Vector<BasicObject*> *ar, BasicObject* obj, bool ret) {
  
try {
// If there are no children, just go back
if (ar->size() == 0)
return false;

// If the last element matches, resize and go back
if (ar->data()[ar->size()-1] == obj) {
if (ret)
obj->removeParent(this, false);
ar->resize(ar->size()-1,true);
return true;
}

bool done = false;

// Check for a match; once we have one, start contracting the array
for (uInt i = 0; i < ar->size()-1; i ++) {
if (ar->data()[i] == obj)
done = true;
if (done == true) 
ar->data()[i] = ar->data()[i+1];
}

// If there was no match, go back
if (done == false)
return false;

// Resize array if there was a match
if (ret)
obj->removeParent(this, false);
ar->resize(ar->size()-1,true);

} catch (AipsError x) {
cerr << x.getMesg() << endl;
return false;
}
return true;
}*/
