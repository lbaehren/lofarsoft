
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 724 2007-09-10 10:17:21Z baehren                        $
##-------------------------------------------------------------------------------
##
## A simple shell script to configure and install as many as possible of
## the packages in the LOFAR USG distribution. Simply run
## 
##   ./build.sh <package> [options]
##
## from within the build directory - and the script should take care of
## the rest.
##

basedir=`pwd`
HOSTNAME=`hostname -s`
SYSTEM_NAME=`uname -s`
SYSTEM_CPU=`uname -p`
FORCE_BUILD=0;
CLEAN_BUILD=0;
REPORT_BUILD=0;
BUILD_TESTS=0;
ENABLE_MULTITHREADING=1;

## Required minimum version of CMake; system should provide a version at
## at least matching the one provided as part of the USG distribution

REQUIRED_MAJOR_VERSION=2
REQUIRED_MINOR_VERSION=6

##______________________________________________________________________________
## Environment variables; in particular we need to ensure, the CMake binary can 
## be found. However failure in that could also signify, that there is no CMake
## available, so we need to make to sure we build it first.

## move one directory up to get a clean result when setting LOFARSOFT

cd $basedir/..
export LOFARSOFT=`pwd`

## check if $LOFARSOFT/release/bin is in the PATH already; if not we need to
## add it 

tmp=`echo $PATH | tr ':' '\n' | grep $LOFARSOFT`

if [ "$tmp" == "$LOFARSOFT/release/bin" ] ; then
    echo "-- Adding release/bin to PATH..."
    export PATH=$PATH:$LOFARSOFT/release/bin
fi

## move back into $LOFARSOFT/build

cd $basedir

##______________________________________________________________________________
## Helper functions

print_help ()
{
    echo "";
    echo "NAME";
    echo "    build.sh -- script for (batch) control of builds";
    echo "";
    echo "SYNOPSIS";
    echo "    build.sh <package> [build-option]";
    echo "    build.sh { clean | clean-all | clean-build | clean-release}";
    echo "    build.sh { -h | --h | -help | --help | help }";
    echo "";
    echo "DESCRIPTION";
    echo "    A simple shell script to configure and install as many as possible of";
    echo "    the packages in the LOFAR USG distribution.";
    echo "";
    echo "    <package> = bdsm bison blitz boost casacore cfitsio cmake cr dal flex";
    echo "                hdf5 plplot python wcslib";
    echo "";
    echo "    The following options are available:";
    echo "";
    echo "    --force-build  = Force build from provided source code, even if system-wide";
    echo "                     version exists; otherwise the system-wise version of a";
    echo "                     library/package/binary will be used.";
    echo "";
    echo "    --new-build    = Clean up the build directory before the configuration step;";
    echo "                     use this in order to erase previous configuration settings";
    echo "                     and start fresh.";
    echo "";
    echo "    --report-build = Relay the results from building and testing back to the";
    echo "                     dashboard server for later inspection. This option is"
    echo "                     useful especially when your build failed (in parts) and"
    echo "                     you want help someone to track down the errors."
    echo "";
    echo "    clean, clean-build = Clean out the build directory."
    echo "    clean-release      = Clean out the release directory."
    echo "    clean-all          = Do a clean-build and a clean-release."
}

##______________________________________________________________________________
## Determine the make command to use (e.g. if to activate multi-threading)

make_command ()
{
  ## parallelize build on selected systems

  if [ "$SYSTEM_NAME" == "Darwin" ] ; then
    SYSTEM_NOF_CPU=5
  else
    ## try to determine the number of cores/CPUs
    if test -f /proc/cpuinfo ; then
      SYSTEM_NOF_CPU=`cat /proc/cpuinfo  | grep processor | wc -l`
    else
      SYSTEM_NOF_CPU=1
    fi
  fi

  if [ "$HOSTNAME" == "ray" ] ; then
    ENABLE_MULTITHREADING=0;
  fi

  ## set make command based on control parameter

  if [ "$ENABLE_MULTITHREADING" == "1" ] ; then 
    var_make="make -j $SYSTEM_NOF_CPU"
  else 
    var_make="make"
  fi

}

##______________________________________________________________________________
## Build CMake from source 

build_cmake ()
{
    ## local variables
    
    HAVE_CMAKE=0
    CMAKE_MAJOR_VERSION=0
    CMAKE_MINOR_VERSION=0

    ## [1] Check the system if there already is a version of CMake
    ##     installed
    
    if test -z `which cmake` ; then {
	HAVE_CMAKE=0;
    } else {
	HAVE_CMAKE=1;
    } fi;
	
    ## [2] If a version of CMake is installed, check if it is a recent
    ##     enough correct version.
	
    if [[ ${HAVE_CMAKE} == 1 ]] ; then {
		
        ## obtain version information
		
	CMAKE_VERSION=`cmake --version | tr " " "\n" | grep -v cmake | grep -v version`
	CMAKE_MAJOR_VERSION=`echo $CMAKE_VERSION | tr "." " " | tr "-" " "  | awk '{print $1}'`
	CMAKE_MINOR_VERSION=`echo $CMAKE_VERSION | tr "." " " | tr "-" " "  | awk '{print $2}'`
	
	## build condition
		
	if [[ ${CMAKE_MAJOR_VERSION} -lt ${REQUIRED_MAJOR_VERSION} ]] ; then {
	    echo "CMake major version < ${REQUIRED_MAJOR_VERSION} ; build required";
	    HAVE_CMAKE=0;
	} else {
	    if [[ ${CMAKE_MINOR_VERSION} -lt ${REQUIRED_MINOR_VERSION} ]] ; then {
		echo "CMake minor version < ${REQUIRED_MINOR_VERSION} ; build required";
		HAVE_CMAKE=0;
	    } fi
	} fi

    } fi

    ## [3] If required initiate build of CMake from provided sources

    if [[ ${HAVE_CMAKE} == 0 ]] ; then {

    ## Check if the source code to build cmake is available; if this
    ## is not the case with error, since this is the bottom-most 
    ## position in the dependency chain
    if test -d $basedir/../external/cmake ; then
	echo "[`date`] Found directory with source code for CMake."
    else
	echo "[`date`] Missing source directory for CMake! Unable to continue!"
	exit 1;
    fi
	## prepare to build cmake from its source
    if test -d cmake ; then
	cd cmake;
    else
	mkdir cmake;
	cd cmake;
    fi
	## run the configure script
    echo "[`date`] Running configure script for CMake ..."
    $basedir/../external/cmake/configure --prefix=$basedir/../release
	## build and install
    echo "[`date`] Initiating build and install of CMake ..."
    $var_make && $var_make install
    export PATH=$PATH:$basedir/../release/bin

    } fi;

    ## Check if after the build and installation indeed now we have a
    ## working and recent enough version available

    if test -z `which cmake` ; then { exit 1; } fi
}

## -------------------------------------------------------------------
## Generic instructions for building on of the packages in the code
## tree.
#
# \param buildDir  -- directory in which to perform the build; this is a 
#                     sub-directory created below "build"
# \param sourceDir -- directory in which the source code of the package can be
#                     found; the path is given relativ to $LOFARSOFT
build_package ()
{
  buildDir=$1
  sourceDir=$2
  buildOptions=$3
  # check if the build directory exists
  cd $basedir
  if test -d "$buildDir" ; then
  {
    # Feedback
    echo "[`date`] Building package";
    echo "-- Build directory ..... : $buildDir";
    echo "-- Source directory .... : $sourceDir";
    echo "-- Configuration options : $buildOptions";
    echo "-- Send build log ...... : $REPORT_BUILD";
    # change into the build directory
    cd $buildDir
    # run cmake on the source directory
    if test -z "$buildOptions" ; then
	cmake $basedir/../$sourceDir
    else 
	cmake $basedir/../$sourceDir $buildOptions
    fi
	    # build the package
    if test -z "`make help | grep install`" ; then
	echo "[`date`] No target install for $buildDir."
    else
	if [[ ${REPORT_BUILD} == 1 ]] ; then
	    $var_make Experimental;
	    $var_make install;
	else
	    $var_make install;
	fi
    fi
  }
  else 
  {
      echo "[`date`] No build directory $buildDir - creating it now."
      mkdir $buildDir;
	    # recursive call
      build_package $buildDir $sourceDir $buildOptions
  }
  fi
}

## -----------------------------------------------------------------------------
## check command line parameters

## [1] Required parameter: Name of the package to build -- if now package name
##                         is provided, we print the help information

if test -z $1 ; then
    print_help
    exit;
else
    case $1 in
	-h)
	    print_help
	    exit;
	;;
	--h)
	    print_help
	    exit;
	;;
	-help)
	    print_help
	    exit;
	;;
	--help)
	    print_help
	    exit;
	;;
	help)
	    print_help
	    exit;
	;;
	-clean-build)
	    echo "------------------------------------------------------------";
	    echo "[`date`] Cleaning up build directories..."
	    echo "------------------------------------------------------------";
	    ## clean up $LOFARSOFT/build
	    rm -rf *~ *.log CMake* CPack* Makefile external cmake_install.cmake;
	    rm -rf bdsm bison blitz boost;
	    rm -rf casacore cfitsio cmake config contrib cr; 
	    rm -rf dal dsp;
	    rm -rf flex;
	    rm -rf hdf5;
	    rm -rf mathgl;
	    rm -rf plplot pyrap python;
	    rm -rf rm root;
	    rm -rf qt;
	    rm -rf sip startools szip;
	    rm -rf vtk;
	    rm -rf wcslib wcstools wget;
	    rm -rf zlib;
	    ## additional cleaning up within the "external" tree
	    cd $basedir/../external/hdf5; make distclean
	    cd $basedir/../external/wget; make distclean
	    ##
	    echo "------------------------------------------------------------";
	    echo "[`date`] Finished cleaning up build directories"
	    echo "------------------------------------------------------------";
	    exit;
	;;
	clean-build)
	    $basedir/build.sh -clean-build;
	    exit;
	;;
	-clean-release)
	    rm -rf $basedir/../release/bin
	    rm -rf $basedir/../release/doc
	    rm -rf $basedir/../release/etc
	    rm -rf $basedir/../release/include
	    rm -rf $basedir/../release/info
	    rm -rf $basedir/../release/lib
	    rm -rf $basedir/../release/man
	    rm -rf $basedir/../release/share
	    exit;
	;;
	clean-release)
	    $basedir/build.sh -clean-release;
	    exit;
	;;
	-clean-all)
	    $basedir/build.sh -clean-build;
	    $basedir/build.sh -clean-release;
	    exit;
	;;
	clean-all)
	    $basedir/build.sh -clean-all;
	    exit;
	;;
	-clean)
	    $basedir/build.sh -clean-build;
	    exit;
	;;
	clean)
	    $basedir/build.sh -clean;
	    exit;
	;;
	*)
	    param_packageName=$1;
	    echo " -- Selected package: $param_packageName";
	;;
    esac
fi

## [2] Optional parameters: Force build of package from provided source?

option_found=true
while [ "$option_found" == "true" ]
  do
  case $2 in
    --force-build)
      FORCE_BUILD=1;
      shift;
      echo " -- Recognized build option; forcing build."; 
    ;;
    --new-build)
      CLEAN_BUILD=1;
      shift;
      echo " -- Recognized clean-build option; forcing new configuration."; 
    ;;
    --report-build)
      REPORT_BUILD=1;
      shift
      echo " -- Recognized build option; reporting build/test results."; 
    ;;
    --multithreading)
      ENABLE_MULTITHREADING=1;
      shift
      echo " -- Recognized build option; multi-threading enabled."; 
    ;;
    *)
    option_found=false
  esac
done

## -----------------------------------------------------------------------------
## Build individual/multiple packages

make_command

## since all further operations need CMake as central tool, we need to make sure
## it is available first.

build_cmake

## once we have make sure CMake is available, we can continue

case $param_packageName in 
    ## --------------------------------------------------------------------------
    ## --- USG software packages ------------------------------------------------
    ## --------------------------------------------------------------------------
    rm)
        echo "[`date`] Processing packages required for RM ..."
		cd $basedir; ./build.sh dal
		echo "[`date`] Building RM package ..."
		build_package rm src/RM "-DRM_FORCE_BUILD:BOOL=$FORCE_BUILD";
    ;;
    ## --------------------------------------------------------------------------
    ## --- General testing of environment ---------------------------------------
    ## --------------------------------------------------------------------------
    config)
        if test -d config ; then
	    cd config;
	    rm -rf *
	    cmake $basedir/../devel_common/cmake
	else 
	    mkdir config
	    $basedir/build.sh config
	fi
	;;
    all)
        echo "[`date`] Building external packages not build otherwise";
	$basedir/build.sh bison;
	$basedir/build.sh flex;
	echo "[`date`] Building all USG packages";
	$basedir/build.sh cr;
    ;;
esac
