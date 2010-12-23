#!/usr/bin/env python
#
# Script to search pulsars in LOFAR data
# Documentation can be found in Pulsar_Search_LOFAR.txt file
#
# Author : Vishal Gajjar (22 Aug 2010) 
# Email : gajjar@ncra.tifr.res.in
#
# Vlad, Nov 4, 2010
# + wrong indent fixed and "@" character removed in line 491
# + updated mpi Presto programs (with added -runavg option) checked in to USG
#
# Vlad, Nov 5-7, 2010
# + name of log-files changed
# + single-pulse is running together with periodicity search (with no plot creating)
#   and then runs on *.singlepulse files to create the plots for several DM ranges
# + bring all possibly tunable parameters to the beginning of the script
# + add working directory option (make default as /dev/shm) 
# + make tarballs of all logs and other files, plus creating png and move them to
#   the output directory, remove everything in scratch dir
# + read master.rfi and zap corresponding freq channels
# + birdies' file and run zapbirds
# + zapping first freq channel in each subband (it's junk there)
# + parallelization (search + prepfold), separate log files
# + added hi_accel_search and corresponding cmd option
# + added a few options to control execution of different steps in the pipeline
#
# Todo:
# ~ -numout ??? (not sure it's really that important now)
#-------------------------------------------------------------------------------------------#

import os, sys, glob
import numpy, time, math, optparse
import presto, sifting
import tarfile
import psr_utils as pu
import infodata as inf
import math

# some tunable parameters
is_fold_dat            = False   # if True then dat-files will be used to fold the candidates (maybe should put this as cmd option)
                                 # dat-files will be kept in the scratchdir until the end when the whole scratchdir will be removed
is_fold_known_psrs     = True    # if False, then all candidates that were selected to be harmonics of known psrs will be folded
                                 # on top of others (though among these others, these harmonics can also be)
hwired_ddplan_id       = "shallow"  # DDplan ID, Description of DDplan(s) set and/check below
bright_catalog         = os.environ["LOFARSOFT"] + "/release/share/pulsar/data/psr_cats.txt"   # Catalog of brightest pulsars
rfi_master_file        = os.environ["LOFARSOFT"] + "/release/share/pulsar/data/master.rfi"     # Master list of frequent RFIs
birdies_file           = ""      # List of birdies to zap in FFTs. If empty, the generic one will be created (with 50 & 100 Hz harmonics)
lo_acc_candlist_file   = "candidates_lo_accel.txt"  # ASCII list of lo-accel candidates
hi_acc_candlist_file   = "candidates_hi_accel.txt"  # ASCII list of hi-accel candidates
blk                    = 1000    # largest number of DMs to be read by mpiprepsubband
dm_tolerance           = 3       # tolerance in DM when comparing candidates with known strong PSRs
period_tolerance       = 0.0005  # (s) - tolerance in P when comparing candidates with known strong normal PSRs
ms_period_tolerance    = 0.00001 # (s) - tolerance in P when comparing candidates with known strong mPSRs (P<30ms)
max_psr_harm           = 10      # maximum harmonic of known pulsars to consider when comparing with candidates
rfifind_blocks         = 2048    # number of blocks to integratea (for rfifind), usually ~2.6 s
singlepulse_threshold  = 5.0     # threshold SNR for candidate determination
singlepulse_plot_SNR   = 5.5     # threshold SNR for singlepulse plot
singlepulse_maxwidth   = 0.1     # max pulse width in seconds
to_prepfold_sigma      = 6.0     # incoherent sum significance to fold candidates
max_lo_cands_to_fold   = 20      # Never fold more than this many lo-accel candidates
lo_accel_numharm       = 16      # max harmonics
lo_accel_sigma         = 2.0     # threshold gaussian significance
lo_accel_zmax          = 0       # bins
lo_accel_flo           = 2.0     # Hz
max_hi_cands_to_fold   = 10      # Never fold more than this many hi-accel candidates
hi_accel_numharm       = 8       # max harmonics
hi_accel_sigma         = 3.0     # threshold gaussian significance
hi_accel_zmax          = 50      # bins
hi_accel_flo           = 1.0     # Hz
numhits_to_fold        = 2       # Number of DMs with a detection needed to fold
extra_prepfold_options = ""      # Extra options that can be passed to prepfold command to fold candidates (e.g. -nosearch)
low_DM_cutoff          = 1.0     # Lowest DM to consider as a "real" pulsar
waittime               = 5       # in sec. Interval to check for searching scripts to finish
### Description of the DDplans ###
#
# "shallow" is for the 1st Pulsar Shallow survey
# my run: DDplan.py -f 142.376708984375 -b 6.8359375 -n 560 -t 0.00032768 -d 500.0 -r 0.5 
#                              LODM   DMSTEP NDM/call   #calls NCHAN DOWNSAMP
ddplans_heap = {"shallow" : [ [0.00,   0.03, 1044,       1,    560,     1],
                              [31.32,  0.05, 380,        1,    560,     2], 
                              [50.32,  0.1,  449,        1,    560,     4],
                              [95.22,  0.2,  463,        1,    560,     8],
                              [187.82, 0.5,  445,        1,    560,    16],
                              [410.32, 1.0,  90,         1,    560,    32]
                            ]
               }
#
# Sifting specific parameters (don't touch without good reason!)
sifting.sigma_threshold = to_prepfold_sigma-1.0  # incoherent power threshold (sigma)
sifting.c_pow_threshold = 100.0                  # coherent power threshold
sifting.r_err           = 1.1    # Fourier bin tolerence for candidate equivalence
sifting.short_period    = 0.0005 # Shortest period candidates to consider (s)
sifting.long_period     = 15.0   # Longest period candidates to consider (s)
sifting.harm_pow_cutoff = 8.0    # Power required in at least one harmonic
###################################################################################################

def get_baryv(ra, dec, mjd, T, obs="LOFAR"):
    """
    get_baryv(ra, dec, mjd, T):
        Determine the average barycentric velocity towards 'ra', 'dec'
        during an observation from 'obs'.  The RA and DEC are in the
        standard string format (i.e. 'hh:mm:ss.ssss' and 'dd:mm:ss.ssss').
        'T' is in sec and 'mjd' is (of course) in MJD.
    """
    tts = pu.span(mjd, mjd+T/86400.0, 100)
    nn = len(tts)
    bts = numpy.zeros(nn, dtype=numpy.float64)
    vel = numpy.zeros(nn, dtype=numpy.float64)
    presto.barycenter(tts, bts, vel, nn, ra, dec, obs, "DE200")
    return vel.mean()

class dedisp_plan:
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

def timed_execute(cmd, run_cmd=1):
    """
    timed_execute(cmd):
        Execute the command 'cmd' after logging the command
            to STDOUT.  Return the wall-clock amount of time
            the command took to execute.
    """
    sys.stdout.write("\n'"+cmd+"'\n")
    sys.stdout.flush()
    start = time.time()
    if run_cmd:  os.system(cmd) # for test all command are blocked
    end = time.time()
    return end - start


def radial_distance(RA1,DEC1,RA2,DEC2):
    """
     radial_distance: prog calculates radial distance between 
		given (RA1 DEC1) and (RA2 DEC2) position. It uses the formula 
	 	used in ATNF pulsar catalogue to calculate distances between 
		the pulsars. It is knowm as Haversine formula and it 
		can be found here. http://en.wikipedia.org/wiki/Great-circle_distance

    """
    RA1 = RA_to_rad(RA1)
    RA2 = RA_to_rad(RA2)
    DEC1 = DEC_to_rad(DEC1)
    DEC2 = DEC_to_rad(DEC2)
   
    dist = 2*math.asin(math.sqrt((math.sin((DEC1 - DEC2)/2))**2 + math.cos(DEC1)*math.cos(DEC2)*(math.sin((RA1-RA2)/2))**2))
    dist = rad_to_deg(dist)
    return dist

def RA_to_rad(RA):
    """
    RA_to_rad : converts RA which is in hh:mm:ss.ss formate into radian
    """
    RA = RA.split(":")	
    RA_H = float(RA[0])
    RA_M = float(RA[1])
    RA_S = float(RA[2])
  
    deg = (RA_H + RA_M/60 + RA_S/3600)*15
    rad = deg * math.pi / 180.0
    return rad
  
def DEC_to_rad(DEC):
    """
    DEC_to_rad : converts radial dd:mm:ss.ss into radian
    """
    DEC = DEC.split(":")
    DEC_H = float(DEC[0])
    DEC_M = float(DEC[1])
    DEC_S = float(DEC[2])
   
    deg = (DEC_H + DEC_M/60 + DEC_S/3600)
    rad = deg * math.pi / 180.0
    return rad


def rad_to_deg(rad):
    """
    deg_to_rad : converts radian to deg
    """
    deg = rad * 180 / math.pi
    return deg

# function to be used when sorting the list of found known pulsars
def psr_compare (a, b):
	return cmp(float(a[4]), float(b[4]))

# function to be used when sorting the list of harmonics of known pulsars
def harm_compare (a, b):
	return cmp(float(a[6]), float(b[6]))	

# function to compare when sorting the list of candidates
def upo_compare (a, b):
	return cmp(float(a[2]), float(b[2]))

# function to be used when sorting DMs
def dm_compare (a, b):
	return cmp(float(a), float(b))

# function that checks that pulsar is already in the list of found bright pulsars
def does_pulsar_exist (psrname, realcand):
	for el in realcand:
		if (el[0] == psrname):
			return True
	return False

# function that checks that the same harmonic of the same pulsar is already in the list of harmonics found
def does_harm_exist (harm, psrname, harmcand):
	for el in harmcand:
		if (el[0] == harm and el[1] == psrname):
			return True
	return False

# function that adjusts DDplan in order that number of DMs in each pass
# will be divisible by (ncores - 1). Also should take care of if number of DMs > blk, then
# more than 1 iterations should be arranged. Value of blk should already be divisible by (ncores - 1)
def adjust_ddplan (ddplans, blk, ncores):
	np = ncores - 1
	adjusted_ddplans = []
	for pp in range(0, len(ddplans)):
		totdm = ddplans[pp].dmsperpass * ddplans[pp].numpasses
		hidm = ddplans[pp].lodm + totdm * ddplans[pp].dmstep 
		if pp != 0:
			ddplans[pp].lodm = ddplans[pp-1].lodm + ddplans[pp-1].dmsperpass * ddplans[pp-1].dmstep
		dmrange = hidm - ddplans[pp].lodm
		ndms = int (math.ceil(dmrange / ddplans[pp].dmstep))
		niter = int(ndms/blk) # number of iterations
		leftdm = ndms % blk   # leftover number of DMs
		if leftdm % np != 0:
			leftdm = (int(leftdm/np) + 1) * np
		ndms = niter * blk + leftdm
		ddplans[pp].dmsperpass = ndms
		ddplans[pp].numpasses = 1
		# we need to init a new dedisp_plan (rather than correct the old one) 
		# because we should also correct the list of DMs inside the DDplan class
		adjusted_ddplans.append(dedisp_plan(ddplans[pp].lodm, ddplans[pp].dmstep, ndms, 1, ddplans[pp].numsub, ddplans[pp].downsamp))
	return adjusted_ddplans

# function that creates the generic birdies file
def create_birdies_file (dir, name):
	bname = dir + name
        fid = open(bname, "w")
	line="# Freq\t\tWidth\n#--------------------\n50.0\t\t0.05\n100.0\t\t0.10\n"
        fid.write(line)
	fid.close()
	return bname

# function that checks what freq channels fall into RFI range
# and return the range string for rfifind
def get_zapchan_range (freq, width, lofq, chan_width, bandwidth):
	start_freq=freq-width/2.
	end_freq=start_freq+width
	if start_freq < lofq-chan_width/2.: 
		start_freq=lofq-chan_width/2.
	if end_freq > lofq-chan_width/2.+bandwidth: 
		end_freq = lofq-chan_width/2.+bandwidth
	first_zap = int((start_freq-lofq+chan_width/2.)/chan_width)
	n_zap = int((end_freq - start_freq + 0.75*chan_width)/chan_width)
	end_zap=first_zap+n_zap-1
	# the range in rfifind is specified by ":"
	return "%d:%d" % (first_zap, end_zap)

# function to get the folding parameters for prepfold
def get_folding_options(cand):
	"""
	Return folding options for prepfold for folding the subbands
	"""
	# Folding rules are based on the facts that we want:
	#   1.  Between 24 and 200 bins in the profiles
	#   2.  For most candidates, we want to search length = 101 p/pd/DM cubes
	#       (The side of the cube is always 2*M*N+1 where M is the "factor",
	#       either -npfact (for p and pd) or -ndmfact, and N is the number of bins
	#       in the profile).  A search of 101^3 points is pretty fast.
	#   3.  For slow pulsars (where N=100 or 200), since we'll have to search
	#       many points, we'll use fewer intervals in time (-npart 30)
	#   4.  For the slowest pulsars, in order to avoid RFI, we'll
	#       not search in period-derivative.
	#   characters "_" will be substituted by spaces in the script
	p = 1.0 / cand.f
	if p < 0.002:
		Mp, Mdm, N = 2, 2, 24
		otheropts = "-npart_50_-ndmfact_3"
	elif p < 0.05:
		Mp, Mdm, N = 2, 1, 50
		otheropts = "-npart_40_-pstep_1_-pdstep_2_-dmstep_3"
	elif p < 0.5:
		Mp, Mdm, N = 1, 1, 100
		otheropts = "-npart_30_-pstep_1_-pdstep_2_-dmstep_1"
	else:
		Mp, Mdm, N = 1, 1, 200
		otheropts = "-npart_30_-nopdsearch_-pstep_1_-pdstep_2_-dmstep_1"
	otheropts += "_-n_%d_-npfact_%d_-ndmfact_%d" % (N, Mp, Mdm)
	return otheropts
	
# function to create general search script to exploit the parallelization
def create_search_script(name, is_fold_dat):
	batchfile = open(name,"w")
	batchfile.write("#!/bin/bash\n#\n")
	batchfile.write("outdir=$1\n")
	batchfile.write("shift\n")
	batchfile.write("core=$1\n")
	batchfile.write("shift\n")
	batchfile.write("log=\"$outdir/searching_core$core.log\"\n")
	batchfile.write("end=\"$outdir/core$core.done\"\n")
	batchfile.write("date >> $log\n")
	batchfile.write("for stem in $*; do\n")
	batchfile.write(" echo $stem.dat >> $log\n")
	batchfile.write(" echo \"realfft $stem.dat >> $log\" >> $log\n")
	batchfile.write(" realfft $stem.dat >> $log\n")
	batchfile.write(" echo \"zapbirds -zap -zapfile %s -baryv %.6g $stem.fft >> $log\" >> $log\n" % (birdies_file, baryv))
	batchfile.write(" zapbirds -zap -zapfile %s -baryv %.6g $stem.fft >> $log\n" % (birdies_file, baryv))
	batchfile.write(" echo \"rednoise $stem.fft >> $log\" >> $log\n")
	batchfile.write(" rednoise $stem.fft >> $log\n")
	batchfile.write(" mv $stem\"_red.fft\" $stem.fft\n")
	batchfile.write(" echo \"accelsearch -numharm %d -sigma %f -zmax %d -flo %f $stem.fft >> $log\" >> $log\n" % (lo_accel_numharm, lo_accel_sigma, lo_accel_zmax, lo_accel_flo))
	batchfile.write(" accelsearch -numharm %d -sigma %f -zmax %d -flo %f $stem.fft >> $log\n" % (lo_accel_numharm, lo_accel_sigma, lo_accel_zmax, lo_accel_flo))
	# running hi-accel search
	if is_run_hi_accel_search:
		batchfile.write(" echo \"accelsearch -numharm %d -sigma %f -zmax %d -flo %f $stem.fft >> $log\" >> $log\n" % (hi_accel_numharm, hi_accel_sigma, hi_accel_zmax, hi_accel_flo))
		batchfile.write(" accelsearch -numharm %d -sigma %f -zmax %d -flo %f $stem.fft >> $log\n" % (hi_accel_numharm, hi_accel_sigma, hi_accel_zmax, hi_accel_flo))
	###
	batchfile.write(" echo \"single_pulse_search.py --noplot --maxwidth %f --threshold %f $stem.dat >> $log\" >> $log\n" % (singlepulse_maxwidth, singlepulse_threshold))
	batchfile.write(" single_pulse_search.py --noplot --maxwidth %f --threshold %f $stem.dat >> $log\n" % (singlepulse_maxwidth, singlepulse_threshold))
	if is_fold_dat:  # keeping the dat-files
		batchfile.write(" rm -f $stem.fft\n")
	else:
		batchfile.write(" rm -f $stem.dat $stem.fft\n")
	batchfile.write("done\n")
	batchfile.write("date >> $log\n")
	batchfile.write("touch $end\n")
	batchfile.close()
	# make it executable
	cmd="chmod +x %s" % (name)
	os.system(cmd)

# function to create general script to fold the candidates
def create_fold_script(scratchdir, name, maskfile, extra_prepfold_options, basename, infile):
	batchfile = open(scratchdir+name,"w")
	batchfile.write("#!/bin/bash\n#\n")
	batchfile.write("outdir=$1\n")
	batchfile.write("shift\n")
	batchfile.write("core=$1\n")
	batchfile.write("shift\n")
	batchfile.write("log=\"$outdir/prepfold_core$core.log\"\n")
	batchfile.write("end=\"$outdir/fold_core$core.done\"\n")
	batchfile.write("workdir=\"%s.$core\"\n" % (scratchdir))
	batchfile.write("curdir=`pwd`\n")
	batchfile.write("mkdir -p $workdir\n")
	batchfile.write("cd $workdir\n")       # we need this temp working dir to have different places for resid2.tmp files
					       # produced by prepfold
	batchfile.write("date >> $log\n")
	batchfile.write("for params in $*; do\n")
	batchfile.write(" echo >> $log\n")
	batchfile.write(" accelcand=`echo $params | cut -d : -f 1`\n")
	batchfile.write(" accelfile=`echo $params | cut -d : -f 2`\n")
	batchfile.write(" dm=`echo $params | cut -d : -f 3`\n")
	batchfile.write(" outfile=`echo $params | cut -d : -f 4`\n")
	batchfile.write(" options=`echo $params | cut -d : -f 5 | sed s/\_/\ /g -`\n")
	if is_fold_dat:  # if dat-files are kept, then using as input the corresponding dat-file
		batchfile.write(" echo \"prepfold -noxwin -mask %s -runavg -accelcand $accelcand -accelfile $accelfile -dm $dm -o $outfile $options %s %s%s_DM$dm.dat >> $log\" >> $log\n" % (maskfile, extra_prepfold_options, scratchdir, basename))
		batchfile.write(" prepfold -noxwin -mask %s -runavg -accelcand $accelcand -accelfile $accelfile -dm $dm -o $outfile $options %s %s%s_DM$dm.dat >> $log\n" % (maskfile, extra_prepfold_options, scratchdir, basename))
	else:
		batchfile.write(" echo \"prepfold -noxwin -mask %s -runavg -accelcand $accelcand -accelfile $accelfile -dm $dm -o $outfile $options %s %s >> $log\" >> $log\n" % (maskfile, extra_prepfold_options, infile))
		batchfile.write(" prepfold -noxwin -mask %s -runavg -accelcand $accelcand -accelfile $accelfile -dm $dm -o $outfile $options %s %s >> $log\n" % (maskfile, extra_prepfold_options, infile))
	batchfile.write(" echo >> $log\n")
	batchfile.write("done\n")
	batchfile.write("mv -f * %s\n" % (scratchdir))
	batchfile.write("cd $curdir\n")
	batchfile.write("rm -rf $workdir\n")
	batchfile.write("date >> $log\n")
	batchfile.write("touch $end\n")
	batchfile.close()
	# make it executable
	cmd="chmod +x %s" % (scratchdir+name)
	os.system(cmd)

# function that is the common searching block to be executed after each mpirun
def run_searching (scratchdir, outfile, search_script, waittime):
	start_search_time = time.time()
	# cleaning '*.done' files
	cmd="rm -f %s" % (scratchdir+"core*.done")
	os.system(cmd)

	# making a sorted list of DMs
	dms=[f.split(".dat")[0].split("_DM")[1] for f in glob.glob(scratchdir + "*_DM*.dat")]
	dms.sort(dm_compare)
	ndms=numpy.size(dms)
	start_block=0
	nprocess=0  # number of started background processes
	for core in numpy.arange(0, ncores):
		if ndms == 0:  # in case number of DMs is less then ncores
			break
		if ndms%(ncores-core) != 0: 
			dmblock=int(ndms/(ncores-core))+1
		else:
			dmblock=int(ndms/(ncores-core))
		dmslice=dms[start_block:start_block+dmblock]	
		datgroup=["%s_DM%s" % (outfile, dm) for dm in dmslice]
		cmd="%s%s %s %d %s &" % (scratchdir, search_script, scratchdir, core, " ".join(datgroup))
		print "Starting searching on core %d for %d DMs [%s - %s] ..." % (core, dmblock, dmslice[0], dmslice[-1])
		sys.stdout.flush()
		os.system(cmd)	
		ndms -= dmblock
		start_block += dmblock
		nprocess += 1
	# waiting for searching scripts to finish
	# checking the presence "*.done" files that indicate that script instance has finished
	if nprocess != 0:
		cmd="/bin/bash -c \"ls -1 %s%s 2>/dev/null | wc -l\"" % (scratchdir, "core?.done")
		while (1):
			nfinish = int(os.popen(cmd).readlines()[0][:-1])
			if nfinish == nprocess:
				cmd="rm -f %s" % (scratchdir+"core?.done")
				os.system(cmd)
				print "Finished."
				break
			time.sleep(waittime)
	end_search_time = time.time()
	search_time = end_search_time - start_search_time
	return search_time

#########################################################################################################################
#                                                     M A I N
#########################################################################################################################
if __name__ == "__main__":

	#
	# Parsing the command line options
	#
	parser = optparse.OptionParser()
	parser.add_option('--ddplan',dest='ddplanflag', metavar='0/1/2',
          	help="DDplan flag,  0: To run DDplan script and determine ddplan from it," 
		     "1: To use the hardwired DDplan, 2: For quick DM search",
	        default=3, type='int')
	parser.add_option('--i',dest='inpath',metavar='INPATH',
       		help='Path to input files',
	        type='string')
	parser.add_option('--o',dest='outpath',metavar='OUTPATH',
	        help='Output directory where all results will be copied. If exists it will overwrite it if not then it will create one.',
	        type='string')
	parser.add_option('--s',dest='scratchpath',metavar='SCRATCHDIR',
	        help='Scratch directory where all the processing is done. Default is /dev/shm', 
		default="/dev/shm", type='string')
	parser.add_option('--zapeveryN',dest='zapeveryN',metavar='ZAP_EVERY_Nth_CHANNEL',
                help='Rfifind will zap every N\'th channel, default - every 16th',
                default=16, type='int')	
	parser.add_option('--no-hi-accel', action="store_false", dest="is_run_hi_accel_search",
                help='Do not run hi-accel searching. Default is both lo-accel and hi-accel', default=True)          
	parser.add_option('--skip-rfifind', action="store_true", dest="is_skip_rfifind",
                help='Skip running rfifind. However (mpi)prepsubband still expects rfi-mask file in the scratch directory', default=False)          
	parser.add_option('--skip-dedispersion', action="store_true", dest="is_skip_dedispersion",
                help='Skip running (mpi)prepsubband. Only searching will be done on all available *.dat files', default=False)          
	parser.add_option('--sift-and-fold-only', action="store_true", dest="is_sift_n_fold_only",
                help='Skipping everything except sifting and folding the candidates', default=False)          
	parser.add_option('--lodm',dest='lodm',metavar='LO_DM',
                help='low dm vlaue for quick DM search, only used when ddplan = 2, default = 0.0',
		default=0, type='float')
	parser.add_option('--dmstep',dest='dmstep',metavar='DM_STEP',
                help='dm step for quick DM search, only used when ddplan = 2, default = 1',
                default=1, type='float')
	parser.add_option('--ndm',dest='ndm',metavar='Num_DM',
                help='Number of DMs for quick DM search, only used when ddplan = 2, default = 150. Note: with mpi this number will be adjusted to be divisible by (np - 1)',
                default=150, type='float')
	parser.add_option('--downsamp',dest='downsamp',metavar='DOWNSAMPLE',
                help='Down sample the input data for quick DM search, only used when ddplan = 2, default = 1',
                default=1, type='float')
	parser.add_option('--no-mpi', action="store_false", dest="mpiflag",
                help='To use regular prepsubband instead of mpiprepsubband', default=True)          
	parser.add_option('--np',dest='np',metavar='NUMBER_OF_CORES',
                help='Number of cores to be used with (mpi)prepsubband and searching and folding steps. default = 8.',
                default=8, type='int')	

	options, args = parser.parse_args()
	
	if (not options.inpath) or (not options.outpath):
	        print 'Input and output paths are required.'
        	print parser.print_help()
	        sys.exit(1)

	# If user did not choose explicitely DDplan
	if options.ddplanflag == 3:
		print "DDplan usage is not set. Use --ddplan option and maybe also --lodm, --dmstep, --ndm options"
		sys.exit(1)
	if options.ddplanflag < 0 or options.ddplanflag > 3:
		print "DDplan usage is incorrect. Choose the correct one"
		sys.exit(1)

	# Assigning options variable to global ones
	inpath = options.inpath
	outpath = options.outpath	
	scratchpath = options.scratchpath
	ddplanflag = options.ddplanflag
	LODM = options.lodm
	DMSTEP = options.dmstep
	NDM = options.ndm
	DOWNSAMP = options.downsamp
	mpiflag = options.mpiflag	
	ncores = options.np
	zapeveryN = options.zapeveryN
	is_run_hi_accel_search = options.is_run_hi_accel_search
	is_skip_rfifind = options.is_skip_rfifind
	is_skip_dedispersion = options.is_skip_dedispersion
	is_sift_n_fold_only = options.is_sift_n_fold_only
	if is_sift_n_fold_only:
		is_skip_rfifind = True
		is_skip_dedispersion = True
	# adjusting the block value if using MPI, to have it divisible by (ncores - 1)
	if mpiflag:
		np = ncores - 1 
	        blk = int(blk/np) * np

	# Inpath, outpath, and scratchpath can have / at the end or it can be without it. 
        # In both the cases it will process the script
	if (inpath.split('/')[-1]==''): 
		indir = inpath 
	else: 
		indir = inpath + "/"
	
	if (outpath.split('/')[-1]==''):
                outdir = outpath
	else:
		outdir = outpath + "/"

	# if using /dev/shm as a scratch, then also create an extra sub-dir to keep search stuff separate
	if scratchpath == "/dev/shm":
		scratchpath = scratchpath + "/" + outdir.split("/")[-2]

	if (scratchpath.split('/')[-1]==''):
                scratchdir = scratchpath
	else:
		scratchdir = scratchpath + "/"

	infile = indir + "*.sub[0-9]???"
	inffile = glob.glob(indir+"*.sub.inf")
	
	# basename for the output file (same as input file excluding .sub*)
	basename = inffile[0].split("/")[-1].split(".sub")[0]
	outfile = scratchdir + basename
	
	totime = 0
	if(not os.path.isdir(indir)):
		print "Input dir does not exist\n"
		sys.exit()
	if(not os.path.isdir(outdir)):
		print "\nOutput dir does not exist. Created: " + outpath
		cmd="mkdir -p " + outpath
	        totime += timed_execute(cmd,1)
	if(not os.path.isdir(scratchdir)):
		print "\nScratch dir does not exist. Created: " + scratchpath
		cmd="mkdir -p " + scratchpath
	        totime += timed_execute(cmd,1)

       
	# Reading the inf-file
	id = inf.infodata(inffile[0])
        lofq = id.lofreq   # central freq of the lowest channel
	bandwidth = id.BW  # total BW
        chan = id.numchan  # number of channels
	chan_width = id.chan_width  # width of the channel
	samptime = id.dt   # sampling interval
	mjd = id.epoch     # MJD
	dur = id.N * id.dt # duration of obs
	RA = id.RA
	DEC = id.DEC
	PSR = id.object
        # Determine the average barycentric velocity of the observation
        baryv = get_baryv(RA, DEC, mjd, dur, obs="LOFAR")

	# Checking if Master RFI list and Pulsar catalogs exist
	if not os.path.exists(bright_catalog):
		print "Can't fine the catalog of brightest pulsars: %s" % (bright_catalog)
		sys.exit(1)
	if not os.path.exists(rfi_master_file):
		print "Can't find the RFI master list: %s" % (rfi_master_file)
		sys.exit(1)

	birdies_comment=""
	if birdies_file == "": # not specified, so will generate the basic one with 50 and 100 Hz 
		birdies_comment=" (generic with 50 & 100 Hz harmonics)"
		# create generic birdies file
		birdies_file = create_birdies_file (scratchdir, "generic.zaplist")
	else:
		if not os.path.exists(birdies_file):
			print "Can't find the birdies zaplist: %s" % (birdies_file)
			sys.exit(1)


	# Printing some info about the dataset and in/out directories
	print
	print "CHANNELS : %d" % (chan)
	print "Lowest Frequency : %f" % (lofq)
	print "BANDWIDTH : %f" % (bandwidth)
	print "SAMPLING TIME : %f" % (samptime)
	print "MJD : %f" % (mjd)
	print "DURATION : %f" % (dur)
	print "INPUT File(s) :  %s" % (infile)
	print "SCRATCH Dir : %s" % (scratchdir)
	print "OUTPUT Dir : %s" % (outdir)
	print "INF FILE : %s" % (inffile[0])
	print "PULSAR CATALOG : %s" % (bright_catalog)
	print "RFI MASTER FILE : %s" % (rfi_master_file)
	print "BIRDIES' FILE : %s%s" % (birdies_file, birdies_comment) 
	print "ZAP EVERY %d'th CHANNEL" % (zapeveryN)
	print

	ddplans = []   # chosen set of DDplans
 
	# running DDlan.py  in auto mode
	if ddplanflag == 0:
		print "Auto run of DDplan.py ..."
		outddfile = scratchdir + "DDplan.log"
		cmd = "DDplan.py -f %s -b %s -n %s -t %f -r %f -o %s > %s" % (lofq+(bandwidth/2), bandwidth, chan, samptime, samptime, scratchdir + "DDplan.ps", outddfile)
		timed_execute(cmd,1)
   
		fp = open(outddfile,"r")
		lines = fp.read().split("\n")
		fp.close()

		# This is to take direct plan by runing DDplan.py command and using the output to do (mpi)prepsubband
		# The order in which arg are passed ==> lodm  dmstep  dms/call  num_calls  num_sub  downsamp
		ddplan_calls = []  # array of sub-arrays of DDplan values for all separate calls
		for ii in range(0, len(lines)):
			if 'WorkFract' in lines[ii]:
				start_line = ii + 1
				break
		for ii in range(start_line, len(lines)):
			if lines[ii] == '': break
			ddplan_calls.append(lines[ii].split())
		if len(ddplan_calls) == 0:  # something that should not happen
			print "Error while running DDplan.py or reading the DDplan"
			sys.exit(1)
		for call in ddplan_calls:
			ddplans.append(dedisp_plan(float(call[0]), float(call[2]), float(call[4]), 1, chan, float(call[3])))

	# This is for hardcoded DDplan for RSPA with HBA. Cant be used with mpiprepsubband  
	if ddplanflag == 1: 
		print "Hardwired values of the DDplan will be used"
		for ipass in numpy.arange(0, len(ddplans_heap[hwired_ddplan_id])):
			ddplans.append(dedisp_plan(ddplans_heap[hwired_ddplan_id][ipass][0], \
                                                   ddplans_heap[hwired_ddplan_id][ipass][1], \
                                                   ddplans_heap[hwired_ddplan_id][ipass][2], \
                                                   ddplans_heap[hwired_ddplan_id][ipass][3], \
                                                   ddplans_heap[hwired_ddplan_id][ipass][4], \
                                                   ddplans_heap[hwired_ddplan_id][ipass][5]))

	# User-specified values for low, high DMs and DM step
	if ddplanflag == 2:
		print "Quick DM search with DM range [%g - %g] and DM step = %g" % (LODM, HIDM, DMSTEP)	
		ddplans = [dedisp_plan(LODM, DMSTEP, NDM, 1, chan, DOWNSAMP)]

	# Adjusting DDplan when using MPI
	# This is necessary due to mpiprepssuband can not run more than 1000 DMs simultaneously and
	# number of DMs should be divisible by (ncores - 1)
	if mpiflag:
		print "DDplan will be adjusted in order to #DMs in each pass to be divisible by (%d - 1) nodes" % (ncores)
		ddplans = adjust_ddplan (ddplans, blk, ncores)

	# printing the DM plan to be used
	print "\n%-7s %-6s %-9s %-8s %-7s" % ("LODM", "DMSTEP", "DMPERPASS", "DOWNSAMP", "NUMPASS")
        for ddplan in ddplans:
                 print "%-7.2f %-6.2f %-9d %-8d %-7d" % (ddplan.lodm, ddplan.dmstep, ddplan.dmsperpass, ddplan.downsamp, ddplan.numpasses)
	print
	# Forming the list of all DMs (to be used later in sifting step)
	dmstrs = []
	for ddplan in ddplans:
		for passnum in range(ddplan.numpasses):
			for dmstr in ddplan.dmlist[passnum]:
				dmstrs.append(dmstr)

	# Open file that keeps timing info of each processing step
	exectime_file = outfile + ".exectime"
        rfp = open(exectime_file,"w")
	rfp.write("\n")
	
        # rfifind (block ~2.6s)
	# create a string of channels to zap
	# zap every 16'th channel
	zapchans=range(0,chan,zapeveryN)
	zapstring=",".join(["%d" % (f) for f in zapchans])
	# read rfi_master_file
	rfi_center_freq, rfi_width = numpy.loadtxt(rfi_master_file, dtype=float, usecols=(0,1), comments='#', unpack=True)
	# zap channels from rfi_master_file
	master_zapchans=[]
	for (fr, wi) in zip (rfi_center_freq, rfi_width):	
		# checking first that at least part of the RFI freq lies within our bandwidth
		if (fr-wi/2. >= lofq-chan_width/2. and fr-wi/2. <  lofq-chan_width/2.+bandwidth) or \
                   (fr+wi/2. >  lofq-chan_width/2. and fr+wi/2. <= lofq-chan_width/2.+bandwidth):
			master_zapchans.append(get_zapchan_range(fr, wi, lofq, chan_width, bandwidth))
	if numpy.size(master_zapchans) != 0:
		master_zapchans_str=",".join(master_zapchans)
		zapstring=master_zapchans_str+","+zapstring
	# running rfifind
        cmd="rfifind -blocks %d -zapchan %s -o %s %s > %s" % (rfifind_blocks, zapstring, outfile, infile, outfile+"_rfifind.log")
        print "Running RFI excision..."
	if is_skip_rfifind:  # skipping rfifind
		print "skipped"
		rfi_time = 0
	else:
		rfi_time = timed_execute(cmd,1)

	print "RFI masking time (s) : %.2f   [%.1f h]" % (rfi_time, rfi_time/3600.)
	rfp.write("RFI masking time (s) : %.2f   [%.1f h]\n" % (rfi_time, rfi_time/3600.))
        totime += rfi_time
	maskfile=outfile+"_rfifind.mask"
	
	print "\nMASK FILE : %s\n" % (maskfile)

	# cleaning previous log files
	cmd="rm -f %s %s %s" % (outfile + "_prepsubband.log", scratchdir+"searching_core*.log", scratchdir+"core*.done")
	totime += timed_execute(cmd,1)	

	# Make a search script and run searching in parallel using 'ncores' cores
	search_script = "psrsearch.sh"
	print "Creating search script: %s" % (search_script)
	create_search_script(scratchdir+search_script, is_fold_dat)

	tot_prep_time = 0
	tot_search_time = 0

	# only running this part if it's not skipped from the commmand line
	if not is_skip_dedispersion:
		if not mpiflag:
			print "\nRunning prepsubband ...."
			for ddplan in ddplans:
				totdm = ddplan.dmsperpass * ddplan.numpasses
				print "Pass for DM range: [%g-%g)" % (ddplan.lodm, ddplan.lodm + totdm * ddplan.dmstep)
 	
				# loop over the blocks of DMs if the total #DMs in this pass > blk (1000)
				niter = (totdm % blk == 0 and int(totdm/blk) or int(totdm/blk) + 1) # number of iterations
				for jj in range(0, niter):
					dm_start = ddplan.lodm + ddplan.dmstep * jj * blk
					if (totdm - jj*blk) >= blk: # Because prepsubband can handle only 1000 dm values
						dm_end = dm_start + ddplan.dmstep * blk
						cmd = "prepsubband -mask %s -runavg -noclip -lodm %.2f -dmstep %.2f -numdms %d -downsamp %d -o %s %s >> %s " % (maskfile, dm_start, ddplan.dmstep, blk, ddplan.downsamp, outfile,infile, outfile + "_prepsubband.log")
					else: # last chunk of the DMs
						dm_end = ddplan.lodm + ddplan.dmstep * totdm
						cmd = "prepsubband -mask %s -runavg -noclip -lodm %.2f -dmstep %.2f -numdms %d -downsamp %d -o %s %s >> %s " % (maskfile, dm_start, ddplan.dmstep, totdm - jj*blk, ddplan.downsamp,outfile,infile, outfile+"_prepsubband.log")

					print "\nIteration: %d  DM range: [%g-%g)" % (jj, dm_start, dm_end)
					prepsubband_time = timed_execute(cmd,1)
					tot_prep_time += prepsubband_time
					print "Prepsubband time (s) : %.2f   [%.1f h]" % (prepsubband_time, prepsubband_time/3600.)
					rfp.write("prepsubband time for DM range [%g-%g) and DM step = %g (s) : %.2f   [%.1f h]\n" % (dm_start, dm_end, ddplan.dmstep, prepsubband_time, prepsubband_time/3600.))
					totime += prepsubband_time

					# and run searching...
					search_time = run_searching (scratchdir, outfile, search_script, waittime)
					tot_search_time += search_time
					totime += search_time
					print "Search time (s) : %.2f   [%.1f h]" % (search_time, search_time/3600.)
					rfp.write ("Search time for DM range [%g-%g) and DM step = %g (s) : %.2f   [%.1f h]\n" % (dm_start, dm_end, ddplan.dmstep, search_time, search_time/3600.))

		# For mpi prepsubband to work the number of DM should be divisible by number of nodes/cores - 1 that can be used.
		# Here for given number of DMs, it will round off to the nearest value where it matches with the number divisible
		if mpiflag:
			print "\nRunning mpiprepsubband ...."
			for ddplan in ddplans:
				totdm = ddplan.dmsperpass * ddplan.numpasses
				print "Pass for DM range: [%g-%g)" % (ddplan.lodm, ddplan.lodm+totdm*ddplan.dmstep)

				# loop over the blocks of DMs if the total #DMs in this pass > blk (1000)
				niter = (totdm % blk == 0 and int(totdm/blk) or int(totdm/blk) + 1) # number of iterations
				for jj in range(0, niter):
					dm_start = ddplan.lodm + ddplan.dmstep * jj * blk
					if (totdm - jj*blk) >= blk: # Because prepsubband can handle only 1000 dm values
						dm_end = dm_start + ddplan.dmstep * blk
						cmd = "mpirun --mca btl ^openib -np %d mpiprepsubband -runavg -mask %s -noclip -lodm %.2f -dmstep %.2f -numdms %d -downsamp %d -o %s %s >> %s " % (np + 1, maskfile, dm_start, ddplan.dmstep, blk, ddplan.downsamp, outfile, infile, outfile+"_prepsubband.log")
					else:
						dm_end = ddplan.lodm + ddplan.dmstep * totdm
						cmd = "mpirun --mca btl ^openib -np %d mpiprepsubband -runavg -mask %s -noclip -lodm %.2f -dmstep %.2f -numdms %d -downsamp %d -o %s %s >> %s " % (np + 1, maskfile, dm_start, ddplan.dmstep, totdm - jj*blk, ddplan.downsamp, outfile, infile, outfile+"_prepsubband.log")

					print "\nIteration: %d  DM range: [%g-%g)" % (jj, dm_start, dm_end)
					prepsubband_time = timed_execute(cmd,1)
					tot_prep_time += prepsubband_time
					print "MPI Prepsubband time (s) : %.2f   [%.1f h]" % (prepsubband_time, prepsubband_time/3600.)
					rfp.write("mpiprepsubband time for DM range [%g-%g) and DM step = %g (s) : %.2f   [%.1f h]\n" % (dm_start, dm_end, ddplan.dmstep, prepsubband_time, prepsubband_time/3600.))
					totime += prepsubband_time

					# and run searching...
					search_time = run_searching (scratchdir, outfile, search_script, waittime)
					tot_search_time += search_time
					totime += search_time
					print "Search time (s) : %.2f   [%.1f h]" % (search_time, search_time/3600.)
					rfp.write ("Search time for DM range [%g-%g) and DM step = %g (s) : %.2f   [%.1f h]\n" % (dm_start, dm_end, ddplan.dmstep, search_time, search_time/3600.))

		print "Total prepsubband time (s) : %.2f   [%.1f h]" % (tot_prep_time, tot_prep_time/3600.)
		rfp.write("Total prepsubband time (s) : %.2f   [%.1f h]\n" % (tot_prep_time, tot_prep_time/3600.))
		print "Total search time (s) : %.2f   [%.1f h]" % (tot_search_time, tot_search_time/3600.)
		rfp.write("Total search time (s) : %.2f   [%.1f h]\n" % (tot_search_time, tot_search_time/3600.))
		print
		sys.stdout.flush()

	# running the search here only when dedispersion skipped from the cmdline
	if is_skip_dedispersion and not is_sift_n_fold_only:
		print "\nRunning (mpi)prepsubband ... skipped"
		print "Searching ..."
		search_time = run_searching (scratchdir, outfile, search_script, waittime)
		totime += search_time
		print "Total search time (s) : %.2f   [%.1f h]" % (search_time, search_time/3600.)
		rfp.write ("Total search time (s) : %.2f   [%.1f h]\n" % (search_time, search_time/3600.))

	if is_sift_n_fold_only:
		print "\nRunning (mpi)prepsubband ... skipped"
		print "Running searching ... skipped"

	# Generating single-pulse plot
	print "Generating single-pulse plots ..."
	sys.stdout.flush()
	# before generating plots we have to cd to the scratch directory
	# and after go back to the current directory
	# Otherwise the default name of the ps-file could be ".p" rather than *.ps"
	currentdir=os.getcwd()
	os.chdir(scratchdir)

	basedmb = basename+"_DM"
	basedme = ".singlepulse "
	sp_time = 0
	# The following will make plots for DM ranges:
	#    0-30, 20-110, 100-310, 300-1000+, 0-1000+
	dmglobs = [basedmb+"[0-9].[0-9][0-9]"+basedme +
		basedmb+"[012][0-9].[0-9][0-9]"+basedme,
		basedmb+"[2-9][0-9].[0-9][0-9]"+basedme +
		basedmb+"10[0-9].[0-9][0-9]"+basedme,
		basedmb+"[12][0-9][0-9].[0-9][0-9]"+basedme +
		basedmb+"30[0-9].[0-9][0-9]"+basedme,
		basedmb+"[3-9][0-9][0-9].[0-9][0-9]"+basedme +
		basedmb+"1[0-9][0-9][0-9].[0-9][0-9]"+basedme,
		basedmb+"[0-9].[0-9][0-9]"+basedme +
		basedmb+"[0-9][0-9].[0-9][0-9]"+basedme +
		basedmb+"[0-9][0-9][0-9].[0-9][0-9]"+basedme +
		basedmb+"1[0-9][0-9][0-9].[0-9][0-9]"+basedme]
	dmrangestrs = ["0-30", "20-110", "100-310", "300-1000+", "0-1000+"]
	psname = basename+"_singlepulse.ps"
	for dmglob, dmrangestr in zip(dmglobs, dmrangestrs):
		try: # we need this in case there are not many DMs to make all plots
			cmd = 'single_pulse_search.py --threshold %f -g "%s"' % (singlepulse_plot_SNR, dmglob)
			sp_time += timed_execute(cmd,1)
			try:
				os.rename(psname, basename+"_DMs%s_singlepulse.ps"%dmrangestr)
       			except: pass
		except: pass
	totime += sp_time
	# cd back to the current directory
	os.chdir(currentdir)
	print
	print "Total time to make single-pulse plots (s) : %.2f   [%.1f h]" % (sp_time, sp_time/3600.)
	rfp.write("Total time to make single-pulse plots (s) : %.2f   [%.1f h]\n" % (sp_time, sp_time/3600.))
	sys.stdout.flush()
	
	#
	# Following will sort out the candidates 
	#
	# read_candidate will generate collective information about lo-accel and hi-accel cands
	print
	print "Sifting lo-accel candidates ..."
	sys.stdout.flush()
	start_sifting_time = time.time()
	lo_accel_cands = sifting.read_candidates(glob.glob(scratchdir + "*ACCEL_%d" % (lo_accel_zmax)))
	# Remove candidates with same period and low significance. 
	if len(lo_accel_cands):
		lo_accel_cands = sifting.remove_duplicate_candidates(lo_accel_cands) 
	if len(lo_accel_cands):
		lo_accel_cands = sifting.remove_DM_problems(lo_accel_cands, numhits_to_fold, dmstrs, low_DM_cutoff)
	if len(lo_accel_cands):
		lo_accel_cands.sort(sifting.cmp_sigma)
		sifting.write_candlist(lo_accel_cands,scratchdir+lo_acc_candlist_file)

	# if running hi-accel search
	if is_run_hi_accel_search:
		print "Sifting hi-accel candidates ..."
		sys.stdout.flush()
		hi_accel_cands = sifting.read_candidates(glob.glob(scratchdir + "*ACCEL_%d" % (hi_accel_zmax)))
		if len(hi_accel_cands):
			hi_accel_cands = sifting.remove_duplicate_candidates(hi_accel_cands) 
		if len(hi_accel_cands):
			hi_accel_cands = sifting.remove_DM_problems(hi_accel_cands, numhits_to_fold, dmstrs, low_DM_cutoff)
		if len(hi_accel_cands):
			hi_accel_cands.sort(sifting.cmp_sigma)
			sifting.write_candlist(hi_accel_cands,scratchdir+hi_acc_candlist_file)

	end_sifting_time = time.time()
	sifting_time = end_sifting_time - start_sifting_time
	totime += sifting_time
	print "Sifting the candidates time (s) : %.2f   [%.1f h]" % (sifting_time, sifting_time/3600.)
	rfp.write("Sifting the candidates time (s) : %.2f   [%.1f h]\n" % (sifting_time, sifting_time/3600.))
	sys.stdout.flush()

	# Read the pulsar catalog
	psrname, rRA, rDEC = numpy.loadtxt(bright_catalog, dtype=str, usecols=(1,3,4), comments='#', unpack=True)
	rPeriod, rDM, s400 = numpy.loadtxt(bright_catalog, dtype=float, usecols=(5,6,7), comments='#', unpack=True)

	# This part is to find out in all the candidates found, how many of them are known to be strong pulsars. 
	# In addition to that it also calculates the radial distances to these PSRs from the given data field
	# center. The Candidate DM and true pulsar DM will be compared withing range of 1 (dm_tolerance) 
	# and period will be consider in the range of 0.5 ms for normal PSRs (period_tolerance)
	# and 0.01 ms for mPSRs (ms_period_tolerance)
	realcand = []    # list of known pulsars found
	harmcand = []    # list of all (up to 10th) harmonics of known pulsars
	normalcand = []  # list of other candidates
	folded_cands = []

	accel_cands = lo_accel_cands[:]
        # if running hi-accel search, merging hi-accel candidates as well
        if is_run_hi_accel_search:
		accel_cands += hi_accel_cands

	for cand in accel_cands:
		is_normalcand = True
		is_pulsar_found = False
		# loop on list of known brightest pulsars
		for kk in numpy.arange(numpy.size(psrname)):
			# If the DMs are in +/- 5 range and periods are in +/- 0.5 msec range
			if abs(cand.DM - rDM[kk]) < dm_tolerance:
				# For harmonic search actual candidate period will be compared upto 
				# 10th (max_psr_harm) Harmonic of known pulsar period
				for harm in range(1, max_psr_harm):
					if  (cand.p >= 0.03 and abs(cand.p - rPeriod[kk]/harm) < period_tolerance) or \
					    (cand.p <  0.03 and abs(cand.p - rPeriod[kk]/harm) < ms_period_tolerance):
						if not does_harm_exist(harm, psrname[kk], harmcand):
							harmcand.append([harm, psrname[kk], rPeriod[kk], rDM[kk], cand.p, cand.DM, cand.sigma])
							if is_fold_known_psrs: # added candidate to folded list 
								folded_cands.append(cand)
						if not is_pulsar_found:
							is_pulsar_found = True
							is_normalcand = False
							# check first if this pulsar was picked up already by another candidate
							if not does_pulsar_exist(psrname[kk], realcand):
								radial_offset = radial_distance(RA, DEC, rRA[kk], rDEC[kk])
								realcand.append([psrname[kk], rPeriod[kk], rDM[kk], s400[kk], radial_offset])
			if is_pulsar_found: # move to another candidate
				break

		# If the candidate is not a known strong pulsar
		if is_normalcand:
			normalcand.append([cand.p, cand.DM, cand.sigma])	

	#
	# Writing the Candidates Info to the REPORT file
	#
	candrep_file = outfile + ".report"
        candrep = open(candrep_file, "w")
	print 
	print "=== Candidates Report === [ %s ]" % (basename + ".report")

	# This will write the detail report file containing information about detection and 
	# radial distances of the PSR detected. 
	str = "\nDetected pulsars: %d" % (len(realcand))
	candrep.write(str+"\n")
	print str
	if len(realcand) != 0:
		str = "PSR\t\tP(s)\t\tDM(pc/cm3)\ts400(mJy)\tOffset(deg)"
		candrep.write(str+"\n")
		print str
		# sorting by radial offset
		realcand.sort(psr_compare)
		for ii in realcand:
			str = "%s\t%f\t%g\t\t%g\t\t%.1f" % (ii[0], ii[1], ii[2], ii[3], ii[4])
			candrep.write(str+"\n")
			print str

	# Reporting about the harmonics of the known bright pulsars
	str = "\nHarmonics detected: %d" % (len(harmcand))
	candrep.write(str+"\n")
	print str
	if len(harmcand) != 0:
		str = "%-10s   %-10s   %-13s   %-11s   %-19s   %-16s   %-8s" % ("Harm_Num", "PSR", "Detected P(s)", "Delta P(ms)", "Detected DM(pc/cm3)", "Delta DM(pc/cm3)", "Sigma")
		candrep.write(str+"\n")
		print str
		# sorting by Sigma
		harmcand.sort(harm_compare, reverse=True)
		for ii in harmcand:
			str = "%-10d   %-10s   %-13g   %-11.3f   %-19g   %-16.3f   %-8.2f" % (ii[0], ii[1], ii[4], (ii[4]-ii[2]/ii[0])*1000., ii[5], (ii[5]-ii[3]), ii[6])
			candrep.write(str+"\n")
			print str
	
	# Reporting about normal candidates
	upos_to_print=20
	str = "\nUnidentified Pulsating Objects (UPOs): %d" % (len(normalcand))
	candrep.write(str+"\n")
	print str
	if len(normalcand) != 0:
		str = "#\tDetected P(s)\tDetected DM(pc/cm3)\tSigma"
		candrep.write(str+"\n")
		print str
		# sorting by Sigma
		normalcand.sort(upo_compare, reverse=True)
		counter = 0
		for ii in normalcand:
			str = "%d\t%f\t%.2f\t\t\t%.2f" % (counter, ii[0], ii[1], ii[2]) 
			candrep.write(str+"\n")
			if counter < upos_to_print:
				print str
			counter += 1
	print "..."
	print "Only %d best candidates are printed. The rest is in the *.report file" % (upos_to_print)
	candrep.close()

	#
	# Fold the best candidates
	#
	# cleaning previous log files
	cmd="rm -f %s %s" % (scratchdir+"prepfold_core*.log", scratchdir+"fold_core*.done")
	totime += timed_execute(cmd,1)	

	start_fold_time = time.time()
	cands_folded = 0
	for cand in lo_accel_cands:
		if cands_folded == max_lo_cands_to_fold:
			break
		elif cand.sigma > to_prepfold_sigma:
			cands_folded += 1
			folded_cands.append(cand) 
	# if running hi-accel search
	if is_run_hi_accel_search:
		cands_folded = 0
		for cand in hi_accel_cands:
			if cands_folded == max_hi_cands_to_fold:
				break
			elif cand.sigma > to_prepfold_sigma:
				cands_folded += 1
				folded_cands.append(cand) 
	nfolded=numpy.size(folded_cands)

	# Make a fold script and run it in parallel using 'ncores' cores
	print "Folding best %d candidates ..." % (nfolded)
	if is_fold_known_psrs:
		print "   including %d harmonics of possibly known pulsars" % (len(harmcand))
	fold_script = "psrfold.sh"
	print "Creating fold script: %s" % (fold_script)
	create_fold_script(scratchdir, fold_script, maskfile, extra_prepfold_options, basename, infile)
	sys.stdout.flush()
	start_block=0
	nprocess=0  # number of started background processes
	for core in numpy.arange(0, ncores):
		if nfolded == 0: # in case number of candidates to fold is less than ncores
			break
		if nfolded%(ncores-core) != 0: 
			foldblock=int(nfolded/(ncores-core))+1
		else:
			foldblock=int(nfolded/(ncores-core))
		foldslice=folded_cands[start_block:start_block+foldblock]	
		foldgroup=["%d:%s.cand:%s:%s:%s" % (ff.candnum, ff.filename, ff.DMstr, basename+"_DM%s_Z%s" % (ff.DMstr, ff.filename.split("_")[-1]), get_folding_options(ff)) for ff in foldslice]
		cmd="%s%s %s %d %s &" % (scratchdir, fold_script, scratchdir, core, " ".join(foldgroup))
		print "Starting folding on core %d for %d candidates ..." % (core, foldblock)
		sys.stdout.flush()
		os.system(cmd)	
		time.sleep(5)  # wait 5 sec before to start next script (in order not to mess with resid2.tmp)
		nfolded -= foldblock
		start_block += foldblock
		nprocess += 1
	# waiting for folding scripts to finish
	# checking the presence "fold_core*.done" files that indicate that script instance has finished
	if nprocess != 0:
		cmd="/bin/bash -c \"ls -1 %s%s 2>/dev/null | wc -l\"" % (scratchdir, "fold_core?.done")
		while (1):
			nfinish = int(os.popen(cmd).readlines()[0][:-1])
			if nfinish == nprocess:
				cmd="rm -f %s" % (scratchdir+"fold_core?.done")
				os.system(cmd)
				print "Finished."
				break
			time.sleep(waittime)
	end_fold_time = time.time()
	prepfold_time = end_fold_time - start_fold_time
	totime += prepfold_time
	print "Fold time (s) : %.2f   [%.1f h]" % (prepfold_time, prepfold_time/3600.)
	rfp.write("Total prepfold time (s) : %.2f   [%.1f h]\n" % (prepfold_time, prepfold_time/3600.))	
	sys.stdout.flush()

	# Now step through the .ps files and convert them to .png and gzip them
	# Also, creating tarballs of results and copy them to the OUTPUT directory
	psfiles = glob.glob(scratchdir + "*.ps")
    	for psfile in psfiles:
        	if "singlepulse" in psfile:
			cmd="convert %s %s" % (psfile, psfile.replace(".ps", ".png"))
        	else:
			cmd="convert -rotate 90 %s %s" % (psfile, psfile.replace(".ps", ".png"))
		totime += timed_execute(cmd,1)
		cmd="gzip -f %s" % (psfile)
		totime += timed_execute(cmd,1)

	print "Total runtime (s) : %.2f   [%.1f h]" % (totime, totime/3600.)
        rfp.write("Total runtime (s) : %.2f   [%.1f h]\n" % (totime, totime/3600.))
	rfp.close()
	sys.stdout.flush()

	print
	print "Archiving ..."
	sys.stdout.flush()
	# Tar up the results files
	tar_suffixes = ["_ACCEL_%d.tgz"%lo_accel_zmax, 
                        "_ACCEL_%d.cand.tgz"%lo_accel_zmax, 
                        "_singlepulse.tgz", 
                        "_inf.tgz", 
                        "_pfd.tgz",
                        "_pfd.bestprof.tgz",
                        ".log.tgz"]
	tar_globs = ["*_ACCEL_%d"%lo_accel_zmax,
                     "*_ACCEL_%d.cand"%lo_accel_zmax,
                     "*.singlepulse",
                     "*_DM[0-9]*.inf",
                     "*.pfd",
                     "*.pfd.bestprof",
                     "*.log"]
        # if running hi-accel search then add extra suffixes and globs
        if is_run_hi_accel_search:
		tar_suffixes = tar_suffixes + ["_ACCEL_%d.tgz"%hi_accel_zmax, "_ACCEL_%d.cand.tgz"%hi_accel_zmax]
		tar_globs = tar_globs + ["*_ACCEL_%d"%hi_accel_zmax, "*_ACCEL_%d.cand"%hi_accel_zmax]
	for (tar_suffix, tar_glob) in zip(tar_suffixes, tar_globs):
        	tf = tarfile.open(outfile + str(tar_suffix), "w:gz")
		for infile in glob.glob(scratchdir + str(tar_glob)):
            		tf.add(infile, arcname=infile.split("/")[-1])
	tf.close()

	print
	print "Rsync'ing the results to the output dir ..."
	print
	sys.stdout.flush()

	# Copy all the important files to the output directory
	if is_run_hi_accel_search:
		cmd = "rsync -avxP %s*rfifind.[bimors]* %s*.tgz %s*.ps.gz %s*.png %s*.exectime %s*.report %s*.sh %s %s %s" % (scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir+lo_acc_candlist_file, scratchdir+hi_acc_candlist_file, outdir)
	else:
		cmd = "rsync -avxP %s*rfifind.[bimors]* %s*.tgz %s*.ps.gz %s*.png %s*.exectime %s*.report %s*.sh %s %s" % (scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir, scratchdir+lo_acc_candlist_file, outdir)
	
	os.system(cmd)
	# Remove all the stuff in the scratchdir directory
	cmd = "rm -rf %s" % (scratchdir)
	os.system(cmd)
