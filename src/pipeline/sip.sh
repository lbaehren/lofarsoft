#!/bin/bash

PIPELINECMD=/opt/pipeline/scipts/mac-sip.py
INPUTDATA=/paths/to/files

parset=$1
target_host=$2
service_name=$3
cntlr_name=$4

treeid=`echo $parset | sed -e's/.*[a-zA-Z]\+\([0-9]\+\)/\1/'`
export PYTHONPATH=/opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/lofar/lib/python2.5/site-packages:/opt/pythonlibs/lib/python/site-packages
export LD_LIBRARY_PATH=/opt/pipeline/dependencies/lib:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/casacore/lib:/opt/LofIm/daily/lofar/lib:/opt/wcslib/lib/:/opt/hdf5/lib:/opt/LofIm/daily/casarest/lib:/data/sys/opt/lofar/external/log4cplus/lib

python $PIPELINECMD $INPUTDATA --controllername $cntlr_name --servicemask $service_name --treeid $treeid --targethost $target_host -j 213 -d
