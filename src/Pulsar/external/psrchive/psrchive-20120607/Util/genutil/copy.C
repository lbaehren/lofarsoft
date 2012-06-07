/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "strutil.h"
#include "Error.h"

#include <vector>
using namespace std;

void copy (FILE* from, FILE* to, ssize_t nbytes)
{
  vector<char> buffer (256);

  // cerr << "nbytes=" << nbytes << endl;

  while (!feof(from) && (nbytes != 0)) {

    size_t to_read = buffer.size();
    if (nbytes > 0 && unsigned(nbytes) < buffer.size())
      to_read = nbytes;

    // cerr << "to read=" << to_read << endl;

    size_t did_read = fread (&buffer[0], 1, to_read, from);
    if (did_read < 0)
      throw Error (FailedSys, "copy" "fread");

    nbytes -= did_read;

    // cerr << "nbytes=" << nbytes << endl;

    size_t did_write = fwrite (&buffer[0], 1, did_read, to);
    if (did_write < did_read)
      throw Error (FailedSys, "copy" "fwrite");
    
  }
}
