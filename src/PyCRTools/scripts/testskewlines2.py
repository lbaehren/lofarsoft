#execfile(PYP+'scripts/testskewlines2.py')

file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
file["ANTENNA_SET"]="LBA_OUTER"
file["BLOCKSIZE"]=2**17

evenantennas=["017000000","017000003","017000004","017000006","017001008","017001011","017001013","017001014","017002016","017002018","017002021","017002022","017003024","017003027","017003028","017003030","017004032","017004034","017004036","017004038","017005040","017005042","017005044","017005046","017006048","017006050","017006052","017006054","017007056","017007058","017007060","017007062","017008064","017008067","017008068","017008070","017009072","017009074","017009076","017009078","017010080","017010082","017010084","017010086","017011088","017011090","017011092","017011094"]

oddantennas=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

file["SELECTED_DIPOLES"]=oddantennas
timeseries_data=file["TIMESERIES_DATA"]
positions=file["ANTENNA_POSITIONS"]
positionsT=hArray_transpose(positions)

#First determine where the pulse is in a simple incoherent sum of all time series data

# trun loads the task LocatePulseTrain - see $LOFARSOFT/src/CR-Tools/implement/Pypeline/modules/tasks/pulsecal.py
# rf is a module that contains a few helper functions for radio interferometry
pulse=trun("LocatePulseTrain",timeseries_data,nsigma=7,maxgap=3)

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

N=48
NT=N*(N-1)*(N-2)/6 # Number of Triangles ....

triangle_distances=direction.centers[...,0:3].vectorlength(direction.meancenter)
triangle_distances_mean=triangle_distances.mean()
triangle_distances_stddev=triangle_distances.stddev()
triangle_distances_max=triangle_distances.max()

triangle_subarray_radius=(triangle_distances_max-triangle_distances_mean)/2

NSubArrays=N/2
azelr=hArray(float,[NSubArrays,3])
xyz=hArray(float,[NSubArrays,3])
azelr.fillrangevec(hArray([0.0,0.0,triangle_distances_max]),hArray([2.*pi/NSubArrays,0.0,0.]))
hCoordinateConvert(azelr[...],CoordinateTypes.AzElRadius,xyz[...],CoordinateTypes.Cartesian,False)
xyz+=direction.meancenter

indx=hArray(int,[NSubArrays,NT])
triangle_separations=hArray(float,[NSubArrays,NT])
triangle_separations[...].vectorseparation(xyz[...],direction.centers)

#Find the triangles that are close to the respective grid point on the circle
ntriangles_subarray=indx[...].findlessthan(triangle_separations[...],triangle_subarray_radius)

scrt=hArray(float,[NSubArrays,ntriangles_subarray.max(),3])
subdirections=hArray(float,[NSubArrays,3])
suborigins=hArray(float,[NSubArrays,3])

scrt[...].copyvec(direction.centers,indx[...],ntriangles_subarray,Vector([3]))
suborigins[...].mean(scrt[...,[0]:ntriangles_subarray])
suboriginsT=suborigins.Transpose()

scrt[...].copyvec(direction.directions,indx[...],ntriangles_subarray,Vector([3]))
subdirections[...].mean(scrt[...,[0]:ntriangles_subarray])
subdirections[...] /= subdirections[...].vectorlength()

subpoints2=hArray(copy=subdirections)
subpoints2 *= 1000.
subpoints2 += suborigins

meanpoint2=hArray(copy=direction.meandirection)
meanpoint2 *= 1000
meanpoint2 += direction.meancenter

import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot([direction.meancenter[0],meanpoint2[0]],[direction.meancenter[1],meanpoint2[1]],[direction.meancenter[2],meanpoint2[2]], linewidth=5,label="Mean Direction")
for i in range(NSubArrays):
    ax.plot([suborigins[i,0],subpoints2[i,0]],[suborigins[i,1],subpoints2[i,1]],[suborigins[i,2],subpoints2[i,2]], label="Sub-Array "+str(i),linewidth=1)

ax.plot(positionsT[0].vec(),positionsT[1].vec(),positionsT[2].vec(),marker='x',linestyle='',label="Antennas")
ax.plot(suboriginsT[0].vec(),suboriginsT[1].vec(),suboriginsT[2].vec(),marker='o',linestyle='',label="Sub-Arrays")
ax.legend()


subdistances=hArray(float,[NSubArrays])
subbaselines=hArray(float,[NSubArrays])

hSkewLinesDistanceToClosestApproach(subdistances,subbaselines,direction.meancenter,direction.meandirection,suborigins,subdirections,10000.);
positive_distances=subdistances.Select(">=",0)
print "Distance estimate = ",positive_distances.mean().val(),"+/-",positive_distances.stddev().val()
print "Distances of sub-arrays =",subdistances

"""

distances=hArray(float,[NT])
baselines=hArray(float,[NT])

hSkewLinesDistanceToClosestApproach(distances,baselines,meanorigin,meandirection,origins,directions,10000.);

indx2=hArray(int,[NT])
ngood2=indx2.findgreaterthan(error_distance,25).val()
scrt2=hArray(float,[ngood2])
scrt2.copy(distance,indx,ngood2)


distance.abs()
#plt.plot(error_distance,distance,linestyle='None',marker='+',legend=None)

import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot([0,1],[0,1],[0,1], label='parametric curve')
ax.plot([0,1],[0,-11],[0,-1], label='parametric curve')
ax.plot(positionsT[0].vec(),positionsT[1].vec(),positionsT[2].vec(),marker='x',linestyle='')
#ax.plot(positionsT[0].vec(),positionsT[1].vec(),positionsT[2].vec())



import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
#import numpy as np
#import matplotlib.pyplot as plt

mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
ax = fig.gca(projection='3d')
theta = np.linspace(-4 * np.pi, 4 * np.pi, 100)
z = np.linspace(-2, 2, 100)
r = z**2 + 1
x = r * np.sin(theta)
y = r * np.cos(theta)
#ax.plot(x, y, z, label='parametric curve')
ax.plot([0,1],[0,1],[0,1], label='parametric curve')
ax.plot([0,1],[0,-11],[0,-1], label='parametric curve')
ax.legend()
"""
