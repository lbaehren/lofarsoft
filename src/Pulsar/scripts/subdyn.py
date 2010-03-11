import numpy as np
import array as ar
import os, os.path, stat, glob, sys, getopt

is_saveonly = False      # if True, script will save the dynamic spectrum in png file
is_excludeonly = False   # if True, only completely bad subbands will be excluded
threshold = 6 # threhold in sigmas to clip RFIs
rfilimit = 30  # (in percents) if more, whole subband will be excluded
tsamp=0.00131072  #  sampling interval (s)
samples2show = 0 #  size of window to show in seconds (if 0, show the whole file(s))
samples_offset = 0 # offset from the beginning to skip (if 0 , show the whole file(s))
Nbins = 750 # number of bins to average
histbins = 100 # number of bins in the histogram
xlow=-32768 # lowest sample value (for signed short)
xhigh=32767 # hoghest sample value
samplesize = 2 # 2 bytes, because we have 16-bit data
fs=10 # fontsize

def usage (prg):
        """ prints the usage info about the current program
        """
        print "Program %s plots the dynamic spectrum based on raw subbands data\n" % (prg,)
        print "Usage: %s [-n, --nbins <value>] [-t, --threshold <value>]\n\
			[--rfilimit <value>] [--excludeonly] [-h, --help] <*sub>\n\
         -n, --nbins <value>     - number of samples to average (default: 750)\n\
	 -w, --win   <value>     - size of the window (in seconds) to show\n\
	 -l, --offset  <value>   - offset from the beginning (in seconds)\n\
         -t, --threshold <value> - threshold (in sigma) to clip RFI (default: 6)\n\
	 --excludeonly           - only exclude completely junk subbands\n\
	 --rfilimit <value>      - percent of RFI per subband allowed not to exclude whole subband (default: 30)\n\
	 --saveonly              - only saves png file and exits\n\
         -h, --help     - print this message\n" % (prg,)


def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "hn:t:w:l:", ["help", "nbins=", "threshold=", "rfilimit=", "excludeonly", "saveonly", "win=", "offset="])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage (prg)
                                        sys.exit()

                                if opt in ("-n", "--nbins"):
                                        global Nbins
                                        Nbins = long(arg)

                                if opt in ("-t", "--threshold"):
                                        global threshold
                                        threshold = float(arg)

                                if opt in ("--rfilimit"):
                                        global rfilimit
                                        rfilimit = float(arg)

                                if opt in ("--excludeonly"):
                                        global is_excludeonly
					is_excludeonly = True

                                if opt in ("--saveonly"):
					global is_saveonly
					is_saveonly = True

				if opt in ("-w", "--win"):
					global samples2show
					samples2show = float(arg)

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
	

def setup_plot(x, title, colormap):
	""" initializing dynamic spectrum plot
	"""
	global cax, cbar
	plt.clf()
	ax = fig.add_subplot (111)
	cax = ax.imshow(x, interpolation='nearest', aspect='auto', origin='lower', cmap=colormap)
	cbar = fig.colorbar (cax, orientation='horizontal', spacing='uniform', pad=0.1)
        def printsub (x, pos=None): return '%d' % (subband_offset + x)
	ax.yaxis.set_major_formatter(ticker.FuncFormatter(printsub))
	def printtime (x, pos=None): return '%1.0f'%(float(samples_offset)*tsamp + float(x) * tsamp * Nbins)
	ax.xaxis.set_major_formatter(ticker.FuncFormatter(printtime))
	for label in ax.get_xticklabels(): label.set_fontsize(fs)
	for label in ax.get_yticklabels(): label.set_fontsize(fs)
	plt.xlabel ("Time (s)", fontsize=fs)
	plt.ylabel ("Subband", fontsize=fs)
	fig.suptitle (title, fontsize=fs, y=0.94)
	plt.draw()

def plot_update(x):
	""" updating dynamic spectrum
	"""
	cax.set_data(x)
	cbar.set_array(x)
	cbar.autoscale()
	cax.changed()
	plt.draw()

def press(event):
        """ event handler for pressed keys in the main window
        """
        if event.key == 'm' or event.key == 'M': setup_plot (mask, "Clipping mask", cm.Reds)
        if event.key == 'c' or event.key == 'C': setup_plot (clipped, "Cleaned dynamic spectrum", cm.jet)
        if event.key == 'd' or event.key == 'D': setup_plot (spectrum, "Original dynamic spectrum", cm.jet)
	if event.key == ' ':
		subband_to_exclude = int(event.ydata + 0.5)
		badbands.append(subband_to_exclude)
		print "subband %d will be excluded" % (subband_to_exclude + subband_offset)
		clipped[subband_to_exclude] = np.zeros(int(size/Nbins))
	if event.key == 'enter': 
		print "Clipping RFI from subbands files ...", 
		sys.stdout.flush()
		clipsubband()
		print "done. Original files moved to 'backup' directory"
	if event.key == 'q' or event.key == 'Q': plt.close()
	if event.key == '?': plot_help()
	if event.key == 'b' or event.key == 'B':
		global selected_subband
		selected_subband = int(event.ydata + 0.5)
		plot_help_subband()
		setup_plottime(spectrum, selected_subband, "Time series with mask ovelapped", overlap=True)
	plt.show()

def plot_help():
        """ prints the info about keys available for usage
        """
        print
        print "Press following keys"
        print " m or M - to plot the clipping mask"
        print " d or D - to plot the dynamic spectrum"
	print " c or C - to plot the clipped dynamic spectrum"
	print " b or B - to plot time series of the selected subband in a separate window"
	print " enter  - clip RFI from subbands files"
	print " space  - select subband to be excluded"
	print "   ?    - print this help"
        print " q or Q - quit"
        print

def clipsubband():
	""" move subband files to backup directory and create new clipped subbands
	"""
	path="/".join(subfiles[0].split("/")[0:-1])
	if path == "": path = "./.orig"
	else: path += "/" + ".orig"
	if os.path.exists(path):  # change the name of backup directory to .origN (if .orig exists)
		origpath = path
		origcounter = 0
		while os.path.exists(path):
			origcounter += 1
			path = origpath + str(origcounter)
	cmd = "mkdir -p " + path
	os.system (cmd)
	# excluding first very bad subbands
	for s in np.unique(badbands):
		# move original subband file to backup (.orig) directory
		cmd = "cp " + subfiles[s] + " " + path
		os.system (cmd)
		# create a zero-sized subband file
		cmd = "touch " + subfiles[s]
		os.system (cmd)

	if is_excludeonly == False: # correcting other subbands for RFI
		indarr = np.arange (0, nfiles, 1)
		for i in np.unique(badbands): indarr.remove(i)
		for i in indarr:
			f = open(subfiles[i], "rb")
			data = ar.array('h')  # 'h' - for signed short
			data.read(f, sizes[i])
			f.close()
			cmd = "cp " + subfiles[i] + " " + path
			os.system (cmd)
			ndata = np.array(data)
			# clipping
			for elem in clipindices[i]: ndata[elem*Nbins:(elem+1)*Nbins] = 0
			# writing the new subband file
			f = open(subfiles[i], "wb")
			out = ar.array('h')
			out.fromlist(ndata.tolist())
			out.tofile(f)
			f.close()

def press_in_band(event):
        """ event handler for pressed keys in the subband window
        """
        if event.key == 'q' or event.key == 'Q': plt.close()
	if event.key == '?': plot_help_subband()
	if event.key == 'm' or event.key == 'M': plottime(mask, selected_subband, "Clipping mask", clr="red", overlap=False)
	if event.key == 'c' or event.key == 'C': plottime(clipped, selected_subband, "Flagged time series", overlap=False)
	if event.key == 'b' or event.key == 'B': plottime(spectrum, selected_subband, "Time series with mask ovelapped", overlap=True)
	if event.key == 't' or event.key == 'T': plottime(spectrum, selected_subband, "Original time series", overlap=False)
	if event.key == 'h' or event.key == 'H': plothist(spectrum, selected_subband, "Samples histogram")

def plot_help_subband():
        """ prints the info about keys available for usage in a subband window
        """
        print
        print "Following keys are available in the separate subband window:"
        print " m or M - to plot the clipping mask"
	print " c or C - to plot flagged time series"
	print " b or B - to plot original time series with the mask overlapped"
	print " t or T - to plot original time series"
	print " h or H - to plot samples histogram"
	print "   ?    - print this help"
        print " q or Q - quit"
        print

def setup_plottime(series, selband, title, clr="blue", overlap=False):
        """ setup the separate window with the time series for selected subband
        """
	global figt
        figt = plt.figure()
        figt.canvas.mpl_connect('key_press_event', press_in_band)
	plottime (series, selband, title, clr, overlap)

def plottime(series, selband, title, clr="blue", overlap=False):
	""" plots the separate window with the time series for selected subband
	"""
	plt.clf()
	axt = figt.add_subplot(111)
	def printtime (x, pos=None): return '%1.0f'%(float(samples_offset)*tsamp + float(x) * tsamp * Nbins)
	plt.xlabel("Time (s)", fontsize=fs)
	plt.ylabel("Flux density (arb. units)", fontsize=fs)

	axt.plot (range(0,int(size/Nbins)), series[selband], color=clr)
	if overlap: 
		axt.plot (range(0,int(size/Nbins)), mask[selband], color='red')
	axt.set_xlim(xmin=0, xmax=int(size/Nbins))
	axt.xaxis.set_major_formatter(ticker.FuncFormatter(printtime))
	axt.yaxis.set_major_locator(ticker.MaxNLocator(4))
	axt.annotate(subfiles[selband].split("/")[-1].split(".")[1] + ",   " + title, xy=(0,0), xytext=(0.0, 1.02), xycoords='axes fraction', fontsize=fs)
	for label in axt.get_xticklabels(): label.set_fontsize(fs)
	for label in axt.get_yticklabels():
        	label.set_fontsize(fs)

	plt.show()

def plothist (series, selband, title):
	""" plots the histogram of the chosen subband
	"""
	plt.clf()
	axt = figt.add_subplot(111)
	def percent (x, pos=None): return '%1.2f'%(float(x) * 100./int(size/Nbins))
	plt.xlabel("Sample Value", fontsize=fs)
	plt.ylabel("Fraction (%)", fontsize=fs)

	axt.hist (series[selband], histbins, range=(xlow, xhigh))
	axt.set_xlim(xmin=xlow, xmax=xhigh)
	axt.yaxis.set_major_formatter(ticker.FuncFormatter(percent))
	axt.xaxis.set_major_locator(ticker.AutoLocator())
	axt.annotate(subfiles[selband].split("/")[-1].split(".")[1] + ",   " + title, xy=(0,0), xytext=(0.0, 1.02), xycoords='axes fraction', fontsize=fs)
	for label in axt.get_xticklabels(): label.set_fontsize(fs)
	for label in axt.get_yticklabels():
	        label.set_fontsize(fs)

	plt.show()


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
	for line in open(inffile):
		if line.startswith(" Width of each time series bin"):
			tsamp = float(line.split("=")[-1].strip())
		if line.startswith(" Central freq of low channel"):
			cfreq = float(line.split("=")[-1].strip())
		if line.startswith(" Channel bandwidth"):
			chanbw = float(line.split("=")[-1].strip())

	# handle offset from the beginning
	if samples_offset > 0:
		samples_offset = int (samples_offset / tsamp)
		if samples_offset > size - 2:
			samples_offset = 0
	# will show only samples2show number of bins (if chosen)
	if samples2show > 0:
		samples2show = int(samples2show / tsamp)	
		if size-samples_offset > samples2show: 
			size = samples2show
			sizes = [samples2show for value in sizes]
		else:
			if samples_offset > 0:
				sizes = [value - samples_offset for value in sizes]

	# first subband number
	# i am reading [1] file (and then decrease value by 1) rather than just reading [0] file
	# to avoid problem with a need having first file called *.sub000
	subband_offset = int(subfiles[1].split(".sub")[-1])-1

	if is_saveonly:
		pngname = subfiles[0].split(".sub")[0] + ".sub" + str(subband_offset) + "-" + str(subband_offset+nfiles-1) + ".png"

	# forming the array for having the dynamic spectrum
	spectrum=np.zeros((nfiles, int(size/Nbins)))

	# forming a mask file with samples to reject
	mask=np.zeros((nfiles, int(size/Nbins)))
	clipped=np.zeros((nfiles, int(size/Nbins)))  # clipped data (only for plotting)
	mean=np.zeros((nfiles, int(size/Nbins)))     # 2D array of means
	rms=np.zeros((nfiles, int(size/Nbins)))      # 2D array of rms's
	levels=np.zeros((nfiles, int(size/Nbins)))   # 2D array of levels (samples in sigma)

	# dictionary with indices of RFI'ish samples to be clipped
	clipindices = {}  # for not _very_ bad subbands
	badbands = []     # list of completely bad subbands

	if is_saveonly == False:
		plt.ion() # interactive plotting
		fig=plt.figure()
		fig.canvas.mpl_connect('key_press_event', press)
		setup_plot(spectrum, "Original dynamic spectrum", cm.jet)
		plot_help()

	for i in np.arange(0, nfiles, 1):
		f = open(subfiles[i], "rb")
		data = ar.array('h')  # 'h' - for signed short
		f.seek (samples_offset * samplesize)  # position to the first sample to read
		data.read(f, sizes[i])
		f.close()
		ndata = np.array(data)
		spectrum[i] = [np.average(ndata[k*Nbins:(k+1)*Nbins]) for k in np.arange(0, int(size/Nbins), 1)]
		# calculate mean and rms in the windows of Nbins size
		# to exclude outliers we sort the values in Nbins interval first and then use only first half of it
		mean[i] = [np.mean(np.sort(ndata[k*Nbins:(k+1)*Nbins])[0:Nbins/2]) for k in np.arange(0, int(size/Nbins), 1)]
		rms[i] = [np.std(np.sort(ndata[k*Nbins:(k+1)*Nbins])[0:Nbins/2]) for k in np.arange(0, int(size/Nbins), 1)]

		# check if all values in rms are zeros. If so, then exclude thsi subband
		if np.size(np.trim_zeros(np.sort(rms[i]), 'bf')) != 0:
			sig = np.trim_zeros(np.sort(rms[i]), 'bf')[0]
			# I should remove np.abs at some point, because it's not really correct. I am using it here
			# to avoid really huge negative spikes
			av = np.trim_zeros(np.sort(np.abs(mean[i])), 'bf')[0]

			levels[i] = [np.abs((float(k - av))/sig) for k in spectrum[i]]
			mask[i] = [(levels[i][k] > threshold and spectrum[i][k] or 0) for k in np.arange(0, int(size/Nbins), 1)]
			clipped[i] = [(spectrum[i][k] - mask[i][k]) for k in np.arange(0, int(size/Nbins), 1)]

			# are there many zeros?
			rfi_fraction = (float(len(clipped[i]) - len(np.trim_zeros(clipped[i])))/len(clipped[i]))*100.
		else:
			rfi_fraction = 100.

		if rfi_fraction >= rfilimit:  # bad subband  
			clipped[i] = np.zeros(int(size/Nbins))
			badbands.append(i)
			print "subband %d will be excluded (rfi fraction = %.2f%%)" % (i+subband_offset, rfi_fraction)
		else:
			clipindices[i] = np.where(levels[i] > threshold)[0]

		if is_saveonly == False: plot_update(spectrum)
		
	badchanfreqs = [cfreq + float(sb) * chanbw for sb in badbands]
	rfirepname = subfiles[0].split(".sub")[0] + ".sub" + str(subband_offset) + "-" + str(subband_offset+nfiles-1) + ".rfirep"
	np.savetxt("." + rfirepname, np.transpose((badbands, badchanfreqs)), fmt="%d\t\t%.6g")
	rfirep = open (rfirepname, 'w')	
	rfirep.write("# Subband	Freq (MHz)\n")
	rfirep.close()
	os.system("cat " + "." + rfirepname + " >> " + rfirepname)
	os.system("rm -f " + "." + rfirepname)

	if is_saveonly:
		fig=plt.figure()
		setup_plot(spectrum, "Original dynamic spectrum", cm.jet)
		plt.savefig(pngname)

	plt.show()
