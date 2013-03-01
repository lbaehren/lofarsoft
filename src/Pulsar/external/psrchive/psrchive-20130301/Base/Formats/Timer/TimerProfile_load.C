/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"
#include "fcomp.h"

#include <stdio.h>

using namespace std;

/*!
  \pre The Profile must have been resized before calling this function
*/
void Pulsar::TimerProfile_load (FILE* fptr, Profile* profile, 
				bool big_endian)
{
  float centrefreq;      /* centre frequency of profile MHz */
  float wt;              /* weight of profile               */
  int   poln;	         /* 0=I, 1=LL, 2=RR, 3=LR, 4=RL, 5=Q, 6=U, 7=V */
  int   nbin;

  if (Profile::verbose)
    cerr << "Pulsar::TimerProfile_load start offset=" << ftell(fptr) << "\r";

  //Read in the centre frequency
  if (fread(&centrefreq,sizeof(centrefreq),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread centrefreq");
  //Number of bins
  if (fread(&nbin,sizeof(nbin),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread nbin");
  // Poln
  if (fread(&poln,sizeof(poln),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread poln");
  // Weight
  if (fread(&wt,sizeof(wt),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread wt");
 
  if (big_endian) {
    if (Profile::verbose) 
      cerr << "TimerProfile_load big endian\n";
    fromBigEndian(&centrefreq,sizeof(centrefreq));
    fromBigEndian(&nbin,sizeof(nbin));
    fromBigEndian(&poln,sizeof(poln));
    fromBigEndian(&wt,sizeof(wt));
  }
  else {
    if (Profile::verbose)
      cerr << "TimerProfile_load little endian\n";
    fromLittleEndian(&centrefreq,sizeof(centrefreq));
    fromLittleEndian(&nbin,sizeof(nbin));
    fromLittleEndian(&poln,sizeof(poln));
    fromLittleEndian(&wt,sizeof(wt));
  }

  if (unsigned(nbin) != profile->get_nbin())
    throw Error (InvalidState, "TimerProfile_load",
		 "from file nbin=%d != Profile::nbin=%d", 
                 nbin, profile->get_nbin());

  if (Profile::verbose)
    cerr << "Pulsar::TimerProfile_load nbin=" << nbin << " poln=" << poln
         << "wt=" << wt << " cfreq=" << centrefreq << "\r";

  // uncompress the data and read in as 2byte integers
  if (Profile::no_amps)
    fseek(fptr, sizeof(float)*2+sizeof(unsigned short int)*nbin, SEEK_CUR);
  else if (fcompread (nbin,profile->get_amps(),fptr,big_endian)!=0)
    throw Error (FailedCall, "TimerProfile_load", "fcompread data");

  if (Profile::verbose)
    cerr << "Pulsar::TimerProfile_load end offset=" << ftell(fptr) << "\r";

  profile -> set_weight (wt);
  profile -> set_centre_frequency (centrefreq);

}
