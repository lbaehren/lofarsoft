#!/bin/csh
# This script updates the include file sigproc.h for the sigproc library
set v = `awk '{print $1}' version.history | tail -2 | head -1`
echo "/* sigproc.h: Automatically generated include file for sigproc-$v */" \
	                              >! sigproc.h
echo \#include \"polyco.h\"           >> sigproc.h
echo \#include \"epn.h\"              >> sigproc.h
echo \#include \"version.h\"          >> sigproc.h
grep includefile *.c | awk -F"/*" '{print $1";"}' \
	| awk -F: '{print $2}' | sort >> sigproc.h
chmod 666 sigproc.h
exit
