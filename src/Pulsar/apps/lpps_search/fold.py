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
from lpps_search.util import DirectoryNotEmpty, WrongPermissions
from lpps_search.sift import sift_accel_cands
from lpps_search.sift import plot_p_histogram, plot_f_histogram, plot_p_dm
from lpps_search import knownpulsar
from lpps_search.par import get_p0

def fold(cand_dir, basename, accel_cand, fold_dir, subband_globpattern, 
        **kwargs):
    '''Perform the pulsar fold.'''
    mask_filename = kwargs.get('mask_filename', '')

    commands = []
    commands.append(get_command(
        *get_folding_command(cand_dir, basename, accel_cand, 
            subband_globpattern, mask_filename)
    ))
    return commands

def get_prepfold_resolution_options(p):
    '''
    Get the search / fold resolution defaults based on the candidate period. 

    Note: Based on GBT drift scan defaults.
    '''

    # -n is N in GBT code
    # -npfact is Mp in GBT code
    # -ndmfact is Mdm in GBT code

    if p < 0.002:
        options = {
            '-npart' : '50',
            '-n' : '24',
            '-npfact' : '2',
            '-ndmfact' : '2',
        } 
    elif p < 0.05:
        options = {
            '-npart' : '40',
            '-pstep' : '1',
            '-pdstep' : '2',
            '-dmstep' : '3',
            '-n' : '50',
            '-npfact' : '2',
            '-ndmfact' : '1',
        }
    else:
        options = {
            '-npart' : '30',
            '-pstep' : '1',
            '-pdstep' : '2',
            '-dmstep' : '1',
            '-npfact' : '1',
            '-ndmfact' : '1',
        }
        if p < 0.5:
            options['-n'] = '100'
        else:
            options['-nopdsearch'] = ''
            options['-n'] = '200'

    return options
    

def get_folding_command(cand_dir, basename, accel_cand, subband_globpattern,
        mask_filename = ''):
    '''
    Create prepfold commandline for folding acceleration search candidates.
    '''
    # 1 : GBT code uses the dedispersion plan to base its decisions about 
    # downsample factor on. Strictly speaking I don't have that dedispersion
    # plan available - TODO : figure out whether it can be reconstructed baesd
    # on what I do know (after crawling a directory). Probably based on .inf
    # files (if they are in fact intact).
    p = 1 / accel_cand.f
    z_max = int(accel_cand.filename.split('_')[-1]) 
    # Set the prepfoild folding and search resoultion (according to GBT 
    # defaults).
    options = get_prepfold_resolution_options(p)

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
    })
    if mask_filename:
        options['-mask'] = mask_filename
    # Something todo with subband files ...
    args = [subband_globpattern]  
    return 'prepfold', options, args

def delete_from_dict(d, keys):
    for k in keys:
        try:
           del d[k]
        except KeyError, e:
            pass 

def get_folding_command_ke(basename, dm, dat_file, mask_filename='', 
    par_filename='', search=False):
    '''
    Create prepfold commandline for folding timeseries on a known ephemeris.
    '''

    p = get_p0(par_filename)
    # Set the prepfoild folding and search resoultion (according to GBT 
    # defaults).
    options = get_prepfold_resolution_options(p)
    # Delete some of the unneeded commandline options (as compared to folding
    # the acceleration searh output), then set some options that are needed
    # based on whether you want to search a bit during folding:
    if search:
        delete_from_dict(options, ['-ndmfact'])
        options.update({
            '-nopdsearch' : '',
            '-o' : basename + '_DM%.2f' % dm + '_NO_PDSEARCH',
        })
    else:
        delete_from_dict(options, ['-nopdsearch', '-npfact', '-ndmfact'])
        options.update({
            '-nosearch' : '',
            '-o' : basename + '_DM%.2f' % dm + '_NO_SEARCH',
        })
    # Set the options that are always needed for folding on a known 
    # ephermeris: 
    options.update({
        '-runavg' : '', 
        '-noxwin' : '',
        '-dm' : '%.2f' % dm, 
        '-par' : par_filename
    })
    # Set the rfifind mask file if present:    
    if mask_filename:
        options['-mask'] = mask_filename
    # Set dat file to fold on:
    args = [dat_file] 
    return 'prepfold', options, args


def main(folddir, subbdir, canddir, basename, **kwargs):
    '''Importable version of the whole script.'''

    mask_filename = kwargs.get('mask_filename', '')
    n_cores = kwargs.get('n_cores', 8)
    zaplist_file = kwargs.get('zaplist_file', '')
    n_candidates_cutoff = kwargs.get('n_candidates_cutoff', 20)
    minimum_dm_cutoff = kwargs.get('minimum_dm_cutoff', 0)
    metadata = kwargs.get('metadata', None)
    no_fold = kwargs.get('no_fold', False)

    # Check that the directories are available and that the output directory
    # is empty and writable.
    cand_dir = os.path.abspath(canddir)
    subb_dir = os.path.abspath(subbdir)
    fold_dir = os.path.abspath(folddir)

    # TODO : add a check of whether the directories exist 
    if not os.access(fold_dir, os.F_OK | os.R_OK | os.W_OK):
        raise WrongPermissions(fold_dir)
    
    files = os.listdir(fold_dir)
    if len(files) > 0: 
        raise DirectoryNotEmpty(fold_dir)
 
    # search for all the accelcand files and sift them
    unsifted_candidates, sifted_candidates = sift_accel_cands(cand_dir, basename,
         zaplist_file=zaplist_file, n_candidates_cutoff=n_candidates_cutoff, 
        minimum_dm_cutoff=minimum_dm_cutoff, metadata=metadata)
    if len(sifted_candidates) == 0:
        print 'In directory %s there are no candidate files.' % cand_dir
        assert len(sifted_candidates) > 0
    
    bright_pulsars = knownpulsar.load_bright_pulsar_catalog()
    # Make histograms of candidate period and candidate frequency before and
    # after sifting.
    histogram_dir = os.path.join(fold_dir, 'CANIDATE_HISTOGRAMS')
    os.mkdir(histogram_dir)
    try:
        if unsifted_candidates: 
            plot_p_histogram(unsifted_candidates, 
                os.path.join(histogram_dir, 'before_sifting_periods.pdf'))
            plot_f_histogram(unsifted_candidates,
                os.path.join(histogram_dir, 'before_sifting_frequencies.pdf'))
            plot_p_dm(unsifted_candidates, bright_pulsars, 
                os.path.join(histogram_dir, 'before.pdf'))
        if sifted_candidates: 
            plot_p_histogram(sifted_candidates, 
                os.path.join(histogram_dir, 'after_sifting_periods.pdf'))
            plot_f_histogram(sifted_candidates,
                os.path.join(histogram_dir, 'after_sifting_frequencies.pdf'))
            plot_p_dm(sifted_candidates, bright_pulsars, 
                os.path.join(histogram_dir, 'after.pdf'))
    except AttributeError, e:
        print 'You are probably using the wrong version of Matplotlib.'
        # TODO : print traceback without crashing
        print 'No plots created.'

    # spit out a file with the candidates that survived the sifting
    txt_out_dir = os.path.join(fold_dir, 'SIFTED_CANDIDATES')
    os.mkdir(txt_out_dir)
    candidates_out_file = os.path.join(txt_out_dir, 'sifted_candidates.txt')
    sifting.write_candlist(sifted_candidates, candidates_out_file)

    if not no_fold:
        # using knowledge about the directory layout:
        subband_globpattern = os.path.join(subb_dir, basename + '.sub[0-9]???')

        # construct all the folding commandlines
        folding_commands = [['cd %s' % os.path.join(fold_dir, 'CORE_%d' % i)] for i in range(n_cores)]
        for i, c in enumerate(sifted_candidates):
            core_index = i % n_cores
            l = fold(cand_dir, basename, c, fold_dir, subband_globpattern, 
                mask_filename=mask_filename)
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

    else:
        print 'Skipping the folding phase, exiting.'

