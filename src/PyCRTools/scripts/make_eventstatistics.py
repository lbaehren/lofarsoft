import pytmf
import pycrtools as cr
from pycrtools import lora
import os
import sys
import pycrtools.tasks as tasks
from time import gmtime
import datetime
import numpy as np
from pylab import pi
import matplotlib.ticker as Ticker


#-----------------------------------------------------------------------------
# Return space angle between two zenith/azimuth points
# Everything is in radians
def spaceAngle(zen1, az1, zen2, az2):
  return np.arccos(np.sin(zen1)*np.sin(zen2)*np.cos(az1-az2) 
              + np.cos(zen1)*np.cos(zen2))


               
#-----------------------------------------------------------------------------
#
# Polar skyplot.  Arguments are a 2D array of coordinates in
# (zenith, azimuth) in degrees, and a smearing angle,
# also in degrees.
#
cr.plt.clf()
cr.plt.cla()


def skyplot(coords, smear, titleStr="", logZ=False, savePlot=False, plotName="skyplot.ps"):

                  
    print "Skyplot: %d points" % (coords.shape[0])

    # Maximum zenith angle
    ZENMAX = np.radians(105.)
    
    # Create calculation grid
    deltatheta = 2.*pi/100.
    theta = np.arange(0., 2.*pi+0.5*deltatheta, deltatheta)    
    R = np.arange(0., ZENMAX, deltatheta/4.)
    r,t = np.meshgrid(R, theta)

    # Weight is a Gaussian smear of each point based on
    # the space angle
    Z = r*0.0
    smearRad = np.radians(smear)
    for p in coords:
        print "Adding point: ", p
        r0 = np.radians(p[0])
        t0 = np.radians(p[1])
        phi = spaceAngle(r,t,r0,t0)
        w = np.exp(-(phi*phi)/(2*smearRad*smearRad))
        #print "Adding point (",p[0],",",p[1],"): wMax = %g" % (w.max())
        Z = Z + w

    print "ZMin = %g ZMax = %g" % (Z.min(), Z.max())

    # Convert to Cartesian coordinates
    X = r*np.cos(t)
    Y = r*np.sin(t)
    ax = cr.plt.subplot(111)

    # Set levels: keep bottom level above background so single
    # events are visible
    if not logZ:
      levs = np.linspace(1/np.sqrt(np.e), np.ceil(Z.max()), 100)
    else:
      Z = np.ma.log10(Z)
      levs = np.linspace(np.log10(1/np.sqrt(np.e)), np.ceil(Z.max()), 100)
      
    # Plot the contours
    cs = ax.contourf(X,Y,Z, levs, facecolor='white') #, cmap=P.get_cmap('hot'))

    # Make sure aspect ratio is preserved
    ax.set_aspect('equal')
    # Turn off rectangular frame.
    ax.set_frame_on(False)
    # Turn off Cartesian axis ticks
    ax.set_xticks([])
    ax.set_yticks([])
    
    # Draw the polar grid
    rmax = max(R)
    # FIX ME THIS IS HARD-CODED and depends on the maximum zenith angle ZENMAX
    nZenStep = 7
    nAziStep = 12
    for i in range(nZenStep):
        if (i != 1):
            # This is the horizon
            ax.plot((1-i/float(nZenStep))*rmax*np.cos(theta),(1-i/float(nZenStep))*rmax*np.sin(theta),'gray')
        else:
            ax.plot((1-i/float(nZenStep))*rmax*np.cos(theta),(1-i/float(nZenStep))*rmax*np.sin(theta),
                    'black',linewidth=2)
    for i in range(nAziStep):
        ax.plot(R*np.cos(i*2*pi/nAziStep),R*np.sin(i*2*pi/nAziStep),'gray')

    # Label the axes in a highly primitive manner
    ax.annotate('0',  xy=(0*rmax/nZenStep+0.01, 0.02))
    ax.annotate('45', xy=(3*rmax/nZenStep-0.1, 0.02))
    ax.annotate('90', xy=(6*rmax/nZenStep+0.01, 0.02))

    ax.annotate('E', xy=(rmax - 0.1, -0.12))
    ax.annotate('N', xy=(0.02, rmax - 0.12))
    ax.annotate('W', xy=(-rmax + 0.02, -0.12))
    ax.annotate('S', xy=(0.02, -rmax + 0.04))

    cr.plt.suptitle(titleStr, fontsize=10, x=0.15, y=0.98, horizontalalignment='left', verticalalignment='bottom', family='monospace')

    formatter = Ticker.FormatStrFormatter("%.2f")
    cr.plt.colorbar(cs, ax=ax, format=formatter)
    
    if savePlot:
    # Save as postscript file
        print "Saving plot as "+plotName
        cr.plt.savefig(plotName, facecolor='white')


# Longitude of LOFAR
L = pytmf.deg2rad(6.869837540)

path = '/Volumes/Data/VHECR/LORAtriggered/results'
dirList=os.listdir(path)

rmslist = []
timelist = []
spectrallist = []
azimuthgood = []
zenithgood = []
azimuthbad = []
zenithbad = []
for fname in dirList:
    if "VHECR" in fname:
        Ev = str(fname)
        try:
            query = ['002000002','005008071']
            statistics=cr.trun("eventstatistics",topdir=path,events=Ev, antennaquery=query)
            print statistics
            rms = statistics[0]
            utc = statistics[1]
            direction = statistics[2]
            status = statistics[3]
            spectral_power = statistics[4]
            
            if direction[0] >= 0:
                if direction[1] >= 0:
                    if status == True:
                        print "OK"
                        zenithgood.append(90-direction[1])
                        azimuthgood.append(90-direction[0])
                    if status == False:
                        print "Bad"
                        zenithbad.append(90-direction[1])
                        azimuthbad.append(90-direction[0])                        
                       
            timestr = gmtime(utc)
            #daysec = timestr[3]*3600+timestr[4]*60+timestr[5]
            rmslist.append(rms)
            if spectral_power > 100000:
                spectral_power = -100
            spectrallist.append(spectral_power)
    
            t = datetime.datetime.utcfromtimestamp(utc)	
    
            # Calculate JD(UT1)
            ut = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.))
    
            # Calculate JD(TT)
            dtt = pytmf.delta_tt_utc(pytmf.date2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.)))
            tt = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + (float(t.second) + dtt / 3600.)) / 24.));
    
            # Calculate Local Apparant Sidereal Time
            theta_L = pytmf.rad2circle(pytmf.last(ut, tt, L));
    
            timelist.append(theta_L)    
        
        except KeyboardInterrupt:
            raise
        except Exception as e:
            print e
            continue     


# Warning: RMS of antenna not in the run are set to -100

cr.plt.plot(timelist,rmslist,linestyle="None",marker='o') 
cr.plt.xlabel("LST [rad]")
cr.plt.ylabel("calibrated timeseries rms of antenna %s [a.u.]"%query[0])

print "time", timelist
print "spectral", spectrallist
cr.plt.figure()
cr.plt.plot(timelist,spectrallist,linestyle="None",marker='s')
cr.plt.xlabel("LST [rad]")
cr.plt.ylabel("antenna spectral power of antenna %s [a.u.]"%query[0])

coord = np.array(([zenithgood, azimuthgood]))  
coord = coord.transpose()
cr.plt.figure()
skyplot(coord, 2,titleStr="Events seen",plotName="skyplot-good.ps") 


print "Bad Angles:", zenithbad, azimuthbad
coordbad = np.array(([zenithbad, azimuthbad]))  
coordbad = coordbad.transpose()
cr.plt.figure()
skyplot(coordbad,2,titleStr="Events missed",plotName="skyplot-bad.ps")


#np.savetxt("timelist", np.asarray(timelist))
#np.savetxt("rmslist", np.asarray(rmslist))

