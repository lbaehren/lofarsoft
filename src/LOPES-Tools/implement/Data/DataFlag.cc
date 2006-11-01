
#include <lopes/Data/DataFlag.h>

// --- Construction ------------------------------------------------------------

template <class T> DataFlag<T>::DataFlag (const Int& num)
  : ClippingFraction<T> ()
{
  Vector<T> flags (num, 1.0);
  //
  DataFlag<T>::setFlags (flags);
}

template <class T> DataFlag<T>::DataFlag (const Int& num,
		    const T& threshold)
  : ClippingFraction<T> (), threshold_p(threshold)
{
  Vector<T> flags (num, 1.0);
  //
  DataFlag<T>::setFlags (flags);
}

template <class T> DataFlag<T>::DataFlag (const Int& num,
					  const T& threshold,
					  const Vector<T>& limits)
  : ClippingFraction<T> (limits), threshold_p(threshold)
{
  Vector<T> flags (num, 1.0);
  //
  DataFlag<T>::setFlags (flags);
}

template <class T> DataFlag<T>::DataFlag (const Vector<Bool>& flags)
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

template <class T> Bool DataFlag<T>::float2bool (const T& flag)
{
  if (flag > threshold_p) {
    return True;
  } else {
    return False;
  }
}

template <class T> void DataFlag<T>::flags (Bool& flag,
		      const uInt& num)
{
  T fflag (0.0);
  //
  DataFlag<T>::flags (fflag, num);
  // 
  flag = DataFlag<T>::float2bool (fflag);
}

template <class T> void DataFlag<T>::flags (T& flag,
		      const uInt& num)
{
  // check if the dataset identifier is valid
  if (num < flags_p.nelements()) {
    flag = flags_p(num);
  } else {
    cerr << "[DataFlag<T>::flags]" << endl;
  }
}

template <class T> void DataFlag<T>::flags (Vector<Bool>& flags)
{
  uInt nofFiles (flags_p.nelements());
  //
  flags.resize (nofFiles);
  //
  for (uInt n(0); n<nofFiles; n++) {
    flags(n) = float2bool(flags_p(n));
  }
}

template <class T> void DataFlag<T>::flags (Vector<T>& flags)
{
  flags.resize (flags_p.shape());
  flags = flags_p;
}

template <class T> void DataFlag<T>::setFlags (const Vector<Bool>& bFlags)
{
  Int nofElements (bFlags.nelements());
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
					       const Int& num)
{;}

template <class T> void DataFlag<T>::flagData (Vector<Bool>& flags,
					       const Matrix<T>& data)
{;}

template <class T> void DataFlag<T>::flagData (Vector<T>& flags,
					       const Matrix<T>& data)
{;}

