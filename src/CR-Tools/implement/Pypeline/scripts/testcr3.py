"""
Test beamforming and RFI excision for CR event

Run with
execfile(PYP+"/scripts/testcr3.py")

"""
dataset="LOPES"       
dataset="LOFAR"       

def makeGrid(AZ,EL,Distance,offset=5*deg):
    return [dict(az=AZ-5, el=EL+5,r=Distance),dict(az=AZ, el=EL+5,r=Distance),dict(az=AZ+5, el=EL+5,r=Distance),
            dict(az=AZ-5, el=EL,r=Distance),dict(az=AZ, el=EL,r=Distance),dict(az=AZ+5, el=EL,r=Distance),
            dict(az=AZ-5, el=EL-5,r=Distance),dict(az=AZ, el=EL-5,r=Distance),dict(az=AZ+5, el=EL-5,r=Distance)]

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

pardict=dict(
    doplot=True,
    qualitycheck=True,
    spikeexcess=100, # after all we are looking for spikes
    randomize_peaks=False,
    FarField=False,
    BeamFormer=dict(
            filefilter="$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5",
            pointings=makeGrid(143.4092*deg, 81.7932*deg, 600.3),
            nantennas_stride=2,
            nantennas_start=1
        ) if dataset=="LOFAR" else dict(
            filefilter="$LOFARSOFT/data/lopes/2004.01.12.00:28:11.577.event",
            pointings=makeGrid(41.9898208*deg,64.70544*deg,1750),
            cal_delays=dict(zip(Vector(int, 8, fill=[10101,10102,10201,10202,20101,20102,20201,20202]),[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08])),
            phase_center=[-22.1927,15.3167,0],
            NyquistZone=2
        ),
    FitBaseline=dict(
        ncoeffs=20,
        numax=75,
        numin=45,
        fittype="BSPLINE",
        splineorder=3),
    ApplyBaseline=dict(
        rmsfactor=3.5,
        doplot=False
        )
    )

plt.hanging=False # Use true if exectuion hangs after plotting one window
plt.EDP64bug=True # use True if your system crashes for plotting semilog axes

#print "---> BeamFormer"
bf=tasks.beamformer.BeamFormer(); bf(pardict=pardict); # BeamForm
bf.avspec.par.xvalues.setUnit("M","")

"""
#print "---> Start Fitbaseline"
fitb=tasks.fitbaseline.FitBaseline(); fitb(bf.avspec,pardict=pardict);

#Calculate a smooth version of the spectrum which is later used to set amplitudes
calcb=tasks.fitbaseline.CalcBaseline(); calcb(bf.avspec,pardict=pardict,invert=False,HanningUp=False,normalize=False);
amplitudes=hArray(copy=calcb.baseline)

#Now calculate it again, but now to flatten the spectrum
calcb=tasks.fitbaseline.CalcBaseline()(bf.avspec,pardict=pardict);

#... and to find channels with RFI spikes
apply_baseline=tasks.fitbaseline.ApplyBaseline()(bf.avspec,pardict=pardict)

#Now we apply them to the beam-formed spectrum
bf.beams[0,...].randomizephase(apply_baseline.dirty_channels[...,[0]:apply_baseline.ndirty_channels.vec()],amplitudes[...])

"""
#And finally we make an inverse fft and plot the different beams (inv FFT is done by the plotting routine or by bf.tcalc)
if dataset=="LOPES": bf.tplot(doabs=True,smooth=9,xlim=(-3.5,0),ylim=(0,150))
if dataset=="LOFAR": bf.tplot(doabs=True,smooth=9,xlim=(-2,0),ylim=(0,80))

