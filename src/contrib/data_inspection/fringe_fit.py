#! /usr/bin/env python
import sys,math,beam_dr
import pydal as dal
from pylab import *
from numpy import *
from scipy import *
from random import *
import coordinates as coords

# set default values and check usage
if len(sys.argv) == 5:
	channel_range = range(0,256)
	time_range = [-1]
elif len(sys.argv) == 6:
	if len((sys.argv[5]).split(':')) == 2:  channel_range = range(int((sys.argv[5]).split(':')[0]),int((sys.argv[5]).split(':')[1]))
	elif len((sys.argv[5]).split(':')) == 1 and int(sys.argv[5]) != -1: channel_range = [int(sys.argv[5])]
	elif int(sys.argv[5]) == -1:  channel_range = range(0,256)
	time_range = [-1]
elif len(sys.argv) == 7:
	if len((sys.argv[5]).split(':')) == 2:  channel_range = range(int((sys.argv[5]).split(':')[0]),int((sys.argv[5]).split(':')[1]))
	elif len((sys.argv[5]).split(':')) == 1 and int(sys.argv[5]) != -1: channel_range = [int(sys.argv[5])]
	elif int(sys.argv[5]) == -1:  channel_range = range(0,256)
	if len((sys.argv[6]).split(':')) == 2:  time_range = [int((sys.argv[6]).split(':')[0]),int((sys.argv[6]).split(':')[1])]
	else:  time_range = [-1]
else:
	print "Usage:"
	print "\tfringe_fit.py <file> <antenna1> <antenna2> " + \
	      "<sub-band> [channel range] [time range]"
	print "\t<> required"
	print "\tChannel and Time in relative bin coordinates; channel range averaged, time range selected."
	print "\t-1 means use all channels.  Default fits the real part of xx for all times and channels."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"
verbose = 1
ra_casa = math.radians(coords.hmstora(23.,23.,24.))
dec_casa = math.radians(coords.dmstodec(58.,48.,54.))
ra_cyga = math.radians(coords.hmstora(19.,59.,28.3))
dec_cyga = math.radians(coords.dmstodec(40.,44.,02.))
ra_vira = math.radians(coords.hmstora(12.,30.,49.5))
dec_vira = math.radians(coords.dmstodec(12.,23.,21.5))
ra_crab = math.radians(coords.hmstora(5.,34.,31.8))
dec_crab = math.radians(coords.dmstodec(22.,0.,52.9))
ntrials = 20

# open file
msds= dal.dalDataset()
if ( msds.open(sys.argv[1]) ):
	sys.exit(1)

# get phase dir
fieldtab = msds.openTable('FIELD')
phasedircol = fieldtab.getColumn('PHASE_DIR')
phasedir = phasedircol.data()[0][0]
ra_pc = phasedir[0]
dec_pc = phasedir[1]

# set lmn
lmn_casa = coords.radec_to_lmn(ra_pc,dec_pc,ra_casa,dec_casa)
lmn_cyga = coords.radec_to_lmn(ra_pc,dec_pc,ra_cyga,dec_cyga)
lmn_vira = coords.radec_to_lmn(ra_pc,dec_pc,ra_vira,dec_vira)
lmn_crab = coords.radec_to_lmn(ra_pc,dec_pc,ra_crab,dec_crab)

# get frequencies
freqtab = msds.openTable('SPECTRAL_WINDOW')
freqcol = freqtab.getColumn('CHAN_FREQ')
freq = freqcol.data()[0][channel_range]
mean_freq = mean(freq)
print 'Frequency = ' + str(mean_freq) + ' Hz , Wavelength = ' + str(299792458.0/mean_freq) + ' m'

# open table
tablename = "MAIN";
msds.setFilter( "TIME,UVW," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + sys.argv[4] )
maintable = msds.openTable( tablename );

# get times
time_col = maintable.getColumn("TIME")
time = time_col.data()
if len(time_range) == 1:  time_range = [0,len(time)]
time = time[time_range[0]:time_range[1]]/(24*3600)    # take subset and convert from MJD in seconds to days

if verbose>1:
	print 'Start of data (MJD): ' + str(time[0])
	print 'Total length of data ' + str(len(time)) + ' integrations, ' + str((time[-1]-time[0])*24) + ' hours'

# get the u,v columns
uvw_column = maintable.getColumn("UVW")
uvw = uvw_column.data()
u_trim = uvw[time_range[0]:time_range[1],0]*(mean_freq/299792458.0)  # in units of lambda
v_trim = uvw[time_range[0]:time_range[1],1]*(mean_freq/299792458.0)
w_trim = uvw[time_range[0]:time_range[1],2]*(mean_freq/299792458.0)

# set up array with normalized beam amplitude
beam_casa = []
beam_cyga = []
altaz_casa = []
altaz_cyga = []
for t in time:
	altaz_casa.append(coords.altaz(t*24*3600,math.degrees(ra_casa),math.degrees(dec_casa)))
	altaz_cyga.append(coords.altaz(t*24*3600,math.degrees(ra_cyga),math.degrees(dec_cyga)))
	beam_casa.append(beam_dr.lba(mean_freq,(coords.altaz(t*24*3600,math.degrees(ra_casa),math.degrees(dec_casa)))[0],(coords.altaz(t*24*3600,math.degrees(ra_casa),math.degrees(dec_casa)))[1]))
	beam_cyga.append(beam_dr.lba(mean_freq,(coords.altaz(t*24*3600,math.degrees(ra_cyga),math.degrees(dec_cyga)))[0],(coords.altaz(t*24*3600,math.degrees(ra_cyga),math.degrees(dec_cyga)))[1]))
beam_casa = array(beam_casa)
beam_cyga = array(beam_cyga)
altaz_casa = array(altaz_casa)
altaz_cyga = array(altaz_cyga)

if verbose>1:
	subplot(311)
	plot(time,altaz_casa[:,0],'b',time,altaz_casa[:,1],'r')
	subplot(312)
	plot(time,altaz_cyga[:,0],'b',time,altaz_cyga[:,1],'r')
	subplot(313)
	plot(time,beam_casa,'b',time,beam_cyga,'r')
	show()

# get data
data_col = maintable.getColumn(data_name)
data = data_col.data()

data_reduce = add.reduce(array=data[time_range[0]:time_range[1],channel_range,:],axis=1)/len(channel_range)
data_xxa = abs(data_reduce[:,0])
#data_xxr = (data_reduce[:,0]).real
rel_time = time - time[0]

# calculate fringe rate:  u*l+v*m+w*(n-1)
fr_casa = 2*pi*(u_trim*lmn_casa[0] + v_trim*lmn_casa[1] + w_trim*(lmn_casa[2]-1))
fr_cyga = 2*pi*(u_trim*lmn_cyga[0] + v_trim*lmn_cyga[1] + w_trim*(lmn_cyga[2]-1))
fr_vira = 2*pi*(u_trim*lmn_vira[0] + v_trim*lmn_vira[1] + w_trim*(lmn_vira[2]-1))
fr_crab = 2*pi*(u_trim*lmn_crab[0] + v_trim*lmn_crab[1] + w_trim*(lmn_crab[2]-1))

# Check values of l,m,u,v,fringe rate
if verbose>1:
	print lmn_casa
	print lmn_cyga
	figure(1)
	subplot(311)
	plot(u_trim,v_trim)
	subplot(312)
	plot(rel_time,2*pi*u_trim*lmn_cyga[0],rel_time,2*pi*v_trim*lmn_cyga[1],rel_time,2*pi*w_trim*(lmn_cyga[2]-1))
	subplot(313)
	plot(rel_time,fr_casa,rel_time,fr_cyga,rel_time,fr_vira,rel_time,fr_crab)

# fitting 
x1 = fr_casa
x2 = fr_cyga
x3 = fr_vira
x4 = fr_crab
#fitfunc = lambda p, x1, x2: p[0]*cos(x1+p[1]) + p[2]*cos(x2+p[3])     # for 2 sources, real data
#errfunc = lambda p, x1, x2, y: fitfunc(p,x1,x2) - y
fitfunc = lambda p, x1, x2, beam_casa, beam_cyga: sqrt((p[0]*beam_casa)**2 + (p[2]*beam_cyga)**2 + 2*p[0]*beam_casa*p[2]*beam_cyga*cos(x1-x2+p[1]))  # for 2 sources, data amplitude
errfunc = lambda p, x1, x2, beam_casa, beam_cyga, y: fitfunc(p,x1,x2,beam_casa,beam_cyga) - y
p0_range = [[-max(data_xxa),max(data_xxa)],[0,2*pi],[-max(data_xxa),max(data_xxa)]]

# loop to do multiple fits and keep parameters of best fit
for i in range(ntrials):
	p0 = [p0_range[0][0]+random()*(p0_range[0][1]-p0_range[0][0]),p0_range[1][0]+random()*(p0_range[1][1]-p0_range[1][0]),p0_range[2][0]+random()*(p0_range[2][1]-p0_range[2][0])]
#	p1, cov_x, infodict, mesg, success = optimize.leastsq(errfunc, p0[:], args = (x1, x2, data_xxr), full_output=1)
	p1, cov_x, infodict, mesg, success = optimize.leastsq(errfunc, p0[:], args = (x1, x2, beam_casa, beam_cyga, data_xxa), full_output=1)
	if i == 0:
		best_noise = (array(infodict['fvec'])).std()
		best_params = p1
	if verbose > 1: print 'For i = ' + str(i) + '.  Initial guess: ' + str(p0) +  '. Best fit: ' + str(p1) + '.'
	if (array(infodict['fvec'])).std() < best_noise:
		if verbose > 0:  print 'For i = ' + str(i) + '. Old Noise: ' + str(best_noise) + ', New Noise: ' + str((array(infodict['fvec'])).std())
		best_noise = (array(infodict['fvec'])).std()
		best_params = p1

if verbose > 0:  print 'Best Params: ' + str(best_params)
if verbose > 0:  print 'Ampl_CasA/Ampl_CygA = ' + str(abs(best_params[0])/abs(best_params[2]))

# stuff for fft analysis (for debugging to find fringe rates)
#fft_time = ((arange(len(rel_time))+1)/float(len(rel_time)))*24*60*2.

# plot
figure(2)
subplot(211)
#plot(rel_time,data_xxr,"b--",rel_time,fitfunc(best_params,x1,x2),"r-")
plot(rel_time,data_xxa,"b--",rel_time,fitfunc(best_params,x1,x2,beam_casa,beam_cyga),"r-")
subplot(212)

# more fft stuff
#fft_model = abs(fft(fitfunc(best_params,x1,x2)))
#fft_data = abs(fft(data_xxr))
#plot(fft_time,fft_data,"b--",fft_time,fft_model,"r-")
#plot(fft_time,abs(fft(data_xxr))/max(abs(fft(data_xxr))),"b--",fft_time,abs(fft(fitfunc(best_params,x1,x2)))/max(abs(fft(fitfunc(best_params,x1,x2)))),"r-")
#plot(fft_time,abs(fft(data_xxa))/max(abs(fft(data_xxa))),"b--",fft_time,abs(fft(fitfunc(best_params,x1,x2,beam_casa,beam_cyga)))/max(abs(fft(fitfunc(best_params,x1,x2,beam_casa,beam_cyga)))),"r-")
#axis([0,500,min(append(abs(fft(data_xxr)),abs(fft(fitfunc(best_params,x1,x2))))),max(append(abs(fft(data_xxr)),abs(fft(fitfunc(best_params,x1,x2)))))])

plot(time,beam_casa,'b',time,beam_cyga,'r')
#plot(time,fitfunc(best_params,x1,x2,beam_casa,beam_cyga),'b',time,fitfunc([2*best_params[0],best_params[1],best_params[2]],x1,x2,beam_casa,beam_cyga),'r',time,fitfunc([best_params[0],best_params[1],best_params[0]],x1,x2,beam_casa,beam_cyga),'g')
# more plot stuff
subplot(211)
title("Time vs. xxa, Baseline " + \
      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] +
      ") " + " Chan0(" + str(channel_range[0]) + "), nchan(" + str(len(channel_range)) + ")\n" + sys.argv[1] )
xlabel("Time (MJD)")
ylabel("Intensity")

show()
