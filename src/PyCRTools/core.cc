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



std::vector<HNumber> PyList2STLFloatVec(PyObject* pyob){
  std::vector<HNumber> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyFloat_AsDouble(PyList_GetItem(pyob,i)));
  }
  return vec;
}


std::vector<HInteger> PyList2STLIntVec(PyObject* pyob){
  std::vector<HInteger> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

std::vector<int> PyList2STLInt32Vec(PyObject* pyob){
  std::vector<int> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back((int)PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

std::vector<uint> PyList2STLuIntVec(PyObject* pyob){
  std::vector<uint> vec;
  if (PyList_Check(pyob)){
    HInteger i,size=PyList_Size(pyob);
    vec.reserve(size);
    for (i=0;i<size;++i) vec.push_back(PyInt_AsLong(PyList_GetItem(pyob,i)));
  }
  return vec;
}

// ========================================================================
//  Additional string operation functions
// ========================================================================

/*!
  \brief Convert a string to lower case.

  \param ioString -- In- and output string.

  \return Status -- Result of the opertation: true if successful, false otherwise.
*/

bool stringToLower(std::string& ioString)
{
  bool status = false;

  for (unsigned int i=0; i<ioString.length(); ++i) {
    ioString[i] = tolower(ioString[i]);
  }
  status = true;

  return status;
}


/*!
  \brief Convert a string to uppercase

  \param ioString -- In- and output string.

  \return Status -- result of the operation: true if successful, false otherwise.
*/
bool stringToUpper (std::string& ioString)
{
  bool status = false;

  for (unsigned int i=0; i<ioString.length(); ++i) {
    ioString[i] = toupper(ioString[i]);
  }

  return status;
}


/*!
  \brief Split a string with delimiter characters into multiple pieces.

  \param str -- Input strings.

  \return substrings -- Returns vector with substrings.

  The delimiter characters are space, comma and tab.
*/
std::vector<HString> stringSplit(const HString& str_const)
{
  HString delim(" ,\t"); // Delimiter characters.

  return stringSplit(str_const, delim);
}


/*!
  \brief Split a string with delimiter characters into multiple pieces.

  \param str -- Input strings.
  \param delim -- String containing delimiter characters.

  \return substrings -- Returns vector with substrings.
*/
std::vector<HString> stringSplit(const HString& str_const, const HString& delim)
{
  std::vector<HString> result;

  HString str(str_const);
  size_t pos = str.find_first_of(delim);

  while ( pos != str.npos ) {
    if ( pos > 0 ) {
      result.push_back(str.substr(0,pos));
    }
    str = str.substr(pos+1);
    pos = str.find_first_of(delim);
  }

  if (str.length() > 0) {
    result.push_back(str);
  }

  return result;
}

