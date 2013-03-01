/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <vector>
#include <string>

namespace Pulsar {

  class Archive;
  class Profile;

  // A simple routine to select a standard profile from memory
  Profile* find_standard (const Archive*, const std::vector<Profile*>&);

  // A simple routine to select a standard profile from disk.
  // It assumes all standards have the .std extension.
  Profile* find_standard (const Archive*, const std::string& path);

}

