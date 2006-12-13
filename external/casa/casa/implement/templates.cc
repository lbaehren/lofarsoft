/*!
  \brief Template instantiations

  Template instantiations for the classes in the casa library module; as template
  instantiation within the CASA build environment is done via a set of
  'templates' files - and not directly within the class implementation files -
  we need to create the required symbols at this point.
 */

#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>

#include <casa/Containers/Block.h>

#include <casa/Exceptions/Error.h>

namespace casa{

  // ----------------------------------------------------- Arrays
  
  template class Array<unsigned int>;
  template class Array<int>;
  template class Array<Float>;
  template class Array<Double>;
  template class Array<Complex>;
  template class Array<DComplex>;
  
  template class Cube<unsigned int>;
  template class Cube<int>;
  template class Cube<Float>;
  template class Cube<Double>;
  template class Cube<Complex>;
  template class Cube<DComplex>;
  
  template class MaskedArray<unsigned int>;
  template class MaskedArray<int>;
  template class MaskedArray<float>;
  template class MaskedArray<Double>;
  
  template class Vector<unsigned int>;
  template class Vector<int>;
  template class Vector<Float>;
  template class Vector<Double>;
  template class Vector<Complex>;
  template class Vector<DComplex>;

  // ------------------------------------------------- Containers

  template class Block<unsigned int>;
  template class Block<int>;
  template class Block<Float>;
  template class Block<Double>;
  
  // ------------------------------------------------- Exceptions
  
  template class indexError<String>;
  template class indexError<Double>;
  template class indexError<Float>;
  template class indexError<Int>;
  template class indexError<Short>;
  template class indexError<uInt>;
  template class indexError<void *>;
  
}
