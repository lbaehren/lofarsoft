/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

#include "TemporaryFile.h"
#include "dirutil.h"
#include "Error.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

mode_t getumask ()
{
  mode_t mask = umask(0);
  umask(mask);
  return mask;
}

/*! To protect data, especially when writing the output archive to a
  file of the same name as the input archive, this method unloads data
  to a temporary file before renaming the archive to the requested
  output file name.  The temporary file will be written using a unique
  filename in the same path as the requested output file. */
void Pulsar::Archive::unload (const char* filename) const
{
  // run all verifications
  try
  {
    verify ();
  }
  catch (Error& error)
  {
    throw error += "Pulsar::Archive::unload";
  }

  string unload_to_filename = unload_filename;
  if (filename)
    unload_to_filename = expand(filename);

  if (verbose == 3)
    cerr << "Pulsar::Archive::unload (" << unload_to_filename << ")" << endl;

  TemporaryFile temp (unload_to_filename);

  Reference::To<Predictor> model_backup;

  if (model)
  {
    model_backup = model->clone();

    if (verbose > 2)
    {
      cerr << "Pulsar::Archive::unload paring down predictor=" << endl;
      model->unload (stderr);
    }

    Reference::To<Predictor> pared_down = model->clone();

    if (verbose > 2)
    {
      cerr << "Pulsar::Archive::unload paring down clone=" << endl;
      pared_down->unload (stderr);
    }

    vector<MJD> epochs( get_nsubint() );
    for (unsigned isub=0; isub < get_nsubint(); isub++)
      epochs[isub] = get_Integration(isub)->get_epoch();

    pared_down->keep (epochs);

    if (verbose > 2)
    {
      cerr << "Pulsar::Archive::unload pared down result=" << endl;
      pared_down->unload (stderr);
    }

    const_cast<Archive*>(this)->model = pared_down;
  }

  try
  {
    if (!can_unload())
    {
      Reference::To<Archive> output = Archive::new_Archive (unload_class);
      output-> copy (*this);
      output-> unload_file (temp.get_filename().c_str());
    }
    else
      unload_file (temp.get_filename().c_str());
  }
  catch (Error& error)
  {
    if (model_backup)
      const_cast<Archive*>(this)->model = model_backup;

    throw error += "Pulsar::Archive::unload";
  }

  if (model_backup)
    const_cast<Archive*>(this)->model = model_backup;

  // rename the temporary file with the requested filename
  int ret = rename (temp.get_filename().c_str(), unload_to_filename.c_str());

  if (ret < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed rename");

  temp.set_removed (true);

  ret = chmod (unload_to_filename.c_str(), 0666 & ~getumask());

  if (ret < 0 && verbose)
  {
    char temp[8];
    sprintf (temp, "%x", 0666 & ~getumask());
    cerr << "Pulsar::Archive::unload WARNING failed chmod ("
         << unload_to_filename << ", " << temp << ")" << endl;
  }

  const_cast<Archive*>(this)->unload_filename = unload_to_filename;
  const_cast<Archive*>(this)->__load_filename = unload_to_filename;
}
