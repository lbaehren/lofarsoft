#include "Pulsar/Check.h"

void Pulsar::Archive::Check::ensure_linkage ()
{
}

#include "Pulsar/CalSource.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::CalSource> calsource;

#include "Pulsar/Dedispersed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::Dedispersed> dedispersed;

#include "Pulsar/DeFaradayed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::DeFaradayed> deFaradayed;

