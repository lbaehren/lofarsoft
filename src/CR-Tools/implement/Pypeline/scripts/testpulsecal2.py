#
#from scipy.optimize import fmin

#$PYP/scripts/fix_metadata.py --antenna_set="LBA_OUTER" --nyquist_zone=1 oneshot_level4_CS017_19okt_no-9.h5 
#execfile(PYP+'scripts/testpulsecal2.py')
file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
file["ANTENNA_SET"]="LBA_OUTER"
file["BLOCKSIZE"]=2**17

evenantennas=["017000000","017000003","017000004","017000006","017001008","017001011","017001013","017001014","017002016","017002018","017002021","017002022","017003024","017003027","017003028","017003030","017004032","017004034","017004036","017004038","017005040","017005042","017005044","017005046","017006048","017006050","017006052","017006054","017007056","017007058","017007060","017007062","017008064","017008067","017008068","017008070","017009072","017009074","017009076","017009078","017010080","017010082","017010084","017010086","017011088","017011090","017011092","017011094"]

oddantennas=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

#file["SELECTED_DIPOLES"]=oddantennas
file["SELECTED_DIPOLES"]=evenantennas
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

#Cross correlate all pulses with each other 
crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=5)

#And determine the relative offsets between them
mx=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=10**-9,peak_width=6,splineorder=2)

print "Time lag [ns]: ", mx.lags 
print " "

#Now fit the direction and iterate over cable delays to get a stable solution
direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=10,verbose=True,doplot=True)

print "========================================================================"
print "Fit Arthur Az/El   ->  143.409 deg 81.7932 deg"
print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
print " "
print "Delays =",direction.delays * 1e9

"""
#Even:
Number of iterations used: Task.niter = 9
 ========================================================================
Fit Arthur Az/El   ->  143.409 deg 81.7932 deg
Triangle Fit Az/EL ->  (145.17844721833896, 81.973693266380721) deg
 
Delays = hArray(float, [48L], fill=[0,-0.019902,-0.286907,-2.67741,-2.44819,0.0596772,0.106266,0.358653,-0.849451,-1.14802,-1.23919,-1.70758,0.33844,0.697573,-0.0561886,-0.148015,-7.40922,-2.40301,-1.72062,0.773882,0.686808,-0.540144,-0.840565,-0.569617,-0.807875,-0.410008,0.811708,0.262664,1.02071,-1.43275,-1.97364,-1.74968,-7.02061,0.754395,0.735552,-0.165855,-5.2064,-0.661238,-0.603647,-0.37918,-5.03916,-0.0298481,1.48213,-1.5103,-6.122,0.545559,1.08328,-0.807854]) # len=48 slice=[0:48])

#Odd:
Number of iterations used: Task.niter = 8
========================================================================
Fit Arthur Az/El   ->  143.409 deg 81.7932 deg
Triangle Fit Az/EL ->  (144.1118392216996, 81.84042919170588) deg
 
Delays = hArray(float, [48L], fill=[0,0.102678,1.62956,-1.53215,-1.32714,1.4279,1.8293,0.953364,-0.166563,-0.486748,-0.547745,-0.524701,0.0764327,0.675711,2.05372,1.87189,-5.78966,-0.543436,-1.13814,2.4613,1.54497,0.432689,0.0724031,-0.276677,-0.114464,-0.0659973,0.775349,1.30278,2.53077,-0.855829,-0.44727,-0.745051,-5.17407,2.15853,1.54939,0.342055,-4.72154,0.059357,-0.33143,-0.616612,-5.14568,0.369244,2.17518,-0.965348,-5.74457,2.19715,2.74123,0.172005]) # len=48 slice=[0:48])

"""
