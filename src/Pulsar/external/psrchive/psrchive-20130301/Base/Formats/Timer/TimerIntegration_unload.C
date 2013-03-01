/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Pointing.h"
#include "Error.h"

#include "convert_endian.h"
#include <assert.h>

using namespace std;

template<typename T>
void unload_big_endian (FILE* fptr, unsigned size, const std::vector<T>& data)
{
  assert (size <= data.size());
 
  T* ptr = const_cast<T*>( &(data[0]) );

  N_ToBigEndian (size, ptr);
  fwrite (ptr, size*sizeof(T), 1, fptr);
  N_FromBigEndian (size, ptr);
}

//! unload the subint to file
void Pulsar::TimerIntegration::unload (FILE* fptr) const
{
  if (verbose) 
    cerr << "TimerIntegration::unload epoch=" << get_epoch() 
         << " period=" << mini.pfold << endl;

  pack_Pointing ();

  struct mini* outmini = const_cast<struct mini*>(&mini);

  mini_toBigEndian(outmini);
  if (fwrite (outmini, 1, sizeof(mini), fptr) < 1)
    throw Error (FailedSys, "TimerIntegration::unload", "fwrite mini");
  mini_fromBigEndian(outmini);

  if (verbose) cerr << "Pulsar::TimerIntegration::unload extra start offset="
                    << ftell(fptr) << endl;

  if (verbose) cerr << "TimerIntegration::unload writing " << nchan
                    << " weights" << endl;

  unload_big_endian (fptr, nchan, wts);
 
  if (verbose) cerr << "TimerIntegration::unload writing " << npol
                    << "x" << nchan << " med" << endl;

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    assert (ipol < med.size());
    unload_big_endian (fptr, nchan, med[ipol]);
  }

  if (verbose) cerr << "TimerIntegration::unload writing " << npol
                    << "x" << nchan << " bpass" << endl; 

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    assert (ipol < bpass.size());
    unload_big_endian (fptr, nchan, bpass[ipol]);
  }

  if (verbose) cerr << "Pulsar::TimerIntegration::unload extra end offset="
                    << ftell(fptr) << endl;

  // And the profiles
  if(verbose) cerr << "TimerIntegration::unload unloading profiles npol=" 
		   << npol << " nchan=" << nchan << endl;

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    for (unsigned ichan=0; ichan<nchan; ichan++) try
    {
      if (Profile::verbose) cerr << "TimerIntegration::unload ipol=" << ipol 
                                 << "\t ichan=" << ichan << endl;
	
      TimerProfile_unload (fptr, profiles[ipol][ichan], ipol);
    }
    catch (Error& error)
    {
      error << "\n\tprofile[ipol=" << ipol << "][ichan=" << ichan << "]";
      throw error += "TimerIntegration::unload";
    }
  }

  if (verbose) cerr << "TimerIntegration::unload exit" << endl;
}

void Pulsar::TimerIntegration::pack_Pointing () const
{
  Reference::To<const Pointing> pointing = get<Pointing>();
  if (!pointing)
    return;

  struct mini* outmini = const_cast<struct mini*>(&mini);

  outmini->lst_start = pointing->get_local_sidereal_time();

  outmini->feed_ang = pointing->get_feed_angle().getDegrees();

  outmini->para_angle = pointing->get_parallactic_angle().getDegrees();

  outmini->tel_az = pointing->get_telescope_azimuth().getDegrees();

  outmini->tel_zen = pointing->get_telescope_zenith().getDegrees();
}
