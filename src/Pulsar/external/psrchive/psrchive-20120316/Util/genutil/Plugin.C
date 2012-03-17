/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Plugin.h"
#include "dirutil.h"

#include <iostream>
#include <dlfcn.h>

using namespace std;

bool Registry::Plugin::verbose = false;

void Registry::Plugin::load (const char* path)
{
  if (verbose)
    cerr << "Registry::Plugin::load path = '" << path << "'" << endl;

  vector<string> filenames;

  string globarg = path;
  globarg += "/*.so";

  dirglob (&filenames, globarg);

  ok.clear();
  fail.clear();

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {

    if (verbose)
      cerr << "Registry::Plugin::load dlopen("<< filenames[ifile] <<")"<< endl;

    void* dLibrary = dlopen(filenames[ifile].c_str(), RTLD_GLOBAL | RTLD_NOW);

    if (dLibrary == NULL)  {

      string reason = dlerror();
      fail.push_back (reason);

      if (verbose)
	cerr << "Registry::Plugin::load failed dlopen"
	  " (" << filenames[ifile] << "):" << reason << endl;
    }
    else
      ok.push_back (filenames[ifile]);
  }

}
