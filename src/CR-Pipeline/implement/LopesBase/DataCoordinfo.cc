
#include <LopesBase/DataCoordinfo.h>

// =============================================================================
//
//  Construction / Destruction
//
// =============================================================================

DataCoordinfo::DataCoordinfo ()
{
  name_p = units_p = defAxis_p = domain_p = "UNDEFINED";
}

DataCoordinfo::~DataCoordinfo()
{
  stored_p = False;
}

// =============================================================================
//
//  Access to member data
//
// =============================================================================

String DataCoordinfo::name () { return name_p; }

void DataCoordinfo::setName (const String name) { name_p = name; }

String DataCoordinfo::units () { return units_p; }

void DataCoordinfo::setUnits (const String units) { units_p = units; }

String DataCoordinfo::defAxis () { return defAxis_p; }

void DataCoordinfo::setDefAxis (const String defAxis) { defAxis_p = defAxis; }

String DataCoordinfo::domain () { return domain_p; }

void DataCoordinfo::setDomain (const String domain) { domain_p = domain; }

Bool DataCoordinfo::stored () { return stored_p; }

void DataCoordinfo::setStored (const Bool stored) { stored_p = stored; }

String DataCoordinfo::ref () { return ref_p; }

void DataCoordinfo::setRef (const String ref) { ref_p = ref; }

String DataCoordinfo::type () { return type_p; }

void DataCoordinfo::setType (const String type) { type_p = type; }

String DataCoordinfo::genFunc () { return genFunc_p; }

void DataCoordinfo::setGenFunc (const String genFunc) { genFunc_p = genFunc; }
