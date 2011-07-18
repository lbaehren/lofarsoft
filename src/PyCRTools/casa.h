// ________________________________________________________________________
//                                                CASA library header files

#ifndef HFTOOLS_CASA_DEF_H
#define HFTOOLS_CASA_DEF_H

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordField.h>
#include <casa/Containers/List.h>
#include <casa/Containers/ListIO.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/Exceptions/Error.h>

//Define types that are being used for the data reader and the casa arrays
typedef double CasaNumber;
typedef casa::DComplex CasaComplex;
typedef int CasaInteger;
typedef casa::Record CasaRecord ;

#define CasaMatrix casa::Matrix
#define CasaArray casa::Array
#define CasaVector casa::Vector

#define STL2CASA_SHARED(TYPE,STLVEC,CASAVEC) TYPE * STL_storage = &(STLVEC[0]); casa::IPosition CASA_shape(1,STLVEC.size()); casa::Vector<TYPE> CASAVEC(CASA_shape,STL_storage,casa::SHARE);

casa::Int hSum(const casa::Int*, const casa::Int*);
casa::Int hSum(casa::Int* const&, casa::Int* const&);

template <class S, class T>
void aipsvec2stlvec(CasaVector<S>& data, std::vector<T>& stlvec);

template <class S, class Iter>
void aipsvec2stdit(CasaVector<S>& data, const Iter stlvec, const Iter stlvec_end);

template <class T>
CasaVector<T> stl2casa(std::vector<T>& stlvec)
{
  T * storage = &(stlvec[0]);
  casa::IPosition shape(1,stlvec.size()); //tell casa the size of the vector
  CasaVector<T> casavec(shape,storage,casa::SHARE);
  return casavec;
}

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
    HInteger i,n=data.size();
    stlvec.resize(n);
    for (i=0;i<n;i++) {
	stlvec[i]=hfcast<T>(data[i]);
    };
}

/*!
  \brief Copies a aips++ vector to a vector of a different type specified by iterators or pointers.
 */
template <class S, class Iter>
void aipsvec2stdit(CasaVector<S>& data, const Iter stlvec, const Iter stlvec_end){
  typedef IterValueType T;
  HInteger i,n=data.size();
  Iter vec=stlvec;
  for (i=0; (i<n) && (vec!=stlvec_end); i++) {
    *vec=hfcast<T>(data[i]);
    ++vec;
  };
}

#endif
