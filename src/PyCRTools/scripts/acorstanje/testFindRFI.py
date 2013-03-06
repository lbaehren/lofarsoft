from pycrtools import *
from pycrtools import metadata as md
from pycrtools.tasks import findrfi, calibratefm
from sys import stdout
import os
filelist = listFiles("/Users/acorstanje/triggering/CR/caltestevent/*.h5")
#filelist = listFiles("/Users/acorstanje/triggering/CR/*.986Z*.h5")
#filelist = listFiles("/Users/acorstanje/triggering/CR/paperevent_90373019/*.h5")
outdir = "/Users/acorstanje/triggering/CR/goodlist_results/"

if len(sys.argv) > 2:
    filefilter = sys.argv[1]
    filelist = listFiles(filefilter)
    outdir = sys.argv[2]

f = open(filelist)

#f = open(['/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS002_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS004_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS007_R000_tbb.h5'])

f["BLOCKSIZE"] = 2048
#pol = 1
#selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == pol]

#f["SELECTED_DIPOLES"] = selected_dipoles

nofChannels = f["NOF_SELECTED_DATASETS"]

rfi = trerun("FindRFI", "1", f = f, nofblocks = 5000000, save_plots=True, testplots=True, plot_title=False) # may specify refant here
np.seterr(divide='ignore') # ignore divide by zero errors
yy = rfi.average_spectrum.toNumpy()
freqs = f["FREQUENCY_DATA"].toNumpy() * 1e-6
stationname = md.idToStationName( int(f["DIPOLE_NAMES"][0][0:3]))
antennaset = f["ANTENNA_SET"][0]
for i in range(nofChannels):
    print '\rWriting figure %d' % i,
    stdout.flush()
    y = yy[i]
    totalpower = sum(y)
    logspectrum = np.log(y)
    plt.clf()
    plt.plot(freqs, logspectrum, c='b')
    plt.title('Average spectrum for one antenna\nStation '+ stationname + ', ' + antennaset+', rcu '+ str(i)+', total power = {0:2.2f}'.format(totalpower))
    plt.xlabel('Frequency [MHz]')
    plt.ylabel('log-spectral power [adc units]')
#            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)
    p = os.path.join(outdir, "average_spectrum-rcu{0:03d}.png".format(i))
    plt.savefig(p, dpi=200)
    #self.plotlist.append(p)

yy = rfi.median_average_spectrum.toNumpy()
totalpower = sum(yy)
logspectrum = np.log(yy)
plt.clf()
plt.plot(freqs, logspectrum, c='b')
plt.title('Median-average spectrum over all antennas\nStation '+stationname+', ' + antennaset+', total power = {0:2.2f}'.format(totalpower))
plt.xlabel('Frequency [MHz]')
plt.ylabel('log-spectral power [adc units]')
#            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)
p = os.path.join(outdir, "median-average_spectrum.pdf")
plt.savefig(p)
