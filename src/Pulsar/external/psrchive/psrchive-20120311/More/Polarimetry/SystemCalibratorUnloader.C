/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibratorUnloader.h"
#include "Pulsar/Archive.h"
#include "strutil.h"

using namespace std;

Pulsar::SystemCalibrator::Unloader::Unloader ()
{
  archive_class = "PSRFITS";
}

void Pulsar::SystemCalibrator::Unloader::set_archive_class (const string& name)
{
  archive_class = name;
}

void Pulsar::SystemCalibrator::Unloader::set_program (const string& name)
{
  program = name;
}

void Pulsar::SystemCalibrator::Unloader::set_filename (const string& name)
{
  filename = name;
}

void Pulsar::SystemCalibrator::Unloader::set_extension (const string& ext)
{
  extension = ext;
}

void Pulsar::SystemCalibrator::Unloader::set_filename (const Archive* archive,
						       unsigned subint)
{
  string new_ext = stringprintf ("%04u.tmp", subint);

  if (extension.size())
    new_ext = replace_extension (new_ext, extension);

  filename = replace_extension (archive->get_filename(), new_ext);
}

void Pulsar::SystemCalibrator::Unloader::unload (SystemCalibrator* model)
{
  if (filename.empty())
    throw Error (InvalidState, "Pulsar::SystemCalibrator::Unloader::unload",
		 "filename not set");

  Reference::To<Archive> solution = model->new_solution (archive_class);

  if (program.size())
    cerr << program << ": unloading solution to " << filename << endl;

  solution->unload( filename );
}
