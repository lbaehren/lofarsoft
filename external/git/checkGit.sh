#!/bin/sh

## Basic settings

REQUIRED_VERSION_MAJOR=1
REQUIRED_VERSION_MINOR=6
REQUIRED_VERSION_PATCH=5


## === Do not edit below this point! ============================================


## ==============================================================================
##
##  Functions
##
## ==============================================================================

## Variables

basedir=`pwd`
GIT_REQUIRED_VERSION=$REQUIRED_VERSION_MAJOR.$REQUIRED_VERSION_MINOR.$REQUIRED_VERSION_PATCH

GIT_VERSION_MAJOR=
GIT_VERSION_MINOR=
GIT_VERSION_PATCH=
GIT_VERSION_OK=0
GIT_SOURCE_ARCHIVE=
GIT_INSTALL_PREFIX=

GIT_DOWNLOAD_URL="http://git-core.googlecode.com/files/git-1.7.7.tar.gz"

##_______________________________________________________________________________
##                                                                      git_check
## Description:
##   Check if Git is installed and available on the system. If Git is installed,
##   ensure that the deployed version is correct.
##

git_check ()
{
  ## [1] Check if there is a version of Git installed at all

  if test -z `which git` ; then {
    GIT_VERSION_OK=0;
  } else {
    GIT_VERSION_OK=1;
  } fi;

  ## [2] If Git is installed, check if it is a recent enough version.
	
  if [ ${GIT_VERSION_OK} -eq 1 ] ; then {
		
    ## obtain version information
		
    GIT_VERSION=`git --version | tr " " "\n" | grep -v git | grep -v version`
    GIT_VERSION_MAJOR=`echo $GIT_VERSION | tr "." " " | tr "-" " "  | awk '{print $1}'`
    GIT_VERSION_MINOR=`echo $GIT_VERSION | tr "." " " | tr "-" " "  | awk '{print $2}'`
    GIT_VERSION_PATCH=`echo $GIT_VERSION | tr "." " " | tr "-" " "  | awk '{print $3}'`

    VERSION_NUMBER_FOUND=$(( $GIT_VERSION_PATCH + 1000*$GIT_VERSION_MINOR  + 1000000*$GIT_VERSION_MAJOR ))
    VERSION_NUMBER_REQUIRED=$(( $REQUIRED_VERSION_PATCH + 1000*$REQUIRED_VERSION_MINOR  + 1000000*$REQUIRED_VERSION_MAJOR ))

    echo "Required version of Git = ${GIT_REQUIRED_VERSION} (${VERSION_NUMBER_REQUIRED})"
    echo "Found version of Git    = ${GIT_VERSION} (${VERSION_NUMBER_FOUND})"

    if [ ${VERSION_NUMBER_FOUND} -lt ${VERSION_NUMBER_REQUIRED} ] ; then {
      echo "Found version of Git too old - rebuild required!"
      GIT_VERSION_OK=0;
    } else {
      echo "Found version of Git is ok."
      GIT_VERSION_OK=1;
    } fi;
    
  } fi;
}

##_______________________________________________________________________________
##                                                                      git_fetch

git_fetch ()
{
    wget -c "${GIT_DOWNLOAD_URL}"
}

##_______________________________________________________________________________
##                                                                  git_install

git_install ()
{
  export UPDATE_TYPE='svn'

  ## Expand the source archive

  echo "-- [checkGit] Unpacking source archive ..."

  tar -xzf $GIT_SOURCE_ARCHIVE

  ## Configure Git

  GIT_SOURCE_DIR=`ls | grep git | grep -v tar`
  cd $GIT_SOURCE_DIR

  if [ -z $GIT_INSTALL_PREFIX ] ; then {
    ./configure
  } else {
    ./configure --prefix=$GIT_INSTALL_PREFIX
  } fi;

  ## Build and install Git

  make -j2
  make install

  ## clean up directory 
 
  cd $basedir
  rm -rf $GIT_SOURCE_DIR

}

## ==============================================================================
##
##  Main
##
## ==============================================================================

## Command line parameter: installation prefix _____________

if test -z $1 ; then 
  echo "No installation prefix provided - using default."
else
  GIT_INSTALL_PREFIX=$1
fi

## Check if an ok version of Git is installed ______________

git_check

## If no valid version is found, initiate build from source

if [ ${GIT_VERSION_OK} -eq 0 ] ; then {

  ## Configuration feedback

  echo "Git version        = $GIT_VERSION"
  echo "Git version ok?    = $GIT_VERSION_OK"
  echo "Git source archive = $GIT_SOURCE_ARCHIVE"
  echo "Git install prefix = $GIT_INSTALL_PREFIX"

  ## Set location of sources

  if test -z $2 ; then 
    git_fetch
    GIT_SOURCE_ARCHIVE=`ls git*.gz`
  else
    GIT_SOURCE_ARCHIVE=$2
  fi

  ## Install package

  git_install

} fi;
