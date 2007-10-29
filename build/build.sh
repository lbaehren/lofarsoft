#!/bin/sh

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

## Default values for the optional parameters

param_forceBuild=0;
param_cleanBuild=0;
param_reportBuild='';

## -----------------------------------------------------------------------------
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

# \param buildDir  -- directory in which to perform the build; this is a 
#                     sub-directory created below "build"
# \param sourceDir -- directory in which the source code of the package can be
#                     found; the path is given relativ to $LOFARSOFT
build_package ()
{
    buildDir=$1
    sourceDir=$2
    buildOptions=$3
    # Feedback
    echo "[`date`] Building package";
    echo " -- Build directory ..... : $buildDir";
    echo " -- Source directory .... : $sourceDir";
    echo " -- Configuration options : $buildOptions";
    echo " -- Send build log ...... : $param_reportBuild";
    # check if the build directory exists
    cd $basedir
    if test -d $buildDir ; then
	{
	# change into the build directory
	cd $buildDir
	# run cmake on the source directory
	if test -z $buildOptions ; then
	    cmake $basedir/../$sourceDir
	else 
	    cmake $basedir/../$sourceDir $buildOptions
	fi
	# build the package
	if test -z "`make help | grep install`" ; then
	    echo "[`date`] No target install for $buildDir."
	else
	    if test -z $param_reportBuild ; then
		make install;
	    else
		make Experimental;
		make install;
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
else
    case $1 in
	-h)
		print_help
	;;
	--h)
		print_help
	;;
	-help)
		print_help
	;;
	--help)
		print_help
	;;
	help)
		print_help
	;;
	-clean-build)
		rm -f *~ *.log;
		rm -rf bison blitz boost;
		rm -rf casacore cfitsio cmake config cr; 
		rm -rf dal dsp;
		rm -rf flex;
		rm -rf hdf5;
		rm -rf plplot python;
		rm -rf vtk;
		rm -rf wcslib;
	;;
	clean-build)
		./build.sh -clean-build;
	;;
	-clean-release)
		rm -rf $basedir/../release/bin
		rm -rf $basedir/../release/doc
		rm -rf $basedir/../release/include
		rm -rf $basedir/../release/info
		rm -rf $basedir/../release/lib
		rm -rf $basedir/../release/man
		rm -rf $basedir/../release/share
	;;
	clean-release)
		./build.sh -clean-release;
	;;
	-clean-all)
		./build.sh -clean-build;
		./build.sh -clean-release;
	;;
	clean-all)
		./build.sh -clean-all;
	;;
	-clean)
		./build.sh -clean-build;
	;;
	clean)
		./build.sh -clean;
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
      param_forceBuild=1;
      shift;
      echo " -- Recognized build option; forcing build."; 
    ;;
    --new-build)
      param_cleanBuild=1;
      shift;
      echo " -- Recognized clean-build option; forcing new configuration."; 
    ;;
    --report-build)
      param_reportBuild="--report-build"
      shift
      echo " -- Recognized build option; reporting build/test results."; 
    ;;
    *)
    option_found=false
  esac
done

## Summary of options

#echo "packageName = $param_packageName"
#echo "forceBuild  = $param_forceBuild"
#echo "cleanBuild  = $param_cleanBuild"
#echo "reportBuild = $param_reportBuild"

## -----------------------------------------------------------------------------
## Build individual/multiple packages

case $param_packageName in 
    bison)
		echo "[`date`] Selected package Bison"
		build_package bison external/bison "-DBISON_FORCE_BUILD:BOOL=$param_forceBuild";
    ;;
    blitz)
		echo "[`date`] Selected package Blitz++"
		build_package blitz external/blitz "-DBLITZ_FORCE_BUILD:BOOL=$param_forceBuild";
    ;;
    boost)
		echo "[`date`] Selected package Boost"
		build_package boost external/boost "-DBOOST_FORCE_BUILD:BOOL=$param_forceBuild -DBOOST_FIND_python_ONLY:BOOL=1";
    ;;
    casacore)
		echo "[`date`] Selected package CASACORE"
		## -- build required packages
		./build.sh wcslib $param_reportBuild
		./build.sh cfitsio $param_reportBuild
		## -- build package
		build_package casacore external/casacore
    ;;
    cfitsio)
		echo "[`date`] Selected package CFITSIO"
		build_package cfitsio external/cfitsio
    ;;
    cmake)
		echo "[`date`] Selected package CMake"
		## Check if the source code to build cmake is available; if this
		## is not the case with error, since this is the bottom-most 
		## position in the dependency chain
		if test -d $basedir/../external/cmake ; then
	    	echo "[`date`] Cleaning up source directory ..."
	    	rm -rf $basedir/../external/cmake/Bootstrap.cmk
	    	rm -rf $basedir/../external/cmake/CMakeCache.txt
	    	rm -rf $basedir/../external/cmake/Source/cmConfigure.h
		else
	    	echo "[`date`] Missing source directory for cmake! Unable to continue!"
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
		echo "[`date`] Running configure script for cmake ..."
		$basedir/../external/cmake/configure --prefix=$basedir/../release
		## build and install
		echo "[`date`] Initiating build and install of cmake ..."
		make && make install
		## check if we have been able to create a cmake executable
		if test -f ../../release/bin/cmake ; then
	    	echo "[`date`] Found newly created cmake executable."
	    	export PATH=$PATH:$basedir/../release/bin
		else
	    	echo "[`date`] No cmake executable found in release/bin! Unable to continue!"
	    	exit 1;
		fi
    ;;
    flex)
		echo "[`date`] Selected package Flex"
		build_package flex external/flex "-DFLEX_FORCE_BUILD:BOOL=$param_forceBuild"
    ;;
    hdf5)
		echo "[`date`] Selected package Hdf5"
		build_package hdf5 external/hdf5 "-DHDF5_FORCE_BUILD:BOOL=$param_forceBuild";
    ;;
    plplot)
		echo "[`date`] Selected package Plplot"
		if test -d $basedir/../external/plplot ; then
		## first pass
		mkdir $basedir/plplot 
		cd $basedir/plplot 
		cmake -C $basedir/../devel_common/cmake/SettingsPLplot.cmake $basedir/../external/plplot
		## second pass
	    	build_package plplot external/plplot
		else
		    cd $basedir/../external
	    	## download the source tar-ball from source forge
		    wget -c http://ovh.dl.sourceforge.net/sourceforge/plplot/plplot-5.7.4.tar.gz
		    ## unpack the tar-ball and adjust the name of the newly created directory
	    	tar -xvzf plplot-5.7.4.tar.gz
		    mv plplot-5.7.4 plplot
		    ## remove the tar-ball
	    	rm -f plplot-5.7.4.tar.gz
	    	## recursive call of this method
	    	cd $basedir
	    	./build.sh plplot
		fi
    ;;
    python)
		echo "[`date`] Selected package PYTHON"
		build_package python external/python
    ;;
    vtk)
		echo "[`date`] Selected package VTK"
		echo "-- No configuration and build support available yet!"
    ;;
    wcslib)
		echo "[`date`] Selected package WCSLIB"
		build_package wcslib external/wcslib
    ;;
    ## --------------------------------------------------------------------------
    ## --- USG software packages ------------------------------------------------
    ## --------------------------------------------------------------------------
    dal)
		## external packages
		echo "[`date`] Processing required packages ..."
		./build.sh cmake
		./build.sh bison
		./build.sh flex
		./build.sh casacore --force-build $param_reportBuild
		./build.sh python --force-build $param_reportBuild
		./build.sh boost --force-build $param_reportBuild
		./build.sh hdf5 --force-build $param_reportBuild
		## USG packages
		echo "[`date`] Building Data Access Library ..."
		build_package dal src/DAL
		## Post-installation testing
		echo ""
		echo "------------------------------------------------------------";
		echo ""
		echo "[`date`] To test the DAL installation run:"
		echo ""
		echo "  cd build/dal; ctest"
		echo ""
		echo "------------------------------------------------------------";
		echo ""
    ;;
    cr)
		echo "[`date`] Processing required packages ..."
		./build.sh dal;
		./build.sh plplot;
		echo "[`date`] Building CR-Tools package ..."
		build_package cr src/CR-Tools;
    ;;
    bdsm)
		echo "[`date`] Building BDSM package ..."
		build_package bdsm src/BDSM;
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
	./build.sh config
    fi
    ;;
    all)
    echo "[`date`] Building external packages not build otherwise";
    ./build.sh bison;
    ./build.sh flex;
    echo "[`date`] Building all USG packages";
    ./build.sh cr;
    ;;
esac
