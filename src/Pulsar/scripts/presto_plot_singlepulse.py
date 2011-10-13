#!/usr/bin/env python
import optparse
import os
import sys
from lpps_search.util import symlink_matching, remove_matching
from lpps_search.crawler import get_basename

# Hardcoded time intervals to plot (in minutes!):
TIMES = [(0, 10), (5, 15), (10, 20), (15, 25), (20, 30), (25, 35), (30, 40),
    (35, 45), (40, 50), (45, 55), (50, 60)]
TIMES = [(a * 60, b * 60) for a, b in TIMES]
# Hardcoded DM ranges to make plots for.
DM_RANGES = [
    (0, 10, ['_DM[0-9].??']), 
    (8, 30, ['_DM[8-9].??', '_DM[1-2]?.??']), 
    (28, 60, ['_DM2[8-9].??', '_DM[3-5]?.??']), 
    (58, 100, ['_DM5[8-9].??', '_DM[6-9]?.??']), 
    (98, 200, ['_DM9[8-9].??', '_DM1??.??']), 
    (198, 500, ['_DM19[8-9].??', '_DM[2-4]??.??']), 
    (498, 1000, ['_DM49[8-9].??', '_DM[5-9]??.??']), 
    (998, 3000, ['_DM99[8-9].??', '_DM[1-3]???.??']),
]

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('--searchoutdir', dest='sod', metavar='SEARCH_OUT_DIR',
        type='string')
    options, args = parser.parse_args()
    sod = os.path.abspath(options.sod)
    # Figure out basename for observation:
    basename = get_basename(os.path.join(sod, 'SINGLEPULSE'), r'^(?P<basename>\S+)_DM\d+\.\d{2}\.singlepulse$')   
    # Symlink .inf files into SEARCH_OUT_DIR/SINGLEPULSE:
    remove_matching(os.path.join(sod, 'SINGLEPULSE'), r'\S+\.inf$') # to recover from possible previous problems
    symlink_matching(os.path.join(sod, 'INF'), os.path.join(sod, 'SINGLEPULSE'), r'\S+\.inf$')
    cwd = os.getcwd()
    os.chdir(os.path.join(sod, 'SINGLEPULSE'))
    for ts, te in TIMES:
        for dm_lo, dm_hi, glob_chunks in DM_RANGES:
            globpattern = ' '.join(['%s%s.singlepulse' % (basename, g) for g in glob_chunks])
            cmd = 'single_pulse_search.py -s %(s)d -e %(e)d -t %(t).2f %(g)s' % \
                {'t' : 5.5, 'g' : globpattern, 's' : ts, 'e' : te,}
            print 'Running command :', cmd
            os.system(cmd)
            tempname = '%s_%d-%ds_singlepulse.ps' % (basename, ts, te)
            finalname = '%s_%d-%ds_DM%d_%d_singlepulse.ps' % (basename, ts, te, dm_lo, dm_hi)
            cmd = 'mv %s %s' % (tempname, finalname)
            print 'Running command :', cmd
            os.system(cmd)
    os.chdir(cwd)
    remove_matching(os.path.join(sod, 'SINGLEPULSE'), r'\S+\.inf$')
