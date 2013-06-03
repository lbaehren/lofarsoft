# Script to transform from ITRF to latitude/longitude and UTM coordinates
# This script has to be included in each octocopter timing script.
# Version: Maria Krause, April 2013

import numpy as np
import math
import scipy
from scipy import constants
#import octocopter_position as op

"""Conversion from ITRF to latitude and longitude to UTM"""
_deg2rad = np.pi / 180.0
_rad2deg = 180.0 / np.pi

_EquatorialRadius = 2
_eccentricitySquared = 3

_ellipsoid = [
#  id, Ellipsoid name, Equatorial Radius, square of eccentricity
# first once is a placeholder only, To allow array indices to match id numbers
	[ -1, "Placeholder", 0, 0],
	[ 1, "Airy", 6377563, 0.00667054],
	[ 2, "Australian National", 6378160, 0.006694542],
	[ 3, "Bessel 1841", 6377397, 0.006674372],
	[ 4, "Bessel 1841 (Nambia] ", 6377484, 0.006674372],
	[ 5, "Clarke 1866", 6378206, 0.006768658],
	[ 6, "Clarke 1880", 6378249, 0.006803511],
	[ 7, "Everest", 6377276, 0.006637847],
	[ 8, "Fischer 1960 (Mercury] ", 6378166, 0.006693422],
	[ 9, "Fischer 1968", 6378150, 0.006693422],
	[ 10, "GRS 1967", 6378160, 0.006694605],
	[ 11, "GRS 1980", 6378137, 0.00669438],
	[ 12, "Helmert 1906", 6378200, 0.006693422],
	[ 13, "Hough", 6378270, 0.00672267],
	[ 14, "International", 6378388, 0.00672267],
	[ 15, "Krassovsky", 6378245, 0.006693422],
	[ 16, "Modified Airy", 6377340, 0.00667054],
	[ 17, "Modified Everest", 6377304, 0.006637847],
	[ 18, "Modified Fischer 1960", 6378155, 0.006693422],
	[ 19, "South American 1969", 6378160, 0.006694542],
	[ 20, "WGS 60", 6378165, 0.006693422],
	[ 21, "WGS 66", 6378145, 0.006694542],
	[ 22, "WGS-72", 6378135, 0.006694318],
	[ 23, "WGS-84", 6378137, 0.00669438]
]

#Reference ellipsoids derived from Peter H. Dana's website-
#http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
#Department of Geography, University of Texas at Austin
#Internet: pdana@mail.utexas.edu
#3/22/95

#Source
#Defense Mapping Agency. 1987b. DMA Technical Report: Supplement to Department of Defense World Geodetic System
#1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency

#def LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long,
#			 double &UTMNorthing, double &UTMEasting, char* UTMZone)

def LLtoUTM(ReferenceEllipsoid, Lat, Long):
#converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
#East Longitudes are positive, West longitudes are negative.
#North latitudes are positive, South latitudes are negative
#Lat and Long are in decimal degrees
#Written by Chuck Gantz- chuck.gantz@globalstar.com

    a = _ellipsoid[ReferenceEllipsoid][_EquatorialRadius]
    eccSquared = _ellipsoid[ReferenceEllipsoid][_eccentricitySquared]
    k0 = 0.9996

#Make sure the longitude is between -180.00 .. 179.9
    LongTemp = (Long+180)-int((Long+180)/360)*360-180 # -180.00 .. 179.9

    LatRad = Lat*_deg2rad
    LongRad = LongTemp*_deg2rad

    ZoneNumber = int((LongTemp + 180)/6) + 1

    if Lat >= 56.0 and Lat < 64.0 and LongTemp >= 3.0 and LongTemp < 12.0:
        ZoneNumber = 32

    # Special zones for Svalbard
    if Lat >= 72.0 and Lat < 84.0:
        if  LongTemp >= 0.0  and LongTemp <  9.0:ZoneNumber = 31
        elif LongTemp >= 9.0  and LongTemp < 21.0: ZoneNumber = 33
        elif LongTemp >= 21.0 and LongTemp < 33.0: ZoneNumber = 35
        elif LongTemp >= 33.0 and LongTemp < 42.0: ZoneNumber = 37

    LongOrigin = (ZoneNumber - 1)*6 - 180 + 3 #+3 puts origin in middle of zone
    LongOriginRad = LongOrigin * _deg2rad

    #compute the UTM Zone from the latitude and longitude
    UTMZone = "%d%c" % (ZoneNumber, _UTMLetterDesignator(Lat))

    eccPrime1 = (eccSquared)/(1-eccSquared)
    N = a/np.sqrt(1-eccSquared*np.sin(LatRad)*np.sin(LatRad))
    T = np.tan(LatRad)*np.tan(LatRad)
    C = eccPrime1*np.cos(LatRad)*np.cos(LatRad)
    A = np.cos(LatRad)*(LongRad-LongOriginRad)

    M = a*((1
            - eccSquared/4
            - 3*eccSquared*eccSquared/64
            - 5*eccSquared*eccSquared*eccSquared/256)*LatRad
           - (3*eccSquared/8
              + 3*eccSquared*eccSquared/32
              + 45*eccSquared*eccSquared*eccSquared/1024)*np.sin(2*LatRad)
           + (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*np.sin(4*LatRad)
           - (35*eccSquared*eccSquared*eccSquared/3072)*np.sin(6*LatRad))

    UTMEasting = (k0*N*(A+(1-T+C)*A*A*A/6
                        + (5-18*T+T*T+72*C-58*eccPrime1)*A*A*A*A*A/120)
                  + 500000.0)

    UTMNorthing = (k0*(M+N*np.tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
                                        + (61
                                           -58*T
                                           +T*T
                                           +600*C
                                           -330*eccPrime1)*A*A*A*A*A*A/720)))

    if Lat < 0:
        UTMNorthing = UTMNorthing + 10000000.0; #10000000 meter offset for southern hemisphere
    return (UTMZone, UTMEasting, UTMNorthing)


def _UTMLetterDesignator(Lat):
#This routine determines the correct UTM letter designator for the given latitude
#returns 'Z' if latitude is outside the UTM limits of 84N to 80S
#Written by Chuck Gantz- chuck.gantz@globalstar.com

    if 84 >= Lat >= 72: return 'X'
    elif 72 > Lat >= 64: return 'W'
    elif 64 > Lat >= 56: return 'V'
    elif 56 > Lat >= 48: return 'U'
    elif 48 > Lat >= 40: return 'T'
    elif 40 > Lat >= 32: return 'S'
    elif 32 > Lat >= 24: return 'R'
    elif 24 > Lat >= 16: return 'Q'
    elif 16 > Lat >= 8: return 'P'
    elif  8 > Lat >= 0: return 'N'
    elif  0 > Lat >= -8: return 'M'
    elif -8> Lat >= -16: return 'L'
    elif -16 > Lat >= -24: return 'K'
    elif -24 > Lat >= -32: return 'J'
    elif -32 > Lat >= -40: return 'H'
    elif -40 > Lat >= -48: return 'G'
    elif -48 > Lat >= -56: return 'F'
    elif -56 > Lat >= -64: return 'E'
    elif -64 > Lat >= -72: return 'D'
    elif -72 > Lat >= -80: return 'C'
    else: return 'Z'	# if the Latitude is outside the UTM limits

a = 6378137											#Semi-major axis
b = 6356752.314245							#Semi-minor axis
fc = 0.0033528106718309896			#Flattening
e1 = 2*fc-fc*fc									#First eccentricity  e*e
e = np.sqrt(e1)
e2= (a*a-b*b)/(b*b)							#Second eccentricity e'*e'

"""Calculate the time delay with the original octocopter positions above the array"""
def transformation(Time_array,t_UTC, Lat_array, Lon_array, Ele_array, WGS_array, ID_array, ID, X_array, Y_array, Z_array):
	t_exp = []					#expected time
	time_diff_exp = []
	east = []
	north = []
	height = []
	distance = []
	lon = np.zeros((len(X_array)))
	lat = np.zeros((len(X_array)))
	h_ant = np.zeros((len(X_array)))
	east_ant = np.zeros((len(X_array)))
	north_ant = np.zeros((len(X_array)))
	for i in range(0,len(Time_array)):
		if Time_array[i]==t_UTC[0]:
			"""Octocopter position"""
			(zocto, eastocto, northocto) = LLtoUTM(23, Lat_array[i], Lon_array[i])
			eastocto = eastocto#+0.274
			northocto = northocto #-1.511
			hocto = Ele_array[i]+WGS_array[i]#+0.258
			for j in range(0,len(ID_array)):
				#if ID_array[j]=='002000000':
					# Position of the central antenna of LOFAR to calculate the time delays with respect to that antenna
				#	lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
				#	p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
				#	r = np.sqrt(p*p + Z_array[j]*Z_array[j])
				#	mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
				#	lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
				#	h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
				#	(z, east0, north0) = LLtoUTM(23, lat0, lon0)
				for k in range(0,len(ID)):
					if ID_array[j][3:]=='000001' and ID_array[j]==ID[k]:
						"""Central antenna position"""
						lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[j])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
						(z, east0, north0) = LLtoUTM(23, lat0, lon0)
						pos_east0 = east0-eastocto
						pos_north0 = north0-northocto
						pos_height0 = h0-hocto
						dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
						texp0 = dist0/(constants.c*1e-9)
					if ID_array[j][3:]=='000000' and ID_array[j]==ID[k]:
						"""Central antenna position"""
						lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[j])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
						(z, east0, north0) = LLtoUTM(23, lat0, lon0)
						pos_east0 = east0-eastocto
						pos_north0 = north0-northocto
						pos_height0 = h0-hocto
						dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
						texp0 = dist0/(constants.c*1e-9)
					"""Each antenna position"""
					if ID_array[j]==ID[k]:
						lon[j] = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[j])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat[j] = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h_ant[j] = p*np.cos(lat[j]*_deg2rad)+Z_array[j]*np.sin(lat[j]*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat[j]*_deg2rad),2))
						(z, east_ant[j], north_ant[j]) = LLtoUTM(23, lat[j], lon[j])
						pos_east = east_ant[j]-eastocto
						pos_north = north_ant[j]-northocto
						pos_height = h_ant[j]-hocto
						dist = np.sqrt( pow(pos_east,2) + pow(pos_north,2) + pow(pos_height,2))
						d = np.sqrt(pow(east0-east_ant[j],2)+pow(north0-north_ant[j],2)+pow(h0-h_ant[j],2))
						distance.append(d)
						texp = (dist-dist0)/(constants.c*1e-9)
						tdiff = texp
						time_diff_exp.append(tdiff)
						east.append(east_ant[j])
						north.append(north_ant[j])
						height.append(h_ant[j])
		elif (Time_array[i]+2.0==t_UTC[0] and t_UTC[0]==1348058616):				#for file L65325_D20120919T124336
			"""Octocopter position"""
			(zocto, eastocto, northocto) = LLtoUTM(23, Lat_array[i], Lon_array[i])
			eastocto = eastocto#+0.274
			northocto = northocto #-1.511
			hocto = Ele_array[i]+WGS_array[i]#+0.258
			for j in range(0,len(ID_array)):
				for k in range(0,len(ID)):
					if ID_array[j][3:]=='000001' and ID_array[j]==ID[k]:
						"""Central antenna position"""
						lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[k])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
						(z, east0, north0) = LLtoUTM(23, lat0, lon0)
						pos_east0 = east0-eastocto
						pos_north0 = north0-northocto
						pos_height0 = h0-hocto
						dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
						texp0 = dist0/(constants.c*1e-9)
					if ID_array[j][3:]=='000000' and ID_array[j]==ID[k]:
						"""Central antenna position"""
						lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[k])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
						(z, east0, north0) = LLtoUTM(23, lat0, lon0)
						pos_east0 = east0-eastocto
						pos_north0 = north0-northocto
						pos_height0 = h0-hocto
						dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
						texp0 = dist0/(constants.c*1e-9)
					"""Each antenna position"""
					if ID_array[j]==ID[k]:
						lon[j] = np.arctan(Y_array[j]/X_array[j])*_rad2deg
						p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
						r = np.sqrt(p*p + Z_array[j]*Z_array[j])
						mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
						lat[j] = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
						h_ant[j] = p*np.cos(lat[j]*_deg2rad)+Z_array[j]*np.sin(lat[j]*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat[j]*_deg2rad),2))
						(z, east_ant[j], north_ant[j]) = LLtoUTM(23, lat[j], lon[j])
						pos_east = east_ant[j]-eastocto
						pos_north = north_ant[j]-northocto
						pos_height = h_ant[j]-hocto
						dist = np.sqrt( pow(pos_east,2) + pow(pos_north,2) + pow(pos_height,2))
						d = np.sqrt(pow(east0-east_ant[j],2)+pow(north0-north_ant[j],2)+pow(h0-h_ant[j],2))
						distance.append(d)
						texp = (dist-dist0)/(constants.c*1e-9)
						tdiff = texp
						time_diff_exp.append(tdiff)
						east.append(east_ant[j])
						north.append(north_ant[j])
						height.append(h_ant[j])

	return time_diff_exp, north, east, height, northocto, eastocto, hocto, north0, east0, h0, distance

def central_antenna_position(x0_station,y0_station,z0_station):
	lon = np.arctan(y0_station/x0_station)*_rad2deg
	p = np.sqrt(x0_station*x0_station + y0_station*y0_station)
	r = np.sqrt(p*p + z0_station*z0_station)
	mu = np.arctan(z0_station/p * ( (1-fc)+e1*a/r ))*_rad2deg
	lat = np.arctan( (z0_station*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
	(z, east0_station, north0_station) = LLtoUTM(23, lat, lon)

	return east0_station, north0_station
