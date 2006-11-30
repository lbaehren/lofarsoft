
#include <Data/DataFlag.h>

using std::cerr;
using std::endl;

namespace LOPES {  // namespace LOPES -- begin
  
  // --- Construction ------------------------------------------------------------
  
  template <class T> DataFlag<T>::DataFlag (const int& num)
    : ClippingFraction<T> ()
  {
    Vector<T> flags (num, 1);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const int& num,
					    const T& threshold)
    : ClippingFraction<T> (), threshold_p(threshold)
  {
    Vector<T> flags (num, 1);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const int& num,
					    const T& threshold,
					    const Vector<T>& limits)
    : ClippingFraction<T> (limits), threshold_p(threshold)
  {
    Vector<T> flags (num, 1);
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Vector<bool>& flags)
    : ClippingFraction<T> ()
  {
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> DataFlag<T>::DataFlag (const Vector<T>& flags)
    : ClippingFraction<T> ()
  {
    DataFlag<T>::setFlags (flags);
  }
  
  // --- Destruction -------------------------------------------------------------
  
  template <class T> DataFlag<T>::~DataFlag ()
  {}
  
  // --- Flagging threshold ------------------------------------------------------
  
  template <class T> T DataFlag<T>::threshold ()
  {
    return threshold_p;
  }
  
  template <class T> void DataFlag<T>::setThreshold (const T& threshold)
  {
    threshold_p = threshold;
  }
  
  // --- Access to the flags -----------------------------------------------------
  
  template <class T> bool DataFlag<T>::float2bool (const T& flag)
  {
    if (flag > threshold_p) {
      return true;
    } else {
      return false;
    }
  }
  
  template <class T> void DataFlag<T>::flags (bool& flag,
					      const uint& num)
  {
    T fflag (0);
    //
    DataFlag<T>::flags (fflag, num);
    // 
    flag = DataFlag<T>::float2bool (fflag);
  }
  
  template <class T> void DataFlag<T>::flags (T& flag,
					      const uint& num)
  {
    // check if the dataset identifier is valid
    if (num < flags_p.nelements()) {
      flag = flags_p(num);
    } else {
      cerr << "[DataFlag<T>::flags]" << endl;
    }
  }
  
  template <class T> void DataFlag<T>::flags (Vector<bool>& flags)
  {
    uint nofFiles (flags_p.nelements());
    //
    flags.resize (nofFiles);
    //
    for (uint n(0); n<nofFiles; n++) {
      flags(n) = float2bool(flags_p(n));
    }
  }
  
  template <class T> void DataFlag<T>::flags (Vector<T>& flags)
  {
    flags.resize (flags_p.shape());
    flags = flags_p;
  }
  
  template <class T> void DataFlag<T>::setFlags (const Vector<bool>& bFlags)
  {
    int nofElements (bFlags.nelements());
    Vector<T> flags (nofElements, 1.0);
    //
    for (int n(0); n<nofElements; n++) {
      if (!bFlags(n)) {
	flags(n) = 0.0;
      }
    }
    //
    DataFlag<T>::setFlags (flags);
  }
  
  template <class T> void DataFlag<T>::setFlags (const Vector<T>& flags)
  {
    flags_p.resize (flags.nelements());
    flags_p = flags;
  }
  
  // --- Set the flags for a set of data -----------------------------------------
  
  template <class T> void DataFlag<T>::setFlags (const Matrix<T>& data)
  {;}
  
  template <class T> void DataFlag<T>::setFlags (const Vector<T>& data,
						 const int& num)
  {;}
  
  template <class T> void DataFlag<T>::flagData (Vector<bool>& flags,
						 const Matrix<T>& data)
  {;}
  
  template <class T> void DataFlag<T>::flagData (Vector<T>& flags,
						 const Matrix<T>& data)
  {;}

  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================

//   template DataFlag<int>;
//   template DataFlag<uint>;
//   template DataFlag<float>;
//   template DataFlag<double>;
  
}  // namespace LOPES -- end
