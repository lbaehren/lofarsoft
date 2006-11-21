
#include <stdlib.h>

#ifdef HAVE_CASA
#include <Utilities/CMakeTesting.h>
#endif

/*!
  \brief A simple test for compiling a program using CASA libraries
*/

int main ()
{
  //! Create default object
  {
    MyArray<double> obj;
  }
  
  return 0;
}
