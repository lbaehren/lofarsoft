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
    
    Usage: 
    
    LDF = cr.trun("Shower",positions=positions, signals=signals, signals_uncertainties=signals_uncertainties, core=core,direction=direction,core_uncertainties=core_uncertainties,direction_uncertainties=direction_uncertainties, ldf_enable=True)
    
    Task can be run without giving uncertainties. The resulting plot will then also not contain any uncertainties. 
    
    FOOTPRINT = cr.trun("Shower", positions=positions, signals=signals, timelags = timelags, direction=direction, core = core, footprint_enable=True)
        
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
            doc = "Uncertainties on the direction of the shower in [eAZ,eEL,Cov]"), 
        timelags = dict(default=None,
            doc = "Timelags of signals given in nanoseconds [NAntennas x (X,Y,Z)]"),
        eventid = dict(default = None,
            doc="Give Event ID to be specified in plot title "), 
            
        save_plots = dict(default=False,
            doc="Saving plost instead of showing them.") ,              
        
        ldf_enable = dict( default = False, 
            doc = "Draw Lateral Distribution Function, signal vs. distance from shower axis"), 
        ldf_logplot = dict(default = True,
            doc = "Draw LDF with log axis"), 
        ldf_remove_outliers = dict(default = True,
            doc = "Do not allow values > 1000000 or < 0.001 for signal strength"),
        ldf_color_x = dict(default='#B30424', doc = "color signals x"),
        ldf_color_y = dict(default='#68C8F7', doc = "color signals y"),
        ldf_color_z = dict(default='#FF8C19', doc = "color signals z"),
        ldf_marker_x = dict(default='s', doc = "marker signals x"),
        ldf_marker_y = dict(default='o', doc = "marker siganls y"),
        ldf_marker_z = dict(default='v', doc = "marker siganls z"),  
        
        footprint_enable = dict(default = False,
            doc = "Draw footprint of the shower"),  
        footprint_colormap = dict(default="autumn",doc="colormap to use for LOFAR timing"),
        footprint_marker_lofar = dict(default='o', doc="Marker for LOFAR stations in footprint"),
        footprint_use_background = dict(default=True, doc="Use LOFAR map as background for footprint"),
        footprint_largest_point = dict(default=300,doc="Largest point in plot for LOFAR"),
        footprint_use_title = dict(default=True,doc="Draw title indicating polarizations and event id (if given)"),
         
        footprint_shower_enable = dict(default=True, doc='Draw shower geometry in footprint'), 
        footprint_shower_color = dict(default="#151B8D", doc='Color in which the shower geometry is drawn'),            
             
        footprint_lora_enable = dict(default = False,
            doc = "Draw Information from LORA"), 
        lora_positions = dict(default=None,
            doc="LORA detector positions, given in [Nx3], [NDetectors x (X,Y,Z)], X,Y,Y kartesian" ),  
        lora_signals = dict(default=None, 
            doc="Signals in LORA, given in [NDetectors]"),
        lora_timelags = dict(default=None,
            doc = "Timelags of signals in LORA detectors, given in nanoseconds [NDetectors] "), 
        footprint_color_lora = dict(default='#730909',doc="Color used for LORA plots. If set to 'time' uses the arrival time" ),    
        footprint_marker_lora = dict(default='',doc="Marker for LORA stations in footprint"),
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
        positions = cr.hArray(positions)
        
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
        
        # ---------------------  LDF  ------------------------------ #
        
        if self.ldf_enable:
        
            Dist = self.__GetDistance(self.core,self.direction,self.positions)
        
            if self.ldf_remove_outliers:
                for i in xrange(len(self.signals[:,0])):
                    if self.signals[i,0] > 100000:
                        self.signals[i,0] = 10
                    if self.signals[i,0] < 0.001:
                        self.signals[i,0] = 1
                    if self.signals[i,1] > 100000:
                        self.signals[i,1] = 10
                    if self.signals[i,1] < 0.001:
                        self.signals[i,1] = 1
                    if self.signals[i,2] > 100000:
                        self.signals[i,2] = 10
                    if self.signals[i,2] < 0.001:
                        self.signals[i,2] = 1                              
            
            if self.core_uncertainties is not None and self.direction_uncertainties is not None and self.signals_uncertainties is not None:
                
                Dist_uncert = self.__GetTotalDistanceUncertainty(self.core,self.core_uncertainties,self.positions,self.direction,self.direction_uncertainties,Dist)
                
                if self.ldf_logplot:
                    sig_lower = np.copy(self.signals_uncertainties)   # Avoid negative errorbars
                    sig_lower.fill(0.001)
                    sig_lower = np.maximum(sig_lower,self.signals - self.signals_uncertainties)
                    sig_uncer = self.signals - sig_lower
                else:
                    sig_lower = np.copy(self.signals_uncertainties)      

                
                cr.plt.figure()
                cr.plt.errorbar(Dist,self.signals[:,0],yerr=[sig_uncer[:,0],self.signals_uncertainties[:,0]],xerr=Dist_uncert,c=self.ldf_color_x,marker=self.ldf_marker_x,linestyle="None",label="x")
                cr.plt.errorbar(Dist,self.signals[:,1],yerr=[sig_uncer[:,1],self.signals_uncertainties[:,0]],xerr=Dist_uncert,c=self.ldf_color_y,marker=self.ldf_marker_y,linestyle="None",label="y")
                cr.plt.errorbar(Dist,self.signals[:,2],yerr=[sig_uncer[:,2],self.signals_uncertainties[:,0]],xerr=Dist_uncert,c=self.ldf_color_z,marker=self.ldf_marker_z,linestyle="None",label="z")     
                
                cr.plt.xlabel("Distance to Shower Axis [m]")
                cr.plt.ylabel("Signal [a.u.]")
                
                if self.ldf_logplot:
                    cr.plt.yscale("log")
                cr.plt.legend(loc='upper right', shadow=False, numpoints=1)

            
            else:
                print "Not all uncertainties given, do not plot uncertainties"    
                cr.plt.figure()
                cr.plt.scatter(Dist,self.signals[:,0],c=self.ldf_color_x,marker=self.ldf_marker_x,label="x")
                cr.plt.scatter(Dist,self.signals[:,1],c=self.ldf_color_y,marker=self.ldf_marker_y,label="y")
                cr.plt.scatter(Dist,self.signals[:,2],c=self.ldf_color_z,marker=self.ldf_marker_z,label="z")
                
                cr.plt.xlabel("Distance to Shower Axis [m]")
                cr.plt.ylabel("Signal [a.u.]")
                
                if self.ldf_logplot:
                    cr.plt.yscale("log")
                cr.plt.legend(loc='upper right', shadow=False, scatterpoints=1)
                
            if self.save_plots:
                print "Not implemented yet"
            else:    
                 cr.plt.show()   
        
       #---------------------- FOOTPRINT --------------------# 
        
                
        if self.footprint_enable: 
        
            if self.lora_signals is not None and self.lora_positions is not None:
                
                if self.lora_timelags is not None and self.footprint_color_lora == 'time':
                    print "Draw LORA with timelags and color, still to be implemented"
                else:
                    print "Draw LORA wth signals only, still to be implemented"
    
            if self.footprint_use_background:
                    from os import environ
                    from os.path import isfile
                    if "LOFARSOFT" in environ.keys():
                        bgimname=environ["LOFARSOFT"]+"/src/PyCRTools/extras/LORA_layout_background.png"
                        if isfile(bgimname):
                            bgim=cr.plt.imread(bgimname)
                        else:
                            print "WARNING Cannot plot layout"
                    else:
                        print "WARNING Cannot plot layout. Environment variable LOFARSOFT not found."
        
            if self.footprint_shower_enable and self.direction is not None and self.core is not None:
                    
                    dcos=cr.cos(cr.radians(self.direction[0]))
                    dsin=cr.sin(cr.radians(self.direction[0]))
                    elev=self.direction[1]
                    

            if self.positions is not None and self.signals is not None:

                self.sizes0 = self.signals[:,0]
                self.sizes0 /= self.sizes0.max()
                self.sizes0 *= self.footprint_largest_point
                
                if self.timelags is None:
                   self.scolors="blue"
                   print "WARNING, footprint does not represent the time, only the signal strength"
                
                # POL 0
                
                cr.plt.figure()
                if bgim is not None and self.footprint_use_background:
                    cr.plt.imshow(bgim,origin='upper',extent=[-375/2,375/2,-375/2-6*120/227,375/2-6*120/227],alpha=1.0)
                if self.timelags is not None:
                    self.scolors=self.timelags[:,0]                
                cr.plt.scatter(self.positions[:,0],self.positions[:,1],s=self.sizes0,c=self.scolors,marker=self.footprint_marker_lofar,cmap=self.footprint_colormap)
                cr.plt.xlabel("LOFAR East [meters] ")
                cr.plt.ylabel("LOFAR North [meters] ")
                if self.timelags is not None:
                    self.cbar=cr.plt.colorbar()
                    self.cbar.set_label("Time of arrival (ns)")
                if self.footprint_use_title:
                    if self.eventid:
                        self.title = str(self.eventid)+" in pol X" 
                    else:
                        self.title = 'pol X'    
                    cr.plt.title(self.title)
                
                #Plotting the shower
                if self.footprint_shower_enable:
                    cr.plt.arrow(self.core[0]+elev*dsin,self.core[1]+elev*dcos,-elev*dsin,-elev*dcos,lw=4,color=self.footprint_shower_color)
                    cr.plt.scatter(self.core[0],self.core[1],marker='x',s=600,color=self.footprint_shower_color,linewidth=4)                
                
                #POL 1
                
                self.sizes1 = self.signals[:,1]
                self.sizes1 /= self.sizes1.max()
                self.sizes1 *= self.footprint_largest_point
                                
                cr.plt.figure()
                if bgim is not None and self.footprint_use_background:
                    cr.plt.imshow(bgim,origin='upper',extent=[-375/2,375/2,-375/2-6*120/227,375/2-6*120/227],alpha=1.0)
                if self.timelags is not None:
                    self.scolors=self.timelags[:,1]
                cr.plt.scatter(self.positions[:,0],self.positions[:,1],s=self.sizes1,c=self.scolors,marker=self.footprint_marker_lofar,cmap=self.footprint_colormap)    
                cr.plt.xlabel("LOFAR East [meters] ")
                cr.plt.ylabel("LOFAR North [meters] ")
                if self.timelags is not None:
                    self.cbar=cr.plt.colorbar()
                    self.cbar.set_label("Time of arrival (ns)")
                if self.footprint_use_title:
                    if self.eventid:
                        self.title = str(self.eventid)+" in pol Y" 
                    else:
                        self.title = 'pol Y'    
                    cr.plt.title(self.title)                
                
                #Plotting the shower
                if self.footprint_shower_enable:
                    cr.plt.arrow(self.core[0]+elev*dsin,self.core[1]+elev*dcos,-elev*dsin,-elev*dcos,lw=4,color=self.footprint_shower_color)
                    cr.plt.scatter(self.core[0],self.core[1],marker='x',s=600,color=self.footprint_shower_color,linewidth=4)                
                
                #POL 2
                if self.signals.shape[1] == 3:
                
                    self.sizes2 = self.signals[:,2]
                    self.sizes2 /= self.sizes2.max()
                    self.sizes2 *= self.footprint_largest_point
                                    
                    cr.plt.figure()
                    if bgim is not None and self.footprint_use_background:
                        cr.plt.imshow(bgim,origin='upper',extent=[-375/2,375/2,-375/2-6*120/227,375/2-6*120/227],alpha=1.0)
                    if self.timelags is not None:
                        self.scolors=self.timelags[:,2]
                    cr.plt.scatter(self.positions[:,0],self.positions[:,1],s=self.sizes2,c=self.scolors,marker=self.footprint_marker_lofar,cmap=self.footprint_colormap)    
                    cr.plt.xlabel("LOFAR East [meters] ")
                    cr.plt.ylabel("LOFAR North [meters] ")
                    
                    if self.timelags is not None:
                        self.cbar=cr.plt.colorbar()
                        self.cbar.set_label("Time of arrival (ns)")
                    if self.footprint_use_title:
                        if self.eventid:
                            self.title = str(self.eventid)+" in pol Z" 
                        else:
                            self.title = 'pol Z'    
                        cr.plt.title(self.title)                
                    
                    #Plotting the shower
                    if self.footprint_shower_enable:
                        cr.plt.arrow(self.core[0]+elev*dsin,self.core[1]+elev*dcos,-elev*dsin,-elev*dcos,lw=4,color=self.footprint_shower_color)
                        cr.plt.scatter(self.core[0],self.core[1],marker='x',s=600,color=self.footprint_shower_color,linewidth=4)                


                if self.save_plots:
                    print "Not implemented yet"
                else:    
                    cr.plt.show()
            
            else:
                print "WARNING: Give at least positions and signals to plot a footprint"                