## This is a template Python module.
#  Use this template for each module in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For scripts (e.g. files that you want to execute from the commandline)
#  you should use scripts/template.py instead.

"""This module provides function to do (incoherent) dedispersion.

"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.

import numpy as np
# import matplotlib.pyplot as plt
# import pycrtools as hf


def freq_to_delay(DM, freq, timeresolution=1):
    """This is a docstring, each function should have one.

    *DM* Dispersion measure in cm / pc^3
    *freq* Numpy vector of frequencies in Hz
    *timeresolution* Time resolution in seconds (default 1 sec)
    Examples (also for doctests):
    Returns list of delays in units of the provided timeresolution

    >>> example_function(1,2)
    (1, 2)
    """

    ref_freq_sqr = np.square(max(freq) / 1e9)
    freq_sqr = np.square(freq / 1e9)
    offsets = 1 / freq_sqr - 1 / ref_freq_sqr
    offsets *= 1 * 4.15e-3 * DM / timeresolution

    return offsets


def dedisperse_array(dynspec, DM, frequencies, timeresolution):
    """Dedipserse an array. Returns an array with dedispersed values
    of the same length as the timeaxis of the array. Data will be dedispersed
    to the original time axis of the highest frequency.

    *dynspec*        Two dimensional numpy array. First axis frequency,
                     second axis time
    *DM*             Dispersion for which to dedisperse the data (in cm/pc^3)
    *frequencies*    numpy array of frequency values of the frequency axis
                     of the dynspec
    *timeresolution* Resolution of the time axis of the array

    returns array with dedispersed data values

    """

    offsets = freq_to_delay(DM, frequencies, timeresolution)

    dshape = dynspec.shape
    print dshape
    nrfreq = dshape[0]
    nrtime = dshape[1]
    ddshape = [nrtime]
    for num in range(2, len(dynspec.shape)):
        ddshape.append(dynspec.shape[num])
    dedisp_data = np.zeros(ddshape, dtype=float)

    nrtime -= max(offsets)

    # Check if data can be dispersed at all
    if nrtime < 0:
        print "Cannot dedisperse data"
        return dedisp_data

    for freq in range(nrfreq):
        dedisp_data[0:nrtime] += dynspec[freq, int(offsets[freq]):int(offsets[freq]) + nrtime]

    return dedisp_data


def dedisperse_fitsimage(filename, DM, outfilename, nfreq_bands=1, integrationtime=1, rfimitigation=0, fstart=0, fend=None):
    """Dedipserse a fits image. Writes the dedispersed image to the 3rd argument. values
    of the same length as the timeaxis of the array. Data will be dedispersed
    to the original time axis of the highest frequency. Time axis should contain keyword "TIME" and be given in seconds, Frequency axis should contain keyword "FREQ". "CTYPE#", "CRVAL#" , "CRPIX#", "CUNIT" and "CDELT#" are used to specify coordinates.

    *filename*       Name of image to dedisperse
    *DM*                   Dispersion for which to dedisperse the data (in cm/pc^3)
    *outfilename*    Name to save the image to
    *nfreq_bands*          Number of frequency bands for the dedisperse data
    *integrationtime *     Number of samples to integrate the image over
    *rfimitigation*        Cut values above (rfimitagation*mean_value ). Default = 0 means off.
    *fstart*               Frequency number from which to start dedispersion (default=0)
    *fend*                 Frequency number up to which dedispersion should be performed (default=None, means till end)

    >>> example:

    filename='/Users/STV/Astro/data/TBB/Crab/pulseNov2010/crab_pulse_image-20101104.fits'
    outfilename='/Users/STV/Astro/data/TBB/Crab/pulseNov2010/crab_pulse_movie_2.fits'
    DM=56.8
    nfrequencybands=3 # Makes 3 bands
    integrationtime=4 # samples
    rfimitigationlevel=4 # Cut off values above 4 * mean ( image )
    fstart=0
    fend=None

    import dedispersion as dd

    dd.dedisperse_fitsimage(filename,DM,outfilename,nfrequencybands,integrationtime, rfimitigationlevel, fstart, fend)



    """

    import pyfits

    subdivisions = nfreq_bands
    dynspec = pyfits.getdata(filename)

    if rfimitigation > 0:
        dynspec[dynspec > rfimitigation * dynspec.mean()] = 0

    # Obtain metadata
    hdr = pyfits.getheader(filename)

    metadata = {}
    for key, value in hdr.items():
        metadata[key] = value

    # Obtain time and frequency axes
    naxis = metadata['NAXIS']

    for ax_nr in range(1, 1 + naxis):
        if 'TIME' in metadata['CTYPE' + str(ax_nr)]:
            time_ax_nr = ax_nr

    for ax_nr in range(1, 1 + naxis):
        if 'FREQ' in metadata['CTYPE' + str(ax_nr)]:
            freq_ax_nr = ax_nr

    print "Time axis =", time_ax_nr
    print "Freq axis =", freq_ax_nr
    timeresolution = float(metadata['CDELT' + str(time_ax_nr)])
    df = float(metadata['CDELT' + str(freq_ax_nr)])
    f0 = float(metadata['CRVAL' + str(freq_ax_nr)])
    if 'CRPIX' + str(freq_ax_nr) in metadata.keys():
        f0 -= metadata['CRPIX' + str(freq_ax_nr)] * df
    funit = metadata['CUNIT' + str(freq_ax_nr)]
    time_ax_nr = naxis - time_ax_nr
    freq_ax_nr = naxis - freq_ax_nr

    # Swap axes such that freq axis is 0th axis and timeaxis is 1st axis
    dynspec = np.swapaxes(dynspec, 0, freq_ax_nr)
    if time_ax_nr != 0:
        dynspec = np.swapaxes(dynspec, 1, time_ax_nr)

    # Integrate spectrum
    if integrationtime > 1:
        dynspec = dynspec[:, 0:int(dynspec.shape[1] / integrationtime) * integrationtime]
        dynspec = dynspec.reshape(dynspec.shape[0], dynspec.shape[1] / integrationtime, integrationtime, dynspec.shape[2], dynspec.shape[3])
        dynspec = np.sum(dynspec, axis=2)

    shape = dynspec.shape
    nrfreq = shape[0]
    nrtime = shape[1]

    timeresolution *= integrationtime
# Construct frequency axis
    frequencies = np.arange(f0, f0 + nrfreq * df, df)
    if funit == 'MHz':
        frequencies *= 1e6
    if funit == 'GHz':
        frequencies *= 1e9
    if funit == 'kHz':
        frequencies *= 1e3

    dynspec = dynspec[fstart:fend]
    frequencies = frequencies[fstart:fend]
    nrfreq = frequencies.shape[0]

    dedispshape = list(shape)
    dedispshape[0] = subdivisions
    dedisp = np.zeros(dedispshape, dtype=float)
    for div in range(subdivisions):
        dedisp[div] = dedisperse_array(dynspec[int(div * nrfreq * 1.0 / subdivisions):int((div + 1) * nrfreq * 1.0 / subdivisions), :],
        DM, frequencies[int(div * nrfreq * 1.0 / subdivisions):int((div + 1) * nrfreq * 1.0 / subdivisions)], timeresolution)
        # dedisp[div,0:len(dedisp2)]=dedisp2

    dedisp = np.swapaxes(dedisp, 0, freq_ax_nr)
    if time_ax_nr != 0:
        dedisp = np.swapaxes(dedisp, 1, time_ax_nr)

# Make new image
# Update frequency resolution
    metadata['CDELT' + str(naxis - freq_ax_nr)] = df * int(div * nrfreq * 1.0 / subdivisions)
    metadata['CDELT' + str(naxis - time_ax_nr)] = timeresolution

    hdu = pyfits.PrimaryHDU(dedisp)
    new_image_hdr = hdu.header

    for key in metadata.keys():
        new_image_hdr.update(key, metadata[key])

    hdu.writeto(outfilename)

    return True

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__ == '__main__':
    import doctest
    doctest.testmod()
