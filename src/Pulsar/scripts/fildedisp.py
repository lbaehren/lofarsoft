#!/usr/bin/env python
#
# Script to read Sigproc data with GP there, dedisperse it
# and produce a diagnostic plot
#
# Vlad & Anya, Aug 15, 2010 (c)
############################################################
import numpy as np
import pyfits as py
import os, sys
import getopt
import sigproc

# - Default values of global parameters, can be changed in command line options
fs=10 # fontsize for plotting
dm=56.8  # default value of DM
# if True, then creates .png file
is_saveonly = False
# if True then opens interactive Xwindow
is_plotselect = False
# Central freq and channel bandwidth (default is to read them from header)
fc=-100
chbw=0
# Time and frequency decimation factors
tdf=1
fdf=1
# Value of GP sigma to plot in the figure
gpsigma=-10
# All samples stronger than clip_level will be clipped (substituted by zero)
clip_level=1.e9
# Offset in samples for undedispersed and dedispersed spectra 
# If set not to zero, then samples before offset will be wrapped and plotted in the end of array
offset=0
# Set the zoom region (around max value) for dedispersed spectrum
zoomtime=-1   # number of non-decimated samples. If == -1, show full array
# Width of GP (the spectrum will be calculated as average in this width around GP)
gpwidth=1
# colormap scaling for 2D plots for imshow (default = SQRT)
plot_scaling='SQRT'




def usage (prg):
        """ Prints info how to use the script.
Input argument is the name of the script.
Return value is None
        """
        print "Program %s creates diagnostic plot for selected fil-file with pulse" % (prg, )
        print "Usage: %s [options] <fil-file>\n\
               [--saveonly] - saves plots to .png rather than plots the first one in Xwindow\n\
               [--plotselect] - opens interactive Xwindow, so by pressing the 'S' one can save the plot\n\
               [--dm <value of DM for dedispersion, default = 56.8]\n\
               [--fcntr <value of central freq in MHz, if header value is wrong>]\n\
               [--chbw <value of channel bandwidth in MHz, if header value is wrong>]\n\
               [--td <time decimation factor>]\n\
               [--fd <frequency decimation factor>]\n\
               [--gpsigma <sigma of GP to show in the plot>]\n\
               [--cliplevel <all values larger than this (in sigmas) will be set to zero, default=1.e9>\n\
               [--offset <offset for 2D spectra in samples>]\n\
               [--zoomtime <zoom region in samples for dedispersed spectrum in samples>]\n\
               [--width <pulse width in samples, spectrum will be calculated as average in this width, default=1>]\n\
               [--scaling <colormap scaling for 2D plots, default=SQRT. Other possible values: LINEAR, LOG, EXP, SQUARE>]\n\
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
                        opts, args = getopt.getopt (argv, "h", ["help", "saveonly", "dm=", "fcntr=", "chbw=", "td=", "fd=", "gpsigma=", "cliplevel=", "offset=", "zoomtime=", "width=", "scaling=", "plotselect"])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage(prg)
                                        sys.exit()
                                if opt in ("--saveonly"):
                                        global is_saveonly
                                        is_saveonly = True
                                if opt in ("--dm"):
                                        global dm
                                        dm = float(arg)
                                if opt in ("--fcntr"):
                                        global fc
                                        fc = float(arg)
                                if opt in ("--chbw"):
                                        global chbw
                                        chbw = float(arg)
                                if opt in ("--gpsigma"):
                                        global gpsigma
                                        gpsigma = float(arg)
                                if opt in ("--td"):
                                        global tdf
                                        tdf = int(arg)
					if tdf <= 0:
						tdf=1
                                if opt in ("--fd"):
                                        global fdf
                                        fdf = int(arg)
					if fdf <= 0:
						fdf=1
                                if opt in ("--cliplevel"):
                                        global clip_level
                                        clip_level = float(arg)
                                if opt in ("--offset"):
                                        global offset
                                        offset = int(arg)
                                if opt in ("--zoomtime"):
                                        global zoomtime
                                        zoomtime = int(arg)
                                if opt in ("--width"):
                                        global gpwidth
                                        gpwidth = int(arg)
                                if opt in ("--scaling"):
                                        global plot_scaling
                                        plot_scaling = arg
                                if opt in ("--plotselect"):
                                        global is_plotselect
                                        is_plotselect = True

                        # defining global variable here with the list of input fil files
                        global infiles
                        infiles = args

                except getopt.GetoptError:
                        print "Wrong option!"
                        usage(prg)
                        sys.exit(2)


def getdelay (dm, freq):
	"""
	"""
	deltat = (10000. * dm / 2.41) * (1. / (freq * freq))
	return deltat

def plot_help():
        """ prints the info about keys available for usage
        """
        print
        print "Press following keys"
        print " s/S       - to save the plot to png file and quit"
        print " space/q/Q - quit without saving, corresponding fil-file will be removed"
        print "   ?       - print this help"
        print

def press(event):
        """ event handler for pressed keys
        """
        if event.key == 's' or event.key == 'S': 
		plt.savefig(pngname)
		print "saving to '%s' ..." % (pngname,)	
		plt.close()
        if event.key == 'q' or event.key == 'Q' or event.key == ' ': 
		os.system("rm -f %s" % (file,))
		print "removing '%s' ..." % (file,)
		plt.close()
        if event.key == '?': plot_help()


################################################################################################################ 
if __name__ == "__main__":
        parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])

	global file
	file = infiles[0]
	global pngname
	pngname = file.split("/")[-1].split(".fil")[0] + ".png"
 
	if is_saveonly:
		import matplotlib
		matplotlib.use('Agg')
	else:
		import matplotlib

import matplotlib.pyplot as plt
import matplotlib.pylab as plb
import matplotlib.cm as cm
from matplotlib.ticker import *
from kapteyn.mplutil import VariableColormap

(filhdr, hdrlen) = sigproc.read_header(file)

# sampling interval
tbin=filhdr['tsamp']
# number of channels
nchans=filhdr['nchans']
# start time
gpmjd=filhdr['tstart']
# center freq of the first channel
if fc == -100:
	fcntr=filhdr['fch1']
else:
	fcntr=fc
# channel width
# should be negative to refer that fch1 - is the highest channel
if chbw == 0:
	foff=filhdr['foff']
else:
	foff=chbw
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
# number of bins
size=(os.path.getsize(file)-hdrlen)/nchans/(nbits/8)
	
# frequency array
freqs = [fcntr + i*foff for i in np.arange(nchans)]

infile=open(file, "rb")
infile.seek(hdrlen, 0)
data = np.fromfile(infile, dtype=dtype, count=nchans*size)
data.shape = (size,nchans)
if offset != 0:
	data[:,:] = np.r_[data[offset:,:],data[:offset,:]]
infile.close()

fdsize=int(nchans/fdf)
tdsize=int(size/tdf)

delays = [getdelay(dm, freq) for freq in freqs]
dedisp = np.zeros((size,nchans), dtype=np.float32)
# this array holds the time series (sum of all channels with proper delays)
ts = np.zeros(size, dtype=np.float32)
spectrum = np.zeros(nchans, dtype=np.float32)
noisespectrum = np.zeros(nchans, dtype=np.float32)
smoothspectrum = np.zeros(fdsize, dtype=np.float32)

sigmas=data.std(axis=0)
means=data.mean(axis=0)

for i in range(nchans):
	dt = int((delays[i] - delays[0])/tbin + 0.5)
	if dt>=size:
		print "Warning! The dispersion delay for channel %d (%f MHz) is %d samples > window size of %d!" % (i, freqs[i], dt, size) 
	dt = dt % size
	data[:,i] = (data[:,i] - means[i])/sigmas[i]
	data[np.where (data[:,i] > clip_level),i] = 0.0
	dedisp[:,i] = np.r_[data[dt:,i],data[:dt,i]]
	ts += dedisp[:,i]
	
		

# GP spectrum
max = ts.argmax()
wb=int(float(gpwidth/float(2.)))
wa=gpwidth-wb
spectrum = dedisp[max-wb:max+wa,:].mean(0)
noisespectrum = dedisp[0:gpwidth,:].mean(0)

if max-wb>0:
	if max+wa<size:
		tssigma=ts[0:max-wb].std()
		tsmean=ts[0:max-wb].mean()
	else:
		if size-max-wa>max-wb:
			tssigma=ts[max+wa:size].std()
			tsmean=ts[max+wa:size].mean()
		else:
			tssigma=ts[0:max-wb].std()
			tsmean=ts[0:max-wb].mean()
else:
	tssigma=ts[max+wa:size].std()
	tsmean=ts[max+wa:size].mean()

ts = (ts-tsmean)/tssigma

gpnumber = int(file.split("_gp")[1].split(".fil")[0].split("_sigma")[0])

decimdata=np.zeros((tdsize, fdsize), dtype=np.float32)
decimdedisp=np.zeros((tdsize, fdsize), dtype=np.float32)

if tdf == 1 and fdf == 1:
	decimdata = data
	decimdedisp = dedisp

if tdf != 1 and fdf == 1:
	counter=0
	for i in np.arange(0, tdsize*tdf, tdf):
		decimdata[counter,:] = data[i:i+tdf,:].mean(0)
		decimdedisp[counter,:] = dedisp[i:i+tdf,:].mean(0)
		counter += 1
	ts = [ts[j*tdf:(j+1)*tdf].mean() for j in np.arange(tdsize)]

if fdf != 1 and tdf == 1:
	counter=0
	for i in np.arange(0, fdsize*fdf, fdf):
		decimdata[:,counter] = data[:,i:i+fdf].mean(1)
		decimdedisp[:,counter] = dedisp[:,i:i+fdf].mean(1)
		counter += 1
	spectrum = [spectrum[j*fdf:(j+1)*fdf].mean() for j in np.arange(fdsize)]
	noisespectrum = [noisespectrum[j*fdf:(j+1)*fdf].mean() for j in np.arange(fdsize)]

if fdf != 1 and tdf != 1:
	counter=0
	tdata=np.empty(((tdsize, nchans)), dtype=np.float32)
	tdedisp=np.empty(((tdsize, nchans)), dtype=np.float32)
	for i in np.arange(0, tdsize*tdf, tdf):
		tdata[counter,:] = data[i:i+tdf,:].mean(0)
		tdedisp[counter,:] = dedisp[i:i+tdf,:].mean(0)
		counter += 1
	counter=0
	for i in np.arange(0, fdsize*fdf, fdf):
		decimdata[:,counter] = tdata[:,i:i+fdf].mean(1)
		decimdedisp[:,counter] = tdedisp[:,i:i+fdf].mean(1)
		counter += 1
	ts = [ts[j*tdf:(j+1)*tdf].mean() for j in np.arange(tdsize)]
	spectrum = [spectrum[j*fdf:(j+1)*fdf].mean() for j in np.arange(fdsize)]
	noisespectrum = [noisespectrum[j*fdf:(j+1)*fdf].mean() for j in np.arange(fdsize)]


# smoothing the spectrum
nsmooth=int(fdsize/25)
smoothspectrum=[np.array(spectrum[j:j+nsmooth]).mean() for j in np.arange(fdsize-nsmooth)]


###### Graph part ######

# dimensions of the central plot
left = 0.30
bottom = 0.08
width = 0.39
height = 0.55

if is_plotselect:
	plt.ion() # interactive plotting
fig = plt.figure()
if is_plotselect:
	fig.canvas.mpl_connect('key_press_event', press) # open keyboard events handler
	plot_help()

# First plot (dispersion broadening)
ax = plt.axes([0.74, 0.137, 0.2, 0.743])
ax.xaxis.tick_bottom()
ax.xaxis.set_label_position("bottom")
ax.xaxis.set_major_locator(MaxNLocator(5))
ax.yaxis.tick_left()
ax.yaxis.set_label_position("left")
ax.yaxis.set_major_formatter(NullFormatter())
ax.yaxis.set_major_locator(MaxNLocator(5))
for label in ax.get_xticklabels(): label.set_fontsize(fs)
for label in ax.get_yticklabels(): 
	label.set_fontsize(fs)
	label.set_rotation(90)
if fdf == 1:
	plt.xlabel('Channel number', fontsize=fs)
else:
	plt.xlabel('Channel number (x%d)' % (fdf,), fontsize=fs)
colormap=VariableColormap(cm.gist_stern)
colormap.set_scale(plot_scaling)
cax = ax.imshow(decimdata, interpolation=None, aspect='auto', origin='lower', cmap=colormap)
if gpsigma == -10:
	ax.annotate("GP #%d" % (gpnumber,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.85), fontsize=fs)
else:
	ax.annotate("GP #%d  Sigma: %.2f" % (gpnumber,gpsigma), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.85), fontsize=fs)
ax.annotate("Start MJD " + str(gpmjd), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.82), fontsize=fs)
ax.annotate("Nsamples: " + str(size), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.79), fontsize=fs)
ax.annotate("Nchannels: " + str(nchans), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.77), fontsize=fs)
ax.annotate("Samping interval: %.2f ms" % (tbin * 1.e3,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.74), fontsize=fs)
ax.annotate("DM: %.3f pc/cm^3" % (dm,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.72), fontsize=fs)
ax.annotate("Width: %d" % (gpwidth,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.70), fontsize=fs)
ax.annotate("Time decimation: %d" % (tdf,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.67), fontsize=fs)
ax.annotate("Freq decimation: %d" % (fdf,), xy=(0,0), xycoords='figure fraction', xytext=(0.06,0.64), fontsize=fs)
ax.set_xlim(xmin=0,xmax=fdsize)

axl = plt.twinx()
axl.yaxis.tick_right()
axl.yaxis.set_label_position("right")
axl.set_ylim(ymin=0,ymax=tbin*1.e3*size)
plt.ylabel('Time (ms)', fontsize=fs)
for label in axl.get_yticklabels(): 
	label.set_fontsize(fs)
	label.set_rotation(90)
axb = plt.twiny()
axb.xaxis.tick_top()
axb.xaxis.set_label_position("top")
axb.set_xlim(xmin=freqs[0],xmax=freqs[-1])
axb.xaxis.set_major_locator(MaxNLocator(5))
axb.xaxis.set_minor_locator(MultipleLocator(5))
for label in axb.get_xticklabels(): label.set_fontsize(fs)
plt.xlabel('Frequency (MHz)',fontsize=fs)

# Second plot (average profile)
ax3 = plt.axes([left, bottom+height, width, height-0.3])
ax3.xaxis.tick_bottom()
ax3.xaxis.set_label_position("bottom")
ax3.xaxis.set_major_formatter(NullFormatter())
ax3.xaxis.set_major_locator(MaxNLocator(5))
ax3.yaxis.tick_right()
ax3.yaxis.set_label_position("right")
for label in ax3.get_yticklabels():
	label.set_rotation(90)
	label.set_fontsize(fs)
plt.ylabel('Flux density ($\sigma$)', fontsize=fs)
if zoomtime == -1:
	ax3.plot(range(tdsize),ts)
	ax3.set_xlim(xmin=0,xmax=tdsize)
	ax3.set_ylim(ymin=np.min(ts),ymax=np.max(ts))
else:
	ax3.plot(range(tdsize),ts)
	ax3.set_xlim(xmin=int((max-zoomtime/2)/tdf),xmax=int((max+zoomtime/2)/tdf))
	ax3.set_ylim(ymin=np.min(ts[int((max-zoomtime/2)/tdf):int((max+zoomtime/2)/tdf)]),ymax=np.max(ts[int((max-zoomtime/2)/tdf):int((max+zoomtime/2)/tdf)]))
axb3 = plt.twiny()
axb3.xaxis.tick_top()
axb3.xaxis.set_label_position("top")
if zoomtime == -1:
	axb3.set_xlim(xmin=0,xmax=tdsize*tdf*tbin*1.e3)
	axb3.set_ylim(ymin=np.min(ts),ymax=np.max(ts))
else:
	axb3.set_xlim(xmin=int((max-zoomtime/2)/tdf)*tdf*tbin*1.e3,xmax=int((max+zoomtime/2)/tdf)*tdf*tbin*1.e3)
	axb3.set_ylim(ymin=np.min(ts[int((max-zoomtime/2)/tdf):int((max+zoomtime/2)/tdf)]),ymax=np.max(ts[int((max-zoomtime/2)/tdf):int((max+zoomtime/2)/tdf)]))
for label in axb3.get_xticklabels(): label.set_fontsize(fs)
plt.xlabel('Time (ms)', fontsize=fs)
axr3 = plt.twinx()
axr3.yaxis.tick_left()
axr3.yaxis.set_label_position("left")
axr3.yaxis.set_major_formatter(NullFormatter())

# Third plot (GP spectrum)
ax4 = plt.axes([left-0.24, bottom+0.165, width-0.15, width-0.005])
for label in ax4.get_xticklabels(): label.set_fontsize(fs)
plt.xlabel('Spectral flux density ($\sigma$)', fontsize=fs)
ax4.plot(spectrum, range(fdsize), "blue", antialiased=True, alpha=0.5)
ax4.plot(noisespectrum, range(fdsize), "green", antialiased=True)
ax4.plot(smoothspectrum, range(fdsize-nsmooth), "red", antialiased=True)
ax4.set_ylim(ymin=0,ymax=fdsize)
ax4.yaxis.set_major_locator(NullLocator())
ax4.yaxis.set_major_formatter(NullFormatter())
ax4.invert_xaxis()

ax4t = plt.twinx()
ax4t.yaxis.tick_left()
ax4t.yaxis.set_label_position("left")
ax4t.yaxis.set_major_locator(MaxNLocator(5))
ax4t.set_ylim(ymin=0,ymax=fdsize*fdf)
ax4t.invert_yaxis()
for label in ax4t.get_yticklabels(): 
	label.set_rotation(90)
	label.set_fontsize(fs)
plt.ylabel('Channel number', fontsize=fs)

# Fourth plot (aligned)
ax2 = plt.axes([left, bottom, width, height])
ax2.xaxis.tick_top()
ax2.xaxis.set_label_position("top")
ax2.xaxis.set_major_formatter(NullFormatter())
ax2.xaxis.set_major_locator(NullLocator())
ax2.yaxis.tick_left()
ax2.yaxis.set_label_position("left")
ax2.yaxis.set_major_formatter(NullFormatter())

cax2 = ax2.imshow(decimdedisp.T, interpolation=None, aspect='auto', origin='upper', cmap=colormap)
cbar2 = fig.colorbar(cax2, orientation='horizontal', spacing='proportional')
cbar2.ax.set_xlabel("Flux density ($\sigma$)", fontsize=fs)
for label in cbar2.ax.get_xticklabels(): label.set_fontsize(fs)
if zoomtime == -1:
	ax2.set_xlim(xmin=0,xmax=tdsize)
else:
	ax2.set_xlim(xmin=int((max-zoomtime/2)/tdf),xmax=int((max+zoomtime/2)/tdf))
ax2.set_ylim(ymin=0,ymax=fdsize)

axt3 = plt.twiny()
axt3.xaxis.tick_bottom()
axt3.xaxis.set_label_position("bottom")
if zoomtime == -1:
	axt3.set_xlim(xmin=0,xmax=tdsize*tdf*tbin*1.e3)
else:
	axt3.set_xlim(xmin=int((max-zoomtime/2)/tdf)*tdf*tbin*1.e3,xmax=int((max+zoomtime/2)/tdf)*tdf*tbin*1.e3)
for label in axt3.get_xticklabels(): label.set_fontsize(fs)
plt.xlabel('Time (ms)', fontsize=fs)
axt3.yaxis.tick_left()
axt3.yaxis.set_label_position("left")
axt3.yaxis.set_major_formatter(NullFormatter())
#axt3.set_ylim(ymin=freqs[0],ymax=freqs[-1])


axl2 = plt.twinx()
axl2.yaxis.set_major_locator(MaxNLocator(5))
axl2.yaxis.set_minor_locator(MultipleLocator(5))
axl2.yaxis.tick_right()
axl2.yaxis.set_label_position("right")
axl2.set_ylim(ymin=freqs[0],ymax=freqs[-1])
axl2.invert_yaxis()
plt.ylabel('Frequency (MHz)',fontsize=fs)
for label in axl2.get_yticklabels(): 
	label.set_rotation(90)
	label.set_fontsize(fs)

axb2 = plt.twiny()
axb2.xaxis.tick_top()
axb2.xaxis.set_label_position("top")
axb2.xaxis.set_major_formatter(NullFormatter())
if zoomtime == -1:
	axb2.set_xlim(xmin=0,xmax=tdsize)
else:
	axb2.set_xlim(xmin=int((max-zoomtime/2)/tdf),xmax=int((max+zoomtime/2)/tdf))
ax.xaxis.set_major_locator(MaxNLocator(4))
ax3.yaxis.set_major_locator(MaxNLocator(5))

if is_saveonly: 
	plt.savefig(pngname)
else:
	plt.show()
