#!/bin/sh

##
## A simple shell script to configure and install as many as possible of
## the packages in the LOFAR USG distribution. Simple run
## 
##   ./build.sh
##
## from within the build directory - and the script should take care of
## the rest.
##

basedir=`pwd`

## -------------------------------------------------------------------
## Helper functions

build_package ()
{
    buildDir=$1
    sourceDir=$2
    # check if the build directory exists
    cd $basedir
    if test -d $buildDir ; then
	{
	# change into the build directory
	cd $buildDir
	# clean up the directory before we do anything else
	rm -rf *
	# run cmake on the source directory
	cmake $basedir/../$sourceDir
	# build the package
	make install;
	}
    else 
	{
	echo "No build directory $buildDir - creating it now."
	mkdir $buildDir;
	# recursive call
	build_package $buildDir $sourceDir
	}
    fi
}

## [1] External packages ---------------------------------------------

for var_package in pgplot wcslib cfitsio casacore
{
   build_package $var_package external/$var_package
}

cd $basedir
if test -d hdf5 ; then
    cd hdf5
    rm -rf *
else 
    mkdir hdf5
    cd hdf5
fi
cmake ../../external/hdf5

## [2] USG packages ----------------------------------------

cd $basedir/dal
rm -rf *
cmake ../../src/DAL
make install

cd $basedir/cr
rm -rf *
cmake ../../src/CR-Tools
make && make install
ctest

