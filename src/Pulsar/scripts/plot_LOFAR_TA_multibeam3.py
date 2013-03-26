#!/usr/bin/env python
#
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import matplotlib.pylab as plb
import matplotlib.ticker as ticker
from matplotlib.patches import Ellipse, Rectangle
import numpy as np
import sys, re
import ephem
from math import cos, atan, pi
from operator import indexOf
from optparse import OptionParser

RAs = []
DECs = []
Chisqs = []
RAD2DEG = 1./0.0174532925


## Weighted average plus error 
def wmom(arrin, weights_in, inputmean=None, calcerr=False, sdev=False):
    """
    NAME:
      wmom()
      
    PURPOSE:
      Calculate the weighted mean, error, and optionally standard deviation of
      an input array.  By default error is calculated assuming the weights are
      1/err^2, but if you send calcerr=True this assumption is dropped and the
      error is determined from the weighted scatter.

    CALLING SEQUENCE:
     wmean,werr = wmom(arr, weights, inputmean=None, calcerr=False, sdev=False)
    
    INPUTS:
      arr: A numpy array or a sequence that can be converted.
      weights: A set of weights for each elements in array.
    OPTIONAL INPUTS:
      inputmean: 
          An input mean value, around which them mean is calculated.
      calcerr=False: 
          Calculate the weighted error.  By default the error is calculated as
          1/sqrt( weights.sum() ).  If calcerr=True it is calculated as sqrt(
          (w**2 * (arr-mean)**2).sum() )/weights.sum()
      sdev=False: 
          If True, also return the weighted standard deviation as a third
          element in the tuple.

    OUTPUTS:
      wmean, werr: A tuple of the weighted mean and error. If sdev=True the
         tuple will also contain sdev: wmean,werr,wsdev

    REVISION HISTORY:
      Converted from IDL: 2006-10-23. Erin Sheldon, NYU

   """
    
    # no copy made if they are already arrays
    arr = np.array(arrin, ndmin=1, copy=False)
    
    # Weights is forced to be type double. All resulting calculations
    # will also be double
    weights = np.array(weights_in, ndmin=1, dtype='f8', copy=False)
  
    wtot = weights.sum()
        
    # user has input a mean value
    if inputmean is None:
        wmean = ( weights*arr ).sum()/wtot
    else:
        wmean=float(inputmean)

    # how should error be calculated?
    if calcerr:
        werr2 = ( weights**2 * (arr-wmean)**2 ).sum()
        werr = np.sqrt( werr2 )/wtot
    else:
        werr = 1.0/np.sqrt(wtot)

    # should output include the weighted standard deviation?
    if sdev:
        wvar = ( weights*(arr-wmean)**2 ).sum()/wtot
        wsdev = np.sqrt(wvar)
        return wmean,werr,wsdev
    else:
        return wmean,werr

# formatted string output
def str_f(val):
    string = str(val)
    # if the : is on the 2nd spot, add a zero
    if (str(val).find(':')==1):
        string = '0' + str(val)
    return string

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
	parser.add_option("-d", "--diameter",action="store",type="float", dest="diameter",
        	          help="Provide here the size of the element (in m) to get a subplot of the beam shape")
		  
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
		# get the central coordinate
		if line.find("Observation.Beam[%s].angle1" % (options.sap)) >= 0:
			rarad=np.float64(line.split(" = ")[-1])
		if line.find("Observation.Beam[%s].angle2" % (options.sap)) >= 0:
			decrad=np.float64(line.split(" = ")[-1])
		# get some numbers for the beam estimate
		if line.find("OLAP.storageStationNames = [") >= 0:
			stationlist=line.split(" = ")[-1]
		if line.find("Observation.startTime") >= 0:
			# pyephem needs yyyy/mm/dd format
			starttime=line.split("'")[-2].replace('-','/')
		if line.find("Observation.subbandList") >= 0:
			subbandlist=line.split(" = ")[-1]

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
		psr=line.split("_")[-1].split(" chi")[0]
		if psr != options.target: continue
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
#	fov_size=np.max([np.abs(np.min(RAs_good)), np.abs(np.max(RAs_good)), np.abs(np.min(DECs_good)), np.abs(np.max(DECs_good))])
#	fov_size=fov_size+0.2*fov_size # increase it by 20% for better viewing
	RA_size=np.max([np.abs(np.min(RAs_good)), np.abs(np.max(RAs_good))])
	RA_size=RA_size+0.2*RA_size # increase it by 20% for better viewing
	DEC_size=np.max([np.abs(np.min(DECs_good)), np.abs(np.max(DECs_good))])
	DEC_size=DEC_size+0.2*DEC_size # increase it by 20% for better viewing

	#s = np.ones(len(RAs))*300
	#c = np.zeros(len(RAs))+0.01
	#uncomment this if you want to block out the blank beams w/blue as white
	#scatter(RAs[:,1]*RAD2DEG,DECs[:,1]*RAD2DEG,s=s, marker='o', c=c, hold='on')

	x = np.array(RAs_good)*RAD2DEG
	y = np.array(DECs_good)*RAD2DEG
	s = np.ones(len(RAs_good))*300
	c = Chisqs[:,1]

	xmin=-RA_size * RAD2DEG
	xmax=RA_size * RAD2DEG
	ymin=-DEC_size * RAD2DEG
	ymax=DEC_size * RAD2DEG

	### Determine the weighted best position of the folded pulsar ###
	# subtract min
	weights = [val - min(c) for val in c]

#	# remove the baseline points (<10% max)
#	for i,val in enumerate(weights):
#		if val < 0.1*max(weights):
#			weights[i] = 0.0

        # only use the two circles (18 points) around the maximum value at index j
        j=np.argsort(c)[-1]
        dsq=np.zeros(len(RAs_good))
        for i,val in enumerate(weights):
            dsq[i] = (x[j]-x[i])**2 + (y[j]-y[i])**2
        # set the weights of the farther-out beams to zero
        for i,val in enumerate(weights):
            if dsq[i] > np.sort(dsq)[18]:
                weights[i] = 0.0

	# calculate the weighted mean:
	xmean,xerr = wmom(x, weights, inputmean=None, calcerr=True, sdev=False)
	ymean,yerr = wmom(y, weights, inputmean=None, calcerr=True, sdev=False)
#	print xmean, ymean
#	print rarad, decrad

	# convert to HHMMSS
	src      = ephem.FixedBody()
	src._ra  = float(rarad+xmean/RAD2DEG)
	src._dec = float(decrad+ymean/RAD2DEG)
#	print str(src._ra), str(src._dec)

	err      = ephem.FixedBody()
	err._ra  = float(xerr/RAD2DEG)/cos(decrad)
	err._dec = float(yerr/RAD2DEG)
#	print "RA_err : %f arcmin" % (xerr*60)
#	print "DEC_err: %f arcmin" % (yerr*60)

        print "Best-fit position: (RAJ, DECJ) = ( %s +/- %s , %s +/- %s )" % (str_f(src._ra), str_f(err._ra), str_f(src._dec), str_f(err._dec))

	# Estimate the beam shape, only for HBA data
	if stationlist.find("HBA") >= 0:

		plot_beam=True
		# set up the observatory
		src._epoch = ephem.J2000
		telescope = ephem.Observer()
		telescope.lat = ephem.degrees('52.91511')
		telescope.long = ephem.degrees('6.869883')
		telescope.elevation = 0
		telescope.date = ephem.Date(starttime)
		src.compute(telescope)
#		print src.ra, src.dec, src.alt, src.az 

		# calculate the approximate beam size
		bottomsubband=float(re.findall(r"\d+", subbandlist)[0])
		l = 300.0 / (120 + 0.195*bottomsubband) # bottom wavelength in m

#		ncs = stationlist.count("CS")
#		nrs = stationlist.count("RS")
#		diameter  = (ncs*35 + nrs*50)/(ncs+nrs)   # average station in m
#		diameter = options.diameter

                diameter = 300 # superterp
                if re.search("CS0[123]", stationlist)!=None:
                    diameter = 600  # next group of stations
                if re.search("CS[12345]", stationlist)!=None:
                    diameter = 3000 # single-clock core
                if options.diameter != None:
                    diameter = options.diameter
#               print "Using diameter = %f" % (diameter)
		beamsize = 1.22 * l / diameter            # fwhm in rad

#		print "Beamsize = %f" % (beamsize)

		# calculate the beam angle in ra-dec plot
		raoff  = telescope.radec_of(src.az,src.alt+beamsize)[0]-telescope.radec_of(src.az,src.alt)[0]
		decoff = telescope.radec_of(src.az,src.alt+beamsize)[1]-telescope.radec_of(src.az,src.alt)[1]
#		print "Centre: %f %f " % telescope.radec_of(src.az,src.alt)
#		print "Centre: %f %f " % telescope.radec_of(src.az,src.alt+beamsize)
#		print "Centre: %f %f " % (raoff, decoff)
		beamsize = beamsize*RAD2DEG

	else:
		plot_beam=False


        ### PLOTS ####
        
	fig = plt.figure()

        # Two type of plots
        #           LABEL          OUTFILE              COLOR RANGE
        plots = [ ("Linear Scale", options.outlinscale, c         ),
                  ("Log Scale",    options.outlogscale, np.log(c) ) ]

        # Make the two plots
        for (label, filename, c) in plots:
		ax = fig.add_subplot(111)
		#add the beam numbers per beam onto the plot as text
		plt.scatter(x,y,s=s, marker='o', c=c, hold='on')
		for i in np.arange(np.size(x)):
			plt.text(x[i], y[i], str(int(Beams_good[i])), color='black', fontsize=6, ha="center", va="center")
	
		# plot the best position
		plt.errorbar(xmean, ymean, yerr=yerr, xerr=xerr, fmt='w', linewidth=3)
		plt.errorbar(xmean, ymean, yerr=yerr, xerr=xerr, fmt='black')
		# plus a label with the value, top left
		plt.errorbar(xmin+(xmax-xmin)/20, ymax-(ymax-ymin)/20, yerr=yerr, xerr=xerr, fmt='black')
	#	plt.text(xmin+(xmax-xmin)/20+2*xerr, ymax-(ymax-ymin)/20, " dx = %f +/- %f \n dy = %f +/- %f" % (xmean, xerr, ymean, yerr),
		plt.text(xmin+(xmax-xmin)/20+2*xerr, ymax-(ymax-ymin)/20, " %s +/- %s \n %s +/- %s " % 
                         (str_f(src._ra), str_f(err._ra), str_f(src._dec), str_f(err._dec)),
			 color='black', fontsize=6, ha="left", va="center")
	
		# plot the beam shape
		if plot_beam==True:
			angle=atan(raoff/decoff)
	#		plt.plot([0,raoff*RAD2DEG],[0,decoff*RAD2DEG])
	#		print "Angle",  angle*RAD2DEG
	#		print float(pi/2), float(src.alt), cos(pi/2-src.alt)
			e = Ellipse(xy=[xmin+0.7*beamsize,ymin+0.7*beamsize], 
                                    width=beamsize, height=beamsize/cos(pi/2-src.alt), 
                                    angle=-angle*RAD2DEG, fc='white')
			ax.add_artist(e)
			r = Rectangle((xmin, ymin), 1.4*beamsize, 1.4*beamsize, fc='white')
			ax.add_patch(r)
	
		#axis("equal")
		plt.xlim(xmin, xmax)
		plt.ylim(ymin, ymax)
	
		plt.xlabel("Right Ascension Offset [deg]\nOBSID=" + str(obsid), fontsize=12)
		plt.ylabel("Declination Offset [deg]", fontsize=12)
	
		# label the plot
		fig.suptitle("SAP #" + options.sap +". Cumulative S/N of PSR " + 
                             options.target + " in " + str(np.size(x)) + 
                             " (out of " + str(nbeams) + 
                             ")\nSimultaneous Tied-Array Beams [" + label + 
                             "]", fontsize=14)
		cb = plb.colorbar()
		cb.ax.set_ylabel("Cumulative S/N", fontsize=14)
		# rotating labels on colorbar 90 deg
		#for label in cb.ax.get_yticklabels():
		#	label.set_rotation(90)
	
		# set minor ticks
		ax.xaxis.set_minor_locator(ticker.AutoMinorLocator())
		ax.yaxis.set_minor_locator(ticker.AutoMinorLocator())
	
		# write the linear scale output file
		plt.savefig(filename)
	
		#### LOG plot ####
	
		# clear the plotting and now set up the log plot
		plt.clf()


	print "Writing output plots: " + str(options.outlogscale) + " " + str(options.outlinscale)
