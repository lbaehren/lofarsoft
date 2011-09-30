#!/usr/bin/env python
'''
Sifting script that knows how to take several lpps_search_script.py output
directories and sift all the 'raw' candidates. 

NOTE: future goals
- support reading mixed candidate / inf directories
- produce candidate plots for the whole dataset (showing what got culled)
- fire of the folding for the individual beams
'''
import optparse
import os
import shutil
import random
import sys
import subprocess
import time

import sifting

from lpps_search import duplicates
from lpps_search import crawler
from lpps_search import inf
from lpps_search import sift
from lpps_search import fold
from lpps_search import knownpulsar
from lpps_search.sift import plot_p_histogram, plot_f_histogram, plot_p_dm

N_CANDIDATES_CUTOFF = 200


def parse_list_of_directories(s, must_exist=False):
    dirs = []
    if s[0] == '[' and s[-1] == ']':
        tmp = s[1:-1].split(',')
        for d in tmp:
            d = os.path.abspath(d)
            if not os.path.exists(d):
                raise ValueError('Directory %s does not exist' % d)
            dirs.append(d)
    else:
        print 'Trying to parse : %s' % s
        raise ValueError('Directories need to be inside of [] and contain no spaces.')
    return dirs

def parse_list_of_files(s, must_exist=False):
    dirs = []
    if s[0] == '[' and s[-1] == ']':
        tmp = s[1:-1].split(',')
        for f in tmp:
            f = os.path.abspath(f)
            if not os.path.exists(f):
                raise ValueError('File %s does not exist' % f)
            dirs.append(f)
    else:
        print 'Trying to parse : %s' % s
        raise ValueError('Files need to be inside of [] and contain no spaces.')
    return dirs
    


def create_mixed_dirs(search_out_dirs, work_dirs):
    for wd in work_dirs:
        if len(os.listdir(wd)) > 0:
            raise Exception('Output directory %s not empty.' % wd)

    for sd, wd in zip(search_out_dirs, work_dirs):
        mixed_dir = os.path.join(wd, 'CANDIDATES')
        inf_dir = os.path.join(sd, 'INF')
        cand_dir = os.path.join(sd, 'ACCELSEARCH')

        os.makedirs(mixed_dir)
        for f in os.listdir(inf_dir):
            f = os.path.join(inf_dir, f)
            if os.path.isfile(f):
                shutil.copy(f, mixed_dir)
        for f in os.listdir(cand_dir):
            f = os.path.join(cand_dir, f)
            if os.path.isfile(f):
                shutil.copy(f, mixed_dir)
        mixed_dirs.append(mixed_dir)
    return mixed_dirs

def n_beam_sift(mixed_dirs, basenames, *args, **kwargs):
    sifted_per_beam = []
    unsifted_per_beam = []

    all_sifted_candidates = [] 
    for md, basename in zip(mixed_dirs, basenames): 
        # assumes DM=0 to be present
        metadata = inf.inf_reader(os.path.join(md, basename + '_DM0.00.inf'))
        u, s = sift.sift_accel_cands(md, basename, metadata=metadata, n_candidates_cutoff=0)
        all_sifted_candidates.extend(s)
        unsifted_per_beam.append(u)

    n_before = len(all_sifted_candidates)
    deduplicated = duplicates.remove_duplicates(all_sifted_candidates)
    print 'Removing duplicates across beams removed %d candidates' % \
        (n_before - len(deduplicated))

    tmp = dict([(md, list()) for md in mixed_dirs])
    for c in deduplicated:
        dest = os.path.split(c.filename)[0] 
        tmp[dest].append(c)

    for md in mixed_dirs:
        sifted_per_beam.append(tmp[md])

    return unsifted_per_beam, sifted_per_beam 

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('--o', help='Output directories.', dest='sift_out_dirs',
        metavar='OUTPUT_DIRECTIES', type='string', default='')
    parser.add_option('--i_searchout', help='LPPS style search output directories',
        metavar='SEARCH_OUT_DIRS', type='string', default='', 
        dest='search_out_dirs')
    parser.add_option('--i_mixed', help='Mixed candidate/inf directories',
        metavar='MIXED_DIRS', type='string', default='',
        dest='mixed_dirs')
    parser.add_option('--nf', help='Don\'t perform a fold.', dest='no_fold',
        metavar='NO_FOLD', action='store_true', default=False)
    parser.add_option('--ncores', metavar='N_CORES', default=8, type='int', 
        help='Number of processor cores to use default is 8.', dest='ncores')
    parser.add_option('--zap_file', type='string', metavar='ZAPFILE',
        help='Apply zaplist file (birdies) during sifting', default='',
        dest='zap_file')
    parser.add_option('--i_subb', help='Input directories for sub band files.',
        type='string', metavar='SUBB_DIRS', dest='subb_dirs')
    parser.add_option('--masks', help='Rfifind mask files.', type='string',
        metavar='RFI_MASK_FILES', dest='masks', default='')

    options, args = parser.parse_args()
    start_time = time.time()

    print 'N BEAM SIFTING / FOLDING SCRIPT, CHECKING COMMAND LINE OPTIONS.'

    # check commandline options for consistency
    if not (options.mixed_dirs or options.search_out_dirs):
        print 'Please specify either the --i_searchout or the --i_mixed option.'
        parser.print_help()
        sys.exit()

    if options.subb_dirs:
        subb_dirs = parse_list_of_directories(options.subb_dirs)
    else:
        subb_dirs = []

    if options.sift_out_dirs:
        sift_out_dirs = parse_list_of_directories(options.sift_out_dirs)
    else:
        print 'No output directories provided.'
        parser.print_help()
        sys.exit()

    if options.mixed_dirs:
        mixed_dirs = parse_list_of_directories(options.mixed_dirs, 
            must_exist=True)
        search_out_dirs = []
        if len(mixed_dirs) != len(sift_out_dirs):
            print 'Specify equally many input as output directories.'
            parser.print_help()
            sys.exit()
        if subb_dirs and len(subb_dirs) != len(mixed_dirs):
            print 'Specify equally many subband as output directories.'
            parser.print_help()
            sys.exit()
        basenames = []
        for md in mixed_dirs:
            basename = crawler.get_basename(md, r'^(?P<basename>\S+)_DM\d+\.\d{2}\.inf$')
            basenames.append(basename)
    else:
        search_out_dirs = parse_list_of_directories(options.search_out_dirs,
            must_exist=True)
        mixed_dirs = []
        if len(search_out_dirs) != len(sift_out_dirs):
            print 'Specify equally many input as output directories.'
            parser.print_help()
            sys.exit()
        if subb_dirs and len(subb_dirs) != len(sift_out_dirs):
            print'Specify equally many subband as output directories.'
            parser.print_help()
            sys.exit()

        basenames = []
        for sd in search_out_dirs:
            tmp = os.path.join(sd, 'INF')
            basename = crawler.get_basename(tmp, r'^(?P<basename>\S+)_DM\d+\.\d{2}\.inf$')
            basenames.append(basename)
 
    if not options.no_fold and not subb_dirs:
        print 'If you want to fold, you need to provide the --i_subb option.'
        parser.print_help()
        sys.exit()

    # deal with rfifind masks
    if mixed_dirs and options.masks:
        masks = parse_list_of_files(options.masks)
        if not options.no_fold and len(masks) != len(mixed_dirs):
            print 'Specify an rfimask for each beam.'
            parser.print_help()
            sys.exit()    
    elif search_out_dirs:
        masks = []
        for i, d in enumerate(search_out_dirs):
            files = os.listdir(os.path.join(d, 'RFIFIND'))
            L = len(basenames[i])
            found = False
            for f in files:
                if f[L:] == '_rfifind.mask':
                    masks.append(os.path.join(d, 'RFIFIND', f))
                    found = True
                    break
            if not found:
                print 'Something is wrong with search output directory %s' % d
                print 'Cannot find the rfifind mask.'
                parser.print_help()
                sys.exit(1)
 
                
    else:
        print 'Not using rfifind masks during folding, proceeding!'
        masks = []

    # deal with .zaplist files
    if options.zap_file:
        zap_file = os.path.abspath(options.zap_file)
        if not os.path.exists(zap_file):
            print 'File specified with --zap_file does not exist'
            parser.print_help()
            sys.exit(1)
    else:
        zap_file = ''

    # start processing:
    if search_out_dirs:
        print 'START COPYING FILES OVER (WILL TAKE A WHILE)'
        mixed_dirs = create_mixed_dirs(search_out_dirs, sift_out_dirs)
    print 'COMMAND LINE CHECKS OUT, PROCEEDING.'


    # perform the sift across beams
    print 'SIFTING CANDIDATES IN ALL DATA SETS (BEAMS)'
    u, s = n_beam_sift(mixed_dirs, basenames, zaplist_file=zap_file)
    u_count = sum(len(x) for x in u)
    s_count = sum(len(x) for x in s)
    print 'TOTAL NUMBER OF CANDIDATES BEFORE SIFTING %d' % u_count
    print 'TOTAL NUMBER OF CANDIDATES AFTER SIFTING %d' % s_count
    
    tmp_s = []
    for x in s:
        tmp_s.append(x[:N_CANDIDATES_CUTOFF])
    s = tmp_s
    print 'TOTAL NUMBER OF CANDIDATES AFTER APPLYING CUTOFF %d' % s_count


    # TODO : refactor the following into several functions (folding, forking
    # etc)
    cwd = os.getcwd()
    if not options.no_fold:
        print '\nFOLDING ALL %d BEAMS USING AT MAXIMUM %d CHILD PROCESSES' % \
            (len(search_out_dirs), options.ncores)
        print 'MIGHT TAKE A WHILE DEPENDING ON DATA ...'
        # Loop over the output directories for the sifting/folding and perform
        # the folding of the candidates (parallelized by forking this script).
        for i, d in enumerate(sift_out_dirs):
            print '\nWORKING ON DATASET (BEAM) %s .' % search_out_dirs[i]
            # figure out which data/directories are used for this dataset:
            candidates = s[i]
            basename = basenames[i]
            if subb_dirs:
                subband_dir = subb_dirs[i]
            else:
                subband_dir = ''
            if masks:
                mask = masks[i]
            else:
                mask = ''
            md = mixed_dirs[i]
            subband_globpattern = os.path.join(subband_dir, basename + '.sub[0-9]???')
            # set up per CPU core folding directories for this dataset:
            fold_dirs = [os.path.join(d, 'FOLDS', 'CORE_%d' % ii) for ii in range(options.ncores)]
            for fold_dir in fold_dirs:
                if not os.path.exists(fold_dir):
                    os.makedirs(fold_dir)
            # perform the folds in forked child processes:
            child_pids = []
            for core_idx in range(min(len(candidates), options.ncores)):
                child_pid = os.fork()

                if child_pid == 0:
                    fold_dir = fold_dirs[core_idx]
                    log_filename = os.path.join(fold_dir, 'log.txt')
                    # Add try except block around the log, investigate whether
                    # using the logging module is better.
                    log = open(log_filename, 'w')
                    sys.stdout = log
                    for ii, c in enumerate(candidates):
                        # Do processing as we are now in a forked off child: 
                        if ii % options.ncores != core_idx: continue
                        os.chdir(fold_dir)
                        # Call into lpps_search/fold.py
                        cmd = fold.fold(md, basename, c, fold_dir, 
                            subband_globpattern, mask_filename=mask)[0]
                        log.write('\'' + cmd + '\'\n')
                        log.flush()
                        p = subprocess.Popen(cmd, stdout=log, stderr=log, shell=True)
                        p.wait()
                        # TODO : Think of ways to act on a return code other than 0
                        print 'Return code %d' % p.returncode
                    log.close()
                    sys.exit(0)
                else:
                    child_pids.append(child_pid)
            # TODO : check return code of childred. Make children communicate
            # failures as a non-zero return code.
            while True:
                    try:
                        os.wait()
                        print 'CHILD RETURNED'
                    except OSError, e:
                        print 'ALL CHILD (FOLDING) PROCESSES FINISHED FOR %s .' % \
                            search_out_dirs[i]
                        break
        print 'FOLDING FOR ALL DATASETS (BEAMS) IS FINISHED.'
        os.chdir(cwd)
    # Always write out the sifted candidates files and plots (if matplotlib behaves)
    print '\nWRITING CANDIDATE FILES FOR ALL DATASETS (BEAMS)'
    for i, d in enumerate(sift_out_dirs): 
        tmp = os.path.join(d, 'FOLDS', 'SIFTED_CANDIDATES')
        if not os.path.exists(tmp):
            os.makedirs(tmp)

        candidates_out_file = os.path.join(tmp, 'sifted_candidates.txt')
        sifting.write_candlist(s[i], candidates_out_file)
    print '\nCREATING PLOTS FOR ALL DATASETS (BEAMS) IF MATPLOTLIB IS AVAILABLE'
    for i, d in enumerate(sift_out_dirs): 
        bright_pulsars = knownpulsar.load_bright_pulsar_catalog()
        # Make histograms of candidate period and candidate frequency before and
        # after sifting.
            
        histogram_dir = os.path.join(d, 'FOLDS',  'CANIDATE_HISTOGRAMS')
        if not os.path.exists(histogram_dir):
            os.makedirs(histogram_dir)
        try:
            if u[i]: 
                plot_p_histogram(u[i], 
                    os.path.join(histogram_dir, 'before_sifting_periods.pdf'))
                plot_f_histogram(u[i],
                    os.path.join(histogram_dir, 'before_sifting_frequencies.pdf'))
                plot_p_dm(u[i], bright_pulsars, 
                    os.path.join(histogram_dir, 'before.pdf'))
            if s[i]:
                plot_p_histogram(s[i], 
                    os.path.join(histogram_dir, 'after_sifting_periods.pdf'))
                plot_f_histogram(s[i],
                    os.path.join(histogram_dir, 'after_sifting_frequencies.pdf'))
                plot_p_dm(s[i], bright_pulsars, 
                    os.path.join(histogram_dir, 'after.pdf'))
        except AttributeError, e:
            print 'You are probably using the wrong version of Matplotlib.'
            # TODO : print traceback without crashing
            print 'No plots created.'
    print '\nN BEAM SIFTING / FOLDING SCRIPT IS DONE.'
    end_time = time.time()
    print 'TIME SPENT SIFTING AND FOLDING : %.2f SECONDS.' % (end_time - start_time)
