import numpy as np
import array as ar
import os, stat, glob, sys, getopt

is_saveonly = False   # if True, the script saves only png files and exits
Nx=4  # number of histograms on X-axis
Ny=5  # on Y-axis
tsamp=0.00131072  #  sampling interval (s)
Nbins = 750 # number of bins to average
samplesize = 2 # 2 bytes, because we have 16-bit data
fs=10 # fontsize


def usage (prg):
        """ prints the usage info about the current program
        """
        print "Program %s plots the time series of the subbands file\n" % (prg,)
        print "Usage: %s [-n, --nbins <value>] [-h, --help] <*sub>\n\
         -n, --nbins <value> - number of samples to average (default: 750)\n\
	 --saveonly          - only saves png files and exits\n\
         -h, --help     - print this message\n" % (prg,)

def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "hn:", ["help", "nbins=", "saveonly"])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage (prg)
                                        sys.exit()
                                if opt in ("-n", "--nbins"):
                                        global Nbins
                                        Nbins = long(arg)

				if opt in ("--saveonly"):
					global is_saveonly
					is_saveonly = True

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


def plot_page (index):
        """ plots the page of several histograms for subbands
        """
	cind=nfiles - index*Nx*Ny
	maxtsperpage = cind < Nx*Ny and cind or Nx*Ny

	plt.clf()    # clear the figure

        # loop on every histogram in the page
	for i in np.arange(0, maxtsperpage, 1):
		ind = i+index*Nx*Ny
		def printtime (x, pos=None): return '%1.0f'%(float(x) * tsamp * Nbins)

		if tax[ind] == ind:   # if True, then we don't read file again
			f = open(subfiles[ind], "rb")
			data = ar.array('h')  # 'h' - for signed short
			data.read(f, size)
			f.close()
			n = np.array(data)
        		ts[ind] = [np.average(n[k*Nbins:(k+1)*Nbins]) for k in np.arange(0, int(size/Nbins), 1)]

		tax[ind] = fig.add_subplot(Ny, Nx, i + 1)
		tax[ind].plot (range(0,int(size/Nbins)), ts[ind])
		tax[ind].set_xlim(xmin=0, xmax=int(size/Nbins))
		tax[ind].xaxis.set_major_formatter(ticker.FuncFormatter(printtime))
		tax[ind].annotate(subfiles[ind].split("/")[-1].split(".")[1], xy=(0,0), xytext=(0.65, 0.85), xycoords='axes fraction', fontsize=fs-2)
		if i == 0: 
			tax[ind].annotate("Page: " + str(index), xy=(0,0), xytext=(0.05, 0.95), xycoords='figure fraction', fontsize=fs)
			tax[ind].annotate("Time (s)", xy=(0,0), xytext=(0.45, 0.03), xycoords='figure fraction', fontsize=fs)
			tax[ind].annotate("Flux density (arb. units)", xy=(0,0), xytext=(0.05, 0.38), xycoords='figure fraction', fontsize=fs, rotation=90)
		if i<maxtsperpage-Nx:   # don't print x-labels on top histograms (only in the lowest line)
			tax[ind].xaxis.set_major_formatter(ticker.NullFormatter())
		else:
			tax[ind].xaxis.set_major_locator(ticker.MaxNLocator(4))
			for label in tax[ind].get_xticklabels(): label.set_fontsize(fs-2)   # decrease the fontsize of x-labels
		tax[ind].yaxis.set_major_locator(ticker.MaxNLocator(3))
		for label in tax[ind].get_yticklabels():  # decrease the fontsize and rotate y-labels
			label.set_fontsize(fs-2)
			label.set_rotation(90)
		# plotting plot by plot
		if is_saveonly == False: plt.draw()

	# save page to png-file
	plt.savefig(outnamestem + str(index) + ".png")

	plt.show()

def press(event):
        """ 
        """
        if event.key == 'enter': plot_page ((count + 1 >= npages and npages - 1 or count + 1)%npages)
        if event.key == 'backspace': plot_page ((count - 1 < 0 and 0 or count - 1)%npages)
        if event.key == '>': plot_page ((count + 10 >= npages and npages - 1 or count + 10)%npages)
        if event.key == '<': plot_page ((count - 10 < 0 and 0 or count - 10)%npages)
        if event.key == ' ': plot_page (0)
        if event.key == 'q' or event.key == 'Q': plt.close()
        if event.key == '0' or \
           event.key == '1' or \
           event.key == '2' or \
           event.key == '3' or \
           event.key == '4' or \
           event.key == '5' or \
           event.key == '6' or \
           event.key == '7' or \
           event.key == '8' or \
           event.key == '9':
                step=int(event.key)
                plot_page (int(count/10)*10 + step >= npages and npages - 1 or int(count/10)*10 + step)

def plot_help():
        """ prints the info about keys available for usage
        """
        print
        print "Press following keys"
        print " Enter/Backspace - for the next/previous page"
        print " >/<             - to increase/decrease page number by 10"
        print " 0-9             - to go to corresponding page number within current tens"
        print " Space           - to go to the first page"
        print " q or Q          - quit"
        print


if __name__=="__main__":
        """ the main function....
        """
        parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])
        if is_saveonly:
                import matplotlib
                matplotlib.use('Agg')
        else:
                import matplotlib

        import matplotlib.pyplot as plt
        import matplotlib.ticker as ticker

        nfiles=len(subfiles)

        # filestem for output png files
        outnamestem=subfiles[0].split("/")[-1].split(".sub")[0] + ".subtime"

        # reading inf-file to get info about sampling interval
        inffile = subfiles[0].split(".sub")[0] + ".sub.inf"
        for line in open(inffile):
                if line.startswith(" Width of each time series bin"):
                        tsamp = float(line.split("=")[-1].strip())


        npages=int(np.ceil(float(nfiles)/(Nx * Ny)))
        count=0

        sizes=[os.stat(file)[stat.ST_SIZE] / samplesize for file in subfiles]
        size=max(sizes)

        print "Number of subbands: ", nfiles
        print "Number of pages: ", npages

        tax = [i for i in np.arange(0, nfiles, 1)]
        ts = np.zeros((nfiles, int(size/Nbins)))

        if is_saveonly == False:
                plt.ion() # turning ON interactive plotting
                fig = plt.figure()
                fig.subplots_adjust(hspace=0.001)
                fig.canvas.mpl_connect('key_press_event', press)
                plot_help()
                plot_page(count)
        else:
                fig = plt.figure()
                fig.subplots_adjust(hspace=0.001)
                for pg in np.arange(0, npages, 1): plot_page(pg)
