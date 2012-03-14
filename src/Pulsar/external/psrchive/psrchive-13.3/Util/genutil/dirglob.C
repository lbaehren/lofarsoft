/***************************************************************************
 *
 *   Copyright (C) 2000-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "dirutil.h"
#include "Error.h"

#include <iostream>

#include <glob.h>
#include <string.h>

using namespace std;

string dirglob_program;

bool is_glob_argument (const char* text)
{
  if (strchr (text, '?'))
    return true;
  if (strchr (text, '*'))
    return true;
  if (strchr (text, '['))
    return true;

  return false;
}

bool is_glob_argument (const string& text)
{
  return is_glob_argument (text.c_str());
}
 
void dirglob (vector<string>* filenames, const char* text)
{
  glob_t rglob;

  int ret = glob (text, GLOB_NOSORT, NULL, &rglob);
  if (ret != 0 
#ifdef GLOB_NOMATCH
      && ret != GLOB_NOMATCH
#endif
              )
    throw Error (FailedSys, "dirglob", "error calling glob");

  if (!dirglob_program.empty() && rglob.gl_pathc == 0)
    cerr << dirglob_program << ": '" << text << "' not found" << endl;

  for (size_t ifile=0; ifile < rglob.gl_pathc; ifile++)
    filenames->push_back (string(rglob.gl_pathv[ifile]));

  globfree (&rglob);
}

void dirglob (vector<string>* filenames, const string& text)
{
  dirglob (filenames, text.c_str());
}

