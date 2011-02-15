#!/bin/sh

## Basic settings

REQUIRED_VERSION_MAJOR=2
REQUIRED_VERSION_MINOR=8
REQUIRED_VERSION_PATCH=4


## === Do not edit below this point! ============================================


## ==============================================================================
##
##  Functions
##
## ==============================================================================

## Variables

basedir=`pwd`
CMAKE_REQUIRED_VERSION=$REQUIRED_VERSION_MAJOR.$REQUIRED_VERSION_MINOR

CMAKE_VERSION_MAJOR=
CMAKE_VERSION_MINOR=
CMAKE_VERSION_PATCH=
CMAKE_VERSION_OK=
CMAKE_SOURCE_ARCHIVE=
CMAKE_INSTALL_PREFIX=

##_______________________________________________________________________________
##                                                                    cmake_check

cmake_check ()
{
  ## [1] Check if there is a version of CMake installed at all

  if test -z `which cmake` ; then {
    CMAKE_VERSION_OK=0;
  } else {
    CMAKE_VERSION_OK=1;
  } fi;

  ## [2] If CMake is installed, check if it is a recent enough version.
	
  if [ ${CMAKE_VERSION_OK} -eq 1 ] ; then {
		
    ## obtain version information
		
    CMAKE_VERSION=`cmake --version | tr " " "\n" | grep -v cmake | grep -v version`
    CMAKE_VERSION_MAJOR=`echo $CMAKE_VERSION | tr "." " " | tr "-" " "  | awk '{print $1}'`
    CMAKE_VERSION_MINOR=`echo $CMAKE_VERSION | tr "." " " | tr "-" " "  | awk '{print $2}'`
    CMAKE_VERSION_PATCH=`echo $CMAKE_VERSION | tr "." " " | tr "-" " "  | awk '{print $3}'`
    ## Check version

    if [ ${CMAKE_VERSION_MAJOR} -lt ${REQUIRED_VERSION_MAJOR} ] ; then {
      echo "CMake major version < ${REQUIRED_VERSION_MAJOR} ; build required";
      CMAKE_VERSION_OK=0;
    } else {
      if [ ${CMAKE_VERSION_MINOR} -lt ${REQUIRED_VERSION_MINOR} ] ; then {
        echo "CMake minor version < ${REQUIRED_VERSION_MINOR} ; build required";
        CMAKE_VERSION_OK=0;
      } else {
        if [ ${CMAKE_VERSION_PATCH} -lt ${REQUIRED_VERSION_PATCH} ] ; then {
          echo "CMake patch version < ${REQUIRED_VERSION_PATCH} ; build required";
          CMAKE_VERSION_OK=0;
        } fi;
     } fi;
    } fi;

  } fi;
}

##_______________________________________________________________________________
##                                                                  cmake_install

cmake_install ()
{
  export UPDATE_TYPE='svn'

  ## Check if the source archive is available

  if [ -f $CMAKE_SOURCE_ARCHIVE ] ; then {
    echo "-- [checkCMake] Found source archive -- initiating build ..."
  } else {
    wget -c http://www.cmake.org/files/v2.8/cmake-2.8.2.tar.gz
  } fi;

  ## Expand the source archive

  echo "-- [checkCMake] Unpacking source archive ..."

  tar -xzf $CMAKE_SOURCE_ARCHIVE

  ## Configure CMake

  CMAKE_SOURCE_DIR=`ls | grep cmake | grep $CMAKE_REQUIRED_VERSION | grep -v tar`
  cd $CMAKE_SOURCE_DIR

  if test -z $CMAKE_INSTALL_PREFIX ; then {
    ./configure
  } else {
    ./configure --prefix=$CMAKE_INSTALL_PREFIX
  } fi;

  ## Build and install CMake

  make -j2
  make install

  ## clean up directory 
 
  cd $basedir
  rm -rf $CMAKE_SOURCE_DIR

}

## ==============================================================================
##
##  Main
##
## ==============================================================================

## Check input arguments

if test -z $1 ; then 
  echo "No input parameter provided!"
  exit
else
  CMAKE_SOURCE_ARCHIVE=$1
fi

if test -z $2 ; then 
  echo "No installation prefix provided - using default."
else
  CMAKE_INSTALL_PREFIX=$2
fi

## Check if an ok version of CMake is installed

cmake_check

## If no valid version is found, initiate build from source

if [ ${CMAKE_VERSION_OK} -eq 0 ] ; then {

  echo "CMake version        = $CMAKE_VERSION"
  echo "CMake verison ok?    = $CMAKE_VERSION_OK"
  echo "CMake source archive = $CMAKE_SOURCE_ARCHIVE"
  echo "CMake install prefix = $CMAKE_INSTALL_PREFIX"

  cmake_install

} fi;
