#! /usr/bin/env python

##########################################################################
#
# Software to correlate LOFAR TBB data (h5 format) from a Single Station
#
# individual channelwidth can be choosen or can be calculated for differ.
# RM resolution
#
##########################################################################


from pycrtools import *
from pycrtools import metadata as md
import numpy as np
import pycrtools as cr
import sys
import os
import os.path
import re
import optparse
import numpy as np
import matplotlib.pyplot as plt
import pyfits
from pylab import *
from numpy import *
from math import *
from scipy import *
from numpy.fft import fft
import pickle
from scipy.special import iv
import time
from datetime import datetime
import threading
from tempfile import mkdtemp
import Queue


start = time.time()

getFFTData = False      # get FFTData instead of timeseries-data
var = False         
divide = True

if getFFTData:
    divide = False

BSbig = 200# for divide = True

stationname = "CS003" #"DE601"      # for Effelsberg
#antennafilename = LOFARSOFT+"data/lofar/StaticMetaData/AntennaArrays/"+stationname+"-AntennaArrays.conf"

#-------------get in files and choose options--------------------

usage = "usage: %prog [options] file0.h5 file1.h5 ..."
parser = optparse.OptionParser(usage=usage)

parser.add_option('-s', '--startblock', help = 'startblock to read in the data', default = 100000 )
parser.add_option('-b', '--blocksize',  help = 'blocksize as a factor of 1024 or var ', default = 1)
parser.add_option('-n', '--nblocks',    help = 'number of blocks to read in; 0 means all', default = 2)
parser.add_option('-f', '--startchan',  help = 'startfrequency in MHz ', default = 0)
parser.add_option('-e', '--endchan',    help = 'endfrequency in MHz ', default = 100)  
parser.add_option('-r', '--removercu',  help = 'remove one RCU [ list not implemented yet ] ', default = -1)
parser.add_option('-o', '--outfile',    help = 'outfile for the cm', default = "correlation")

(options, args) = parser.parse_args()

startblock = int(options.startblock)
wide = options.blocksize
nblocks = int(options.nblocks)
startfreq = float(options.startchan)
endfreq = float(options.endchan)
outfile = str(options.outfile)

removercu = int(options.removercu)

if wide == "var":
    var = True
#else:
#   wide = int(wide)

#-------------calculating needed frequencies for given RM--------

if var:
    print "calculation of the frequencies \n"
    RM = 100            # max RM that is expected
    deltaphi = 180          # max deltaphi (depol.) within one SB
    fre = startfre
    freliste = []
    BSliste = []
    
    const = 0.5*deltaphi/(3*3*10000*RM)*2*pi/360
    deltav0 = const*fre**3
    wide = int(round(0.2/deltav0))  # equal to BS0
    
    while math.floor(fre) <= endfre:
        deltav = const*fre**3
        BS = 0.2/deltav
        freliste.append(fre)
        fre += deltav
        BSliste.append(round(wide/BS))
    
    lenBSliste = len(BSliste)
    print " - ready -\n"

#----------------------------------------------------------------

startfre = int(startfreq/200*1024*wide)
endfre = int(endfreq/200*1024*wide)

share = 1
if divide:
    if wide <= BSbig:
        share = int(round(BSbig/wide))
        wide *= share

blocksize = int(wide * 1024)
print 'Taking blocksize = %d' % blocksize

files = args
count = 0
for n in files:
    count += 1

data = [array]*count
shiftmax = 0
f = [array]*count

#-------------read in RCUs---------------------------------------
select_index = [array] * count
for i in range(0,count,1):
    data[i] = cr.open(files[i],blocksize)
    f[i] = cr.TBBData(files[i])
    
    # select dipoles with correct data length ( >= median data length) to prevent i/o errors when reading data
    datalen = np.array(data[i]["DATA_LENGTH"])
    medianlen = np.median(datalen)
    select_index[i] = list(np.where(datalen >= medianlen)[0])
    for x in select_index[i]:
        # remove even /odd counterpart! Need matching no. of even and odd RCUs
        if x % 2 == 0 and x+1 not in select_index[i]:
            select_index[i].remove(x)
        if x % 2 == 1 and x-1 not in select_index[i]:
            select_index[i].remove(x)
    dipoles = np.array(data[i]["DIPOLE_NAMES"])
    selected_dipoles_numpystr = list(dipoles[select_index[i]])
    selected_dipoles = [str(x) for x in selected_dipoles_numpystr] # needs to be Python-strings
    print 'Number of selected dipoles: %d' % len(selected_dipoles)
    notselected = [int(x) % 100 for x in dipoles if x not in selected_dipoles]
                       
    if len(notselected) > 0:
        print 'RCUs removed because of too little data:'
        print notselected
    data[i]["SELECTED_DIPOLES"] = selected_dipoles
    f[i].selectDipoles(selected_dipoles)
        
#        removelist = [removercu, removercu + 1] if removercu % 2 == 0 else [removercu - 1, removercu]
#        selected_dipoles = [x for x in data[i]["DIPOLE_NAMES"] if int(x) % 100 not in removelist]

    if i == 0:
#        import pdb; pdb.set_trace()
        rcun = data[i]["SELECTED_DIPOLES"]
        nant = len(rcun)
        print 'rcun = %d' % nant
        shifts = zeros((count,nant),dtype = int)
    
    if i != 0:
        rcun2 = data[i]["SELECTED_DIPOLES"]
        rcun = hstack((rcun, rcun2))
        del rcun2

#-------------read in antenna positions -------------------------

for i in range(0,count,1):
    data[i] = cr.open(files[i],blocksize)
    f[i] = cr.TBBData(files[i])

    # reuse selection from above...
    dipoles = np.array(data[i]["DIPOLE_NAMES"])
    selected_dipoles_numpystr = list(dipoles[select_index])
    selected_dipoles = [str(x) for x in selected_dipoles_numpystr] # needs to be Python-strings
    print 'Number of selected dipoles: %d' % len(selected_dipoles)
#    notselected = [x for x in dipoles if x not in selected_dipoles]
#    if len(notselected) > 0:
#        print 'RCUs removed because of too little data:'
#        print notselected
    data[i]["SELECTED_DIPOLES"] = selected_dipoles
    f[i].selectDipoles(selected_dipoles)
    
    if i == 0:
        antposp = data[i]["ITRFANTENNA_POSITIONS"].toNumpy()
        nant = len(antposp)
        print '%d antenna positions read in' % nant
    
    if i != 0:
        antposp2 = data[i]["ITRFANTENNA_POSITIONS"].toNumpy()
        antposp = vstack((antposp, antposp2))
        del antposp2
    
    #-------------shifts between differnt RCUs-----------------------
    
    shifts[i] = data[i]["SAMPLE_NUMBER"]
    
    localmax = max(shifts[i])
    if localmax > shiftmax:
        shiftmax = localmax

for i in range(0,count,1):
    shifts[i] = ((shiftmax-shifts[i]))
    
    if getFFTData:
        shifts[i] /= blocksize

rculiste = [array]*len(rcun)
for rcu in range(0,len(rcun),1):
    rculiste[rcu] = (int(rcun[rcu])%1000)

rcun = rculiste
#antnr = max(rcun)+1
antnr = 0
for d in data:
    antnr += d["NOF_SELECTED_DATASETS"]

print "BLAH", antnr
print shifts
print '# shifts:' 
print shifts.shape


#----------------------------------------------------------------

lenfre = blocksize/2 + 1

if nblocks == 0:
    nblocks = int((data[0]["DATA_LENGTH"][0]-shiftmax/1024-startblock-10000)/1024/wide)

if not var:
    freliste = (cr.open(files[0], blocksize/share)).getFrequencies().toNumpy()[startfre:endfre]
    freliste /= 1000000

timestamp = (data[0])["TIME"][0]
timedate = datetime.fromtimestamp(timestamp)        # real time
timeutc = datetime.utcfromtimestamp(timestamp)      # UTC time
timetuple = time.gmtime(timestamp)          # time tuple

del shiftmax, files, rculiste

#-------------coeffizients for PolyphaseFilter------------------- 
# (S.Faint, W.Read, 2003)

if not getFFTData:
    #print "\nStart calculating filter coefficients\n"
    
    a = 16.0/pi #for Kaiser window
    
    if divide:
        blocksize /= share
    h = cr.hArray(complex, dimensions = (1,1,blocksize))
    
    for n in range(0,blocksize,1):
        # Kaiser-Window:
        #   wn = iv(0,pi*a*sqrt(1-(2.0*n/blocksize-1)*(2.0*n/blocksize-1)))/(iv(0,pi*a))
        # Hamming-window:
        #   wn = 0.54+0.46*math.cos(2*n/blocksize*pi)
        # Blackman-Window:
        wn = 0.42 - 0.5*math.cos(2.0*n/blocksize*pi) + 0.08*cos(4.0*n/blocksize*pi)
        
        h[0,0,n] = wn * sinc((n-float(blocksize)/2+0.5)/blocksize) 
        del wn
    
    if divide:
        blocksize *= share
    
    h = h.toNumpy()

#print " - ready -\n"

#-------------reading in the data--------------------------------

class readin(threading.Thread):
    def __init__(self, i, block, offset):
        self.i = i
        self.block = block
        self.offset = offset
        threading.Thread.__init__(self)
    def run(self):
        if getFFTData:
            self.offset = cr.hArray(self.block + shifts[self.i])
            data[self.i].getFFTData(fftdata[self.i*nant:(self.i+1)*nant], self.offset)  
        else:
            self.offset = cr.hArray(self.block + shifts[self.i])
            self.offset = cr.hArray((self.block)*blocksize + shifts[self.i]+startblock)
            #                        data[self.i]["BLOCKSIZE"] = blocksize
            #                        print "BLA", fftdata.shape(), self.i*nant, (self.i+1)*nant, self.offset
            #                        data[self.i].getTimeseriesData(fftdata[self.i*nant:(self.i+1)*nant], self.offset)
            #import pdb; pdb.set_trace()
            cr.hReadTimeseriesData(fftdata[self.i*nant:(self.i+1)*nant], self.offset, blocksize, f[self.i])
            fftdata[94] = 0
            fftdata[95] = 0

#-------------correlate the data---------------------------------

class correlation(threading.Thread):
    def __init__(self, fre, fftdatan):
        self.fre = fre
        self.fftdatan = fftdatan
        threading.Thread.__init__(self)
    def run(self):
        acm[:,:,self.fre] += dot((self.fftdatan.transpose()).conjugate(), self.fftdatan)

#-------------main slope for reading and correlation-------------

class main(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):  
        for block in range(0, nblocks,1 ):
            
            ready = [array]*count
            for i in range(0, count,1):
                offset = 0
                threadread = readin(i, block, offset)
                threadread.start()
                ready[i] = threadread
            
            for threadread in ready:
                threadread.join()
            del ready
            
            fftdatan = fftdata.toNumpy()
            
            if block != 0:
                for threadcorr in corry:
                    threadcorr.join()
                del corry
            
            fftdatan = hsplit(fftdata.toNumpy(),share)
            
            if not getFFTData:
                fftdatan *= h
                fftdatan = (fft(fftdatan))[:,:,startfre:endfre]
            else:
                fftdatan = fftdatan[:][:][startfre:endfre]
            
            corry = [array]*len(freliste)
            if var:
                diffBS = 0
                for fre in range(0,lenBSliste,1):
                    diffBS += BSliste[fre]
                    threadcorr = correlation(fre, fftdatan[:,:,int(diffBS)])
                    threadcorr.start()
                    corry[fre] = threadcorr
            else:
                for fre in range(0,endfre-startfre,1):
                    threadcorr = correlation(fre,fftdatan[:,:,fre])
                    threadcorr.start()
                    corry[fre] = threadcorr
            
            if block == (nblocks-1):
                for threadcorr in corry:
                    threadcorr.join()
            
            print "block: ",repr(block+1).rjust(len(str(nblocks))),"/",nblocks

#-------------start of main programm-----------------------------

fftdatan = zeros((share,antnr, blocksize/share),dtype = complex)

if getFFTData:
    fftdata = cr.hArray(complex, dimensions = (antnr,lenfre))
else:
    fftdata = cr.hArray(float, dimensions = (antnr,blocksize))

acm = zeros((antnr,antnr,len(freliste)),dtype = complex)

print "\n\nNumber of input files:    %i" % (count)
print "Blocksize:                %i" % (blocksize/wide)
print "Startblock:               %i " % (startblock)
print "Startfrequency:           %6.2f MHz" % (startfreq)
print "Endfrequency:             %6.2f MHz" % (endfreq)
print "------------------------------------------------"

print "\nSTART CORRELATION\n"

threadmain = main()
threadmain.start()
threadmain.join()

del fftdata, fftdatan, data, h
if var:
    del BSliste

##------------sorting of the correlation matrix------------------
#
#corrm = zeros((antnr,antnr,len(freliste)),dtype = complex)
#   
#i1 = 0
#for rcu1 in rcun:
#   i2 = 0
#   for rcu2 in rcun:
#       corrm[rcu1][rcu2] = (acm[i1][i2]).conjugate()
#       i2 += 1
#   i1 += 1
#
#del acm
#
#print "\n - ready - \n"
corrm = acm.conjugate()

#-------------write correlation matrix in a file-----------------

print "Start writing \n"
u = int(round(len(corrm[0][0])/1000))
print u

of = open(outfile+".pickle",'w')
antposp = md.convertITRFToLocal(cr.hArray(antposp)).toNumpy()
pickle.dump(antposp,of,1)
pickle.dump(timedate,of,1)
pickle.dump(timeutc,of,1)
pickle.dump(timetuple,of,1)
pickle.dump(stationname, of,1)
pickle.dump(freliste,of,1)
pickle.dump(u,of,1)

np.save(outfile+".npy", corrm)

#if u == 5:
#   u = 10
#
#if u == 0:
#   pickle.dump(corrm,of,1)
#else:
#u = 8
nant = corrm.shape[0]
#print corrm.shape
#u = 4
#if u == 0:
#   pickle.dump(corrm,of,1)
#else:
#    for uc in range(0,u,1):
#        pickle.dump(corrm[uc*nant/u:(uc+1)*nant/u],of,1)

print "data sucessfully written to file:", outfile + ".pickle", "and", outfile + ".npy"

#outfile = ("acm_"+str(nblocks)+"_2048.out")

end = time.time()

print "\nTime:   ", round((end-start)/60,2), "min,   ", round(end-start), " sec"


#-------------code for reading in the corrmatrix-----------------

"""
    
    dataset = open(dataset)
    timedate = pickle.load(dataset)
    timeutc = pickle.load(dataset)
    timetuple = pickle.load(dataset)
    station = pickle.load(dataset)
    freliste = pickle.load(dataset)
    u = pickle.load(dataset)
    
    data = pickle.load(dataset)
    if u != 0:
    for uc in range(0,u-1,1):
    data2 = pickle.load(dataset)
    data = vstack((data,data2))
    del data2
    
    #data as numpy array with [ # rcus, # rcus, # channels]
    
    print "\n File informations \n"
    print "Observing Time      ", timedate
    print "Obs Time in UTC     ", timeutc
    print "Station             ", station
    print "Startfrequency      ", round(freliste[0],2),"MHz"
    print "Endfrequency        ", round(freliste[-1],2),"MHz"
    print "Number of channels  ", len(freliste), "\n"
    
    """

