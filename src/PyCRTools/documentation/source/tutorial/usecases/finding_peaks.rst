.. _finding_peaks:


Finding peaks in a vector
=========================

.. [in preparation]

In the following example we try to find peaks in some artificially
generated data.

First we make a test time series data set for 4 antennas::

    >>> data = hArray(float, [4,512], name='Random series with peaks')

and fill it with random data that have arbitrary offsets::

    >>> data.random(-1024,1024)
    >>> data[...] += Vector([-128.,256., 385.,-50.])

Then we put some peaks at location 2-3, 32, and 64-67 in each of the 4
data sets::

    >>> for i in range(4):
    ...     data[i,[2,3,32,64,65,67],...] = Vector([4096.,5097,-4096,4096,5099,3096])

Now, we reverse-engineer and try finding all 5 sigma peaks::

    >>> nsigma = 5

First make a scratch array that will contain the locations of the
peaks.  A location is actually a 'slice' in the array, i.e. given by
its beginning and ending position (plus one). The length of the return
array must be pre-allocated and should be long enough to contain all
peaks (at maximum as long as the input array)::

    >>> datapeaks = hArray(int, [4,5,2], name="Location of peaks")

Now, retrieve the mean and RMS of the array to set the thresholds
above and below which one considers a peak to be significant::

    >>> datamean = data[...].mean()
    >>> datathreshold2 = data[...].stddev(datamean)
    >>> datathreshold2 *= nsigma
    >>> datathreshold1 = datathreshold2*(-1)
    >>> datathreshold1 += datamean
    >>> datathreshold2 += datamean

Finally, we determine the input parameters for the search algorithm::

    >>> maxgap = Vector(int, len(datamean), fill=10)

The gap vector tells the algorithm how many samples can be between two
values that are above threshold, so that the two peaks are considered
as one::

    >>> minlength = Vector(int, len(datamean), fill=1)

A minimum length can be specified to exclude peaks that consists of
only a single or a few values (no relevant here, so set to 1, i.e. all
peaks are relevant). Then call :func:`hFindSequenceOutside` (or
:func:`hFindSequenceBetween`, :func:`hFindSequenceGreatererThan`,
:func:`hFindSequenceLessEqual` ...)::

    >>> npeaks = datapeaks[...].findsequenceoutside(data[...], datathreshold1, datathreshold2, maxgap, minlength)

The return value is the number of peaks found (in each row of the data set)::

    >>> npeaks
    Vector(int, 4, fill=[3,3,3,3])

And the slices are actually contained in the return vector for each antenna::

    >>> datapeaks.mprint()
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]


