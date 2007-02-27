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

## -------------------------------------------------------------------
## Append the detected configuration information to the existing "makedefs"

file (APPEND ../makedefs "\n\n")
file (APPEND ../makedefs "#-----------------------------------------------------------\n")
file (APPEND ../makedefs "# Configuration settings detected by CMake\n")
file (APPEND ../makedefs "\n")


## -------------------------------------------------------------------
## Define where to search for libraries and header files

set (bin_locations
  /bin
  /usr/sbin
  /usr/bin
  /usr/bin/X11
  /usr/local/sbin
  /usr/local/bin
  /sw/bin
)

set (include_locations
  /usr/include
  /usr/local/include
  /sw/include
)

set (lib_locations
  /opt/casa/local/lib
  /usr/lib
  /usr/local/lib
  /usr/X11R6/lib
  /sw/lib
)

## -------------------------------------------------------------------
## FFTW Highly optimized FFT routines

find_library (libfftw fftw ${lib_locations})
find_library (librfftw rfftw ${lib_locations})

if (libfftw)
  if (librfftw)
    file (APPEND ../makedefs "FFTWLIB := ${libfftw} ${librfftw}\n\n")
  else (librfftw)
    file (APPEND ../makedefs "FFTWLIB := ${libfftw}\n\n")
  endif (librfftw)
endif (libfftw)

## -------------------------------------------------------------------
## Motif

find_library (libXm Xm ${lib_locations})
find_library (libXpm Xpm ${lib_locations})
find_library (libXp Xp ${lib_locations})

file (APPEND ../makedefs "MotifLIB := ${libXm} ${libXpm} ${libXp}\n\n")

## -------------------------------------------------------------------
## POPT

find_library (libpopt popt ${lib_locations})

file (APPEND ../makedefs "POPTLIB  := ${libpopt}\n\n")

## -------------------------------------------------------------------
## TCL (version 7.6 or later)

find_library (libtcl tcl ${lib_locations})
find_library (libdl dl ${lib_locations})
find_library (libtk tk ${lib_locations})

file (APPEND ../makedefs "TCLLIB  := ${libtcl} ${libdl}\n\n")
file (APPEND ../makedefs "TKLIB   := ${libtk}\n\n")

## -------------------------------------------------------------------
## LAPACK (FORTRAN Linear Algebra Package).

find_library (liblapack lapack ${lib_locations})
find_library (libcblas cblas ${lib_locations})
find_library (libblas blas ${lib_locations})
find_library (libatlas atlas ${lib_locations})

if (liblapack AND libblas)
  set (LAPACKLIB "${LAPACKLIB} ${libblas}")
  if (cblas)
    set (LAPACKLIB "${LAPACKLIB} ${cblas}")
  endif (cblas)
  if (libatlas)
    set (LAPACKLIB "${LAPACKLIB} ${libatlas}")
  endif (libatlas)
  file (APPEND ../makedefs "LAPACKLIB   := ${LAPACKLIB}\n\n")
else (liblapack AND libblas)
  message (ERROR "Unable to locate liblapack & libblas - this is serious!")
endif (liblapack AND libblas)

#LAPACKLIB  :=-llapack -lblas -lcblas -latlas

## -------------------------------------------------------------------
## LaTeX applications

find_program (tex tex ${bin_locations})
find_program (latex latex ${bin_locations})
find_program (bibtex bibtex ${bin_locations})
find_program (dvips dvips ${bin_locations})
find_program (texindex texindex ${bin_locations})
find_program (makeindex makeindex ${bin_locations})
find_program (latex2html latex2html ${bin_locations})

file (APPEND ../makedefs "TEX           := ${tex}\n")
file (APPEND ../makedefs "LATEX         := ${latex}\n")
file (APPEND ../makedefs "BIBTEX        := ${bibtex}\n")
file (APPEND ../makedefs "DVIPS         := ${dvips}\n")
file (APPEND ../makedefs "TEXINDEX      := ${texindex}\n")
file (APPEND ../makedefs "MAKEINDEX     := ${makeindex}\n")
file (APPEND ../makedefs "LATEX2HTMLBIN := ${latex2html}\n")
