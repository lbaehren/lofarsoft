/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
 *  Heino Falcke <h.falcke@astro.ru.nl>                                   *
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
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mIO.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mVector.h"
#include "mIO.h"
#include "mMath.h"

#ifdef PYCRTOOLS_WITH_AERA
#include "aera/Datareader.h"
#include "aera/Data/Header.h"
#include "aera/Data/Event.h"
#include "aera/Data/EventHeader.h"
#include "aera/Data/LocalStation.h"
#endif /* PYCRTOOLS_WITH_AERA */

// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: IO functions
// ========================================================================

//-----------------------------------------------------------------------
//$DOCSTRING: Dump a single vector to a file in binary format (machine dependent).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteFileBinary
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write file to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.writefilebinary(filename) -> dumps vector to file

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend

  Example:
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinary(v,\"test.dat\")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinary(x,\"test.dat\")
  >>> x
  Vector(float, 10, [3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0])
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, HString filename) {
  if (vec_end<=vec) return;
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  ofstream outfile(filename.c_str(), ios::out | ios::binary | ios::trunc);
  if (outfile){
    outfile.write(v1, (HInteger)(v2-v1));
    outfile.close();
  } else throw PyCR::IOError("hWriteFileBinary: Unable to open file "+filename+".");
}

template <>
void HFPP_FUNC_NAME(const std::_Bit_iterator vec, const std::_Bit_iterator vec_end, HString filename) {
  // Sanity check
  if (vec_end<=vec) return;

  // Implementation
  fstream outfile(filename.c_str(), ios::out | ios::binary | ios::trunc);
  if (outfile) {
    std::_Bit_iterator it = vec;
    std::string raw = "";
    while (it != vec_end) {
      if (*it)
        raw += "1";
      else
        raw += "0";
      ++it;
    }
    outfile << raw;
    outfile.close();
  } else {
    throw PyCR::IOError("hWriteFileBinary: Unable to open file " + filename + ".");
  }
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Dump a single vector and append it to the end of a file in binary format (machine dependent).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteFileBinaryAppend
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write file to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.writefilebinaryappend(filename,true) -> appends binary vector data to end of file

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend

  Example:
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinary(v,\"test.dat\")
  >>> v.fill(2.0)
  >>> hWriteFileBinaryAppend(v,\"test.dat\")
  >>> x=Vector(float,20,fill=1.0)
  >>> hReadFileBinary(x,\"test.dat\")
  >>> x
  Vec(float,20)=[3.0,3.0,3.0,3.0,3.0,...,2.0,2.0]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, HString filename) {
  if (vec_end<=vec) return;
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::out | ios::binary | ios::app);
  if (outfile){
    outfile.write(v1, (HInteger)(v2-v1));
    outfile.close();
  } else throw PyCR::IOError("hWriteFileBinaryAppend: Unable to open file "+filename+".");
}

template <>
void HFPP_FUNC_NAME(const std::_Bit_iterator vec, const std::_Bit_iterator vec_end, HString filename) {
  // Sanity check
  if (vec_end<=vec) return;

  // Implementation
  fstream outfile(filename.c_str(), ios::out | ios::binary | ios::app);
  if (outfile) {
    std::_Bit_iterator it = vec;
    std::string raw = "";
    while (it != vec_end) {
      if (*it)
        raw += "1";
      else
        raw += "0";
      ++it;
    }
    outfile << raw;
    outfile.close();
  } else {
    throw PyCR::IOError("hWriteFileBinaryAppend: Unable to open file " + filename + ".");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Dump a single vector to a file in binary format (machine dependent).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteFileBinary
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write file to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(start)()("Start writing at element 'start' (zero-based index), the data block length to write is determined by the vector length.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Attention: if a file exists already and a start value is specified,
  then it will not be overwritten, but the data will be written into
  the location asked by start. This can be dangerous if one writes a
  smaller file (e.g. with ``start=0``) to a pre-existing older and larger
  file because then the 'new' file will contain the rest of the old
  file.

  Usage:
  vec.writedump(filename,start) -> writes binary vector data at location start in the file

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend

  Example:
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinary(v,\"test.dat\",0)
  >>> v.fill(2.0)
  >>> hWriteFileBinary(v,\"test.dat\",5)
  >>> x=Vector(float,15,fill=1.0)
  >>> hReadFileBinary(x,\"test.dat\")
  >>> x
  Vector(float, 15, [3.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0])
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, HString filename, HInteger start) {
  if (vec_end<=vec) return;
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*(vec+1)));
  char * vend = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile;
  ifstream f(filename.c_str(), ios::binary | ios::in);
  if (f) {//file exists
    f.close();
    outfile.open(filename.c_str(), ios::binary | ios::out | ios::in);
  } else {//file exists so, needs to be created (and cannot be opened as r/w)
    f.close();
    outfile.open(filename.c_str(), ios::binary | ios::out);
  };
  if (outfile){
    if (start>0) outfile.seekp(((HInteger) (v2-v1))*start);
    outfile.write(v1, (HInteger)(vend-v1));
    outfile.close();
  } else throw PyCR::IOError("hWriteFileBinary: Unable to open file "+filename+".");
}

template <>
void HFPP_FUNC_NAME(const std::_Bit_iterator vec, const std::_Bit_iterator vec_end, HString filename, HInteger start) {
  // Sanity check
  if (vec_end <= vec) return;

  // Generating raw string
  std::_Bit_iterator it = vec;
  std::string raw = "";
  while (it != vec_end) {
    if (*it)
      raw += "1";
    else
      raw += "0";
    ++it;
  }

  // Writing to file
  fstream outfile;
  ifstream infile(filename.c_str(), ios::binary | ios::in);
  int vec_len = std::distance(vec, vec_end);
  int str_len = vec_len;
  if (infile) {
    infile.close();
    outfile.open(filename.c_str(), ios::binary | ios::out | ios::in);
  } else {
    infile.close();
    outfile.open(filename.c_str(), ios::binary | ios::out);
  }
  if (outfile) {
    if (start > 0) outfile.seekg(start * vec_len);
    outfile.write(raw.c_str(), str_len);
    outfile.close();
  } else {
    throw PyCR::IOError("hWriteFileBinary: Unable to open file " + filename + ".");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Read a section (block) of a single vector from a file which was dumped in (machine-dependent) binary format.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadFileBinary
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to read data from. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(start)()("Start reading at element 'start' (zero-based index), the data block length to read is determined by the vector length.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.readdump(filename,start) -> reads a specfic block from file starting at element start (blocklength determined by ``len(vec)``).

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend, hWriteFileText, hReadFileText

  Example:
  >>> x=hArray(range(20))
  >>> x.writefilebinary(\"test.dat\")
  >>> v=hArray(int,10)
  >>> v.readfilebinary(\"test.dat\",0)
  >>> v.pprint()
  [0,1,2,3,4,5,6,7,8,9]

  #Now read the third block (of length 3) into the first part of the vector
  >>> v[0:3].readfilebinary(\"test.dat\",2*3)
  >>> v.pprint()
  [6,7,8,3,4,5,6,7,8,9]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename, HInteger start) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*(vec+1)));
  char * vend = reinterpret_cast<char*>(&(*vec_end));
  ifstream outfile(filename.c_str(), ios::in | ios::binary);
  if (outfile){
    if (start>0) outfile.seekg(((HInteger) (v2-v1))*start);
    outfile.read(v1, (HInteger)(vend-v1));
    outfile.close();
  } else throw PyCR::IOError("hReadFileBinary: Unable to open file "+filename+".");
}

template <>
void HFPP_FUNC_NAME(const std::_Bit_iterator vec, const std::_Bit_iterator vec_end, HString filename, HInteger start) {
  int vec_len = std::distance(vec, vec_end);
  int str_len = vec_len;
  char raw[str_len];

  fstream outfile(filename.c_str(), ios::in | ios::binary);
  if (outfile) {
    if (start > 0) {
      outfile.seekg(vec_len * start);
    }
    outfile.read(raw, vec_len);
    outfile.close();

    std::_Bit_iterator it = vec;
    int i = 0;
    while ((it != vec_end) && (i < str_len)) {
      if (raw[i] == '1') {
        *it = true;
      } else {
        *it = false;
      }
      ++i;
      ++it;
    }
  } else {
    throw PyCR::IOError("hWriteFileBinary: Unable to open file " + filename + ".");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Read a section (block) of a single vector from a file which was dumped in (machine-dependent) binary format with offsets.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hOffsetReadFileBinary
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to read data from. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(start)()("Start reading at element 'start' (zero-based index), the data block length to read is determined by the vector length.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> v=hArray(range(100))
  >>> hWriteFileBinary(v, \"test.dat\")
  >>> n=hArray(int, 10)
  >>> i=hArray(range(0,20,2))
  >>> n
  hArray(int, [10L], fill=[0,0,0,0,0,0,0,0,0,0]) # len=10 slice=[0:10])
  >>> hOffsetReadFileBinary(n, \"test.dat\", i)
  >>> n
  hArray(int, [10L], fill=[0,2,4,6,8,10,12,14,16,18]) # len=10 slice=[0:10])

*/
template <class Iter, class IIter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, HString filename, const IIter start, const IIter start_end)
{

  HInteger N = std::distance(vec, vec_end);

  if (N != std::distance(start, start_end))
  {
    throw PyCR::IOError("hOffsetReadFileBinary: Offset array has wrong size.");
  }

  IIter si = start;

  IterValueType temp = 0;

  ifstream outfile(filename.c_str(), ios::in | ios::binary);

  if (outfile)
  {

    for (int i=0; i<N; i++)
    {
      outfile.seekg(sizeof(IterValueType) * *si++);

      outfile.read((char*) &temp, sizeof(IterValueType));

      *(vec+i) = temp;
    }

  } else throw PyCR::IOError("hOffsetReadFileBinary: Unable to open file "+filename+".");
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Read a single vector from a file which was dumped in (machine-dependent) binary format
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadFileBinary
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to read data from. The vector needs to have the length corresponding to the file size.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.readdump(filename) -> reads dumped vector from file

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend, hWriteFileText, hReadFileText

  Example:
  >>> v=Vector(float,10,fill=3.0)
  >>> hWriteFileBinary(v,\"test.dat\")
  >>> x=Vector(float,10,fill=1.0)
  >>> hReadFileBinary(x,\"test.dat\")
  >>> x
  Vec(float,10)=[3.0,3.0,3.0,3.0,3.0,...]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end, HString filename) {
  char * v1 = reinterpret_cast<char*>(&(*vec));
  char * v2 = reinterpret_cast<char*>(&(*vec_end));
  fstream outfile(filename.c_str(), ios::in | ios::binary);
  if (outfile){
    outfile.read(v1, (HInteger)(v2-v1));
    outfile.close();
  } else throw PyCR::IOError("hReadFileBinary: Unable to open file "+filename+".");
}

template <>
void HFPP_FUNC_NAME(const std::_Bit_iterator vec, const std::_Bit_iterator vec_end, HString filename) {
  hReadFileBinary(vec, vec_end, filename, 0);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Read columns of data from a file in text (ASCII) form into an array
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadFileText
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to read data from.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(skiprows)()("The number of rows to skip before reading the data.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(columns)()("An array with a list of column numbers (starting at zero) from which to read data into the array. Read all if empty.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(nrows)()("The number of rows to read. If equal to -1 read all.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.readtexttable(filename,skiprows,columns,nrows) -> reads a number of columns into the vector vec

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend, hWriteFileText, hReadFileText

  Example:
  >>> from pycrtools import *
  >>> vec=Vector(float,24*3)
  >>> hReadFileText(vec, \"AERAcoordinates-1.txt\", 4, Vector([5,3,7]),10)

  # This will skip the first 4 rows and then read 10 rows where columns
  # 5,3,&7 will be read into the vector vec. The ordering will be [l0.c5,
  # l0.c3, l0.c7, l1.c5, l1.c3, l1.c7, ..., l9.c7] (where l and c stand
  # for row and column respectively)
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(const Iter vec,   const Iter vec_end,
                    const HString filename,
                    const HInteger skiprows,
                    const IterI columns, const IterI columns_end,
                    const HInteger nrows)
{
  ifstream infile(filename.c_str(), ios::in);
  HInteger nskip(skiprows);
  HInteger rowsread(0);
  HString row;
  IterI col_it;
  Iter vec_it(vec);
  HInteger col_max = 0;

  typedef IterValueType T;
  vector<HString> words;

  // Check column numbers
  col_it = columns;
  while (col_it != columns_end) {
    if (*col_it < 0) {
      throw PyCR::IndexError("Negative index numbers not allowed");
    }
    if (*col_it > col_max) {
      col_max = *col_it;
    }
    ++col_it;
  }

  if (infile){
    while(getline(infile,row) && (!infile.eof()) && (rowsread != nrows) && (vec_it < vec_end)) {
      if (nskip>0) {
	--nskip;
      } else {
	++rowsread;
	words = stringSplit(row); //Split the row into words separated by whitespaces
        if (col_max < (HInteger)words.size()) {
          col_it = columns;
          while (col_it != columns_end) { //loop over all columns listed in the columns array
            if ( vec_it < vec_end) {
              *vec_it = hfcast<T>(words[*col_it]);
               ++vec_it;
            }
            ++col_it;
          }
         } else {
          // warning: column index out of range for this line: line is skipped
          cout << "Warning: requested field not found in row " << rowsread << ": row skipped" << endl;
        }
      }
    }
    infile.close();
  } else {
    throw PyCR::IOError("hReadFileText: Unable to open file "+filename+".");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Write columns of data in an array to a (ASCII) text file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWriteFileText
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output data vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(filename)()("Filename (including path if necessary) to write data to.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HString)(header)()("The header to preceed the table, e.g. a description of the columns.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(ncolumns)()("The number of columns in the table.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(nrows)()("The number of rows to write. If equal to -1 write all.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HBool)(append)()("If true append data to the file, rather than overwrite it.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.writetexttable(filename,header,ncolumns,nrows) -> write vec as table into filename

  See also:
  hReadFileBinary, hWriteFileBinary, hWriteFileBinaryAppend, hWriteFileText, hReadFileText

  Example:
  >>> vec=Vector(float,24*3,fill=range(24*3))
  >>> vec.writetexttable(\"table.dat\",\"# col1, col2, col3\", 3, 24,False)
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
                    const HString filename,
                    const HString header,
                    const HInteger ncolumns,
                    const HInteger nrows,
                    const HBool append)
{
  Iter vec_it(vec);
  HInteger row(0);
  HInteger col;
  ofstream outfile;

  // Checks on input parameters
  if (ncolumns <= 0) {
    throw PyCR::ValueError("Number of columns <= 0.");
  }
  if ((std::distance(vec,vec_end) % ncolumns) != 0) {
    throw PyCR::ValueError("Vector size is not a multiple of the number of columns.");
  }

  if (append) {
    outfile.open(filename.c_str(), ios::out | ios::app);
  } else {
    outfile.open(filename.c_str(), ios::out);
  }

  if (outfile) {
    if (!append) { // Add a header for new files.
      outfile << header << endl;
    }
    while((row != nrows) && (vec_it != vec_end)) {
      col=ncolumns;
      while (col != 0) { // Loop over all columns
	if (vec_it != vec_end) {
	  outfile << *vec_it << " ";
	  ++vec_it; --col;
	}
      }
      ++row;
      outfile << endl;
    }
    outfile.close();
  } else {
    throw PyCR::IOError("hWriteFileText: Unable to open file "+filename+".");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



// ========================================================================
//
//  AERA
//
// ========================================================================

#ifdef PYCRTOOLS_WITH_AERA

//$DOCSTRING: Function to open a file based on a filename and data format version and returning an AERA datareader object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileOpen
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (AERADatareader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to open including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HInteger)(version)()("Version of the data format of file to open")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:

  datapath = cr.LOFARSOFT + "/data/aera/"
  datafile = "ad000538.f0001"
  filename = datapath + datafile
  version = 1
  aerafile = cr.hAERAFileOpen(filename, version)
*/
AERA::Datareader& HFPP_FUNC_NAME (const HString Filename, const HInteger version)
{
  AERA::Datareader* dr_ptr = NULL;

  dr_ptr = new AERA::Datareader(version);

  if (dr_ptr != NULL) {
    dr_ptr->open(Filename);
    dr_ptr->read();
  }

  return *dr_ptr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Function to open a file based on a filename and returning an AERA datareader object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileOpen
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (AERADatareader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to open including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:

  datapath = cr.LOFARSOFT + "/data/aera/"
  datafile = "ad000413.f0001"
  filename = datapath + datafile
  aerafile = cr.hAERAFileOpen(filename)
*/
AERA::Datareader& HFPP_FUNC_NAME (const HString Filename)
{
  AERA::Datareader* dr_ptr = NULL;

  dr_ptr = new AERA::Datareader();

  if (dr_ptr != NULL) {
    dr_ptr->open(Filename);
    dr_ptr->read();
  }

  return *dr_ptr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Function to close an AERA data file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileClose
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:

  datapath = cr.LOFARSOFT + "/data/aera/"
  datafile = "ad000413.f0001"
  filename = datapath + datafile
  aerafile = cr.hAERAFileOpen(filename)

  cr.hAERAFileClose(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;

  status = dr.close();

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Get attribute of an AERA data file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetAttribute
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HPyObject)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(keyName)()("Key name of parameter")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The following keywords can be used to obtain file information:

    ===================  ========================
    **File keyname**     **Description**
    filename             File name
    nevents              Number of events
    runnumber            Run number
    runmode              Run mode
    fileserialnr         File serial number
    firsteventnr         First event number
    firsteventtime       Time of first event
    lasteventnr          Last event number
    lasteventtime        Time of last event
    ===================  ========================

  The following keywords can be used to obtain info about the *current* event:

    =================== ================================
    **Event keyname**   **Description**
    nlocalstations      Number of local stations
    eventlength         Event length
    eventid             Event ID
    t3                  T3 event ID
    timestampsec        Timestamp in seconds
    timestampnanosec    Nanosecond part of timestamp
    eventtype           Type of event
    =================== ================================

  The following keywords can be used to obtain info about the *current* localstation:

    ========================= ===============================
    **Localstation keyname**  **Description**
    bodylength                Length of the event body
    eventnumber               Event number
    localstationid            Local station ID
    hardware                  Hardware description
    headerlength              Header length
    seconds                   Seconds
    nanoseconds               Nanoseconds
    triggerflag               Trigger flag
    triggerposition           Trigger position
    samplingfreq              Sampling frequency
    channelmask               Channel mask
    adcresolution             ADC resolution
    tracelength               Trace length
    version                   Data version
    messageid                 Message ID
    ========================= ===============================
*/
HPyObject HFPP_FUNC_NAME (AERA::Datareader& dr, const std::string keyValue)
{
  std::string keyName = keyValue;
  HPyObject pyObj = HPyObject();
  AERA::Data::Header* header_ptr = dr.header();
  stringToLower(keyName);

  // ___________________________________________________________________
  //                                                                File

  if ( "filename" == keyName ) { //                            Filename
    std::string result = dr.getFileName();
    return HPyObject(result);
  } else if ( "nevents" == keyName ) { //              Number of events
    int result = dr.nEvents();
    return HPyObject(result);
  } else if ( NULL != header_ptr ) {
    if ( "runnumber" == keyName ) { //                       Run number
      int result = header_ptr->getRunNumber();
      return HPyObject(result);
    } else if ( "runmode" == keyName ) { //                    Run mode
      int result = header_ptr->getRunMode();
      return HPyObject(result);
    } else if ( "fileserialnr" == keyName ) { //         File serial nr
      int result = header_ptr->getFileSerialNumber();
      return HPyObject(result);
    } else if ( "firsteventnr" == keyName ) { //     First event number
      int result = header_ptr->getFirstEvent();
      return HPyObject(result);
    } else if ( "firsteventtime" == keyName ) { //     Time first event
      std::string result = header_ptr->getFirstEventTime();
      return HPyObject(result);
    } else if ( "lasteventnr" == keyName ) {  //      Last event number
      int result = header_ptr->getLastEvent();
      return HPyObject(result);
    } else if ( "lasteventtime" == keyName ) {  //      Time last event
      std::string result = header_ptr->getLastEventTime();
      return HPyObject(result);
    }
  }

  // ___________________________________________________________________
  //                                                               Event

  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::EventHeader* eventhdr_ptr = NULL;

  if ( NULL != event_ptr ) {
    if ( "nlocalstations" == keyName ) { //    Number of local stations
      AERA::Data::Event* event_ptr = dr.currentEvent();
      int result = event_ptr->nLocalStations();
      return HPyObject(result);
    }
    eventhdr_ptr = event_ptr->eventHeader();
    if ( NULL != eventhdr_ptr ) {
      if ( "eventlength" == keyName ) { //                 Event length
        int result = eventhdr_ptr->getEventLength();
        return HPyObject(result);
      } else if ( "eventid" == keyName ) { //                  Event ID
        int result = eventhdr_ptr->getEventID();
        return HPyObject(result);
      } else if ( "t3" == keyName ) { //                    T3 event ID
        int result = eventhdr_ptr->getT3eventID();
        return HPyObject(result);
      } else if ( "timestampsec" == keyName ) { //        Timestamp (s)
        unsigned int result = eventhdr_ptr->getSeconds();
        return HPyObject(result);
      } else if ( "timestampnanosec" == keyName ) { //   Timestamp (ns)
        unsigned int result = eventhdr_ptr->getNanoSeconds();
        return HPyObject(result);
      } else if ( "eventtype" == keyName ) {//        Type of the event
        int result = eventhdr_ptr->getEventType();
        return HPyObject(result);
      }
    }
  } else {
    throw PyCR::ValueError("Event pointer is NULL.");
    return pyObj;
  }

  // ___________________________________________________________________
  //                                                       Local station

  if ( NULL != event_ptr ) {
    AERA::Data::LocalStation* ls_ptr = event_ptr->currentLocalStation();
    if ( NULL != ls_ptr ) {
      if ( "bodylength" == keyName ) { // Length of event body
        UINT16 result = ls_ptr->getLength();
        return HPyObject(result);
      } else if ( "eventnumber" == keyName ) { // Event number
        UINT16 result = ls_ptr->getEventNumber();
        return HPyObject(result);
      } else if ( "localstationid" == keyName ) { // Local station ID
        UINT16 result = ls_ptr->getLocalStationID();
        return HPyObject(result);
      } else if ( "hardware" == keyName ) { // Hardware description
        std::string result = ls_ptr->getHardware();
        return HPyObject(result);
      } else if ( "headerlength" == keyName ) { // Header length
        UINT16 result = ls_ptr->getHeaderLength();
        return HPyObject(result);
      } else if ( "seconds" == keyName ) {     // Seconds
        unsigned int result = ls_ptr->getSeconds();
        return HPyObject(result);
      } else if ( "nanoseconds" == keyName ) { // Nanoseconds
        unsigned int result = ls_ptr->getNanoSeconds();
        return HPyObject(result);
      } else if ( "triggerflag" == keyName ) { // Trigger flag
        UINT16 result = ls_ptr->getTriggerFlag();
        return HPyObject(result);
      } else if ( "triggerposition" == keyName ) { // Trigger position
        UINT16 result = ls_ptr->getTriggerPosition();
        return HPyObject(result);
      } else if ( "samplingfreq" == keyName ) { // Sampling frequency
        UINT16 result = ls_ptr->getSamplingFrequency();
        return HPyObject(result);
      } else if ( "channelmask" == keyName ) { // Channel mask
        UINT16 result = ls_ptr->getChannelMask();
        return HPyObject(result);
      } else if ( "adcresolution" == keyName ) { // ADC resolution
        UINT16 result = ls_ptr->getADCResolution();
        return HPyObject(result);
      } else if ( "tracelength" == keyName ) { // Trace length
        UINT16 result = ls_ptr->getTraceLength();
        return HPyObject(result);
      } else if ( "version" == keyName ) { // Version
        UINT16 result = ls_ptr->getVersion();
        return HPyObject(result);
      } else if ( "messageid" == keyName ) { // Message ID
        unsigned short result = ls_ptr->getMessageID();
        return HPyObject(result);
      }
    } else {
      throw PyCR::ValueError("LocalStation pointer is NULL.");
      return pyObj;
    }
  }

  // ___________________________________________________________________
  //                                                                Test

  if ( "tvectori" == keyName ) { //                  Test vector integer
    std::vector<HInteger> result(1024,hfnull<HInteger>());
    return HPyObject(result);
  } else   if ( "tvectorf" == keyName ) { //          Test vector number
    std::vector<HNumber> result(1024,hfnull<HNumber>());
    return HPyObject(result);
  } else   if ( "tvectorc" == keyName ) { //         Test vector complex
    std::vector<HComplex> result(1024,hfnull<HComplex>());
    return HPyObject(result);
  }

  // ___________________________________________________________________
  //                                                          Other keys

  throw PyCR::KeyError("Key is not defined.");

  if ( "default" == keyName ) { //                              Default
    cout << "Using the default value (0)" << endl;
    pyObj = HPyObject(hfnull<HInteger>());
  }

  return pyObj;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Time series data for alle channels of current localstation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetADCData
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (AERA::Datareader& dr,
                     const Iter vec, const Iter vec_end)
{
  Iter v_begin = vec;
  Iter v_end   = vec_end;
  int tracelength = 0;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* station_ptr = NULL;

  if ( NULL != event_ptr ) {
    station_ptr = event_ptr->currentLocalStation();
    if ( NULL != station_ptr ) {
      tracelength = station_ptr->getTraceLength();
      for (int channel = 0; channel < station_ptr->nChannels(); ++channel) {
        v_begin = vec + tracelength*channel;
        v_end = v_begin + tracelength;
        if ( (v_begin < vec_end) && (v_end <= vec_end) ) {
          hAERAGetADCData(dr, v_begin, v_end, channel);
        }
      }
    } else {
      cerr << "ERROR: [hAERAGetADCData] LocalStation pointer is NULL." << endl;
    }
  } else {
    cerr << "ERROR: [hAERAGetADCData] Event pointer is NULL." << endl;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Time series data for specified channel of the current localstation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetADCData
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(Channel)()("Channel number to read out")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (AERA::Datareader& dr,
                     const Iter vec, const Iter vec_end,
                     const HInteger channel)
{
  vector<int> ADCChannelData;
  vector<int>::iterator ADC_it;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* station_ptr = NULL;
  Iter vec_it = vec;

  if ( ( 0 <= channel ) && ( station_ptr->nChannels() > channel ) ) {
    if ( NULL != event_ptr ) {
      station_ptr = event_ptr->currentLocalStation();
      if ( NULL != station_ptr ) {
        ADCChannelData = station_ptr->getADCData((unsigned short) channel);
        ADC_it = ADCChannelData.begin();
        while ( ( ADC_it != ADCChannelData.end() ) &&
                ( vec_it != vec_end ) ) {
          *vec_it = (IterValueType)(*ADC_it);
          ++ADC_it; ++vec_it;
        }
      } else {
        cerr << "ERROR [hAERAGetADCData]: Local station pointer is NULL." << endl;
      }
    } else {
      cerr << "ERROR [hAERAGetADCData]: Event pointer is NULL." << endl;
    }
  } else {
    throw PyCR::ValueError("Invalid value of channel number.");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Set the event pointer to the first event of the file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFirstEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the event pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERAFirstEvent(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  event_ptr = dr.firstEvent();
  if ( NULL != event_ptr ) {
    status = hAERAFirstLocalStation(dr);
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the previous event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAPrevEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Return ``True`` if the event pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERAPrevEvent(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  if ( ! dr.isFirstEvent() ) {
    event_ptr = dr.prevEvent();

    if ( event_ptr != NULL ) {
      status = true;
    }
  }
  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the next event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERANextEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the event pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERANextEvent(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  if ( ! dr.isLastEvent() ) {
    event_ptr = dr.nextEvent();

    if ( event_ptr != NULL ) {
      status = true;
    }
  }
  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the last event of this file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALastEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the event pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERALastEvent(aerafile)
*/

HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  event_ptr = dr.lastEvent();
  if ( NULL != event_ptr ) {
    status = hAERAFirstLocalStation(dr);
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the Local Station pointer to the first Local station of this event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFirstLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
 \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the local station pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERAFirstLocalStation(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->firstLocalStation();

    if ( NULL != localstation_ptr ) {
      status = true;
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the previous local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAPrevLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Return ``True`` if the local station pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERAPrevLocalStation(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    if ( ! event_ptr->isFirstLocalStation() ) {
      localstation_ptr = event_ptr->prevLocalStation();

      if ( NULL != localstation_ptr ) {
        status = true;
      }
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the next local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERANextLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the local station pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERANextLocalStation(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL !=  event_ptr ) {
    if ( ! event_ptr->isLastLocalStation() ) {
      localstation_ptr = event_ptr->nextLocalStation();

      if ( NULL != localstation_ptr ) {
        status = true;
      }
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the last local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALastLocalStation
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns ``True`` if the local station pointer was successfully changed, ``False`` otherwise.

  Example:
  status = cr.hAERALastLocalStation(aerafile)
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->lastLocalStation();

    if ( NULL != localstation_ptr ) {
      status = true;
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Print a brief summary of the AERA data file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  cr.hAERAFileSummary(aerafile)
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  dr.summary();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Print a brief summary of the AERA event contents.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAEventSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  cr.hAERAEventSummary(aerafile)
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  AERA::Data::Event* event_ptr = dr.currentEvent();

  if ( NULL != event_ptr ) {
    event_ptr->summary();
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Print a brief summary of the AERA local station contents.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALocalStationSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  cr.hAERALocalStationSummary(aerafile)
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->currentLocalStation();

    if ( NULL != localstation_ptr ) {
      localstation_ptr->summary();
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#endif /* PYCRTOOLS_WITH_AERA */

