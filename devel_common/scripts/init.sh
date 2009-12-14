#############################################################################
# FILE NAME: init.sh
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
#       This version of the script is written in the sh language, 
#       and is compatible bash, too.
#
#############################################################################

SYSTEM_NAME=`uname -s`

# If not running interactively, don't do anything
 [ -z "$PS1" ] && return
 
##______________________________________________________________________________
## Check for the existence of the LOFARSOFT  environment variable

if test -z "$LOFARSOFT" ; then
   echo "ERROR:  Please set the LOFARSOFT environment variable"
else  # only execute the following if LOFARSOFT is set

##______________________________________________________________________________
##  Add the LOFAR executables to the path

export PATH=$LOFARSOFT/release/share/pulsar/bin:$LOFARSOFT/release/bin:$PATH

##______________________________________________________________________________
##  Add the location of the libraries

if [ "$SYSTEM_NAME" == "Darwin" ] ; then
    export DYLD_LIBRARY_PATH=$LOFARSOFT/release/share/pulsar/lib:$LOFARSOFT/release/lib:$DYLD_LIBRARY_PATH
else 
    export LD_LIBRARY_PATH=$LOFARSOFT/release/share/pulsar/lib:$LOFARSOFT/release/lib:$LD_LIBRARY_PATH
fi

##______________________________________________________________________________
## Add the Python libraries to the existing search path

if test -z "$PYTHONPATH" ; then
  export PYTHONPATH=$LOFARSOFT/release/lib/python:$LOFARSOFT/release/share/pulsar/bin:$LOFARSOFT/release/share/pulsar/lib/python:$LOFARSOFT/build/cr/implement/GUI:
else
  export PYTHONPATH=$LOFARSOFT/release/lib/python:$LOFARSOFT/release/share/pulsar/bin:$LOFARSOFT/release/share/pulsar/lib/python:$LOFARSOFT/build/cr/implement/GUI:$PYTHONPATH
fi

for PY_VERSION in 2.6 2.5 2.4 
{
  if test -d $LOFARSOFT/release/lib/python$PY_VERSION ; then
    export PYTHONPATH=$LOFARSOFT/release/lib/python$PY_VERSION:$PYTHONPATH
  fi

  if test -d $LOFARSOFT/release/lib/python$PY_VERSION/site-packages ; then
    export PYTHONPATH=$LOFARSOFT/release/lib/python$PY_VERSION/site-packages:$PYTHONPATH
  fi

  ## Location of Fink-installed packages

  if test -d /sw/lib/python$PY_VERSION/site-packages ; then
    export PYTHONPATH=$PYTHONPATH:/sw/lib/python$PY_VERSION/site-packages
  fi

  if test -d /sw/lib/python$PY_VERSION/site-packages/PyQt4 ; then
    export PYTHONPATH=$PYTHONPATH:/sw/lib/python$PY_VERSION/site-packages/PyQt4
  fi
}

##______________________________________________________________________________
## Define the AIPSPATH environment variable, used to locate measures data

if test -z "$AIPSPATH" ; then
  export AIPSPATH=$LOFARSOFT
fi

##______________________________________________________________________________
## Define the Pulsar software environment variables

if test  ! -z "$TEMPO"  -a  "$TEMPO" != "${LOFARSOFT}/release/share/pulsar/bin" ; then
  echo "-- Warning, resetting your TEMPO environment variable from:"
  echo "           $TEMPO to ${LOFARSOFT}/release/share/pulsar/bin"
fi
export TEMPO=${LOFARSOFT}/release/share/pulsar/bin

if test ! -z "$PRESTO" -a  "$PRESTO" != "${LOFARSOFT}/release/share/pulsar" ; then
  echo "-- Warning, resetting your PRESTO environment variable from:"
  echo "           $PRESTO to ${LOFARSOFT}/release/share/pulsar"
fi
export PRESTO=${LOFARSOFT}/release/share/pulsar

#############################################################################
#                    Finished configuration                                 #  
#############################################################################

fi # end of: if test -z "$LOFARSOFT" ; then
