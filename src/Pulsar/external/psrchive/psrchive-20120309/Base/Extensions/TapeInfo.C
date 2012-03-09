/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TapeInfo.h"

//! Default constructor
Pulsar::TapeInfo::TapeInfo ()
  : Extension ("TapeInfo")
{
  file_number = -1;
  tape_label = "unknown";
}

//! Copy constructor
Pulsar::TapeInfo::TapeInfo (const TapeInfo& extension)
  : Extension ("TapeInfo")
{
  operator=(extension);
}

//! Operator =
const Pulsar::TapeInfo&
Pulsar::TapeInfo::operator= (const TapeInfo& extension)
{
  file_number = extension.file_number;
  tape_label = extension.tape_label;
  return *this;
}

//! Destructor
Pulsar::TapeInfo::~TapeInfo ()
{
}
