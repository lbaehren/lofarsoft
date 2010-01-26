/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#define PSRCHIVE_PLUGIN
#include "Pulsar/Agent.h"

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

using namespace Pulsar;
using namespace std;

Registry::List<Archive::Agent> Archive::Agent::registry;

#include "Pulsar/TimerArchive.h"

template
Registry::List<Archive::Agent>::Enter<TimerArchive::Agent>
Archive::Advocate<TimerArchive>::entry;

#include "Pulsar/BasebandArchive.h"

template
Registry::List<Archive::Agent>::Enter<BasebandArchive::Agent>
Archive::Advocate<BasebandArchive>::entry;

#include "Pulsar/EPNArchive.h"

template
Registry::List<Archive::Agent>::Enter<EPNArchive::Agent>
Archive::Advocate<EPNArchive>::entry;

#ifdef HAVE_CFITSIO

#include "Pulsar/ASPArchive.h"

template
Registry::List<Archive::Agent>::Enter<ASPArchive::Agent>
Archive::Advocate<ASPArchive>::entry;

#include "Pulsar/FITSArchive.h"
template
Registry::List<Archive::Agent>::Enter<FITSArchive::Agent>
Archive::Advocate<FITSArchive>::entry;

#endif

#ifdef HAVE_PUMA

#include "Pulsar/PuMaArchive.h"

template
Registry::List<Archive::Agent>::Enter<PuMaArchive::Agent>
Archive::Advocate<PuMaArchive>::entry;

#endif

#include "Pulsar/WAPPArchive.h"

template
Registry::List<Archive::Agent>::Enter<WAPPArchive::Agent>
Archive::Advocate<WAPPArchive>::entry;

#include "Pulsar/PRESTOArchive.h"

template
Registry::List<Archive::Agent>::Enter<PRESTOArchive::Agent>
Archive::Advocate<PRESTOArchive>::entry;

#include "Pulsar/BPPArchive.h"

template
Registry::List<Archive::Agent>::Enter<BPPArchive::Agent>
Archive::Advocate<BPPArchive>::entry;

#include "Pulsar/ASCIIArchive.h"

template
Registry::List<Archive::Agent>::Enter<ASCIIArchive::Agent>
Archive::Advocate<ASCIIArchive>::entry;



Pulsar::Option<string> Pulsar::Archive::unload_class
(
 "Archive::unload_class",

#ifdef HAVE_CFITSIO
 "PSRFITS",
#else
 "Timer",
#endif

 "File format used if unload is not implemented",

 "If the file format used to load the data does not implement an unload \n"
 "method, then the data will be converted to the name file format."
);
