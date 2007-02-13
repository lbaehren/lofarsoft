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
/* $Id: BasicObject.cc,v 1.13 2006/07/05 12:59:12 bahren Exp $*/

#include <Nodes/BasicObject.h>

using std::cerr;
using std::endl;

namespace CR {  // Namespace CR -- begin
  
  // ---------------------------------------------------------------- BasicObject
  
  BasicObject::BasicObject () {
    init();
  }
  
  // --------------------------------------------------------------- ~BasicObject
  
  BasicObject::~BasicObject () {
    cleanup();
  }
  
  // ----------------------------------------------------------------- invalidate
  
  bool BasicObject::invalidate () {
    uint i (0);
    
    if (valid_p) {
      valid_p = false;
      try {
	for (i = 0; i < nChild_p; i ++){
	  child_p[i]->invalidate();
	};
      } catch (AipsError x) {
	cerr << x.getMesg() << endl;
	return false;
      };
    };
    return true;  
  }
  
  // ----------------------------------------------------------------------- init
  
  bool BasicObject::init() {
    className_p = "BasicObject";
    valid_p     = false;
    
    parent_p    = NULL;
    nParent_p   = 0;
    
    child_p     = NULL;
    nChild_p    = 0;
    
    return true;
  }
  
  // -------------------------------------------------------------------- cleanup
  
  bool BasicObject::cleanup () {
    if (parent_p != NULL)
      delete [] parent_p;
    
    if (child_p  != NULL)
      delete [] child_p;
    
    return true;
  }
  
  // ------------------------------------------------------------------- addChild
  
  bool BasicObject::addChild (BasicObject* obj) {
    BasicObject** tmparr;
    
    tmparr = new BasicObject* [nChild_p+1];
    if (child_p != NULL) {
      for (uint i = 0; i < nChild_p; i ++)
	tmparr[i] = child_p[i];
      delete [] child_p;
    }
    tmparr[nChild_p++] = obj;
    child_p = tmparr;
    
    return true;
  }
  
  // ------------------------------------------------------------------ putParent
  
  bool BasicObject::putParent(uint num,
			      BasicObject* obj) {
    if (num >= nParent_p) {
      throw(AipsError("putParent: Index exceeds number of parents."));
      return false;
    };
    
    if (parent_p[num] != NULL) {
      throw(AipsError("putParent: Parent was already set."));
      return false;
    };
    parent_p[num] = obj;
    obj->addChild(this);
    
    return true;
  }
  
  // --------------------------------------------------------------- listChildren
  
  Vector<String> BasicObject::listChildren(){
    Vector<String> erg;
    return erg;
  }
  
  // ---------------------------------------------------------------- listParents
  
  Vector<String> BasicObject::listParents(){
    Vector<String> erg;
    return erg;
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
for (uint i = 0; i < ar->size()-1; i ++) {
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
