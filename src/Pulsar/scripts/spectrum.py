#!/usr/bin/env python
#
# Simple script to calculate the average spectrum over the whole (or part)
# of observation. Currently takes a while to run because of many of I/O
# when reading *.sub???? files
#
# Vlad, Aug 5, 2010 (c)
############################################################################
import numpy as np
import array as ar
import os, os.path, stat, glob, sys, getopt
import infodata as inf

is_saveonly = False      # if True, script will save the dynamic spectrum in png file
tsamp=0.00131072  #  sampling interval (s)
samples_win = 0 #  size of window to use in seconds (if 0, show the whole file(s))
samples_offset = 0 # offset from the beginning to skip (if 0 , show the whole file(s))
samplesize = 2 # 2 bytes, because we have 16-bit data
fs=10 # fontsize

def usage (prg):
        """ prints the usage info about the current program
        """
        print "Program %s plots the spectrum based on raw subbands data\n" % (prg,)
        print "Usage: %s [-w, --win <value>] [-l, --offset <value>]\n\
			[--saveonly] [-h, --help] <*sub>\n\
	 -w, --win   <value>     - size of the window (in seconds) to show\n\
	 -l, --offset  <value>   - offset from the beginning (in seconds)\n\
	 --saveonly              - only saves png file and exits\n\
         -h, --help              - print this message\n" % (prg,)


def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "hw:l:", ["help", "saveonly", "win=", "offset="])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage (prg)
                                        sys.exit()

                                if opt in ("--saveonly"):
					global is_saveonly
					is_saveonly = True

				if opt in ("-w", "--win"):
					global samples_win
					samples_win = float(arg)

				if opt in ("-l", "--offset"):
					global samples_offset
					samples_offset = float(arg)

                        if not args:
                                print "No subband files!\n"
                                usage (prg)
                                sys.exit(2)
                        else:
                                global subfiles
				subfiles = args
				subfiles.sort()

                except getopt.GetoptError:
                        print "Wrong option!"
                        usage (prg)
                        sys.exit(2)
	
if __name__=="__main__":
        parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])
	if is_saveonly:
		import matplotlib
		matplotlib.use("Agg")
	else:
		import matplotlib

	import matplotlib.pyplot as plt
	import matplotlib.ticker as ticker
	import matplotlib.cm as cm
	
	nfiles=len(subfiles)
	# array of file sizes
	sizes = [os.stat(file)[stat.ST_SIZE] / samplesize for file in subfiles]
	# maximum size
	size=max(sizes)

	# reading inf-file to get info about sampling interval
	inffile = subfiles[0].split(".sub")[0] + ".sub.inf"
        id = inf.infodata(inffile)
        cfreq = id.lofreq
        chanbw = id.chan_width

	# handle offset from the beginning
	if samples_offset > 0:
		samples_offset = int (samples_offset / tsamp)
		if samples_offset > size - 2:
			samples_offset = 0
	# will use only samples_win number of bins (if chosen)
	if samples_win > 0:
		samples_win = int(samples_win / tsamp)	
		if size-samples_offset > samples_win: 
			size = samples_win
			sizes = [samples_win for value in sizes]
		else:
			if samples_offset > 0:
				sizes = [value - samples_offset for value in sizes]

	if is_saveonly:
		pngname = subfiles[0].split(".sub")[0] + ".spectrum.png"

	# array for spectrum values
	spectrum = np.zeros(nfiles, dtype=float)
	freqs= np.zeros(nfiles, dtype=float)
	freqs = [cfreq + i*chanbw for i in np.arange(nfiles)]

	for i in np.arange(0, nfiles, 1):
		print i
		f = open(subfiles[i], "rb")
		data = ar.array('h')  # 'h' - for signed short
		f.seek (samples_offset * samplesize)  # position to the first sample to read
		data.read(f, sizes[i])
		f.close()
		ndata = np.array(data)
		spectrum[i] = np.array(data).sum()

	fig=plt.figure()
	ax = fig.add_subplot(111)
	plt.xlabel("Frequency (MHz)", fontsize=fs)
	plt.ylabel("Power (arb. units)", fontsize=fs)

	ax.plot (freqs, spectrum, color="green")
	if is_saveonly:
		plt.savefig(pngname)
	plt.show()

