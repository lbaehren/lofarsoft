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
    

    template <class Iter> bool copyCastFromNumpyComplex(bpl::numeric::array pydata,
							Iter outdata, Iter outdata_end) {
      typedef typename iterator_traits<Iter>::value_type T;
      // test if the numpy-array has the preperties we need
      // (they throw exceptions if not)
      num_util::check_contiguous(pydata);
      
      int i=0, size=num_util::size(pydata);
      Iter currdata=outdata;
      
      switch (num_util::type(pydata)){
      case PyArray_BYTE: 
	{
	  char *charptr = numpyBeginPtr<char>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  { 
	    *currdata = static_cast<T>(charptr[i]);
	    currdata++; i++;
	  }
	}
	break;
      case PyArray_UBYTE:
	{
	  unsigned char *ucharptr = numpyBeginPtr<unsigned char>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  { 
	    *currdata = static_cast<T>(ucharptr[i]);
	    currdata++; i++;
	  }
	}
	break;
      case PyArray_SHORT:
	{
	  short *shortptr = numpyBeginPtr<short>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {  
	    *currdata = static_cast<T>(shortptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_USHORT:
	{
	  unsigned short *ushortptr = numpyBeginPtr<unsigned short>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {  
	    *currdata = static_cast<T>(ushortptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_INT:
	{
	  int *intptr = numpyBeginPtr<int>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(intptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_UINT:
	{
	  unsigned int *uintptr = numpyBeginPtr<unsigned int>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(uintptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_LONG:
	{
	  long *longptr = numpyBeginPtr<long>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(longptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_ULONG:
	{
	  unsigned long *ulongptr = numpyBeginPtr<unsigned long>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(ulongptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_LONGLONG:
	{
	  long long *llptr = numpyBeginPtr<long long>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(llptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_ULONGLONG:
	{
	  unsigned long long *ullptr = numpyBeginPtr<unsigned long long>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(ullptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_FLOAT:
	{
	  float *flptr = numpyBeginPtr<float>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(flptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_DOUBLE:
	{
	  double *doubleptr = numpyBeginPtr<double>(pydata);
	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(doubleptr[i]);
	    currdata++; i++;
	  }
	}
      	break;
      case PyArray_CFLOAT:
      	{
      	  std::complex<float>  *complexptr = numpyBeginPtr< std::complex<float> >(pydata);
      	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(complexptr[i]);
	    currdata++; i++;
	  }
      	}
      	break;
      case PyArray_CDOUBLE:
      	{
      	  std::complex<double>  *dcomplexptr = numpyBeginPtr< std::complex<double> >(pydata);
      	  while ( (i<size) && (currdata != outdata_end))  {   
	    *currdata = static_cast<T>(dcomplexptr[i]);
	    currdata++; i++;
	  }
      	}
      	break;
      default:
	cout << "copyCastFromNumpy: Type of numpy-array not implemented!" << endl;
	break;
      };
      
      if (i != size){
	cerr << "copyCastFromNumpy: Supplied ierators and numpy array have different sitze!" << endl;
	return false;
      }
      
      return true;
    };
    
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 std::complex<float> *outdata, 
					 std::complex<float> *outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
    };
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 std::complex<double> *outdata, 
					 std::complex<double> *outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
    };
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 vector< std::complex<float> >::iterator outdata, 
					 vector< std::complex<float> >::iterator outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
    };
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 vector< std::complex<double> >::iterator outdata, 
					 vector< std::complex<double> >::iterator outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
    };
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 casa::Array< std::complex<float> >::IteratorSTL outdata, 
					 casa::Array< std::complex<float> >::IteratorSTL outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
    };
    template <> bool copyCastFromNumpy<>(bpl::numeric::array pydata,
					 casa::Array< std::complex<double> >::IteratorSTL outdata, 
					 casa::Array< std::complex<double> >::IteratorSTL outdata_end) {
      return copyCastFromNumpyComplex(pydata, outdata, outdata_end);
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
