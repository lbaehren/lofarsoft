.. _quality_check:

Quality check of time series data
---------------------------------

For an automatic pipeline it is essential to check whether the data is
of good quality, or whether one needs to flag particular
antennas. Here we demonstrate a simple, but effective way to do this.

The basic parameters to look at are the mean value of the time series
(indicating potential DC offsets), the root-mean-square (RMS)
deviation (related to the power received), and the number of peaks in
the data (indicating potential RFI problems).

For cosmic ray data, we expect spikes and peaks to be in the middle of
a trace, so we will just look at the first or/and last quarter of a
data set and set the block size accordingly::

    >>> blocksize = min(filesize/4, maxblocksize)

We will then read this block of data into an appropriately sized data array::

    >>> datafile["BLOCKSIZE"] = blocksize)
    >>> datafile["BLOCK"] = 3
    >>> dataarray = datafile["TIMESERIES_DATA"]

The array now contains all the measured voltages of the selected
antennas in the file.

First we calculate the mean over all samples for each antennas (and
make use of the looping through the Ellipsis (...) object)::

    >>> datamean = dataarray[...].mean()
    Vector(float, 96, fill=[-2.49914,0.448457,-2.29027,2.78281,-6.2273,-4.90307,1.2423,-4.99803,-12.3286,-0.447695,-1.35516,1.34645,0.644004,-7.3673,-1.26443,-2.51115,-2.02316,3.394,-3.67658,-8.27854,-2.36221,-8.10352,2.20496,-6.07238,-7.75541,6.13283,3.26021,9.88207,-6.10551,6.92123,-5.86564,7.61791,-3.11711,3.61326,-7.13393,-3.8909,-0.603437,-2.58229,3.79641,2.33381,-0.937578,-3.37383,-8.68578,4.83615,-4.20088,6.58414,2.78584,-0.407637,-10.504,4.17754,-5.28557,-7.06748,3.18658,-0.72459,7.24666,-0.95668,-1.09168,10.4589,-0.963457,5.30045,-13.8324,-2.36326,6.56246,-4.32684,-0.918027,-1.10451,6.25357,-7.70752,-0.946719,-0.562695,-4.74014,4.96701,2.06908,-2.58238,4.02195,4.71707,-1.25971,4.35111,-4.63531,-0.72209,-7.8359,0.818789,-0.518262,-3.82338,6.25988,-8.67096,-4.47324,2.87482,-2.28172,-2.18484,-4.78846,-1.0026,-6.63945,-6.22959,-4.30672,-0.160312])

Similarly we get the RMS (where we spare the algorithm from recalculating the
mean, by providing it as input - actually a list of means)::

    >>> dataRMS = dataarray[...].stddev(datamean)
    Vector(float, 96, fill=[5.04449,5.01723,5.26096,4.98572,5.37563,5.07432,4.91542,4.9402,5.2736,4.98952,4.96921,5.14133,4.91443,5.16093,5.01382,5.14176,4.94637,5.04762,4.39342,4.45745,4.906,5.15883,4.98609,5.03352,5.00145,5.11712,4.97606,4.90079,4.87231,4.44023,5.1047,5.02705,5.02176,4.95146,4.83415,5.10108,4.96166,5.21537,5.01323,5.29725,4.82649,5.27223,4.71546,5.33592,4.59843,4.97653,4.5693,5.3501,4.64532,5.42013,4.64602,5.10377,2.66954,2.68755,4.89145,4.85627,4.82727,5.06666,4.52601,4.99731,4.38781,4.81783,4.94913,4.99755,5.3306,5.17204,5.00776,5.21997,4.90073,5.63104,4.67176,5.40568,4.77315,5.3259,4.67169,5.29384,4.64128,5.22491,4.52084,5.50576,4.72219,5.31989,4.80866,5.31713,4.50236,5.08738,5.01341,5.08758,5.04554,5.04889,5.06667,5.19572,4.91733,5.46251,4.56464,5.2483])

and finally we get the total number of peaks 5 sigma above the noise::

    >>> Npeaks_detected = dataarray[...].countgreaterthanabs(dataRMS*5)
    Vector(int, 96, fill=[0,0,0,0,4,1,0,1,197,0,0,0,0,6,0,0,0,0,2,34,1,10,0,2,6,1,1,78,4,5,5,8,1,0,7,0,2,0,0,0,0,0,47,0,0,4,0,0,127,0,2,7,4,0,11,0,0,60,0,5,2156,0,10,1,0,0,7,4,0,0,0,1,0,0,0,0,0,0,3,0,19,0,0,0,9,23,0,0,0,0,2,0,5,2,0,0])

To see whether we have more peaks than expected, we first calculate
the expected number of peaks for a Gaussian distribution and our
``BLOCKSIZE``, as well as the error on that number::

    >>> Npeaks_expected = funcGaussian(5, 1,0)*blocksize
    >>> Npeaks_error = sqrt(Npeaks_expected)

So, that we can get a normalized quantity::

    >>> G = (Npeaks_detected - Npeaks_expected)/Npeaks_error

which should be of order unity if we have roughly a Gaussian
distribution. If it is much larger or less than unity we have more or
less peaks than expected and the data is clearly not Gaussian noise.

We do the calculation of G using our STL vectors (even though speed is not
of the essence here)::

    >>> dataNonGaussianity = Vector(float, nAntennas)
    >>> dataNonGaussianity.sub(Npeaks_detected, Npeaks_expected)
    >>> dataNonGaussianity /= Npeaks_error

The next stept is to make a nice table of the results and check
whether these parameters are within the limits we have imposed (based
on empirical studies of the data).

To ease the operation we combine all the data into one python array
(using the zip function - zip, as in zipper)::

    >>> dataproperties = zip(selectedAntennas,datamean,dataRMS,Npeaks_detected,dataNonGaussianity)

which is a rather nasty collection of numbers. So, we print a nice
table (restricting it to the first 5 antennas)::

    >>> for prop in dataproperties[0:5]: print "Antenna {0:3s}: mean={1: 6.2f}, rms={2:6.1f}, npeaks={3:5d}, spikyness={4: 7.2f}".format(*prop)
    Antenna 003000000: mean= -2.50, rms=   5.0, npeaks=    0, spikyness=  -0.28
    Antenna 003000001: mean=  0.45, rms=   5.0, npeaks=    0, spikyness=  -0.28
    Antenna 003000002: mean= -2.29, rms=   5.3, npeaks=    0, spikyness=  -0.28
    Antenna 003000003: mean=  2.78, rms=   5.0, npeaks=    0, spikyness=  -0.28
    Antenna 003000004: mean= -6.23, rms=   5.4, npeaks=    4, spikyness=  14.22

To check automatically whether all parameters are in the allowed
range, we can use a little python helper function, using a python
"dict" as database for allowed parameters::

    >>> qualitycriteria = {"mean":(-15,15),"rms":(5,15),"spikyness":(-3,3)}
    >>> CheckParameterConformance(dataproperties[0], {"mean":1,"rms":2,"spikyness":4}, qualitycriteria)

The first parameter is just the list of numbers of the mean, RMS,
etc. of one antenna we created above. The second parameter is a dict,
describing which parameter to find at which position in the input
list, and the third parameter is yet another dict specifying for each
parameter the range of allowed upper and lower values. The result is a
list of parameter names, where the antennas failed the test. The list
is empty if the antenna passed it.

Finally, we do not want to do this manually all the time. So, a little
python function is available, that does the quality checking for you
and returns a list with failed antennas and their properties::

    >>> badantennalist = CRQualityCheck(qualitycriteria, datafile=datafile, dataarray=dataarray, blocksize=blocksize, verbose=False)
    Block=     0, Antenna   3: mean=  2.76, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna   6: mean=  1.24, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna   7: mean= -4.98, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna   9: mean= -0.48, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  10: mean= -1.34, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  12: mean=  0.68, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  16: mean= -2.02, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  18: mean= -3.66, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  19: mean= -8.29, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  20: mean= -2.40, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  22: mean=  2.18, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  24: mean= -7.77, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  27: mean=  9.85, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  28: mean= -6.12, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  29: mean=  6.91, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  33: mean=  3.62, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  34: mean= -7.17, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  36: mean= -0.62, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  40: mean= -0.95, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  42: mean= -8.66, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  44: mean= -4.25, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  45: mean=  6.57, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  46: mean=  2.78, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  48: mean=-10.48, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  50: mean= -5.30, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  52: mean=  3.14, rms=   2.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  53: mean= -0.71, rms=   2.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  54: mean=  7.21, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  55: mean= -0.95, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  56: mean= -1.08, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  58: mean= -0.98, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  60: mean=-13.82, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  61: mean= -2.36, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  62: mean=  6.54, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  68: mean= -0.93, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  70: mean= -4.72, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  72: mean=  2.08, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  74: mean=  4.01, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  76: mean= -1.24, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  78: mean= -4.60, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  80: mean= -7.82, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  82: mean= -0.50, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  84: mean=  6.25, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  92: mean= -6.62, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     0, Antenna  94: mean= -4.27, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna   3: mean=  2.78, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna   6: mean=  1.24, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna   7: mean= -5.00, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna   9: mean= -0.45, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  10: mean= -1.36, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  12: mean=  0.64, rms=   4.9, npeaks=    1, spikyness=   3.35   ['rms', 'spikyness']
    Block=     3, Antenna  16: mean= -2.02, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  18: mean= -3.68, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  19: mean= -8.28, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  20: mean= -2.36, rms=   4.9, npeaks=    1, spikyness=   3.35   ['rms', 'spikyness']
    Block=     3, Antenna  22: mean=  2.20, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  26: mean=  3.26, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  27: mean=  9.88, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  28: mean= -6.11, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  29: mean=  6.92, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  33: mean=  3.61, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  34: mean= -7.13, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  36: mean= -0.60, rms=   5.0, npeaks=    1, spikyness=   3.35   ['rms', 'spikyness']
    Block=     3, Antenna  40: mean= -0.94, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  42: mean= -8.69, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  44: mean= -4.20, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  45: mean=  6.58, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  46: mean=  2.79, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  48: mean=-10.50, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  50: mean= -5.29, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  52: mean=  3.19, rms=   2.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  53: mean= -0.72, rms=   2.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  54: mean=  7.25, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  55: mean= -0.96, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  56: mean= -1.09, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  58: mean= -0.96, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  59: mean=  5.30, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  60: mean=-13.83, rms=   4.4, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  61: mean= -2.36, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  62: mean=  6.56, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  63: mean= -4.33, rms=   5.0, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  68: mean= -0.95, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  70: mean= -4.74, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  72: mean=  2.07, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  74: mean=  4.02, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  76: mean= -1.26, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  78: mean= -4.64, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  80: mean= -7.84, rms=   4.7, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  82: mean= -0.52, rms=   4.8, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  84: mean=  6.26, rms=   4.5, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  92: mean= -6.64, rms=   4.9, npeaks=    0, spikyness=  -0.28   ['rms']
    Block=     3, Antenna  94: mean= -4.31, rms=   4.6, npeaks=    0, spikyness=  -0.28   ['rms']

(first the block , then the antenna number, then a list with the
*mean*, *rms*, *npeaks*, and *spikyness*, and finally the failed
fields)

Note, that this function can be called with ``file=None``. In this
case the data provided in the datararray will be used.

