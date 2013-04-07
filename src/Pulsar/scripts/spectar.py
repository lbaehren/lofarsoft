#!/usr/bin/python
#Program to read dspsr archive files and produce spectra

from optparse import OptionParser
import ephem
import psrchive
import pyfits
import numpy as np
import sys
import string
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def factors(n, m):
    a=[]
    for i in range(1,int(n/2+1)):
        if n % i == 0 and i < m: 
            a.append(i)
    return a

parser = OptionParser()
parser.add_option("-f", "--file", dest="filenm")
parser.add_option("-p", "--parset", dest="parset")
parser.add_option("-c", "--chan", dest="fscr", type="int", default=20)
parser.add_option("-b", "--bin", dest="bscr", type="int", default=-1)
parser.add_option("--sap", dest="sapid", type="int", default=0)
parser.add_option("--tab", dest="tabid", type="int", default=0)
parser.add_option("-s", "--sourcemap", dest="skyplot", action="store_true", default=False)
parser.add_option("-r", "--plotprofs", dest="plotprofs", action="store_true", default=False)
parser.add_option("-o", "--out", dest="out", default="")
parser.add_option("-x", "--show", dest="spectraplot", action="store_true", default=False)
(opts, args) = parser.parse_args()

if opts.filenm==None or opts.parset==None:
    print parser.print_help()
    sys.exit(1)

# CS TAB half-FWHM in radians (at 120 MHz)
tabrad = 0.0035
ra_offset = 0.0
dec_offset = 0.0

#parse parset
for line in open(opts.parset, 'r').readlines():
    if "Observation.Beam[%d].angle1" % (opts.sapid) in line:
        alt = float( string.split(line, "= ")[1].strip() )
    if "Observation.Beam[%d].TiedArrayBeam[%d].angle1" % (opts.sapid, opts.tabid) in line:
        ra_offset = float( string.split(line, "= ")[1].strip() )
    if "Observation.Beam[%d].angle2" % (opts.sapid) in line:
        az = float( string.split(line, "= ")[1].strip() )
    if "Observation.Beam[%d].TiedArrayBeam[%d].angle2" % (opts.sapid, opts.tabid) in line:
        dec_offset = float( string.split(line, "= ")[1].strip() )
    if abs(ra_offset) > tabrad or abs(dec_offset) > tabrad:
        print "The offset of the beam %d is too large from the SAP%d center" % (tabid, sapid)
	sys.exit(1)    
    else:
        alt += ra_offset
        az += dec_offset 
    if "Observation.Beam[%d].TiedArrayBeam[%d].coherent" % (opts.sapid, opts.tabid) in line:
        is_coherent = string.split(line, "= ")[1].strip().lower()[:1]
        if is_coherent == 'f':
            print "The given beam is incoherent. Exiting."
            sys.exit(1)
    if "OLAP.PencilInfo.flysEye" in line:
        is_fe = string.split(line, "= ")[1].strip().lower()[:1]
	if is_fe == 't':
            print "The given observation is FlysEye. Exiting."
            sys.exit(1)
    if "Observation.ObservationControl.OnlineControl.OLAP.storageStationNames = AllStations" in line:
        continue
    elif "OLAP.storageStationNames" in line:
        line = string.split(line, "= ")[1].strip().strip(']').strip('[')
        stations = string.split(line, ',')
        n_stat = len(stations)
    if "Observation.startTime" in line:
        start = string.split(line, "= ")[1].strip()
        start = start.strip("'")
        start = start.replace('-','/')
    if "Observation.Scheduler.taskDuration" in line:
        deltat = float(string.split(line, "= ")[1].strip())/2

arch = psrchive.Archive_load(opts.filenm)
psr = arch.get_source()
nchan = arch.get_nchan()
new_nchan = factors(nchan, opts.fscr)[-1]
nbin = arch.get_nbin()
nint = arch.get_nsubint()
arch.dedisperse()
arch.tscrunch_to_nsub(1)
arch.fscrunch_to_nchan(new_nchan)    
if opts.bscr != -1:
    arch.bscrunch_to_nbin(opts.bscr)
data = arch.get_data()[0][0]
nchan = len(data)
freq_lo = arch.get_centre_frequency() - arch.get_bandwidth()/2.0
bw_c = arch.get_bandwidth()/nchan
kb = 1.3806503e3 # *1e23 to convert to mJy
if freq_lo > 100:
    n_stat = n_stat/2
t_int = arch.integration_length()
spec = []
freqs = []
err = []
limits=[]
err_limits=[]
freq_limits=[]
bandpass = []
z=[]

#define observatory
lofar = ephem.Observer()
lofar.long = '6.85'      #Might need to improve these
lofar.lat = '52.9'
lofar.elevation = 18.9
lofar.date = start
lofar.date += ephem.second*deltat

#define source
src = ephem.Equatorial(alt,az,epoch=ephem.J2000)
src = ephem.readdb("PSR,f|L,"+str(src.ra)+","+str(src.dec)+",2.02,2000")
src.compute(lofar)

#calculate galactic longitude and latitude from J2000
gal = ephem.Galactic(src, epoch=ephem.J2000)
GLr = float(gal.long)
GBr = float(gal.lat)

#calculate angle of source at observing time
alt = float(src.alt)
az  = float(src.az)

#find sky temperature in direction of the pulsar
lofarsoft = os.environ['LOFARSOFT']
if lofarsoft == "":
    skymap="/home/hassall/findTsky/lambda_mollweide_haslam408_dsds.fits"
else:
    skymap=lofarsoft + "/release/share/pulsar/data/lambda_mollweide_haslam408_dsds.fits"
fln = pyfits.open(skymap)
x_scale = fln[1].header['cdelt1']
y_scale = fln[1].header['cdelt2']
x_ref = fln[1].header['crpix1']
y_ref = fln[1].header['crpix2']
rawdata = fln[1].data
pi = 3.141592654

if GLr > pi:
    GLr = -2.0*pi + GLr
#GLr = GL/57.29577951
#GBr = GB/57.29577951

theta = GBr
delta_theta = 0.5
i = 0
while delta_theta > 0.0001:
    theta_old = theta
    top = 2*theta + np.sin(2*theta) - pi*np.sin(GBr)
    bottom = 2 + 2*np.cos(2*theta)
    theta = theta - top/bottom
    delta_theta = abs(theta - theta_old)
    i+=1

x = 2.0*2.0**0.5/pi*GLr*np.cos(theta)
x = x*57.29577951
x = x/x_scale + x_ref

y = 2.0**0.5*np.sin(theta)
y = y*57.29577951
y = y/y_scale + y_ref
x = int(x)
y = int(y)
Tsky = rawdata[y,x]*(300.0/408.0)**2.55
if opts.skyplot==True:
    plt.imshow(rawdata, origin='lower')
    plt.scatter(x,y, color='1.0')
    plt.xlim(0,4096)
    plt.ylim(0,2048)
    plt.xticks(())
    plt.yticks(())
    plt.show()

#print summary of data to check nothing looks stupid
print 50*"-"
print "Running spectar.py on "+psr
print "RA:", src.ra
print "Dec:", src.dec
print "GL: "+str(GLr)
print "GB: "+str(GBr)
print "Alt: " + str(alt)
print "Az: " + str(az)
print "Epoch " + str(lofar.date)
print "Tsky: "+str(Tsky)
print "N_stations: "+str(n_stat)
print "Frequency range: " + str(freq_lo) + "-" + str(freq_lo + bw_c*nchan) + " MHz"
print "Nchan: "+str(nchan)
print 50*"-"

#begin calibration
for i in range(nchan):
    freq = freq_lo + i*bw_c
    #find signal-to-noise ratio
    noise = np.min([np.std(data[i][0:0.4*len(data[i])]),np.std(data[i][0.2*len(data[i]):0.6*len(data[i])]),np.std(data[i][0.4*len(data[i]):0.8*len(data[i])]),np.std(data[i][0.6*len(data[i]):1.0*len(data[i])])])
    prof = np.array(data[i])
    prof = prof - np.median(prof)
    prof /= noise
    if opts.plotprofs == True:
        plt.plot(prof + 3*i)
    signal = np.max(prof)

    #find (relative) pulse width
    W = float(len(prof[prof>0.7*np.max(prof)]))
    P = float(len(prof))
    if W < 3.0:
        print "WARNING: Pulse is unresolved at "+str(freq)+"MHz"
    res_corr = W**0.5

    #Beam correction
    jones = os.popen("/home/hassall/fluxCal/get_beam_values "+str(freq*1e6)+" " + str(az) + " " +str(alt))
    jones = string.split(jones.readline())
    line = np.array(jones, dtype='float')
    xx = (line[0]**2 + line[1]**2)**0.5
    xy = (line[2]**2 + line[3]**2)**0.5
    X  = ( xx + xy )
    yx = (line[4]**2 + line[5]**2)**0.5
    yy = (line[6]**2 + line[7]**2)**0.5
    Y  = ( yx + yy )
    beamcorr = (X**2 + Y**2)**0.5

    #account for beam in Aeff/T measurements
    jones = os.popen("/home/hassall/fluxCal/get_beam_values "+str(freq*1e6)+" " + str(3.14159) + " " +str(1.46782))
    jones = string.split(jones.readline())
    line = np.array(jones, dtype='float')
    xx = (line[0]**2 + line[1]**2)**0.5
    xy = (line[2]**2 + line[3]**2)**0.5
    X  = ( xx + xy )
    yx = (line[4]**2 + line[5]**2)**0.5
    yy = (line[6]**2 + line[7]**2)**0.5
    Y  = ( yx + yy )
    casBeamcorr = (X**2 + Y**2)**0.5

    if beamcorr != 0:
        beamcorr = beamcorr/casBeamcorr

    #polynomial expressions for Aeff and Tinst from fit to Wijnholds (2011)
    #HBA
    if freq_lo > 100:
        A_eff_poly = (-4.69323950336e-12, 3.4644462219e-09, -4.69371100939e-05, 9.61230768673e-05, 26.5188023385)
        T_inst_poly = (7.05564467327e-08, -6.66404696836e-05, 0.0261748789364, -5.46923853821, 640.871742765, -39912.7706015, 1032242.77844)
#        T_inst_poly = (-3.79183088284e-13, 5.63963812382e-10, -3.76028741248e-07, 0.000148008308944, -0.0380841029773, 6.69348286531, -813.747974693, 67568.8542937, -3667171.32567, 117466061.759, -1686265424.44)
    #LBA
    else:
        A_eff_poly = (6.11941983385e-06, -0.00144016295395, 0.130257493869, -5.80665936301, 120.27374171)
        T_inst_poly = (2.83265433529e-05, -0.00870954199409, 1.09582298486, -72.2659900479, 2642.49354074, -51196.2644086, 418100.750887)
#        T_inst_poly=(6.11135659724e-09, -2.99352217676e-06, 0.000644294917127, -0.0799195431043, 6.29240791869, -325.936335696, 11101.8877581, -239681.189154, 2974666.61556, -16155008.4695)

    #calculate Aeff from polynomial 
    Aeff = 0.0
    dpoly = len(A_eff_poly)
    for ii in range(dpoly):
        Aeff += A_eff_poly[ii]*freq**(dpoly-ii-1)

    #calculate Tinst from polynomial 
    Tinst = 0.0
    dpoly = len(T_inst_poly)
    for ii in range(dpoly):
        Tinst += T_inst_poly[ii]*freq**(dpoly-ii-1)
    
    #Add Tsky
    Tsys = Tinst + Tsky*(300/freq)**2.55

    #Find Aeff/Tsys
    A_T = Aeff/Tsys

    #multiply by number of stations
    A_T = A_T*n_stat
    if A_T < 0:
        continue

    #calculate flux and errors
    flux = signal*(2*kb/(A_T*(bw_c*t_int*2)**0.5))*(W/(P-W))**0.5*res_corr*beamcorr
    noise = 2*kb/(A_T*(bw_c*t_int*2)**0.5)*res_corr*beamcorr
    if signal < 3.5:
        limits.append(1.5*noise)
        freq_limits.append(freq)
        err_limits.append(noise)
    else:
        spec.append(flux)        
        freqs.append(freq)
        err.append(noise)
        bandpass.append(np.mean(data[i]))
        z.append(A_T)

###Noise estimate###
#z = np.array(z)
#z = 2*kb/(z*bw_c**0.5*0.001**0.5*2)
#print (sum(z**2))**0.5/1000
####################

if opts.plotprofs==True:
    plt.xlim(0,len(prof))
    plt.show()

#plot
if opts.spectraplot==True:
    fig = plt.figure()
    fig.add_subplot(212)
    plt.plot(freqs, bandpass)
    plt.xlabel("Frequency (MHz)")
    plt.ylabel("Bandpass")
    plt.yticks(())
    fig.add_subplot(211)
    if len(freq_limits) > 0:
        plt.errorbar(freq_limits, limits, yerr=err_limits, lolims=True, linestyle='None')
        plt.ylim(1,1.1*max(limits))
    if len(freqs) > 0:
        plt.errorbar(freqs,spec, yerr=err, fmt='.')
        plt.ylim(1,1.1*max(spec))
    plt.title('PSR ' + psr)
    plt.xlabel("Frequency (MHz)")
    plt.ylabel("Flux Density (mJy)")
    #plt.xscale('log')
    #plt.yscale('log')
    plt.show()

#write out spectra
if len(opts.out)>0:
    print "Writing spectra to", opts.out
    f = open(opts.out, 'w')
    for i in range(len(freqs)):
        print >> f, freqs[i], spec[i], err[i]
    f.close()
