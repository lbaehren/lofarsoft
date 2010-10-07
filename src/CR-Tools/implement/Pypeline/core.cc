/**************************************************************************
 *  Core implementation file for CR Pipeline Python bindings.             *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/


// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

//========================================================================
//$SECTION:           Administrative Vector Function
//========================================================================



vector<HNumber> PyList2STLFloatVec(PyObject* pyob){
  std::vector<HNumber> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyFloat_AsDouble(PyList_GetItem(pyob,i)));
  }
  return vec;
}


vector<HInteger> PyList2STLIntVec(PyObject* pyob){
  std::vector<HInteger> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

vector<int> PyList2STLInt32Vec(PyObject* pyob){
  std::vector<int> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back((int)PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

vector<uint> PyList2STLuIntVec(PyObject* pyob){
  std::vector<uint> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

