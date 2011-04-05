"""
Test beamforming and RFI excision for CR event

Run with
execfile(PYP+"/scripts/testcr3.py")

"""

file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")

def makeGrid(AZ,EL,Distance,offset=5*deg):
    return [dict(az=AZ-5, el=EL+5,r=Distance),dict(az=AZ, el=EL+5,r=Distance),dict(az=AZ+5, el=EL+5,r=Distance),
            dict(az=AZ-5, el=EL,r=Distance),dict(az=AZ, el=EL,r=Distance),dict(az=AZ+5, el=EL,r=Distance),
            dict(az=AZ-5, el=EL-5,r=Distance),dict(az=AZ, el=EL-5,r=Distance),dict(az=AZ+5, el=EL-5,r=Distance)]
    
pardict=dict(
    doplot=False,
    qualitycheck=False,
    BeamFormer=dict(
        filefilter="$LOFARSOFT/data/lopes/2004.01.12.00:28:11.577.event",
        antenna_positions=dict(map(lambda x: (x[0],x[1].array()),zip(file["antennaIDs"],file.getCalData("Position")))),
        pointings=makeGrid(41.9898208*deg,64.70544*deg,1750),
        cal_delays=dict(zip(file["antennaIDs"],[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08])),
        phase_center=[-22.1927,15.3167,0],
        FarField=False,
        NyquistZone=2,
        randomize_peaks=False),
    FitBaseline=dict(
        ncoeffs=20,
        numax=75,
        numin=45,
        fittype="BSPLINE",
        splineorder=3),
    ApplyBaseline=dict(
        rmsfactor=3.5,
        doplot=True
        )
    )

plt.hanging=True

#print "---> BeamFormer"
bf=tasks.beamformer.BeamFormer(); bf(pardict=pardict); # BeamForm
bf.avspec.par.xvalues.setUnit("M","")

#print "---> Start Fitbaseline"
fitb=tasks.fitbaseline.FitBaseline()(bf.avspec,pardict=pardict);

#Calculate a smooth version of the spectrum which is later used to set amplitudes
calcb=tasks.fitbaseline.CalcBaseline()(bf.avspec,pardict=pardict,invert=False,HanningUp=False,normalize=False);
amplitudes=hArray(copy=calcb.baseline)

#Now calculate it again, but now to flatten the spectrum
calcb=tasks.fitbaseline.CalcBaseline()(bf.avspec,pardict=pardict);

#... and to find channels with RFI spikes
apply_baseline=tasks.fitbaseline.ApplyBaseline()(bf.avspec,pardict=pardict)

#Now we apply them to the beam-formed spectrum
bf.beams[0,...].randomizephase(apply_baseline.dirty_channels[...,[0]:apply_baseline.ndirty_channels.vec()],amplitudes[...])

#And finally we make an inverse fft and plot the different beams (inv FFT is done by the plotting routine or by bf.tcalc)
bf.tplot(doabs=True,smooth=9,xlim=(-3.5,0),ylim=(0,150))

