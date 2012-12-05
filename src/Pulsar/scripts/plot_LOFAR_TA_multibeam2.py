#!/usr/bin/env python
#
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import matplotlib.pylab as plb
import matplotlib.ticker as ticker
import numpy as np
import sys
from operator import indexOf
from optparse import OptionParser

RAs = []
DECs = []
Chisqs = []
RAD2DEG = 1./0.0174532925

if __name__ == '__main__':

	parser = OptionParser()
	parser.add_option("-p", "--parset", action="store", type="string",
			  dest="parset",   help="The name of the observation parset file")
	parser.add_option("-c", "--chi", action="store", type="string",
			  dest="chisq",   help="Name of the observation OBSID_chi-squared.txt file")
	parser.add_option("-t", "--target",action="store",type="string", dest="target",
        	          help="Give the name of the target (for title of plot)")
	parser.add_option("-o", "--out_logscale",action="store",type="string", dest="outlogscale",
        	          help="Give output file name of the log scale plot")
	parser.add_option("-n", "--out_linscale",action="store",type="string", dest="outlinscale",
        	          help="Give output file name of the linear scale plot")
	parser.add_option("-s", "--sap",action="store",type="string", dest="sap",
        	          help="Give number of SAP (subarray pointing)")
		  
	(options, args) = parser.parse_args()
	if len(sys.argv) < 2:
		parser.print_help()
		sys.exit(0)

	#Grab the RAs and DECs ;  note these are NOT in numerical order of beam number
	#Observation.Beam[0].TiedArrayBeam[139].angle1 = 0.00755693767342
	# also checking for IS beams
	nisbeams = 0
	file = open(options.parset,"r")
	for line in file.readlines():
		if line.find("Observation.Beam[%s].Tied" % (options.sap)) >= 0 and line.find("angle1") >= 0:
			RAs.append([int(line.split("TiedArrayBeam[")[1].split("]")[0]),float(line.split()[-1])])
		if line.find("Observation.Beam[%s].Tied" % (options.sap)) >= 0 and line.find("angle2") >= 0:
			DECs.append([int(line.split("TiedArrayBeam[")[1].split("]")[0]),float(line.split()[-1])])
		# check "coherent" attribute of the beam
		if line.find("Observation.Beam[%s].Tied" % (options.sap)) >= 0 and line.find("coherent") >= 0:
			beamtype = line.split()[-1].lower()[:1]
			if beamtype == 'f': nisbeams += 1
	file.close()
	RAs = np.array(RAs)
	DECs = np.array(DECs)
	nbeams = len(RAs) - nisbeams

	line=options.parset
	obsid=str(line.split(".")[0])

	#Grab the chisqs
	#format is like this (note, list is NOT in beam numerical order):
	#file=./stokes/RSP113/B2217+47_L30847_RSP113_PSR_B2217+47.pfd.th.png obs=CS__RSP113_B2217+47 chi-sq=36.36464
	#CS_SAP0_BEAM0_
	file = open(options.chisq,"r")
	for line in file.readlines():
		beam = int(line.split("CS_SAP%s_BEAM" % (options.sap))[1].split("_")[0])
		chisq = float(line.split("chi-sq=")[1])
		Chisqs.append([beam,chisq])    
	file.close()

	#Determine which beams are actually taken
	Chisqs = np.array(Chisqs)

	Chisqs = Chisqs[Chisqs[:,0].argsort(),:]

	# sets the RAs and DECs to zero where there is a missing beam
	indices = []
	for i in Chisqs:
		indices.append(indexOf(RAs[:,0],i[0]))
	RAs_good = RAs[:,1][indices]
	DECs_good = DECs[:,1][indices]
	Beams_good = []
	Beams_good = RAs[:,0][indices]

	# determine the size of FoV to plot
	fov_size=np.max([np.abs(np.min(RAs_good)), np.abs(np.max(RAs_good)), np.abs(np.min(DECs_good)), np.abs(np.max(DECs_good))])
	fov_size=fov_size+0.2*fov_size # increase it by 20% for better viewing

	#s = np.ones(len(RAs))*300
	#c = np.zeros(len(RAs))+0.01
	#uncomment this if you want to block out the blank beams w/blue as white
	#scatter(RAs[:,1]*RAD2DEG,DECs[:,1]*RAD2DEG,s=s, marker='o', c=c, hold='on')

	x = np.array(RAs_good)*RAD2DEG
	y = np.array(DECs_good)*RAD2DEG
	s = np.ones(len(RAs_good))*300
	c = Chisqs[:,1]

	xmin=-fov_size * RAD2DEG
	xmax=fov_size * RAD2DEG
	ymin=-fov_size * RAD2DEG
	ymax=fov_size * RAD2DEG

	### LINEAR plot ###

	#Jason played around with other scaling factors here:
	#c = Chisqs[:,1]/max(Chisqs[:,1])
	#Do a square-root scaling???
	#c = c**0.5/max(c**0.5)
	#c = c**0.25/max(c**0.25)
	#c = np.log(c)

	fig = plt.figure()
	ax = fig.add_subplot(111)
	#add the beam numbers per beam onto the plot as text
	plt.scatter(x,y,s=s, marker='o', c=c, hold='on')
	for i in np.arange(np.size(x)):
		plt.text(x[i], y[i], str(int(Beams_good[i])), color='black', fontsize=6, ha="center", va="center")

	#axis("equal")
	plt.xlim(xmin, xmax)
	plt.ylim(ymin, ymax)

	plt.xlabel("Right Ascension Offset [deg]\nOBSID=" + str(obsid), fontsize=12)
	plt.ylabel("Declination Offset [deg]", fontsize=12)

	# label the plot
	fig.suptitle("SAP #" + options.sap +". Cumulative S/N of PSR " + options.target + " in " + str(np.size(x)) + " (out of " + str(nbeams) + ")\nSimultaneous Tied-Array Beams [Linear Scale]", fontsize=14)
	cb = plb.colorbar()
	cb.ax.set_ylabel("Cumulative S/N", fontsize=14)
	# rotating labels on colorbar 90 deg
	#for label in cb.ax.get_yticklabels():
	#	label.set_rotation(90)

	# set minor ticks
	ax.xaxis.set_minor_locator(ticker.AutoMinorLocator())
	ax.yaxis.set_minor_locator(ticker.AutoMinorLocator())

	# write the linear scale output file
	plt.savefig(options.outlinscale)

	#### LOG plot ####

	# clear the plotting and now set up the log plot
	plt.clf()

	# log of the chi-sq values for the lot plot
	c = Chisqs[:,1]
	c = np.log(c)

	fig = plt.figure()
	ax = fig.add_subplot(111)
	#add the beam numbers per beam onto the plot as text
	plt.scatter(x,y,s=s, marker='o', c=c, hold='on')
	for i in np.arange(np.size(x)):
		plt.text(x[i], y[i], str(int(Beams_good[i])), color='black', fontsize=6, ha="center", va="center")

	#axis("equal")
	plt.xlim(xmin, xmax)
	plt.ylim(ymin, ymax)

	plt.xlabel("Right Ascension Offset [deg]\nOBSID=" + str(obsid), fontsize=12)
	plt.ylabel("Declination Offset [deg]", fontsize=12)

	# label the plot
	fig.suptitle("SAP #" + options.sap +". Cumulative S/N of PSR " + options.target + " in " + str(np.size(x)) + " (out of " + str(nbeams) + ")\nSimultaneous Tied-Array Beams [Log Scale]", fontsize=14)
	cb = plb.colorbar()
	cb.ax.set_ylabel("Cumulative S/N", fontsize=14)
	# rotating labels on colorbar 90 deg
	#for label in cb.ax.get_yticklabels():
	#	label.set_rotation(90)

	# set minor ticks
	ax.xaxis.set_minor_locator(ticker.AutoMinorLocator())
	ax.yaxis.set_minor_locator(ticker.AutoMinorLocator())

	#use this to send plot to stdout
	#plt.show()

	# write the log scale output file
	plt.savefig(options.outlogscale)

	print "Writing output plots: " + str(options.outlogscale) + " " + str(options.outlinscale)
