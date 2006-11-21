
#include <Utilities/CMakeTesting.h>

#ifdef HAVE_CASA

template <class T>
MyArray<T>::MyArray ()
{
  casa::IPosition shape(1,1);
  vect_p.resize (shape);
  vect_p = 0;
}

template <class T>
MyArray<T>::MyArray (casa::Array<T> & vect) {
  setArray (vect);
}

template <class T>
MyArray<T>::~MyArray ()
{}

template <class T>
void MyArray<T>::setArray (casa::Array<T> & vect)
{
  vect_p.resize (vect.shape());
  vect_p = vect;
  //
  nofElements_p = vect.nelements();
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

// template class casa::Array<int>;
// template class casa::Array<unsigned int>;
// template class casa::Array<float>;
// template class casa::Array<double>;

template class MyArray<int>;
template class MyArray<unsigned int>;
template class MyArray<float>;
template class MyArray<double>;

#endif
