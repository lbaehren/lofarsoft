from pycrtools import *
from pycrtools.tasks import findrfi, calibratefm

def getDatafilesForEventID(eventID, datadir = '/vol/astro/lofar/vhecr/lora_triggered/data'):
    eventTime = datetime.datetime(2010, 1, 1) + datetime.timedelta(seconds = id)
    # make date+time string
    s = eventTime.strftime("%Y%m%dT%H%M%S")
    filefilter = os.path.join(datadir, '*'+s+'*.h5')
    filelist = listFiles(filefilter)

    return filelist


filelist = listFiles("/Users/acorstanje/triggering/CR/caltestevent/L94481_D20130217T094022.078Z_CS0[01][134567]_R000_tbb.h5")
#filelist = listFiles("/Volumes/WDdata/octocopter/data/L65345_D20120919T120330.995Z_CS0[0][35]_R000_tbb.h5")
outdir = "/Users/acorstanje/triggering/CR/calibratefm"

#filelist = listFiles('/vol/astro/lofar/caltest/L62972_D20120801T100626.433Z_CS00?_R000_tbb.h5')
#outdir = '/vol/astro/lofar/caltest/calfmtest'
#filelist = listFiles("/Users/acorstanje/triggering/CR/*.986Z*.h5")

f = open(filelist)

#f = open(['/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS002_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS004_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS007_R000_tbb.h5'])

f["BLOCKSIZE"] = 8000
pol = 1
selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == pol]

f["SELECTED_DIPOLES"] = selected_dipoles
print selected_dipoles
rfi = trerun("FindRFI", "1", f = f, nofblocks = 200, save_plots=True) # may specify refant here
bad_antenna_ids = rfi.bad_antennas
bad_antenna_indices = [selected_dipoles.index(x) for x in bad_antenna_ids]

event_plot_prefix = os.path.join(outdir, "calibratefm-{0}-".format(1))

phase_average = rfi.phase_average
phase_RMS = rfi.phase_RMS
median_phase_spreads = rfi.median_phase_spreads
lines = [88.0, 88.6, 90.8]#, 91.8]#, 94.8]
delays5 = dict()
for station in f["STATION_LIST"]:
    delays5[station] = []

n=0
strongestLines = []

for line in lines:
#    print rfi.phase_average.checksum()
#    print rfi.median_phase_spreads.checksum()
    print line
    calfm = trerun("CalibrateFM", str(n), f = f, phase_RMS = hArray(copy=phase_RMS), phase_average = hArray(copy=phase_average), median_phase_spreads = hArray(copy=median_phase_spreads), lines = None, freq_range = [line-0.1, line+0.1], refant=rfi.refant, referenceTransmitterGPS = (6.403565, 52.902671), correctOneSampleShifts = True, flagged_antennas=bad_antenna_indices, doplot=False, testplots=False, save_plots=False, plot_prefix=event_plot_prefix)
    n+=1
    for station in f["STATION_LIST"]:
        delays5[station].append(calfm.interStationDelays[station])

    strongestLines.append(calfm.bestFrequency / 1.0e6) # the actual strongest line found, e.g. 88.625 MHz when 88.6 was given

lines = strongestLines # use the strongest frequencies in what follows.

for station in f["STATION_LIST"]:
    print '----'
    print ' '
    print 'Delays:'
    for delay in delays5[station]:
        print '%5.3f' % delay

    theseDelays = np.array(delays5[station])
    d = - theseDelays[0]
    moduli = []
    for line in lines:
        moduli.append(200.0 / line * 5.0)

    wrappeddelays = np.zeros(len(lines))
    goodness = np.zeros(2000)
    minimumGoodness = 1e9
    bestDelay = 0.0
    for k in range(2000):
        d = theseDelays[0] + k * moduli[0] # real delay
        for n in range(len(lines)):
            wrappeddelays[n] = d % moduli[n]
    #        wrappeddelays[n] %= moduli[n]
            if wrappeddelays[n] > moduli[n] / 2:
                wrappeddelays[n] -= moduli[n]
        goodness[k] = max(abs(wrappeddelays - theseDelays))
        if goodness[k] < minimumGoodness:
            bestDelay = d
            minimumGoodness = goodness[k]

    plt.figure()
    x = np.arange(2000.0)
    x *= moduli[0]
    x += theseDelays[0]
    plt.plot(x, goodness, 'o')
    plt.title(station)

    print 'Deviations: '
    for n in range(len(lines)):
        print '%3.4f MHz: %5.3f' % (lines[n], theseDelays[n] - bestDelay % (200.0 / lines[n] * 5.0))

    print 'Deviation bestdelay - 11.3636 ns'
    bestDelay += 200.0 / lines[0] * 5.0
    for n in range(len(lines)):
        print '%3.4f MHz: %5.3f' % (lines[n], theseDelays[n] - bestDelay % (200.0 / lines[n] * 5.0))


#calfm = trerun("CalibrateFM", "x", f = f, phase_RMS = rfi.phase_RMS, phase_average = rfi.phase_average, median_phase_spreads = rfi.median_phase_spreads, freq_range = [10, 70], direction_resolution = [0.1, 5], refant=rfi.refant, referenceTransmitterGPS = None, correctOneSampleShifts = True, doplot=True, testplots=True)
#[88.0, 88.6, 90.8, 91.8, 94.8]
