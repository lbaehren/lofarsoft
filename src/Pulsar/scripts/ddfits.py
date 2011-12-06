#!/usr/bin/env python
#
# Dedisperse the PSRFITS data from 2bf2fits
#
# Vlad Kondratiev (c) Dec 6, 2011
#
import numpy as np
import pyfits as py
import os, sys, getopt
import math
import optparse as opt
import gc

# dedisperse the data
def getdelay (dm, freq):
        """ dedisperse the data
        """
        deltat = (10000. * dm / 2.41) * (1. / (freq * freq))
        return deltat

# MAIN
if __name__ == "__main__":
        #
        # Parsing the command line options
        #
        usage = "Usage: %prog [options] [-h, --help] <.fits>"
        cmdline = opt.OptionParser(usage)
        cmdline.add_option('-b', '--sample', dest='start_sample', metavar='SAMPLE',
                           help="start sample (default: from the start of the file)", default=-1, type='int')
        cmdline.add_option('-s', '--skip', dest='start_time', metavar='SECONDS',
                           help="start time in seconds. If both start sample and skip time are given, then start sample will be used. Default: %default", default=0, type='float')
        cmdline.add_option('-T', '--time', dest='time_dur', metavar='SECONDS',
                           help="number of seconds to process (default: till the end of the file)", default=-1, type='float')
        cmdline.add_option('-D', '--dm', dest='dm', metavar='DM',
                           help="dispersion measure (default: undef)", default=-1, type='float')
        cmdline.add_option('-v', '--verbose', action="store_true", dest="is_verbose",
                           help="verbose output", default=False)

        group = opt.OptionGroup(cmdline, "Expert Options")
        group.add_option('--itersize', dest='rows_per_iter', metavar='ROWS',
                           help="number of rows to process in one iteration (default: 4 x Nbadrows)", default=-1, type='int')
        group.add_option('--stepsize', dest='rows_per_step', metavar='ROWS',
                           help="number of rows to concatenate at a time (default: itersize/30)", default=-1, type='int')
        cmdline.add_option_group(group)

        # reading cmd options
        (opts,args) = cmdline.parse_args()

        # check if input file is given
        if len(args) == 0:
                cmdline.print_usage()
                sys.exit(0)

	if opts.dm == -1:
		print "Have to set DM for dedispersion using -D option!"
		sys.exit(1)

	infile = args[0]

	# reading the first file in the list 
	# to get info about the file with GP
	hdulist=py.open(infile, 'readonly', memmap=True)
	# number of samples in spectrum
	nsblk=hdulist[1].header['nsblk']
	# number of spectra in the file
	nrows=hdulist[1].header['naxis2']
	# sampling interval
	tbin=hdulist[1].header['tbin']
	# number of bins per file
	nbins=nsblk * nrows
	# total time per file (in seconds)
	durfile=nbins * tbin
	# number of channels
	nchans=hdulist[1].header['nchan']

	if opts.start_sample == -1:
		opts.start_sample = int(opts.start_time / tbin)

	freqs=hdulist[1].data[0]['DAT_FREQ']
	delays = [getdelay(opts.dm, freq) for freq in freqs]
	maxdelay = int((delays[0] - delays[-1])/tbin + 0.5)
        dt = [int((delays[i] - delays[-1])/tbin + 0.5) for i in range(nchans)]
	badrows = int(math.ceil(float(maxdelay)/float(nsblk)))

	startrow = int(opts.start_sample / nsblk)
	if opts.time_dur == -1:
		endrow = nrows-badrows
	else:
		endrow = int((opts.start_sample + int(math.ceil(opts.time_dur/tbin)) + maxdelay) / nsblk) + 1

	print "nrows = %d, nsblk = %d" % (nrows, nsblk)
	print "freqs = %f - %f MHz" % (freqs[0], freqs[-1])
	print "nchans = %d, tbin = %g s, total time = %.3f s" % (nchans, tbin, durfile)
	print "Max delay = %f s = %d samples = %d rows" % (delays[0] - delays[-1], maxdelay, badrows)
	print "startrow = %d, endrow = %d" % (startrow, endrow)
	print "output size = %d samples (%.3f s)" % (nsblk*(endrow-startrow), nsblk*(endrow-startrow)*tbin)

	itersize = 4 * badrows
	niter = int(math.ceil(float(endrow-startrow) / float(itersize - badrows)))
	if niter <= 1:
		itersize = endrow-startrow
		niter = 1
	print "Number of iterations = %d of %d rows each" % (niter, itersize)
	print "%d good rows per iteration" % (itersize-badrows)
	step = int(itersize/30)
	nsteps = int(math.ceil(float(itersize) / float(step)))
	print "Number of steps = %d of %d rows each" % (nsteps, step)
	if opts.rows_per_iter != -1:
		itersize = opts.rows_per_iter
		niter = int(math.ceil(float(endrow-startrow) / float(itersize - badrows)))
		if niter <= 1:
			itersize = endrow-startrow
			niter = 1
		step = int(itersize/30)
		nsteps = int(math.ceil(float(itersize) / float(step)))
		if nsteps <= 1:
			step = itersize
			nsteps = 1
		print
		print "Expert override ->"
		print "   Number of iterations = %d of %d rows each" % (niter, itersize)
		print "   %d good rows per iteration" % (itersize-badrows)
		print "   Number of steps = %d of %d rows each" % (nsteps, step)
	if opts.rows_per_step != -1:
		step = opts.rows_per_step
		nsteps = int(math.ceil(float(itersize) / float(step)))
		if nsteps <= 1:
			step = itersize
			nsteps = 1
		print
		print "Expert override ->"
		print "   Number of steps = %d of %d rows each" % (nsteps, step)

	print
	# creating output file
	print "creating the output file ..."
	# we change the number of rows to 1 just to speed up process of initial creation of the file
	# otherwise, full size will be allocated on disk right away
	tbdata=hdulist[1].data[startrow:startrow+1]
	hdulist[1].data=tbdata
	outfile = infile.split("/")[-1].split(".fits")[0] + ".dd.fits"
	hdulist.writeto(outfile, clobber=True)
	hdulist.close()

	# reading the input file again to get pointer for the data
	rhdu=py.open(infile, 'readonly', memmap=1)
	tbdata=rhdu[1].data[startrow:endrow+badrows]

	# opening output file to update header info and write out dedispersed data further in the loop
	print "updating header info ..."
	hdulist=py.open(outfile, 'update', memmap=1)
	header=hdulist[1].header
	header['naxis2'] = endrow - startrow
	head0=hdulist[0].header
	mjd=head0['stt_imjd']+(head0['stt_smjd']+head0['stt_offs']+startrow*nsblk*tbin)/86400.
	time=(mjd-int(mjd)) * 86400.
	off=time - int(time)
	head0['stt_imjd'] = int(mjd)
	head0['stt_smjd'] = int(time)
	head0['stt_offs'] = off
	hdulist.close()

	for ii in range(niter):
		print "iteration = %d (%d)" % (ii, niter)
		hdulist=py.open(outfile, 'update', memmap=1)

		print " reading, preparing... ",
		sys.stdout.flush()

		data = []
		srow = ii * (itersize - badrows)
		lastrow = srow + itersize > endrow+badrows and endrow+badrows or srow+itersize
		if srow+step>lastrow:
			step=lastrow-srow
		if step > 1:
			print "%d-%d, " % (srow, srow+step),
			sys.stdout.flush()
			data=np.concatenate([np.reshape (tbdata[row]['DATA'], (-1, nchans)).T for row in range(srow, srow+step)], axis=1)
			for jj in np.arange(srow+step, lastrow, step):
				print "%d-%d, " % (jj, jj+step > lastrow and lastrow or jj+step), 
				sys.stdout.flush()
				data=np.append(data, np.concatenate([np.reshape (tbdata[row]['DATA'], (-1, nchans)).T for row in range(jj, jj+step > lastrow and lastrow or jj+step)], axis=1), axis=1)
			print
		else:
			print "%d-%d" % (srow, lastrow)
			data=np.concatenate([np.reshape (tbdata[row]['DATA'], (-1, nchans)).T for row in range(srow, lastrow)], axis=1)

#		print " dedispersing..."
#		data = [np.r_[data[i,dt[i]:],data[i,:dt[i]]] for i in range(nchans)]
#		for ch in range(nchans):
#			data[ch,:] = np.r_[data[ch,dt[ch]:],data[ch,:dt[ch]]]

		print " dedispersing... ",
		if opts.is_verbose: print "% 6.2f%s" % (0.0, "%"),
		sys.stdout.flush()
		for row in np.arange(srow, lastrow-badrows):
			if (row-srow) % 10 == 0 and opts.is_verbose:
				print "\b\b\b\b\b\b\b\b% 6.2f%s" % (100.*(float(row-srow)/float(lastrow-badrows-srow)), "%"),
				sys.stdout.flush()
			offset=(row-srow)*nsblk
			dedisp = np.array([data[ch,offset+dt[ch]:offset+nsblk+dt[ch]] for ch in range(nchans)])
			hdulist[1].data[row]['DATA'] = np.ravel(dedisp.T)
		if opts.is_verbose: print "\b\b\b\b\b\b\b\b% 6.2f%s" % (100.0, "%")
		gc.collect() # calling garbage collection trying to help to free the memory

		print " writing..."
		hdulist.close()
		gc.collect()
