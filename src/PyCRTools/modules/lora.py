"""LORA
"""
import pycrtools as cr
import re


def loraTimestampToBlocknumber(lora_seconds, lora_nanoseconds, starttime, samplenumber, clockoffset=1e4, blocksize=2 ** 16):
    """Calculates block number corresponding to LORA timestamp and the
    sample number within that block (i.e. returns a tuple
    (``blocknumber``,``samplenumber``)).

    Input parameters:

    =================== ==============================
    *lora_seconds*      LORA timestamp in seconds (UTC timestamp, second after 1st January 1970).
    *lora_nanoseconds*  LORA timestamp in nanoseconds.
    *starttime*         LOFAR_TBB timestamp.
    *samplenumber*      Sample number.
    *clockoffset*       Clock offset between LORA and LOFAR.
    *blocksize*         Blocksize of the LOFAR data.
    =================== ==============================

    """

    lora_samplenumber = (lora_nanoseconds - clockoffset) / 5

    value = (lora_samplenumber - samplenumber) + 2e8 * (lora_seconds - starttime)

    if value < 0:
        raise ValueError("Event not in file.")

    return (int(value / blocksize), int(value % blocksize))


def loraInfo(lora_second, datadir="/data/VHECR/LORAtriggered/LORA/", checkSurroundingSecond=True, silent=False):
    """ Reads in a file from LORA and returns a dictionary with
    important parameters, such as the core position (``core``), the
    energy (``energy``) and the direction (``direction``) of the
    shower. For the direction the Az(imuth) is defined in degrees from
    North through East. El(evation) is in degrees from the horizon
    upwards.

    Input parameters:

    ======================== =========================
    *lora_second*            UTC timestamp of LORA event or filename
    *datadir*                Directory of LORA event files
    *checkSurroundingSecond* Checks if there is a dataset of neighbouring seconds if not of this second
    *silent*                 Print message of directory sought for
    ======================== =========================

    """

    import os
    import time

    # Create output
    loradata = {}

    # Make sure directory ends with "/"
    datadir = datadir.rstrip("/") + "/"

    if not "LORAdata" in str(lora_second):
    # Get filename
        timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(lora_second))
        filename = "LORAdata-" + timestr + ".dat"
        # print os.path.isfile(datadir+filename)
        if not os.path.isfile(datadir + filename):
            if checkSurroundingSecond:
                if not silent:
                    print "Unable to find file, looking at neighbouring seconds"
                timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(lora_second + 1))
                filename = "LORAdata-" + timestr + ".dat"
                if not silent:
                    print "file was not there. Now checking file", datadir + filename
                if not os.path.isfile(datadir + filename):
                    timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(lora_second - 1))
                    filename = "LORAdata-" + timestr + ".dat"
                    if not silent:
                        print "file was not there. Now checking file", datadir + filename
                    if not os.path.isfile(datadir + filename):
                        print "File does not exist. Either the directory is wrong,the LORA event is not yet processed or there was no LORA trigger for this file"
                        return None
            else:
                print "1 File", datadir + filename, "does not exist. Either the directory is wrong,the LORA event is not yet processed or there was no LORA trigger for this file. If the timestamp is just one second off you can try it again with set checkSurroundingSeconds = True"
                return None
    else:
        filename = lora_second

    loradata["datafile"] = datadir + filename
    loradata["ldf"] = datadir + filename[:-4] + ".png"
    file = open(datadir + filename)
    lines = file.readlines()
    file.close()
    if len(lines) < 2:
        return None
    firstline = lines[0].strip('/').split()
    secondline = lines[1].strip('/').split()
    # Coreuncertainties and Moliere radius have been added later
    reference = ['UTC_Time(secs)', 'nsecs', 'Core(X)', 'Core(Y)', 'Elevation', 'Azimuth', 'Energy(eV)', 'CoreE(X)', 'CoreE(Y)', 'Moliere_rad(m)', 'ElevaErr', 'AziErr', 'EnergyErr(eV)']
    len_firstline = len(firstline)

    if len_firstline != 13:
        if len_firstline != 10:
            if len_firstline != 9:
                if len_firstline != 7:
                    print "Check LORA file format, update version ?!"
                    assert False
    for (a, b, c) in zip(firstline, reference[0:len_firstline], secondline):
        # "Check if data format is still as defined"
        assert a == b
        loradata[a] = float(c)
    detectorindex = lines[3].strip('/').split()
    detectorreference = ['Det_no.', 'X_Cord(m)', 'Y_Cord(m)', 'Z_Cord(m)', 'UTC_time', '(10*nsecs)', 'Particle_Density(/m2)']
    for (a, b) in zip(detectorindex, detectorreference):
        # "Check if data format is still as defined"
        assert b == a
    detectorkeys = ["detectorid", "posX", "posY", "posZ", "time", "10*nsec", "particle_density(/m2)"]
    for k in detectorkeys:
        loradata[k] = []
    for l in lines[4:]:
        info = l.split()
        for a, b in zip(detectorkeys[0:-1], info[0:-1]):
            # print a,b
            loradata[a].append(float(b))
        loradata[detectorkeys[-1]].append(float(info[-1]))

    loradata["core"] = cr.hArray([loradata["Core(X)"], loradata["Core(Y)"], 0.], name="shower core parameters from lora", unit="m")
    loradata["energy"] = loradata["Energy(eV)"]
    loradata["direction"] = cr.hArray([loradata["Azimuth"], loradata["Elevation"]], name="shower direction from lora", unit="degrees")

    # Paramaters added in later versions, keeping compatibility with old versions
    if len_firstline >= 9:
        loradata["coreuncertainties"] = cr.hArray([loradata["CoreE(X)"], loradata["CoreE(Y)"], 0], name="shower core uncertainties from lora", unit="m")
    else:
        loradata["coreuncertainties"] = cr.hArray([-1., -1., 0.], name="shower core uncertainties default", unit="m")

    if len_firstline >= 10:
        loradata["moliere"] = loradata['Moliere_rad(m)']
    else:
        loradata["moliere"] = -1

    loradata_cleankeys = {}
    for key in loradata.keys():
        cleankey = re.sub('\W', '_', key)
        cleankey = re.sub('_+$', '', cleankey)
        loradata_cleankeys[cleankey] = loradata[key]

    return loradata_cleankeys


def nsecFromSec(lora_second, logfile="/data/VHECR/LORAtriggered/LORA/LORAtime4"):
    """
    Get nanosecond timing from LORA logfile.

    Input parameters:

    ============== ==============================
    *lora_second*  Second from which to obtain the nanosecond time info.
    *logfile*      LORA logfile from which to obtain the time info.
    ============== ==============================

    Output:

    Tuple containing LORA second and nanosecond timing info.
    """
    f = open(logfile, 'r')
    lines = f.readlines()
    p = {}
    for l in lines:
        lspl = l.split()
        p[int(lspl[0])] = int(lspl[1])
    if lora_second in p.keys():
        return (lora_second, p[lora_second])
    elif lora_second - 1 in p.keys():
        return (lora_second - 1, p[lora_second - 1])
    elif lora_second + 1 in p.keys():
        return (lora_second + 1, p[lora_second + 1])
    else:
        return (None, None)


def plotReceived(logfile="/data/VHECR/LORAtriggered/LORA/LORAreceived", days_to_average=1, cumulative=True, plotAllowed=True, plotNotAllowed=True, plotNoObservation=True, stacked=True):
    """
    Plot the triggers that were received from LORA at LOFAR. The trigger can be
    handled in three ways. Either there is no observation, so nothing is done.
    If there is an observation, it can do two things. Either triggering was
    allowed and TBB data should have been obtained, or triggering was not
    allowed and no TBB data was obtained.

    Input parameters:

    ===================  =====
    *logfile*            location of LORAreceived file
    *days_to_average*    plot histogram averaged of these many days (<1 allowed)
    *cumulative*         intgrate over time
    *plotAllowed*        plot triggers that were allowed to obtain TBB data
    *plotNotAllowed*     plot triggers that were not allowed to obtain TBB data
    *plotNoObservation*  plot triggers during a time when no observation was running
    *stacked*            stack the triggers together
    ===================  =====

    """
    import matplotlib.pyplot as plt
    from numpy import arange
    import time
    # Read in logfile
    file = open(logfile)
    lines = file.readlines()
    # Only send the lines triggered by LORA
    lines = [l for l in lines if 'LORA' in l]
    # Select which type of trigger the timestamp belongs to
    allowed = [int(l.split()[0]) for l in lines if 'allowed' in l and 'not' not in l]
    notallowed = [int(l.split()[0]) for l in lines if 'allowed' in l and 'not' in l]
    noobservation = [int(l.split()[0]) for l in lines if 'no observation' in l]
    # Filter out false timestamps that occasionally occor
    allowed = [a for a in allowed if a > 1000000000]
    notallowed = [a for a in notallowed if a > 1000000000]
    noobservation = [a for a in noobservation if a > 1000000000]
    # Convert to MJD
    allowed = [a / 86400. + 40587 for a in allowed]
    notallowed = [a / 86400. + 40587 for a in notallowed]
    noobservation = [a / 86400. + 40587 for a in noobservation]

    plotdata = []
    plotlength = 0
    mylegend = []
    if plotAllowed:
        plotdata.append(allowed)
        mylegend.append('allowed')
        plotlength += 1
    if plotNotAllowed:
        plotdata.append(notallowed)
        mylegend.append('not allowed')
        plotlength += 1
    if plotNoObservation:
        plotdata.append(noobservation)
        mylegend.append('no observation')
        plotlength += 1

    if stacked:
        histtype = 'barstacked'
    else:
        histtype = 'bar'

    if plotlength == 1:
        plotdata = plotdata[0]

    startdate = int(min(allowed[0], notallowed[0], noobservation[0]))
    enddate = int(max(allowed[-1], notallowed[-1], noobservation[-1])) + 2
    if days_to_average > enddate - startdate:
        days_to_average = enddate - startdate
    bins = arange(startdate, enddate, days_to_average)
    plot = plt.hist(plotdata, bins=bins, histtype=histtype, cumulative=cumulative, label=mylegend)

    plt.legend()
    plt.title('Triggers send by LORA and received by LOFAR \n current date ' + str(time.time() / 86400 + 40588)[0:8])

    return plot
