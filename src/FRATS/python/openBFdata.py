# import libraries, either locally or from global installation
# bfdata reads in beamformed LOFAR data
# dedispersion does incoherent dedispersion on arrays or fitsimages
try:
    import bfdata as bf
    import dedispersion
except:
    from pycrtools import bfdata as bf
    from pycrtools import dedispersion
import numpy as np
import sys
print sys.argv
obsid='L54717'
#obsid='L39700'
cutlevel=5
parsetdir='/vol/astro/lofar/frats/bf/parsets/'
datadir='/vol/astro/lofar/frats/bf/'
if len(sys.argv)>1:
    obsid=sys.argv[1]
elif len(sys.argv)>3:
    parsetdir=sys.argv[2]
    datadir=sys.argv[3]
else:
    usage sys.argv[0] <obsid> [<parsetdir>] [<datadir>]
    quit()
obsids=[obsid]
block=0
chunksize=1

obsid=obsids[0]
blocksize=192 #dr.samples
block=34
fileindex=2
# open a datareader
dr=bf.BFDataReader(parsetdir+'/'+obsid+'.parset',datadir,True)
# need to fix some issues, but we circumfent them for MSSS bf data by setting:
dr.nrsubbands=80
dr.par['nrsubbands']=80
dr.h5=True
totCh=dr.par['nrsubbands']*dr.par['channels']
dr.setDatatype('coherentstokes')
# set the blocksize
dr.setblocksize(blocksize)
mutech={}
f1=1500
f2=3000
import matplotlib.pyplot as plt
import pickle
title=''
print block
# read a block of data
data=dr.read(block)[fileindex]
# divide the data in chunks
data1=data.reshape((dr.samples/chunksize,chunksize,totCh))
# sum over the chunks?
subspec=np.sum(data1,axis=1)
# power of frequency spectrum
powerspec=np.sum(subspec,axis=0)
std=np.std(subspec,axis=0)
# determine which channels to flag
# The flagging itself can be done by setting 
stdNorm=std/powerspec
stdNormSort=np.copy(stdNorm)
stdNormSort.sort()
Lstd=len(stdNormSort)
av=np.average(stdNormSort[int(0.1*Lstd):int(0.9*Lstd)])
std1=np.std(stdNormSort[int(0.1*Lstd):int(0.9*Lstd)])
index=np.arange(0,Lstd)
muteindex=index[stdNorm>av+cutlevel*std1]
# flag data
data[:,muteindex]=np.average(data)



# plot dynamic spectrum and collapsed spectrum
def myplot(data):
    plt.subplot(221)
    plt.imshow(data[:,0,:],aspect='auto')
    plt.subplot(222)
    plt.plot(np.sum(data[:,0,:],axis=1))
    plt.subplot(223)
    plt.plot(np.sum(data[:,0,:],axis=0))
    plt.subplot(224)
    plt.imshow(np.log(data[:,0,:]),aspect='auto')

# reshape data into streams
data3=data.reshape(dr.samples,nrstreams,totCh/nrstreams)
# calculate frequencies for each stream
freqs=dr.par['frequencies']
freqs=freqs[0:totCh].reshape(nrstreams,totCh/nrstreams)
stream=5
# dedisperse data for the specified stream
dd=dedispersion.dedisperse_array(np.transpose(data3[:,stream,:]),12.455,freqs[0],dr.par['timeresolution'])


#mutelist=[]
##for k,v in mutech.iteritems():
#    mutelist.extend(v)
#myhist=plt.hist(mutelist,3904,[0,3903],histtype='bar',edgecolor='none')
#noncomplete=[int(b)+1 for a,b in zip(myhist[0],myhist[1]) if 0.5<a<5.5]
#complete=[int(b)+1 for a,b in zip(myhist[0],myhist[1]) if a==6]
#nc=[(n,[k for k,v in mutech.iteritems() if n in v]) for n in noncomplete]
#
