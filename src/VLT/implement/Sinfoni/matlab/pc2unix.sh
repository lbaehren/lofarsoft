#!/bin/sh
#
# When run on a PC, Matlab prints floating point numbers using three digits 
# for the exponent. On other platforms (e.g. linux), Matlab uses two digits. 
# Furthermore, Matlab will use "Inf" and "NaN", whereas C++ uses "inf" and
# "nan". 
# This shell script will convert all characters to lowercase, and it will
# remove the third digit whenever it is redundant (i.e. whenever it is zero).
#

if [ $# -lt 1 ]; then
  echo "Usage: `basename $0` <input-file> [<output-file>]"
  exit 1
fi

infile=$1
outfile=$1
tmpfile=/tmp/`basename $infile`

if [ $# -gt 1 ]; then
  outfile=$2
fi

sed -e 's/Inf/inf/g' -e 's/NaN/nan/g' \
    -e 's/e\([+-]\)0\([0-9][0-9]\)/e\1\2/g' $infile > $tmpfile

mv $tmpfile $outfile

