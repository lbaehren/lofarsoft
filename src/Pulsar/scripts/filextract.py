#!/usr/bin/env python
#
# Script to extract GPs from raw Sigproc data
#
# Vlad, Aug 11, 2010 (c)
###########################################################
import numpy as np
import os, sys
import getopt
import sigproc

def usage (prg):
	""" Prints info how to use the script.
Input argument is the name of the script.
Return value is None
	"""
	print "Program %s extracts raw data for GPs" % (prg, )
	print "Usage: %s [options] <*.fil>\n\
               [-s, --sample <GP sample>]\n\
               [-t, --time <GP time in seconds>]\n\
               [-w, --window <window around GP in samples> (default = 512)]\n\
               [-n, --number <GP number to be used to form outfile name>]\n\
               [-f, --fcntr <center frequency of the highest channel (to substitute the header value>]\n\
               [-c, --chbw <channel width (to substitute the header value)>]\n\
               [-h, --help]" % (prg, )

def parsecmdline(prg, argv):
	""" Main script function. Takes two arguments, 
name of the script and list of the command line arguments.
Return value is None
	"""
	if not argv:
		usage(prg)
		sys.exit()
	else:
		try:
			opts, args = getopt.getopt (argv, "ht:s:w:n:f:c:", ["help", "time=", "sample=", "window=", "number=", "fcntr=", "chbw="])
			for opt, arg in opts:
				if opt in ("-h", "--help"):
					usage(prg)
					sys.exit()
				if opt in ("-t", "--time"):
					global gptime
					gptime = float(arg)
				if opt in ("-s", "--sample"):
					global gpsample
					gpsample = long(arg)
				if opt in ("-w", "--window"):
					global window
					window = long(arg)
				if opt in ("-n", "--number"):
					global gpnumber
					gpnumber = long(arg)
                                if opt in ("--fcntr"):
                                        global fc
                                        fc = float(arg)
                                if opt in ("--chbw"):
                                        global chbw
                                        chbw = float(arg)

			# defining global variable here with the list of input fil files
			global infiles
			infiles = args
	
		except getopt.GetoptError:
			print "Wrong option!"
			usage(prg)
			sys.exit(2)

if __name__ == "__main__":
	# default time of GP and sample of GP
	global gptime, gpsample, window, gpnumber, fc, chbw
	gptime = gpsample = -1
	# default value of samples around GP to extract
	window = 512
	# default value for GP number
	gpnumber = 0
	# default value for central frequency (read from header)
	fc = -100
	# default value for channel bandwidth (read from header)
	chbw = 0


	parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])

# if both GP time and sample are not defined
if gptime == -1 and gpsample == -1:
	print "You have to specify either the time or sample of GP!\n"
	sys.exit(2)

# reading the first file in the list 
# to get info about the file with GP
(filhdr, hdrlen) = sigproc.read_header(infiles[0])

# sampling interval
tbin=filhdr['tsamp']
# number of channels
nchans=filhdr['nchans']
# start time
tstart=filhdr['tstart']
# number of bits
nbits=filhdr['nbits']
if nbits == 32:
	dtype=np.float32
elif nbits == 16:
	dtype=np.ushort
elif nbits == 8:
	dtype=np.ubyte
else:
	print "Can't recognise the data type in the header: %d" % (nbits, )
	sys.exit()
# number of bins per file
nbins=(os.path.getsize(infiles[0])-hdrlen)/nchans/(nbits/8)

# if GP sample is not defined we derive it
# if both GP time and sample were defined in the command line
# GP sample has a priority
if gpsample == -1:
	gpsample = long(gptime / tbin)

# file number with GP
gpfileN = int(gpsample / nbins)
# check if file number larger than the size of array
if gpfileN >= len(infiles):
	print "Not enough input files to cut current GP!\n"
	sys.exit(2)

gpfile = infiles[gpfileN]
gpsample_in_file = gpsample - gpfileN * nbins
start = gpsample_in_file - int(window/2.)
#start = gpsample_in_file - int(window/4.)

if start<0:
	print "GP is close to start, window will be 0 -", window, "with GP sample =", gpsample_in_file
	start = 0

if start + window > nbins:
	print "GP is close to end, window will be", nbins, "-", nbins - window, "with GP sample =", gpsample_in_file
	start = nbins - window

# read cut with GP from the corresponding fil-file
infile=open(gpfile, "rb")
newhdr=""
while 1:
	param, val = sigproc.read_hdr_val(infile, stdout=False)

        if param=="tstart":
		val = tstart + ((gpsample_in_file + gpfileN * nbins) * tbin) / 86400.
	if param=="fch1" and fc != -100:
		val=fc
	if param=="foff" and chbw != 0:
		val=chbw

	# Append to the new hdr string
	newhdr += sigproc.addto_hdr(param, val)

	# Break out of the loop if the header is over
	if param=="HEADER_END":
		break

infile.seek(hdrlen+nchans*start*(nbits/8), 0)
x = np.fromfile(infile, dtype=dtype, count=nchans*window)
infile.close()

# writing raw GP data
outfile = gpfile.split("/")[-1].split(".fil")[0] + "_gp" + "%05d.fil" % (gpnumber,)
ofile=open(outfile, "wb")
ofile.write(newhdr)
x.tofile(ofile)
ofile.close()
