"""

Task: Event Statistics for LORA/LOFAR 
=================================


"""


import pycrtools as cr
import pycrtools.tasks as tasks
import os
import time

def GetInformationFromFile(topdir, events):

    eventdirs=cr.listFiles([os.path.join(topdir,event) for event in events])
    
    for eventdir in eventdirs:
        
        print "Processing event in directory:",eventdir
        
        par={}
        antid={0:[],1:[]}
        signal={0:[],1:[]}
        positions={0:[],1:[]}

        res={}
    
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir,"results.py")):
                continue
            resfile=open(os.path.join(datadir,"results.py"))
            print "Processing data results directory:",datadir
            
            try:
                execfile(os.path.join(datadir,"results.py"),res)
                res=res["results"]
            except:
                continue
                print "File skipped, NaN found!" 
            
            try:
                status = res["status"]
            except:
                status = "OK assumed" #the status has not be propagated for old files

                antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
                positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])  

                # Comment: Still to check, which pulse definition most suitable for LDF ploting
                
                signal[res["polarization"]].extend(res[plot_parameter])

                    
        if res == {}:
            print "No results file found"
            sys.exit(0)
        
        #print "Number of dipoles found:",ndipoles
        
        timesec=res["TIME"]
        timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(timesec))

        timens=str(res["SAMPLE_INTERVAL"]*res["SAMPLE_NUMBER"]*1000)[0:3]
        time_stamp=timestr+"."+timens+"Z" 
        
        par["event_id"]= time_stamp
        
        par["antenna_set"] = res["ANTENNA_SET"]
        
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

    return par



class eventstatistics(tasks.Task):

    """
    This task will make some plots for event statitsics. Information about how to run will be added.     
    """ 


    parameters = dict(
        topdir=dict(default="/data/VHECR/LORAtriggered/results",doc="provide topdir",unit=""),
        eventslist=dict(default=lambda self:self.events if isinstance(self.events,list) else [self.events],doc="list with event names to process (directories in topdir)",unit=""),
        events=dict(default=["VHECR_LORA-20110612T231913.199Z"],doc="Event directories in topdir - either as list or as single string",unit=""), 
        results=dict(default=lambda self:GetInformationFromFile(self.topdir,self.eventslist),doc="Provide task with topdirectory",unit=""),
        direction_lora=dict(default=lambda self:self.results["loradirection"],doc="hArray of shower direction [az, el] as provided by Lora. Az is eastwards from north and El is up from horizon.", unit="degrees")
        # to add other quantities
        )
        
    def init(self):
        pass
    
    def call(self):
        pass
    
    def run(self):
    
        """
        Making all sorts of things. 
        
        """
        print self.eventslist
        print self.events
        print " This is where it will start doing statistics"
        print self.direction_lora
        
        
        
        

