#!/usr/bin/env python
'''
Script that folds pulsar candidates found by the PRESTO acceleration search.

Note : still needs a lot better sifting algorithm, the prepfold commands are
generated like the prepfold command that the GBT drift scan produces.
'''

# Standard library imports:
import os
import sys
import glob
# PRESTO imports
import sifting
# matplotlib imports
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot

# LOFAR PILOT PULSAR SURVEY imports
from lpps_search import crawler
from lpps_search import inf


def plot_p_histogram(candidates, full_path):
    l = [c.p for c in candidates]
    pyplot.clf()
    pyplot.hist(l, 1000, histtype='step')
    pyplot.xlabel('Period (s)')
    pyplot.ylabel('Number of candidates')
    pyplot.savefig(full_path)
    pyplot.clf()

def plot_f_histogram(candidates, full_path):
    l = [c.f for c in candidates]
    pyplot.clf()
    pyplot.hist(l, 1000, histtype='step')
    pyplot.xlabel('Frequency (Hz)')
    pyplot.ylabel('Number of candidates')
    pyplot.savefig(full_path)
    pyplot.clf()

# TODO: Split sift_accel_cands into 2 steps (reading and sifting) to 
# facilitate plotting the candidate period and candidate frequency historgrams
# before and after sifting.

def sift_accel_cands(cand_dir, basename):
    '''Sift through the candidate pulsars found by accelsearch.'''
    crawl_results = crawler.find_accelsearch_output(cand_dir, basename)
    # Reorder the results to send them to PRESTO's sifting module (it needs a
    # list of DM values as strings).
    accel_cands_found_for = {}
    for dm, z_dict in crawl_results.iteritems():
        for z_max, files in z_dict.iteritems():
            # An succesful run of accelsearch that found candidates leaves
            # behind 3 files. Ignore all other cases:
            if len(files) != 3: continue
            # For each value of z_max keep a list of trial DMs that have 
            # acceleration search candidates:
            try:
                accel_cands_found_for[z_max].append(dm)
            except KeyError, e:
                accel_cands_found_for[z_max] = [dm]
   
    # For each value of z_max, sift the candidates that were found and put
    # them all in one big list:
    unsifted_accelcands = []
    sifted_accelcands = []
    for z_max, accel_cands in accel_cands_found_for.iteritems():
        # Read all the acceleration candidates:
        files = glob.glob(os.path.join(cand_dir, 
            basename + '_DM*.*_ACCEL_%d' % z_max))
        accel_cands = sifting.read_candidates(files)
        unsifted_accelcands.extend(accel_cands)
        # The sifting module also needs all the trial DM values 
        dm_strs = ['%.2f' % dm for dm in accel_cands_found_for[z_max]]
        
        # Look in all the output candidates sift them using the PRESTO sifting
        # module.
        
        if accel_cands:
            accel_cands = sifting.remove_duplicate_candidates(accel_cands)
        if accel_cands:
            accel_cands = sifting.remove_DM_problems(accel_cands, 2, dm_strs, 1.)
        if accel_cands:
            accel_cands.sort(sifting.cmp_sigma)
        # PRESTO seems to return None in stead of an empty list
        if accel_cands == None: accel_cands = [] 
        MAX_CANDIDATES = 10
        sifted_accelcands.extend(accel_cands[:MAX_CANDIDATES])
    return unsifted_accelcands, sifted_accelcands 

