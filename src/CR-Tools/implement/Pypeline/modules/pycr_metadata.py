"""This module reads in calibration metadata from file in the early fases of LOFAR. In the future this should be replaced by reading the metadata from the files.
"""

## Imports
import pycrtools as hf
import numpy as np
import struct
import os

def getStationPhaseCalibration(station, antennaset):
    """Read phase calibration data for a station.

    *station* station name or ID.
    *mode* observation mode. Currently supported: "LBA_OUTER"

    returns weights for 512 subbands.

    Examples (also for doctests):

    >>> pycr_metadata.station_phase_calibration_factors("CS302","LBA_OUTER")
    
    >>> pycr_metadata.station_phase_calibration_factors(122,"LBA_OUTER")
     
    """

    
    # Return mode nr depending on observation mode 
    if antennaset in [ "LBA_OUTER" , 1, "1" ]:
        modenr="1"
    else:
        print "Calibration data not yet available. Returning 1"
        complexdata=np.zeros(shape=(96,512),dtype=complex)
        complexdata.real=1
        return complexdata

    # Convert station name to file identifier
    if   station in [ "CS002",2,"002" ]:
        stationname = "002"
    elif station in [ "CS003",3,"003" ]:
        stationname = "003"
    elif station in [ "CS004",4,"004" ]:
        stationname = "004"
    elif station in [ "CS005",5,"005" ]:
        stationname = "005"
    elif station in [ "CS006",6,"006" ]:
        stationname = "006"
    elif station in [ "CS007",7,"007" ]:
        stationname = "007"
    else:
        print "Calibration data not yet available. Returning 1"
        complexdata=np.zeros(shape=(96,512),dtype=complex)
        complexdata.real=1
        return complexdata

    # filename
    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    filename=LOFARSOFT+'/data/calibration/PhaseCalibration/CalTable_'+stationname+'_mode'+modenr+'.dat'
    file=open(filename)

    # reading in 96 * 512 * 2 doubles
    fmt='98304d'
    
    # Calculate the size to be read
    sz=struct.calcsize(fmt)

    # read from the file
    rawdata=file.read(sz)

    # unpack so the data is represented as doubles
    data=struct.unpack(fmt,rawdata)

    # 
    data=np.array(data)
    data.resize(512,96,2)
    
    complexdata=np.empty(shape=(512,96),dtype=complex)
    complexdata.real=data[:,:,0]
    complexdata.imag=data[:,:,1]
        
    return complexdata.transpose()

def getCableDelays(station,antennaset):
    """ Get cable delays in s. 
    
    *station*  Station name or ID e.g. "CS302", 142
    *antennaset*   Set of antennas e.g. LBA_OUTER, HBA
    
    returns "numpy array of (rcus * cable delays ) for all dipoles in a station"
    """

    # Check station id type
    if isinstance(station, int): 
        # Convert a station id to a station name
        station=idToStationName(station)
    # LBA_OUTER = LBL
    # LBA_INNER = LBH
    # HBA = HBA
    if "LBA_OUTER" in antennaset:
        rcu_connection="LBL"
    elif "LBA_INNER" in antennaset:
        rcu_connection="LBH"
    elif "HBA" in antennaset:
        rcu_connection="HBA"
    else:
        print "Antenna set not yet supported"
        return "Antenna set not yet supported"

    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    cabfilename=LOFARSOFT+'/data/calibration/CableDelays/'+station+'-CableDelays.conf'
    cabfile=open(cabfilename)
   
    cable_delays=np.zeros(96)

    str_line='' 
    while "RCUnr" not in str_line:
        str_line = cabfile.readline()

    str_line = cabfile.readline()
    for i in range(96):
        str_line = cabfile.readline()
        sep_line = str_line.split()
        if rcu_connection == "LBL":
            cable_delays[int(sep_line[0])]=float(sep_line[2])*1e-9
        elif rcu_connection == "LBH":
            cable_delays[int(sep_line[0])]=float(sep_line[4])*1e-9
        elif rcu_connection == "HBA":
            cable_delays[int(sep_line[0])]=float(sep_line[6])*1e-9

    return cable_delays

def idToStationName(station_id):
    """Returns the station name from a station_id
    The station_id is crfile["antennaIDs"]/1000000
    The station name is more commonly used to identify the station
    for example for the files with metadata"""

    if ( station_id > 255 ):
        print "Unvalid station id"
        return "Unvalid station id"
    digit1=station_id/100
    digit2=np.mod(station_id,100)/10
    digit3=np.mod(station_id,10)
    
    if digit1 == 0:
        station_name="CS"+str(digit1)+str(digit2)+str(digit3)
    elif digit1 == 2:
        digit1=6
        if digit2 != 0:
            print "Unkown international station please add it to the list"
            return "Unknown international station"
        if digit3 > 4:
            print "Unkown international station please add it to the list"
            return "Unknown international station"
        if digit3 <= 4:
            station_nr=digit1*100+digit2*10+digit3;
            station_name="DE"+str(station_nr)
    elif digit1==1:
        station_nr=digit1*100+(digit2/2)*100+np.mod(digit2,2)*10+digit3
        if digit3<=2:
            station_name="CS"+str(station_nr)
        else:
            station_name="RS"+str(station_nr)

    return station_name

def getRelativeAntennaPositions(station,antennaset,return_as_hArray=False):
    """Returns the antenna positions of all the antennas in the station
    relative to the station center for the specified antennaset. 
    station can be the name or id of the station. Default returns as numpy
    array, option to return as hArray.

    *station*      Name or id of the station. e.g. "CS302" or 142
    *antennaset*   Antennaset used for this station. Options:
                   LBA_INNER
                   LBA_OUTER
                   LBA_X
                   LBA_Y
                   LBA_SPARSE0
                   LBA_SPARSE1
                   HBA_0
                   HBA_1
                   HBA
    *return_as_hArray*  Return as hArray.

    Examples:
    >>> import pycr_metadata as md
    >>>
    >>> ant_pos=md.get_antenna_positions(142,"LBA_INNER",True)
    >>> ant_pos
    hArray(float,[96, 3]) # len=288, slice=[0:288], vec -> [-0.0,0.0,-0.0,-0.0,0.0,-0.0,-0.0004,2.55,...]
    
    
    >>> ant_pos=md.get_antenna_positions("CS005","HBA",False)
    >>> ant_pos 
    array([[  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
           [  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
            [  1.28090000e+01,   2.88400000e+00,   1.00000000e-03],....
    
    
    >>> antenna_ids=file["antennaIDs"]
    [1420000005,142000008,142000080]
    >>> station_id=antenna_ids[0]/1000000
    142
    >>> rcu_ids=np.mod(antenna_ids,1000)
    array([5, 8, 80])
    >>> all_antenna_pos=md.get_antenna_positions(station_id,"LBA_INNER",False)
    >>> used_antenna_pos=all_antenna_pos[rcu_ids]
    array([[  2.25000000e+00,   1.35000000e+00,  -1.00000000e-03],
           [  4.00000000e-04,  -2.55000000e+00,   1.00000000e-03],
           [  8.53000000e-01,   1.37240000e+01,  -3.00000000e-03]])
    
    """

    # Known antennasets
    names = ['LBA_INNER', 'LBA_OUTER', 'LBA_X', 'LBA_Y', 'LBA_SPARSE0', 'LBA_SPARSE1', 'HBA_0', 'HBA_1', 'HBA']

    # Check if requested antennaset is known
    assert antennaset in names

    # Check station id type
    if isinstance(station, int): 
        # Convert a station id to a station name
        station=idToStationName(station)

    # Obtain filename of antenna positions
    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    filename=LOFARSOFT+"data/calibration/AntennaArrays/"+station+"-AntennaArrays.conf"
    
    # Open file
    f = open(filename, 'r') 

    # Find position of antennaset in file
    str = ''
    while antennaset != str.strip():
        str = f.readline()
    
    # Skip name and station reference position
    str = f.readline()
    str = f.readline()

    # Get number of antennas and the number of directions
    nrantennas = int(str.split()[0])
    nrdir = int(str.split()[4])

    antpos = []
    for i in range(nrantennas):
        line = f.readline().split()

        # Odd numbered antennas
        antpos.append([float(line[0]),float(line[1]),float(line[2])])

        # Even numbered antennas
        antpos.append([float(line[3]),float(line[4]),float(line[5])])
   
    # Return requested type
    if return_as_hArray:
        antpos=hf.hArray(antpos,[2*int(nrantennas),int(nrdir)])
    else:
        antpos=np.asarray(antpos).reshape(2*int(nrantennas),int(nrdir))
   
    return antpos

def getClockCorrection(station,time=1278480000):
    """Get clock correction for superterp stations in seconds. Currently static values.
    
    *station* Station name or number for which to get the correction.
    *time* Optional. Linux time of observation. As clocks drift the value from the correct time should be given. Not yet implemented.
    """
    # Convert a station id to a station name
    if type(station)==type(1): # name is a station_id number
        station=idToStationName(station)

    clockcorrection={}
    clockcorrection["CS002"]=8.2724449975934222e-06
    clockcorrection["CS003"]=6.8756447631247787e-06
    clockcorrection["CS004"]=7.8462185662334179e-06
    clockcorrection["CS005"]=8.4978152695672203e-06
    clockcorrection["CS006"]=7.8374740231534724e-06
    clockcorrection["CS007"]=7.8673363857885218e-06
    
    if station in clockcorrection.keys():
        return clockcorrection[station]
    else:
        print "NO VALUE FOUND IN THE DATABASE"
        return 0

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

