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


def GetInformationFromFile(topdir, events):

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
            if "nan" in resfile.read():
                print "WARNING nan found. skipping file", resfile.name
                continue
            print "    Processing data results directory:",datadir
            res={}
            execfile(os.path.join(datadir,"results.py"),res)
            res=res["results"]
            antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
            
            positions2[res["polarization"]].extend(res["antenna_positions_ITRF_m"])  
            #positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])  "in newer version of pipeline available"
 
            
            # check, which pulse definition most suitable for LDF ploting
            
            signal[res["polarization"]].extend(res["pulses_maxima_y"])  
            #signal[res["polarization"]].extend(res["pulses_strength"])
            
            ndipoles[res["polarization"]]+=res["ndipoles"]
    
    
        print "Number of dipoles found:",ndipoles
        
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
        
        # check, which polarization (0,1) corresponds to NS, EW resp., if use XYZ positions, change conversion here

        posEW=cr.hArray(float,[ndipoles[0],3],positions2[0],name="Antenna Positions",units="m") 
        par["positionsEW"]=cr.metadata.convertITRFToLocal(posEW)
  
        posNS=cr.hArray(float,[ndipoles[1],3],positions2[1],name="Antenna Positions",units="m")
        par["positionsNS"]=cr.metadata.convertITRFToLocal(posNS)
        
        # check normalization of signal (to be confirmed with respect to signal calibration)
        
        par["signalsNS"]=cr.hArray(signal[1])*65536*100
        par["signalsEW"]=cr.hArray(signal[0])*65536*100

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
              events=dict(default=["VHECR_LORA-20110612T231913.199Z"],doc="provide events in topdir",unit=""),
              results=dict(default=lambda self:GetInformationFromFile(self.topdir,self.events),doc="Provide task with topdirectory",unit=""),
              positionsNS=dict(default=lambda self:self.results["positionsNS"],doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",unit="m"),
              positionsEW=dict(default=lambda self:self.results["positionsEW"],doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",unit="m"),
              signalsNS=dict(default=lambda self:self.results["signalsNS"],doc="hArray of dimension [NAnt,1] with signals in antennas, polNS",unit="a.u."),
              signalsEW=dict(default=lambda self:self.results["signalsEW"],doc="hArray of dimension [NAnt,1] with signals in antennas, polEW",unit="a.u."),
              signaluncertaintiesNS=dict(default=None, doc="hArray of dimension [NAnt,1], signaluncertainties in NS", unit="a.u."),
              signaluncertaintiesEW=dict(default=None, doc="hArray of dimension [NAnt,1], signaluncertainties in EW", unit="a.u."),
              loracore = dict(default=lambda self:self.results["loracore"],doc="hArray of core position [x,y,z] as provided by Lora ",unit="m"),
              loradirection = dict(default=lambda self:self.results["loradirection"],doc="hArray of shower direction [az, el] as provided by Lora. Azimuth is eastwards from north and elevation is up from horizon.", unit="degrees"),
              loracoreuncertainties = dict(default=cr.hArray([1.,1.,0.]),doc="hArray of uncertainties of core position [ex,ey,cov]",unit="m"),
              loradirectionuncertainties = dict(default=cr.hArray([1.,1.,0.]),doc="hArray of uncertainties of direction [eAz,eEl,cov]",unit="degrees"),
              eventid = dict(default=None, doc="EventId for LOFAR Event"),
              logplot=dict(default=True, doc="Draw y-axis logarithmically")
              )

## Functions for shower geometry##


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
        
    
    def GetTotalDistanceUncertainty(self,loracore,loracoreuncertainties,positions,loradirections,loradirectionuncertainties,distances): 
        
        uncertainties = cr.hArray(float,dimensions=len(distances),fill=5.)
        
        print "Warning: Propagation of distance uncertainties not yet implemented!"
        
        return uncertainties 
        
        #     Old implementation for AERA: to be converted to LOFAR standards   
   
   
    
    def init(self):
        pass
    
    def call(self):
        pass
    
    def run(self):
       
        
        DistancesEW = self.GetDistance(self.loracore,self.loradirection,self.positionsEW)
        DistancesNS = self.GetDistance(self.loracore,self.loradirection,self.positionsNS)
        
        DistUncertaintiesNS = self.GetTotalDistanceUncertainty(self.loracore,self.loracoreuncertainties,self.positionsNS,self.loradirection,self.loradirectionuncertainties,DistancesNS)
        DistUncertaintiesEW = self.GetTotalDistanceUncertainty(self.loracore,self.loracoreuncertainties,self.positionsNS,self.loradirection,self.loradirectionuncertainties,DistancesEW)
        

        
        if self.signaluncertaintiesEW:            
            if self.signaluncertaintiesNS:
                
                cr.plt.errorbar(DistancesEW.vec(),self.signalsEW.vec(),self.signaluncertaintiesEW.vec(),DistUncertaintiesEW.vec(),'g',linestyle="None",label="pol 0")
                cr.plt.errorbar(DistancesNS.vec(),self.signalsNS.vec(),self.signaluncertaintiesNS.vec(),DistUncertaintiesNS.vec(),'b',linestyle="None",label="pol 1")
                cr.plt.legend(('pol 0','pol 1'),loc='upper right', shadow=False)
                
                
                cr.plt.xlabel("Distance to shower axis [m]")
                cr.plt.ylabel("Power [a.u.]")
        else:
            print "Warning, uncertainties on signals used are default! Provide information for correct calculation"
            
            self.signaluncertaintiesEW = cr.hArray(float,dimensions=len(DistancesEW),fill=100) 
            self.signaluncertaintiesNS = cr.hArray(float,dimensions=len(DistancesNS),fill=100)
          
                
            cr.plt.errorbar(DistancesEW.vec(),self.signalsEW.vec(),self.signaluncertaintiesEW.vec(),DistUncertaintiesEW.vec(),'rs',linestyle="None",label="pol 0")
            cr.plt.errorbar(DistancesNS.vec(),self.signalsNS.vec(),self.signaluncertaintiesNS.vec(),DistUncertaintiesNS.vec(),'bo',linestyle="None",label="pol 1")
            cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
            
            
            cr.plt.xlabel("Distance to shower axis [m]")
            cr.plt.ylabel("Power [a.u.]")  
        
        if self.logplot:

            cr.plt.yscale("log")
            cr.plt.axis(xmin=0,xmax=400)
            
        if self.eventid:
            cr.plt.title(str(self.title))  

    
            
    
