#include "Reference.h"
#include "tostring.h"
#include <vector>

/*
  This simple program evaluates the cost of checking the pointer every
  time it is accessed.  

  It was used to verify that the new Reference::Able and Reference::To
  paradigm did not incur a performance hit.  There is an extra pointer
  check in the new model, but the overall scheme is much simpler than
  the previous version, in which the Reference::Able class maintained
  a vector of addresses of pointers to itself.

  To repeat the comparison with the old scheme,
  please see many_references.C

*/

class Junk : public Reference::Able
{
public:
  // junk is as junk does
  void does () {};
};

int main (int argc, char** argv)
{
  unsigned n = 1024 * 1024 * 1024;
  if (argc>1)
    n *= fromstring<unsigned> (argv[1]);

  Junk j;

  Reference::To<Junk> r = &j;

  for (unsigned i=0; i<n; i++)
    r->does();

  return 0;
}
