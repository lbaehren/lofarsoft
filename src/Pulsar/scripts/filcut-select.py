#!/usr/bin/env python
#
# Script to make "cuts" of raw Sigproc data with GPs
# based on .singlepulse file and run fildedisp.py
# This is a wrapper for filextract.py and fildedisp.py
#
# Vlad, Aug 11, 2010 (c)
#########################################################
import numpy as np
import sys, os, glob
import getopt

# default time of GP and sample of GP
global spfile
# sigma threshold
threshold = 10.0
# width limit in samples (default is _very_ large to include all pulses)
width=1000
# size of the window
size=512
# fcenter (in case value in the header is wrong)
fc="-100"
# channel width (in case value in the header is wrong)
chbw="0"
# if True then save plots to png
is_saveonly = False
# freq and time decimation factors
fdf=1
tdf=1
# DM to use (if not set than value from .singlepulse file will be used)
# if True then use value from the command line rather than from .singlepulse file
is_dm = False
# All samples stronger than clip_level will be clipped (substituted by zero)
clip_level=1.e9
# Set the zoom region (around max value) for dedispersed spectrum
zoomtime=-1   # number of non-decimated samples. If == -1, show full array
# Width of GP (the spectrum will be calculated as average in this width around GP)



def usage (prg):
	""" Prints info how to use the script.
Input argument is the name of the script.
Return value is None
	"""
	print "Program %s extracts raw data for GPs using the .singlepulse file" % (prg, )
	print "Usage: %s [options] <*.fil>\n\
               [--file <.singlepulse file, this option is required>]\n\
               [--threshold <threshold value in sigmas, default=10>]\n\
               [--width <largest GP width to consider in samples (exclusive), default=1000 (all pulses)>]\n\
               [--size <number of samples to cut around GP, default=512>]\n\
               [--fcntr <value of central freq in MHz to use with fildedisp.py, if header value is wrong>]\n\
               [--chbw <value of channel bandwidth in MHz to use with fildedisp.py, if header value is wrong>]\n\
               [--fd <frequency decimation factor>]\n\
               [--td <time decimation factor>]\n\
               [--dm <value of DM to use with fildedisp.py, default - use value from .singlepulse file>]\n\
               [--cliplevel <all values larger than this (in sigmas) will be set to zero, default=1.e9>\n\
               [--zoomtime <zoom region in samples for dedispersed spectrum in samples>]\n\
               [-h, --help] - print this help" % (prg, )

def parsecmdline (prg, argv):
	""" Main script function. Takes two arguments, 
name of the script and list of the command line arguments.
Return value is None
	"""
	if not argv:
		usage(prg)
		sys.exit()
	else:
		try:
			opts, args = getopt.getopt (argv, "h", ["help", "file=", "threshold=", "width=", "size=", "fcntr=", "chbw=", "fd=", "td=", "dm=", "cliplevel=", "zoomtime="])
			for opt, arg in opts:
				if opt in ("-h", "--help"):
					usage(prg)
					sys.exit()
				if opt in ("--file"):
					global spfile
					spfile = arg
				if opt in ("--threshold"):
					global threshold
					threshold = float(arg)
				if opt in ("--width"):
					global width
					width = int(arg)
				if opt in ("--size"):
					global size
					size = int(arg)
				if opt in ("--fcntr"):
					global fc
					fc = arg
				if opt in ("--chbw"):
					global chbw
					chbw = arg
				if opt in ("--fd"):
					global fdf
					fdf = int(arg)
					if fdf <=0:
						fdf = 1
				if opt in ("--td"):
					global tdf
					tdf = int(arg)
					if tdf <=0:
						tdf = 1
				if opt in ("--dm"):
					global dm 
					dm = float(arg)
					global is_dm
					is_dm = True
                                if opt in ("--cliplevel"):
                                        global clip_level
                                        clip_level = float(arg)
                                if opt in ("--zoomtime"):
                                        global zoomtime
                                        zoomtime = int(arg)


			# defining global variable here with the list of input fil files
			global infiles
			infiles = args
	
		except getopt.GetoptError:
			print "Wrong option!"
			usage(prg)
			sys.exit(2)

if __name__ == "__main__":
	parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])

# if spfile is not defined, throw the error
if spfile == "":
	print "You have to specify .singlepulse file!\n"
	sys.exit(2)

dms, sigma = np.loadtxt(spfile, usecols=(0,1), dtype=float, unpack=True, comments='#')
sample, downfact = np.loadtxt(spfile, usecols=(3,4), dtype=long, unpack=True, comments='#')
sample7 = [sample[i] for i in np.arange(len(sample)) if sigma[i] > threshold and downfact[i] < width]
indices = [i for i in np.arange(len(sample)) if sigma[i] > threshold and downfact[i] < width]
counter = 0
for s in sample7:
	print "%d: %d sigma = %.3f   sample = %d   width = %d" % (counter, indices[counter], sigma[indices[counter]], s, downfact[indices[counter]])
	cmd = "filextract.py -w %d --sample %d -n %d --fcntr %s --chbw %s --sigma %f %s" % (size, s, indices[counter], fc, chbw, sigma[indices[counter]], " ".join(infiles))
	os.system(cmd)	
	# getting the name of previous output fil-file
	mask = "*_gp%05d_sigma*.fil" % (indices[counter],)
	gps = glob.glob (mask)	
	cmd = "fildedisp.py --plotselect --dm %f --width %d --fd %d --td %d --gpsigma %f --fcntr %s --chbw %s --cliplevel %f --zoomtime %d " % (is_dm and dm or dms[indices[counter]], downfact[indices[counter]], fdf, tdf, sigma[indices[counter]], fc, chbw, clip_level, zoomtime) 
	os.system(cmd + gps[0])
	counter += 1
