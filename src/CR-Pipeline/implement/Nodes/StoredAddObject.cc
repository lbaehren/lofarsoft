/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: StoredAddObject.cc,v 1.1 2005/07/27 16:12:18 horneff Exp $*/

#include <Nodes/StoredAddObject.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::AipsError;
using casa::Complex;
using casa::DComplex;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  // --- Construction ------------------------------------------------------------
  
  template<class T> StoredAddObject<T>::StoredAddObject (uint parents)
    : StoredObject<T>() {
    BasicObject::className_p = "StoredAddObject";
    if (parents <= 1) {
      throw(AipsError("StoredAddObject: Need at least two parents"));
    }
    nParent_p = parents;
    parent_p = new BasicObject* [nParent_p];
    uint i;
    for (i=0;i<nParent_p;i++) { parent_p[i] = NULL;};
  }
  
  // --- Destruction -------------------------------------------------------------
  
  template<class T> StoredAddObject<T>::~StoredAddObject ()
  {;}
  
  
  // --- Children & Parents ----------------------------------------------------
  
  bool StoredAddObject<double>::setParent(uint num, BasicObject* obj) {
    if (obj->format(EF_isComplex) || !obj->format(EF_isNumeric)){
      throw(AipsError("setParent: Parent not of desired format!"));
      return false;
    }
    return putParent(num,obj);
  }
  
  bool StoredAddObject<DComplex>::setParent(uint num, BasicObject* obj) {
    if (!obj->format(EF_isNumeric)){
      throw(AipsError("setParent: Parent not of desired format!"));
      return false;
    }
    return putParent(num,obj);
  }
  
  // --- Parameters -------------------------------------------------------------
  
  bool StoredAddObject<double>::updateStorage(){
    Vector<double> tmpvec;
    double tmpval=0.;
    bool had_vec=false;
    uint i;
    try{
      for (i=0;i<nParent_p;i++) {
	if (!parent_p[i]->format(EF_isScalar)){
	  if (!had_vec) {
	    had_vec=true;
	    parent_p[i]->get(&data_p);
	    data_p.unique();
	  } else {
	    parent_p[i]->get(&tmpvec);
	    data_p += tmpvec;
	  };
	} else {
	  parent_p[i]->get(&tmpvec);
	  tmpval += tmpvec(1);
	};
      };
      if (had_vec) {
	data_p += tmpval;
      } else {
	data_p = Vector<double>(1,tmpval);
      };
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      return false;
    };
    
    cout << "Ran StoredAddObject<double>::updateStorage() sucessfully" <<endl;
    
    valid_p = true;
    return true;
  }
  
  bool StoredAddObject<DComplex>::updateStorage(){
    Vector<double> Dvec;
    Vector<DComplex> Cvec,tmpCvec;
    DComplex tmpval=1.;
    bool had_vec=false;
    uint i;
    try{
      for (i=0;i<nParent_p;i++) {
	if (!parent_p[i]->format(EF_isScalar)){
	  if (!had_vec) {
	    had_vec=true;
	    if (!parent_p[i]->format(EF_isComplex)){
	      parent_p[i]->get(&Dvec);
	      data_p.resize(Dvec.shape());
	      convertArray(data_p,Dvec);
	    } else {
	      parent_p[i]->get(&data_p);
	      data_p.unique();
	    };
	  } else {
	    if (!parent_p[i]->format(EF_isComplex)){
	      parent_p[i]->get(&Dvec);
	      tmpCvec.resize(Dvec.shape());
	      convertArray(tmpCvec,Dvec);
	      Cvec.reference(tmpCvec);
	    } else {
	      parent_p[i]->get(&Cvec);
	    };
	    data_p += Cvec;
	  };
	} else {
	  if (!parent_p[i]->format(EF_isComplex)){
	    parent_p[i]->get(&Dvec);
	    tmpval += Dvec(1);
	  } else {
	    parent_p[i]->get(&Cvec);
	    tmpval += Cvec(1);
	  };
	}
      };
      if (had_vec) {
	data_p += tmpval;
      } else {
	data_p = Vector<DComplex>(1,tmpval);
      };
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      return false;
    };
    
    cout << "Ran StoredAddObject<DComplex>::updateStorage() sucessfully" <<endl;
    
    valid_p = true;
    return true;
  }

  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================
  
  template StoredAddObject<float>;
  template StoredAddObject<double>;
  template StoredAddObject<Complex>;
  template StoredAddObject<DComplex>;
  
}  // Namespace CR -- end
  
