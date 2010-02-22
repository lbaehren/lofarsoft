/*-------------------------------------------------------------------------*
 | $Id:: hftools.cc 3995 2010-01-18 10:09:09Z falcke                       $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Heino Falcke (www.astro.ru.nl/~falcke)                                *
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

/*!
  HFTOOLS Version 0.1 - Tools to manipulate vectors and generate
  wrappers for python
*/
#include <memory>

using namespace std;


#undef HFPP_VERBOSE
#include "hftools.h"

//#include "htest.cc"


//Some definitions needed for the preprosessor programming:
//Copy source code (where specified with COPY_TO HFILE START) to the file hwrappers-hftools.iter.cc.h
//-----------------------------------------------------------------------
//$FILENAME: HFILE=hftools.def.h
//-----------------------------------------------------------------------

// Tell the preprocessor (for generating wrappers) that this is a c++
// source code file (brackets are crucial)


#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE CC
//-----------------------


//========================================================================
//                    Casting & Conversion Functions
//========================================================================
template<class T> T hfnull(){};
template<> inline HString  hfnull<HString>(){HString null=""; return null;}
template<> inline HPointer hfnull<HPointer>(){return NULL;}
template<> inline HInteger hfnull<HInteger>(){return 0;}
template<> inline HNumber  hfnull<HNumber>(){return 0.0;}
template<> inline HComplex hfnull<HComplex>(){return 0.0;}

//Identity
template<class T> inline T hfcast(/*const*/ T v){return v;}

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v+0.5));}
template<>  inline HInteger hfcast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)+0.5));}
template<>  inline HNumber hfcast<HNumber>(HComplex v){return real(v);}


inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}


inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}

template <class T>
CasaVector<T> & stl2casa(std::vector<T> stlvec)
{
  T * storage = &(stlvec[0]);
  casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
  CasaVector<T> casavec(shape,storage,casa::SHARE);
  return casavec;
}


//========================================================================
//                        Matrix Class
//========================================================================

// Testing a simple Matrix class that is built upon std::vectors
// we do actually not really want to do this ....
namespace std {

  template <class T, class Alloc>
  class hmatrix : public vector<T,Alloc> {

  public:
    template <class Iter> hmatrix(Iter it1, Iter it2);
    hmatrix(const char* buf);
    hmatrix();
    ~hmatrix();

    void setNumberOfDimensions(HInteger newdim);
    HInteger getNumberOfDimensions();

    void setDimensionN(HInteger n, HInteger size);

    void setDimensions(HInteger dim0);
    void setDimensions(HInteger dim0, HInteger dim1);
    void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3);


  private:
    HInteger ndim;
    vector<HInteger>* dimensions_p;
  };


  template <class T, class Alloc> template <class Iter> hmatrix<T,Alloc>::hmatrix(Iter it1, Iter it2) : vector<T,Alloc>(it1,it2) {};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(const char* buf) : vector<T,Alloc>(buf){};
  template <class T, class Alloc> hmatrix<T,Alloc>::hmatrix(){
    ndim=0;
    dimensions_p=NULL;
    //    setDimension(2);
  };
  template <class T, class Alloc> hmatrix<T,Alloc>::~hmatrix(){};

  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setNumberOfDimensions(HInteger newdim){
    if (newdim==ndim) return;
    if (dimensions_p != NULL) delete dimensions_p;
    ndim=newdim;
    dimensions_p = new vector<HString>(ndim);
  };

  template <class T, class Alloc>
  HInteger hmatrix<T,Alloc>::getNumberOfDimensions(){return ndim;};

  template <class T, class Alloc>
  void hmatrix<T,Alloc>::setDimensionN(HInteger n, HInteger size){
  }

  template <class T, class Alloc>
  void setDimensions(HInteger dim0){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1){}
  template <class T, class Alloc>
  void setDimensions(HInteger dim0, HInteger dim1, HInteger dim3){}
};

//========================================================================
//                        Helper Functions
//========================================================================


/*!
\brief Determine the filetype based on the extension of the filename
 */
HString hgetFiletype(HString filename){
  HString ext=hgetFileExtension(filename);
  HString typ="";
  if (ext=="event") typ="LOPESEvent";
  else if (ext=="h5") typ="LOFAR_TBB";
  return typ;
}

/*!
\brief Get the extension of the filename, i.e. the text after the last dot.
 */
HString hgetFileExtension(HString filename){
  HInteger size=filename.size();
  HInteger pos=filename.rfind('.',size);
  pos++;
  if (pos>=size) {return "";}
  else {return filename.substr(pos,size-pos);};
}


//========================================================================
//                    Administrative Vector Functions
//========================================================================


//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hFill)("Fills a vector with a constant value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief Fills a vector with a constant value.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param fill_value: Value to fill vector with
*/
template <class Iter>
void hFill(const Iter vec,const Iter vec_end, const IterValueType fill_value)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=fill_value;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Make and return a new vector of the same size and type as the input vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNew
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
std::vector<T> HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> vnew(vec.size());
  return vnew;
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize a vector to a new length.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size of vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

 */
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize a vector to a new length and will new elements in vector with a specific value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(fill)()("Value to fill new vector elements with")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class T>
void hResize(std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize an STL vector to the same length as a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class T, class S>
void hResize(std::vector<T> & vec1,std::vector<S> & vec2)
{
  vec1.resize(vec2.size());
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Resize a casa vector to the same length as a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class T, class S>
void hResize(casa::Vector<T> & vec1,casa::Vector<S> & vec2)
{
  vec1.resize(*(vec2.shape().begin()));
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvert
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_CODE_PRE hResize(vec2,vec1); //code will be inserted in wrapper generation in cc file
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec1,const Iterin vec1_end, const Iter vec2_start,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin it=vec1;
  Iter itout=vec2_start;
  while ((it!=vec1_end) && (itout !=vec2_end)) {
    *itout=hfcast<T>(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies a vector to another one.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(outvec)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out_start: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec,const Iterin vec_end, const Iter out_start,const Iter out_end)
{
  typedef IterValueType T;
  Iterin it=vec;
  Iter itout=out_start;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=*it;
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//========================================================================
//                           Math Functions
//========================================================================

//$DOCSTRING: Returns the squared value of the parameter
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME square
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Value to be squared")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class T>
inline T square(T val)
{
  return val*val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Implementation of the Gauss function
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME funcGaussian
//------------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(x)()("Position at which the Gaussian is evaluated")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(sigma)()("Width of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(mu)()("Mean value of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
inline HNumber funcGaussian (HNumber x,
			     HNumber sigma,
			     HNumber mu)
{
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));
};
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,sin,sinh,sqrt,square,tan,tanh
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out_start: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter>
void h{$MFUNC!CAPS}2(const Iter vec,const Iter vec_end, const Iter out_start,const Iter out_end)
{
  Iter it=vec;
  Iter itout=out_start;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param data_start: STL Iterator pointing to the first element of an array with
         output values.
  \param vec_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter1,class Iter2>
void h{$MFUNC!CAPS}2(const Iter1 vec,const Iter1 vec_end, const Iter2 out_start,const Iter2 out_end)
{
  Iter1 it=vec;
  Iter2 itout=out_start;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


#define HFPP_OPERATOR_Mul *
#define HFPP_OPERATOR_INPLACE_Mul *=
#define HFPP_OPERATOR_Add +
#define HFPP_OPERATOR_INPLACE_Add +=
#define HFPP_OPERATOR_Div /
#define HFPP_OPERATOR_INPLACE_Div /=
#define HFPP_OPERATOR_Sub -
#define HFPP_OPERATOR_INPLACE_Sub -=

//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================


//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  typedef IterValueType T;
  Iter it=vec1;
  Iterin itout=vec2;
  while ((it!=vec1_end) && (itout !=vec2_end)) {
    *it HFPP_OPERATOR_INPLACE_$MFUNC hfcast<T>(*itout);
    ++it; ++itout;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the vector and a scalar (applied to each element), which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec1: STL Iterator pointing to the first element of an array with
         input and output values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S>
void hi{$MFUNC}2(const Iter vec1,const Iter vec1_end, S val)
{
  typedef IterValueType T;
  Iter it=vec1;
  T val_t = hfcast<T>(val);
  while (it!=vec1_end) {
    *it HFPP_OPERATOR_INPLACE_$MFUNC val_t;
    ++it;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the third vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec3)()("Vector containing the results")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param vec2: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3_start: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter>
void HFPP_FUNC_NAME(const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end, const Iter vec3_start,const Iter vec3_end)
{
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec3_start;
  while ((it1!=vec1_end) && (it2 !=vec2_end) && (itout !=vec3_end)) {
    *itout = hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Performs a $MFUNC between the vector and a scalar, where the result is returned in the second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)(STDIT)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec1: STL Iterator pointing to the first element of an array with
         input  values.
  \param vec1_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter>
void h{$MFUNC}2(const Iterin1 vec1,const Iterin1 vec1_end, S val, const Iter vec2,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin1 itin=vec1;
  Iter itout=vec2;
  T val_t=hfcast<T>(val);
  while ((itin!=vec1_end) && (itout !=vec2_end)) {
    *itout = hfcast<T>(*itin) HFPP_OPERATOR_$MFUNC  val_t;
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE

//$DOCSTRING: Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNegate
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector
*/

template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  IterValueType fac = -1;
  while (it!=vec_end) {
    *it=(*it) * fac;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the values in a vector and returns the value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {sum+=*it; ++it;};
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of all elements in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  typedef IterValueType T;
  T mean=hSum(vec,vec_end);
  if (vec_end>vec) mean/=(vec_end-vec);
  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Sorts a vector in place
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSort
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted in place. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Sorts a vector in place and returns the median value of the elements
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSortMedian
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted first. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
  if (vec_end!=vec) return *(vec+(vec_end-vec)/2);
  else return hfnull<IterValueType>();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the median value of the elements
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMedian
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
T HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> scratch(vec);
  return hSortMedian(scratch);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation around a mean value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(mean)()("The mean value of the vector caluclated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType hStdDev (const Iter vec,const Iter vec_end, const IterValueType mean)
{
  typedef IterValueType T;
  T scrt,sum=hfnull<T>();
  HInteger len=vec_end-vec;
  Iter it=vec;
  while (it<vec_end) {
    scrt = *it - mean;
    sum += scrt * scrt;
    ++it;
  };
  if (len>1) return sqrt(sum / (len-1));
  else return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation of a vector of values
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType hStdDev (const Iter vec,const Iter vec_end)
{
  return hStdDev(vec,vec_end,hMean(vec,vec_end));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec1,
		  const Iter vec1_end,
		  const Iter vec2,
		  const Iter vec2_end)
{
  if (vec2>=vec2_end) return; //If size 0 do nothing
  if (vec1>=vec1_end) return; //If size 0 do nothing
  HInteger ilen=(vec1_end-vec1);
  HInteger olen=(vec2_end-vec2);
  HInteger blen=max(ilen/(olen-1),1);
  //use max to avoid infinite loops if output vector is too large
  Iter it2,it1=vec1;
  Iter ito=vec2,ito_end=vec2_end-1;
  //only produce the first N-1 blocks in the output vector
  while ((it1<vec1_end) && (ito<ito_end)) {
    it2=min(it1+blen,vec1_end);
    *ito=hMean(it1,it2);
    it1=it2;
    ++ito;
    }
  *ito=hMean(it2,vec1_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector by a cetain factor and return a new vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(downsample_factor)()("Factor by which to reduce original size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a new smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class T>
std::vector<T> HFPP_FUNC_NAME (
		  std::vector<T> & vec,
		  HNumber downsample_factor
		  )
{
  std::vector<T>
 newvec(floor(vec.size()/downsample_factor+0.5));
  hDownsample(vec,newvec);
  return newvec;
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFindLowerBound
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Sorted numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(value)()("value to search for")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

Finds -- through a binary search and interpolation -- the location in
  a monotonically increasing vector, where the search value is just
  above or equal to the value in the vector.

This requires random access iterators, in order to have an optimal search result.

*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec,
			 const Iter vec_end,
			 const IterValueType value)
//iterator_traits<Iter>::value_type value)
{
  HNumber value_n=hfcast<HNumber>(value); //This also works for Complex then
  HInteger niter=0;
  if (vec==vec_end) return 0;
  HInteger maxpos=vec_end-vec-1,posguess;
  Iter it1=vec,it2=vec_end-1,it0;
  if (value_n<=hfcast<HNumber>(*it1)) return 0;
  if (value_n>=hfcast<HNumber>(*it2)) return maxpos;
  posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*maxpos;
  it0=vec+posguess;
  if (it0<it1) return hfcast<HInteger>(it1-vec); //Error, Non monotonic
  if (it0>=it2) return hfcast<HInteger>(it2-vec); //Error, Non monotonic
  //  cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
  while (!((value_n < hfcast<HNumber>(*(it0+1))) && (value_n >= hfcast<HNumber>(*it0)))) {
    if (value_n > hfcast<HNumber>(*it0)) {
      it1=it0;
    } else {
      it2=it0;
    };
    it0=it1+(it2-it1)/2;
    if (*it0>value_n) it2=it0; //Binary search step
    else it1=it0;
    posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*(it2-it1)+(it1-vec);
    it0=vec+posguess;
    ++niter;
    //cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
    if (it0<it1) return it1-vec; //Error, Non monotonic
    if (it0>it2) return it2-vec; //Error, Non monotonic
  };
  return posguess;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

/*
//Wrapper for c-style arrays
HInteger hFindLowerBound(const HNumber* vec,
			 const HInteger len,
			 const HNumber value)
{
  return hFindLowerBound(vec,vec+len,value);
}
*/

//$DOCSTRING: Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFlatWeights
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  std::vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING:  Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearWeights
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

The vector is normalized to give a sum of unity. Can be used by
hRunningAverage.
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  std::vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGaussianWeights
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>


std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(wlen/4.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  for (i=0; i<wlen; i++) weights[i]=weights[i]/sum;
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a normalized weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWeights
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
vector<HNumber> hWeights(HInteger wlen, hWEIGHTS wtype){
  vector<HNumber> weights;
  if (wlen<1) wlen=1;
  switch (wtype) {
  case WEIGHTS_LINEAR:
    weights=hLinearWeights(wlen);
    break;
  case WEIGHTS_GAUSSIAN:
    weights=hGaussianWeights(wlen);
    break;
  default: //  WEIGHTS_FLAT:
    weights=hFlatWeights(wlen);
  };
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Calculate the running average of an input vector using a weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(weights)()("Weight vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class DataIter, class NumVecIter>
void HFPP_FUNC_NAME (const DataIter  idata,
		     const DataIter  idata_end,
		     const DataIter  odata,
		     const DataIter  odata_start,
		     const NumVecIter weights,
		     const NumVecIter weights_end)
{
  HInteger l=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=l/2;
  /* To avoid too many rounding errors with Integers */
  typename DataIter::value_type fac = l*10;

  DataIter  dit;
  DataIter  dit2;
  DataIter  din=idata;
  DataIter  dout=odata;
  NumVecIter wit;

  while (din!=idata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights; // weight iterators set to beginning of weights
    while (wit!=weights_end) {
      if (dit<idata) dit2=idata;
      else if (dit>=idata_end) dit2=idata_end-1;
      else dit2=dit;
      *dout=*dout+((*dit2)*(*wit)*fac);
      ++dit; ++wit;
    };
    *dout/=fac;
    ++dout; ++din; //point to the next element in data input and output vector
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Overloaded function to automatically calculate weights.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter idata,
		     const DataIter idata_end,
		     const DataIter odata,
		     const DataIter odata_end,
		     HInteger wlen,
		     hWEIGHTS wtype)
{
  vector<HNumber> weights = hWeights(wlen, wtype);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (idata,
							idata_end,
							odata,
							odata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




//========================================================================
//                     RF (Radio Frequency) Functions
//========================================================================


//$DOCSTRING: Calculates the power of a complex spectrum and add it to an output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSpectralPower
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL)
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  \param vec: STL Iterator pointing to the first element of an array with
         input values.
  \param vec_end: STL Iterator pointing to the end of the input vector

  \param out_start: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec,const Iterin vec_end, const Iter out_start,const Iter out_end)
{
  typedef IterValueType T;
  Iterin it=vec;
  Iter itout=out_start;
  while ((it!=vec_end) && (itout !=out_end)) {
    *itout+=real((*it)*conj(*it));
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




//========================================================================
//                             I/O Functions
//========================================================================

//------------------------------------------------------------------------
//#define HFPP_FUNC_NAME hCloseFile
//------------------------------------------------------------------------
/*!
  \brief Function to close a file with a datareader object providing the pointer to the object as an integer.

  \param iptr: Pointer to a DataReader object, stored as an integer.
*/
void hCloseFile(HIntPointer iptr) {
  union{void* ptr; CR::DataReader* drp;};
  ptr=IntAsPtr(iptr);
  if (ptr!=Null_p) delete drp;
}

//------------------------------------------------------------------------
//#define HFPP_FUNC_NAME hOpenFile
//------------------------------------------------------------------------
/*!
  \brief Function to open a file based on a filename and returning a pointer to a datareader object as an integer.

  \param filename: Full name of the file to be opened.
*/

HIntPointer hOpenFile(HString Filename) {

  bool opened;
  union{HIntPointer iptr; void* ptr; CR::DataReader* drp; CR::LOFAR_TBB* tbb; CR::LopesEventIn* lep;};

  //Create the a pointer to the DataReader object and store the pointer

  HString Filetype = hgetFiletype(Filename);
  if (Filetype=="LOPESEvent") {
    lep = new CR::LopesEventIn;
    opened=lep->attachFile(Filename);
    cout << "File="<<Filename << endl; lep->summary();
  } else if (Filetype=="LOFAR_TBB") {
    tbb = new CR::LOFAR_TBB(Filename,1024);
    opened=tbb!=NULL;
    MSG("Opening File="<<Filename);tbb->summary();
    /*    if (opened) {
      CasaVector<int> OffsetsCasa = tbb->sample_offset();
      aipsvec2stlvec(OffsetsCasa, Offsets);
      };
    */
  } else {
    ERROR("DataFunc_CR_dataReaderObject: Unknown Filetype = " << Filetype  << ", Filename=" << Filename);
    opened=false;
  }

  if (!opened){
    ERROR("DataFunc_CR_dataReaderObject: Opening file " << Filename << " failed.");
    return PtrAsInt(Null_p);
  };

  //Read the data Header, containing important information about the file (e.g. size)
  //CasaRecord hdr=drp->headerRecord();
  //uint i,nfields=hdr.nfields();
  //  for (i=0; i<nfields; i++) MSG("hdr name="<<hdr.name(i) << " type="<<hdr.dataType(i));

  return iptr;
}

//------------------------------------------------------------------------
template <class S, class T>
void aipscol2stlvec(casa::Matrix<S> &data, std::vector<T>& stlvec, const HInteger col){
    HInteger i,nrow,ncol;

    nrow=data.nrow();
    ncol=data.ncolumn();
    //    if (ncol>1) {MSG("aipscol2stlvec: ncol="<<ncol <<" (nrow="<<nrow<<")");};
    if (col>=ncol) {
	ERROR("aipscol2stlvec: column number col=" << col << " is larger than total number of columns (" << ncol << ") in matrix.");
	stlvec.clear();
	return;
    }

    stlvec.resize(nrow);
    CasaVector<S> CASAVec = data.column(col);

//    p=stlvec.begin();

    for (i=0;i<nrow;i++) {
//	*p=hfcast<T>(CASAVec[i]);
	stlvec[i]=hfcast<T>(CASAVec[i]);
//	p++;
    };
}

/*!
  \brief The function converts an aips++ vector to an stl vector
 */

template <class S, class T>
void aipsvec2stlvec(CasaVector<S>& data, std::vector<T>& stlvec){
    HInteger i,n;
//    std::vector<R>::iterator p;

    n=data.size();
    stlvec.resize(n);
//    p=stlvec.begin();
    for (i=0;i<n;i++) {
//	*p=hfcast<T>(data[i]);
	stlvec[i]=hfcast<T>(data[i]);
//	p++;
    };
}


//This function is copied from hfget.cc since I don't know a better
//way to do a proper instantiation of a templated function across two
//cpp files ..
template <class T, class S>
void copycast_vec(std::vector<T> &vi, std::vector<S> & vo) {
  typedef typename std::vector<T>::iterator Tit;
  typedef typename std::vector<S>::iterator Sit;
  Tit it1=vi.begin();
  Tit end=vi.end();
  if (it1==end) {vo.clear();}
  else {
    vo.assign(vi.size(),hfnull<S>()); //make the new vector equal in size and initialize with proper Null values
    Sit it2=vo.begin();
    while (it1!=end) {
      *it2=hfcast<S>(*it1);
      it1++; it2++;
    };
  };
}



//$DOCSTRING: Read data from a Datareader object (pointer in iptr) into a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadFile
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Data (output) vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HIntPointer)(iptr)()("Integer containing pointer to the datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HString)(Datatype)()("Name of the data column to be retrieved (e.g., FFT, Fx,Time, Frequency...)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(Antenna)()("Antenna number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(Blocksize)()("Length of the data block to read")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(Block)()("Block number to read")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HInteger)(Stride)()("Stride between blocks")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HInteger)(Shift)()("Shift start of first block by n samples")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//#define HFPP_PARDEF_8 (HInteger)(Offsets)()("Offsets per antenna")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------

/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example on how to use this with the Python wrapper

datareader_ptr=hOpenFile("data/lofar/RS307C-readfullsecond.h5")
datareader_ptr=hOpenFile("/Users/falcke/LOFAR/usg/data/lopes/test.event")

#offsets=IntVec()
data=FloatVec()
idata=IntVec()
cdata=ComplexVec()
sdata=StringVec()
Datatype="Fx"
Antenna=1
Blocksize=1024
Block=10
Stride=0
Shift=0
hReadFile(idata,datareader_ptr,Datatype,Antenna,Blocksize,Block,Stride,Shift,offsets)
hCloseFile(datareader_ptr)

The data will then be in the vector idata. You can covert that to a
Python list with [].extend(idata)
*/

template <class T>
void HFPP_FUNC_NAME(std::vector<T> & vec,
		    HIntPointer iptr,
		    HString Datatype,
		    HInteger Antenna,
		    HInteger Blocksize,
		    HInteger Block,
		    HInteger Stride,
		    HInteger Shift)
//	       std::vector<HInteger> & Offsets)
{

  DataReader *drp=reinterpret_cast<DataReader*>(iptr);

  //First retrieve the pointer to the pointer to the dataRead and check whether it is non-NULL.
  if (drp==Null_p){
    ERROR("dataRead: pointer to FileObject is NULL, DataReader not found.");
    return;
  };

//!!!One Needs to verify somehow that the parameters make sense !!!
  if (Antenna > static_cast<HInteger>(drp->nofAntennas()-1)) {
    ERROR("Requested Antenna number too large!");
    return;
  };
  drp->setBlocksize(Blocksize);
  drp->setBlock(Block);
  drp->setStride(Stride);
  drp->setShift(Shift);

  CasaVector<uint> antennas(1,Antenna);
  drp->setSelectedAntennas(antennas);

  address ncol;

  #define copy_ary2vp  ncol=ary.ncolumn(); /* MSG("ncol="<<ncol<<", Antenna="<<Antenna); */ if (ncol>1 && Antenna<ncol) aipscol2stlvec(ary,vec,Antenna); else aipscol2stlvec(ary,vec,0);


  if (Datatype=="Time") {
    if (typeid(vec) == typeid(vector<double>)) {
      std::vector<double> * vec_p;
      vec_p=reinterpret_cast<vector<double>*>(&vec); //That is just a tr
      drp->timeValues(*vec_p);
    }  else {
      std::vector<double> tmpvec;
      drp->timeValues(tmpvec);
      hConvert(tmpvec,vec);
    };
  }
  else if (Datatype=="Frequency") {
    //vector<HNumber>* vp2; *vp2 = drp->frequencyValues().tovec();
    //    std::vector<double> vals;
    //copycast_vec(vals,vec);
    CasaVector<double> val = drp->frequencyValues();
    aipsvec2stlvec(val,vec);
  }
  else if (Datatype=="Fx") {
//hf #define MAKE_UNION_CASA_MATRIX_AND_STLVEC(TYPECASA,STLVEC,CASAVEC) \
//     casa::IPosition shape(2,STLVEC.size()); shape(1)=1;	\
//     casa::Matrix<TYPECASA> CASAVEC(shape,reinterpret_cast<TYPECASA*>(&(STLVEC[0])),casa::SHARE)
//     if (typeid(vec)==typeid(std::vector<double>)) {
//       vec.resize(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,vec,mtrx);
//       drp->fx(mtrx);
//     } else {
//       std::vector<double> nvec(Blocksize);
//       MAKE_UNION_CASA_MATRIX_AND_STLVEC(double,nvec,mtrx);
//       drp->fx(mtrx);
//       copycast_vec(nvec,vec); //Copy back to output vector and convert type
//     };
    CasaMatrix<CasaNumber> ary=drp->fx();
    copy_ary2vp;
  }
  else if (Datatype=="Voltage") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->voltage();
    copy_ary2vp;
  }
  else if (Datatype=="invFFT") {
    //    std::vector<HNumber>* vp2 = new std::vector<HNumber>;
    CasaMatrix<CasaNumber> ary=drp->invfft();
    copy_ary2vp;
  }
  else if (Datatype=="FFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->fft();
    copy_ary2vp;
  }
  else if (Datatype=="CalFFT") {
    //    std::vector<HComplex>* vp2 = new std::vector<HComplex>;
    CasaMatrix<CasaComplex> ary=drp->calfft();
    copy_ary2vp;
  }
  else {
    ERROR("DataFunc_CR_dataRead: Datatype=" << Datatype << " is unknown.");
    vec.clear();
    return;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

/*
  else if (Datatype=="Fx") {aipscol2stlvec(drp->fx(),*vp,0);}
  else if (Datatype=="Voltage") {aipscol2stlvec(drp->voltage(),*vp,0);}
  else if (Datatype=="invFFT") {aipscol2stlvec(drp->invfft(),*vp,0);}
  else if (Datatype=="FFT") {aipscol2stlvec(drp->fft(),*vp,0);}
  else if (Datatype=="CalFFT") {aipscol2stlvec(drp->calfft(),*vp,0);}
*/

#include <Pypeline/hftools.hpp>
