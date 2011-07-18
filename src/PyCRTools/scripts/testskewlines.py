#execfile(PYP+'scripts/testskewlines.py')

file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
file["ANTENNA_SET"]="LBA_OUTER"
file["BLOCKSIZE"]=2**17
oddantennas=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]

file["SELECTED_DIPOLES"]=oddantennas
positions=file["ANTENNA_POSITIONS"]
timeseries_data=file["TIMESERIES_DATA"]
(start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
start-=16; end=start+int(2**ceil(log(end-start,2)));  
timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])

timeseries_data_cut[...].copy(timeseries_data[...,start:end])
timeseries_data_cut.abs()
timeseries_data_cut[...].runningaverage(7,hWEIGHTS.GAUSSIAN)
mx=task("FitMaxima")(timeseries_data_cut,doplot=True,refant=0,sampleinterval=5,plotend=48,peak_width=10,legend=file["SELECTED_DIPOLES"])

mx.lags *= 10**-9

N=48
NT=N*(N-1)*(N-2)/6 # Number of Triangles ....
directions=hArray(float,[NT,3])
er=hArray(float,[NT])
weights=hArray(float,[NT])
origins=hArray(float,[NT,3])
hDirectionTriangulationsCartesian(directions,er,origins,weights,positions[-N:],hArray(mx.lags[-N:]),+1);

positionsT=hArray_transpose(positions)
indx=hArray(int,[NT])
ngood=indx.findlessthan(er,1e-10).val()

scrt=hArray(float,[ngood,3])
scrt.copyvec(directions,indx,ngood,3)
meandirection=hArray(float,[3])
meandirection.mean(scrt)
meandirection /= meandirection.vectorlength().val()

scrt.copyvec(origins,indx,ngood,3)
meanorigin=hArray(float,[3])
meanorigin.mean(scrt)

#meandirection - > hArray(float, [3L], fill=[0.0824118,-0.111287,0.990365]) # len=3 slice=[0:3])
#pytmf.spherical2cartesian(1, pi2-81.7932*deg,pi2-143.4092*deg) -> (0.085090553169460376, -0.11461297030462134, 0.98975929639446536)

sp=pytmf.cartesian2spherical(meandirection[0],meandirection[1],meandirection[2]); azel=(180-(sp[2]+pi2)/deg,90-(sp[1])/deg)
# azel -> (143.47870717075114, 82.040161833968313)

triangle_distances=origins[...,0:3].vectorlength(meanorigin)
triangle_distances_mean=triangle_distances.mean()
triangle_distances_stddev=triangle_distances.stddev()
triangle_distances_max=triangle_distances.max()

triangle_subarray_radius=(triangle_distances_max-triangle_distances_mean)/2

NSubArrays=N/2
azelr=hArray(float,[NSubArrays,3])
xyz=hArray(float,[NSubArrays,3])
azelr.fillrangevec(hArray([0.0,0.0,triangle_distances_max]),hArray([2.*pi/NSubArrays,0.0,0.]))
hCoordinateConvert(azelr[...],CoordinateTypes.AzElRadius,xyz[...],CoordinateTypes.Cartesian,False)
xyz+=meanorigin

indx=hArray(int,[NSubArrays,NT])
triangle_separations=hArray(float,[NSubArrays,NT])
triangle_separations[...].vectorseparation(xyz[...],origins)

#Find the triangles that are close to the respective grid point on the circle
ntriangles_subarray=indx[...].findlessthan(triangle_separations[...],triangle_subarray_radius)

scrt=hArray(float,[NSubArrays,ntriangles_subarray.max(),3])
subdirections=hArray(float,[NSubArrays,3])
suborigins=hArray(float,[NSubArrays,3])

scrt[...].copyvec(origins,indx[...],ntriangles_subarray,Vector([3]))
suborigins[...].mean(scrt[...,[0]:ntriangles_subarray])
suboriginsT=suborigins.Transpose()

scrt[...].copyvec(directions,indx[...],ntriangles_subarray,Vector([3]))
subdirections[...].mean(scrt[...,[0]:ntriangles_subarray])
subdirections[...] /= subdirections[...].vectorlength()

subpoints2=hArray(copy=subdirections)
subpoints2 *= 1000.
subpoints2 += suborigins

meanpoint2=hArray(copy=meandirection)
meanpoint2 *= 1000
meanpoint2 += meanorigin

import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot([meanorigin[0],meanpoint2[0]],[meanorigin[1],meanpoint2[1]],[meanorigin[2],meanpoint2[2]], linewidth=5,label="Mean Direction")
for i in range(NSubArrays):
    ax.plot([suborigins[i,0],subpoints2[i,0]],[suborigins[i,1],subpoints2[i,1]],[suborigins[i,2],subpoints2[i,2]], label="Sub-Array "+str(i),linewidth=1)

ax.plot(positionsT[0].vec(),positionsT[1].vec(),positionsT[2].vec(),marker='x',linestyle='',label="Antennas")
ax.plot(suboriginsT[0].vec(),suboriginsT[1].vec(),suboriginsT[2].vec(),marker='o',linestyle='',label="Sub-Arrays")
ax.legend()


subdistances=hArray(float,[NSubArrays])
subbaselines=hArray(float,[NSubArrays])

hSkewLinesDistanceToClosestApproach(subdistances,subbaselines,meanorigin,meandirection,suborigins,subdirections,10000.);
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
