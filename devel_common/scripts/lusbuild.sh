#!/bin/bash
#
# Daily build of User Software Group software
#
# Builds are done in a daily varying build directory. 
#
# For each build, a logfile is put in the build directory with a date extension
#
#set echo

# Define some directories

cd $APS_LOCAL/lus/buildscripts

# Set daily environment variables
. common.sh

lus_srcroot=${lus_rootdir}/src
lus_codedir=$lus_srcroot/code

runTests ()
{
    echo "Running test procedures" >>$1

    make -j4 ExperimentalStart  1>>$1 2>&1 

    make -j4 ExperimentalConfigure 1>>$1 2>&1 
    make ExperimentalSubmit  1>>$1 2>&1  

    make -j4 ExperimentalBuild  1>>$1 2>&1 
    make ExperimentalSubmit  1>>$1 2>&1 

    make -j4 ExperimentalTest  1>>$1 2>&1 
    make ExperimentalSubmit  1>>$1 2>&1 

#    make ExperimentalMemCheck  1>>$1 2>&1 
#    make ExperimentalSubmit  1>>$1 2>&1 
}

# Define output files
#date=`date +%Y%m%d` 
#time=`date +%H%M%S`

# Create destination directories ; uncomment next lines for new checkout
rm -fr ${lus_codedir}/release/*
rm -fr ${lus_codedir}/build/*
rm -fr ${lus_srcroot}
mkdir -p $lus_srcroot
mkdir -p $lus_logdir

if [ ! -d $lus_srcroot ]; then 
    echo "Could not create directory "$lus_srcroot
    exit 1
fi

if [ ! -d $lus_logdir ]; then 
    echo "Could not create directory "$lus_logdir
    exit 1
fi

if [ -e $lus_logfile ]; then 
  rm -f $lus_logfile >/dev/null 2>&1
fi

touch $lus_logfile
if [ ! -e $lus_logfile ]; then
   echo "Could not create output file "$lus_logfile
   out=/dev/null
fi

if [ -e $lus_logfile ]; then 
  rm -f $lus_logfile >/dev/null 2>&1
fi

touch $lus_logfile
if [ ! -e $lus_logfile ]; then
   echo "Could not create output file "$lus_logfile
   lus_logfile=/dev/null
fi

curdate=`date +%Y-%m-%d\ %H:%M:%S`
echo "Starting build of LUS software at $curdate" >> $lus_logfile

cd $lus_srcroot

# Get source code; uncomment next line for full checkout otherwise update
svn co http://usg.lofar.org/svn/code/trunk code 1>>$lus_logfile 2>&1
cd code
#svn cleanup  1>>$lus_logfile 2>&1 
#svn up  1>>$lus_logfile 2>&1 

if [ $? -ne 0 ]; then 
    echo "Problem during check-out!" >> $lus_logfile
    exit 1
fi

if [ ! -d $lus_codedir ]; then 
    echo "Could not find directory $lus_codedir" >> $lus_logfile
    exit 1
fi

# Initialize, Configure
cd $lus_codedir

export LOFARSOFT=$lus_codedir
export PATH=$LOFARSOFT/release/bin:$PATH
. $LOFARSOFT/devel_common/scripts/init.sh  1>>$lus_logfile 2>&1 

## Boostrap the fresh checkout
cd  $lus_codedir
./bootstrap  \
  -DHDF5_HDF5_CPP_LIBRARY:FILEPATH=$LOFARSOFT/release/lib/libhdf5_cpp.so \
  -DHDF5_HDF5_HL_LIBRARY:FILEPATH=$LOFARSOFT/release/lib/libhdf5_hl.so \
  -DHDF5_HDF5_LIBRARY:FILEPATH=$LOFARSOFT/release/lib/libhdf5.so \
  -DHDF5_INCLUDES:PATH=$LOFARSOFT/release/include/hdf5 \
  -DDAL_FROM_REPOS=TRUE \
  -DANAAMIKA_ENABLE_FBDSM:BOOL=NO \
1>>$lus_logfile 2>&1 

## Change into the build directory
cd  $lus_codedir/build

if [ $? -ne 0 ]; then 
   echo "Problem during configure" >>$lus_logfile 2>&1 
   exit 1
fi

# Then build everything (DAL first!)
#for LUS_PACKAGE in dal anaamika cr pulsar rm
for LUS_PACKAGE in dal pulsar anaamika pycrtools 
{
    ## Feedback
    echo "Building $LUS_PACKAGE"  1>>$lus_logfile

    ## Change into common build directory
    cd  $lus_codedir/build

    ## Build and install package
    result_package=`make -j4 -k $LUS_PACKAGE  1>>$lus_logfile 2>&1; echo $?`
    echo "==============================" >>$lus_logfile
    echo "make $LUS_PACKAGE result: "${result_package} >>$lus_logfile
    echo "==============================" >>$lus_logfile

    ## Experimental build to submit test results to Dashboard
    ${LUS_PACKAGE}_Experimental $lus_logfile
}

# Status message
curdate=`date +%Y-%m-%d\ %H:%M:%S`
echo -n "Build finished at ${curdate}; " >> $lus_logfile

 
# Reset link to latest build.
#rm $lus_rootdir/dal  1>> $lus_logfile
#ln -s $lus_codedir/release $lus_rootdir/dal  1>>$lus_logfile 2>&1 

now=`date +%Y-%m-%d\ %H:%M:%S`
echo "Done at $now." >> $lus_logfile
