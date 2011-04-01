/**************************************************************************
 *  Header file of Vector module for CR Pipeline Python bindings          *
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

#ifndef CR_PIPELINE_VECTOR_H
#define CR_PIPELINE_VECTOR_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

// ________________________________________________________________________
//                                        Vector to raw string for storrage

/*!
  \brief Write the vector content to a raw (binary) string.
*/
template <class T> HString hWriteRawVector(std::vector<T> &vec)
{
  size_t size = vec.size()*sizeof(T);
  char* buffer = new char[size];

  T* vec_p = &(vec[0]);

  memcpy(buffer, vec_p, size);

  HString str(buffer, size);

  return str;
};

/*!
  \brief Read the vector content from a raw (binary) string.
*/
template <class T> void hReadRawVector(std::vector<T> &vec, HString raw)
{
  size_t size = vec.size()*sizeof(T);

  if (size != raw.size())
  {
    throw PyCR::ValueError("Vector size does not match raw string.");
  }

  memcpy(&(vec[0]), raw.c_str(), size);
};

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mVector.def.h"

#endif /* CR_PIPELINE_VECTOR_H */
