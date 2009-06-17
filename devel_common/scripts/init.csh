#############################################################################
# FILE NAME: init.csh
#
# DEVELOPMENT: LOFAR USG
#
# DATE: 10/04/07
#
# DESCRIPTION:
#
#       This script configures the user's environment for the LOFAR 
#       user software package.
#
#       When the script is sourced, it checks for the existence of a
#       LOFARSOFT environment variable. If not found, an error message
#       is issued. If found, the script proceeds to add the path to 
#       the executables to the user's path variable.
#
#       Similarly, the existence of the PYTHONPATH variable is examined
#       and if not present is defined. This addition allows user's to 
#       utilize the included python modules.
#
#       This version of the script is written in the csh language, 
#       and is compatible tcsh, too.
#
#############################################################################

SYSTEM_NAME=`uname -s`

##______________________________________________________________________________
## Check for the existence of the LOFARSOFT  environment variable

if(${?LOFARSOFT} == 0) then
   echo "ERROR:  Please set the LOFARSOFT environment variable"
   exit
else
   echo "Initializing LOFAR software environment"
endif


##______________________________________________________________________________
##  Add the LOFAR executables to the path

echo "-- Add the LOFAR executables to the path"

set path = ( $LOFARSOFT/release/bin $path )


##______________________________________________________________________________
##  Add the location of the libraries

if ( "$SYSTEM_NAME" == "Darwin" ) then
    setenv DYLD_LIBRARY_PATH $(LOFARSOFT)/release/lib:$(DYLD_LIBRARY_PATH)
else 
    setenv LD_LIBRARY_PATH $(LOFARSOFT)/release/lib:$(LD_LIBRARY_PATH)
endif

#############################################################################
#
# Add the Python libraries to the existing search path
#
#############################################################################

echo "-- Add the Python libraries to the existing search path"

if ($?PYTHONPATH) then
   if ( $PYTHONPATH != $LOFARSOFT/release/lib ) then   
      setenv PYTHONPATH ${LOFARSOFT}/release/lib/python:${PYTHONPATH}
   endif
else 
   setenv PYTHONPATH ${LOFARSOFT}/release/lib/python
endif

for PY_VERSION in 2.6 2.5 2.4 
{
  if test -d $LOFARSOFT/release/lib/python$PY_VERSION ; then
    export PYTHONPATH=$LOFARSOFT/release/lib/python$PY_VERSION:$PYTHONPATH
  endif

  if test -d $LOFARSOFT/release/lib/python$PY_VERSION/site-packages ; then
    export PYTHONPATH=$LOFARSOFT/release/lib/python$PY_VERSION/site-packages:$PYTHONPATH
  endif

  ## Location of Fink-installed packages

  if test -d /sw/lib/python$PY_VERSION/site-packages ; then
    export PYTHONPATH=$PYTHONPATH:/sw/lib/python$PY_VERSION/site-packages
  endif

  if test -d /sw/lib/python$PY_VERSION/site-packages/PyQt4 ; then
    export PYTHONPATH=$PYTHONPATH:/sw/lib/python$PY_VERSION/site-packages/PyQt4
  endif
}

#############################################################################
#
#  Define the AIPSPATH environment variable, used to locate measures data
#
#############################################################################

if ($?AIPSPATH) then
    setenv AIPSPATH ${LOFARSOFT}
endif

#############################################################################
#                    Finished configuration                                 #  
#############################################################################

