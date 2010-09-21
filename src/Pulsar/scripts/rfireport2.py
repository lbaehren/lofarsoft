#!/usr/bin/env python
import glob, os, sys, getopt, re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.patches as patches
import matplotlib.path as path

import infodata as inf

Nbins=7630 # corresponds usually to 10s
is_createreports = False  # True, if we want to create *.rfirep files first
is_percents = False       # True, if to plot histogram with normalized bins
is_dir_to_exclude = False # True, if there are dirs to exclude
direxclmask=[]            # mask to exclude directories
inputdirs = []            # directories with *.sub??? files. They will either be found from the top-level directory
                          # or directly specified from the command line
is_check_dropouts = False # True if we want to check channels for drop-outs
freqbin = 0.1953125       # default value of freqbin (size of the subband)
histleft = 10.            # lowest freq for histogram (default)
histright = 240.          # highest freq for histogram (default)
# user defined values of histleft and histright from the command line
userleft = -100
userright = -100

cexec_egrep_string="egrep -v \'\\*\\*\\*\\*\\*\' |egrep -v \'\\-\\-\\-\\-\\-\'"


def usage (prg):
        """ Create report files and plots the 'bad chan' histogram
        """
        print "Program %s creates the RFI report files and plots summary histogram\n" % (prg,)
        print "Usage: %s [--nbins <value>] [--freqbin <value>] [--lba] [--hba] [--flow] [--fhigh]\n\
                         [--createreports] [--percents] [--excludedirs <value>] [--help] <input top-level dirs>\n\
         --nbins <value>         - number of samples to average (default: 7630)\n\
         --freqbin <value>       - size of the histogram bin in MHz (default is the width of subband, ~195 kHz)\n\
         --createreports         - run first subdyn.py for every dataset under the top-level directory\n\
	 --percents              - every bin in the histogram is normalized by the total number\n\
				   of observations in _this_ bin\n\
         --excludedirs <value>   - mask to exclude directories\n\
         --checkdropouts         - checking the channels for drop-outs\n\
         --lba                   - will use only for LBA datasets and sets the histogram range to 10-90 MHz (default is 10-240 MHz)\n\
         --hba                   - only HBA datasets will be considered, histogram range: 110-240 MHz\n\
         --flow                  - sets the lowest freq value for the histogram, does not affect checking for LBA or HBA obs\n\
         --fhigh                 - sets the highest freq for the histogram\n\
         --help                  - print this message\n" % (prg,)

def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "", ["help", "nbins=", "freqbin=", "createreports", "percents", "excludedirs=", "checkdropouts", "lba", "hba", "flow", "fhigh"])
                        for opt, arg in opts:
                                if opt in ("--help"):
                                        usage (prg)
                                        sys.exit()

                                if opt in ("--nbins"):
                                        global Nbins
                                        Nbins = long(arg)

                                if opt in ("--freqbin"):
                                        global freqbin
                                        freqbin = float(arg)

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

                                if opt in ("--checkdropouts"):
                                        global is_check_dropouts
                                        is_check_dropouts = True

                                if opt in ("--lba"):
					global histright
					histright = 90.

                                if opt in ("--hba"):
					global histleft
					histleft = 110.

                                if opt in ("--flow"):
                                        global userleft
                                        userleft = float(arg)

                                if opt in ("--fhigh"):
                                        global userright
                                        userright = float(arg)

                        if not args:
                                print "No top-level directory!\n"
                                usage (prg)
				sys.exit(2)
                        else:
				global inputdirs
				inputdirs = args

                except getopt.GetoptError:
                        print "Wrong option!"
                        usage (prg)
                        sys.exit(2)

if __name__=="__main__":
	parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])
	if histleft > histright:
		print "You can't set both --lba and --hba simultaneously!"
		sys.exit(0)
	if userleft != -100:
		if userleft > histright:
			print "User setting of flow is bad!"
			sys.exit(0)
	if userright != -100:
		if userright < histleft:
			print "User setting of fhigh is bad!"
			sys.exit(0)
	if userleft != -100 and userright != -100:
		if userleft > userright:
			print "User settings of flow and fhigh are bad!"
			sys.exit(0)

	# search for incoherentstokes directories
	dirs = []
	for d in inputdirs:
		cmd="find %s -type d -name 'incoherentstokes' -print 2>/dev/null | grep -v denied | grep -v such" % (d,)
		dirs = np.append(dirs, [dir[:-1] for dir in os.popen(cmd).readlines()])

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
		print "No incoherentstokes directories found"
		sys.exit(0)

	freqs=np.empty(0)
	direxists=0
	# reading all *.rfirep files 
	workdirs = []
	for d in dirs:
		rfireps=glob.glob(d + '/*.rfirep')
		if len(rfireps) > 0:
			rfirep=rfireps[0]
			# checking if there is an inf-file as well
			sinf = glob.glob(d + '/RSPA/*.sub.inf')
			if len(sinf) > 0:
	        		id = inf.infodata(sinf[0])
        			lofreq = id.lofreq - id.chan_width/2.
				if lofreq >= histleft and lofreq <= histright:
					# checking the number of lines in the file
					# if there is only 1 line (with the comment) than ignoring this file
					fd=open(rfirep)
					number_lines=len(fd.readlines())
					fd.close()

					workdirs = np.append(workdirs, d)

					if number_lines > 1:
						fs=np.loadtxt(rfirep, comments='#', usecols=(1,1), dtype=float, unpack=True)[0]
						if np.size(fs) == 1:
							fs = np.array([fs])
						freqs=np.append(freqs, fs)
						(sbs)=np.loadtxt(rfirep, comments='#', usecols=(0,0), dtype=int, unpack=True)[0]
						if np.size(sbs) == 1:
							sbs = np.array([sbs])

						# checking subbands for drop-outs
						if is_check_dropouts == True:
							flag=0
							for sb in np.arange(np.size(sbs)):
								sbfile=glob.glob("%s/RSPA/*.sub%04d" % (d, int(sbs[sb])))
								if len(sbfile)>0:
									if flag == 0:
										flag=1
										direxists += 1
									sbfile=sbfile[0]
									cmd1="dt-ss -i %s | grep max | grep -v xmax | awk \'{print $3}\' -" % (sbfile,)
									dtmax=os.popen(cmd1).readlines()
									cmd2="dt-ss -i %s | grep min | grep -v xmin | awk \'{print $3}\' -" % (sbfile,)
									dtmin=os.popen(cmd2).readlines()
									if len(dtmax) > 0 and len(dtmin) > 0:
										dtmax=int(os.popen(cmd1).readlines()[0][:-1])
										dtmin=int(os.popen(cmd2).readlines()[0][:-1])
										if dtmax == 0 and dtmin == 0:
											print "dtmax = %d   dir: %s  subN: %d  file: %s" % (dtmax, d, sbs[sb], sbfile)
									else:
										print d, sbs[sb], "problem with dt!"
		else:   # will be reading separate *.rfirep files
			rfireps=glob.glob(d + '/RSP*/*.rfirep')
			if len(rfireps) > 0:
				# checking if there is an inf-file as well
				sinf = glob.glob(d + '/RSPA/*.sub.inf')
				if len(sinf) > 0:
	        			id = inf.infodata(sinf[0])
        				lofreq = id.lofreq - id.chan_width/2.
					if lofreq >= histleft and lofreq <= histright:
						workdirs = np.append(workdirs, d)

						for rfirep in rfireps:
							# checking the number of lines in the file
							# if there is only 1 line (with the comment) than ignoring this file
							fd=open(rfirep)
							number_lines=len(fd.readlines())
							fd.close()

							if number_lines > 1:
								fs=np.loadtxt(rfirep, comments='#', usecols=(1,1), dtype=float, unpack=True)[0]
								if np.size(fs) == 1:
									fs = np.array([fs])
								freqs=np.append(freqs, fs)


	# sorting the freqs and making the histogram
	freqs = np.sort(freqs, kind='mergesoft')
	freqs_unique = np.unique(freqs)

	# to calculate the total number of observations using every particular freq channel
	# create a dictionary with a key = freq and value = number of obs with this chan
	Nobsperchan = {}
	for key in freqs_unique: Nobsperchan[key] = 0
	number_of_obs=np.size(workdirs)
	# reading all *.sub.inf files to get the range of frequencies used
	for d in workdirs:
		sinf = glob.glob(d + '/RSPA/*.sub.inf')
	        id = inf.infodata(sinf[0])
        	cfreq = id.lofreq
        	chanbw = id.chan_width
        	totbw = id.BW
		for fr in freqs_unique.compress((freqs_unique >= cfreq-chanbw/2.) & (freqs_unique <= cfreq + totbw - chanbw/2.)):
			Nobsperchan[fr] += 1


	# tweaking the range of the histogram and the actual number of bins, to 
	# have actual bin width as the multiple of freq channel
	if userleft != -100:
		histleft = userleft
	if userright != -100:
		histright = userright
	freqspan = histright - histleft
	histbins = int((histright - histleft) / freqbin) + 1
	freqspan = histbins * freqbin
	histright = histleft + freqspan

	print 

        fig = plt.figure()
        ax = fig.add_subplot(111)

	# to plot simple histogram Number vs. Freq. chan
	if not is_percents:
		(ydata, bins, patches) = ax.hist(freqs, histbins, range=(histleft,histright), normed=False, cumulative=False, facecolor='green', alpha=0.75)
		ax.set_xlim(bins[0]-(bins[1]-bins[0])/2., bins[-1]+(bins[1]-bins[0])/2.)
		plt.ylabel("Number")
#		for w in np.arange(np.size(ydata)):
#			if ydata[w] != 0: 
#				print "%.4f\t - %d" % ((bins[w+1]+bins[w])/2.,ydata[w])

	# to plot the histogram of Normalized bins (normalized by the actual number of total observations
	# with the particular frequency channel bin
	else:
		(ydata, bins) = np.histogram(freqs, histbins, normed=False, range=(histleft,histright))
		# now we have to calculate the total number of observations in histogram bin having total number of
		# observations in every freq channel
		for w in np.arange(np.size(ydata)):
			bintotal = 0
			for fr in freqs_unique:
				if (fr >= bins[w]) & (fr < bins[w+1]): 
					bintotal = bintotal + Nobsperchan[fr]
				if (w == np.size(ydata) - 1) and (fr == bins[w+1]):
					bintotal = bintotal + Nobsperchan[fr]
			if bintotal != 0:
				ydata[w] = (float(ydata[w]) / bintotal) * 100.
#				print "%.4f\t - %d [%d]\t %.2f" % ((bins[w+1]+bins[w])/2.,ydata[w], bintotal, (float(ydata[w]) / bintotal) * 100.)
#				ydata[w] = (float(ydata[w]) / bintotal) * 100.

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
		ax.xaxis.set_minor_locator(ticker.MultipleLocator(10))
		ax.yaxis.set_minor_locator(ticker.MaxNLocator(5))
		plt.ylabel("Fraction (%)")
#		plt.ylabel("Percent")

	if is_check_dropouts == True:
		print "Number of dirs with raw subfiles that exists when checking for dropouts: %d" % (direxists,)
		print
	print "Total number of directories = ", len(dirs)
	print "Number of good directories (obs) = ", np.size(workdirs)

	print "Frequency span = %.4g MHz [%.4g - %.4g]" % (freqspan, histleft, histright)
	print "Minimum frequency separation = %.4g MHz" % (freqbin,)
	print "Current histogram bin width = %.4g MHz" % (bins[1] - bins[0],)

	# common part for both types of histograms
#	plt.title("Bad frequency channels over %d observations" % (number_of_obs,))
	plt.title("RFI summary after %d observations" % (number_of_obs,))
	plt.xlabel("Frequency (MHz)")
	plt.grid(True)

	plt.show()
