/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "find_standard.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

using namespace std;

//
// Function to select a standard profile from a loaded list
//
Pulsar::Profile* Pulsar::find_standard (const Pulsar::Archive* data, 
					const vector<Profile*>& cans) {
  int selected = -1;
  float diff = 1e6;

  for (unsigned i = 0; i < cans.size(); i++) {
    float temp = fabs(data->get_centre_frequency() - 
		      cans[i]->get_centre_frequency());
    if (temp < diff) {
      diff = temp;
      selected = i;
    }
  }

  if (selected < 0) {
    throw Error(FailedCall, "Pulsar::find_standard",
		"No suitable profile found");
  }
  else {
    return cans[selected];
  }
}

//
// Function to select a standard profile from a set of filenames
//
Pulsar::Profile* Pulsar::find_standard (const Archive* data, 
					const string& path) {
  
  // Store the current working directory
  char* directory = new char[1024];
  getcwd(directory, 1024);
  
  if (chdir(path.c_str()) != 0) {
    chdir(directory);
    cerr << "No suitable standard profile found." << endl;
    return 0;
  }
  
  vector <string> the_stds;
  
  char temp[128];
  FILE* fptr = popen("ls -1 *.std", "r");
  if (ferror(fptr)==0) {
    while(fscanf(fptr, "%s\n", temp) == 1) {
      the_stds.push_back(temp);
    }
  }
  
  Reference::To <Archive> candidate = 0;
  
  for (unsigned i = 0; i < the_stds.size(); i++) {
    candidate = Archive::load("./" + the_stds[i]);
    candidate -> centre();
    if ((data -> get_source()) != (candidate -> get_source())) {
      candidate = 0;
      continue;
    }
    if ((data -> get_centre_frequency() != 
	 candidate -> get_centre_frequency())) {
      candidate = 0;
      continue;
    }
    candidate -> tscrunch();
    candidate -> pscrunch();
    candidate -> fscrunch();
    
    chdir(directory);
    
    cerr << "Selected standard profile: " << the_stds[i] << endl;
    
    return (candidate.release() -> get_Profile(0,0,0));
  }
  
  chdir(directory);
  cerr << "No standard profile available." << endl;
  return 0;
  
}
