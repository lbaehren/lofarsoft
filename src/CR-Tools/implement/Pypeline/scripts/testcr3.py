"""
Test beamforming and RFI excision for CR event

Run with
execfile(PYP+"/scripts/testcr3.py")
execfile('/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testcr3.py')
"""
from pycrtools import *

"""
For LOPES file:
    file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
    cal_delays=dict(zip(file["antennaIDs"],[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08])),
    cal_delays are file["Delays"] plus 'Horneffer Delays', i.e. those that came out of the calibration pipeline, which we don't have yet.

For LOFAR file:
/mnt/lofar/tbb-CR/CS017_testdataset_19okt2010/oneshot_level4_CS017_19okt_no-9.h5

Beamformed at (az, el, R):

(143.4092, 81.7932, 600.3) odd
(144.5977, 81.9986, 836.3) even
"""
dataset="LOPES"       
dataset="LOFAR"       

pardict=dict(
    doplot=False,
    qualitycheck=False,
    spikeexcess=100, # after all we are looking for spikes
    randomize_peaks=False,
    FarField=False,
#    plot_center=0.50221,plotlen=32,
    BeamFormer=dict(
            filefilter="$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5",
            pointings=rf.makeAZELRDictGrid(143.4092*deg, 81.7932*deg, 600.3),
            blocklen=2**17,
            nantennas_stride=2,
            nantennas_start=1
        ) if dataset=="LOFAR" else dict(
            filefilter="$LOFARSOFT/data/lopes/2004.01.12.00:28:11.577.event",
            pointings=rf.makeAZELRDictGrid(41.9898208*deg,64.70544*deg,1750),
            cal_delays=dict(zip(Vector(int, 8, fill=[10101,10102,10201,10202,20101,20102,20201,20202]),[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08])),
            phase_center=[-22.1927,15.3167,0],
            NyquistZone=2
        ),
    FitBaseline=dict(
        ncoeffs=20,
        numin=30,
        numax=80,
        fittype="BSPLINE",
        splineorder=3) if dataset=="LOFAR" else dict(
        ncoeffs=20,
        numin=45,
        numax=75,
        fittype="BSPLINE",
        splineorder=3),
    ApplyBaseline=dict(
        rmsfactor=5,
        plotchannel=5 if dataset=="LOFAR" else 0,
        doplot=True
        )
    )


plt.hanging=False # Use true if exectuion hangs after plotting one window
plt.EDP64bug=True # use True if your system crashes for plotting semilog axes

print "---> BeamFormer"
bf=tasks.beamformer.BeamFormer();

###The following block can be commented out
bf(pardict=pardict); # BeamForm
bf.avspec.par.xvalues.setUnit("M","")
avspec=bf.avspec
bm=bf.beams

### and be replaced by this block once the average spectrum has been calculated (and written to disk)
#One can also read the stored data back from file and proceed without re-calculating the spectrum ...
#bm=hArrayRead('oneshot_level4_CS017_19okt_no-9.h5.beams.pcr')
#bm.par.avspec.par.xvalues.setUnit("M","")
#avspec=bm.par.avspec



#print "---> Start Fitbaseline"
fitb=tasks.fitbaseline.FitBaseline(); fitb(avspec,pardict=pardict)

#Calculate a smooth version of the spectrum which is later used to set amplitudes
calcb=tasks.fitbaseline.CalcBaseline(); calcb(avspec,pardict=pardict,invert=False,HanningUp=False,normalize=False);
amplitudes=hArray(copy=calcb.baseline)
amplitudes.sqrt()

#Now calculate it again, but now to flatten the spectrum
calcb=tasks.fitbaseline.CalcBaseline()(avspec,pardict=pardict);

#... and to find channels with RFI spikes
apply_baseline=tasks.fitbaseline.ApplyBaseline()(avspec,pardict=pardict)

#Now we apply them to the beam-formed spectrum
bm[0,...].randomizephase(apply_baseline.dirty_channels[...,[0]:apply_baseline.ndirty_channels.vec()],amplitudes[...])

#And finally we make an inverse fft and plot the different beams (inv FFT is done by the plotting routine or by bf.tcalc)
if dataset=="LOPES": bf.tplot(bm,doabs=True,smooth=9,xlim=(-3.5,0))
if dataset=="LOFAR": bf.tplot(bm,doabs=True,smooth=9,xlim=(0.5,2.5))


"""
spec=hArray(float,properties=bf.beams)
spec[0,...].spectralpower(bf.beams[0,...])
spec[0,...].runningaverage(2**7,hWEIGHTS.GAUSSIAN)
spec[0,4] /= spec[0,0]
spec[0,4].plot()

bf.tbeams[0,4].plot();bf.tbeam_incoherent.plot(clf=False,xvalues=bf.tbeams.par.xvalues)


"""
