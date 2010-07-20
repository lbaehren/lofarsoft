#!/bin/bash

# This is a dummy shell script which accepts a list as input and 
# echo's that input to STDOUT;  if a list of input is not given
# an error message is echo'd to STDERR.

echo "Starting script dummy_echo.sh at " `date`

numfiles=0
numfiles=$#

# If there are no input arguments, then write an error message to STDERR (note, STDERR here!)
if (( $numfiles < 1 )) 
then
   echo "Error: dummy_echo.sh requires a list of files as input (one file is the minumum set)" >&2 
   exit 1
fi

filenames=$@

## Turn on various echo statements depending on what you want to print to STDOUT
echo "The number of files is: " $numfiles
echo "The file names are: " $filenames
echo "These same file name can be echo'd one by one to STDOUT:"
for file in $filenames
do
   echo $file
done

echo "Ended script dummy_echo.sh successfully at " `date`

exit 0

