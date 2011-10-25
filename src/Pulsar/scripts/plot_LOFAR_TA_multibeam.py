#!/usr/bin/env python
import matplotlib
matplotlib.use('TkAgg') 
from pylab import *
import numpy
from operator import indexOf
from optparse import OptionParser

RAs = []
DECs = []
Chisqs = []

RAD2DEG = 1./0.0174532925

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
		  
(options, args) = parser.parse_args()


#Grab the RAs and DECs ;  note these are NOT is numerical order of beam number
#Observation.Beam[0].TiedArrayBeam[139].angle1 = 0.00755693767342
file = open(options.parset,"r")
for line in file.readlines():
    if line.find("Observation.Beam[0].Tied") >= 0 and line.find("angle1") >= 0:
        RAs.append([int(line.split("TiedArrayBeam[")[1].split("]")[0]),float(line.split()[-1])])
    if line.find("Observation.Beam[0].Tied") >= 0 and line.find("angle2") >= 0:
        DECs.append([int(line.split("TiedArrayBeam[")[1].split("]")[0]),float(line.split()[-1])])
file.close()
RAs = numpy.array(RAs)
DECs = numpy.array(DECs)

#Grab the chisqs
#format is like this (note, list is NOT in beam numerical order):
#file=./stokes/RSP113/B2217+47_L30847_RSP113_PSR_B2217+47.pfd.th.png obs=CS__RSP113_B2217+47 chi-sq=36.36464
file = open(options.chisq,"r")
for line in file.readlines():
    beam = int(line.split("CS__RSP")[1].split("_")[0])
    chisq = float(line.split("chi-sq=")[1])
    Chisqs.append([beam,chisq])    
file.close()

#Determine which beams are actually taken
Chisqs = numpy.array(Chisqs)

Chisqs = Chisqs[Chisqs[:,0].argsort(),:]

# sets the RAs and DECs to zero where there is a missing beam
indices = []
for i in Chisqs:
    indices.append(indexOf(RAs[:,0],i[0]))
RAs_good = RAs[:,1][indices]
DECs_good = DECs[:,1][indices]
Beams_good = []
Beams_good = RAs[:,0][indices]

s = numpy.ones(len(RAs))*300
c = numpy.zeros(len(RAs))+0.01

#uncomment this if you want to block out the blank beams w/blue as white
#scatter(RAs[:,1]*RAD2DEG,DECs[:,1]*RAD2DEG,s=s, marker='o', c=c, hold='on')

x = numpy.array(RAs_good)*RAD2DEG
y = numpy.array(DECs_good)*RAD2DEG
s = numpy.ones(len(RAs_good))*300
c = Chisqs[:,1]

#Jason played around with other scaling factors here:
#c = Chisqs[:,1]/max(Chisqs[:,1])
#Do a square-root scaling???
#c = c**0.5/max(c**0.5)
#c = c**0.25/max(c**0.25)
#c = numpy.log(c)

#add the beam numbers per beam onto the plot as text
i=0
while i < (x.size - 1) :
    text(x[i], y[i], str(int(Beams_good[i])), color='red', fontsize=6, ha="center")
    i += 1

scatter(x,y,s=s, marker='o', c=c, hold='on')

#axis("equal")
xlim(-0.08*RAD2DEG,0.08*RAD2DEG)
ylim(-0.08*RAD2DEG,0.08*RAD2DEG)

xlabel("Right Ascension Offset [deg]", fontsize=14)
ylabel("Declination Offset [deg]", fontsize=14)

suptitle("Cumulative S/N of PSR " + options.target + " in " + str(size(c)) + " (out of " + str(int((DECs[:,0].size)/2)) + ")\n Simultaneous Tied-Array Beams [Linear Scale]", fontsize=14)
colorbar()

# write the linear scale output file
file = open(options.outlinscale,"w")
savefig(file)
file.close()

# clear the plotting and now set up the log plot
clf()

# log of the chi-sq values for the lot plot
c = Chisqs[:,1]
c = numpy.log(c)

scatter(x,y,s=s, marker='o', c=c, hold='on')
i=0
while i < (x.size - 1) :
    text(x[i], y[i], str(int(Beams_good[i])), color='red', fontsize=6, ha="center")
    i += 1

#axis("equal")
xlim(-0.08*RAD2DEG,0.08*RAD2DEG)
ylim(-0.08*RAD2DEG,0.08*RAD2DEG)

xlabel("Right Ascension Offset [deg]", fontsize=14)
ylabel("Declination Offset [deg]", fontsize=14)

# label the plot [note, there are 2x as many beams in the parset right now;  can remove the divide by 2 later
suptitle("Cumulative S/N of PSR " + options.target + " in " + str(size(c)) + " (out of " + str(int((DECs[:,0].size)/2)) + ")\n Simultaneous Tied-Array Beams [Log Scale]", fontsize=14)

# for LOTAS, number of beams is correct
###suptitle("Cumulative S/N of PSR " + options.target + " in " + str(size(c)) + " (out of " + str(int((DECs[:,0].size))) + ")\n Simultaneous Tied-Array Beams [Log Scale]", fontsize=14)

colorbar()

#use this to send plot to stdout
#show()

# write the log scale output file
#savefig('myfilename.png')
file = open(options.outlogscale,"w")
savefig(file)
file.close()

print "Writing output plots: " + str(options.outlogscale) + " " + str(options.outlinscale)
