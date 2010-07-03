/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 4824 2010-05-06 09:07:23Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
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

#include <Bindings/pycr_numpycasa.h>

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin
  
  // ============================================================================
  //
  // Template specializations
  //
  // ============================================================================
  
#if LONG_MAX < 4294967296
    template<> PyArray_TYPES getEnum<int>(void) {
      return num_util::getEnum<long>();
    }
    
    template<> PyArray_TYPES getEnum<unsigned int>(void) {
      return num_util::getEnum<unsigned long>();
    }
#endif // LONG_MAX > 2147483647
    
    template <> bool casaFromNumpyCast<>(bpl::numeric::array pydata, 
					 casa::Array<casa::Complex> &casaarray ) {
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_contiguous(pydata);
      
      // get the shape of the numpy-array
      int i,rank = num_util::rank(pydata);
      int size=num_util::size(pydata);
      casa::IPosition shape(rank);
      for (i=0; i<rank; i++){
	shape[(rank-i-1)] = num_util::get_dim(pydata, i);
      };
      
      //reshape the output array and get pointer to it
      casaarray.resize(shape);
      Bool  deleteIt;
      casa::Complex * datapointer = casaarray.getStorage(deleteIt);
      
      switch (num_util::type(pydata)){
      case PyArray_BYTE: 
	{
	  char *charptr = numpyBeginPtr<char>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(charptr[i]);
	}
	break;
      case PyArray_UBYTE:
	{
	  unsigned char *ucharptr = numpyBeginPtr<unsigned char>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(ucharptr[i]);
	}
	break;
      case PyArray_SHORT:
	{
	  short *shortptr = numpyBeginPtr<short>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(shortptr[i]);
	}
      	break;
      case PyArray_USHORT:
	{
	  unsigned short *ushortptr = numpyBeginPtr<unsigned short>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(ushortptr[i]);
	}
      	break;
      case PyArray_INT:
	{
	  int *intptr = numpyBeginPtr<int>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(intptr[i]);
	}
      	break;
      case PyArray_UINT:
	{
	  unsigned int *uintptr = numpyBeginPtr<unsigned int>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(uintptr[i]);
	}
      	break;
      case PyArray_LONG:
	{
	  long *longptr = numpyBeginPtr<long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(longptr[i]);
	}
      	break;
      case PyArray_ULONG:
	{
	  unsigned long *ulongptr = numpyBeginPtr<unsigned long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(ulongptr[i]);
	}
      	break;
      case PyArray_LONGLONG:
	{
	  long long *llptr = numpyBeginPtr<long long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(llptr[i]);
	}
      	break;
      case PyArray_ULONGLONG:
	{
	  unsigned long long *ullptr = numpyBeginPtr<unsigned long long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(ullptr[i]);
	}
      	break;
      case PyArray_FLOAT:
	{
	  float *flptr = numpyBeginPtr<float>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(flptr[i]);
	}
      	break;
      case PyArray_DOUBLE:
	{
	  double *doubleptr = numpyBeginPtr<double>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(doubleptr[i]);
	}
      	break;
      case PyArray_CFLOAT:
      	{
      	  std::complex<float>  *complexptr = numpyBeginPtr< std::complex<float> >(pydata);
      	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(complexptr[i]);
      	}
      	break;
      case PyArray_CDOUBLE:
      	{
      	  std::complex<double>  *dcomplexptr = numpyBeginPtr< std::complex<double> >(pydata);
      	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::Complex>(dcomplexptr[i]);
      	}
      	break;
      default:
	cout << "casaFromNumpyCast: Type of numpy-array not implemented!" << endl;
	break;
      };
      casaarray.putStorage(datapointer, deleteIt);  
      return true;
    };

    template <> bool casaFromNumpyCast(bpl::numeric::array pydata, 
				       casa::Array<casa::DComplex> &casaarray ) {
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_contiguous(pydata);
      
      // get the shape of the numpy-array
      int i,rank = num_util::rank(pydata);
      int size=num_util::size(pydata);
      casa::IPosition shape(rank);
      for (i=0; i<rank; i++){
	shape[(rank-i-1)] = num_util::get_dim(pydata, i);
      };
      
      //reshape the output array and get pointer to it
      casaarray.resize(shape);
      Bool  deleteIt;
      casa::DComplex * datapointer = casaarray.getStorage(deleteIt);
      
      switch (num_util::type(pydata)){
      case PyArray_BYTE: 
	{
	  char *charptr = numpyBeginPtr<char>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(charptr[i]);
	}
	break;
      case PyArray_UBYTE:
	{
	  unsigned char *ucharptr = numpyBeginPtr<unsigned char>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(ucharptr[i]);
	}
	break;
      case PyArray_SHORT:
	{
	  short *shortptr = numpyBeginPtr<short>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(shortptr[i]);
	}
      	break;
      case PyArray_USHORT:
	{
	  unsigned short *ushortptr = numpyBeginPtr<unsigned short>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(ushortptr[i]);
	}
      	break;
      case PyArray_INT:
	{
	  int *intptr = numpyBeginPtr<int>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(intptr[i]);
	}
      	break;
      case PyArray_UINT:
	{
	  unsigned int *uintptr = numpyBeginPtr<unsigned int>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(uintptr[i]);
	}
      	break;
      case PyArray_LONG:
	{
	  long *longptr = numpyBeginPtr<long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(longptr[i]);
	}
      	break;
      case PyArray_ULONG:
	{
	  unsigned long *ulongptr = numpyBeginPtr<unsigned long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(ulongptr[i]);
	}
      	break;
      case PyArray_LONGLONG:
	{
	  long long *llptr = numpyBeginPtr<long long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(llptr[i]);
	}
      	break;
      case PyArray_ULONGLONG:
	{
	  unsigned long long *ullptr = numpyBeginPtr<unsigned long long>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(ullptr[i]);
	}
      	break;
      case PyArray_FLOAT:
	{
	  float *flptr = numpyBeginPtr<float>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(flptr[i]);
	}
      	break;
      case PyArray_DOUBLE:
	{
	  double *doubleptr = numpyBeginPtr<double>(pydata);
	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(doubleptr[i]);
	}
      	break;
      case PyArray_CFLOAT:
      	{
      	  std::complex<float>  *complexptr = numpyBeginPtr< std::complex<float> >(pydata);
      	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(complexptr[i]);
      	}
      	break;
      case PyArray_CDOUBLE:
      	{
      	  std::complex<double>  *dcomplexptr = numpyBeginPtr< std::complex<double> >(pydata);
      	  for (i=0; i<size; i++) datapointer[i] = static_cast<casa::DComplex>(dcomplexptr[i]);
      	}
      	break;
      default:
	cout << "casaFromNumpyCast: Type of numpy-array not implemented!" << endl;
	break;
      };
      casaarray.putStorage(datapointer, deleteIt);  
      return true;
    };


  // ============================================================================
  //
  // Template instantations
  //
  // ============================================================================

    template bool casaFromNumpyCast(bpl::numeric::array pydata, 
				      casa::Array<int> &casarray);
    template bool casaFromNumpyCast(bpl::numeric::array pydata, 
				    casa::Array<casa::Complex> &casarray);
    template bool casaFromNumpyCast(bpl::numeric::array pydata, 
				    casa::Array<casa::DComplex> &casarray);

 
  } // Namespace PYCR -- end
 
} // Namespace CR -- end
