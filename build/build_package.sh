
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
    # change into the build directory
    cd $buildDir
    echo "-- Changed into directory `pwd`"

    # run cmake on the source directory
    if test -z "$buildOptions" ; then
	cmake $sourceDir
    else 
	cmake $sourceDir $buildOptions
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

##______________________________________________________________________________
## Parsing of input parameters

if test -z $1 ; then
    echo "[build_package] No package name provided! Unable to continue!
    exit;"
else
    PACKAGE_NAME=$1
fi 

if test -z $2 ; then
    echo "[build_package] No package source directory provided! Unable to continue!";
    exit;
else
    PACKAGE_SOURCE_DIR=$2
fi

echo "[`date`]"
echo "-- Package name       = ${PACKAGE_NAME}"
echo "-- Package source dir = ${PACKAGE_SOURCE_DIR}"

make_command
build_package $PACKAGE_NAME $PACKAGE_SOURCE_DIR