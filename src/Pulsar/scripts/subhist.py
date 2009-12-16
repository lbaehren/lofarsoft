import numpy as np
import array as ar
import os, stat, sys, getopt

is_saveonly = False  # if True, the script saves only the png file and exits
Nbins = 100 # number of bins in the histogram
xlow=-32768 # lowest sample value (for signed short)
xhigh=32767 # hoghest sample value
samplesize = 2 # 2 bytes, because we have 16-bit data
fs=14 # fontsize

def usage (prg):
	""" prints the usage info about the current program
	"""
	print "Program %s plots the sample histogram of the subband file\n" % (prg,)
	print "Usage: %s [--xmin <value>] [-h, --help] <subband file>\n\
	 --xmin <value> - min value of the histogram range\n\
	 --saveonly     - only saves png file and exits\n\
	 -h, --help     - print this message\n" % (prg,)

def parsecmdline (prg, argv):
	""" parse the command line arguments
	"""
	if not argv:
		usage (prg)
		sys.exit()
	else:
		try:
			opts, args = getopt.getopt (argv, "h", ["help", "xmin=", "saveonly"])
			for opt, arg in opts:
				if opt in ("-h", "--help"):
					usage (prg)
					sys.exit()
				if opt in ("--xmin"):
					global xlow
					xlow = long(arg)

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

	def percent (x, pos=None): return '%1.2f'%(float(x) * 100./size)


	f = open(subfile, "rb")
	data = ar.array('h')  # 'h' - for signed short
	data.read(f, size)
	f.close()

	n = np.array(data)

	fig = plt.figure()
	ax = fig.add_subplot(111)
	plt.xlabel("Sample Value", fontsize=fs)
	plt.ylabel("Fraction (%)", fontsize=fs)

	ax.hist (n, Nbins, range=(xlow, xhigh))
	ax.set_xlim(xmin=xlow, xmax=xhigh)
	ax.yaxis.set_major_formatter(ticker.FuncFormatter(percent))
	ax.xaxis.set_major_locator(ticker.AutoLocator())
	ax.annotate(subfile.split("/")[-1].split(".")[1], xy=(0,0), xytext=(0.8, 0.9), xycoords='axes fraction', fontsize=fs-2)
	for label in ax.get_xticklabels(): label.set_fontsize(fs-2)
	for label in ax.get_yticklabels():
		label.set_fontsize(fs-2)
	#	label.set_rotation(90)

	if is_saveonly: plt.savefig(pngname)
	plt.show()
