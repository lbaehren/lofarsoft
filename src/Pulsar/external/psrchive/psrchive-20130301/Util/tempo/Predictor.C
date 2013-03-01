/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Predictor.h"
#include "FilePtr.h"

void Pulsar::Predictor::load_file (const std::string& filename)
{
  FilePtr fptr ( filename, "r" );
  load (fptr);
}

void Pulsar::Predictor::unload_file (const std::string& filename) const
{
  FilePtr fptr ( filename, "w" );
  unload (fptr);
}

