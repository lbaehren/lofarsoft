import glob, os, sys, getopt, re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.patches as patches
import matplotlib.path as path


Nbins=7630 # corresponds usually to 10s
histbins=100
is_createreports = False  # True, if we want to create *.rfirep files first
is_percents = False       # True, if to plot histogram with normalized bins
top_level_dir = "."       # top-level directory
is_dir_to_exclude = False # True, if there are dirs to exclude
direxclmask=[]            # mask to exclude directories
dirs = []                 # directories with *.sub??? files. They will either be found from the top-level directory
                          # or directly specified from the command line
is_top_level_dir = False  # True, if we specify input dirs from the command line


def usage (prg):
        """ Create report files and plots the 'bad chan' histogram
        """
        print "Program %s creates the RFI report files and plots summary histogram\n" % (prg,)
        print "Usage: %s [-n, --nbins <value>] [-t, --histbins <value>]\n\
                         [--createreports] [--percents] [--excludedirs <value>] [-h, --help] <top-level directory | input dirs>\n\
         -n, --nbins <value>     - number of samples to average (default: 7630)\n\
         -t, --histbins <value>  - number of histogram bins (default: 100)\n\
         --createreports         - run first subdyn.py for every dataset under the top-level directory\n\
	 --percents              - every bin in the histogram is normalized by the total number\n\
				   of observations in _this_ bin\n\
         --excludedirs <value>   - mask to exclude directories\n\
         -h, --help              - print this message\n" % (prg,)

def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "hn:t:", ["help", "nbins=", "histbins=", "createreports", "percents", "excludedirs="])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage (prg)
                                        sys.exit()

                                if opt in ("-n", "--nbins"):
                                        global Nbins
                                        Nbins = long(arg)

                                if opt in ("-t", "--histbins"):
                                        global histbins
                                        histbins = long(arg)

                                if opt in ("--createreports"):
                                        global is_createreports
                                        is_createreports = True

                                if opt in ("--percents"):
                                        global is_percents
                                        is_percents = True

                                if opt in ("--excludedirs"):
                                        global is_dir_to_exclude
                                        is_dir_to_exclude = True
					global direxclmask
					direxclmask = arg.split(" ")

                        if not args:
                                print "No top-level direcory or input directories!\n"
                                usage (prg)
                                sys.exit(2)
                        else:
				if len(args) == 1:
                                	global top_level_dir
                                	top_level_dir = args[0]
					global is_top_level_dir
					is_top_level_dir = True
				else:
					global dirs
					dirs = args

                except getopt.GetoptError:
                        print "Wrong option!"
                        usage (prg)
                        sys.exit(2)

if __name__=="__main__":
	parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])

	if is_top_level_dir:
		# search for directories RSP? and write them to ".find" file
		dirs = [dir[:-1] for dir in os.popen("find %s -type d -group pulsar -wholename \"*/RSP?*\" -print" % (top_level_dir,)).readlines()]
	currentdir = os.getcwd()

	# exclude directories
	if is_dir_to_exclude:
		dirsexclude = []
		for e in np.arange(len(direxclmask)):
			for d in dirs:
				if re.match(direxclmask[e], d): 
					dirsexclude.append(d)
		for d in dirsexclude: dirs.remove(d)

	# check if there are no directories found
	if len(dirs) == 0:
		print "No RSP? directories found"
		sys.exit(0)

	# running "subdyn.py" script to create a dynamic spectrum and the list of bad channels *.rfirep
	Nsubfiles = {}
	for d in dirs:
		os.chdir(d)
		print d
		rfireps=glob.glob('*.rfirep')
		subfiles=glob.glob('*.sub[0-9]???')
		[subfiles.append(i) for i in glob.glob('*.sub[0-9]??')]
		Nsubfiles[d] = len(subfiles)  # number of subfiles in the directory
		if len(rfireps) > 0:
			print "skipping ..."
		else:
			cmd="python ~/bin/subdyn.py --saveonly -n " + str(Nbins) + " "
			cmd = cmd + ' '.join(subfiles)
			if is_createreports:
				os.system(cmd)

		os.chdir(currentdir)	

	freqs=np.empty(0)
	# reading all *.rfirep files 
	for d in dirs:
		rfireps=glob.glob(d + '/*.rfirep')
		if len(rfireps) > 0:
			rfirep=rfireps[0]
			# checking the number of lines in the file
			# if there is only 1 line (with the comment) than ignoring this file
			fd=open(rfirep)
			number_lines=len(fd.readlines())
			fd.close()
			if number_lines > 1:
				fs=np.loadtxt(rfirep, skiprows=1, usecols=(1,1), dtype=float, unpack=True)[0]
				freqs=np.append(freqs, fs)

	# sorting the freqs and making the histogram
	freqs = np.sort(freqs, kind='mergesoft')
	freqspan = freqs[-1] - freqs[0]
	freqs_unique = np.unique(freqs)
	
	# to calculate the total number of observations using every particular freq channel
	# create a dictionary with a key = freq and value = number of obs with this chan
	Nobsperchan = {}
	for key in freqs_unique: Nobsperchan[key] = 0
	number_of_obs=0
	# reading all *.sub.inf files to get the range of frequencies used
	for d in dirs:
		rfireps=glob.glob(d + '/*.rfirep')
		if len(rfireps) > 0:
			number_of_obs += 1
			sinf = glob.glob(d + '/*.sub.inf')[0]
		        for line in open(sinf):
                		if line.startswith(" Central freq of low channel"):
                        		cfreq = float(line.split("=")[-1].strip())
                		if line.startswith(" Channel bandwidth"):
                        		chanbw = float(line.split("=")[-1].strip())
			for fr in freqs_unique.compress((freqs_unique >= cfreq) & (freqs_unique < cfreq + chanbw * Nsubfiles[d])):
				Nobsperchan[fr] += 1


	diffs = [freqs_unique[n+1] - freqs_unique[n] for n in np.arange(np.size(freqs_unique)-1)]
	freqbin = np.sort(diffs, kind='mergesoft')[0]

	print 

        fig = plt.figure()
        ax = fig.add_subplot(111)

	# tweaking the range of the histogram and the actual number of bins, to 
	# have actual bin width as the multiple of freq channel
	binwidth = float(freqspan)/histbins
	nch = np.ceil((binwidth/freqbin))
	binwidth = nch * freqbin
	updspan = binwidth * histbins
	hleft =  freqs[0] - ((updspan - freqspan) / 2.)
	hright = freqs[-1] + ((updspan - freqspan) / 2.)

	# to plot simple histogram Number vs. Freq. chan
	if not is_percents:
		(ydata, bins, patches) = ax.hist(freqs, histbins, range=(hleft,hright), normed=False, cumulative=False, facecolor='green', alpha=0.75)
		ax.set_xlim(bins[0]-(bins[1]-bins[0])/2., bins[-1]+(bins[1]-bins[0])/2.)
		plt.ylabel("Number")
		for w in np.arange(np.size(ydata)):
			if ydata[w] != 0: print "%.4f\t - %d" % ((bins[w+1]+bins[w])/2.,ydata[w])

	# to plot the histogram of Normalized bins (normalized by the actual number of total observations
	# with the particular frequency channel bin
	else:
		(ydata, bins) = np.histogram(freqs, histbins, normed=False, range=(hleft,hright))
		# now we have to calculate the total number of observations in histogram bin having total number of
		# observations in every freq channel
		for w in np.arange(np.size(ydata)):
			bintotal = 0
			for fr in freqs_unique:
				if (fr >= bins[w]) & (fr < bins[w+1]): 
					bintotal = bintotal + Nobsperchan[fr]
				if (w == np.size(ydata) - 1) & (fr == bins[w+1]):
					bintotal = bintotal + Nobsperchan[fr]
			if bintotal != 0:
				print "%.4f\t - %d [%d]\t %.2f" % ((bins[w+1]+bins[w])/2.,ydata[w], bintotal, (float(ydata[w]) / bintotal) * 100.)
				ydata[w] = (float(ydata[w]) / bintotal) * 100.

        	# Plotting histogram using corrected values for ydata
        	# get the corners of the rectangles for the histogram
		left = np.array(bins[:-1])
		right = np.array(bins[1:])
		bottom = np.zeros(len(left))
		top = bottom + ydata
        	# we need a (numrects x numsides x 2) numpy array for the path helper
        	# function to build a compound path
		XY = np.array([[left,left,right,right], [bottom,top,top,bottom]]).T
        	# get the Path object
		barpath = path.Path.make_compound_path_from_polys(XY)
        	# make a patch out of it
		patch = patches.PathPatch(barpath, facecolor='green', alpha=0.75)
		ax.add_patch(patch)
        	# update the view limits
		ax.set_xlim(left[0]-(bins[1]-bins[0])/2., right[-1]+(bins[1]-bins[0])/2.)
		ax.set_ylim(bottom.min(), top.max())
#		ax.set_ylim(0., 100.)
		plt.ylabel("Percent")

	print
	print "Number of directories = ", len(dirs)
	print "Number of processed directories = %d [number of obs: %d]" % (number_of_obs, number_of_obs/4)

	print "Frequency span = %.4g MHz [%.4g - %.4g]" % (freqspan, freqs[0], freqs[-1])
	print "Minimum frequency separation = %.4g MHz" % (freqbin,)
	print "Number of bins to plot all frequencies separately = %.1f (current: %d)" % (float(freqspan)/freqbin, histbins)
	print "Current histogram bin width = %.4g MHz" % (bins[1] - bins[0],)

	# common part for both types of histograms
	plt.title("Bad frequency channels over %d observations" % (number_of_obs/4,))
	plt.xlabel("Frequency (MHz)")
	plt.grid(True)

	plt.show()
