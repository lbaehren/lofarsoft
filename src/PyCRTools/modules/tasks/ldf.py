"""

Task: Lateral Signal Distribution (LDF) 
=================================

Created by Anna Nelles

"""

import pycrtools as cr
import pycrtools.tasks as tasks
from pycrtools.tasks import shortcuts as sc
import pytmf
import os


def GetInformationFromFile(topdir, events, plot_parameter="pulses_maxima_y"):

    eventdirs=cr.listFiles([os.path.join(topdir,event) for event in events])
    
    for eventdir in eventdirs:
        
        print "Processing event in directory:",eventdir
        
        par={}
        antid={0:[],1:[]}
        signal={0:[],1:[]}
        positions={0:[],1:[]}
        positions2={0:[],1:[]}
        ndipoles={0:0,1:0}
    
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir,"results.py")):
                continue
            resfile=open(os.path.join(datadir,"results.py"))
#            if "nan" in resfile.read():
#                print "WARNING nan found. skipping file", resfile.name
#                continue
            print "Processing data results directory:",datadir
            res={}
            execfile(os.path.join(datadir,"results.py"),res)
            res=res["results"]
            antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
            
            positions[res["polarization"]].extend(res["antenna_positions_ITRF_m"])  
            
            # No conversion needed for: Available in more recent version of pipeline:" 
            #positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])  

            # check, which pulse definition most suitable for LDF ploting
            
#            signal[res["polarization"]].extend(res["pulses_maxima_y"])  
            signal[res["polarization"]].extend(res[plot_parameter])
            
            ndipoles[res["polarization"]]+=res["ndipoles"]
    
    
        print "Number of dipoles found:",ndipoles
        
        par["eventid"]="LOFAR "+res["FILENAME"].split('-')[1]
        
 
        # LORA parameters
        # current fix for LORA core only being propagated with two coordinates instead of three
        
        lorcore=cr.hArray(res["pulse_core_lora"])        
        Dim = lorcore.getDim()
        if 2 in Dim:         
            par["loracore"]=cr.hArray([lorcore[0],lorcore[1],0.0])
        else:    
            if 3 in Dim:
                par["loracore"] = lorcore
            else:
                print "Warning, Coreposition of LORA seems to contain the wrong number of coordinates"
                
        par["loradirection"]=cr.hArray(res["pulse_direction_lora"])
        par["loraenergy"]=res["pulse_energy_lora"]
        
        # if use XYZ positions, change conversion here

        pos0=cr.hArray(float,[ndipoles[0],3],positions[0],name="Antenna Positions",units="m") 
        par["positions0"]=cr.metadata.convertITRFToLocal(pos0)
  
        pos1=cr.hArray(float,[ndipoles[1],3],positions[1],name="Antenna Positions",units="m")
        par["positions1"]=cr.metadata.convertITRFToLocal(pos1)
        
        # check normalization of signal (to be confirmed with respect to signal calibration)
        
        par["signals1"]=cr.hArray(signal[1])
        par["signals0"]=cr.hArray(signal[0])

    return par


class ldf(tasks.Task):

    """
    This task will use the information given by the LOFAR stations (signal strength) together with information from LORA (shower core and direction) to create a plot showing the dependecy of the signal strength in the antennas on the distance to the shower axis as measured with LORA. In cosmic ray physics this is usually referred to as the lateral distribution or LDF. 
    
    It can take into account uncertainties on the signal strength (as soon as provided by the pipeline) as well as uncertainties on the core position and direction. If none of the uncertainties are provided the LDF will be drawn with default errors and an output warning will remind the user. 
    
    Example:: 
    
    import pycrtools as cr
    
    ldf=cr.trun("ldf",topdir="/Users/annanelles/Uni/LOFAR/data/",events=["VHECR_LORA-20110612T235954.184Z"])
    
    cr.plt.show()
    
    """ 


    parameters = dict(
        topdir=dict(default="/data/VHECR/LORAtriggered/results",doc="provide topdir",unit=""),
        eventslist=dict(default=lambda self:self.events if isinstance(self.events,list) else [self.events],doc="list with event names to process (directories in topdir)",unit=""),
        events=dict(default=["VHECR_LORA-20110612T231913.199Z"],doc="Event directories in topdir - either as list or as single string",unit=""),
        results=dict(default=lambda self:GetInformationFromFile(self.topdir,self.eventslist,plot_parameter=self.plot_parameter),doc="Provide task with topdirectory",unit=""),
        positions1=dict(default=lambda self:self.results["positions1"],doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions in pol 1 (x0,y0,z0,...)",unit="m"),
        positions0=dict(default=lambda self:self.results["positions0"],doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions in pol 0 (x0,y0,z0,...)",unit="m"),
        signals1=dict(default=lambda self:self.results["signals1"],doc="hArray of dimension [NAnt,1] with signals in antennas, pol 1",unit="a.u."),
        signals0=dict(default=lambda self:self.results["signals0"],doc="hArray of dimension [NAnt,1] with signals in antennas, pol 0",unit="a.u."),
        signaluncertainties1=dict(default=None, doc="hArray of dimension [NAnt,1], signaluncertainties in pol 1", unit="a.u."),
        signaluncertainties0=dict(default=None, doc="hArray of dimension [NAnt,1], signaluncertainties in pol 0", unit="a.u."),
        loracore = dict(default=lambda self:self.results["loracore"],doc="hArray of core position [x,y,z] as provided by Lora ",unit="m"),
        loradirection = dict(default=lambda self:self.results["loradirection"],doc="hArray of shower direction [az, el] as provided by Lora. Az is eastwards from north and El is up from horizon.", unit="degrees"),
        loracoreuncertainties = dict(default=None, doc="hArray of uncertainties of core position [ex,ey,cov]",unit="m"),
        loradirectionuncertainties = dict(default=None, doc="hArray of uncertainties of direction [eAz,eEl,cov]",unit="degrees"),
        eventid = dict(default=lambda self:self.results["eventid"], doc="EventId for LOFAR Event"),
        square=dict(default=False, doc="Square the data (to get power) before plotting."),
        plot_parameter=dict(default="pulses_maxima_y", doc="Which parameter of the results-dict to plot: 'pulses_strength' or 'pulses_maxima_y'"),
        logplot=dict(default=True, doc="Draw y-axis logarithmically"),
        plot_clf = dict(default=True,doc="Clean window before plotting?"),
        plot_xmin = dict(default=0,doc="Mininum value of x-axis"),
        plot_xmax = dict(default=400,doc="Maximum value of x-axis")
        )


    ## Functions for shower geometry and uncertainty propagation

    def GetDistance(self,loracore,loradirection,positions):        
        
        distances = cr.hArray(copy=positions)
        finaldistance = cr.hArray(copy=positions)
        
        theta = cr.radians(loradirection[1])           # spherical coordinates: 0 = horizon, 90 = vertical
        phi = cr.radians(450 -loradirection[0])        # recalculate to 0 = east counter-clockwise
    
        
        distances = positions - loracore          
        axis = cr.hArray([cr.cos(theta)*cr.cos(phi),cr.cos(theta)*cr.sin(phi),cr.sin(theta)])  # shower axis
                
        finaldistance[...].crossproduct(distances[...],axis)
        dist = cr.hVectorLength(finaldistance[...])
        
        return dist  
        
    
    def GetTotalDistanceUncertainty(self,loracore,loracoreuncertainties,positions,loradirection,loradirectionuncertainties,distances): 
        
        # Assuming differences in z-coordinates = 0
        
        dist = distances.vec()
        
        pos1 = cr.hArray_transpose(positions)[0].vec()
        pos2 = cr.hArray_transpose(positions)[1].vec()
        
        core1 = float(loracore[0])
        core2 = float(loracore[1])
        
        phi = cr.radians(450 -loradirection[0])        # recalculate to 0 = east counter-clockwise
        theta = cr.radians(loradirection[1])           # spherical coordinates: 0 = horizon, 90 = vertical
        
        
        ecore1 = float(loracoreuncertainties[0])        
        ecore2 = float(loracoreuncertainties[1])
        covcore = float(loracoreuncertainties[2])
        
        ephi = cr.radians(float(loradirectionuncertainties[0]))
        etheta = cr.radians(float(loradirectionuncertainties[1]))
        covangles = float(loradirectionuncertainties[2])
        
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
 
   
   
    
    def init(self):
        pass
    
    def call(self):
        pass
    
    def run(self):
        
        if self.loracoreuncertainties == None:
            self.loracoreuncertainties = cr.hArray([5.,5.,0.])
            print "Warning: Using default for core uncertainties!"
        
        if self.loradirectionuncertainties == None:
            self.loradirectionuncertainties = cr.hArray([1.,1.,0])
            print "Warning: Using default for direction uncertainties!"      
        
        self.Distances0 = self.GetDistance(self.loracore,self.loradirection,self.positions0)
        self.Distances1 = self.GetDistance(self.loracore,self.loradirection,self.positions1)

        self.signals0.par.xvalues=cr.hArray(self.Distances0)
        self.signals1.par.xvalues=cr.hArray(self.Distances1)

        if self.square:
            self.signals0.square()
            self.signals1.square()
            
        self.DistUncertainties0 = self.GetTotalDistanceUncertainty(self.loracore,self.loracoreuncertainties,self.positions0,self.loradirection,self.loradirectionuncertainties,self.Distances0)
        self.DistUncertainties1 = self.GetTotalDistanceUncertainty(self.loracore,self.loracoreuncertainties,self.positions1,self.loradirection,self.loradirectionuncertainties,self.Distances1)

        if self.plot_clf:
            cr.plt.clf()

        if self.signals0:
            if self.signaluncertainties0:
                cr.plt.errorbar(self.Distances0.vec(),self.signals0.vec(),self.signaluncertainties0.vec(),self.DistUncertainties0.vec(),color='r',linestyle="None",label="pol 0")
            else:
                self.signals0.plot(color='r',linestyle="None",marker="o",label="pol 0",clf=False)           

        if self.signals1:
            if self.signaluncertainties1:
                cr.plt.errorbar(self.Distances1.vec(),self.signals1.vec(),self.signaluncertainties1.vec(),self.DistUncertainties0.vec(),color='b',linestyle="None",label="pol 2")
            else:
                self.signals1.plot(color='b',linestyle="None",marker="o",label="pol 1",clf=False)           

        cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
        cr.plt.xlabel("Distance to Shower Axis [m]")
        cr.plt.ylabel("Power [a.u.]")
        cr.plt.axis(xmin=self.plot_xmin,xmax=self.plot_xmax)
                
        if self.logplot:
            cr.plt.yscale("log")
            cr.plt.axis(xmin=self.plot_xmin,xmax=self.plot_xmax)
            
        if self.eventid:
            cr.plt.title(str(self.eventid))
              

    
            
    
