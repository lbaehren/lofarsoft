from pycrtools import *
from pycrtools.tasks import findrfi, calibratefm, wavefront_full

#filelist = listFiles("/Users/acorstanje/triggering/CR/caltestevent/*.h5")
#filelist = listFiles("/Users/acorstanje/triggering/CR/*.986Z*.h5")
#filelist = listFiles("/Users/acorstanje/triggering/CR/goodlist/*.371Z_CS00*.h5")

#f = open(filelist)

f = open(['/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS002_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS004_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5', '/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS007_R000_tbb.h5'])

f["BLOCKSIZE"] = 65536
pol = 0
selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == pol]

f["SELECTED_DIPOLES"] = selected_dipoles

#rfi = trerun("FindRFI", "1", f = f, nofblocks = 300, save_plots=True, testplots=True, plot_title=False) # may specify refant here

wf = trerun("Wavefront_full", "1", f=f, loradir='/Users/acorstanje/triggering/CR/LORAnew', interStationDelays = None)


