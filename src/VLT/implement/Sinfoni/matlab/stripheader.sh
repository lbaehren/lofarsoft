#!/bin/sh
#
# Strip the header from an ASCII data file.
# A header consists of one or more lines starting with a "#" character.

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

sed '/^#/d' $infile > $tmpfile

mv $tmpfile $outfile

