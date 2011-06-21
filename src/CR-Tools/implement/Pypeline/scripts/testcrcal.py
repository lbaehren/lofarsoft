#
#$PYP/scripts/fix_metadata.py --antenna_set="LBA_OUTER" --nyquist_zone=1 oneshot_level4_CS017_19okt_no-9.h5 

#execfile(PYP+'scripts/testcrcal.py')

file=open("/Users/falcke/LOFAR/work/CR/LORAweekdump--2-15.h5")
file["ANTENNA_SET"]="LBA_OUTER"
file["BLOCKSIZE"]=2**16
file["BLOCK"]=93

file["SELECTED_DIPOLES"]="odd"
file["SELECTED_DIPOLES"]=file["SELECTED_DIPOLES"][0:-3] # Take out last two empty antennas

timeseries_data=file["TIMESERIES_DATA"]
positions=file["ANTENNA_POSITIONS"]

#First determine where the pulse is in a simple incoherent sum of all time series data

# trun loads the task LocatePulseTrain - see $LOFARSOFT/src/CR-Tools/implement/Pypeline/modules/tasks/pulsecal.py
# rf is a module that contains a few helper functions for radio interferometry
pulse=trun("LocatePulseTrain",timeseries_data,nsigma=7,maxgap=3)

if pulse.npeaks <= 0:
    raise ValueError("No pulse found!")

#Normalize the data which was cut around the main pulse
pulse.timeseries_data_cut[...]-=pulse.timeseries_data_cut[...].mean()
pulse.timeseries_data_cut[...]/=pulse.timeseries_data_cut[...].stddev(0)

#pulse.timeseries_data_cut[0:2,...].plot()

#Cross correlate all pulses with each other 
crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=10)

## Have the suspicion that CrossCorrelateAntennas caused a seg fault when ending Python

#And determine the relative offsets between them
mx=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=5*10**-9/crosscorr.oversamplefactor)#,peak_width=6,splineorder=2)

print "Time lag [ns]: ", mx.lags 
print " "

#Time lag [ns]:  Vector(float, 45, fill=[0,-4.40625e-09,-3.03125e-09,1.75e-09,5e-09,3.125e-09,-7.90625e-09,-1.16875e-08,-8.28125e-09,-7.5625e-09,-8.96875e-09,-2.90625e-09,8e-09,9.96875e-09,4.375e-10,-5.71875e-09,-1.03125e-09,-1.275e-08,-1.38125e-08,-1.725e-08,-1.54375e-08,-1.12812e-08,-1.02187e-08,-1.5625e-10,6.65625e-09,1.6875e-08,1.57813e-08,5.46875e-09,-3.75e-10,-1.02812e-08,-1.525e-08,-1.30937e-08,-2.00937e-08,-2.00937e-08,-2.025e-08,-1.59062e-08,-1.70312e-08,-1.16562e-08,-3.34375e-09,6.90625e-09,1.75e-08,2.1125e-08,2.50313e-08,1.75938e-08,9.96875e-09])

t0=time.clock()

#Now fit the direction and iterate over cable delays to get a stable solution
direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=9,verbose=True,doplot=True)
#direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=3)
print "Time for fitting:",time.clock()-t0

print "========================================================================"
print "Fit Arthur Az/El   ->  xxx deg xxx deg"
print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
print " "
print "Delays =",direction.delays * 1e9

#cal_delays=dict(zip(file["antennaIDs"],[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08])),

#bf=trun("BeamFormer2",data=pulse.timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],maxnantennas=46,antpos=file["ANTENNA_POSITIONS"],FarField=True,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,))),cable_delays=direction.delays,calc_timeseries=True,doplot=2,plotspec=False,verbose=False)


"""
#Even:

#Odd:

"""
