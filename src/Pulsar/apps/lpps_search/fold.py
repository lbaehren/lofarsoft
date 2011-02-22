#!/usr/bin/env python
'''
Script that folds pulsar candidates found by the PRESTO acceleration search.

Note : still needs a lot better sifting algorithm, the prepfold commands are
generated like the prepfold command that the GBT drift scan produces.
'''

# Standard library imports:
import os
import sys
import optparse
import glob
import stat
import subprocess
# PRESTO imports
import sifting
# LOFAR PILOT PULSAR SURVEY imports
from lpps_search import crawler
from lpps_search import inf
from lpps_search.util import create_script, run_as_script
from lpps_search.util import get_command, run_command
from lpps_search.util import MissingOptionException
from lpps_search.util import MissingCommandLineOptionException
from lpps_search.util import DirectoryNotEmpty, WrongPermissions
from lpps_search.sift import sift_accel_cands
from lpps_search.sift import plot_p_histogram, plot_f_histogram

# All the options that need to be specified for this module (either on the 
# commandline or as keyword arguments to the main function) :
REQUIRED_OPTIONS = ['canddir', 'subbdir', 'folddir', 'basename']
OPTIONAL_OPTIONS = ['ncores']

# -----------------------------------------------------------------------------

def fold(cand_dir, basename, accel_cand, fold_dir, subband_globpattern, 
        mask_filename = '' ):
    '''Perform the pulsar fold.'''
    mask_filename = '' 
    commands = []
    commands.append(get_command(
        *get_folding_command(cand_dir, basename, accel_cand, 
            subband_globpattern, mask_filename)
    ))
    return commands

def get_folding_command(cand_dir, basename, accel_cand, subband_globpattern,
        mask_filename = ''):
    # 1 : GBT code uses the dedispersion plan to base its decisions about 
    # downsample factor on. Strictly speaking I don't have that dedispersion
    # plan available - TODO : figure out whether it can be reconstructed baesd
    # on what I do know (after crawling a directory). Probably based on .inf
    # files (if they are in fact intact).
    p = 1 / accel_cand.f
    z_max = int(accel_cand.filename.split('_')[-1]) 
    # Determine the options for prepfold based on the GBT search script's
    # logic:
    if p < 0.002:
        options = {
            '-npart' : '50',
            '-ndmfact' : '3'
        } 
        n, npfact, ndmfact = 24, 2, 2
    elif p < 0.05:
        options = {
            '-npart' : '40',
            '-pstep' : '1',
            '-pdstep' : '2',
            '-dmstep' : '3',
        }
        n, npfact, ndmfact = 50, 2, 1
    elif p < 0.5:
        options = {
            '-npart' : '30',
            '-pstep' : '1',
            '-pdstep' : '2',
            '-dmstep' : '1',
        }
        n, npfact, ndmfact = 100, 1, 1
    else:
        options = {
            '-npart' : '30',
            '-nopdsearch' : '',
            '-pstep' : '1',
            '-pdstep' : '2', 
            '-dmstep' : '1',
        }
        n, npfact, ndmfact = 200, 1, 1
    options.update({
        '-runavg' : '',
        '-noxwin' : '',
        '-accelcand' : str(accel_cand.candnum),
        # accel_cand.accelfile contains the full path to the file because of
        # the way we call the sifting.read_candidates function : 
        '-accelfile' : accel_cand.filename + '.cand',
        '-dm' : '%.2f' % accel_cand.DM,
        # Output in working directory to make TEMPO and prepfold behave 
        # correctly :
        '-o' : basename + '_DM%.2f_Z%d' % (accel_cand.DM, z_max),
        '-n' : str(n),              # N in GBT code
        '-npfact' : str(npfact),    # Mp in GBT code
        '-ndmfact' : str(ndmfact),  # Mdm in GBT code
    })
    if mask_filename:
        options['-mask'] = mask_filename
    # TODO : think about the directory where everything should run
    # Something todo with subband files ...
    args = [subband_globpattern]  
    return 'prepfold', options, args

def main(*args, **kwargs):
    '''Importable version of the whole script.'''
    # Check that all the required keyword arguments are present
    for k in REQUIRED_OPTIONS:
        if not kwargs.has_key(k):
            raise MissingOptionException(k)
    # Check that the directories are available and that the output directory
    # is empty and writable.
    cand_dir = os.path.abspath(kwargs['canddir'])
    subb_dir = os.path.abspath(kwargs['subbdir'])
    fold_dir = os.path.abspath(kwargs['folddir'])
    n_cores = kwargs.get('ncores', 8)
    basename = kwargs['basename']

    # TODO : add a check of whether the directories exist 
    if not os.access(fold_dir, os.F_OK | os.R_OK | os.W_OK):
        raise WrongPermissions(fold_dir)
    
    files = os.listdir(fold_dir)
    if len(files) > 0: 
        raise DirectoryNotEmpty(fold_dir)
 
    # search for all the accelcand files and sift them
    unsifted_candidates, sifted_candidates = sift_accel_cands(cand_dir, basename)
    if len(sifted_candidates) == 0:
        print 'In directory %s there are no candidate files.' % cand_dir
        assert len(sifted_candidates) > 0
    
    # Make histograms of candidate period and candidate frequency before and
    # after sifting.
    histogram_dir = os.path.join(fold_dir, 'CANIDATE_HISTOGRAMS')
    os.mkdir(histogram_dir)
    if unsifted_candidates: 
        plot_p_histogram(unsifted_candidates, 
            os.path.join(histogram_dir, 'before_sifting_periods.pdf'))
        plot_f_histogram(unsifted_candidates,
            os.path.join(histogram_dir, 'before_sifting_frequencies.pdf'))
    if sifted_candidates: 
        plot_p_histogram(sifted_candidates, 
            os.path.join(histogram_dir, 'after_sifting_periods.pdf'))
        plot_f_histogram(sifted_candidates,
            os.path.join(histogram_dir, 'after_sifting_frequencies.pdf'))
    # using knowledge about the directory layout:
    mask_filename = os.path.join(cand_dir, basename + '_rfifind.mask')
    subband_globpattern = os.path.join(subb_dir, basename + '.sub[0-9]???')
    # construct all the folding commandlines
    folding_commands = [['cd %s' % os.path.join(fold_dir, 'CORE_%d' % i)] for i in range(n_cores)]
    for i, c in enumerate(sifted_candidates):
        core_index = i % n_cores
        l = fold(cand_dir, basename, c, fold_dir, subband_globpattern, 
            mask_filename)
        folding_commands[core_index].extend(l)
    else:
        pass

    for core_index in range(n_cores):
        try:
            p = os.path.join(fold_dir, 'CORE_%d' % core_index) 
            print 'making directory', p
            os.mkdir(p)
        except Exception, e:
            print 'Cannot make per core folding output directories'
            raise
    pids = []
    for i in range(n_cores):
        script_filename = os.path.join(fold_dir, 'CORE_%d' % i, 'script.sh')
        log_filename = os.path.join(fold_dir, 'CORE_%d' % i, 'log.txt')
        pid = run_as_script(folding_commands[i], script_filename, log_filename)
        pids.append(pid)    

    for pid in pids:
        pid.wait()
    print 'DONE FOLDING'


