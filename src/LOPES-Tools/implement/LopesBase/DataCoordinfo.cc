
#include <LopesBase/DataCoordinfo.h>

namespace LOPES {  // Namespace LOPES -- begin
  
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
    stored_p = false;
  }
  
  // =============================================================================
  //
  //  Access to member data
  //
  // =============================================================================
  
  String DataCoordinfo::name () {
    return name_p;
  }
  
  void DataCoordinfo::setName (String const &name) {
    name_p = name;
  }
  
  String DataCoordinfo::units () {
    return units_p;
  }
  
  void DataCoordinfo::setUnits (String const &units) {
    units_p = units;
  }
  
  String DataCoordinfo::defAxis () {
    return defAxis_p;
  }
  
  void DataCoordinfo::setDefAxis (String const &defAxis) {
    defAxis_p = defAxis;
  }
  
  String DataCoordinfo::domain () {
    return domain_p;
  }
  
  void DataCoordinfo::setDomain (String const &domain) {
    domain_p = domain;
  }
  
  bool DataCoordinfo::stored () {
    return stored_p;
  }
  
  void DataCoordinfo::setStored (const bool stored) {
    stored_p = stored;
  }
  
  String DataCoordinfo::ref () {
    return ref_p;
  }
  
  void DataCoordinfo::setRef (String const &ref) {
    ref_p = ref;
  }
  
  String DataCoordinfo::type () {
    return type_p;
  }
  
  void DataCoordinfo::setType (String const &type) {
    type_p = type;
  }
  
  String DataCoordinfo::genFunc () {
    return genFunc_p;
  }
  
  void DataCoordinfo::setGenFunc (String const &genFunc) {
    genFunc_p = genFunc;
  }
  
}  // Namespace LOPES -- end
