#!/bin/sh

# Checks if the content of the new def.h file  (.def.h.new) differs from the
# original one (.def.h).

# This is needed in order not to regenerate the python bindings if the
# interface is not changed.

if test -f $1.def.h.latest ; then
#    echo "$1.def.h.latest exists"
    if test -f $1.def.h ; then
#	echo "$1.def.h exists"
	if test "`diff $1.def.h.latest $1.def.h|wc -l`" -ne "0" ; then
#	    echo "differences found!"
	    cp $1.def.h.latest $1.def.h
	fi
    else
#	echo "Creating $1.def.h"
	cp $1.def.h.latest $1.def.h
    fi
fi

