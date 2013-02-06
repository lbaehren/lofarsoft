"""
Various function that are handy for cr physics analysis and more.
"""

import numpy as np
from pytmf import deg2rad
from pytmf import date2jd


def strdate2jd(strdate):
    """Transforms a date on string form ('2012-01-25 21:11:54') to julian day.
    """
    import re

    date_all = [int(a) for a in re.findall(r'\w+', strdate)]
    date = [date_all[0], date_all[1], float(date_all[2]) + date_all[3] / 24. + date_all[4] / (24 * 60.) + date_all[5] / (24. * 60 * 60)]
    utc = date2jd(date[0], date[1], date[2])

    return utc

# Converts spherical theta, phi to kartesian (x,y,z) vector


def toVector(phi, theta, degrees=True):  # in degrees
    if degrees:
        p = np.deg2rad(phi)
        t = np.deg2rad(theta)
    else:
        p = phi
        t = theta
    a = np.array([np.cos(p) * np.sin(t), np.sin(p) * np.sin(t), np.cos(t)])
    return a

# Converts kartesian (x,y,z) tp spherical theta, phi


def toAngles(array, degrees=True):
    r = np.sqrt(array[0] ** 2 + array[1] ** 2 + array[2] ** 2)
    theta = np.arccos(array[2] / r)
    phi = np.arctan2(array[1], array[0])
    if degrees:
        theta = np.rad2deg(theta)
        phi = np.rad2deg(phi)
    return [phi, theta]


# Averages over list of thetas and phis (standard convention), via transformation in to Cartesian vectors
def averageDirection(philist, thetalist, degrees=True):  # in degrees
    vec = np.array([0., 0., 0.])
    for i in xrange(len(thetalist)):
        theta = thetalist[i]
        phi = philist[i]
        vec += toVector(theta, phi, degrees)
    vec = vec / np.sqrt(vec[0] ** 2 + vec[1] ** 2 + vec[2] ** 2)
    av = toAngles(vec)
    return av

# Averages over list of azimuth and elevations (LOFAR convention) via transformation in to Cartesian vectors


def averageDirectionLOFAR(azlist, ellist, degrees=True):  # in degrees

    if degrees:
        philist = 90 - np.asarray(ellist)
        thetalist = 90 - np.asarray(azlist)
    else:
        philist = np.pi / 2. - np.asarray(ellist)
        thetalist = np.pi / 2. - np.asarray(azlist)

    av = averageDirection(philist, thetalist, degrees)

    av[0] = 90 - av[0]
    av[1] = 90 - av[1]

    return av

# Using cuts on LORA reconstruction, will yield true when LORA reconstruction is considered reliable


def applyLORAcuts(core, moliere, elevation):
    quality = False

    if math.sqrt(core[0] ** 2 + core[1] ** 2) < 150:
        if moliere < 100 and moliere > 20:
            if elevation > 55:
                quality = True

    return quality

# Calculates space angle between two sets of angles (convetion kartesian)


def spaceAngle(zen1, az1, zen2, az2):
    return np.arccos(np.sin(zen1) * np.sin(zen2) * np.cos(az1 - az2)
                + np.cos(zen1) * np.cos(zen2))


def select_quadrant(antenna_coordinates, core_position, rotation=45.0):
    """
    *antenna_coordinates* as numpy array of shape (nantennas, 3)
    *core_position* as numpy array of shape (3, )
    *rotation* rotation of antennas with respect to North in degrees.
    """

    # Get only first two antennas
    ac = antenna_coordinates[:, 0:2]

    # Convert angle to radians
    theta = deg2rad(rotation)

    # Setup rotation matrix
    R = np.array([[np.cos(theta), -1.0 * np.sin(theta)], [np.sin(theta), np.cos(theta)]])

    # Calculate difference in coordinates
    diff_c = ac - np.repeat(core_position[0:2], ac.shape[0]).reshape(2, ac.shape[0]).transpose()

    # Rotate and return quadrant for each antenna
    q = np.zeros(ac.shape[0])
    for i in range(ac.shape[0]):

        temp = np.dot(R, diff_c[i])

        if temp[0] >= 0 and temp[1] >= 0:
            q[i] = 0
        elif temp[0] < 0 and temp[1] > 0:
            q[i] = 1
        elif temp[0] >= 0 and temp[1] <= 0:
            q[i] = 2
        elif temp[0] < 0 and temp[1] < 0:
            q[i] = 3
        else:
            raise ValueError("Something went wrong here.")

    return q
