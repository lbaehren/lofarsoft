#! /usr/bin/env python
# A prototype Cosmic Ray pipeline based on PyCRtools.
# Created by Arthur Corstanje, Jan 17, 2011. Email: a.corstanje@astro.ru.nl

import os
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
import subprocess
from optparse import OptionParser

from pycrtools import datacheck as dc
from pycrtools import rficlean as rf
from pycrtools import pulsefit as pf
from pycrtools import matching as match # possibly push this down to pulsefit?
from pycrtools import footprint as fp
from pycrtools import tasks
import pycrtools as cr
# import beamformer as bf

antennaset = 'LBA_OUTER' # hack around missing info in data files

def writeDictOneLine(outfile, dict):
    for key in dict:
        if type(dict[key]) == np.float64:
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s; ' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))

def writeDict(outfile, dict):
    for key in dict:
#        print '%s %s %s' % (key, dict[key], type(dict[key]))
        if type(dict[key]) == dict: # if we have a dict inside our dict, print it on one line (and hope it's small)
            writeDictOneLine(outfile, dict[key])
        elif type(dict[key]) == np.float64:
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s\n' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))
    outfile.write('\n')

def writeResultLine(outfile, pulseCountResult, triggerFitResult, fullDirectionResult, footprintResult, filename, timestamp, sampleNumber):
    d = triggerFitResult
    az = str(d["az"])
    el = str(d["el"])
    mse = str(d["mse"])
    outString = az + ' ' + el + ' ' + mse + ' '

    d = fullDirectionResult["odd"]
    az = str(d["az"])
    el = str(d["el"])
    R = str(d["R"])
    optHeightOdd = str(d["optValue"])
    outString += az + ' ' + el + ' ' + R + ' ' + optHeightOdd + ' '

    d = fullDirectionResult["even"]
    az = str(d["az"])
    el = str(d["el"])
    R = str(d["R"])
    optHeightEven = str(d["optValue"])
    outString += az + ' ' + el + ' ' + R + ' ' + optHeightEven + ' '

    d = pulseCountResult
    avgCount = str(d["avgPulseCount"])
    maxCount = str(d["maxPulseCount"])
    outString += avgCount + ' ' + maxCount + ' '

    summedPulseHeight = str(footprintResult["summedPulseHeight"])
    coherencyFactor = (float(optHeightOdd) + float(optHeightEven)) / float(summedPulseHeight) # very crude way of estimating coherency
    coherencyFactor = str(coherencyFactor)
    outString += summedPulseHeight + ' ' + coherencyFactor + ' '

    outString += str(timestamp) + ' '
    outString += str(sampleNumber) + ' '
    outString += filename
    outfile.write(outString + '\n')

def runAnalysis(files, outfilename, asciiFilename, blocksize = 2048, doPlot = False):
    """ Input: list of files to process, trigger info as read in by match.readtriggers(...), filename for results output
    """
    outfile = open(outfilename, mode='a')
    if not os.path.isfile(asciiFilename) or os.path.getsize(asciiFilename) < 10:
        asciiOutfile = open(asciiFilename, mode='a')
        headerString = 'trig.az trig.el trig.mse odd.az odd.el odd.R odd.height even.az even.el even.R even.height avgCount maxCount summedHeight coherency time samplenumber filename\n'
        asciiOutfile.write(headerString)
        asciiOutfile.flush() # so other threads will see it
    else:
        asciiOutfile = open(asciiFilename, mode='a') # hmm, duplicate code

    n = 0
    for file in files:
        n += 1
        print 'Processing file %d out of %d' % (n, len(files))
        result = dc.safeOpenFile(file, antennaset)
        #print result

        writeDict(outfile, result)
        if not result["success"]:
            continue
        crfile = result["file"] # blocksize is 2 * 65536 by default ('almost' entire file)
        # get all timeseries data
        cr_alldata = crfile["EMPTY_TIMESERIES_DATA"]
        nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
        crfile.getTimeseriesData(cr_alldata, 0)

#        crfile.set("blocksize", 32768)
#        import pdb; pdb.set_trace()
#        rf.cleanSpectrum(crfile)

        outfile.write('\n')
        # do quality check and rfi cleaning here
        result = dc.qualityCheck(crfile, cr_alldata, doPlot = doPlot)
        flaggedList = result["flagged"]
        writeDict(outfile, result)
        if not result["success"]:
            continue
        qualityCheckResult = result
        fileTimestamp = crfile["TIME"][0]
        fileSampleNumber = crfile["SAMPLE_NUMBER"][0]
        triggers = match.readtriggers(crfile)
        #import pdb; pdb.set_trace()
        if len(triggers) == 0:
            writeDict(outfile, dict(success=False, reason="Trigger file couldn't be read"))
            continue
        #print flaglist
        # find initial direction of incoming pulse, using trigger logs
        result = pf.initialDirectionFit(crfile, cr_alldata, fitType = 'linearFit')
        writeDict(outfile, result)       
        
        #result = pf.triggerMessageFit(crfile, triggers, 'linearFit') 
#        triggerMessageFit = tasks.pulsefittask.triggerMessageFit() # MOVE
#        print 'DOING NEW TRIGGER MSG FIT TASK'
#        triggerMessageFit(datafile = crfile, triggers = triggers, fitType = 'linearFit')
#        result = triggerMessageFit.result
#        print result
#        print 'DONE'
#        writeDict(outfile, result)
        result = pf.triggerMessageFit(crfile, triggers, 'linearFit')
        writeDict(outfile, result)
        if not result["success"]:
            continue
        triggerFitResult = result
        # cut out a block of length 'blocksize' containing the pulse
        pulseMidpoint = int(triggerFitResult["avgToffset"] * 200.0e6)
        cr_efield = cr.hArray(copy=cr_alldata, dimensions = [nofAntennas, blocksize])
        start = pulseMidpoint - blocksize/2
        stop = pulseMidpoint + blocksize/2
        cr_efield[...].copy(cr_alldata[..., start:stop])
        
        result = fp.footprintForCRdata(crfile, cr_efield, doPlot = doPlot)
        if not result["success"]:
            continue
        writeDict(outfile, result)
        footprintResult = result
        # now find the final direction based on all data, using initial direction as starting point
        try: # apparently it's dangerous...
            result = pf.fullDirectionFit(crfile, cr_efield, triggerFitResult, blocksize, flaggedList = flaggedList, FarField = False, method = options.method, doPlot = doPlot)
            fullDirectionResult = result
            writeDict(outfile, result)
            if not result["success"]:
                continue

        except (ZeroDivisionError, IndexError), msg:
            print 'EROR!'
            print msg
        writeResultLine(asciiOutfile, qualityCheckResult, triggerFitResult, fullDirectionResult, footprintResult,
                        crfile["FILENAME"], fileTimestamp, fileSampleNumber)
        bfEven = result["even"]["optBeam"]
        bfOdd = result["odd"]["optBeam"]

        if doPlot:
            bfEven.plot()
            raw_input("--- Plotted optimal beam for even antennas - press Enter to continue...")
#            frequency.read(datafile,"FREQUENCY_DATA")
            fftlength = blocksize/2 + 1
            spec = cr.hArray(complex, dimensions=[fftlength])
            spec.fftw(bfEven)
            power=cr.hArray(float,[fftlength],par=dict(logplot="y")) # xvalues=frequency
            power.spectralpower(spec)
            
            power.plot()
            raw_input("--- Plotted power spectrum of optimal beam for even antennas - press Enter to continue...")
            
            bfOdd.plot()
            raw_input("--- Plotted optimal beam for odd antennas - press Enter to continue...")

        outfile.flush()
    # end for
    outfile.close()
    asciiOutfile.close()
    #return (bfEven, bfOdd)

# get list of files to process
# Parse command line options
parser=OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
parser.add_option("-n", "--nthreads",
                  type="int", dest="nthreads", default=2,
                  help="Number of processes to use in parallel, default = 2")
parser.add_option("-b", "--blocksize",
                  type="int", dest="blocksize", default=2048,
                  help="Blocksize to use for simplex fitting of pulse direction+distance. Default is 2048")
parser.add_option("-m", "--method",
                  type="string", dest="method", default='smoothedAbs',
                  help="Pulse maximization method. Possible are 'abs', 'smoothedAbs' and 'smoothedPower'")
parser.add_option("-f", "--files",
                  type="string", dest="files", default='/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-9.h5',
                  help="Files to process")
parser.add_option("-o", "--outfilepath",
                  type="string", dest="outfilePath", default='./',
                  help="Path for output files")
parser.add_option("--doPlot", action="store_true", dest="doPlot", default=False,
                  help="Plots are done when this flag is set")

(options, args)=parser.parse_args()

nthreads = options.nthreads
datafiles = options.files
method = options.method
blocksize = options.blocksize
print 'Using %d threads' % nthreads
print 'Using pulse maximization method %s' % method

#if len(sys.argv) > 2:
#    datafiles = sys.argv[1]
#    triggerMessageFile = sys.argv[2]
#    nthreads = int(sys.argv[2])
#    print 'Using %d threads' % nthreads
#elif len(sys.argv) > 1:
#    datafiles = sys.argv[1]
#    print 'Taking default trigger message file (i.e. name constructed from date and station name in the hdf5 data file).'
#else:
#    print 'No files given on command line, using a default set instead.'
#    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/automatic_obs_test-15febOvernight--147-10*.h5'
#    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/automatic_obs_test-15febOvernight--147-441.h5'
#    datafiles = '/Users/acorstanje/triggering/MACdatarun_2feb2011/automatic_obs_test-2feb-2-26.h5'
#    datafiles = '/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-9.h5'

sortstring = 'sort -n --field-separator="-" --key=18'
outfile = options.outfilePath + 'crPipelineResults.txt'
outfileAscii = options.outfilePath + 'asciiPipelineResults.txt'
antennaset="LBA_OUTER"

#fd = os.popen('ls '+ datafiles+' | ' + sortstring)
#p1 = subprocess.Popen(['ls '+ datafiles + ' | ' + sortstring], shell=True, stdout=subprocess.PIPE)
(directory, files) = os.path.split(datafiles)
p1 = subprocess.Popen(['find ' + directory + ' -type f -name "'+ files + '" | ' + sortstring], shell=True, stdout=subprocess.PIPE)

output = p1.communicate()[0]
files = output.splitlines()
nofiles = len(files)
print "Number of files to process:", nofiles

if nofiles > nthreads:
    print '--- Spawning new processes for each file ---'
    thisScriptsPath = os.environ['LOFARSOFT'] + '/src/CR-Tools/implement/Pypeline/scripts/crpipeline.py'
    processes = []
    for i in range(nthreads):
        thisProcess = subprocess.Popen([thisScriptsPath, '--files='+files[i], '--method='+options.method, '--outfilepath='+options.outfilePath, '--blocksize='+options.blocksize])
        processes.append(thisProcess)
    i = nthreads
#    i = 2
    while i < nofiles:
        time.sleep(0.2)
        for k in range(nthreads):
            if processes[k].poll() != None:
                print 'Going to do: %s' % files[i]
                processes[k] = subprocess.Popen([thisScriptsPath, '--files='+files[i], '--method='+options.method, '--outfilepath='+options.outfilePath, '--blocksize='+options.blocksize])
                i += 1
                
else:
    runAnalysis(files, outfile, outfileAscii, blocksize = options.blocksize, doPlot = options.doPlot)
#fitergs = dict()




#       n_az = len(fitDataEven[2][0, 0])
#  n_el = len(fitDataOdd[2][1, 0])
#  az_max = fitDataEven[2][0].max()
#  az_min = fitDataEven[2][0].min()
#  el_max = fitDataEven[2][1].max()
#  el_min = fitDataEven[2][1].min()

#  pixarray = np.zeros( (n_el,n_az) )
#  pixarray = -1.0 * fitDataEven[3].T # of transpose?
#  plt.imshow(pixarray,cmap=plt.cm.hot,extent=(az_min, az_max, el_min, el_max) )
