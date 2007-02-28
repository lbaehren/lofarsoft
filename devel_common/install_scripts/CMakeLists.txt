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
## Append the detected configuration information to the existing "makedefs"

set (outfile "makedefs")

file (APPEND ${outfile} "\n\n")
file (APPEND ${outfile} "#-----------------------------------------------------------\n")
file (APPEND ${outfile} "# Configuration settings detected by CMake\n")
file (APPEND ${outfile} "\n")


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
  /sw/bin
)

set (include_locations
  ./include
  /usr/include
  /usr/local/include
  /sw/include
)

set (lib_locations
  ./lib
  /opt/casa/local/lib
  /usr/lib
  /usr/local/lib
  /usr/X11R6/lib
  /sw/lib
)

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
    list (APPEND BOOST_LIBRARIES ${BOOST_${lib}})
  endif (BOOST_${lib})
endforeach (lib)

file (APPEND ${outfile} "BOOSTLIB := ${BOOST_LIBRARIES}\n\n")

## ------------------------------------------------------------------------------
## CFITSIO

find_library (libcfitsio cfitsio ${lib_locations})

file (APPEND ${outfile} "CFITSIOLIB := ${libcfitsio}\n\n")

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

if (liblapack AND libblas)
  set (LAPACKLIB "${liblapack} ${libblas}")
  if (libcblas)
    set (LAPACKLIB "${LAPACKLIB} ${libcblas}")
  endif (libcblas)
  if (libatlas)
    set (LAPACKLIB "${LAPACKLIB} ${libatlas}")
  endif (libatlas)
  file (APPEND ${outfile} "LAPACKLIB := ${LAPACKLIB}\n\n")
else (liblapack AND libblas)
  message (ERROR "Unable to locate liblapack & libblas - this is serious!")
endif (liblapack AND libblas)

## ------------------------------------------------------------------------------
## Motif

find_library (libXm Xm ${lib_locations})
find_library (libXpm Xpm ${lib_locations})
find_library (libXp Xp ${lib_locations})

file (APPEND ${outfile} "MotifLIB := ${libXm} ${libXpm} ${libXp}\n\n")

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

find_library (libpopt popt ${lib_locations})

file (APPEND ${outfile} "POPTLIB := ${libpopt}\n\n")

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
## Tcl/Tk (version 7.6 or later)

find_library (libtcl tcl ${lib_locations})
find_library (libdl dl ${lib_locations})
find_library (libtk tk ${lib_locations})

file (APPEND ${outfile} "TCLLIB := ${libtcl} ${libdl}\n\n")
file (APPEND ${outfile} "TKLIB  := ${libtk}\n\n")

## ------------------------------------------------------------------------------
## X11

find_library (libXt Xt ${lib_locations})
find_library (libXmu Xmu ${lib_locations})
find_library (libX11 X11 ${lib_locations})
find_library (libXext Xext ${lib_locations})

file (APPEND ${outfile} "X11LIB := ${libXt} ${libXmu} ${libX11} ${libXext}\n\n")

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
