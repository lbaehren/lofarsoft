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


#############################################################################
#
# Check for the existence of the LOFARSOFT  environment variable
#
#############################################################################
if(${?LOFARSOFT} == 0) then
   echo "ERROR:  Please set the LOFARSOFT environment variable"
   exit
else
   echo "Initializing LOFAR software environment"
endif


#############################################################################
#
# Add the LOFAR executables to the path
#
#############################################################################
set path = ( $path $LOFARSOFT/release/bin )


#############################################################################
#
# Add the Python libraries to the existing search path
#
#############################################################################
if ($?PYTHONPATH) then
   if ( $PYTHONPATH != $LOFARSOFT/release/lib ) then   
      setenv PYTHONPATH ${PYTHONPATH}:${LOFARSOFT}/release/lib
   endif
else 
   setenv PYTHONPATH ${LOFARSOFT}/release/lib
endif


#############################################################################
#                    Finished configuration                                 #  
#############################################################################

