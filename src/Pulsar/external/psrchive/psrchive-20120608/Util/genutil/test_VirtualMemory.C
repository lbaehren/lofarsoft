/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "VirtualMemory.h"
#include "dirutil.h"

#include <vector>
#include <iostream>

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

using namespace std;

int main (int argc, char** argv) try
{
  struct rlimit rlim;
  getrlimit (RLIMIT_AS, &rlim);
  if (rlim.rlim_cur == RLIM_INFINITY)
	cerr << "infinity = " << RLIM_INFINITY << endl;
  cerr << "RLIMIT_AS=" << rlim.rlim_cur << " " << rlim.rlim_cur << endl;

  cerr << endl <<
    "This program tests the maximum amount of virtual memory that can be\n"
	"managed by mmap.  It creates two very large files in the present working\n"
	"directory, which can be very painful if it is NFS mounted.\n"
	"Furthermore, on failure, the program may not clean up after itself.\n"
	"\n"
	"PRESS ENTER ONLY IF YOU ARE CERTAIN THAT YOU WOULD LIKE TO PROCEED\n"
	"\n"
	"Otherwise, Ctrl-C\n"
	<< endl;

  getchar();

  string path = ".";

  if (argc > 1)
	path = argv[1];

  string filename = path + "/test_VirtualMemory_swapspace";
  string filename2 = filename + "_2";
  string filename1 = filename + "_1";

  unsigned number_of_arrays = 1024 * 1024;
  unsigned array_size = 1024 * 8;

  {
    // disable unlinking the swap file so that it can be tested
    bool unlink_swapfile = false;

    VirtualMemory manager1 (filename1, unlink_swapfile);
    VirtualMemory manager2 (filename2, unlink_swapfile);

    // the filename is used as the base for a random filename
    filename = manager1.get_filename();

    cerr << "swap filename1=" << filename << endl;

    float* null = 0;
    vector<float*> arrays1 (number_of_arrays, null);
    vector<float*> arrays2 (number_of_arrays, null);

    float count = 0;
    for (unsigned i=0; i < number_of_arrays; i++)
    {
      arrays1[i] = manager1.create<float> (array_size);
	  arrays1[i][array_size-1] = count;

      arrays2[i] = manager2.create<float> (array_size);
	  arrays2[i][array_size-1] = count;

	  count ++;
    }

    uint64_t size = filesize (filename.c_str());
    uint64_t expected = number_of_arrays * array_size * sizeof(float);
    if (size < expected)
    {
      cerr << "swap space size=" << size << " < expected=" << expected << endl;
      return -1;
    }

    for (unsigned i=0; i < arrays1.size(); i++)
    {
      manager1.destroy (arrays1[i]);
	  manager2.destroy (arrays2[i]);
	}
  }

  if (file_exists (filename.c_str()))
  {
    cerr << "swap file exists after VirtualMemory instance passed out of scope"
	 << endl;
    return -1;
  }

  cerr << "VirtualMemory class passes tests" << endl;

  return 0;
}
catch (Error& error)
{
	  cerr << error << endl;
	  return -1;
}

