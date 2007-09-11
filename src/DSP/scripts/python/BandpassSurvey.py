import os, pickle
from numpy import *
from MeasurementSet import *
from pylab import *

def compare_func(x, y): 
    if len(x) > len(y): return 1
    if len(x) < len(y): return -1
    return cmp(x, y)


def CreatePickles(msdir = '/raid/data/renting/L2007_03736'):
    """Create pickles with averaged bandpasses for each autocorrelation XX"""
    print 'Using directory: ' + os.getcwd()
    mslist = os.listdir(msdir)
    for ms in mslist:
        print 'Procseesing: ' + ms
        temp = MeasurementSet(msdir+ '/' + ms)
        Mdata = zeros((temp.antennaCount, 256))
        for baseline in temp.baselines:
            if baseline[0] == baseline[1]:
                print baseline
                data = temp.getBaselineData(baseline)
                m = data.mean(axis=2, dtype=float64)
                Mdata[baseline[0] -1] = m[0]
        f = open( ms + '.pickle', 'w')
        print 'writing pickle for: ' + ms
        pickle.dump(Mdata, f)

def ReadPickles(numAntennas, pdir='.'):
    plist = os.listdir(pdir)
    plist.sort(compare_func)
    bandpasses = zeros((len(plist), numAntennas, 256), dtype='float64')
    for p in plist:
        f = open(pdir + '/' + p)
        data = pickle.load(f)
        bandpasses[plist.index(p)] = data
    return bandpasses

def PlotSPW(data,YM=0.4):
    SPWcount = data.shape[0]
    nchan = data.shape[2]
    figure()
    i = 1
    while i <= SPWcount:
        subplot(6, SPWcount / 6, i)
        m = data.mean(axis=1)[i-1, :]
        s = data.std(axis=1)[i-1, :]
        a = arange(0, nchan)
        plot(a, m-s, 'r', a, m,'g',  a, m+s, 'r')
        axis(xmax = nchan, ymax = YM)
        xticks([])
        title('SB' + str(i-1), fontsize='small')
        i += 1

def PlotAntenna(data, YM= 2):
    ANTcount = data.shape[1]
    nchan = data.shape[2]
    figure()
    i = 1
    while i <= ANTcount:
        subplot(4, ANTcount / 4, i)
        m = data.mean(axis=0)[i-1, :]
        s = data.std(axis=0)[i-1, :]
        a = arange(0, nchan)
        plot(a, m,'g',  a, m+s, 'r')
        axis(xmax = nchan, ymax = YM)
        xticks([])
        title('ANT' + str(i-1), fontsize='small')
        i += 1

def Contort(data):
    s = data[:,:,50].mean(axis=1)
    a = swapaxes(data, 0, 2)
    A = a /s
    return swapaxes(A, 0, 2)

    

