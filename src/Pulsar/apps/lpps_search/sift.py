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
import copy
# PRESTO imports
import sifting
# matplotlib imports
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot
import psr_utils

# LOFAR PILOT PULSAR SURVEY imports
from lpps_search import crawler
from lpps_search import inf
from lpps_search import knownpulsar
from lpps_search import duplicates 

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

def plot_p_dm(candidates, bright_pulsars, full_path):
    c_dms = [c.DM for c in candidates]
    c_ps = [c.p for c in candidates]
    bp_dms = [bp.dm for bp in bright_pulsars]
    bp_ps = [bp.p0 for bp in bright_pulsars]
    pyplot.clf()
    pyplot.scatter(bp_ps, bp_dms, marker='s')
    pyplot.scatter(c_ps, c_dms, marker='x')
    pyplot.xlabel('Period (s)')
    pyplot.ylabel('Dispersion measure (cm^-3 pc)')
    pyplot.savefig(full_path)
    pyplot.clf()

# TODO : check the layout of the zaplist files < - DONE (with Jason)
# Assumption is now:
# column 0 : center frequency
# column 1 : frequency width (f_width = f_hi - f_lo)


def read_zaplist_file(zaplist_file):
    '''
    Read the contents of a zaplist file.
    '''
    birdies = []
    try:
        f = open(zaplist_file)
        try:
            for line in f:
                if line and line[0] == '#': continue
                tmp = line.split() 
                if len(tmp) >= 2: # in case harmonics are also listed?!
                    try: 
                        f_center = float(tmp[0])
                        f_width = float(tmp[1])
                    except ValueError, e:
                        pass
                    else:
                        birdies.append((f_center, f_width))
        finally:
            f.close()
    except OSError, e:
        print 'Failed to openm %s' % zaplist_file
        raise
    return birdies


def apply_zap_list(cand_list, zaplist_file):
    '''
    Remove bad rotational frequencies from the list of candidates. (To remove
    known RFI frequencies.)
    '''
    birdies = read_zaplist_file(zaplist_file)
    out_cand_list = []

    for c in cand_list:
        is_rfi = False
        for f_center, f_width in birdies:
            f_lo = f_center - (f_width / 2)
            f_hi = f_center + (f_width / 2)
            if f_lo <= c.f < f_hi:
                is_rfi = True
                break
        if not is_rfi:
            out_cand_list.append(c)
    return out_cand_list
            

def filter_on_effective_time_resolution(candidates, metadata, n_bins):
    '''
    Reject pulsar candidates with fewer than n_bins bins across their profile.
    '''
    out_cand_list = []
    channel_bandwidth = metadata.channel_bandwidth
    central_frequency = ((metadata.n_channels/2) - 0.5) * channel_bandwidth +\
        metadata.low_channel_central_freq
    for c in candidates:
        intra_channel_smearing = psr_utils.dm_smear(c.DM, channel_bandwidth,
            central_frequency)
        if c.p / intra_channel_smearing > n_bins:
            out_cand_list.append(c)
    print 'Filtering on effective time resolution:' 
    print '  Removed %d out %d candidates.' % (
        len(candidates) - len(out_cand_list), len(candidates))
    return out_cand_list 
            
            
# TODO: Split sift_accel_cands into 2 steps (reading and sifting) to 
# facilitate plotting the candidate period and candidate frequency historgrams
# before and after sifting.

def sift_accel_cands(cand_dir, basename, **kwargs): 
    '''Sift through the candidate pulsars found by accelsearch.'''

    # TODO : fix the line below to move the DM to some central settings area
    n_candidates_cutoff = kwargs.get('n_candidates_cutoff', 1)
    minimum_dm_cutoff = kwargs.get('minimum_dm_cutoff', 1)
    zaplist_file = kwargs.get('zaplist_file', '') 
    metadata = kwargs.get('metadata', None)

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
    # TODO : fix the case where the script crashes because there are not
    # enough candidates.
    for z_max in accel_cands_found_for.keys():
        # Read all the acceleration candidates:
        files = glob.glob(os.path.join(cand_dir, 
            basename + '_DM*.*_ACCEL_%d' % z_max))
        accel_cands = sifting.read_candidates(files)
        unsifted_accelcands.extend(accel_cands)
        # The sifting module also needs all the trial DM values 
        dm_strs = ['%.2f' % dm for dm in accel_cands_found_for[z_max]]
        
        # Look in all the output candidates sift them using the PRESTO sifting
        # module.
        print 'Number of candidates %d' % len(accel_cands)        
        if accel_cands:
            accel_cands = sifting.remove_duplicate_candidates(accel_cands)
        print 'Number of candidates %d' % len(accel_cands)        
        if accel_cands:
            accel_cands = sifting.remove_DM_problems(accel_cands, 2, dm_strs, minimum_dm_cutoff)
        print 'Number of candidates %d' % len(accel_cands)        
        if accel_cands:
            # The PRESTO sifting.py module can crash if there are not enough
            # pulsar candidates, below I test for those symptoms and because
            # the PRESTO sifting.remove_harmonics overwrites its inputs I 
            # keep a temporary copy around in case there is a crash. 
            # TODO : check whether this fix does not create problems further
            # down in the sifting process. If there are more problems it is 
            # probably better to discard the candidates and write that to the
            # log.
            tmp_accel_cands = copy.deepcopy(accel_cands)
            try:
                accel_cands = sifting.remove_harmonics(accel_cands)
            except IndexError, e:
                accel_cands = tmp_accel_cands
        print 'Number of candidates %d' % len(accel_cands)        
        if accel_cands:
            bpc = knownpulsar.load_bright_pulsar_catalog()
            accel_cands = knownpulsar.remove_bright_pulsars(accel_cands, bpc)
        print 'Number of candidates %d' % len(accel_cands)        
        if accel_cands:
            accel_cands = duplicates.remove_duplicates(accel_cands)
        print 'Number of candidates %d' % len(accel_cands)        
        sifted_accelcands.extend(accel_cands)
    sifted_accelcands = duplicates.remove_duplicates(sifted_accelcands)
    
    if sifted_accelcands:
        sifted_accelcands.sort(sifting.cmp_sigma)
    # PRESTO seems to return None in stead of an empty list, fix that:
    if sifted_accelcands == None: sifted_accelcands = []
    # Apply zaplist to candidates (used if stand alone sifting script is used).
    if zaplist_file:
        print 'Before applying zaplist there are %d candidates.' % len(sifted_accelcands)
        sifted_accelcands = apply_zap_list(sifted_accelcands, zaplist_file)
        print 'After applying zaplist there are %d candidates.' % len(sifted_accelcands)
    # Filter the candidates by checking their effective timeresolution, we
    # want at least 6 bins across the pulse profile.
    if metadata:
        sifted_accelcands = filter_on_effective_time_resolution(
            sifted_accelcands, metadata, 6)
    else:
        print 'No metadata - no filtering on effective time resoultion!'
    # optional cut on DM
    if minimum_dm_cutoff:
        assert minimum_dm_cutoff >= 0
        sifted_accelcands = [c for c in sifted_accelcands if c.DM >= minimum_dm_cutoff]        
 
    if n_candidates_cutoff:
        # We want an integer that is larger than 0 if a cutoff is specified.
        assert n_candidates_cutoff > 0 and (type(n_candidates_cutoff) == type(1))
        sifted_accelcands = sifted_accelcands[:n_candidates_cutoff]
       
    return unsifted_accelcands, sifted_accelcands 

