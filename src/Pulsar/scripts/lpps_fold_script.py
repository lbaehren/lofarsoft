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

# All the options that need to be specified for this module (either on the 
# commandline or as keyword arguments to the main function) :
REQUIRED_OPTIONS = ['canddir', 'subbdir', 'folddir', 'basename']
OPTIONAL_OPTIONS = ['ncores']

# -----------------------------------------------------------------------------

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
    sifted_accelcands = []
    for z_max, accel_cands in accel_cands_found_for.iteritems():
        # Read all the acceleration candidates:
        files = glob.glob(os.path.join(cand_dir, 
            basename + '_DM*.*_ACCEL_%d' % z_max))
        accel_cands = sifting.read_candidates(files)
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
    return sifted_accelcands 

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
    sifted_candidates = sift_accel_cands(cand_dir, basename)
    assert len(sifted_candidates) > 0

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



if __name__ == '__main__':
    # Set up and run the commandline parser:
    # Note that here the commandline option and the name of it internally are
    # same (but without the --).
    parser = optparse.OptionParser()
    parser.add_option('--canddir', help='Directory containing candidate files.',
        type='string', dest='canddir', metavar='CANDDIR')
    parser.add_option('--subbdir', help='Directory containing subband files',
        type='string', dest='subbdir', metavar='SUBBDIR')
    parser.add_option('--folddir', help='Root dir for folds.',
        type='string', dest='folddir', metavar='FOLDDIR')
    parser.add_option('--ncores', metavar='NCORES', default=8, type='int', 
        help='Number of processor cores to use default is 8.')
    parser.add_option('--basename', type='string', metavar='BASENAME', 
        help='Basename of PRESTO files to look for.', dest='basename')
    options, args = parser.parse_args()
    # Check that all the required options were supplied on the commandline:
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

