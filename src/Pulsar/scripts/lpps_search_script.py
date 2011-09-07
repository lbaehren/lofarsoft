#!/usr/bin/env python
'''
MSSS pulsar search script, based on the search script for the GBT driftscan
and Vishal Gajjar's work on blind searching of LOFAR data. 

By Thijs Coenen (october 2010)


This module contains a class SearchRun that implements the pulsar search
pipeline. When you instantiate it, it will check whether the input data 
matches expectation (the input data directory should point to one of the 
RSP* directories containing the .subXXXX files and a .sub.inf file), 
whether the output and working directories are empty and writeable.

You make a list of DedispPlan instances describing your desired dedipersion
plan and pass that to the run_search method of your SearchRun instance. The 
run_search method also take the number of available processor cores as an
optional argument. The run_search method then does all the work. The first
thing it does is call the rebatch method that figures out how to call 
prepsubband and mpiprepsubband, this step creates a new list of DedispPlan 
instances with all the constraints that prepsubband and mpiprepsubband put om
a dedispersion plan taken into account (the actual logic that figure out the
constraints is in the mpiprepsubband_helper and prepsubband_helper functions).
Each step in the pulsar search pipeline  can be found in the run_search method
and each step that corresponds to a call out to a PRESTO executable is wrapped
in a Python function (see for instance the run_rfifind and run_prepsubband 
functions). 

The way commandlines are actually built can be found in the get_command and 
run_command functions. 

'''

# Standard library imports:
from __future__ import division
import optparse
import sys
import os
import errno
from os.path import join
import re
import copy
import glob
import math
import subprocess
import shutil
import time
import logging
import stat
import pprint

# Common python library imports:
import numpy
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot

# PRESTO / PSR UTILS / LOFAR PULSAR library imports:
import presto
import sifting
import psr_utils

# TODO : commit fixed .inf reader to ssps Git repository. 
#from ssps.presto.files.inf import inf_reader
from lpps_search.inf import inf_reader, ra2ascii, dec2ascii
import lpps_search.crawler as crawler
import lpps_search.fold as folder
from lpps_search.fold import get_folding_command_ke
from lpps_search.util import create_script, run_as_script
from lpps_search.util import get_command, run_command
from lpps_search.util import DirectoryNotEmpty, WrongPermissions
from lpps_search import crawler
from lpps_search.bestprof import parse_pfd_bestprof_file 

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
MAX_BATCH_SIZE = 1000
MAX_BATCH_SIZE_MPI = 1000

class DedispersionPlan(object):
    def __init__(self, *args, **kwargs):
        self.batches = []
        self.name = kwargs.get('name', 'NOT NAMED')

    def add_batch(self, lo_dm, dm_step, n_dms, downfact, n_sub):
        self.batches.append((lo_dm, dm_step, n_dms, downfact, n_sub))

    def get_dms(self):
        out = []
        for lo_dm, dm_step, n_dms, downfact, n_sub in self.batches:
            for i in range(n_dms):
                out.append(lo_dm + i * dm_step)
        return out

def create_batches(dedispersion_plan, n_cores):
    '''
    Take a DedispersionPlan instance and return DMBatch instances.
    '''
    if n_cores > 1:
        use_mpi = True
        n_available = n_cores - 1
    else:
        use_mpi = False
        n_available = 1

    if use_mpi:
        dms_per_batch = (MAX_BATCH_SIZE_MPI // n_available) * n_available
    else:
        dms_per_batch = MAX_BATCH_SIZE

    out = []
    for lo_dm, dm_step, n_dms, downfact, n_sub in dedispersion_plan.batches:
        while n_dms >= dms_per_batch:
            nb = DMBatch(lo_dm, dm_step, dms_per_batch, downfact, n_sub,
                n_available, use_mpi)
            out.append(nb)
            lo_dm += dm_step * dms_per_batch
            n_dms -= dms_per_batch
        if use_mpi and n_dms >= n_available:
            n = n_dms // n_available            
            nb = DMBatch(lo_dm, dm_step, n * n_available, downfact, n_sub, 
                n_available, use_mpi)
            out.append(nb)
            lo_dm += n * n_available * dm_step
            n_dms -= n * n_available
        if n_dms > 0:
            nb = DMBatch(lo_dm, dm_step, n_dms, downfact, n_sub, n_dms, use_mpi)
            out.append(nb)
    return out 

class DMBatch(object):
    def __init__(self, lo_dm, dm_step, n_dms, downfact, n_sub, n_cores, 
        use_mpi):

        self.lo_dm = lo_dm
        self.dm_step = dm_step
        self.n_dms = n_dms
        self.downfact = downfact
        self.n_sub = n_sub
        self.n_cores = n_cores
        self.use_mpi = use_mpi

        self.dedispersion_time = 0
        self.search_time = 0

    def get_dms(self):
        return [self.lo_dm + i * self.dm_step for i in range(self.n_dms)]


# ----------------------------------------------------------------------------
N_CANDIDATES_CUTOFF = 20
MINIMUM_DM_CUTOFF = 0
# ----------------------------------------------------------------------------
# -- Utility functions from GBT or Vishal's scripts --------------------------

class DedispPlan(object):
    """
    class dedisp_plan(lodm, dmstep, dmsperpass, numpasses, numsub, downsamp)
       A class describing a de-dispersion plan for prepsubband in detail. 
       Imported from GBT_search pipe-line.       
    """
    def __init__(self, lodm, dmstep, dmsperpass, numpasses, numsub, downsamp):
        self.lodm = float(lodm)
        self.dmstep = float(dmstep)
        self.dmsperpass = int(dmsperpass)
        self.numpasses = int(numpasses)
        self.numsub = int(numsub)
        self.downsamp = int(downsamp)
        self.sub_dmstep = self.dmsperpass * self.dmstep
        self.dmlist = []  # These are strings for comparison with filenames
        self.subdmlist = []
        for ii in range(self.numpasses):
             self.subdmlist.append("%.2f"%(self.lodm + (ii+0.5)*self.sub_dmstep))
             lodm = self.lodm + ii * self.sub_dmstep
             dmlist = ["%.2f"%dm for dm in \
                       numpy.arange(self.dmsperpass)*self.dmstep + lodm]
             self.dmlist.append(dmlist)

        self.prepsubband_time = None
        self.search_time = None

    def get_dms(self):
        '''
        Return a list of dm values covered by this DedispPlan instance.
        '''
        return [self.lodm + self.dmstep * i  for i in range(self.dmsperpass)]

def get_baryv(ra, dec, mjd, T, obs="LF"):
    """ 
    get_baryv(ra, dec, mjd, T):
        Determine the average barycentric velocity towards 'ra', 'dec'
        during an observation from 'obs'.  The RA and DEC are in the
        standard string format (i.e. 'hh:mm:ss.ssss' and 'dd:mm:ss.ssss').
        'T' is in sec and 'mjd' is (of course) in MJD.
    """
    tts = psr_utils.span(mjd, mjd+T/86400.0, 100)
    nn = len(tts)
    bts = numpy.zeros(nn, dtype=numpy.float64)
    vel = numpy.zeros(nn, dtype=numpy.float64)
    presto.barycenter(tts, bts, vel, nn, ra, dec, obs, "DE200")
    return vel.mean()


# ----------------------------------------------------------------------------

def read_bad_observing_frequencies(rfi_file):
    '''Read system wide master.rfi file. (Note: path hardcoded for now!)'''

    bad_freq_ranges = []
    
    try:
        f = open(rfi_file, 'r')
        try:
            # parse master.rfi file:
            # assumes that comments are allowed on lines that start with #
            # assumes that there are the following numbers for each line
            # 1 the middle of the bad frequency range
            # 2 the width in frequency of that bad observing frequency range
            # ignores anything else
            for l in f:
                if l[0] == '#': continue
                tmp = l.split()
                if len(tmp) != 2: continue
                try:
                    middle = float(tmp[0])
                    width = float(tmp[1])
                except ValueError, e:
                    pass
                else:
                    bad_freq_ranges.append((middle - width, middle + width))
        finally:
            f.close()
    except IOError, e:
        print 'Failed to open :', rfi_file
 
    return bad_freq_ranges

def intersect_channel_map_with_freq_range(bad_freq_mapping, freq_range):
    '''Intersect channel index 2 frequency range mapping with another range.'''
    indexes = []
    for channel_idx, fr in bad_freq_mapping.iteritems():
        if fr[1] >= freq_range[0] and fr[0] <= freq_range[1]:
            indexes.append(channel_idx)
    return indexes


def determine_numout(n_bins):
    '''
    Determine an appropriate length for the dedispersed timeseries.
    
    Note : an appropriate length is one that is easily factorisable in 
    factors of 2, 3, 5, 7 and 11. (This speads up the Fast Fourier
    Transforms, i.e. the realfft calls.)
    '''
    # Needs the itertools module from Python 2.6, since the LOFAR cluster
    # has Python 2.5 installed, here is a little code copied from the 
    # Python documenation:
    # (Retrieved from http://docs.python.org/library/itertools.html on
    # october 25 2010.)
    
    def combinations_with_replacement(iterable, r):
        # combinations_with_replacement('ABC', 2) --> AA AB AC BB BC CC
        pool = tuple(iterable)
        n = len(pool)
        if not n and r:
            return
        indices = [0] * r
        yield tuple(pool[i] for i in indices)
        while True:
            for i in reversed(range(r)):
                if indices[i] != n - 1:
                    break
            else:
                return
            indices[i:] = [indices[i] + 1] * (r - i)
            yield tuple(pool[i] for i in indices)       

    # Find the length of the data, then see whether it is easily 
    # factorisable, if not pad the data a little so that the data has an
    # easily factorisable length.

    # If the data is very short just use a minimum length of 1024 .
    # Note : this 1024 should be larger than the maxmimum downsample
    # factor from the dedispersion plan.
    if n_bins < 1024:
        return 1024
    # If the number of bins is already a power of 2 just return it.
    if (n_bins != 0) and (n_bins & (n_bins - 1) == 0):
        return n_bins 
    # Find a length that is slightly larger than the data length that is
    # easily factorisable. 
    else:
        p = math.log10(n_bins) / math.log10(2)
        base = 2 ** int(p - 5)
        best_trial = n_bins * n_bins
        for t in combinations_with_replacement((1,2,3,5,7,11), 20):
            trial = base
            for i in t:
                trial *= i
            if trial >= n_bins and trial < best_trial:
                best_trial = trial
        return best_trial

# ----------------------------------------------------------------------------
# -- Individual steps in the pipeline (functions wrapping parts of presto   --
# -- whether they are python modules or system calls does not matter).      --
# ----------------------------------------------------------------------------

def symlink_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Symlink matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            os.symlink(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Symlinked %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def move_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Move matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            shutil.move(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Moved %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def copy_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Copy matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            shutil.copy(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Copied %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def remove_matching(target_dir, file_regexp):
    file_pattern = re.compile(file_regexp)
    files = os.listdir(target_dir)
    print 'Remove matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            os.remove(join(target_dir, f))
        else:
            n_not_matched += 1
    print 'Removed %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

 
def run_rfifind(subband_dir, basename, work_dir, *args, **kwargs):
    '''
    Run PRESTO rfifind.
    '''
    bad_channels = kwargs.get('bad_channels', None)
    clean_up = kwargs.get('clean_up', True)
    # temporarily store the working directory
    CWD = os.getcwd()
    try:
        # jump to rfifind working directory (create if necessary)
        try:
            os.makedirs(work_dir)
        except OSError, e:
            if e.errno != errno.EEXIST:
                print 'Creating the working directory for rfifind failed!'
                raise
        os.chdir(work_dir)
        # symlink subband files to rfifind working dir
        try:
            symlink_matching(subband_dir, work_dir, '^' + re.escape(basename) + '\.sub\d+$')
        except OSError:
            print 'Could not symlink subband files to working rfifind working directory.'
            raise
        # symlink .inf file to rfifind working dir
        try:
            symlink_matching(subband_dir, work_dir, '^' + re.escape(basename) + '\.sub\.inf$')
        except OSError:
            print 'Could not symlink .sub.inf file to rfifind working directory.'
            raise
        # create a globpattern to pass to rfifind that grabs all the subband files
        subband_globpattern = basename + '.sub[0-9]???'
        # set all the commandline options
        options = {
            '-blocks' : str(512),
            '-o' : basename,
        }
        if bad_channels:
            options['-zapchan'] = ','.join([str(idx) for idx in bad_channels]) 
        # run rfifind (taking care to check the exit status)
        status = run_command('rfifind', options, [subband_globpattern])
        if status != 0:
            raise Exception('There was a problem running rfifind.')
        # remove the now unused symlinks
        if clean_up:
            try:
                remove_matching(work_dir, '^' + re.escape(basename) + '\.sub\d+$')
                remove_matching(work_dir, '^' + re.escape(basename) + '\.sub\d+$')
            except:
                print 'Failure whilst cleaning up rfifind working directory.'
                raise
    finally:
        # jump back to previous working directory
        os.chdir(CWD)

    return



def run_prepsubband(batch, n_cores, subband_globpattern, result_dir,
    basename, mask_file, numout, zero_dm=False):
    '''
    Run the (mpi)prepsubband commandline given a DDplan instance.

    Note : only use with the DDplan instances that are created by the 
    rebatch member fuction of the SearchRun class (since they always have
    ddplan.numpasses == 1 and an appropriate ddplan.dmsperpass --- exactly
    what this function expects).
    '''
    new_numout = numout / batch.downfact
#    print numout, ddplan.downsamp, new_numout
    assert numout % batch.downfact == 0
    new_numout = int(new_numout)
    prepsubband_options = {
        '-noclip' : '',
        '-mask' : mask_file, 
        '-o' : join(result_dir, basename),
        '-lodm' : '%.2f' % batch.lo_dm,
        '-numdms' : str(batch.n_dms),
        '-downsamp' : str(batch.downfact),
        '-dmstep' : '%.2f' % batch.dm_step,
        '-numout' : str(new_numout),
        '-nsub' : str(batch.n_sub),
        '-runavg' : '', 
    }
    

    MPIRUN_OPTIONS = {
       '--mca' : 'btl',
        '^openib' : '',
    }


    prepsubband_parameters =  [subband_globpattern]

    if n_cores > 1:
        if zero_dm: # -zerodm is only in mpiprepsubband
            prepsubband_options['-zerodm'] = ''
        tmp = get_command('mpiprepsubband', prepsubband_options,
            prepsubband_parameters)
        mpirun_options = copy.copy(MPIRUN_OPTIONS)
        mpirun_options['-np'] = str(n_cores)
        status = run_command('mpirun', mpirun_options, [tmp])
    else:
        status = run_command('prepsubband', prepsubband_options,
            prepsubband_parameters)

    return status

def analyze_dm_trial(source_dir, basename, dm, work_dir, *args, **kwargs):
    '''
    Run accelsearch and singlepulse search on a given DM trial.

    zap_file, dat_dir and work_dir should be full paths.
    If run in parallel, make sure it is in different directories.
    '''
    do_accelsearch = kwargs.get('do_accelsearch', True)
    do_singlepulsesearch = kwargs.get('do_singlepulsesearch', True)
    bary_v = kwargs.get('bary_v', None)
    zap_file_fullpath = kwargs.get('zap_file_fullpath', None)
    z_values = kwargs.get('z_values', [50])
    stderr = kwargs.get('stdout', sys.stdout)
    stdout = kwargs.get('stderr', sys.stderr)
    if not (do_accelsearch or do_singlepulsesearch):
        return
    CWD = os.getcwd()
    try:
        # create working directory if it does not exists
        try:
            os.makedirs(work_dir)
        except OSError, e:
            if e.errno != errno.EEXIST:
                raise
        os.chdir(work_dir)

        # symlink .dat and .inf files (for now all of them)
        dat_file = basename + '_DM%.2f.dat' % dm
        inf_file = basename + '_DM%.2f.inf' % dm
        fft_file = basename + '_DM%.2f.fft' % dm
        red_fft_file = basename + '_DM%.2f_red.fft' % dm 
        os.symlink(join(source_dir, dat_file), join(work_dir, dat_file))
        os.symlink(join(source_dir, inf_file), join(work_dir, inf_file))

        if zap_file_fullpath:
            zap_file_dir, zap_file = os.path.split(zap_file_fullpath)
            try:
                os.symlink(zap_file_fullpath, join(work_dir, zap_file))
            except OSError, e:
                if e.errno != errno.EEXIST:
                    raise
        else:
            zap_file = ''
        
        if do_accelsearch:
            # perform fourier transform and appropriate corrections:
            run_realfft_zapbirds_rednoise(work_dir, basename, dm, bary_v, zap_file,
                stderr=stderr, stdout=stdout)
            # TODO add loop over z_max
            for z in z_values:
                run_accelsearch(fft_file, z, stderr=stderr, stdout=stdout)
            
        if do_singlepulsesearch:
            # run single pulse search
            run_single_pulse_search(work_dir, basename, dm, stderr=stderr, stdout=stdout)
        # remove symlinks to data files
        os.remove(join(work_dir, dat_file))
        os.remove(join(work_dir, inf_file))
    finally:
        os.chdir(CWD)

    return

def run_realfft_zapbirds_rednoise(work_dir, basename, dm, bary_v, zap_file, *args, **kwargs):
    '''
    Get PRESTO realfft and rednoise commands.

    Assumes workdir contains zap_file!
    '''

    stderr = kwargs.get('stdout', sys.stdout)
    stdout = kwargs.get('stderr', sys.stderr)

    CWD = os.getcwd()
    try:
        os.chdir(work_dir)

        dat_file = basename + '_DM%.2f' % dm + '.dat'
        fft_file = basename + '_DM%.2f' % dm + '.fft'
        red_fft_file = basename + '_DM%.2f' % dm + '_red.fft'    

        # Fourier transform takes place in the same directory as the .dat file.
        realfft_status = run_command('realfft', {'-outdir' : '.'}, [dat_file],
            stdout=stdout, stderr=stderr) 
        # Run zapbirds if zap_file is available
        if zap_file:
            zapbirds_status = run_command('zapbirds', {
                '-zap' : '',
                '-zapfile' : zap_file,
                '-baryv' : '%6g' % bary_v,
            }, [fft_file], stderr=stderr, stdout=stdout) 
        # Run rednoise correction.
        rednoise_status = run_command('rednoise', {}, [fft_file], stderr=stderr, stdout=stdout) 
        # copy rednoise corrected fft back to the normal fft
        shutil.move(red_fft_file, fft_file)
    finally:
        os.chdir(CWD) 

    return

 
def run_accelsearch(fft_file, z_max, *args, **kwargs):
    '''Get PRESTO accelsearch command.'''
    # TODO : add way of propagating extra options to accelsearch
    stderr = kwargs.get('stdout', sys.stdout)
    stdout = kwargs.get('stderr', sys.stderr)

    OPTIONS = {
        '-numharm' : str(8),
        '-sigma' : str(6),
        '-flo' : '%.2f' % 1,
        '-zmax' : '%d' % z_max,
    }
    accelsearch_status = run_command('accelsearch', OPTIONS, [fft_file], 
        stderr=stderr, stdout=stdout)

    if accelsearch_status != 0:
        print 'Failure for accelsearch on %s in directory %s.'
        raise Exception('accelsearch failure')

    return

def run_single_pulse_search(work_dir, basename, dm, *args, **kwargs):
    '''Run PRESTO single_pulse_search.py for searching.'''

    stderr = kwargs.get('stdout', sys.stdout)
    stdout = kwargs.get('stderr', sys.stderr)

    dat_file = join(work_dir, basename + '_DM%.2f' % dm + '.dat')
    singlepulse_status = run_command('single_pulse_search.py', {'-p' : ''},
        [dat_file], stderr=stderr, stdout=stdout)

    return

def run_single_pulse_search_plotter(work_dir, basename):
    '''Run PRESTO single_pulse_search.py for plotting.'''
    # Think about dealing with the case where there are too many input files
    # (so the script needs to be run for subsets of the data). For now use
    # the -g option for single_pulse_search.py to pass the .singlepulse files.
    
    globpattern = join(work_dir, basename + '_DM*.*.singlepulse')
    status = run_command('single_pulse_search.py', {
        '-g' : '"' + globpattern + '"',
    }, [])

    return status
    
def fold_on_known_ephemeris(source_dir, basename, dm, work_dir, 
    par_file_fullpaths, *args, **kwargs):

    '''Fold for known ephemeris provided as .par file.'''
    mask_file_fullpath = kwargs.get('mask_file_fullpath', '')
    CWD = os.getcwd()
    try:
        os.chdir(work_dir)
        # link .dat/.inf files to working directory
        dat_file = basename + '_DM%.2f' % dm + '.dat'
        inf_file = basename + '_DM%.2f' % dm + '.inf'
        os.symlink(join(source_dir, dat_file), join(work_dir, dat_file))
        os.symlink(join(source_dir, inf_file), join(work_dir, inf_file))
        for par_file_fullpath in par_file_fullpaths:
            # if necessary copy .par files to working directory
            par_file = os.path.split(par_file_fullpath)[1]
            if not os.path.exists(join(work_dir, par_file)):
                # Probably does not need a try except block (as one can 
                # repeatedly copy a file using shutil.copy without
                # exceptions being raised. XXX
                shutil.copy(par_file_fullpath, join(work_dir, par_file))
            mask_file = os.path.split(mask_file_fullpath)[1]
            tmp = join(work_dir, mask_file)
            # link rfifind output to working directory
            if mask_file_fullpath:
                try:
                    os.symlink(mask_file_fullpath, tmp)
                except OSError, e:
                    if e.errno != errno.EEXIST:
                        raise
            # run folding for known ephemeris (twice!)
            bin, options, parameters = get_folding_command_ke(
                basename, dm, basename + '_DM%.2f' % dm + '.dat', 
                mask_file, par_file, False
            )
            prepfold_status = run_command(bin, options, parameters)
            bin, options, parameters = get_folding_command_ke(
                basename, dm, basename + '_DM%.2f' % dm + '.dat', 
                mask_file, par_file, True 
            )
            prepfold_status = run_command(bin, options, parameters)
        # Clean up the data files (leaving the .par files and rfifind output).
        os.remove(join(work_dir, dat_file))
        os.remove(join(work_dir, inf_file))
    finally:
        os.chdir(CWD)

    return  

# ----------------------------------------------------------------------------

class SearchRun(object):
    def __init__(self, in_dir, work_dir, out_dir, rfi_file, zap_file, rfi_dir):
        '''This class describes a blind pulsar search.'''
        # Set directories used in search
        self.in_dir = os.path.abspath(in_dir)
        self.work_dir = os.path.abspath(work_dir) 
        self.out_dir = os.path.abspath(out_dir)

        # Check whether working directory is empty and writable
        if not os.access(self.work_dir, os.F_OK | os.R_OK | os.W_OK):
            raise WrongPermissions(self.work_dir)

        files = os.listdir(self.work_dir)
        if len(files) > 0:
            raise DirectoryNotEmpty(self.work_dir)

        # Check whether output directory is empty and writable
        if not os.access(self.out_dir, os.F_OK | os.R_OK | os.W_OK):
            raise WrongPermissions(self.out_dir)

        files = os.listdir(self.out_dir)    
        if len(files) > 0:
            raise DirectoryNotEmpty(self.out_dir) 


        self.basename = self.determine_basename()
        # Read the metadata from the input directory and check that it is
        # correct (old datasets had a mismatch between the number of samples
        # in the .inf file and the size of the .subXXXX files on disk).
        self.metadata = inf_reader(join(self.in_dir, self.basename + \
            '.sub.inf')) 

        # Old data sets have incorrect metadata, deal with that.
        datafile_size = os.path.getsize(join(self.in_dir, 
            self.basename + '.sub0000'))
        if self.metadata.n_bins * 2 != datafile_size:
            if self.metadata.n_bins == 999 and datafile_size % 2 == 0:
                self.metadata.n_bins = datafile_size // 2
                logging.info(
                    'Overruled wrong metadata (timeseries length was 999)'
                )
            else:    
                raise Exception('Metadata incorrect')

        if rfi_file:
            self.rfi_file = os.path.abspath(rfi_file)
        else:
            try:
                lofarsoft = os.environ['LOFARSOFT'] 
            except KeyError, e:
                print 'No RFI specified. $LOFARSOFT not set.'
                raise

            self.rfi_file = join(lofarsoft, 'release', 'share', 
                'pulsar', 'data', 'master.rfi')
        self.zap_file = zap_file

        # Deal with optional pre-existing rfifind output:
        rfifind_input_dir = os.path.abspath(rfi_dir)
        if not len(crawler.find_rfifind_output(rfi_dir, self.basename)) == 7:
            print 'rfifind output not complete, ignoring - rerunning rfifind.'
            self.rfifind_input_dir = ''
        else:
            print 'rfifind output complete, using the old mask'
            self.rfifind_input_dir = rfifind_input_dir

        ra_str = ra2ascii(*self.metadata.j2000_ra)
        dec_str = dec2ascii(*self.metadata.j2000_dec)
        self.bary_v = get_baryv(ra_str, dec_str, self.metadata.epoch_mjd, 
            self.metadata.n_bins * self.metadata.bin_width, obs='LF') 
        print 'Calculated barycentric velocity :', self.bary_v
        logging.info('Prepared for pulsar search run.')
        logging.info('Using %s as input directory.' % self.in_dir)
        logging.info('Using %s as working directory.' % self.work_dir)
        logging.info('Using %s as output directory.' % self.out_dir)


    def get_subband_globpattern(self):
        '''
        Get a globpattern that matches the raw data (the subbands)
        
        Note : the globpattern contains the full path to the raw data.'''
        return join(self.in_dir, self.basename + '.sub[0-9]???')
    
    def determine_basename(self):
        '''Find the basename of this observation from raw data directory.'''
        SUBB_PATTERN = re.compile(r'(?P<basename>\S+)\.sub\d{4}$')
        return crawler.get_basename(self.in_dir, SUBB_PATTERN)

    def determine_channel_frequency_range_mapping(self):
        '''
        Make a mapping from channel number to frequency range.

        Note : Assumes that the observing frequency range is contiguous (like
        it is expressed in the .inf files).
        '''
        
        n_channels = self.metadata.n_channels
        c_freq_lo = self.metadata.low_channel_central_freq
        channel_bandwidth = self.metadata.channel_bandwidth       
        
        mapping = {}
        for i in range(n_channels):
            mapping[i] = (
                c_freq_lo + channel_bandwidth * (i - 0.5),
                c_freq_lo + channel_bandwidth * (i + 0.5)
            )
        return mapping
   
    def rebatch(self, ddplans, n_cores):
        '''
        Take the dedispersion plan and cut it in manageable pieces. 
        
        Note : This needs to happen because of the extra logic that deals
        with mpiprepsubband's extra constraints, and the dedispersion plans
        in Vishal's code that have ddplan.dmsperpass > 1000 DM steps per pass.
        '''
        annotated_ddplans = []
        
        for ddplan in ddplans:
            downsample = ddplan.downsamp
            if n_cores > 1:
                rebatched = mpiprepsubband_helper(ddplan, n_cores)
            else:
                rebatched = prepsubband_helper(ddplan)
            
            for chunk in rebatched: 
                new_ddplan = DedispPlan(chunk[0], ddplan.dmstep, chunk[1], 1, 
                    self.metadata.n_channels, downsample)
                annotated_ddplans.append((new_ddplan, chunk[2]))
        
        return annotated_ddplans

    def run_search(self, ddplans, z_values, n_cores = None, 
            no_singlepulse = False, no_accel = False, save_timeseries = False, 
            par_files = None, no_fold = False, zero_dm = False):
        # The GBT drift scan survey uses dedispersion plans as units
        # of work. Because our use of mpiprepsubband (has extra constraints
        # on the number of DM trials it can write) and the fact that
        # Vishal's script contains a ddplan with more than a 1000 DM trials
        # that get created in one go we need to split the work into some
        # smaller units than specified in the dedipersion plan. We create new 
        # DDplans to that take the constraints into account.
        print 'No accelsearch', no_accel
        print 'No single pulse search', no_singlepulse
        if par_files == None:
            par_files = []

#        self.annotated_ddplans = self.rebatch(ddplans, n_cores)
        self.batches = create_batches(ddplans, n_cores)
        m = self.determine_channel_frequency_range_mapping()
        
        print 'frequency mapping:'
        print m
        print 'Reading the master.rfi file.'
        bad_freq_ranges = read_bad_observing_frequencies(self.rfi_file)
        print 'bad frequency ranges:'
        print bad_freq_ranges


        bad_channels  = []

        for fr in bad_freq_ranges:
            bad_channels.extend(intersect_channel_map_with_freq_range(m, fr))

        # Create 
        try:
            os.mkdir(join(self.work_dir, 'RFIFIND'))
            for i in range(n_cores):
                os.mkdir(join(self.work_dir, 'CORE_%d' % i))
            os.mkdir(join(self.out_dir, 'KNOWN_EPHEMERIS'))
            os.mkdir(join(self.out_dir, 'SINGLEPULSE'))
            os.mkdir(join(self.out_dir, 'LOGS'))
            os.mkdir(join(self.out_dir, 'INF'))
            os.mkdir(join(self.out_dir, 'RFIFIND'))
            os.mkdir(join(self.out_dir, 'ACCELSEARCH')) 
            os.mkdir(join(self.out_dir, 'DM0.00'))
            os.mkdir(join(self.out_dir, 'TIMESERIES'))
        except Exception, e:
            raise e

        # Make copies of the TEMPO .par files (if any were given) to the 
        # directories that will be used for folding on a known ephemeris.
        for par_file in par_files:
            assert os.path.split(par_file)[0] != self.out_dir
            for i in range(n_cores):
                shutil.copy(par_file, join(self.work_dir, 'CORE_%d' % i))
            # Also provide copies of the par files in the output directory
            # for future reference.
            shutil.copy(par_file, join(self.out_dir, 'KNOWN_EPHEMERIS', 
                os.path.split(par_file)[1]))

        # Perform rfifind if at all needed
        rfifind_dir = join(self.work_dir, 'RFIFIND')
        if not self.rfifind_input_dir:
            t0 = time.time()
            run_rfifind(self.in_dir, self.basename, rfifind_dir, 
                bad_channels=bad_channels)
            t1 = time.time()
            print 'Running rfifind took %.2f seconds.' % (t1 - t0)
        else:
            print 'COPYING CACHED RFIFIND RESULTS'
            # TODO : rewrite to copy_matching 
            # copy over the old rfifind output
            rfifind_input_files = crawler.find_rfifind_output(
                self.rfifind_input_dir, self.basename)
            for f in rfifind_input_files:
                shutil.copy(join(self.rfifind_input_dir, f), rfifind_dir)


        # Store the location of the rfifind mask file (used by other PRESTO
        # binaries):
        rfifind_mask_file = join(self.work_dir, 'RFIFIND', 
            self.basename + '_rfifind.mask')
        
        ddplan_i = -1 
#        for ddplan, n_cores_to_use in self.annotated_ddplans:
        for batch in self.batches:
            print '=' * 70
            print 'DEDISPERSING FROM DM %.2f TO DM %.2f' % (batch.lo_dm, batch.get_dms()[-1])
            ddplan_i += 1
            # Note : Check whether data needs downsampling (and how GBT does
            # it).

            t_prepsubband_start = time.time() 
            prepsubband_status = run_prepsubband(batch, batch.n_cores,
                self.get_subband_globpattern(), self.work_dir, self.basename,
                rfifind_mask_file, determine_numout(self.metadata.n_bins), 
                zero_dm)
            t_prepsubband_end = time.time()
            batch.prepsubband_time = t_prepsubband_end - t_prepsubband_start

            print '=' * 70
            print 'BEGINNING THE PER DM SEARCH LOOP'
            print 'CHILD PROCESSES DOING THE ACTUAL WORK'
            sys.stdout.flush()
            t_search_start = time.time() 
            child_pids = []
            for core_index in range(n_cores):
                child_pid = os.fork()
                core_work_dir = join(self.work_dir, 'CORE_%d' % core_index)
                core_log_file = join(self.work_dir, 'SEARCH_LOG_CORE_%d_STEP_%d.log.txt' % (core_index, ddplan_i))
                if child_pid == 0: # run search loop
                    try:
                        LOG = open(core_log_file, 'w')
                    except OSError, e:
                        print 'Could not open log file %s.' 
                        print 'Killing child!'
                        sys.exit(1)

                    sys.stdout = LOG
                    sys.stderr = LOG
                    print '=' * 70
                    print 'CHILD_PID', child_pid

                    # SEARCH LOOP:
                    for i, dm in enumerate(batch.get_dms()):
                        # NEW STYLE SEARCH LOOP (use subdirectories per core for everything)
                        # in a directory per CORE, do the following:
                        if i % n_cores != core_index: continue
                        print '=' * 70
                        print 'ANALYZING DM TRIAL DM = %.2f' % dm
                        print 'WORKING DIRECTORY %s' % core_work_dir

                        # SEARCH:
                        # Perform fft + corrections, accelsearch, single pulse search
                        analyze_dm_trial(self.work_dir, self.basename, dm, 
                            core_work_dir, z_values=z_values,
                            zap_file_fullpath=self.zap_file, bary_v=self.bary_v,
                            do_accelsearch=not no_accel,
                            do_singlepulsesearch=not no_singlepulse,
                            stderr=LOG, stdout=LOG)

                        # Known ephemeris folding:
                        for par_file in par_files:
                            fold_on_known_ephemeris(self.work_dir, self.basename, dm,
                                core_work_dir, par_files, stderr=LOG, stdout=LOG)
         
                        # CLEAN UP:
                        # Copy the DM 0 files to the output directories.
                        if dm == 0:
                            file_regexp = r'^' + re.escape(self.basename) +r'_DM0\.00\.\S+$'
                            copy_matching(core_work_dir, 
                                join(self.out_dir, 'DM0.00'), file_regexp)
                            # Also copy over the .inf and .dat files
                            copy_matching(self.work_dir, join(self.out_dir, 'DM0.00'), file_regexp)

                        # TODO : see whether this stuff can be run outside of the search loop (less
                        # talking to the disks, but larger disk use during the search).
                        if not no_accel:
                            # remove datafiles that are no longer necessary
# MAYBE REMOVING THE .fft FILES IS DONE WRONG:
#                            remove_matching(core_work_dir, '^\S+\.fft')
                            file_regexp = r'^' + re.escape(self.basename) + r'_DM%.2f\.fft$' % dm
                            remove_matching(core_work_dir, file_regexp)
                            # move the search output to the top level working directory
                            move_matching(core_work_dir, self.work_dir,
                                '(^\S+\_ACCEL_\d+$|^\S+\_ACCEL_\d+\.cand$|^\S+\_ACCEL_\d+\.txtcand$)'
                            )

                        if not no_singlepulse:
                            move_matching(
                                core_work_dir, self.work_dir, '^\S+\.singlepulse$'
                            )

                        if save_timeseries:
                            # copy timeseries to output directory
                            # This can fail miserably if there is not enough disk!
                            shutil.move(join(self.work_dir, self.basename + '_DM%.2f' % dm + '.dat'),
                                join(self.out_dir, 'TIMESERIES'))
                        else:
                            # remove dedispersed timeseries (.dat files)
                            os.remove(join(self.work_dir, self.basename + '_DM%.2f' % dm + '.dat'))

                        for par_file in par_files:
                            move_matching(core_work_dir, 
                                join(self.out_dir, 'KNOWN_EPHEMERIS'), r'^\S+\.pfd\.(bestprof|ps)')

                    LOG.close()
                    sys.exit(0)

                else: # in main process, keep track of all children
                    child_pids.append(child_pid)     
            assert child_pid != 0
            for cp in child_pids:
                os.waitpid(cp, 0)
            t_search_end = time.time()
            batch.search_time = t_search_end - t_search_start
            print 'DONE!'
        print '=' * 70
        print 'DONE SEARCHING FULL DEDISPERSION PLAN - MOVING ON'
        print sys.stdout.flush()

        if not no_accel:
            print '=' * 70
            print 'LOOKING AT ACCELERATION SEARCH RESULTS' 
            # Run the seperate folding script (after having created a directory
            # for it to write the folds to).
            os.mkdir(join(self.work_dir, 'FOLDS'))
            folder.main(
                folddir=join(self.work_dir, 'FOLDS'),
                subbdir=self.in_dir,
                canddir=self.work_dir,
                basename=self.basename,
                mask_filename=rfifind_mask_file,
                n_cores=n_cores,
                n_candidates_cutoff=N_CANDIDATES_CUTOFF,
                minimum_dm_cutoff=MINIMUM_DM_CUTOFF,
                metadata=self.metadata,
                no_fold=no_fold,
            )
        if not no_singlepulse:
            # Deal with single pulse search plotting
            print '=' * 70
            print 'LOOKING AT SINGLE PULSE SEARCH RESULTS'
            d = os.getcwd()
            os.chdir(self.work_dir)
            status = run_single_pulse_search_plotter('.', self.basename)
            os.chdir(d)

        # create a plots of chi-squared versus DM for the output of the known
        # ephemeris search
        if par_files:
            print '=' * 70
            print 'LOOKING AT KNOWN EPHEMERIS FOLDING RESULTS'
            tmp = crawler.find_pfd_bestprof(join(self.out_dir, 'KNOWN_EPHEMERIS'), self.basename)
            print 'FOUND KNOWN EPHEMERIS FOLDING OUTPUT FILES :', tmp
            tmp_graphs = {}
            for pulsar in tmp:
                tmp_graphs[pulsar] = ([], [])
                for pfd_bestprof_file in tmp[pulsar]:
                    chi_square, best_dm = parse_pfd_bestprof_file(
                        join(self.out_dir, 'KNOWN_EPHEMERIS', pfd_bestprof_file))
                    tmp_graphs[pulsar][0].append(best_dm)
                    tmp_graphs[pulsar][1].append(chi_square)
            for pulsar in tmp_graphs:
                peak_chi_sq = tmp_graphs[pulsar][1][0]
                peak_dm = tmp_graphs[pulsar][0][0]
                for tmp_dm, tmp_chi_sq in zip(tmp_graphs[pulsar][0], tmp_graphs[pulsar][1]):
                    if tmp_chi_sq > peak_chi_sq:
                        peak_chi_sq = tmp_chi_sq
                        peak_dm = tmp_dm
                pyplot.clf()
                pyplot.scatter(tmp_graphs[pulsar][0], tmp_graphs[pulsar][1])
                pyplot.title('Chi square for %s (peak at DM = %.2f)' % (pulsar, peak_dm))
                pyplot.xlabel('DM pc cm^-3')
                pyplot.ylabel('chi square')
                pyplot.savefig(join(self.out_dir, 'KNOWN_EPHEMERIS', pulsar + '.pdf'))
                pyplot.clf()


        print '=' * 70
        print 'MOVING FILES AROUND'
        # Add code that cleans up after the search:
        if not no_accel:
            # move the FOLDS directory to the output directory
            shutil.move(join(self.work_dir, 'FOLDS'), 
                join(self.out_dir, 'FOLDS'))
            # move all the candidate files to the output location
            for f in glob.glob(join(self.work_dir, self.basename \
                + '_DM*.[0-9][0-9]_ACCEL*')):
                shutil.move(f, join(self.out_dir, 'ACCELSEARCH'))
        if not no_singlepulse:
            # move all the singlepulse related stuff
            for f in glob.glob(join(self.work_dir, '*.singlepulse')):
                shutil.move(f, join(self.out_dir, 'SINGLEPULSE'))
            for f in glob.glob(join(self.work_dir, '*_singlepulse.ps')):
                shutil.move(f, join(self.out_dir, 'SINGLEPULSE'))
        # move all the .inf files
        for f in glob.glob(join(self.work_dir, '*.inf')):
            shutil.move(f, join(self.out_dir, 'INF'))
        # move all the search scripts/logs to the LOGS directory
        for f in glob.glob(join(self.work_dir, '*.log.txt')):
            shutil.move(f, join(self.out_dir, 'LOGS'))
        for f in glob.glob(join(self.work_dir, '*.sh')):
            shutil.move(f, join(self.out_dir, 'LOGS'))
        # move the rfifind output
        shutil.move(join(self.work_dir, 'RFIFIND'), 
            join(self.out_dir, 'RFIFIND'))

def mpiprepsubband_helper(ddplan, n_cores):
    '''
    Determine mpiprepsubband options based on ddplan.

    Note : This is a helper function don't call this directly, call
    SearchRun.rebatch instead.
    
    returns a list like:
    [(<lowest DM>, <number of DM trials>, <number of CPU cores>), ...]    
    '''
    MAX_N_DMS = 1000 # Some limitation of ?prepsubband
   
    # Do the math on the number of DM trials to make per prepsubband call,
    # which needs to be a multiple of the (number of cores -1).
    max_n_dms = min(
        (MAX_N_DMS // (n_cores - 1)) * (n_cores - 1),
        (ddplan.dmsperpass // (n_cores - 1)) * (n_cores - 1)
    )
    
    # Deal with the corner case where you want fewer DM trials than there
    # are cores - 1 available:
    if ddplan.dmsperpass < (n_cores - 1):
        return [(ddplan.lodm, ddplan.dmsperpass, ddplan.dmsperpass + 1)]
 
    # This loop calculates how many times mpiprepsubband needs to be
    # called, how many DM trials it should create for each call and how 
    # many cores should be used for each call.
    out = []
    for i in range(ddplan.numpasses):
        cum_n_dms = 0         
        # read cum_n_dms as cumulative number of DM trials (this pass)
        while cum_n_dms < ddplan.dmsperpass:
            lowest_dm = cum_n_dms * ddplan.dmstep + ddplan.lodm + \
                i * ddplan.dmsperpass * ddplan.dmstep
            # Figure out how many cores to use and how many DM trials to this
            # (mpi)prepsubband call.
            if cum_n_dms + max_n_dms > ddplan.dmsperpass:
                n_dms = ddplan.dmsperpass - cum_n_dms
                # For the last few DM trials fewer cores should be used since
                # mpiprepsubband only works if:
                # number of DM trials % (number of cores - 1) == 0
                tmp = (n_dms // (n_cores - 1))
                if tmp > 0:
                    out.append((lowest_dm, tmp * (n_cores - 1), n_cores))
                    lowest_dm += tmp * (n_cores - 1) * ddplan.dmstep
                    n_dms -= tmp * (n_cores -1)
                assert n_dms < n_cores
                out.append((lowest_dm, n_dms, n_dms + 1))
            else:
                # For the normal case we use the maximum number of cores 
                # available.
                n_dms = max_n_dms
                out.append((lowest_dm, n_dms, n_cores))            
            cum_n_dms += max_n_dms            
        return out

def prepsubband_helper(ddplan):
    '''
    Determine prepsubband options based on ddplan. 
    
    Note : This is a helper function don't call this directly, call
    SearchRun.rebatch instead.

    returns a list like:
    [(<lowest DM>, <number of DM trials>, 1), ...]    
    '''
    MAX_N_DMS = 1000 # Some limitation of ?prepsubband
   
    # Do the math on the number of DM trials to make per prepsubband call,
    # which needs to be a multiple of the (number of cores -1).
    max_n_dms = min(MAX_N_DMS, ddplan.dmsperpass)
   
    # This loop calculates how many times mpiprepsubband needs to be
    # called, how many DM trials it should create for each call and how 
    # many cores should be used for each call.
    out = []

    for i in range(ddplan.numpasses):
        cum_n_dms = 0 # read cum_n_dms as cumulative number of DM trials     
        # read cum_n_dms as cumulative number of DM trials (this pass)
        while cum_n_dms < ddplan.dmsperpass:
            lowest_dm = cum_n_dms * ddplan.dmstep + ddplan.lodm + \
                i * ddplan.dmsperpass * ddplan.dmstep 
            # Figure out how many DM trials to make this prepsubband call.
            if cum_n_dms + max_n_dms > ddplan.dmsperpass:
                n_dms = ddplan.dmsperpass - cum_n_dms
                out.append((lowest_dm, n_dms, 1))
            else:
                n_dms = max_n_dms
                out.append((lowest_dm, n_dms, 1))            
            cum_n_dms += max_n_dms 
        return out


if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('--zap_file', dest='zap_file', type='string',
        default='', metavar='ZAP_LIST', 
        help='File with birdies for zapbirds.')
    parser.add_option('--i', help='Input directory for raw data.', 
        type='string', metavar='IN_DIR', dest='in_dir')
    parser.add_option('--o', help='Output directory for search data.',
        type='string', metavar='OUT_DIR', dest='out_dir') 
    parser.add_option('--w', help='Working directory.',
        type='string', metavar='WORK_DIR', dest='work_dir')
    parser.add_option('--rfi', help='RFI file', type='string', default='',
        metavar='RFI_FILE', dest='rfi_file')    
    parser.add_option('--plan', dest='plan', type='string', default='LPPS', 
        metavar='PLAN', help='Dedispersion plan to be used (default=LPPS)'),
    parser.add_option('--z_list', dest='z_list', type='string',
        default='[0,50]', metavar='Z_LIST', 
        help='List of integer z values, default [0,50] - don\'t use spaces.')
    parser.add_option('--ncores', dest='ncores', type='int',
        default=8, help='Number of cores to use (default 8).')
    parser.add_option('--ns', dest='ns', help='Run no single pulse search.',
        action='store_true', default=False)
    parser.add_option('--na', dest='na', help='Run no acceleration search.',
        action='store_true', default=False)
    parser.add_option('--par', dest='par_list', type='string',
        metavar='PAR_LIST', help='List of TEMPO .par files [file1,file2]')
    parser.add_option('--rfidir', dest='rfi_dir', type='string', 
        metavar='RFI_DIR', default='',
        help='Directory containing rfifind output for current data set.') 
    parser.add_option('--st', dest='st', action='store_true', default=False,
        help='Keep dedispersed timeseries around.')
    parser.add_option('--nf', metavar='NO_FOLD', default=False, 
        action='store_true', dest='no_fold',
        help='Don\'t perform pulsar fold.')
    parser.add_option('--zerodm', metavar='ZERO_DM', default=False,
        action='store_true', dest='zero_dm',
        help='Perform \'zero-DMming\' whilst running mpiprepsubband.')
    options, args = parser.parse_args()
    N_CORES = options.ncores

    if not (options.in_dir and options.out_dir and options.work_dir):
        print 'Provide --i, --w and --o options.'
        print parser.print_help()
        sys.exit(1)
    
    # parse list of z values:
    # TODO : refactor list parsing code
    z_values = []
    if options.z_list[0] == '[' and options.z_list[-1] == ']':
        tmp = options.z_list[1:-1].split(',')
        try:
            for z_str in tmp:
                z_values.append(int(z_str))
        except ValueError, e:
            print 'List of z values not specified correctly.'
            raise
    else:
        print 'Z values list needs to be inside of [] and contain no spaces.'
        raise Exception()

    # parse list of filenames for TEMPO .par files
    par_files = []
    if options.par_list and options.par_list[0] == '[' and options.par_list[-1] == ']':
        print 'first check ok!'
        tmp = options.par_list[1:-1].split(',')
        print tmp
        try:
            for par_file in tmp:
                par_files.append(os.path.abspath(par_file))
        except Exception, e:
            print 'Problem occured parsing list of TEMPO .par files'
            raise
    else:
        pass

    # Add basic logging right here.
    logging.basicConfig(
        format='%(asctime)-15s - %(name)s - %(levelname)s - %(message)s',
        level=logging.DEBUG,
    ) 

    if options.zap_file:
        zap_file = os.path.abspath(options.zap_file)
    else:
        zap_file = ''

    t_start = time.time()
    SR = SearchRun(options.in_dir, options.work_dir, options.out_dir, 
        options.rfi_file, zap_file, options.rfi_dir)  

#            DedispPlan(lodm=0, dmstep=1, dmsperpass=7, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=7, dmstep=1, dmsperpass=2, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=9, dmstep=1, dmsperpass=7, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=16, dmstep=1, dmsperpass=2, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
    ddplan_test = DedispersionPlan('TEST')
    ddplan_test.add_batch(0, 1, 7, 1, 128)
    ddplan_test.add_batch(7, 1, 2, 1, 128)
    ddplan_test.add_batch(9, 1, 7, 1, 128)
    ddplan_test.add_batch(16, 1, 2, 1, 128)

    ddplan_lpps = DedispersionPlan('LPPS')
    ddplan_lpps.add_batch(0, 0.05, 1196, 1, 128)
    ddplan_lpps.add_batch(59.8, 0.10, 408, 2, 128)
    ddplan_lpps.add_batch(100.60, 0.20, 449, 4, 128)
    ddplan_lpps.add_batch(190.40, 0.50, 442, 8, 128)
    ddplan_lpps.add_batch(411.4, 1, 89, 16, 128)
    
    PREDEFINED_DEDISPERSION_PLANS = {
#        'LPPS' : [
#            DedispPlan(lodm=0, dmstep=0.05, dmsperpass=1196, 
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=59.8, dmstep=0.10, dmsperpass=408,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=2),
#            DedispPlan(lodm=100.60, dmstep=0.20, dmsperpass=449,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=4),
#            DedispPlan(lodm=190.40, dmstep=0.50, dmsperpass=442,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=8),
#            DedispPlan(lodm=411.4, dmstep=1.0, dmsperpass=89,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=16),
#        ],
#        'LOTAS' : [
#        # DDplan.py -f 143.255615234375 -n 3904 -t 0.00131072 -b 47.65625
#        # Do some tests to see how fast this dedispersion plan is and whether
#        # some resolution should be traded for processing speed.
#            DedispPlan(lodm=0, dmstep=0.02, dmsperpass=6647, 
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=132.940, dmstep=0.03, dmsperpass=2454,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=2),
#            DedispPlan(lodm=206.560, dmstep=0.05, dmsperpass=3258,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=4),
#            DedispPlan(lodm=369.460, dmstep=0.10, dmsperpass=3576,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=8),
#            DedispPlan(lodm=727.060, dmstep=0.30, dmsperpass=910,
#                numpasses=1, numsub=SR.metadata.n_channels, downsamp=16),
#        ],
#        'LoMASS' : [
#            DedispPlan(lodm=0, dmstep=0.01, dmsperpass=601, numpasses=1,
#                numsub=SR.metadata.n_channels, downsamp=2),
#            DedispPlan(lodm=6.010, dmstep=0.01, dmsperpass=171, numpasses=1,
#                numsub=SR.metadata.n_channels, downsamp=4),
#            DedispPlan(lodm=7.720, dmstep=0.02, dmsperpass=376, numpasses=1,
#                numsub=SR.metadata.n_channels, downsamp=8),
#            DedispPlan(lodm=15.240, dmstep=0.03, dmsperpass=399, numpasses=1,
#                numsub=SR.metadata.n_channels, downsamp=16),
#            DedispPlan(lodm=27.210, dmstep=0.05, dmsperpass=46, numpasses=1,
#                numsub=SR.metadata.n_channels, downsamp=32),
#        ],
#        'TEST' : [
#            DedispPlan(lodm=0, dmstep=1, dmsperpass=7, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=7, dmstep=1, dmsperpass=2, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=9, dmstep=1, dmsperpass=7, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#            DedispPlan(lodm=16, dmstep=1, dmsperpass=2, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#        ],
#        'LOTASTEST' : [
#            DedispPlan(lodm=0, dmstep=0.5, dmsperpass=50, numpasses=1, 
#                numsub=SR.metadata.n_channels, downsamp=1),
#        ],
        'TEST' : ddplan_test,
        'LPPS' : ddplan_lpps,
    }
    try:
        ddplans = PREDEFINED_DEDISPERSION_PLANS[options.plan]
    except KeyError, e:
        print 'There is no dedispersion plan called %s .' % options.plan
        print 'Known dedispersion plans:'
        for k in PREDEFINED_DEDISPERSION_PLANS.keys():
            print k
        sys.exit()
    else:
        print 'Using dedispersion plan called %s .' % options.plan

    SR.run_search(ddplans, z_values, N_CORES, options.ns, options.na, options.st,
        par_files, options.no_fold, options.zero_dm)
    t_end = time.time()
    print '\n=== TIMINGS ==='
    search_time = 0
    dedisperse_time = 0 
    for batch in SR.batches:
        print 'From DM %.2f to DM %.2f' % (batch.lo_dm,
             batch.lo_dm + batch.dm_step * batch.n_dms) 
        print 'Dedispersing cost %.2f seconds and searching cost %.2f seconds' % \
            (batch.dedispersion_time, batch.search_time)
        dedisperse_time += batch.dedispersion_time
        search_time += batch.search_time
    t_total = t_end - t_start
    print '\nTotal time spent dedispersing %.2f' % dedisperse_time
    print 'Total time spent searching %.2f' % search_time
    print '\nFolding (+book keeping) took %.2f seconds for the candiates.' % (
        t_total - dedisperse_time - search_time)
    print '\nIt took %.2f seconds to search the data.' % t_total 
    print '(Including bookkeeping and rfifind)'

