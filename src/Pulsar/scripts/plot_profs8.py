import bestprof
import Pgplot
import ppgplot
import numpy
import psr_utils
import glob

NBINS = 256
COLOUR = 2
ii = 0
jump = 0
RSP = ["RSP0","RSP1","RSP2","RSP3","RSP4","RSP5","RSP6","RSP7"]

files = glob.glob("*/*/*pfd.bestprof")
files.sort()

for best in files:
    b = bestprof.bestprof(best)
    prof = b.profile
    prof_rot = psr_utils.rotate(prof,numpy.argmax(b.profile)+NBINS/2)
    prof = prof-numpy.mean(prof_rot[:int(NBINS*0.8)])
    prof = prof/numpy.std(prof_rot[:int(NBINS*0.8)])
    prof_rot = prof_rot-numpy.mean(prof_rot[:int(NBINS*0.8)])
    prof_rot = prof_rot/numpy.std(prof_rot[:int(NBINS*0.8)])
    prof_rot = prof_rot/numpy.max(prof_rot)
    peaksnr = numpy.max(prof_rot)
    print best, "PEAK SNR", peaksnr
    print best, "SNR", numpy.sum(prof_rot)
    Pgplot.plotxy(prof_rot[int(NBINS*0.25):int(NBINS*0.75)]+jump,x=numpy.arange(0,0.5,1./(NBINS)),rangey=[-0.25,8.25],labx="Rotational Phase",laby="Pulse Intensity (arbitrary units)",width=1,color=COLOUR,device="profiles.ps/CPS")
    Pgplot.ppgplot.pgtext(0.1,0.1+jump,RSP[ii])
    COLOUR += 1
    jump += 1
    ii += 1
Pgplot.closeplot()
