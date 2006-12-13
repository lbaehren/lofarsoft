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

namespace casa{
  
  template class Array<unsigned int>;
  template class Array<int>;
  template class Array<float>;
  template class Array<double>;
  
  template class Block<unsigned int>;
  template class Block<int>;
  template class Block<float>;
  template class Block<double>;
  
  template class Cube<unsigned int>;
  template class Cube<int>;
  template class Cube<float>;
  template class Cube<double>;
  
  template class MaskedArray<unsigned int>;
  template class MaskedArray<int>;
  template class MaskedArray<float>;
  template class MaskedArray<double>;
  
  template class Vector<unsigned int>;
  template class Vector<int>;
  template class Vector<float>;
  template class Vector<double>;

}
