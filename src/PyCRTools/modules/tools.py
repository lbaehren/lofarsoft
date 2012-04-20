"""
Various function that are handy for cr physics analysis.
"""

import numpy as np

def toVector(theta, phi, degrees = True):# in degrees
    if degrees:
        p = np.deg2rad(phi)
        t = np.deg2rad(theta)
    else:
        p = phi
        t = theta    
    a = np.array([np.cos(p)*np.sin(t),np.sin(p)*np.sin(t),np.cos(t)])
    return a
    
def toAngles(array, degrees = True):
    r = np.sqrt(array[0]**2+array[1]**2+array[2]**2)
    theta = np.arccos(array[2]/r)
    phi = np.arctan(array[1]/array[0])
    if degrees:
        theta = np.rad2deg(theta)
        phi = np.rad2deg(phi)
    return [theta, phi]
    

def averageDirection(thetalist, philist, degrees = True): # in degrees
    vec = np.array([0.,0.,0.])
    for i in xrange(len(thetalist)):
        theta = thetalist[i]
        phi = philist[i]
        vec += toVector(theta, phi, degrees)
    vec = vec/np.sqrt(vec[0]**2+vec[1]**2+vec[2]**2)
    av = toAngles(vec)    
    return av

def averageDirectionLOFAR(thetalist, philist, degrees = True): # in degrees    
    vec = np.array([0.,0.,0.])
    for i in xrange(len(thetalist)):
        if degrees:
            theta = 90 - thetalist[i]
            phi = 90 - philist[i]
        else:
            theta = np.pi/2. - thetalist[i]
            phi = np.pi/2. - philist[i]                
        vec += toVector(theta, phi, degrees)
    vec = vec/np.sqrt(vec[0]**2+vec[1]**2+vec[2]**2)
    av = toAngles(vec)
    av[0] = 90 - av[0]
    av[1] = 90 - av[1]  
    return av

# Using cuts on LORA reconstruction, will yield true when LORA reconstruction is considered reliable
def applyLORAcuts(core,moliere,elevation):   
    quality = False 
    
    if math.sqrt(core[0]**2 + core[1]**2) < 150:
        if moliere < 100 and moliere > 20:
            if elevation > 55:
                quality = True
                
    return quality 
