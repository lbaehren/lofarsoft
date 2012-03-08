#include "Pulsar/Check.h"

// Checks registered for use in Archive::load and Archive::unload
Registry::List<Pulsar::Archive::Check> Pulsar::Archive::Check::registry;

#include "Pulsar/CalSource.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::CalSource> calsource;

#include "Pulsar/Dedispersed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::Dedispersed> dedispersed;

#include "Pulsar/DeFaradayed.h"
Registry::List<Pulsar::Archive::Check>::Enter<Pulsar::DeFaradayed> deFaradayed;

