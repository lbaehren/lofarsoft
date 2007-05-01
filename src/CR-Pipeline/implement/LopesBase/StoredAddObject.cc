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

/* $Id: StoredAddObject.cc,v 1.4 2007/04/17 11:56:39 bahren Exp $*/

#include <LopesBase/StoredAddObject.h>

// --- Construction ------------------------------------------------------------

template<class T> StoredAddObject<T>::StoredAddObject (uInt parents)
  : StoredObject<T>()
{
  this->className_p = "StoredAddObject";

  if (parents <= 1) {
    throw(AipsError("StoredAddObject: Need at least two parents"));
  };

  this->nParent_p = parents;
  this->parent_p  = new BasicObject* [parents];
  for (uInt i=0; i<parents; i++) {
    this->parent_p[i] = NULL;
  }
}

// --- Destruction -------------------------------------------------------------

template<class T> StoredAddObject<T>::~StoredAddObject ()
{;}


// --- Children & Parents ----------------------------------------------------

template <>
Bool StoredAddObject<Double>::setParent(uInt num, BasicObject* obj) {
  if (obj->format(EF_isComplex) || !obj->format(EF_isNumeric)){
    throw(AipsError("setParent: Parent not of desired format!"));
    return False;
  }
  return putParent(num,obj);
}

template <>
Bool StoredAddObject<DComplex>::setParent(uInt num, BasicObject* obj) {
  if (!obj->format(EF_isNumeric)){
    throw(AipsError("setParent: Parent not of desired format!"));
    return False;
  }
  return putParent(num,obj);
}

// --- Parameters -------------------------------------------------------------

template <>
Bool StoredAddObject<Double>::updateStorage(){
  Vector<Double> tmpvec;
  Double tmpval=0.;
  Bool had_vec=False;
  uInt i;
  try{
    for (i=0;i<nParent_p;i++) {
      if (!parent_p[i]->format(EF_isScalar)){
	if (!had_vec) {
	  had_vec=True;
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
    return False;
  };
  
  cout << "Ran StoredAddObject<Double>::updateStorage() sucessfully" <<endl;
  
  valid_p = True;
  return True;
}

template <>
Bool StoredAddObject<DComplex>::updateStorage(){
  Vector<Double> Dvec;
  Vector<DComplex> Cvec,tmpCvec;
  DComplex tmpval=1.;
  Bool had_vec=False;
  uInt i;
  try{
    for (i=0;i<nParent_p;i++) {
      if (!parent_p[i]->format(EF_isScalar)){
	if (!had_vec) {
	  had_vec=True;
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
    return False;
  };
  
  cout << "Ran StoredAddObject<DComplex>::updateStorage() sucessfully" <<endl;
  
  valid_p = True;
  return True;
}


// ==============================================================================
//
// Template Instantiation
//
// ==============================================================================

template class StoredAddObject<casa::Double>;
template class StoredAddObject<casa::DComplex>;
