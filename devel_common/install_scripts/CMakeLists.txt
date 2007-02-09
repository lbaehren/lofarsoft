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

find_path (CFITSIOINCD fitsio.h ${inc_locations})
find_library (libcfitsio cfitsio ${lib_locations})

file (APPEND makedefs "CFITSIOINCD := ${CFITSIOINCD}\n")
file (APPEND makedefs "CFITSIOLIB  := ${libcfitsio}\n")
file (APPEND makedefs "\n")

## FFTW3 -------------------------------------------------------------

find_path (FFTWINCD fftw.h fftw3.h PATHS ${inc_locations})
find_library (libfftw fftw fftw3 PATHS ${lib_locations})
find_library (libfftwf fftwf fftw3f PATHS ${lib_locations})

file (APPEND makedefs "FFTWINCD := ${FFTWINCD}\n")
file (APPEND makedefs "FFTWLIB  := ${libfftw} ${libfftwf}\n")
file (APPEND makedefs "\n")

## LAPACK ------------------------------------------------------------

find_library (libblas blas PATHS ${lib_locations})
find_library (libcblas cblas PATHS ${lib_locations})
find_library (liblapack lapack PATHS ${lib_locations})

file (APPEND makedefs "LAPACKLIB := ${libblas} ${libcblas} ${liblapack}\n")
file (APPEND makedefs "\n")

## PGPLOT ------------------------------------------------------------

find_path (PGPLOTINCD cpgplot.h ${inc_locations} PATH_SUFFIXES pgplot)
find_library (libpgplot pgplot ${lib_locations} PATH_SUFFIXES pgplot)

file (APPEND makedefs "PGPLOTINCD := ${PGPLOTINCD}\n")
file (APPEND makedefs "PGPLOTLIB  := ${libpgplot}\n")
file (APPEND makedefs "\n")

## Tcl ---------------------------------------------------------------

find_path (TCLINCD tcl.h PATHS ${inc_locations})
find_library (libtcl tcl tcl8.4 tcl8.3 ${inc_locations})
find_library (libdl dl ${inc_locations})

file (APPEND makedefs "TCLINCD := ${TCLINCD}\n")
file (APPEND makedefs "TCLLIB  := ${libtcl} ${libdl}\n")
file (APPEND makedefs "\n")

## Tk ----------------------------------------------------------------

find_path (TKINCD tk.h PATHS ${inc_locations})
find_library (libtk tk tk8.4 tk8.3 ${inc_locations})

file (APPEND makedefs "TKINCD  := ${TKINCD}\n")
file (APPEND makedefs "TKLIB   := ${libtk}\n")
file (APPEND makedefs "\n")
