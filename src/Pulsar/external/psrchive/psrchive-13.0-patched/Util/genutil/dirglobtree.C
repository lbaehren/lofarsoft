/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "dirutil.h"

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>

using namespace std;

void dirglobtree (vector<string>* filenames, 
		  const string& root, const string& pattern)
{
  vector<string> patterns (1, pattern);
  dirglobtree (filenames, root, patterns);
}

void dirglobtree (vector<string>* filenames, 
		  const string& root, const vector<string>& patterns)
{
  string path = root;
  if (path.length())
    path += "/";

  for (unsigned ip=0; ip<patterns.size(); ip++)
    dirglob (filenames, path + patterns[ip]);

  string current = root;
  if (!root.length())
    current = ".";

  DIR* dird = opendir (current.c_str());
  if (dird == NULL) {
    fprintf (stderr, "dirglobtree: Error opening directory:'%s'",
	     current.c_str());
    perror ("");
    return;
  }

  struct dirent *entry;
 
  while ((entry = readdir (dird)) != NULL) {
    string name (entry->d_name);
    string next = path + name;
    if (name != "." && name != ".." && file_is_directory (next.c_str()))
      dirglobtree (filenames, next, patterns);
  }

  closedir (dird);
}
