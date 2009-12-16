import numpy as np
import array as ar
import os, stat, sys, getopt

is_saveonly = False  # if True, the script saves only png file and exits
tsamp=0.00131072  #  sampling interval (s)
Nbins = 750 # number of bins to average
samplesize = 2 # 2 bytes, because we have 16-bit data
fs=12 # fontsize

def usage (prg):
	""" prints the usage info about the current program
	"""
	print "Program %s plots the time series of the subband file\n" % (prg,)
	print "Usage: %s [-n, --nbins <value>] [-h, --help] <subband file>\n\
	 -n, --nbins <value> - number of samples to average (default: 750)\n\
	 --saveonly          - only saves png file and exits\n\
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
				print "No subband file!\n"
				usage (prg)
				sys.exit(2)
			else:
				global subfile
				subfile = args[0]

		except getopt.GetoptError:
			print "Wrong option!"
			usage (prg)
			sys.exit(2)

if __name__=="__main__":
	parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])
	if is_saveonly:
		import matplotlib
		matplotlib.use('Agg')
		pngname = subfile + ".png"
	else:
		import matplotlib

	import matplotlib.pyplot as plt
	import matplotlib.ticker as ticker

	size=os.stat(subfile)[stat.ST_SIZE] / samplesize

	# reading inf-file to get info about sampling interval
	inffile = subfile.split(".sub")[0] + ".sub.inf"
	for line in open(inffile):
		if line.startswith(" Width of each time series bin"):
			tsamp = float(line.split("=")[-1].strip())

	def printtime (x, pos=None): return '%1.2f'%(float(x) * tsamp * Nbins)


	f = open(subfile, "rb")
	data = ar.array('h')  # 'h' - for signed short
	data.read(f, size)
	f.close()

	n = np.array(data)
	ts = [np.average(n[k*Nbins:(k+1)*Nbins]) for k in np.arange(0, int(size/Nbins), 1)]
	#mean = [np.mean(np.sort(n[k*Nbins:(k+1)*Nbins])[0:Nbins/2]) for k in np.arange(0, int(size/Nbins), 1)]
	#rms = [np.std(np.sort(n[k*Nbins:(k+1)*Nbins])[0:Nbins/2]) for k in np.arange(0, int(size/Nbins), 1)]

	#sig = np.trim_zeros(np.sort(rms), 'bf')[0]
	#av = np.trim_zeros(np.sort(np.abs(mean)), 'bf')[0]

	fig = plt.figure()
	ax = fig.add_subplot(111)
	plt.xlabel("Time (s)", fontsize=fs)
	plt.ylabel("Flux density (arb. units)", fontsize=fs)

	#aa=[ts[k] - av for k in np.arange(0, int(size/Nbins), 1)]
	#bb=[float(ts[k] - av)/sig for k in np.arange(0, int(size/Nbins), 1)]
	#mask=[(np.abs(float(ts[k] - av)/sig) >= 5 and ts[k] or 0) for k in np.arange(0, int(size/Nbins), 1)]

	ax.plot (range(0,int(size/Nbins)), ts, color='blue')
	ax.set_xlim(xmin=0, xmax=int(size/Nbins))
	ax.xaxis.set_major_formatter(ticker.FuncFormatter(printtime))
	ax.yaxis.set_major_locator(ticker.MaxNLocator(4))
	ax.annotate(subfile.split("/")[-1].split(".")[1], xy=(0,0), xytext=(0.8, 0.9), xycoords='axes fraction', fontsize=fs-2)
	for label in ax.get_xticklabels(): label.set_fontsize(fs-2)
	for label in ax.get_yticklabels():
		label.set_fontsize(fs-2)
	#	label.set_rotation(90)

	if is_saveonly: plt.savefig(pngname)
plt.show()
