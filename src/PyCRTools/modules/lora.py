"""LORA
"""

def loraTimestampToBlocknumber(lora_seconds, lora_nanoseconds, starttime, samplenumber, clockoffset = 1e4, blocksize = 2**16):
    """Calculates block number corresponding to LORA timestamp.
    The LORA timestamp is given in *lora_seconds* and *lora_nanoseconds*.
    From the LOFAR TBB event you need the *starttime* in seconds, the
    corresponding *samplenumber*, the *clockoffset* between LORA and LOFAR.
    Finally you need the *blocksize* used for reading the LOFAR data.
    """

    lora_samplenumber = (lora_nanoseconds - clockoffset) / 5

    value = (lora_samplenumber - samplenumber) + 2e8 * (lora_seconds - starttime)

    if value < 0:
        raise ValueError("Event not in file.")

    return int(value / blocksize)

def loraInfo(lora_second,datadir="/data/VHECR/LORAtriggered/LORA/",checkSurroundingSecond=True,silent=False):
    """ Reads in a file from LORA and returns a dictionary with important parameters. Az(imuth) is defined in degrees from North through East. El(evation) is in degrees from the horizon upwards.
    
    Input parameters:
    * lora_second * UTC timestamp of LORA event or filename
    * datadir * Directory of LORA event files
    * checkSurroundingSecond * Checks if there is a dataset of neighbouring seconds if not of this second
    * silent *  Print message of directory sought for

    """

    import os
    import time

    # Make sure directory ends with "/"
    datadir=datadir.rstrip("/")+"/"

    if not "LORAdata" in str(lora_second):
    # Get filename
        timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(lora_second))
        filename="LORAdata-"+timestr+".dat"
        if not os.path.isfile(datadir+filename):
            if checkSurroundingSecond:
	       if not silent:
	          print "Unable to find file, looking at neighbouring seconds"
               timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(lora_second+1))
               filename="LORAdata-"+timestr+".dat"
               if not silent:
	          print "file was not there. Now checking file",datadir+filename
               if not os.path.isfile(datadir+filename):
                  timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(lora_second-1))
                  filename="LORAdata-"+timestr+".dat"
                  if not silent:
	             print "file was not there. Now checking file",datadir+filename
                  if not os.path.isfile(datadir+filename):
                     print "File does not exist. Either the directory is wrong,the LORA event is not yet processed or there was no LORA trigger for this file"
                     return None
        else:
	    print "File",datadir+filename,"does not exist. Either the directory is wrong,the LORA event is not yet processed or there was no LORA trigger for this file. If the timestamp is just one second off you can try it again with set checkSurroundingSeconds = True"
	    return None
    else:
        filename=lora_second          
    file=open(datadir+filename)         
    lines=file.readlines()
    file.close()
    firstline=lines[0].strip('/').split()
    secondline=lines[1].strip('/').split()
    reference=['UTC_Time(secs)', 'nsecs', 'Core(X)', 'Core(Y)', 'El', 'Az', 'Energy(eV)']
    loradata={}
    for (a,b,c) in zip(firstline,reference,secondline):
        # "Check if data format is still as defined" 
        assert a==b
        loradata[a]=float(c)
    detectorindex=lines[3].strip('/').split()
    detectorreference=['Det_no.','X_Cord(m)','Y_Cord(m)','Z_Cord(m)','UTC_time','(10*nsecs)','Particle_Density']
    for (a,b) in zip(detectorindex,detectorreference):
        # "Check if data format is still as defined" 
        assert a==b
    detectorkeys=["detectorid","posX","posY","posZ","time","10*nsec","particle_density"]
    for k in detectorkeys:
        loradata[k]=[]
    for l in lines[4:]:
        info=l.split()
        for a,b in zip(detectorkeys[0:-1],info[0:-1]):
            loradata[a].append(int(b))
        loradata[detectorkeys[-1]].append(float(info[-1]))    
    return loradata

   
        

