/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TemporaryDirectory.h"
#include "ThreadContext.h"
#include "dirutil.h"
#include "Error.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

#if HAVE_PTHREAD
static ThreadContext* context = new ThreadContext;
#else
static ThreadContext* context = 0;
#endif

TemporaryDirectory::TemporaryDirectory (const std::string& basename)
{
  base = basename;
}

const string& TemporaryDirectory::get_directory () const
{
  ThreadContext::Lock lock (context);

  if (path.length() == 0)
    build ();

  return path;
}

void TemporaryDirectory::build () const
{
  string root = "/tmp/" + base;

  const char* unknown = "unknown";
  const char* userid = getenv ("USER");
  if (!userid)
    userid = unknown;

  path = (root + "/") + userid;

  if (makedir (path.c_str()) < 0)
  {
    cerr << "TemporaryDirectory: failed to create '" << path << "'" << endl;

    char* home = getenv ("HOME");

    if (home)
      path = home;
    else
      path = ".";

    path += "/" + base + ".tmp";

    if (makedir (path.c_str()) < 0)
      throw Error (InvalidState, "TemporaryDirectory",
		   "cannot create a temporary working path");
  }
  else
  {
    /* the first mkdir was successful; ensure that others can create
       temporary directories at the same root */

    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
    chmod (root.c_str(), mode);
  }
}
