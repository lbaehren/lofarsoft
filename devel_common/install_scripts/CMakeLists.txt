#/***************************************************************************
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
# ***************************************************************************/

## ------------------------------------------------------------------------
## CMake script for generation of host-specific "makedefs" to configure
## AIPS++/CASA
##
## Due to the fact that system "makedefs" need to manually configured, the
## installation of CASA can become rather time-consuming. The task to be 
## be performed however is what commonly is done by a configuration script.
## In order to simplify this step, we can use e.g. CMake to figure out most
## of the settings concerning external dependencies - the result is written
## into the host-specific "makedefs" thereby keeping site-specific settings.

file (WRITE makedefs "## Host-specific makedefs for CASA\n\n")

set (inc_locations
  /include
  /usr/include
  /usr/local/include
  /opt/include
  /opt/local/include
  /sw/include
  )

set (lib_locations
  /lib
  /usr/lib
  /usr/local/lib
  /opt/lib
  /opt/local/lib
  /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
  /sw/lib
  )

## CFITSIO -----------------------------------------------------------

message (STATUS "Checking for CFITSIO...")

find_path (CFITSIOINCD fitsio.h ${inc_locations})
find_library (libcfitsio cfitsio ${lib_locations})

file (APPEND makedefs "CFITSIOINCD := ${CFITSIOINCD}\n")
file (APPEND makedefs "CFITSIOLIBD := \n")
file (APPEND makedefs "CFITSIOLIB  := ${libcfitsio}\n")
file (APPEND makedefs "\n")

## FFTW3 -------------------------------------------------------------

message (STATUS "Checking for FFTW...")

find_path (FFTWINCD fftw.h fftw3.h PATHS ${inc_locations})
find_library (libfftw fftw fftw3 PATHS ${lib_locations})
find_library (libfftwf fftwf fftw3f PATHS ${lib_locations})

file (APPEND makedefs "FFTWINCD := ${FFTWINCD}\n")
file (APPEND makedefs "FFTWLIBD := \n")
file (APPEND makedefs "FFTWLIB  := ${libfftw} ${libfftwf}\n")
file (APPEND makedefs "\n")

## LAPACK ------------------------------------------------------------

message (STATUS "Checking for LAPACK...")

find_library (libatlas atlas PATHS ${lib_locations})
find_library (libblas blas PATHS ${lib_locations})
find_library (libcblas cblas PATHS ${lib_locations})
find_library (liblapack lapack PATHS ${lib_locations})

file (APPEND makedefs "LAPACKINCD := \n")
file (APPEND makedefs "LAPACKLIBD := \n")
file (APPEND makedefs "LAPACKLIB  := ${liblapack} ${libcblas} ${libblas} ${libatlas}\n")
file (APPEND makedefs "\n")

## Motif -------------------------------------------------------------

message (STATUS "Checking for MotifLIB...")

find_path (MotifINCD
  NAMES Xm.h XmP.h
  PATHS ${inc_locations}
  PATH_SUFFIXES Xm
  )

find_library (libXm Xm PATHS ${lib_locations})
find_library (libXpm Xpm PATHS ${lib_locations})
find_library (libXp Xp PATHS ${lib_locations})

file (APPEND makedefs "MotifINCD := ${MotifINCD}\n")
file (APPEND makedefs "MotifLIB  := ${libXm} ${libXpm} ${libXp}\n")
file (APPEND makedefs "\n")

## PGPLOT ------------------------------------------------------------

message (STATUS "Checking for PGPLOT...")

find_path (PGPLOTINCD cpgplot.h ${inc_locations} PATH_SUFFIXES pgplot)
find_library (libpgplot pgplot ${lib_locations} PATH_SUFFIXES pgplot)
find_library (libcpgplot cpgplot ${lib_locations} PATH_SUFFIXES pgplot)
find_library (libpng png ${lib_locations})

file (APPEND makedefs "PGPLOTINCD := ${PGPLOTINCD}\n")
file (APPEND makedefs "PGPLOTLIBD := \n")
file (APPEND makedefs "PGPLOTLIB  := ${libcpgplot} ${libpgplot} ${libpng}\n")
file (APPEND makedefs "\n")

## Python ------------------------------------------------------------

message (STATUS "Checking for PYTHON...")

find_path (PYTHONINCD 
  NAMES Python.h
  PATHS ${inc_locations}
  PATH_SUFFIXES python2.5 python2.5/config
  python2.4 python2.4/config
  python2.3 python2.3/config
  )

find_library (libpython
  NAMES python2.5 python2.4 python2.3
  PATHS ${lib_locations}
  PATH_SUFFIXES python2.5/config python2.4/config python2.3/config
  )

file (APPEND makedefs "PYTHONINCD := ${PYTHONINCD}\n")
file (APPEND makedefs "PYTHONLIBD := \n")
file (APPEND makedefs "PYTHONLIB  := ${libpython}\n")
file (APPEND makedefs "\n")

## Tcl ---------------------------------------------------------------

message (STATUS "Checking for TCL...")

find_path (TCLINCD
  NAMES tcl.h
  PATHS ${inc_locations}
  PATH_SUFFIXES tcl tcl8.4 tcl8.3
  )
find_library (libtcl tcl tcl8.4 tcl8.3 ${inc_locations} )
find_library (libdl dl ${inc_locations})

file (APPEND makedefs "TCLINCD := ${TCLINCD}\n")
file (APPEND makedefs "TCLLIBD := \n")
file (APPEND makedefs "TCLLIB  := ${libtcl} ${libdl}\n")
file (APPEND makedefs "\n")

## Tk ----------------------------------------------------------------

message (STATUS "Checking for TK...")

find_path (TKINCD
  NAMES tk.h
  PATHS ${inc_locations}
  PATH_SUFFIXES tcl tcl8.4 tcl8.3 tk tk8.4 8.3
  )
find_library (libtk tk tk8.4 tk8.3 ${inc_locations})

file (APPEND makedefs "TKINCD := ${TKINCD}\n")
file (APPEND makedefs "TKLIBD := \n")
file (APPEND makedefs "TKLIB  := ${libtk}\n")
file (APPEND makedefs "\n")

## X11 ---------------------------------------------------------------

message (STATUS "Checking for X11...")

find_path (X11INCD
  NAMES Xlib.h Xdef.h
  PATHS ${inc_locations} /usr/X11R6/include
  PATH_SUFFIXES X11
  )

find_library (libXt Xt PATHS ${lin_locations} /usr/X11R6/lib)
find_library (libXmu Xmu PATHS ${lin_locations} /usr/X11R6/lib)
find_library (libX11 X11 PATHS ${lin_locations} /usr/X11R6/lib)
find_library (libXext Xext PATHS ${lin_locations} /usr/X11R6/lib)

file (APPEND makedefs "X11INCD := ${X11INCD}\n")
file (APPEND makedefs "X11LIBD := \n")
file (APPEND makedefs "X11LIB  := ${libXt} ${libXmu} ${libX11} ${libXext}\n")
file (APPEND makedefs "\n")
