
## Name of the project
PROJECT (slalib Fortran)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

file (GLOB sla_sources *.f)

add_library (sla ${sla_sources})

