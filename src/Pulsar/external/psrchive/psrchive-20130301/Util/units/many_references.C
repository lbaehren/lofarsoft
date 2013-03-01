#include "Reference.h"
#include "tostring.h"
#include <vector>

/*

  This simple program evaluates the cost of removing references to
  a Reference::Able object.

  It was used to verify that the new Reference::Able and Reference::To
  paradigm improved performance.  The new scheme uses a single shared
  handle to count references and enable automatic notification of
  deletion.  It is much simpler than the previous version, in which
  the Reference::Able class maintained a vector of addresses of
  pointers to itself.

  To repeat the comparison with the old scheme, first compile:

  cvs update -A
  make many_references many_calls
  mv many_references many_references.new
  mv many_calls many_calls.new

  cvs update -r old-reference ReferenceTo.h ReferenceAble.[hC]
  make many_references many_calls
  mv many_references many_references.old
  mv many_calls many_calls.old

  To time the old binaries vs the new ones: e.g.

  time ./many_calls.new

  In my tests on a MacBook Pro (10.4), I found

  many_calls       old/new = 1.08  (6.5s / 6.0s)
  many_references  old/new = 1.5   (11.5s/ 7.8s)

  Willem van Straten (2007/05/29)

*/

class Junk : public Reference::Able { };

int main (int argc, char** argv)
{
  unsigned n = 1024 * 1024;
  if (argc>1)
    n *= fromstring<unsigned> (argv[1]);

  Junk j;

  for (unsigned i=0; i<n; i++)
    std::vector< Reference::To<Junk> > (32, &j);

  return 0;
}
