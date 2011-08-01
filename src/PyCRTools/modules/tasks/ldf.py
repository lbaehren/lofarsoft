"""
A LDF module by Anna Nelles

last edited July 29th

"""

import pycrtools as cr
import pycrtools.tasks as tasks
import pytmf
from pycrtools.tasks.shortcuts import *




class ldf(tasks.Task):

    parameters = dict(
              positions={doc:"hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",unit:"m"},
              signalsNS={doc:"hArray of dimension [NAnt,1] with signals in antennas, polNS",unit:"a.u."},
              signalsEW={doc:"hArray of dimension [NAnt,1] with signals in antennas, polEW",unit:"a.u."},
              signaluncertaintiesNS={default:None, doc:"hArray of dimension [NAnt,1], signaluncertainties in NS", unit:"a.u."},
              signaluncertaintiesEW={default:None, doc:"hArray of dimension [NAnt,1], signaluncertainties in EW", unit:"a.u."},
              loracore = {doc:"hArray of core position [x,y,z] as provided by Lora ",unit:"m"},
              loradirection = {doc:"hArray of shower direction [az, el] as provided by Lora. Azimuth is eastwards from north and elevation is up from horizon.", unit:"degrees"},
              loracoreuncertainties = {default:cr.hArray([1.,1.,0.]),doc:"hArray of uncertainties of core position [ex,ey,cov]",unit:"m"},
              loradirectionuncertainties = {default:cr.hArray([1.,1.,0.]),doc:"hArray of uncertainties of direction [eAz,eEl,cov]",unit:"degrees"},
              eventid = {default:None, doc:"EventId for LOFAR Event"},
              logplot={default:True, doc:"Draw y-axis logarithmically"}
              
              )
    
    ## Functions used ##
    
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
    
        #uncertainties = cr.hArray(copy=distances)
        
        uncertainties = cr.hArray(float,dimensions=len(distances),fill=5.)
        
        print "Warning: Propagation of distance errors not yet implemented!"
        
        return uncertainties
        
        # to be completed, when data gets available
   
    ##
    
    def init(self):
        pass
    
    def call(self):
        pass
    
    def run(self):
        
        Distances = self.GetDistance(self.loracore,self.loradirection,self.positions)
        
        DistUncertainties = self.GetTotalDistanceUncertainty(self.loracore,self.loracoreuncertainties,self.positions,self.loradirection,self.loradirectionuncertainties,Distances)
        
        self.DistUncertainties = DistUncertainties
        
        if self.signaluncertaintiesEW:            
            if self.signaluncertaintiesNS:
                
                cr.plt.errorbar(Distances.vec(),self.signalsEW.vec(),self.signaluncertaintiesEW.vec(),DistUncertainties.vec(),'g',linestyle="None",label="EW pol")
                cr.plt.errorbar(Distances.vec(),self.signalsNS.vec(),self.signaluncertaintiesNS.vec(),DistUncertainties.vec(),'b',linestyle="None",label="NS pol")
                cr.plt.legend(('EW pol','NS pol'),loc='upper right', shadow=False)
                
                
                cr.plt.xlabel("Distance to shower core [m]")
                cr.plt.ylabel("Power [a.u.]")
        else:
            print "Warning, uncertainties used are default! Provide information for correct calculation"
            
            self.signaluncertaintiesEW = cr.hArray(float,dimensions=len(Distances),fill=10) 
            self.signaluncertaintiesNS = cr.hArray(float,dimensions=len(Distances),fill=10)
            
            cr.plt.errorbar(Distances.vec(),self.signalsEW.vec(),self.signaluncertaintiesEW.vec(),DistUncertainties.vec(),'rs',linestyle="None",label="EW pol")
            cr.plt.errorbar(Distances.vec(),self.signalsNS.vec(),self.signaluncertaintiesNS.vec(),DistUncertainties.vec(),'bo',linestyle="None",label="NS pol")
            cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
            
            
            cr.plt.xlabel("Distance to shower core [m]")
            cr.plt.ylabel("Power [a.u.]")  
        
        if self.logplot:
            cr.plt.yscale("log")
            #cr.plt.axis([0,500,10,100000])
            
        if self.eventid:
            cr.plt.title(str(self.title))  

    
            
    
