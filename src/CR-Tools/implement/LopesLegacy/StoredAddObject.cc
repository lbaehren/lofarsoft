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

/* $Id: StoredAddObject.cc,v 1.5 2007/05/02 08:14:15 bahren Exp $*/

#include <LopesLegacy/StoredAddObject.h>

using std::cerr;
using std::cout;
using std::endl;

namespace CR {  //  Namespace CR -- begin
  
  // --- Construction ------------------------------------------------------------
  
  template<class T> StoredAddObject<T>::StoredAddObject (uInt parents)
    : StoredObject<T>()
  {
    uInt i;
    
    this->className_p = "StoredAddObject";
    if (parents <= 1) {
      throw(AipsError("StoredAddObject: Need at least two parents"));
    };
    this->nParent_p = parents;
    this->parent_p  = new BasicObject* [this->nParent_p];
    for (i=0;i<this->nParent_p;i++) {
      this->parent_p[i] = NULL;
    }
  }
  
  // --- Destruction -------------------------------------------------------------
  
  template<class T> StoredAddObject<T>::~StoredAddObject ()
  {;}
  
  
  // --- Children & Parents ----------------------------------------------------
  
  template <>
  bool StoredAddObject<Double>::setParent(uInt num, BasicObject* obj) {
    if (obj->format(EF_isComplex) || !obj->format(EF_isNumeric)){
      throw(AipsError("setParent: Parent not of desired format!"));
      return false;
    }
    return putParent(num,obj);
  }
  
  template <>
  bool StoredAddObject<DComplex>::setParent(uInt num, BasicObject* obj) {
    if (!obj->format(EF_isNumeric)){
      throw(AipsError("setParent: Parent not of desired format!"));
      return false;
    }
    return putParent(num,obj);
  }
  
  // --- Parameters -------------------------------------------------------------
  
  template <>
  bool StoredAddObject<Double>::updateStorage(){
    Vector<Double> tmpvec;
    Double tmpval=0.;
    bool had_vec=false;
    uInt i;
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
	data_p = Vector<Double>(1,tmpval);
      };
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      return false;
    };
    
    cout << "Ran StoredAddObject<Double>::updateStorage() sucessfully" <<endl;
    
    valid_p = true;
    return true;
  }
  
  template <>
  bool StoredAddObject<DComplex>::updateStorage(){
    Vector<Double> Dvec;
    Vector<DComplex> Cvec,tmpCvec;
    DComplex tmpval=1.;
    bool had_vec=false;
    uInt i;
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
  
  // ==============================================================================
  //
  // Template Instantiation
  //
  // ==============================================================================
  
  template class StoredAddObject<casa::Double>;
  template class StoredAddObject<casa::DComplex>;
  
}  //  Namespace CR -- end
