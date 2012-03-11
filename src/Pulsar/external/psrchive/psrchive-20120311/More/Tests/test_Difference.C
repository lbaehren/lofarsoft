/***************************************************************************
 *
 *   Copyright (C) 2003 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Reference.h"

int main (int argc, char** argv)
{ try {

  // Pulsar::Archive::verbose = true;
  // Error::verbose = true;
  // Pulsar::Integration::verbose = true;
  
  Reference::To<Pulsar::Archive> archive1;
  Reference::To<Pulsar::Archive> archive2;
  Reference::To<Pulsar::Archive> diff_Archive;  
  
  
  string filename1;
  string filename2;
 
  if (argc == 1) {
   cerr << "test_Difference <archive1 2nd moment> <archive2 1st moment>" << endl;
   exit(EXIT_SUCCESS);
  } 

  if (argc>1) {
    filename1 = argv[1];
    cerr << "load archive from " << filename1 << endl;
    archive1 = Pulsar::Archive::load (filename1);
  }
  if (argc>2) {
    filename2 = argv[2];
    cerr << "load archive from " << filename2 << endl;
    archive2 = Pulsar::Archive::load (filename2);
  }

  archive1 -> dedisperse();
  archive1 -> centre();

  archive2 -> dedisperse();
  archive2 -> centre();

  Pulsar::Profile *profile1;
  Pulsar::Profile *profile2;
  
  float max_val1=0;
  float max_val2=0; 

  float * amps1 = NULL;
  float * amps2 = NULL;

  archive1->remove_baseline();

  for (int nchan = 0; nchan < archive2->get_nchan(); nchan++) {
      
      profile1 = archive1->get_Profile(0,0,nchan);
      profile2 = archive2->get_Profile(0,0,nchan);

      float mean_baseline = profile2->mean(profile2->find_min_phase());

      *profile2 -=(mean_baseline);

      profile2->operator*=(6*mean_baseline);

      *profile1 -= *profile2;

  }
  
  archive1 -> unload("difference.ar");
  
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
catch (string& error) {
  cerr << error << endl;
  return -1;
}
 
 return 0;
}
