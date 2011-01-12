#!/usr/bin/env python
'''
PRESTO style single pulse plotter, crawls directory, makes plots.
'''
from __future__ import division
import optparse
import os
import shutil
from lpps_search import crawler
from lpps_fold_script import MissingOptionException, DirectoryNotEmpty 
from lpps_fold_script import WrongPermissions,MissingCommandLineOptionException


REQUIRED_OPTIONS = ['indir', 'outdir', 'basename']
OPTIONAL_OPTIONS = ['dm_list']

N_DMS = 1000
OVERLAP_FRACTION = 0.1

class TooManyDMValues(Exception):
    def __init__(self, n, n_max):
        self.n = n
        self.n_max = n_max

    def __str__(self):
        return 'Too many DM trials (%d whilst max is %d).' % (
            self.n, self.n_max)

def make_safe_dm_ranges(dms, n_dms, overlap_fraction):
    '''
    Create ranges of DMs to use for the PRESTO singlepulse plots.

    Note : ranges are expressed as indices of the dms list of trial DM values,
    the dms parameter is assumed to be sorted. This function is the safe
    default for the plotting.
    '''
    # Check that no more than the maximum amount of DMs are asked for and that
    # the overlap between the plots is no more than 10%.
    if n_dms > N_DMS: 
        n_dms = N_DMS
    if not 0 >= overlap_fraction >= OVERLAP_FRACTION: 
        overlap_fraction = OVERLAP_FRACTION
    
    overlap = int(n_dms * overlap_fraction)
    min_i = 0
    max_i = n_dms 
    command_list = []

    indices = []
    # Find all the ranges of indices
    while max_i < len(dms):
        indices.append((min_i, max_i))
        min_i += n_dms - overlap 
        max_i += n_dms - overlap
    else:
        max_i = len(dms) - 1
        indices.append((min_i, max_i)) 

    return indices

def make_requested_dm_ranges(dms, dm_list, overlap_fraction):
    '''
    Take a list of DMs to use as ranges in DM for single pulse plots.

    Note: the dms parameter is the total list of trial DMs in the data, it is
    expected to be sorted. The dm_list parameter is a list of DM values 
    between which the plots need to be created. The overlap_fraction parameter
    sets the amount of overlap (in DM) between the consecutive plots.
    
    Raises an Exception is the chosen DM ranges would need to many trial DMs
    as set by the global variable N_DMs.
    '''
    indices = []
    dm_list.sort()
    assert len(dm_list) > 1
    dm_ranges = []

    # Create the DM ranges requested with overlap_fraction overlap:
    for i in range(len(dm_list) -1):
        min_dm = dm_list[i]
        max_dm = dm_list[i + 1]
        w = max_dm - min_dm
        assert w > 0
        m = (min_dm + max_dm) / 2 - (1 + overlap_fraction) * w / 2
        M = (min_dm + max_dm) / 2 + (1 + overlap_fraction) * w / 2
        dm_ranges.append((m, M))       

    plot_counter = 0
    # Figure out which indices are in each of the specified DM ranges:
    for min_dm, max_dm in dm_ranges:
        # Initialize the indices to something guaranteed to be out of range.
        min_i = len(dms)
        max_i = -1
        # Which indices are in range:
        for i, dm in enumerate(dms):
            if dm < min_dm: continue
            elif min_dm <= dm <= max_dm:
                if i < min_i: min_i = i
                if i > max_i: max_i = i
            elif dm > max_dm: break
        # Check whether either min_i or max_i was not updated (because the
        # edges of the dm_range were out of range).
        if min_i == len(dms):
            # For this case all the trial DMs were at lower DM than the DMs
            # in the requested DM range. (i.e. ignore this)
            pass
        elif max_i == -1: 
            # For this case only the upper limit was not updated, so the
            # upper index needs to be set to the highest possible value.
            max_i = len(dms) - 1
            indices.append((min_i, max_i))
        else:
            indices.append((min_i, max_i))
        if max_i - min_i + 1 > N_DMS:
            print 'Too many DM trials in DM range [%.2f, %.2f] .' % (
                dms[min_i], dms[max_i])
            print 'That DM range was constructed from [%.2f, %.2f] .'% (
                dm_list[plot_counter], dm_list[plot_counter + 1])
            raise TooManyDMValues(max_i - min_i + 1, N_DMS)
        plot_counter += 1

    return indices        

def main(*args, **kwargs):
    for k in REQUIRED_OPTIONS:
        if not kwargs.has_key(k):
            raise MissingOptionException(k)
    in_dir = os.path.abspath(kwargs['indir'])
    out_dir = os.path.abspath(kwargs['outdir'])
    basename = kwargs['basename'] 
    dm_list = kwargs.get('dm_list', [])
    print 'Looking at %s' % in_dir 
    tmp = crawler.find_single_pulse_search_output(in_dir, basename) 
    dms = list(tmp.keys())
    assert len(dms) > 0
    dms.sort()

    if dm_list:
        try:
            indices = make_requested_dm_ranges(dms, dm_list, 0.1)    
        except TooManyDMValues, e:
            print e
            print 'Requested DM range for the plots needs too many DMs.'
            print 'Ignoring requested ranges, falling back to %d trial DM plots.'\
                % (N_DMS)
            indices = make_safe_dm_ranges(dms, N_DMS, 0.1)
    else:
        indices = make_safe_dm_ranges(dms, N_DMS, 0.1)

    # Run single_pulse_plot.py as appropriate for the DM ranges previously
    # determined:
    os.chdir(in_dir)
    for min_i, max_i in indices:
        out_file = '%s_DM%.2f_%.2f_singlepulse.ps' % \
            (basename, dms[min_i], dms[max_i])
        files = [tmp[dms[i]] for i in range(min_i, max_i + 1)]
        cmd = 'single_pulse_search.py %s' % (' '.join(files))
        print 'Running command : %s' % cmd
        os.system(cmd)
        print 'Moving the plot to %s as %s.' % (out_dir, out_file) 
        shutil.move(basename + '_singlepulse.ps', os.path.join(out_dir, 
            out_file))


if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('--indir', help='Input directory.', type='string',
        metavar='INDIR')
    parser.add_option('--outdir', help='Output directory.', type='string',
        dest='outdir', metavar='OUTDIR')
    parser.add_option('--basename', help='Basename for data.', type='string',
        dest='basename', metavar='BASENAME')
    parser.add_option('--dm_list', dest='dm_list', metavar='DM_LIST', 
        type='string', help='Comma separated list of DM values in [] braces.',
        default='[]')   
    options, args = parser.parse_args()

    dm_list = []
    if dm_list != None:
        if options.dm_list[0] == '[' and options.dm_list[-1] == ']':
            tmp = options.dm_list[1:-1].split(',')
            try:
                for dm_str in tmp:
                    if not dm_str: continue
                    dm_list.append(float(dm_str))
            except ValueError, e:
                print 'List of dm values not specified correctly.'
                raise
        else:
            print 'Z values list needs to be inside of [] and contain no spaces.'
            raise Exception()
        options.dm_list = dm_list # duck punching ... (ugly) 
    kwargs = {}
    for k in REQUIRED_OPTIONS:
        v = getattr(options, k)
        if v == None:
            print 'You need to supply the --%s option' % k
            raise MissingCommandLineOptionException(k)
        else:
            kwargs[k] = v
    for k in OPTIONAL_OPTIONS:
        try:
            v = getattr(options, k)
        except KeyError, e:
            pass
        else:
            kwargs[k] = v

    main(**kwargs)

