# $Id$

lofar_package(Pipeline-MAC 1.0 DEPENDS APLCommon)

lofar_find_package(Boost REQUIRED COMPONENTS python)
lofar_find_package(Python REQUIRED)

#add_subdirectory(include)
add_subdirectory(src)
