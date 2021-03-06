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

set SYSTEM_NAME = `uname -s`

##______________________________________________________________________________
## Check for the existence of the LOFARSOFT  environment variable
    
if(${?LOFARSOFT} == 0) then
    echo "ERROR:  Please set the LOFARSOFT environment variable"
    exit
else
    if ( $?prompt ) then
	echo "Initializing LOFAR software environment"
    endif
endif

##______________________________________________________________________________
##  Add the LOFAR executables to the path

if ( $?prompt ) then
    echo "-- Add the LOFAR executables to the path"
endif

set path = ( $LOFARSOFT/release/share/pulsar/bin $LOFARSOFT/release/bin $LOFARSOFT/release/bin/python $path )


##______________________________________________________________________________
##  Add the location of the libraries

if ( "${SYSTEM_NAME}" == "Darwin" ) then
  if ($?DYLD_LIBRARY_PATH) then
    setenv DYLD_LIBRARY_PATH ${LOFARSOFT}/release/share/pulsar/lib:${LOFARSOFT}/release/lib:${DYLD_LIBRARY_PATH}
  else
    setenv DYLD_LIBRARY_PATH ${LOFARSOFT}/release/share/pulsar/lib:${LOFARSOFT}/release/lib
  endif
else 
  if ($?LD_LIBRARY_PATH) then
    setenv LD_LIBRARY_PATH ${LOFARSOFT}/release/share/pulsar/lib:${LOFARSOFT}/release/lib:${LD_LIBRARY_PATH}
  else
    setenv LD_LIBRARY_PATH ${LOFARSOFT}/release/share/pulsar/lib:${LOFARSOFT}/release/lib
  endif
endif

#############################################################################
#
# Add the Python libraries to the existing search path
#
#############################################################################

if ( $?prompt ) then
    echo "-- Add the Python libraries to the existing search path"
endif

if ($?PYTHONPATH) then
   if ( $PYTHONPATH != $LOFARSOFT/release/lib ) then   
      setenv PYTHONPATH ${LOFARSOFT}/release/lib/python:${LOFARSOFT}/release/share/pulsar/bin:${LOFARSOFT}/release/share/pulsar/lib/python:${PYTHONPATH}:$LOFARSOFT/build/cr/implement/GUI
   endif
else 
   setenv PYTHONPATH ${LOFARSOFT}/release/lib/python:${LOFARSOFT}/release/share/pulsar/bin:${LOFARSOFT}/release/share/pulsar/lib/python:$LOFARSOFT/build/cr/implement/GUI
endif

foreach PY_VERSION (2.6 2.5 2.4) 
  if ( -d ${LOFARSOFT}/release/lib/python${PY_VERSION} ) then
    setenv PYTHONPATH ${LOFARSOFT}/release/lib/python${PY_VERSION}:${PYTHONPATH}
  endif

  if ( -d ${LOFARSOFT}/release/lib/python${PY_VERSION}/site-packages ) then
    setenv PYTHONPATH ${LOFARSOFT}/release/lib/python${PY_VERSION}/site-packages:${PYTHONPATH}
  endif

  ## Location of Pulsar psrchive-installed packages
  if ( -d ${LOFARSOFT}/release/share/pulsar/lib/python${PY_VERSION}/site-packages ) then
    setenv PYTHONPATH ${PYTHONPATH}:${LOFARSOFT}/release/share/pulsar/lib/python${PY_VERSION}/site-packages
  endif

  ## Location of Fink-installed packages

  if ( -d /sw/lib/python${PY_VERSION}/site-packages ) then
    setenv PYTHONPATH ${PYTHONPATH}:/sw/lib/python${PY_VERSION}/site-packages
  endif

  if ( -d /sw/lib/python${PY_VERSION}/site-packages/PyQt4 ) then
    setenv PYTHONPATH ${PYTHONPATH}:/sw/lib/python${PY_VERSION}/site-packages/PyQt4
  endif

end

## Location of CR GUI
if ( -d $LOFARSOFT/build/cr/implement/GUI ) then
  setenv PYTHONPATH ${PYTHONPATH}:$LOFARSOFT/build/cr/implement/GUI
endif


#############################################################################
#
#  Define the AIPSPATH environment variable, used to locate measures data
#
#############################################################################

if ($?AIPSPATH) then
    setenv AIPSPATH ${LOFARSOFT}
endif

#############################################################################
#
#  Define the Pulsar software environment variables
#
#############################################################################

if (${?TEMPO}) then
  if (${TEMPO} != "${LOFARSOFT}/release/share/pulsar/bin") then
    if ( $?prompt ) then
      echo "-- Warning, resetting your TEMPO environment variable from:"
      echo "           $TEMPO to ${LOFARSOFT}/release/share/pulsar/bin"
    endif
  endif
endif
setenv TEMPO ${LOFARSOFT}/release/share/pulsar/bin

if (${?PRESTO}) then 
  if (${PRESTO} != "${LOFARSOFT}/release/share/pulsar") then
    if ( $?prompt ) then
      echo "-- Warning, resetting your PRESTO environment variable from:"
      echo "           $PRESTO to ${LOFARSOFT}/release/share/pulsar"
    endif
  endif
endif
setenv PRESTO ${LOFARSOFT}/release/share/pulsar

if (${?TEMPO2}) then 
  if (${TEMPO2} != "${LOFARSOFT}/release/share/pulsar") then
    if ( $?prompt ) then
      echo "-- Warning, resetting your TEMPO2 environment variable from:"
      echo "           $TEMPO2 to ${LOFARSOFT}/release/share/pulsar"
    endif
  endif
endif
setenv TEMPO2 ${LOFARSOFT}/release/share/pulsar


if (${?PSRCAT_FILE}) then 
  if (${PSRCAT_FILE} != "${LOFARSOFT}/release/share/pulsar/data/psrcat.db") then
    if ( $?prompt ) then
      echo "-- Warning, resetting your PSRCAT_FILE environment variable from:"
      echo "           $PSRCAT_FILE to ${LOFARSOFT}/release/share/pulsar/data/psrcat.db"
    endif
  endif
endif
setenv PSRCAT_FILE ${LOFARSOFT}/release/share/pulsar/data/psrcat.db

#############################################################################
#                    Finished configuration                                 #  
#############################################################################

