# ***************************************************************************
# *   Copyright (C) 2007                                                    *
# *   Lars Baehren (bahren@astron.nl)                                       *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# ***************************************************************************

## ------------------------------------------------------------------------------
## 
## === Introduction and Motivation ===
##
## The main purpose of this script is to help with the configuration step
## required to install CASA from source. By experience the main two bottlenecks
## in this process are
##  1. Installation of all required external packages (e.g. CFITSIO, PGPLOT, etc.)
##  2. Adjustment of the global configuration file ('makedefs')
## While - at least to a certain degree - the first point can behandled by a 
## package manager (such as e.g. apt-get, fink, etc.) or a set of installation
## scripts, the second task usually would be carried out using the GNU Autotools.
## However it is the latter step, which never has been addressed properly, thus
## requiring the user/system admin to manually set up the configuration files, 
## typically going through a cycle of editing and verification.
##
## === Approach to the problem ===
##
## Since for the 'makedefs' the task essentially is to locate libaries and header
## files for a set of packages, any suitable configuration tool can be utilized.
##
##  casa                           ..... AIPSROOT
##  |-- CMakeLists.txt
##  |-- <arch>                     ..... AIPSARCH
##  |   |-- aipsrc
##  |   |-- makedefs
##  |   |-- <site>                 ..... AIPSSITE
##  |   |   |-- aipsrc
##  |   |   |-- makedefs
##  |   |   `-- <host>             ..... AIPSHOST
##  |   |       `-- CMakeLists.txt
##  |   |-- bin
##  |   `-- lib
##  `-- code                       ..... AIPSCODE
##
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Output file to which the configuration is appended

set (outfile "../makedefs")

## ------------------------------------------------------------------------------
## Inspect the system we are working on

## possible variables to describe the architecture

set (AIPSARCH
  darwin
  linux
  linux_gnu
  )

## ------------------------------------------------------------------------------
## Define where to search for libraries and header files

set (bin_locations
  ./bin
  /bin
  /usr/sbin
  /usr/bin
  /usr/bin/X11
  /usr/local/sbin
  /usr/local/bin
  /opt
  /sw/bin
)

set (include_locations
  ./include
  ../../../local/include
  /opt/casa/local/include
  /usr/include
  /usr/local/include
  /opt
  /sw/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include
)

set (lib_locations
  ./lib
  ../../../local/lib
  /opt/casa/local/lib
  /usr/lib
  /usr/local/lib
  /usr/X11R6/lib
  /opt
  /sw/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
)

## ------------------------------------------------------------------------------
## Append the detected configuration information to the existing "makedefs"

file (APPEND ${outfile} "\n\n")
file (APPEND ${outfile} "#===========================================================\n")
file (APPEND ${outfile} "# Configuration settings detected by CMake\n")
file (APPEND ${outfile} "#\n")
file (APPEND ${outfile} "# Search paths for libraries: ${lib_locations}\n")
file (APPEND ${outfile} "# Search paths for headers  : ${include_locations}\n")
file (APPEND ${outfile} "\n")


## ------------------------------------------------------------------------------
## Blitz++

find_path (blitz_inc blitz.h ${include_locations}
  PATH_SUFFIXES blitz blitz++
  )

find_library (blitz_lib blitz ${lib_locations}
  PATH_SUFFIXES blitz blitz++
  )

file (APPEND ${outfile} "BLITZINCD := ${blitz_inc}\n")
file (APPEND ${outfile} "BLITZLIB  := ${blitz_lib}\n\n")

## ------------------------------------------------------------------------------
## Boost

set (libs
  boost_date_time
  boost_filesystem
  boost_iostreams
  boost_program_options
  boost_python
  boost_regex
  boost_serialization
  boost_signals
  boost_test_exec_monitor
  boost_thread
  boost_unit_test_framework
  boost_wave
)

set (BOOST_LIBRARIES "")

foreach (lib ${libs})
  ## try to locate the library
  find_library (BOOST_${lib} ${lib} ${lib}-gcc
    PATHS ${lib_locations}
    PATH_SUFFIXES boost
    )
  ## check if location was successful
  if (BOOST_${lib})
    set (BOOST_LIBRARIES "${BOOST_LIBRARIES} ${BOOST_${lib}}")
  endif (BOOST_${lib})
endforeach (lib)

file (APPEND ${outfile} "BOOSTLIB := ${BOOST_LIBRARIES}\n\n")

## ------------------------------------------------------------------------------
## CFITSIO
##
## - we should include a test here whether or not the library has been built with
##   large file support

find_path (cfitsio_inc fitsio.h
  PATHS ${include_locations}
  PATH_SUFFIXES cfitsio
  )

find_library (cfitsio_lib cfitsio
  PATH ${lib_locations}
  PATH_SUFFIXES cfitsio
  )

if (cfitsio_inc)
  file (APPEND ${outfile} "CFITSIOINCD := ${cfitsio_inc}\n")
else (cfitsio_inc)
  message (STATUS "Unable to find CFITSIO header files!!")
endif (cfitsio_inc)

if (cfitsio_lib)
  file (APPEND ${outfile} "CFITSIOLIB  := ${cfitsio_lib}\n\n")
else (cfitsio_lib)
  message (STATUS "Unable to find libcfitsio!!")
endif (cfitsio_lib)

## ------------------------------------------------------------------------------
## F77

find_library (libgcc
  NAMES gcc g2c
  PATHS ${lib_locations}
  PATH_SUFFIXES gcc/powerpc-apple-darwin8/4.0.0 gcc/powerpc-apple-darwin8/4.0.1
  )

if (libgcc)
  file (APPEND ${outfile} "F77LIB := ${libgcc}\n\n")
else (libgcc)
  message (STATUS "Unable to find libgcc!")
endif (libgcc)

## ------------------------------------------------------------------------------
## FFTW Highly optimized FFT routines

find_library (libfftw fftw ${lib_locations})
find_library (librfftw rfftw ${lib_locations})

if (libfftw)
  if (librfftw)
    file (APPEND ${outfile} "FFTWLIB := ${libfftw} ${librfftw}\n\n")
  else (librfftw)
    file (APPEND ${outfile} "FFTWLIB := ${libfftw}\n\n")
  endif (librfftw)
endif (libfftw)

## ------------------------------------------------------------------------------
## LAPACK (FORTRAN Linear Algebra Package).

find_library (liblapack lapack ${lib_locations})
find_library (libcblas cblas ${lib_locations})
find_library (libblas blas ${lib_locations})
find_library (libatlas atlas ${lib_locations})

if (liblapack)
  set (LAPACKLIB "${liblapack}")
else (liblapack)
  message (ERROR "Unable to locate liblapack - this is serious!")
endif (liblapack)

if (libblas)
  set (LAPACKLIB "${LAPACKLIB} ${libblas}")
endif (libblas)

if (libcblas)
  set (LAPACKLIB "${LAPACKLIB} ${libcblas}")
endif (libcblas)

if (libatlas)
  set (LAPACKLIB "${LAPACKLIB} ${libatlas}")
endif (libatlas)

file (APPEND ${outfile} "LAPACKLIB := ${LAPACKLIB}\n\n")

## ------------------------------------------------------------------------------
## Motif

find_path (Motif_inc Xm.h ${include_locations}
  PATH_SUFFIXES Xm
  )

find_library (libXm Xm ${lib_locations})
find_library (libXpm Xpm ${lib_locations})
find_library (libXp Xp ${lib_locations})

file (APPEND ${outfile} "MotifINCD := ${Motif_inc}\n")
file (APPEND ${outfile} "MotifLIB  := ${libXm} ${libXpm} ${libXp}\n\n")

## ------------------------------------------------------------------------------
## PGPLOT

find_library (libpng png ${lib_locations})
find_library (libpgplot pgplot ${lib_locations})
find_library (libcpgplot cpgplot ${lib_locations})

if (libpng)
  set (PGPLOTLIB "${libpng}")
endif (libpng)

if (libpgplot)
  set (PGPLOTLIB "${libpgplot} ${PGPLOTLIB}")
endif (libpgplot)

if (libcpgplot)
  set (PGPLOTLIB "${libcpgplot} ${PGPLOTLIB}")
endif (libcpgplot)

file (APPEND ${outfile} "PGPLOTLIB := ${PGPLOTLIB}\n\n")

## ------------------------------------------------------------------------------
## POPT

find_path (popt_inc popt.h ${include_locations})
find_library (popt_lib popt ${lib_locations})

file (APPEND ${outfile} "POPTINCD := ${popt_inc}\n")
file (APPEND ${outfile} "POPTLIB  := ${popt_lib}\n\n")

## ------------------------------------------------------------------------------
## Python

find_library (libpython
  NAMES python python2.4 python2.3
  PATHS ${lib_locations}
  PATH_SUFFIXES python2.4/config python2.3/config
  )

if (libpython)
  file (APPEND ${outfile} "PYTHONLIB := ${libpython}\n\n")
else (libpython)
  message (ERROR "Unable to locate PYTHONLIB!")
endif (libpython)

## ------------------------------------------------------------------------------
## Root (optional)

find_path (ROOT_INCLUDES tCanvas.h TCint.h TObject.h
  PATHS ${include_locations}
  PATH_SUFFIXES root root/include
  )

set (ROOT_libs
  Core
  Graf
  Gui
  Hist
  MathCore
  Matrix
  Minuit
  Physics
  Proof
  PyROOT
  Tree
  )

foreach (lib ${ROOT_libs})
  ## try to locate the library
  find_library (ROOT_${lib} ${lib}
    PATHS ${lib_locations}
    PATH_SUFFIXES root root/lib
    )
  ## check if location was successful
  if (ROOT_${lib})
    set (ROOT_LIBRARIES "${ROOT_LIBRARIES} ${ROOT_${lib}}")
  endif (ROOT_${lib})
endforeach (lib)

file (APPEND ${outfile} "ROOTINCD := ${ROOT_INCLUDES}\n")
file (APPEND ${outfile} "ROOTLIB  := ${ROOT_LIBRARIES}\n\n")

## ------------------------------------------------------------------------------
## SOFA

find_library (libsofa sofa ${lib_locations})

if (libsofa)
  file (APPEND ${outfile} "SOFALIB := ${libsofa}\n\n")
endif (libsofa)

## ------------------------------------------------------------------------------
## Tcl/Tk (version 7.6 or later)

find_path (tcl_inc tcl.h ${include_locations} PATH_SUFFIXES tcl8.4 tcl8.3)
find_path (tk_inc tk.h ${include_locations} PATH_SUFFIXES tcl8.4 tcl8.3)

find_library (libtcl tcl ${lib_locations} PATH_SUFFIXES tcl8.4 tcl8.3)
find_library (libdl dl ${lib_locations} PATH_SUFFIXES tcl8.4 tcl8.3)
find_library (libtk tk ${lib_locations} PATH_SUFFIXES tcl8.4 tcl8.3)

file (APPEND ${outfile} "TCLINCD := ${tcl_inc}\n")
file (APPEND ${outfile} "TCLLIB  := ${libtcl} ${libdl}\n\n")

file (APPEND ${outfile} "TKINCD  := ${tk_inc}\n")
file (APPEND ${outfile} "TKLIB   := ${libtk}\n\n")

## ------------------------------------------------------------------------------
## X11

find_path (X_inc X.h ${include_locations}
  PATH_SUFFIXES X11
  )

find_library (libXt Xt ${lib_locations})
find_library (libXmu Xmu ${lib_locations})
find_library (libX11 X11 ${lib_locations})
find_library (libXext Xext ${lib_locations})

file (APPEND ${outfile} "X11INCD := ${X_inc}\n")
file (APPEND ${outfile} "X11LIB  := ${libXt} ${libXmu} ${libX11} ${libXext}\n\n")

find_library (libXpm Xpm ${lib_locations})

file (APPEND ${outfile} "XPMLIB  := ${libXpm}\n")
file (APPEND ${outfile} "XPMINCD := ${X_inc}\n\n")

## ------------------------------------------------------------------------------
## LaTeX applications

find_program (tex tex ${bin_locations})
find_program (latex latex ${bin_locations})
find_program (bibtex bibtex ${bin_locations})
find_program (dvips dvips ${bin_locations})
find_program (texindex texindex ${bin_locations})
find_program (makeindex makeindex ${bin_locations})
find_program (latex2html latex2html ${bin_locations})

file (APPEND ${outfile} "TEX           := ${tex}\n")
file (APPEND ${outfile} "LATEX         := ${latex}\n")
file (APPEND ${outfile} "BIBTEX        := ${bibtex}\n")
file (APPEND ${outfile} "DVIPS         := ${dvips}\n")
file (APPEND ${outfile} "TEXINDEX      := ${texindex}\n")
file (APPEND ${outfile} "MAKEINDEX     := ${makeindex}\n")
file (APPEND ${outfile} "LATEX2HTMLBIN := ${latex2html}\n")
