"""Find a source on the sky based on arrival times and/or phase delays in multiple antennas

.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>
"""

# import pycrtools
import numpy as np
from numpy import sin, cos, tan, arctan2, sqrt, pi
import matplotlib.pyplot as plt  # want to have plotting in here?
from scipy.optimize import brute, fmin
import sys

c = 299792458.0  # speed of light in m/s - air has n ~ 1.0003; and cos(2 degrees) ~ 1 / 1.0003... needed?
twopi = 2 * pi
halfpi = 0.5 * pi
rad2deg = 360.0 / twopi
deg2rad = twopi / 360.0
nan = float('nan')


def GPSplusOffset(startingGPS, offsetXYZ):
    """
    Convert GPS position + xyz offset -> GPS position
    Where (x, y, z) points (east, north, up)

    Linearized approximation; accurate to ~ 1 mm for x, y up to 100 m

    Warning: approximating the Earth as a sphere, error up to 0.3 % is possible!
    However, xyz distances are consistent with greatCircleDistance when using the same Earth radius in both functions.

    Required arguments:

    ============= =================================================
    Parameter     Description
    ============= =================================================
    *startingGPS* tuple/array/list ``(long, lat)`` in degrees;
    *offsetXYZ*   array/list ``[x, y, z]``.
    ============= =================================================
    """
    R_earth = 6367449 * 1.000  # meters; see Wikipedia on Geographic Coordinate System
    metersPerDegree = 111132.954 * 1.000  # in latitude; ignoring nonspherical components

    long = startingGPS[0]
    lat = startingGPS[1]

    x = offsetXYZ[0]
    y = offsetXYZ[1]

    outLat = lat + y / metersPerDegree
    outLong = long + x / (metersPerDegree * cos(lat * deg2rad))

    return (outLong, outLat)


def greatCircleDistance(gps1, gps2):
    """
    Calculate the distance along a great-circle, between 2 GPS points (long, lat) in degrees N, E
    Distance in meters.

    Taken from: http://www.movable-type.co.uk/scripts/latlong.html
    """

    lat1 = gps1[1] * deg2rad
    lat2 = gps2[1] * deg2rad
    long1 = gps1[0] * deg2rad
    long2 = gps2[0] * deg2rad

    delta_lat = lat2 - lat1
    delta_long = long2 - long1

    a = sin(delta_lat / 2) ** 2 + cos(lat1) * cos(lat2) * sin(delta_long / 2) ** 2
    b = 2 * arctan2(sqrt(a), sqrt(1 - a))
    R_earth = 6367449 * 1.000
    distance = R_earth * b

    return distance


def directionFromThreeAntennas(positions, times):
    """
    Given three antenna positions, and (pulse) arrival times for each antenna,
    get a direction of arrival (az, el) assuming a source at infinity (plane wave).
    From three antennas we get in general two solutions for the direction,
    unless the arrival times are out of bounds, i.e. larger than the light-time between two antennas.
    Usually, when the 3 antennas are in a more or less horizontal plane, one of the solutions will appear to come
    from below the horizon (el < 0) and can be discarded.

    Required arguments:

    =========== =================================================
    Parameter   Description
    =========== =================================================
    *positions* array ``(x1, y1, z1, x2, y2, z2, x3, y3, z3)``;
    *times*     array ``(t1, t2, t3)``.
    =========== =================================================

    Input is assumed to be Numpy arrays.

    Output: ``(az1, el1, az2, el2)``, in radians, containing the two solutions.

    """

    p1 = np.array(positions[0:3])

    p2 = np.array(positions[3:6])
    p3 = np.array(positions[6:9])  # yes, for 10 antennas we'd write it differently...
    t1 = times[0]
    t2 = times[1]
    t3 = times[2]

    # First take by definition p1 = 0, t1 = 0, use translation invariance of the result
    t2 -= t1
    t3 -= t1
    p2 -= p1
    p3 -= p1
    t1 = 0
    p1 = np.array(np.zeros(3))
    t2 *= c
    t3 *= c
    # t -> ct

    p2 = p2.astype(float)
    p3 = p3.astype(float)  # !!! There's nothing that stops you from throwing ints at it, which destroys e.g. the inner product ratio below!

    # Now do a coordinate rotation such that z-coordinates z2, z3 are 0, and p2 is on the x-axis (i.e. y2 = 0).
    xx = p2 / np.linalg.norm(p2)  # make x-axis by normalizing p2
    yy = p3 - p2 * (np.dot(p2, p3) / np.dot(p2, p2))  # make y-axis by taking the part of p3 that is perpendicular to p2 (cf. Gram-Schmidt)
    yy = yy / np.linalg.norm(yy)
    zz = np.cross(xx, yy)
    zz = zz / np.linalg.norm(zz)   # and make z-axis by the vector perpendicular to both p2 and p3.
    # We use the fact that we preserve lengths and angles in the new coordinate system.

    # We now have to solve for A and B in:
    # t2 = A x2
    # t3 = A x3 + B y3
    # from the general t_i = A x_i + B y_i + C z_i and using our rotated coordinates.

    a = np.dot(p3, xx)
#    b = np.sqrt(np.dot(p3, p3) - a*a)      # a and b are x3 and y3 resp. in rotated coordinates.
#    NB ! This square root is wrong as it should be +/- sqrt(...) and you don't know the sign. Rewrite and make it easier:
    b = np.dot(p3, yy)

    A = - t2 / np.linalg.norm(p2)
    B = (1 / b) * (-t3 - A * a)

    # Which gives the incoming-signal vector in the rotated coord. frame as (A, B, C).
    # C is free in this coord. system, but follows from imposing length 1 on the signal vector,
    # consistent with (ct)^2 = x^2 + y^2 + z^2.
    square = A * A + B * B
    if square < 1:
        C = np.sqrt(1 - square)  # Note! TWO solutions, +/- sqrt(...). No constraints apart from elevation > 0 in the end result - otherwise both can apply.
        error = 0.0
    #    else (square - 1) < 1e-15: # this does happen because of floating point errors, when time delays 'exactly' match elevation = 0
    else:
        C = 0
        error = np.sqrt(square - 1)  # store the complex component as 'closure error'
        # so this correction is needed to ensure this function correctly inverts timeDelaysFromDirection(...)
#    else:
#        return (nan, nan, nan, nan) # calculation fails, arrival times out of bounds!

    # Now we have to transform this vector back to normal x-y-z coordinates, and then to (az, el) to get our direction.
    # This is where the above xx, yy, zz vectors come in (normal notation: x', y', z')

    signal = A * xx + B * yy + C * zz
    signal2 = A * xx + B * yy - C * zz

    r = np.sqrt(signal[0] ** 2 + signal[1] ** 2 + signal[2] ** 2)

    error += abs(r - 1.0)  # Make sure the normalized vector is really normalized, if not take it as error
    signal /= r
    signal2 /= r

    # Now get az, el from x, y, z...
    x = signal[0]
    y = signal[1]
    z = signal[2]

    theta = np.arccos(z)  # in fact, z/R with R = 1
    phi = np.arctan2(y, x)  # gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if phi < 0:
        phi += twopi

    az1 = halfpi - phi  # phi is w.r.t. x pointing east and y pointing north. So, reverse angle and add 90 degrees...
    el1 = halfpi - theta
    x = signal2[0]
    y = signal2[1]
    z = signal2[2]

    theta = np.arccos(z)  # in fact, z/R with R = 1
    phi = np.arctan2(y, x)  # gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if az1 < 0:
        az1 += twopi

    az2 = halfpi - phi
    el2 = halfpi - theta
    if az2 < 0:
        az2 += twopi

    return (az1, el1, az2, el2, error)


def timeDelaysFromDirection(positions, direction):
    """
    Get time delays for antennas at given position for a given direction.
    Time delays come out as an np-array.

    Required arguments:

    =========== =================================================
    Parameter   Description
    =========== =================================================
    *positions* ``(np-array x1, y1, z1, x2, y2, z2, ...)``
    *direction* (az, el) in radians.
    =========== =================================================

    Example:

    .. doctest::

        >>> pos = np.array([0, 0, 0, 23, -21, 54, 11, 21, 33])
        >>> dir = (1.2, 0.7)
        >>> timeDelaysFromDirection(pos, dir)
        array([ -0.00000000e+00,  -8.73671261e-08,  -1.31017019e-07])

    """

    n = len(positions) / 3
    phi = halfpi - direction[0]  # warning, 90 degree? -- Changed to az = 90_deg - phi
    theta = halfpi - direction[1]  # theta as in standard spherical coords, while el=90 means zenith...

    cartesianDirection = np.array([sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)])
    timeDelays = np.zeros(n)
    for i in range(n):
        thisPosition = np.array(positions[3 * i:3 * (i + 1)])
        timeDelays[i] = - (1 / c) * np.dot(cartesianDirection, thisPosition)  # note the minus sign! Signal vector points down from the sky.

    return timeDelays


def phaseWrap(phases):
    wrapped = phases - twopi * (phases / twopi).round()
#    wrapped = np.zeros(len(phases))
#    for i in range(len(phases)): # improve!
#        wrapped[i] = phases[i] - twopi * round(phases[i] / twopi)
    return wrapped


def phasesFromDirection(positions, direction, freq, nowrap=False):
    # return phases instead of time differences
    phases = (twopi * freq) * timeDelaysFromDirection(positions, direction)
    if nowrap:
        return phases
    else:
        return phaseWrap(phases)


def timeDelaysFromGPSPoint(gps, positions, antids, antennaset):
    """
    Get time delays for a radio signal travelling from GPS point ``gps`` to antennas at ``positions``.
    The antenna ids are needed for identifying the station it belongs to.

    Required arguments:

    ============ =================================================
    Parameter    Description
    ============ =================================================
    *gps*        array / list / tuple ``(long, lat)``
    *positions*  ``(np-array x1, y1, z1, x2, y2, z2, ...)``
    *antids*     Array or list of antenna ids
    *antennaset* e.g. 'LBA_OUTER'
    ============ =================================================
    """

    from pycrtools import metadata as md

    if isinstance(antids[0], basestring):
        stationIDs = np.array([int(x) for x in antids]) / 1000000
    else:
        stationIDs = np.array(list(antids)) / 1000000

    # stationGPS = np.array([md.getStationPositions(stationID, antennaset) for stationID in stationIDs])
    stationGPS = md.getStationPositions('CS002', antennaset)
#    gpspositions = np.zeros(len(antids))
    timedelays = np.zeros(len(antids))
#    import pdb; pdb.set_trace()
    for i in range(len(antids)):
#        import pdb; pdb.set_trace()
        thisposition = positions[3 * i:3 * (i + 1)]
        thisGPS = GPSplusOffset(stationGPS, thisposition)  # stationGPS[i] if variable...
#        gpspositions[i] = thisGPS
        timedelays[i] = greatCircleDistance(gps, thisGPS)

    timedelays -= timedelays[0]
    timedelays /= c

    return timedelays


def timeDelaysFromDirectionAndDistance(positions, direction):
    """
    Get time delays for antennas at given position for a given direction and distance.
    Time delays come out as an np-array.

    Required arguments:

    =========== =================================================
    Parameter   Description
    =========== =================================================
    *positions* ``(np-array x1, y1, z1, x2, y2, z2, ...)``
    *direction* (az, el, R) in radians, meters.
    =========== =================================================

    Example:

    .. doctest::

        >>> pos = np.array([0, 0, 0, 23, -21, 54, 11, 21, 33])
        >>> dir = (1.2, 0.7)
        >>> timeDelaysFromDirection(pos, dir)
        array([ -0.00000000e+00,  -8.73671261e-08,  -1.31017019e-07])

    """

    n = len(positions) / 3
    phi = halfpi - direction[0]  # warning, 90 degree? -- Changed to az = 90_deg - phi
    theta = halfpi - direction[1]  # theta as in standard spherical coords, while el=90 means zenith...
    R = direction[2]

    cartesianDirection = np.array([sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)])
    cartesianSourcePoint = R * cartesianDirection
    referenceTime = (1 / c) * np.linalg.norm(cartesianSourcePoint)  # the time delay at position (0, 0, 0)

    timeDelays = np.zeros(n)
    for i in range(n):
        thisPosition = np.array(positions[3 * i:3 * (i + 1)])
        distanceVector = cartesianSourcePoint - thisPosition

        timeDelays[i] = (1 / c) * np.linalg.norm(distanceVector) - referenceTime  # check accuracy!

    return timeDelays


def testDirectionCalculationForThreeAntennas(positions):
    """
    Given antenna positions, calculate time delays for 'every' signal vector on the sky.
    Then, invert and calculate the signal vector from the given time delays. One out of the 2 possible solutions has to match
    within some floating point roundoff interval. A two-pi error may occur (and gets written out), but that's no problem.

    Example:

        >>> testDirectionCalculationForThreeAntennas(np.array([0, 0, 0, 23, -21, 54, 11, 21, 33]))
        Wrong value for az: 0.0 calculated: 195.081863616 or: 360.0
        el: 18.0 calculated: 60.2318806131 or: 18.0
        Wrong value for az: 0.0 calculated: 196.898077333 or: 360.0
        el: 24.0 calculated: 66.1764021336 or: 24.0
        Wrong value for az: 0.0 calculated: 285.811432122 or: 360.0
        el: 48.0 calculated: 86.4889718066 or: 48.0
        Wrong value for az: 0.0 calculated: 358.619827389 or: 360.0
        el: 66.0 calculated: 71.2661264901 or: 66.0
        Wrong value for az: 0.0 calculated: 360.0 or: 1.33942285143
        el: 72.0 calculated: 72.0 or: 65.3494016156
        Wrong value for az: 0.0 calculated: 360.0 or: 3.05501838316
        el: 78.0 calculated: 78.0 or: 59.4017926229

    """
    azSteps = 60
    elSteps = 15
    tolerance = 1e-7
    s = ''
    for i in range(azSteps):
        for j in range(elSteps):
            az = twopi * float(i) / azSteps
            el = halfpi * float(j) / elSteps
            tdelays = timeDelaysFromDirection(positions, [az, el])
            result = (calc_az, calc_el, calc_az2, calc_el2) = directionFromThreeAntennas(positions, tdelays)

            if (abs(calc_az - az) > tolerance or abs(calc_el - el) > tolerance) and (abs(calc_az2 - az) > tolerance or abs(calc_el2 - el) > tolerance):
                print 'Wrong value for az: ' + str(az * rad2deg) + ' calculated: ' + str(calc_az * rad2deg) + ' or: ' + str(calc_az2 * rad2deg)
                print 'el: ' + str(el * rad2deg) + ' calculated: ' + str(calc_el * rad2deg) + ' or: ' + str(calc_el2 * rad2deg)


def mse(az, el, pos, times):
    """
    Mean-squared error in the times for a given direction on the sky, as used in the brute force search

    Required arguments:

    =========== =================================================
    Parameter   Description
    =========== =================================================
    *az*        Azimuth in radians
    *el*        Elevation in radians
    *pos*       Positions as in the other functions
    *times*     Times as in the other functions
    =========== =================================================

    """

    N = len(times)
    calcTimes = timeDelaysFromDirection(pos, (az, el))
    timeOffsets = calcTimes - times
    mu = (1.0 / N) * np.sum(timeOffsets)  # overall time offset to be subtracted in MSE
    mse = (1.0 / N) * np.dot(timeOffsets, timeOffsets) - mu * mu

    return mse * 1.0e18  # output in nanoseconds-squared


def phaseAverage(phases):
    """ Calculates the average of an array of phases
    It uses vector addition, i.e. converting to exp(i phi), and summing all the complex exponentials.
    This corresponds to concatenating vectors in a polar plot.
    The angle (argument) of the sum vector is the average phase.
    The length of the sum vector can be used as a measure of the variance in the phases.
    Here we just output the average.
    """

    phaseAvg = np.angle(np.sum(np.exp(1j * phases)))

    return phaseAvg


def phaseErrorFromDifference(deltaPhi, freq, allowOutlierCount=0):
    # Calculate phase error given two arrays of phases
    # mean phase difference is subtracted
    mu = phaseAverage(deltaPhi)

    msePerPhase = (2.0 / (freq * twopi) * sin((deltaPhi - mu) / 2)) ** 2

    if allowOutlierCount != 0:
#        import pdb; pdb.set_trace()
        sortedAntennas = msePerPhase.argsort()  # lists antennas
        msePerPhase = msePerPhase[sortedAntennas]
        # print 'Worst antennas: '
        # print sortedAntennas[len(sortedAntennas) - allowOutlierCount:]
        msePerPhase[len(msePerPhase) - allowOutlierCount:] = 0.0  # zero out the worst ones

    mse = np.average(msePerPhase)

    return mse * 1.0e18  # unit ns^2


def phaseError(az, el, pos, phases, freq, allowOutlierCount=0):

    N = len(phases)  # phases assumed 0 for phases[0]?
    calcTimes = timeDelaysFromDirection(pos, (az, el))
    calcTimes -= calcTimes[0]  # same phase ref imposed
    calcPhases = (twopi * freq) * calcTimes

    # wrap around into [-pi, pi]? Not strictly needed as the sin^2 function will do that...
    deltaPhi = (phases - phases[0]) - calcPhases  # measured 'phases' and expected 'calcPhases' at the same reference phase
    # mu = (1.0/N) * np.sum( 2.0/(freq * twopi) * sin(deltaPhi/2) ) # check; uses same procedure as for 'mse' above.
    mu = phaseAverage(deltaPhi)

    msePerAntenna = (2.0 / (freq * twopi) * sin((deltaPhi - mu) / 2)) ** 2  # periodicity 2-pi in deltaPhi needed!

    if allowOutlierCount != 0:
#        import pdb; pdb.set_trace()
        sortedAntennas = msePerAntenna.argsort()  # lists antennas
        msePerAntenna = msePerAntenna[sortedAntennas]
        # print 'Worst antennas: '
        # print sortedAntennas[len(sortedAntennas) - allowOutlierCount:]
        msePerAntenna[len(msePerAntenna) - allowOutlierCount:] = 0.0  # zero out the worst ones

    mse = np.average(msePerAntenna)

    return mse * 1.0e18  # output in nanoseconds-squared; may want to switch to radians-squared?


def mseWithDistance(az, el, R, pos, times, outlierThreshold=0, allowOutlierCount=0):
    """
    Mean-squared error in the times for a given direction and distance, as used in brute force or similar searches.

    Required arguments:

    =========== =================================================
    Parameter   Description
    =========== =================================================
    *az*        Azimuth in radians
    *el*        Elevation in radians
    *R*         Distance in m; reference is the 0-th element of 'pos' and 'times'.
    *pos*       Positions as in the other functions
    *times*     Times as in the other functions
    =========== =================================================
    """

    if allowOutlierCount % 2 == 1:
        print 'Warning: allowOutlierCount should be even! Discarding k/2 lowest and k/2 highest data points'
    N = len(times)
    calcTimes = timeDelaysFromDirectionAndDistance(pos, (az, el, R))
    timeOffsets = times - calcTimes
    mu = np.mean(timeOffsets)  # overall time offset to be subtracted in MSE
    if allowOutlierCount == 0:
        mse = (1.0 / N) * np.dot(timeOffsets, timeOffsets) - mu * mu
    else:
        timeOffsets.sort()  # can be done ascending only; we'll throw out the last ones if needed
        # NB. To be implemented: negative offsets at the beginning are not taken care of...
        removeCount = min(len(np.where(timeOffsets - mu > outlierThreshold)[0]), allowOutlierCount)
        # remove at most 'allowOutlierCount', but only those which are larger than OutlierThreshold
        # as we operate on a sorted list, we only need to discard the last 'removeCount' entries...
        mu = np.mean(timeOffsets[removeCount/2:-removeCount/2]) # average only over used entries
        timeOffsetsSqr = timeOffsets * timeOffsets
        mse = 1.0 / (N - removeCount) * np.sum(timeOffsetsSqr[removeCount/2:-removeCount/2]) - mu*mu

    return mse * c * c


def directionBruteForceSearch(positions, times, azSteps=120, elSteps=30):
    """
    Given N antenna positions, and (pulse) arrival times for each antenna,
    get a direction of arrival (az, el) assuming a source at infinity (plane wave).

    The direction is found using a 'brute-force' search over a grid on the sky.
    For each direction, the corresponding time delays are calculated using timeDelaysFromDirection(positions, direction).
    These are compared to the actual (measured) time delays in 'times'. The mean-squared error (MSE)
    serves as a fit criterion to be minimized. The global minimum and its MSE are returned.
    Note that the square-root of the MSE value is useful to determine the average 'timing noise',
    i.e. the timing deviation from the plane-wave solution.

    Required arguments:

    =========== =========================================================
    Parameter   Description
    =========== =========================================================
    *positions* array ``(x1, y1, z1, x2, y2, z2, x3, y3, z3)``, in meters
    *times*     array ``(t1, t2, t3)``, in seconds.
    =========== =========================================================

    Input is assumed to be Numpy arrays.

    Output: ``(az, el, mse)``, in radians, and seconds-squared.

    """

    # this can probably be done better by Scipy. But for now this is easy and it works...

    N = len(times)

    bestFit = (-1, -1)
    minMSE = 1.0e9

    for i in range(elSteps + 1):
        for j in range(azSteps + 1):
            el = halfpi * float(i) / elSteps
            az = twopi * float(j) / azSteps

            calcTimes = timeDelaysFromDirection(positions, (az, el))
            timeOffsets = calcTimes - times
            mu = (1.0 / N) * np.sum(timeOffsets)  # overall time offset to be subtracted in MSE
            mse = (1.0 / N) * np.dot(timeOffsets, timeOffsets) - mu * mu

            if mse < minMSE:
                minMSE = mse
#                print 'New MinMSE %e' % minMSE
                bestFit = (az, el)

    return bestFit + (minMSE,)


def directionBruteForcePhases(positions, phases, freq, azSteps=360, elSteps=90, allowOutlierCount=0, showImage=False, verbose=False):
    # this can probably be done better by Scipy (brute). But for now this is easy and it works...
    elevations = np.arange(elSteps) * (90.0 / elSteps)
    azimuths = np.arange(azSteps) * (360.0 / azSteps)

    if showImage:
        imarray = np.zeros((elSteps, azSteps))
    elstep = 0
    minPhaseError = 1.0e9
    for el in elevations:
        if verbose:
            print ' %d,' % elstep,
            sys.stdout.flush()
        azstep = 0
        for az in azimuths:
            badness = phaseError(az * deg2rad, el * deg2rad, positions, phases, freq, allowOutlierCount=allowOutlierCount)
            if badness < minPhaseError:
                minPhaseError = badness
                minpos = (az * deg2rad, el * deg2rad)
            if showImage:
                imarray[elSteps - 1 - elstep, azstep] = badness  # hack to get elevations shown bottom to top as 0.0 - 90.0
            azstep += 1
        elstep += 1

    if showImage:
        plt.imshow(imarray, cmap=plt.cm.hot_r, extent=(0, 360.0, 0.0, 90.0))
        plt.ylabel("Elevation [deg]")
        plt.xlabel("Azimuth [deg]")
        plt.title("Phase error [ns^2] as function of incoming direction")
        # show 'good' positions bright by reverse colormap hot_r
        plt.colorbar()
    if verbose:
        print ' '

    return minpos + (minPhaseError,)


def directionForHorizontalArray(positions, times):
    """
    Given N antenna positions, and (pulse) arrival times for each antenna,
    get a direction of arrival (az, el) assuming a source at infinity (plane wave).

    Here, we find the direction assuming all antennas are placed in the z=0 plane.
    If all antennas are co-planar, the best-fitting solution can be found using a 2D-linear fit.
    We find the best-fitting A and B in:

    .. math::

        t = A x + B y + C

    where t is the array of times; x and y are arrays of coordinates of the antennas.
    The C is the overall time offset in the data, that has to be subtracted out.
    The optimal value of C has to be determined in the fit process (it's not just the average time, nor the time at antenna 0).

    This is done using :mod:`numpy.linalg.lstsq`.

    The (az, el) follows from:

    .. math::

        A = \cos(\mathrm{el}) \cos(\mathrm{az})

        B = \cos(\mathrm{el}) \sin(\mathrm{az})

    Required arguments:

    =========== ==========================================================
    Parameter   Description
    =========== ==========================================================
    *positions* array ``(x1, y1, z1, x2, y2, z2, x3, y3, z3)``, in meters.
                NB: :math:`z_i` is simply ignored but still assumed to be
                there!
    *times*     array ``(t1, t2, t3)``, in seconds.
    =========== ==========================================================

    Input is assumed to be Numpy arrays.

    Output: ``(az, el)``, in radians, and seconds-squared.

    """

    # make x, y arrays out of the input position array
#    N = len(positions)
    x = positions[0:-1:3]
    y = positions[1:-1:3]

    # now a crude test for nonzero z-input, |z| > 0.5
    z = positions[2:-1:3]
    if max(abs(z)) > 0.5:
        raise ValueError("Input values of z are nonzero ( > 0.5) !")
        return (-1, -1)

    M = np.vstack([x, y, np.ones(len(x))]).T  # says the linalg.lstsq doc

    (A, B, C) = np.linalg.lstsq(M, c * times)[0]

    el = np.arccos(np.sqrt(A * A + B * B))
    az = halfpi - np.arctan2(-B, -A)  # note minus sign as we want the direction of the _incoming_ vector (from the sky, not towards it)
    # note: Changed to az = 90_deg - phi
    return (az, el)


def testFitMethodsWithTimingNoise(az, el, N_ant, noiselevel):
    """
    A random set of N_ant antennas is generated, and time differences are calculated for given (az, el) direction.
    Then, for the 3 different fit methods, the result is calculated back.
    This is done without noise (should return the input), and with noise

    Required arguments:

    ============ ===================================================
    Parameter    Description
    ============ ===================================================
    *az*         Azimuth
    *el*         Elevation
    *N_ant*      Number of antennae
    *noiselevel* as a fraction of the max time difference across the
                 antennas, e.g. 0.1
    ============ ===================================================

    """

    x = np.random.rand(N_ant) * 100 - 50  # Antenna positions in a 100x100 m field
    y = np.random.rand(N_ant) * 100 - 50
    z = np.zeros(N_ant)  # for use in the planar fit requires z = 0

    pos = np.column_stack([x, y, z]).ravel()  # make flat array alternating x,y,z. This thing can be retained as global if speed is important

    times = timeDelaysFromDirection(pos, (az, el))
    noise = (2 * np.random.rand(N_ant) - 1.0) * max(abs(times)) * noiselevel

    print 'Getting direction from linear fit method, without noise (should return the input):'
    (az, el) = directionForHorizontalArray(pos, times)
    print '(az, el) = (%f, %f)' % (az, el)
    print ' '
    print 'Getting direction from brute force search:'

    result = directionBruteForceSearch(pos, times)
    print '(az, el) = (%f, %f)' % (result[0], result[1])
    print ' '
    print 'Getting direction from triangle method: '
    result = directionFromAllTriangles(pos, times)
    print '(az, el) = (%f, %f)' % (result[0], result[1])
#    print '(u_az, u_el) = (%f, %f)' %(result[2], result[3])
#    print 'Better (az, el) = (%f, %f)' %(result[4], result[5])

    times += noise
    print '\n-----\n'
    print 'Getting direction from linear fit method, WITH noise:'
    (az, el) = directionForHorizontalArray(pos, times)
    print '(az, el) = (%f, %f)' % (az, el)
    print ' '
    print 'Getting direction from brute force search:'

    result = directionBruteForceSearch(pos, times)
    print '(az, el) = (%f, %f)' % (result[0], result[1])
    print ' '
    print 'Getting direction from triangle method: '
    result = directionFromAllTriangles(pos, times)
    print '(az, el) = (%f, %f)' % (result[0], result[1])
#    print '(u_az, u_el) = (%f, %f)' %(result[2], result[3])
#    print 'Better (az, el) = (%f, %f)' %(result[4], result[5])


def testForBiasFromNoisyData(n_trials, N_ant, az, el, noiselevel, fitType='LinearFit'):
    """
    Do a number of trials with random antenna positions and random timing noise.
    Put in the given (az, el) direction, see what comes out of the fits.
    By averaging over these trials, we can see if the fit result is biased or not.

    Required arguments:

    ============ ===================================================
    Parameter    Description
    ============ ===================================================
    *n_trials*
    *N_ant*
    *az*
    *el*
    *noiselevel* as a fraction of the max time difference across the
                 antennas, e.g. 0.1
    *fitType*    ['LinearFit' (default)|'BruteForce'|'Triangles']
    ============ ===================================================

    Output is printed.

    """

    print 'Testing for bias using fit type = %s' % fitType
    x = np.random.rand(N_ant) * 100 - 50  # Antenna positions in a 100x100 m field
    y = np.random.rand(N_ant) * 100 - 50
    z = np.zeros(N_ant)  # for use in the planar fit requires z = 0

    pos = np.column_stack([x, y, z]).ravel()  # make flat array alternating x,y,z
#    az = 1.0
#    el = 0.1
    exactTimes = timeDelaysFromDirection(pos, (az, el))

    results_az = np.zeros(n_trials)
    results_el = np.zeros(n_trials)
    for i in range(n_trials):
        noise = (2 * np.random.rand(N_ant) - 1.0) * max(abs(exactTimes)) * noiselevel
        times = exactTimes + noise

        if fitType == 'LinearFit':
            result = directionForHorizontalArray(pos, times)
        elif fitType == 'BruteForce':
            result = directionBruteForceSearch(pos, times)
        elif fitType == 'Triangles':
            result = directionFromAllTriangles(pos, times)
        else:
            raise ValueError("Wrong fit type specified, must be LinearFit, BruteForce or Triangles")

        results_az[i] = result[0]
        results_el[i] = result[1]

    results_el = np.nan_to_num(results_el)
    results_az = np.nan_to_num(results_az)
    el_avg = np.average(results_el)
    el_std = np.std(results_el)
    az_avg = np.average(results_az)
    az_std = np.std(results_az)

    u_el = el_std / np.sqrt(n_trials)
    u_az = az_std / np.sqrt(n_trials)
#    print results_el.nansum

    print 'Average az = %f' % az_avg
    print 'Average el = %f' % el_avg
    print ' '
    print 'Difference az = %f' % (az_avg - az)
    print 'Difference el = %f' % (el_avg - el)
    print ' '
    print 'Uncertainty az (stddev / sqrt(n_trials)) = %f' % u_az
    print 'Uncertainty el = %f' % u_el


def directionFromAllTriangles(positions, times):
    """
    Make all possible N(N-1)(N-2) / 6 triangles from the antenna positions
    Find the direction of arrival belonging to each triangle;
    Average over all directions to find the best (?) estimate...
    """
    N = len(times)
    trianglecount = N * (N - 1) * (N - 2) / 6  # that's a whole lot, O(N^3)!
    count = 0
    validcount = 0
    solutions_az = np.zeros(trianglecount)
    solutions_el = np.zeros(trianglecount)
    for i in range(N):
        for j in range(i + 1, N):
            for k in range(j + 1, N):
                pos1 = positions[3 * i:3 * (i + 1)]
                pos2 = positions[3 * j:3 * (j + 1)]
                pos3 = positions[3 * k:3 * (k + 1)]
                count += 1
                triangle = np.concatenate((pos1, pos2, pos3))  # need to specify the sequence of arrays as a tuple
                triangleTimes = np.array([times[i], times[j], times[k]])

                (az1, el1, az2, el2) = directionFromThreeAntennas(triangle, triangleTimes)
                # from the two solutions get the one with the highest elevation
                if el2 > el1:
                    az = az2
                    el = el2
                else:
                    az = az1
                    el = el1  # ah, this looks clumsy. Why no (az, el) = (el2 > el1) ? (az2, el2) : (az1, el1)... Hmm, not that pretty either.

                if not (np.isnan(el) or np.isnan(az)):  # valid solution
                    solutions_az[validcount] = az
                    solutions_el[validcount] = el
                    validcount += 1

    solutions_az = solutions_az[0:validcount]
    solutions_el = solutions_el[0:validcount]

    # make direction vectors for all the az, el results, get the average vector, get az, el back from that...
    # that's a better way of averaging the results
    outvec_x = cos(solutions_el) * cos(solutions_az)
    outvec_y = cos(solutions_el) * sin(solutions_az)
    outvec_z = sin(solutions_el)

    avg_x = np.average(outvec_x)
    avg_y = np.average(outvec_y)
    avg_z = np.average(outvec_z)

    el_avg = np.arcsin(avg_z)
    az_avg = np.arctan2(avg_y, avg_x)

#    az_avg = np.average(solutions_az)  # This is a wrong way of averaging results! So no longer using it
#    el_avg = np.average(solutions_el)

    # u_az = np.std(solutions_az) / np.sqrt(validcount) # And then this way of getting the uncertainty is also no longer valid
    # u_el = np.std(solutions_el) / np.sqrt(validcount)

#    print '# Average az = %f' %az_avg
#    print '# Average el = %f' %el_avg
#    print ' '
#    print 'Difference az = %f' %(az_avg - az)
#    print 'Difference el = %f' %(el_avg - el)
#    print ' '
#    print '# Uncertainty az (stddev / sqrt(n_trials)) = %f' %u_az
#    print '# Uncertainty el = %f' %u_el
    print '# Total triangles = %d, valid = %d' % (count, validcount)
    return (az_avg, el_avg)


def timeDelayAzElStandard(position, az, el):
    """Calculate arrival time delays at a set of antennas for
    a signal comming from Azimuth *az* measured Westwards positive from
    North and Elevation *el* measured positive from 0 at the horizon to pi/2
    at zenith.

    *posision* should be a NumPy array of shape (nantennas, 3) where first
    coordinate is East, second North and third Up in meters.

    Returns time delays in seconds.
    """

    # Calculate Cartesian direction
    x = -1.0 * np.cos(el) * np.sin(az)
    y = np.cos(el) * np.cos(az)
    z = np.sin(el)

    direction = np.array([x, y, z])

    return (-1.0 / c) * np.dot(position, direction)


def findDirectionBruteAzElStandard(positions, delays, Ns=100):
    """Find direction of plane wave source based on time *delays* measured
    at given *positions* using a brute force grid search.

    *posision* should be a NumPy array of shape (nantennas, 3) where first
    coordinate is East, second North and third Up in meters.

    *delays* should be a one dimensional NumPy array of length nantennas
    with the values in seconds.

    Returns an array with Azimuth *az* measured Westwards positive from
    North and Elevation *el* measured positive from 0 at the horizon to pi/2
    at zenith.
    """

    def inner(params):
        az, el = params
        return np.sum(np.square(timeDelayAzElStandard(positions, az, el) - delays))

    return brute(inner, ranges=((0., 2 * np.pi), (0., np.pi / 2)), Ns = Ns)


def findDirectionFminAzElStandard(positions, delays):
    """Find direction of plane wave source based on time *delays* measured
    at given *positions* using a downhill simplex algorithm.

    *posision* should be a NumPy array of shape (nantennas, 3) where first
    coordinate is East, second North and third Up in meters.

    *delays* should be a one dimensional NumPy array of length nantennas
    with the values in seconds.

    Returns an array with Azimuth *az* measured Westwards positive from
    North and Elevation *el* measured positive from 0 at the horizon to pi/2
    at zenith.
    """

    def inner(params):
        az, el = params
        return np.sum(np.square(timeDelayAzElStandard(positions, az, el) - delays))

    return fmin(inner, x0=np.array([np.pi, np.pi / 4]))

# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()
