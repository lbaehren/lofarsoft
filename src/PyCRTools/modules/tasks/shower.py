"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import numpy as np

class Shower(Task):
    """
    Making all sorts of plots to understand the shower. 
    
    """

    parameters = dict(
        positions = dict( default = None,
            doc = "Antennas positions given in [Nx3], [NAntennas x (X,Y,Z)], X,Y,Y kartesian" ),
        signals = dict( default = None,
            doc = "Signals in the antennas given in [Nx3], [NAntennas x (pol_x,pol_y,pol_z)]" ),    
        signals_uncertainties = dict(default = None,
            doc = "Uncertainties on signal in antennas in [Nx3]"),        
        core = dict( default = None,
            doc = "Core position of the shower given in [X,Y,Z], kartesian" ),
        core_uncertainties = dict(default = None,
            doc= "Uncertainties of core position of shower in [eX,eY,eZ], kartesian"),    
        direction = dict( default = None,
            doc = "Direction of the shower [AZ, EL], AZ is in degrees eastwards from north and EL is in degrees up from horizon." ),
        direction_uncertainties = dict(default = None, 
            doc = "Uncertainties on the direction of the shower in [eAZ,eEL]"),     
        ldf_enable = dict( default = False, 
            doc = "Draw Lateral Distribution Function, signal vs. distance from shower axis"),            
    )


    def __GetDistance(self,core,direction,positions):


        """
        Calculating the distance of a station to the shower core in the shower plane.
        """
        
        positions = cr.hArray(positions)
        core = cr.hArray(core)
        
        distances = cr.hArray(copy=positions)
        finaldistance = cr.hArray(copy=positions)

        theta = cr.radians(direction[1])           # spherical coordinates: 0 = horizon, 90 = vertical
        phi = cr.radians(450 - direction[0])        # recalculate to 0 = east counter-clockwise


        distances = positions - core
        axis = cr.hArray([cr.cos(theta)*cr.cos(phi),cr.cos(theta)*cr.sin(phi),cr.sin(theta)])  # shower axis

        finaldistance[...].crossproduct(distances[...],axis)
        dist = cr.hVectorLength(finaldistance[...])

        return dist


    def __GetTotalDistanceUncertainty(self,core,coreuncertainties,positions,direction,directionuncertainties,distances):

        """
        Propagating the uncertatinties of the core and the directions into the uncertainties of the distance to the shower axis.

        """

        # Assuming differences in z-coordinates = 0

        dist = distances.vec()

        pos1 = cr.hArray_transpose(positions)[0].vec()
        pos2 = cr.hArray_transpose(positions)[1].vec()

        core1 = float(core[0])
        core2 = float(core[1])

        phi = cr.radians(450 -direction[0])        # recalculate to 0 = east counter-clockwise
        theta = cr.radians(direction[1])           # spherical coordinates: 0 = horizon, 90 = vertical


        ecore1 = float(coreuncertainties[0])
        ecore2 = float(coreuncertainties[1])
        covcore = float(coreuncertainties[2])

        ephi = cr.radians(float(directionuncertainties[0]))
        etheta = cr.radians(float(directionuncertainties[1]))
        covangles = float(directionuncertainties[2])

        # short cuts

        cost = cr.cos(theta)
        sint = cr.sin(theta)
        cos2t = cr.cos(theta)**2
        sin2t = cr.sin(theta)**2
        cosp = cr.cos(phi)
        sinp = cr.sin(phi)
        cos2p = cr.cos(phi)**2
        sin2p = cr.sin(phi)**2

        m1 = pos1 - core1
        m2 = pos2 - core2

        sqrfac = 0.5/dist

        # partical derivatives

        diffc1 = -2.*(m1*sin2t + m1*cos2t*sin2p - m2*cos2t*sinp*cosp) * sqrfac
        diffc2 = -2.*(m2*sin2t + m2*cos2t*cos2p - m1*cos2t*sinp*cosp) * sqrfac
        diffp = 2.*(m1*m1*cos2t*sinp*cosp - m2*m2*cos2t*cosp*sinp- m1*m2*cos2t*(cos2p-sin2p)) *sqrfac

        difft = 2.*((m2*m2)*sint*cost + (m1*m1)*sint*cost - (m1*m1)*sin2p*cost*sint - (m2*m2)*cos2p*cost*sint + m1*m2*sint*cost*sinp*cosp) * sqrfac

        # adding contributions

        err =        diffc1*diffc1 * ecore1*ecore1
        err = err +  diffc2*diffc2 * ecore2*ecore2
        err = err +  diffp*diffp * ephi*ephi
        err = err +  difft*difft * etheta*etheta
        err = err + 2* difft*diffp *covangles
        err = err + 2* diffc1*diffc2 *covcore

        err.sqrt()

        return err
        
    def run(self):
        """Run the task.
        """
        
        if self.ldf_enable:
        
            Dist = self.__GetDistance(self.core,self.direction,self.positions)
            
            cr.plt.figure()
            cr.plt.scatter(Dist,self.signals[:,0],c='b',label="x")
            cr.plt.scatter(Dist,self.signals[:,1],c='g',label="y")
            cr.plt.scatter(Dist,self.signals[:,2],c='r',label="z")
            
            cr.plt.xlabel("Distance to Shower Axis [m]")
            cr.plt.ylabel("Signal [a.u.]")
            cr.plt.yscale("log")
            cr.plt.legend(loc='upper right', shadow=False, scatterpoints=1)
            
            cr.plt.show()
            
            
            if self.core_uncertainties and self.direction_uncertainties and self.signals_uncertainties:
                
                Uncert = self.__GetTotalDistanceUncertainty()
            
            else:
                print "Not all uncertainties given, do not plot uncertainties"    
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
                    