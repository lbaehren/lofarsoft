# $Id: CMakeLists.txt 16712 2010-11-15 10:07:06Z schoenmakers $

install(PROGRAMS 
  createFiles 
  DESTINATION sbin)

# These files end up in ${prefix}/etc
file(GLOB sysconf_data *.conf)
install(FILES
  ${sysconf_data}
  DESTINATION etc)

# These files end up in ${prefix}/etc/StaticMetaData
file(GLOB staticmeta_data 
  *.tmpl 
  *.test 
  *.dat 
  AntennaArrays/*.conf
  AntennaPos/*.conf
  CableDelays/*.conf
  HBADeltas/*.conf
  iHBADeltas/*.conf
  AntennaFields/*.conf
  CalTables/*.dat)
install(FILES
  ${staticmeta_data}
  DESTINATION etc/StaticMetaData)
