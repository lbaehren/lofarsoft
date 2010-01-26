/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextIndex.h"

#include <iostream>
using namespace std;

class container : public Reference::Able {

public:

  container () { ndat = 0; }

  void set_ndat (unsigned _ndat) { ndat = _ndat; }
  unsigned get_ndat () const { return ndat; }

  class interface : public TextInterface::To<container> {

  public:
    interface (container* instance) {
      if (instance)
	set_instance( instance );
      add (&container::get_ndat, "ndat", "number of data elements");
    }

  };

  interface* get_interface () { return new interface(this); }

protected:
  unsigned ndat;
};

int main () try {

  container test;
  test.set_ndat (6);

  // from the third element to the last
  TextIndex index ("dat=3-");
  index.set_container( test.get_interface() );

  if (index.size() != 3) {
    cerr << "test_TextIndex error: size=" << index.size() << endl;
    return -1;
  }

  for (unsigned i=0; i<index.size(); i++)
    if (index.get_index(i) != "dat=" + tostring(i+3)) {
      cerr << "test_TextIndex error: index[" << i << "]=" 
           << index.get_index(i) << endl;
      return -1;
    }

  cerr << "test_TextIndex SUCCESS!" << endl;
  return 0;
}
catch (Error& error) {
  cerr << "test_TextIndex ERROR " << error << endl;
  return -1;
}


