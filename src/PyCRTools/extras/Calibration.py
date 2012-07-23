#!/usr/bin/env python
# -*- coding: utf-8 -*-

##########################################################################
#
# Software to calibrate LOFAR TBB data from a Single Station (All-Sky)
#
##########################################################################

from pylab import *
from numpy import *
from scipy import *
import numpy as np
import time
import os
import re
import sys
import pickle
import pyfits
import threading
from optparse import OptionParser

usage = "usage: %prog [options] filename channel"
parser = OptionParser(usage=usage)
parser.add_option("-n", "--no-calibration",
                  action = "store_false", dest="calibrate", default=True,
                  help = "do not calibrate the data before imaging")
parser.add_option("-a", "--calibrate-all-frequencies",
                  action = "store_true", dest="calibrate_all_frequencies", default=False,
                  help = "calibrate over all frequencies")

(options, args) = parser.parse_args()

if len(sys.argv) != 3:
    parser.print_help()
    exit()

filename = str(sys.argv[1]).split(".")[0]
channel = int(sys.argv[2])

t1 = time.time()

#-------------parameters-----------------------------------------

savefits = False
c = 2.9979245e8
RFIS = False

#-------------read the dataset-----------------------------------

dataset = open(filename+".pickle") #open(dataset)
antposp = pickle.load(dataset)
timedate = pickle.load(dataset)
timeutc = pickle.load(dataset)
timetuple = pickle.load(dataset)
station = pickle.load(dataset)
freliste = pickle.load(dataset)		# in MHz

data = np.load(filename+".npy")
print "MYSHAPE", data.shape
#if u != 0:
#	for uc in range(0,u-1,1):
#		data2 = pickle.load(dataset)
#		data = vstack((data,data2))
#		del data2

print "\n\nFILE INFORMATIONS: \n"
print "Observing Time      ", timedate
print "Obs Time in UTC     ", timeutc
print "Station             ", station
print "Startfrequency      ", round(freliste[0],2),"MHz"
print "Endfrequency        ", round(freliste[-1],2),"MHz"
print "Number of Channels  ", len(freliste)
print "-------------------------------------------\n"

#-------------Position of station and antennas-------------------

antennafilename = os.environ["LOFARSOFT"]+"/data/lofar/StaticMetaData/AntennaArrays/"+station+"-AntennaArrays.conf"
print antennafilename

if (0 <= freliste[0]) and( freliste[-1] <= 100):
	field = 'LBA_OUTER'
elif (110 <= freliste[0]) and( freliste[-1] <= 250):
	field = 'HBA'
else:
	print "wrong frequency falues"
	exit()

nrcu = len(data)
position = open(antennafilename).readlines()

#xpos = []
#ypos = []

count = 0
for lines in position:
	sline=lines.split()
	if sline[0] == field : break
	count += 1

station = position[count+1].split()
longitude = float(station[2])
latitude = float(station[3])
#
#rcu = position[count+2].split()
##nrcu = 2*int(rcu[0])
#print "bla", nrcu
#
#for line in range(count+3, count+3+nrcu/2, 1):
#	  token = position[line].split()
#	  if len(token) >= 2:
#		  xpos.append(float(token[0]))
#		  ypos.append(float(token[1]))
#
#xm = sum(xpos)/nrcu
#ym = sum(ypos)/nrcu

#xpos -= xm
#ypos -= ym

xpos = antposp[0::2,0]
ypos = antposp[0::2,1]

plt.scatter(xpos, ypos)

baselines = zeros((nrcu,nrcu))
baselinesint = zeros((nrcu,nrcu), dtype = int)

for rcu1 in range(0, nrcu/2, 1):
	for rcu2 in range(0, nrcu/2,1):
		baselines[2*rcu1,2*rcu2] = sqrt((xpos[rcu2]-xpos[rcu1])**2 + (ypos[rcu2]-ypos[rcu1])**2)
		baselines[2*rcu1+1,2*rcu2+1] = baselines[2*rcu1,2*rcu2]
		baselinesint[2*rcu1,2*rcu2] = int(baselines[2*rcu1,2*rcu2])
		baselinesint[2*rcu1+1,2*rcu2+1] = int(baselines[2*rcu1,2*rcu2])

print xpos.shape, (1, nrcu/2), nrcu
print ypos.shape, (1, nrcu/2), nrcu
xpos = reshape(xpos,(1,nrcu/2))
ypos = reshape(ypos,(1,nrcu/2))


#-------------Calculation of sourcepositions---------------------

year = float(timetuple[0])
month = float(timetuple[1])
day = float(timetuple[2])
hour = float(timetuple[3]) 	
minu = float(timetuple[4])
sec= float(timetuple[5])

if month <= 2:
  year -= 1
  month += 12
H = hour/24 + minu/1440 + sec/86400
JD = int(365.25*(year+4716))+int(30.6001*(month+1))+day+H+2+int((int(year/100))/4)-int(year/100)-1524.5

n = JD - 2451545.0    	#time since standard equinox J2000
T = float(n)/36525
GWhourangle = mod(280.46061838+13185000.77*T+T*T/2577.765-T*T*T/38710000, 360)	#Greenwich hourangle
vernalequinox = mod(GWhourangle + longitude, 360)	#hourangle of vernal equinox

#[source, rascension(deg, min, sec), declination(deg, min, sec), a, b, c, Intensity(38MHz)]
# see Baars et al., 1977 :
# log S[Jy] = a + b * log v[MHz] + c * (log v[MHz])**2 

sourcelis = array([ ("CasA", 23.0, 23.0, 27.9, 58.0, 48.0, 42.0, 5.625, -0.634, -0.023, 37200),
		("CygA", 19.0, 59.0, 28.3, 40.0, 44.0, 2.0, 5.745, -0.77, 0.0, 22000), 
		("TauA", 5.0, 34.0, 32.0, 22.0, 0.0, 52.0, 3.915, -0.299, 0.0,2430), 
		("VirA", 12.0, 30.0, 49.4, 12.0, 23.0, 28.0, 5.023, -0.856, 0.0,4000), 
		("HerA", 16.0, 51.0, 37.73, 4.0, 58.0, 33.88, 4.963, -1.052, 0.0,1800), 
		("HydA", 9.0, 18.0, 5.7, -12.0, -5.0, -44.0, 4.497, -0.91, 0.0,1200) ])#, 
		#("PerA", 3.0, 19.0, 48.1, 41.0, 30.0, 42.0, 340), 
		#("SgrA", 17.0, 45.0, 40.0, -29.0, 0.0, -28.0, 1) ])

#----------------------------------------------------------------

def source_l_m(so):

	if str(so) == "Sun":
		L = mod(280.460 + 0.9856474*n, 360)     #mean ecliptic longitude of the sun; aberration is included
		g = mod(357.528 + 0.9856003*n, 360)   #mean anormaly
		delta = mod((L + 1.915*math.sin(pi*g/180)+0.02*math.sin(2*pi*g/180)), 360)   #ecliptic longitude of the sun
		ecliptic = (23.439 - 0.0000004*n)*pi/180

		rascension = math.atan(math.cos(ecliptic)*math.sin(pi/180*delta)/cos(pi/180*delta))*180/pi 
		if cos(pi/180*delta) < 0:
		    rascension += 180
		declination = math.asin(math.sin(ecliptic)*sin(pi/180*delta))
		I = 1000

	else:
		for source in range(0,len(sourcelis),1): 
			if str(so) == str(sourcelis[source][0]):
				break

		rascension = (float(sourcelis[source][1])+float(sourcelis[source][2])/60+float(sourcelis[source][3])/3600)*15
		declination = (float(sourcelis[source][4])+float(sourcelis[source][5])/60+float(sourcelis[source][6])/3600)*pi/180

	hourangle = (vernalequinox - rascension)*pi/180

	Elevation = math.asin(math.cos(declination)*math.cos(hourangle)*math.cos(latitude*pi/180)+sin(declination)*sin(latitude*pi/180))*180/pi
	Azimuth = math.atan(math.sin(hourangle)/(math.sin(latitude*pi/180)*math.cos(hourangle)-math.cos(latitude*pi/180)*math.tan(declination)))*180/pi+180
	if (math.cos(hourangle)*sin(latitude*pi/180)-math.tan(declination)*cos(latitude*pi/180))<0: 
	    Azimuth += 180
	if Azimuth < 0:
	   Azimuth += 360
	Azimuth = mod(Azimuth, 360)

	#Conversion into direction cosines

	if Elevation >= 0:
		l = (math.cos((90-Azimuth)*pi/180))*(math.cos((Elevation)*pi/180))
		m = (math.sin((90-Azimuth)*pi/180))*(math.cos((Elevation)*pi/180))
		I = 10**(float(sourcelis[source][7])+float(sourcelis[source][8])*log10(f/10**6)+float(sourcelis[source][9])*(log10(f/10**6))**2)
	else:
		l = 'NaN'
		m = 'NaN'
		I = 'NaN'

	return l,m,I


#-------------Flagging in Baselines------------------------------

def FLAGGING(dat, flag):
	print "\n- Start Flagging in Baselines -"
	basel = []
	aver_basel = []
	num_basel = []
	aver_basel2 = []
	
#########doppelte Schleifen vielleicht ersetzen -> Array flatten, dann durch alle Werte durchgehen

	for rcu1 in range(0,nrcu,2):
		for rcu2 in range(0,nrcu,2):
			count = 0
			for line in range(0,len(basel),1):
				if (baselinesint[rcu1,rcu2] == basel[line]):
					count += 1
					aver_basel[line] += abs(dat[rcu1,rcu2])
					aver_basel2[line] += abs(dat[rcu1+1,rcu2+1])
					num_basel[line] += 1
			if count == 0 and baselinesint[rcu1,rcu2] != 0:
				basel.append(baselinesint[rcu1,rcu2])
				aver_basel.append(abs(dat[rcu1,rcu2]))
				aver_basel2.append(abs(dat[rcu1+1,rcu2+1]))
				num_basel.append(1)

	num_basel = reshape(num_basel,(len(num_basel),1))
	aver_basel = reshape(aver_basel,(len(aver_basel),1))
	aver_basel /= num_basel
	aver_basel2 = reshape(aver_basel2,(len(aver_basel2),1))
	aver_basel2 /= num_basel

	for rcu1 in range(0,nrcu,2):
		for rcu2 in range(0,nrcu,2):
			for line in range(0,len(basel),1):
				if baselinesint[rcu1,rcu2] == basel[line] and abs(dat[rcu1,rcu2]) >= flag_max*aver_basel[line]:
					dat[rcu1,rcu2] = 0.0
					flag[rcu1,rcu2] = 0.0
				elif baselinesint[rcu1,rcu2] == basel[line] and abs(dat[rcu1,rcu2]) <= flag_min*aver_basel[line]:
					dat[rcu1,rcu2] = 0.0
					flag[rcu1,rcu2] = 0.0

				if baselinesint[rcu1+1,rcu2+1] == basel[line] and abs(dat[rcu1+1,rcu2+1]) >= flag_max*aver_basel2[line]:
					dat[rcu1+1,rcu2+1] = 0.0
					flag[rcu1+1,rcu2+1] = 0.0
				elif baselinesint[rcu1+1,rcu2+1] == basel[line] and abs(dat[rcu1+1,rcu2+1]) <= flag_min*aver_basel2[line]:
					dat[rcu1+1,rcu2+1] = 0.0
					flag[rcu1+1,rcu2+1] = 0.0

	return dat, flag
			
#-------------Calibration of gains-------------------------------

def gain_cal_autocorr(datas_c,gain, flag):
	print "\n- Start Calculating Gains using Autocorrelations -\n"
	
	aver_XX = 0.0
	aver_YY = 0.0
	count_X = 0
	count_Y = 0

	sort_data_X = zeros((nrcu/2,2))
	sort_data_Y = zeros((nrcu/2,2))
	"""
	
	for rcu in range(0, nrcu,2):
		sort_data_X[rcu/2] = ([abs(datas_cal[rcu,rcu]),rcu])
		sort_data_Y[rcu/2] = ([abs(datas_cal[rcu+1,rcu+1]),rcu+1])
		
	sort_data_X = sorted(sort_data_X, key=lambda dat:dat[0], reverse=True)
	sort_data_Y = sorted(sort_data_Y, key=lambda dat:dat[0], reverse=True)

	while sort_data_X[0][0] >= 100*sort_data_X[50][0]:
		print "rcu",str(int(sort_data_X[0][1])),"flagged"
		datas_cal[int(sort_data_X[0][1]),int(sort_data_X[0][1])] = 0
		sort_data_X = sort_data_X[1:]


	while sort_data_Y[0][0] >= 100*sort_data_Y[50][0]:
		print "rcu",str(int(sort_data_Y[0][1])),"flagged"
		datas_cal[int(sort_data_Y[0][1]),int(sort_data_Y[0][1])] = 0
		sort_data_Y = sort_data_Y[1:]

	"""

	datas_cal = data[:,:,len(freliste)/2]
	for rcu in range(0,nrcu,2):
		if (abs(datas_cal[rcu,rcu])) != 0:
			aver_XX += (datas_cal[rcu,rcu]).real
			count_X += 1
		if (abs(datas_cal[rcu+1,rcu+1])) != 0:
			aver_YY += (datas_cal[rcu+1,rcu+1]).real
			count_Y += 1

	#meany = mean(abs(datas[1:nrcu:2,1:nrcu:2]))
	#print count_X, count_Y
	#$exit()
	if count_X == 0: count_X = 1
	if count_Y == 0: count_Y = 1
	aver_XX /= count_X
	aver_YY /= count_Y
	#print aver_YY
	#print aver_XX
	#print meany
	#exit()
	new_aver_XX = 0.0
	new_aver_YY = 0.0
	count_X = 0
	count_Y = 0
	XX_limit_low = 0.25*aver_XX
	XX_limit_high = 4.0*aver_XX
	YY_limit_low = 0.25*aver_YY
	YY_limit_high = 4.0*aver_YY

	for rcu in range(0,nrcu,2):
		if (XX_limit_low < datas_cal[rcu,rcu].real < XX_limit_high):
			new_aver_XX += (datas_c[rcu,rcu]).real
			count_X += 1
		else:
			datas_c[rcu] = 0.0
			datas_c[:,rcu] = 0.0
			flag[rcu] = 0.0
			flag[:,rcu] = 0.0
			print "   RCU %3i flagged, because of bad values" % (rcu)

		if (YY_limit_low < datas_cal[rcu+1,rcu+1].real < YY_limit_high):
			new_aver_YY += (datas_c[rcu+1,rcu+1]).real
			count_Y += 1
		else:
			datas_c[rcu+1] = 0.0
			datas_c[:,rcu+1] = 0.0
			flag[rcu+1] = 0.0
			flag[:,rcu+1] = 0.0
			print "   RCU %3i flagged, because of bad values" % (rcu+1)

	if count_X != 0:
		new_aver_XX /= count_X
	if count_Y != 0:
		new_aver_YY /= count_Y

	gain = zeros((nrcu), dtype = complex)

	
	for rcu in range(0,nrcu,2):	
		if datas_c[rcu,rcu] != 0:
			gain[rcu] = sqrt(new_aver_XX/(datas_c[rcu,rcu]).real)
		if datas_c[rcu+1,rcu+1] != 0:
			gain[rcu+1] = sqrt(new_aver_YY/(datas_c[rcu+1,rcu+1]).real)	


	return datas_c, gain, flag
	
#-------------Calculating the amplitudes-------------------------

ll = []
mm = []
s = (-1.0)
while s <= 1.0:
	ll.append(s)
	if savefits:
		mm.append(-s)
	else:
		mm.append(s)
	s += 0.01

lenght_l = len(ll)
lenght_m = len(mm)

def cal_amp(lp,mp,dat):
	#print "\n- start calculating model -"
	ampX = 0.0
	ampY = 0.0

	k = 2.0*pi / c  * f
	wx = exp((dot((mat(xpos)).T,(mat(ll))))*-(1j)*k)
	wy = exp((dot((mat(ypos)).T,(mat(mm))))*-(1j)*k)

	acmXX = zeros((nrcu/2,nrcu/2),dtype = complex)
	acmYY = zeros((nrcu/2,nrcu/2),dtype = complex)
	for rcui in range(0,nrcu,2):
		for rcuj in range(0,nrcu,2):
			acmXX[rcui/2,rcuj/2] = dat[rcui,rcuj]
			acmYY[rcui/2,rcuj/2] = dat[rcui+1,rcuj+1]

	acm = acmXX + acmYY
	
	maxi = 0.0

	for mi in range(int(m)-int(beam[fre]), int(m)+int(beam[fre]),1):
		for li in range(int(l)-int(beam[fre]), int(l)+int(beam[fre]),1):
			weight = array(wx[:,li])*array(wy[:,mi])
			ampt = float((dot(dot((weight.conjugate()).T,array(acm)),weight)).real)
			if ampt > maxi:
				maxi = ampt
				mp = mi
				lp = li 

	weight = array(wx[:,lp])*array(wy[:,mp])
	ampX = float((dot(dot((weight.conjugate()).T,array(acmXX)),weight)).real) 
	ampY = float((dot(dot((weight.conjugate()).T,array(acmYY)),weight)).real)

	amp = [ampX, ampY, maxi, lp, mp]

	return amp

#-------------Calculating model visibilities---------------------

def vis_modelc(sourcelist, pol):
	vis_model = zeros((nrcu,nrcu),dtype = complex)
	
	k = 2.0*pi / c  * f

	for source in range(0, len(sourcelist), 1):
		l = (sourcelist[source][3])
		m = (sourcelist[source][4])
		ampX = sourcelist[source][1]*sqrt(1-l**2-m**2)
		ampY = sourcelist[source][2]*sqrt(1-l**2-m**2)

		for rcui in range (0,nrcu,2):
			for rcuj in range(0,nrcu,2):

				vis_1 = (exp(1*k*1j*(xpos[0,int(rcui/2)]*l+ypos[0,int(rcui/2)]*m)))
				vis_2 = (exp(-1*k*1j*(xpos[0,int(rcuj/2)]*l+ypos[0,int(rcuj/2)]*(m))))

				if (pol == 0) or (pol == 2):
				#if pol != 1:
					vis_model[rcui,rcuj] += ampX*vis_1*vis_2
				if (pol == 1) or (pol == 2):
				#if pol != 0:
					vis_model[rcui+1,rcuj+1] += ampY*vis_1*vis_2

	return vis_model


def vis_modelc0(sourcelist, pol):
	vis_model = zeros((nrcu,nrcu),dtype = complex)
	
	k = 2.0*pi / c  * f

	for source in range(0, len(sourcelist), 1):
		l = (sourcelist[source][3])
		m = (sourcelist[source][4])
		ampX = sourcelist[source][1]
		ampY = sourcelist[source][2]

		for rcui in range (0,nrcu,2):
			for rcuj in range(0,nrcu,2):

				vis_1 = (exp(1*k*1j*(xpos[0,int(rcui/2)]*l+ypos[0,int(rcui/2)]*m)))
				vis_2 = (exp(-1*k*1j*(xpos[0,int(rcuj/2)]*l+ypos[0,int(rcuj/2)]*(m))))

				if pol != 1:
					vis_model[rcui,rcuj] += ampX*vis_1*vis_2#/nX
				if pol != 0:
					vis_model[rcui+1,rcuj+1] += ampY*vis_1*vis_2#/nY

	return vis_model


#-------------Calculating the skymap-----------------------------

# for the imaging part:
def SKYMAP(dat, pol):

	acmXX = zeros((nrcu/2,nrcu/2),dtype = complex)
	acmYY = zeros((nrcu/2,nrcu/2),dtype = complex)
	for rcui in range(0,nrcu,2):
		for rcuj in range(0,nrcu,2):
			acmXX[rcui/2,rcuj/2] = dat[rcui,rcuj]
			acmYY[rcui/2,rcuj/2] = dat[rcui+1,rcuj+1]

	if pol == 0: acm = acmXX
	if pol == 1: acm = acmYY  
	if pol == 2: acm = acmXX + acmYY	# Stokes I
	if pol == 3: acm = acmYY - acmXX	# Stokes Q

	f = freliste[fre] * 1000000
	k = 2.0*pi / c  * f
	wx = exp((dot((mat(xpos)).T,(mat(ll))))*-(1j)*k)
	wy = exp((dot((mat(ypos)).T,(mat(mm))))*-(1j)*k)

	skymap = []

	for ldx in range(0,lenght_l,1):
		for mdx in range(0,lenght_m,1):
			r = (ll[ldx]**2 + mm[mdx]**2)
			if r >= 1:
				skymap.append("zero")
			else:
				weight = array(wx[:,mdx])*array(wy[:,ldx])
				skymap.append((dot(dot((weight.conjugate()).T,array(acm)),weight)).real) 

	minimum = min(skymap)
	for pixel in range(0, len(skymap),1):
		if skymap[pixel] == "zero":
			skymap[pixel] = minimum

	skymap = reshape((skymap),(lenght_l,lenght_m))

	return skymap

# for the source search part:
def SKYMAP2(dat, pol):

	acmXX = zeros((nrcu/2,nrcu/2),dtype = complex)
	acmYY = zeros((nrcu/2,nrcu/2),dtype = complex)
	for rcui in range(0,nrcu,2):
		for rcuj in range(0,nrcu,2):
			acmXX[rcui/2,rcuj/2] = dat[rcui,rcuj]
			acmYY[rcui/2,rcuj/2] = dat[rcui+1,rcuj+1]

	if pol == 0: acm = acmXX
	if pol == 1: acm = acmYY  
	if pol == 2: acm = acmXX + acmYY

	f = freliste[fre] * 1000000
	k = 2.0*pi / c  * f
	wx = exp((dot((mat(xpos)).T,(mat(ll))))*-(1j)*k)
	wy = exp((dot((mat(ypos)).T,(mat(mm))))*-(1j)*k)

	skymap = []

	########### acm gesamt ueberhaupt noetig an dieser Stelle ?????????????????

	for ldx in range(0,lenght_l,1):
		for mdx in range(0,lenght_m,1):
			r = (ll[ldx]**2+mm[mdx]**2)
			if r <= 1:
			  weight = array(wx[:,mdx])*array(wy[:,ldx])
			  skymap.append([1.0, float((dot(dot((weight.conjugate()).T,array(acmXX)),weight)).real), float((dot(dot((weight.conjugate()).T,array(acmYY)),weight)).real), -ll[mdx], -mm[ldx] ])
	#		  skymap.append([float((dot(dot((weight.conjugate()).T,array(acm)),weight)).real), float((dot(dot((weight.conjugate()).T,array(acmXX)),weight)).real), float((dot(dot((weight.conjugate()).T,array(acmYY)),weight)).real), -ll[mdx], -mm[ldx] ])

	return skymap	


#---------------Difference between model and data----------------

def diff_model_real(dat):
	diff = zeros((nrcu,nrcu),dtype = complex)
	for rcui in range(0, nrcu, 2):
		for rcuj in range(0, nrcu,2):
			if baselines[rcui,rcuj] <= max_baselenght:
			#if rcui == rcuj:			
				diff[rcui,rcuj] = dat[rcui,rcuj]
				diff[rcui+1,rcuj+1] = dat[rcui+1,rcuj+1]
		#	elif baselines[rcui,rcuj] <= max_baselenght:
		#		diff[rcui,rcuj] = data[rcui,rcuj,fre]
		#		diff[rcui+1,rcuj+1] = data[rcui+1,rcuj+1,fre]
			else:
				if vis_model[rcui,rcuj] != 0:
					diff[rcui,rcuj] = dat[rcui,rcuj]/vis_model[rcui,rcuj]
				if vis_model[rcui+1,rcuj+1] != 0:
					diff[rcui+1,rcuj+1] = dat[rcui+1,rcuj+1]/vis_model[rcui+1,rcuj+1]
				#diff[rcui+1,rcuj] = data[rcui+1,rcuj,fre]
				#diff[rcui,rcuj+1] = data[rcui,rcuj+1,fre]
	return diff

#-------------Phase Calibration----------------------------------
# see also AIPS GCALC for phase calibration

def CAL_PHASE():

	print "\n- Start Phase Calibration -" 
	Gstart = ones((nrcu), dtype= complex)
	for i in range(0,20,1):
		gn = zeros((nrcu), dtype= complex)
		for rcui in range(0,nrcu,2):
			for rcuj in range(0,nrcu,2):
				z = Gstart[rcuj].conjugate()*diff[rcui,rcuj]
				gn[rcui] += z
				gn[rcuj] += z.conjugate()
				z = Gstart[rcuj+1].conjugate()*diff[rcui+1,rcuj+1]
				gn[rcui+1] += z
				gn[rcuj+1] += z.conjugate()
		G_ph = ones((nrcu), dtype= complex)
		for rcui in range(0,nrcu,1):
			if gn[rcui] != 0:
				gn[rcui] = 1/gn[rcui]	
				G_ph[rcui] = angle(gn[rcui])
				Gstart[rcui] = (math.cos(G_ph[rcui])+1j*math.sin(G_ph[rcui]))
		#print Gstart[14], Gstart[15]
	return Gstart


#-------------Phase + Amplitude Calibration----------------------
# see also AIPS GCALC Amp+Phase calibration

def CAL_AMP_PHASE():
	print "\n- Start Amp-Phase Calibration -\n"
	Gstart = ones((nrcu), dtype= complex)
	for i in range(1,20,1):
		W = 0.25
		Gstartn = zeros((nrcu), dtype= complex)
		for rcui in range(0,nrcu,2):

	###   kalibration nur fuer RCU, die nicht geflaggt wurden (ueber weighting...)!!!!!!!!!!!!!!!!

	### XX und YY parallel kalibrieren, um Zeit zu sparen

			gn = 0.0+1j*0.0
			QQ = 0.0
			gn2 = 0.0+1j*0.0
			QQ2 = 0.0
			for rcuj in range(0,nrcu,2):

				QQ += Gstart[rcui]*Gstart[rcui].conjugate()
				gn += Gstart[rcui]*diff[rcui,rcuj]
				QQ += Gstart[rcuj]*Gstart[rcuj].conjugate()
				gn += (Gstart[rcuj].conjugate()*diff[rcuj,rcui])

				QQ2 += Gstart[rcui+1]*Gstart[rcui+1].conjugate()
				gn2 += Gstart[rcui+1]*diff[rcui+1,rcuj+1]
				QQ2 += Gstart[rcuj+1]*Gstart[rcuj+1].conjugate()
				gn2 += (Gstart[rcuj+1].conjugate()*diff[rcuj+1,rcui+1])
		
			Gstartn[rcui] = Gstart[rcui] + W*(gn/QQ - Gstart[rcui])
			Gstartn[rcui+1] = Gstart[rcui+1] + W*(gn2/QQ2 - Gstart[rcui+1])
		Gstart = Gstartn
		#print Gstart[14], Gstart[15]
	return Gstart

#-------------Apply Gains to the data----------------------------

def Gain(Gain,dat):
	for rcui in range(0,nrcu,1):
		for rcuj in range(0,nrcu,1):
			#if (abs(Gain[rcui]) != 0.0 and abs(Gain[rcuj]) != 0.0):
			dat[rcui,rcuj] *= Gain[rcui]*Gain[rcuj].conjugate()#/(abs(Gain[rcui])*abs(Gain[rcuj]))
	return dat

#-------------Search for Sources in Skymap-----------------------

def RFI_search(skymap,pol):

	#print "- start Source and RFI search -"

	if pol == 0: l = 1
	if pol == 1: l = 2

	RFIso = []
	
	skymaps = sorted(skymap, key=lambda mapi:mapi[l], reverse=True)

	RFIso.append(skymaps[0])	 #####   hier auch l ???????

	for peak in range(1, len(skymaps), 1):
		inBeam = False
		for so in range(0, len(RFIso), 1):
			if (float(RFIso[so][3])-beam_deg[fre] < float(skymaps[peak][3]) < float(RFIso[so][3])+beam_deg[fre]) and (float(RFIso[so][4])-beam_deg[fre] < float(skymaps[peak][4]) < float(RFIso[so][4])+beam_deg[fre]):
				inBeam = True
			if inBeam:
				break
		if not inBeam:
			RFIso.append(skymaps[peak])
		if len(RFIso) >= 10:
			break
	"""
	maxv = 0.8
	i = 0
	if RFIso[-1][0] < maxv*RFIso[0][0]:
		while i <= 7:
			i = 0
			while RFIso[i][0] >= maxv*RFIso[0][0]:
				i += 1
				if i == len(RFIso):
					break
			maxv -= 0.05

		del RFIso[i:]

	print "Anzahl Quellen: ",len(RFIso)
	"""
	return RFIso

#-------------Subtraction of the RFI-----------------------------
#bis jetzt nur RFI im Norden !!!

def sub_RFI(dat, pol):

	skymap = SKYMAP2(dat, pol)
	print "\n- Start Subtraction of RFI -\n"
	if pol != 1: sourcelisteX = RFI_search(skymap,0)
	if pol != 0: sourcelisteY = RFI_search(skymap,1)

	if pol != 1:
		i = 0
		sourcelisteX = sorted(sourcelisteX, key=lambda mapi:mapi[4], reverse=True)
		while sourcelisteX[i][4] >= 0.8: i += 1
		#print i
		del sourcelisteX[i:]
	#	amplX2 = 0.0
	#	for s in range(0, len(sourcelisteX),1):
	#		amplX2 += sourcelisteX[s][1] 
		#amplX = sum((sourcelisteX[:][1]))
		vis_modelX = vis_modelc0(sourcelisteX, 0)
		vis_modelX /= 9216*2#190000#amplX2

	if pol != 0:
		i = 0
		sourcelisteY = sorted(sourcelisteY, key=lambda mapi:mapi[4], reverse=True)
		while sourcelisteY[i][4] >= 0.8: i += 1
		#print i
		del sourcelisteY[i:]
	#	amplY2 = 0.0
		#if len >= 0
		#for s in range(0, len(sourcelisteY),1):
		#	amplY2 += sourcelisteY[s][2]
		#amplY = sum((sourcelisteY[:][2]))
		vis_modelY = vis_modelc0(sourcelisteY, 1)
		vis_modelY /= 9216*2#190000#amplY2

	if pol == 2: vis_model = vis_modelX + vis_modelY
	if pol == 0: vis_model = vis_modelX
	if pol == 1: vis_model = vis_modelY 

	dat -= vis_model

	return dat


#-------------Calibration routine--------------------------------

fre = channel
f = freliste[fre] * 1000000

data /= (250000*1024)
datas = data[:,:,channel]*1

delay = False
delay = False
if delay:
	datasets = '/.aux_mnt/pc20021a/TBB_2208_22/delays10refs.out'
	dataset = open(datasets)
	delay = pickle.load(dataset)

#	da = zeros((len(freliste)),dtype = complex)
#	for fre in range(0,len(freliste),1):
		#f = freliste[fre] * 1000000
#		da[fre] = (-2*1j*pi*f*delay[1]*10**(-9))

	for rcu1 in range(0,nrcu,1):
		for rcu2 in range(0,nrcu,1):
			if delay[rcu1] != 0 and delay[rcu2] != 0:
				datas[rcu1,rcu2] *= exp(2*1j*pi*f*(delay[rcu1]-delay[rcu2])*10**(-9))
			else:
				datas[rcu1,rcu2] = 0.0


vis_flag = ones((nrcu,nrcu))
for rcu1 in range(0,nrcu,1):
	vis_flag[rcu1,rcu1] = 0.0

pol = 2

flag_max = 2.5
flag_min = 0.3

weighting = ones((nrcu,nrcu))

cal_phase = True
cal_amp_phase = True
use_RFI = False#True
flagging = False
autoc = True
RFIS = True
NRFI = False
#NRFI = True		#use northern RFI for the calibration

max_baselenght = 0.0
maxval = 0.8

Gainf = zeros((len(freliste),nrcu), dtype = complex)

if options.calibrate:
	print "CALIBRATION STRATEGY:"
	print "\nPhase     ",cal_phase
	print "Amp+Phase ", cal_amp_phase
	print "RFI       ",use_RFI,"\n"
else:
	print "CHANNEL: %i  -  FREQUENCY: %.2f MHz" % (fre, f/10**6)
	print "\nNo Calibration\n"


beam = 1.22* (3*10**2/freliste)/40/0.017
beam_deg = beam/90

Ateam = []
sou = (["CygA","CasA"])#,"TauA","VirA","HerA", "HydA","PerA"])
for so in sou:
	l,m,I = source_l_m(so)
	if l != 'NaN':
		Ateam.append((I,I,I,l,m))
                if options.calibrate:
			print "Use %s for the Calibration" % (so)

sourceliste = Ateam
nsources = len(Ateam)
#amp = zeros((len(freliste),len(Ateam)+1))

if options.calibrate_all_frequencies:	# Calibration over all frequencies
	start_chan = 0
	end_chan = len(freliste)
else:		# Calibration on selected channel/frequency
	start_chan = channel
	end_chan = channel+1

if options.calibrate:
   for fre in range(start_chan, end_chan, 1):

	gain = ones((nrcu), dtype = complex)

	channel = fre
	f = freliste[fre] * 1000000
	print "\nCHANNEL: %i  -  FREQUENCY: %.2f MHz" % (fre, f/10**6)

	#datas = data[:,:,fre]*1#/(250000*1024)

#	if NRFI:
#		datas = sub_RFI(datas, pol)

	if flagging:
		datas, vis_flag = FLAGGING(datas, vis_flag)

	#datas *= vis_flag

	Gaing = gain+0.0*1j
	if autoc:
		datas, gain, vis_flag = gain_cal_autocorr(datas, gain, vis_flag)
		Gaing = gain+0.0*1j

		for rcu1 in range(0,nrcu,1):
			datas[rcu1,rcu1] = 0.0

		nX = 0.0
		nY = 0.0

		#for rcu1 in range(0,nrcu,2):
		#	for rcu2 in range(0,nrcu,2):
		#		if datas[rcu1,rcu2] != 0:
		#			nX += 1
		#		if datas[rcu1+1,rcu2+1] != 0:
		#			nY += 1

		#print "X:", nX
		#print "Y:", nY

	#	for rcu1 in range(0,nrcu,2):
			#for rcu2 in range(0,nrcu,2):
			#	datas[rcu1,rcu2] /= i
			#	datas[rcu1+1,rcu2+1] /= j
	#		Gaing[rcu1] /= sqrt(i/j)
			#Gaing[rcu1+1] /= sqrt(j)

		datas = Gain(Gaing,datas)

	#datas = sub_RFI(datas, pol)
	if cal_phase:

		#vielleicht in den Modelamplituden die Beam correctur mit reinnehmen (vielleicht schon bei RFIserach ?????????!!!!!!

		#vis_model = vis_modelc0(sourceliste, pol)

		if NRFI: 
			skymap = SKYMAP2(datas,pol)
			if pol != 1: 
				sourcelisteX = RFI_search(skymap,0)
				vis_modelX = vis_modelc0(sourcelisteX, 0)
			if pol != 0: 
				sourcelisteY = RFI_search(skymap,1)
				vis_modelY = vis_modelc0(sourcelisteY, 1)

			if pol == 2: vis_model = vis_modelX + vis_modelY
			if pol == 0: vis_model = vis_modelX
			if pol == 1: vis_model = vis_modelY
		else:
			sourceliste = Ateam
			vis_model = vis_modelc0(sourceliste, 2)

		diff = diff_model_real(datas)

		sum2 = 0.0
		sumY2 = 0.0

		#flat = diff.ravel()
		iX = 0
		iY = 0
		for rcu1 in range(0,nrcu,2):
			for rcu2 in range(0,nrcu,2):
				if abs(diff[rcu1,rcu2]) != 0:		
					sum2 += (abs(diff[rcu1,rcu2]))**2
					iX += 1
				if abs(diff[rcu1+1,rcu2+1]) != 0:
					sumY2 += (abs(diff[rcu1+1,rcu2+1]))**2
					iY += 1	
		if iX == 0: iX = 1
		if iY == 0: iY = 1
		#print iX, iY, sum2, sumY2	
		RMS = sqrt(sum2/iX)
		RMSY = sqrt(sumY2/iY)
		#print "RMS_X: ", RMS
		#print "RMS_Y: ", RMSY

		
		for rcu1 in range(0,nrcu,2):
			for rcu2 in range(0,nrcu,2):
				if abs(diff[rcu1,rcu2]) != 0:
					sigma = RMS/(abs(diff[rcu1,rcu2]) + RMS*0.05)
					diff[rcu1,rcu2] *= (sigma)
				if abs(diff[rcu1+1,rcu2+1]) != 0:
					sigmaY = RMSY/(abs(diff[rcu1+1,rcu2+1]) + RMSY*0.05)
					diff[rcu1+1,rcu2+1] *= (sigmaY)


		G_phase = CAL_PHASE()
		datas = Gain(G_phase,datas)

	#	datas = sub_RFI(datas, pol)
 	#	datas *= vis_flag

		Gaing *= G_phase

		#acmXX = 0
		#acmYY = 0
		#for rcui in range(0,nrcu,2):
		#	for rcuj in range(0,nrcu,2):
		#		acmXX += datas[rcui,rcuj]
		#		acmYY += datas[rcui+1,rcuj+1]

		#ratio = float(acmYY/acmXX)
		#print "Ratio: ",ratio

		#print "data3: ",datas[1,3]

		#datas = sub_RFI(datas, pol)

		if flagging:
			datas, vis_flag = FLAGGING(datas, vis_flag)

	if cal_amp_phase:

		use_RFI = True

		#datas = sub_RFI(datas, pol)
 		#datas *= vis_flag

	#	if NRFI: 
		skymap = SKYMAP2(datas,pol)
		if pol != 1: 
			sourcelisteX = RFI_search(skymap,0)
			vis_modelX = vis_modelc0(sourcelisteX, 0)
		if pol != 0: 
			sourcelisteY = RFI_search(skymap,1)
			vis_modelY = vis_modelc0(sourcelisteY, 1)

		if pol == 2: vis_model = vis_modelX + vis_modelY
		if pol == 0: vis_model = vis_modelX
		if pol == 1: vis_model = vis_modelY
	#	else:
	#		vis_model = vis_modelc0(sourceliste, 2)
	
		diff = diff_model_real(datas)

		sum2 = 0.0
		sumY2 = 0.0
		iX = 0
		iY = 0
		for rcu1 in range(0,nrcu,2):
			for rcu2 in range(0,nrcu,2):
				if abs(diff[rcu1,rcu2]) != 0:		
					sum2 += (abs(diff[rcu1,rcu2]))**2
					iX += 1
				if abs(diff[rcu1+1,rcu2+1]) != 0:
					sumY2 += (abs(diff[rcu1+1,rcu2+1]))**2
					iY += 1	

		#print iX, iY, sum2, sumY2
		if iX == 0: iX = 1
		if iY == 0: iY = 1			
		RMS = sqrt(sum2/iX)
		RMSY = sqrt(sumY2/iY)
		#print "RMS_X: ", RMS
		#print "RMS_Y: ", RMSY


		for rcu1 in range(0,nrcu,2):
			for rcu2 in range(0,nrcu,2):
				if abs(datas[rcu1,rcu2]) > 0.0:
					diff[rcu1,rcu2] *= log(RMS/(abs(diff[rcu1,rcu2])))
				elif abs(datas[rcu1,rcu2]) <= 0:
					diff[rcu1,rcu2] = 0.0
				if abs(datas[rcu1+1,rcu2+1]) > 0.0:
					diff[rcu1+1,rcu2+1] *= log(RMSY/(abs(diff[rcu1+1,rcu2+1])))
				elif abs(datas[rcu1+1,rcu2+1]) <= 0:
					diff[rcu1+1,rcu2+1] = 0.0		
			
		G_amp_ph = CAL_AMP_PHASE()

		datas = Gain(G_amp_ph,datas)

		#vis_model = vis_modelc(sourceliste)
	#	if NRFI:
	#		datas = sub_RFI(datas, pol)
		if flagging:
			datas, vis_flag = FLAGGING(datas, vis_flag)
	#	datas *= vis_flag

		Gaing *= G_amp_ph

		#if flagging:
		#	datas, vis_flag = FLAGGING(datas, vis_flag)

#	Gaing = gain*G_amp_ph*G_phase

	#print "data: ",datas[1,3]
	#print "Gain: ", math.atan2(Gaing[1].imag,Gaing[1].real)
	#print "Gain: ", math.atan2(Gaing[11].imag,Gaing[11].real)
	#print "Gain: ", math.atan2(Gaing[15].imag,Gaing[15].real)

#	if flagging:
#		datas = FLAGGING(datas)

	Gainf[fre] = Gaing
#	print "Gain: ", Gainf[fre]
	"""
	if fre == start_chan:
		skymaptot = SKYMAP(datas)
	else:
		skymap = SKYMAP(datas)
		skymaptot += skymap

	for sou in range(0,len(Ateam),1):
		l = Ateam[sou][0]
		m = Ateam[sou][1]

		if l < 0: lp = int((abs(l)+1)*100)
		else: lp = int(100-l*100)
		if m < 0: mp = int((abs(m)+1)*100)
		else: mp = int(100-(m)*100)

		amp[fre,sou] = round(skymap[mp][lp],2)

	#for the sun:

	if Elevation_sun >= 0:
		l = math.cos((90-Azimuth_sun)*pi/180)*math.cos(Elevation_sun*pi/180)
		m = math.sin((90-Azimuth_sun)*pi/180)*math.cos(Elevation_sun*pi/180)

		if l < 0: lp = int((abs(l)+1)*100)
		else: lp = int(100-l*100)
		if m < 0: mp = int((abs(m)+1)*100)
		else: mp = int(100-(m)*100)

		amp[fre,len(Ateam)] = round(skymap[mp][lp],2)
	"""

#outfile = ("Gainfile_XY_512.out")
#outfile1 = ("Amp_cas_cyg_new.out")
#of = open(outfile,'w')
#of1 = open(outfile1,'w')
#pickle.dump(Gainf,of)
#pickle.dump(amp,of1)
#exit()

datas *= 100

#i = 0
#j = 0

#for rcu1 in range(0,nrcu,2):
#	for rcu2 in range(0,nrcu,2):
#		if datas[rcu1,rcu2] != 0:
#			i += 1
#		if datas[rcu1+1,rcu2+1] != 0:
#			j += 1

#print "X:", i
#print "Y:", j


#datas = vis_model


#acmXX = 0
#acmYY = 0

#for rcu1 in range(0,nrcu,1):
#	datas[rcu1,rcu1] = 0.0

#for rcui in range(0,nrcu,2):
#	for rcuj in range(0,nrcu,2):
#		acmXX += datas[rcui,rcuj]
#		acmYY += datas[rcui+1,rcuj+1]

#ratio = float(acmYY/acmXX)
#print "Ratio: ",ratio


print "- Start Imaging -"

#-------------calculating the skymap-----------------------------

f = freliste[channel] * 1000000
frequency = freliste[channel]
k = 2.0*pi / c  * f

#minm = skymap.min()
#if minm < 0:
#	skymap += 1.1*abs(minm)

# Use Amplitude of CasA or CygA for scaling the Skymap
scale_amp = False
if scale_amp:
	skymap = SKYMAP(datas,2)
	source = 0		# 0:CygA; 1:CasA

	l = Ateam[source][3]
	m = Ateam[source][4]

	if l < 0:
		lp = int((abs(l)+1)*100)
	else:
		lp = int(100-l*100)

	if m < 0:
		mp = int((abs(m)+1)*100)
	else:
		mp = int(100-(m)*100)

	amp = round(skymap[mp][lp],2)

	ampr = Ateam[source][0]

	skymap *= ampr/amp
	print "amp:",ampr, amp

# Calculating the Skymap in Galactic Coordinates
galactic_coord = False
if galactic_coord:
	Azimut = zeros((200,200))
	Elev = zeros((200,200))

	for l1 in range(0,len(ll),1):
		for m1 in range(0,len(mm),1):
			l = ll[l1]
			m = mm[m1]
			if sqrt(l**2+m**2) <= 1:
				if l != 0:
					A = 90 - arctan(m/l)*180/pi
					if sin((90.0-A)*pi/180) != 0:
						h = arccos(m/sin((90.0-A)*pi/180))*180.0/pi
						Azimut[l1,m1] = A
						Elev[l1,m1] = h

	print "AZ:",Azimut[lp,mp], "Elev:",Elev[lp,mp]
	rect = zeros((200,200))
	decl = zeros((200,200))
	gb = zeros((200,200))
	gl = zeros((200,200))


	L = mod(280.460 + 0.9856474*n, 360)     #mean ecliptic longitude of the sun; aberration is included
	g = mod(357.528 + 0.9856003*n, 360)   #mean anormaly
	delta = mod((L + 1.915*math.sin(pi*g/180)+0.02*math.sin(2*pi*g/180)), 360)   #ecliptic longitude of the sun
	ecliptic = (23.439 - 0.0000004*n)*pi/180



	for l in range(0,len(ll),1):
		for m in range(0,len(mm),1):
			d = arcsin(-cos(Elev[l,m]*pi/180)*cos(Azimut[l,m]*pi/180)*cos(latitude*pi/180)+sin(Elev[l,m]*pi/180)*sin(latitude*pi/180))*180/pi
			if cos(d*pi/180) != 0:
				t = arcsin(cos(Elev[l,m]*pi/180)*sin(Azimut[l,m]*pi/180)/cos(d*pi/180))*180/pi
			#	if cos(pi/180*delta) < 0:
			#	   t += 180
				r = mod(vernalequinox - t,360)
				#if 0  < r < 180 :
				#	r += 180

				rect[l,m] = r
				decl[l,m] = d

				b = arcsin(sin(d*pi/180)*sin(27.4*pi/180)+cos(d*pi/180)*cos(27.4*pi/180)*cos((192.25-r)*pi/180))*180/pi
				g1 = 303-arctan(sin((192.25-r)*pi/180)/(cos((192.25-r)*pi/180)*sin(27.4*pi/180)-tan(d*pi/180)*cos(27.4*pi/180)))*180/pi
			#	if g1 <= 0:
			#		print "kleiner"
			#		g1 += 360
			#	if g1 >= 360:
			#		g1 -= 360
				if sin((192.25-r)*pi/180)/(cos((192.25-r)*pi/180)*sin(27.4*pi/180)-tan(d*pi/180)*cos(27.4*pi/180))<0:
					g1 -= 180
				if 290 > g1 > 120 and r < 200: #240 und 290
					g1 -= 180
			#	if 0  < r < 180 :
			#		g1 -= 180

				gb[l,m] = (90+b)
				gl[l,m] = mod(g1,360)

	print "rec:",rect[lp,mp],"dec:",decl[lp,mp]
	print gb[lp,mp], gl[lp,mp]
	print "fr: ", vernalequinox

	mapn = zeros((180,360))

	for l in range(0,len(ll),1):
		for m in range(0,len(mm),1):
			d = int(gb[l,m])
			r = int(gl[l,m])
			mapn[d,r] = skymap[m,l]
	"""
	for l in range(1,len(mapn)-1,1):
		for m in range(1,len(mapn[0])-1,1):
			k = 0
			o = 0.0
			for l1 in range(l-1,l+2,1):
				for m1 in range(m-1,m+2,1):
					if mapn[l1,m1] == 0:
						k += 1
					else:
						o += mapn[l1,m1]
			if k != 9:
				o = float(o)/(9-k)
			if k <= 2 and mapn[l,m] == 0:
				mapn[l,m] = o
			elif k <= 3:
				if o/2 < mapn[l,m] < 2*o :
					a = 1
				else:
	"""
	#print mapn[int(gb[l,m]),int(gl[l,m])]
	outfile = ("sm_mw_324.out")
	of = open(outfile,'w')
	pickle.dump(mapn,of)
		
	axes(projection = 'mollweide')	
	imshow(mapn, interpolation='bilinear',extent=(pi,-pi,pi/2,-pi/2))
	#plot(gb[lp,mp], gl[lp,mp],'ko')
	#extent=(0,24,90,-90)
	#extent=(180,-180,90,-90)
	colorbar()
	show()
	exit()


#datas = sub_RFI(datas, pol)
#skymap = SKYMAP(datas,pol)

"""
skymap = SKYMAP(datas,0)
figure()
imshow((skymap), aspect='auto', interpolation='bilinear',extent=(1,-1,-1,1))
xlabel('E'+' '*35+'l'+' '*35+'W', size=12)
ylabel('S'+' '*35+'m'+' '*35+'N', size=12)
title('XX        '+str(round(frequency,2))+' MHz',size=12)
colorbar()
#savefig("picX.png")


figure()
skymap = SKYMAP(datas,1)
imshow((skymap), aspect='auto', interpolation='bilinear',extent=(1,-1,-1,1))
xlabel('E'+' '*35+'l'+' '*35+'W', size=12)
ylabel('S'+' '*35+'m'+' '*35+'N', size=12)
title('YY        '+str(round(frequency,2))+' MHz',size=12)
colorbar()
"""

#show()
#exit()
#savefig("picY.png")
#exit()


figure()
skymap = SKYMAP(datas,2)


#Creating the skymap

if savefits:
	skymap = reshape((skymap),(1,1,lenght_l,lenght_m))
else:
	skymap = reshape((skymap),(lenght_l,lenght_m))




"""
skymap2 = []
ldx = 0

while ldx <= lenght_l-1:
	mdx = 0
	while mdx <= lenght_m-1:
		l1 = l[ldx]
		m1 = m[mdx]
		r = (l1*l1+m1*m1)
		if r >= 1:
		   skymap2.append("zero")
		else:
		  weight = array(wx[:,mdx])*array(wy[:,ldx])
		  skymap2.append((dot(dot((weight.conjugate()).T,array(acm2)),weight)).real) 
		mdx += 1
	ldx += 1

minimum = min(skymap2)

p = 0
for pixel in skymap2:
  if pixel == "zero":
    skymap2[p] = minimum
  p += 1


if savefits:
	skymap2 = reshape((skymap2),(1,1,lenght_l,lenght_m))
else:
	skymap2 = reshape((skymap2),(lenght_l,lenght_m))


skymap = skymap2/skymap
"""

#Creating the skymap

if savefits:
	skymap = reshape((skymap),(1,1,lenght_l,lenght_m))
else:
	#skymap = reshape((skymap),(lenght_l,lenght_m))

	# for saving the calculated skymaps values:
#	outfile = ("Maps_cal/data/sm_"+str(channel)+".out")
#	of = open(outfile,'w')
#	pickle.dump(skymap,of)

	fre = str(round(frequency,2))

	xlabel('E'+' '*35+'l'+' '*35+'W', size=12)
	ylabel('S'+' '*35+'m'+' '*35+'N', size=12)
	title(fre+' MHz',size=12)

	imshow((skymap), aspect='auto', interpolation='bilinear',extent=(1,-1,-1,1))
	colorbar()



#Conversion into direction cosines and plotting 
#if Elevation_sun >= 0:
#    sun_l = math.cos((90-Azimuth_sun)*pi/180)*math.cos(Elevation_sun*pi/180)
#    sun_m = math.sin((90-Azimuth_sun)*pi/180)*math.cos(Elevation_sun*pi/180)
#    plot(sun_l, sun_m,'k+')
#    print "sun: ",sun_l, sun_m
#    text(sun_l+0.05, sun_m+0.05, 'Sun')


# [source, ra h, ra min, ra sec, dec h, dec min, dec sec]
sourcelist = array([ ("CasA", 23.0, 23.0, 27.9, 58.0, 48.0, 42.0),
		("CygA", 19.0, 59.0, 28.3, 40.0, 44.0, 2.0), 
		("TauA", 5.0, 34.0, 32.0, 22.0, 0.0, 52.0), 
		("VirA", 12.0, 30.0, 49.4, 12.0, 23.0, 28.0), 
		("HerA", 16.0, 51.0, 37.73, 4.0, 58.0, 33.88), 
		("HydA", 9.0, 18.0, 5.7, -12.0, -5.0, -44.0), 
		("PerA", 3.0, 19.0, 48.1, 41.0, 30.0, 42.0), 
		("SgrA", 17.0, 45.0, 40.0, -29.0, 0.0, -28.0),
		("Sun",0,0,0,0,0,0) ])

print "\n Source | Intensity |   l   |   m  "
print "-" *len(str(" Source - Intensity -   l   -   m   "))

for source in range(0, len(sourcelist), 1):
	
	if str(sourcelist[source][0]) == "Sun":
		L = mod(280.460 + 0.9856474*n, 360)     #mean ecliptic longitude of the sun; aberration is included
		g = mod(357.528 + 0.9856003*n, 360)   #mean anormaly
		delta = mod((L + 1.915*math.sin(pi*g/180)+0.02*math.sin(2*pi*g/180)), 360)   #ecliptic longitude of the sun
		ecliptic = (23.439 - 0.0000004*n)*pi/180

		rascension = math.atan(math.cos(ecliptic)*math.sin(pi/180*delta)/cos(pi/180*delta))*180/pi 
		if cos(pi/180*delta) < 0:
		    rascension += 180
		declination = math.asin(math.sin(ecliptic)*sin(pi/180*delta)) 

	else:

		rascension = (float(sourcelist[source][1])+float(sourcelist[source][2])/60+float(sourcelist[source][3])/3060)*15
		declination = (float(sourcelist[source][4])+float(sourcelist[source][5])/60+float(sourcelist[source][6])/3600)*pi/180

	hourangle = (vernalequinox - rascension)*pi/180

	Elevation = math.asin(math.cos(declination)*math.cos(hourangle)*math.cos(latitude*pi/180)+sin(declination)*sin(latitude*pi/180))*180/pi
	Azimuth = math.atan(math.sin(hourangle)/(math.sin(latitude*pi/180)*math.cos(hourangle)-math.cos(latitude*pi/180)*math.tan(declination)))*180/pi+180
	if (math.cos(hourangle)*sin(latitude*pi/180)-math.tan(declination)*cos(latitude*pi/180))<0: 
	    Azimuth += 180
	if Azimuth < 0:
	   Azimuth += 360
	Azimuth = mod(Azimuth, 360)

	#Conversion into direction cosines and plotting 

	if Elevation >= 0:
		l = (math.cos((90-Azimuth)*pi/180))*(math.cos((Elevation)*pi/180))
		m = (math.sin((90-Azimuth)*pi/180))*(math.cos((Elevation)*pi/180))
		plot(l, m,'k+')
		text(l-0.05, m-0.05, str(sourcelist[source][0]))

		if l < 0:
			lp = int((abs(l)+1)*100)
		else:
			lp = int(100-l*100)

		if m < 0:
			mp = int((abs(m)+1)*100)
		else:
			mp = int(100-(m)*100)

		inte = 0
		lis = []

		for mi in range(mp-2, mp+2,1):
			for li in range(lp-2, lp+2,1):
				inte += skymap[mi][li]
				lis.append(skymap[mi][li])

		print "%7s | %9.2f | %5.2f | %5.2f" % (sourcelist[source][0], skymap[mp][lp], l, m)

RFIS = False
if RFIS:
	for source in range(0, len(sourceliste), 1):
		#lp = int(sourcelist[source][3])
		#mp = int(sourcelist[source][4])
		l = sourceliste[source][3]
		m = sourceliste[source][4]
		#print source,l,m
		plot(l, m,'k+')
		text(l-0.05, m-0.05, source)

		#print "RFI",source, ":", round(skymap[mp][lp],2), ";" ,round(l,2),";",round(m,2)


#-------------save image as fits file----------------------------
# bisher noch nicht fuer Galaktische Koordinaten

if savefits:	

	outfile = 'outall.fits'
	hdu = pyfits.PrimaryHDU(skymap)

	hdulist = pyfits.HDUList([hdu])
	prihdr = hdulist[0].header

#	prihdr.update('NAXIS',3)
	prihdr.update('NAXIS1',lenght_l)
	prihdr.update('NAXIS2',lenght_m)
	prihdr.update('NAXIS3',1)
	prihdr.update('NAXIS4',1)
	prihdr.update('EQUINOX',2000.0)	
	#prihdr.update('LONPOLE',0.)
	#prihdr.update('LATPOLE',90.)
	prihdr.update('OBSLON', longitude*0.017)
	prihdr.update('OBSLAT', latitude*0.017)
#	prihdr.update('OBSTIME', timetuple)
	prihdr.update('CTYPE1','RA---TAN')  
	prihdr.update('CRVAL1',GWhourangle)  #value of the axis at the reference point "CRPIX"
	prihdr.update('CDELT1',-(180.0/lenght_l))
	prihdr.update('CRPIX1',float(lenght_l/2)) 
	#prihdr.update('CROTA1',0.)          #rotation, just leave it at 0.
	prihdr.update('CUNIT1','deg')       #the unit in which "CRVAL" and "CDELT" are given
	prihdr.update('CTYPE2','DEC--TAN')
	prihdr.update('CRVAL2',latitude)
	prihdr.update('CDELT2',180.0/lenght_l)
	#prihdr.update('CROTA2',0.)
	prihdr.update('CRPIX2',float(lenght_l)/2)
	prihdr.update('CUNIT2','deg     ')
	prihdr.update('CTYPE3','FREQ    ')
	prihdr.update('CRVAL3',f)
	prihdr.update('CDELT3',frequency)
	prihdr.update('CUNIT3','HZ      ')

	prihdr.update('CUNIT4','        ')

	if os.path.isfile(outfile):
		os.remove(outfile)

	hdulist.writeto(outfile)

	exit()

t2 = time.time()
print "\nTime: %.2f sec" % (t2-t1)
#Save picture or show
#savefig("pic.png")


show()
exit()






