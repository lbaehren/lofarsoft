#!/usr/bin/env python
#
# Script to collect info about LOFAR observations and write out
# the list of obs with different parameters listed, both in ascii
# and Html formats
#
# Vlad, Aug 5, 2010 (c)
###################################################################
import os, sys, glob
import getopt
import numpy as np
import time
import cPickle
import re

# sorting type
sortkind="obsid"  # four kinds of sorting currently: by start time ("time"), 
                  # by volume ("size"), by source pointing ("source"), and default is by ObsID ("obsid")

# if True then will show only those observations newer than some date
is_from=False
fromdate=""
# if True then will show only those observations older than some date
is_to=False
todate=""
# if True then make a list in html format
is_html=False
htmlfile=""  # name of the html file in case is_html == True
# if True then create all sorted html lists linked together
is_linkedhtml = False
linkedhtmlstem=""   # filestem of linked html files if is_linkedhtml = True
# if True then delete dump of obs list (if exists) and recreate it from scratch
# if False then first read the dump, compare obsids with the current ones and process
# only those ObsIDs that do not exist in the dump. After the dump is updated
is_rebuild = False
# if True then updates only the records in the dumpfile without checking if new obs appeared
is_update = False
# if True then obs will be printed one by one (debug mode) (with # = 0 for all)
is_debug = False
debugcounter=0

# if True, then only db file will be read to calculate the statistics of observations
is_stats = False

# View of presenting info (usual (defaul), brief, plots, and mega)
viewtype="usual"

# Setting User name
username=os.environ['USER']
# where to copy profile plots (dop95)
webserver="%s@10.87.2.95" % (username, )
plotsdir="/home/%s/Lofar/plots" % (username, )
webplotsdir="public_html/lofar/plots"
# if False, then do not rsync plots to external webserver
is_torsync = True

# file used in debug mode to write iteration number in main loop
debugfile="/home/%s/.obsinfo_debug" % (username, )

atnflink_start="http://www.atnf.csiro.au/research/pulsar/psrcat/proc_form.php?startUserDefined=true&c1_val=&c2_val=&c3_val=&c4_val=&sort_attr=jname&sort_order=asc&condition=&pulsar_names="
atnflink_end="&ephemeris=long&submit_ephemeris=Get+Ephemeris&coords_unit=raj%2Fdecj&radius=&coords_1=&coords_2=&style=Long+with+last+digit+error&no_value=*&fsize=3&x_axis=&x_scale=linear&y_axis=&y_scale=linear&state=query"
nedlink_start="http://nedwww.ipac.caltech.edu/cgi-bin/nph-objsearch?objname="
nedlink_end="&extend=no&hconst=73&omegam=0.27&omegav=0.73&corr_z=1&out_csys=Equatorial&out_equinox=J2000.0&obj_sort=RA+or+Longitude&of=pre_text&zv_breaker=30000.0&list_limit=5&img_stamp=YES"

# storage nodes to collect info about Pulsar Observations
# we assume that even for the case of long observations when data were spreaded out
# across many other nodes, these three "pulsar" sub5 nodes were used to record a part
# of the data as well
storage_nodes=["lse013", "lse014", "lse015", "lse016", "lse017", "lse018"]
# list of directories withe the data
data_dirs=["/data1", "/data2", "/data3", "/data4"]

# cexec corresponding table
cexec_nodes={'lse013': 'sub5:9', 'lse014': 'sub5:10', 'lse015': 'sub5:11',
             'lse016': 'sub6:9', 'lse017': 'sub6:10', 'lse018': 'sub6:11',
             'lse019': 'sub7:9', 'lse020': 'sub7:10', 'lse021': 'sub7:11',
             'lse022': 'sub8:9', 'lse023': 'sub8:10', 'lse024': 'sub8:11',
             'lse001': 'sub1:9', 'lse002': 'sub1:10', 'lse003': 'sub1:11',
             'lse004': 'sub2:9', 'lse005': 'sub2:10', 'lse006': 'sub2:11',
             'lse007': 'sub3:9', 'lse008': 'sub3:10', 'lse009': 'sub3:11',
             'lse010': 'sub4:9', 'lse011': 'sub4:10', 'lse012': 'sub4:11'}
cexec_egrep_string="egrep -v \'\\*\\*\\*\\*\\*\' |egrep -v \'\\-\\-\\-\\-\\-\'"

# list of lse nodes names for use as keys in the dictionary that keeps the sizes of obs directories in outputInfo class
lsenames = ["lse%03d" % (n, ) for n in np.arange(1,25,1)]

# file to dump the obs table
dumpfile="/home/%s/Lofar/dump.b" % (username, )

# directories with parset files
parset_logdir="/globalhome/lofarsystem/log/"
parset_oldlogdir="/globalhome/lofarsystem/oldlog/"
parset_newlogdir="/globalhome/lofarsystem/production/lofar-trunk/bgfen/log/"
# name of the parset file
parset="RTCP.parset.0"

# list of obs ids
obsids=[]
# list of ObsIDs from the dump file 
dbobsids=[]





# Class obsinfo with info from the parset file
class obsinfo:
	def __init__(self, log):
		self.parset = log	
		# Getting the Date of observation
	        cmd="grep Observation.startTime %s | tr -d \\'" % (self.parset,)
        	self.starttime=os.popen(cmd).readlines()
        	if np.size(self.starttime) > 0:
                	# it means that this keyword exist and we can extract the info
                	self.starttime=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
			# Getting the number of seconds from 1970. Can use this to sort obs out by date/time
			self.seconds=time.mktime(time.strptime(self.starttime, "%Y-%m-%d %H:%M:%S"))
                	smonth=self.starttime.split("-")[1]
                	sday=self.starttime.split("-")[2].split(" ")[0]
                	self.datestring=smonth+sday
        	else:
                	self.datestring="????"
			self.seconds = 0

		# Getting the Antenna info (HBA or LBA)
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	self.antenna=os.popen(cmd).readlines()
        	if np.size(self.antenna)>0:
                	# Antenna array setting exists in parset file
                	self.antenna=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("_")[0]
        	else:
                	self.antenna="?"

		# Getting the Filter setting
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	self.band=os.popen(cmd).readlines()
        	if np.size(self.band)>0:
                	# band filter setting exists in parset file
                	self.band=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("A_")[-1]
        	else:
                	self.band="?"

		# Getting the stations and their number (including separately the number of CS and RS)
#        	cmd="grep 'Observation.VirtualInstrument.stationList' %s" % (self.parset,)
        	cmd="grep 'OLAP.storageStationNames' %s" % (self.parset,)
        	self.stations=os.popen(cmd).readlines()
        	if np.size(self.stations)>0:
                	# Stations setting exists in parset file
                	self.stations=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
			# removing LBA and HBA from station names, replacing HBA ears HBA0 to /0 and HBA1 to /1
			self.stations = re.sub("HBA0", "/0", self.stations)
			self.stations = re.sub("HBA1", "/1", self.stations)
			self.stations = re.sub("HBA", "", self.stations)
			self.stations = re.sub("LBA", "", self.stations)
			stations_array = self.stations.split(",")
			self.nstations = np.size(stations_array)
			self.ncorestations = self.stations.count("CS")
			self.nremotestations = self.stations.count("RS")
			self.stations_string = "%d [%dCS, %dRS]" % (self.nstations, self.ncorestations, self.nremotestations) 
			self.stations_html=""
			for n in np.arange(self.nstations-1):
				if n != 0 and n % 9 == 0: self.stations_html += "<br>"
				self.stations_html += stations_array[n] + ","
			self.stations_html += stations_array[-1]
        	else:
                	self.stations="?"
			self.stations_string="?"

	        # reading the parset file
	        # getting the info about StorageNodes. Note! For old parsets there seems to be no such a keyword Virtual...
        	# However, the old keyword OLAP.storageNodeList has "non-friendly" format, so I just ignore this by now
        	cmd="grep Observation.VirtualInstrument.storageNodeList %s | sed -e 's/lse//g'" % (self.parset,)
        	self.nodeslist=os.popen(cmd).readlines()
        	if np.size(self.nodeslist) > 0:
                	# it means that this keyword exist and we can extract the info
                	self.nodeslist=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
        	# cut the string of nodes if it is too long
        	if len(self.nodeslist)>13:
                	self.nodeslist=self.nodeslist[:13] + "..."

	        # getting the name of /data? where the data are stored
        	cmd="grep Observation.MSNameMask %s" % (self.parset,)
        	self.datadir="/" + os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("/")[1]

	        # getting info about the Type of the data (BF, Imaging, etc.)
        	# check first if data are beamformed
        	cmd="grep outputBeamFormedData %s" % (self.parset,)
        	self.bftype=os.popen(cmd).readlines()
        	if np.size(self.bftype) > 0:
                	# this info exists in parset file
                	self.bftype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.bftype == 'f':
                        	self.bftype = "-"
                	else:
                        	self.bftype = "+"
        	else:
                	self.bftype = "?"

        	# check first if data are filtered
        	cmd="grep outputFilteredData %s" % (self.parset,)
        	self.fdtype=os.popen(cmd).readlines()
        	if np.size(self.fdtype) > 0:
                	# this info exists in parset file
                	self.fdtype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.fdtype == 'f':
                        	self.fdtype = "-"
                	else:
                        	self.fdtype = "+"
        	else:
                	self.fdtype = "?"

	        # check if data are imaging
        	cmd="grep outputCorrelatedData %s" % (self.parset,)
        	self.imtype=os.popen(cmd).readlines()
        	if np.size(self.imtype) > 0:
                	# this info exists in parset file
                	self.imtype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.imtype == 'f':
                        	self.imtype = "-"
                	else:
                        	self.imtype = "+"
        	else:
                	self.imtype = "?"

	        # check if data are incoherent stokes data
        	cmd="grep outputIncoherentStokes %s" % (self.parset,)
        	self.istype=os.popen(cmd).readlines()
        	if np.size(self.istype) > 0:
                	# this info exists in parset file
                	self.istype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.istype == 'f':
                        	self.istype = "-"
                	else:
                        	self.istype = "+"
        	else:
                	self.istype = "?"

	        # check if data are coherent stokes data
        	cmd="grep outputCoherentStokes %s" % (self.parset,)
        	self.cstype=os.popen(cmd).readlines()
        	if np.size(self.cstype) > 0:
                	# this info exists in parset file
                	self.cstype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.cstype == 'f':
                        	self.cstype = "-"
                	else:
                        	self.cstype = "+"
        	else:
                	self.cstype = "?"

	        # check if data are fly's eye mode data
        	cmd="grep PencilInfo.flysEye %s" % (self.parset,)
        	self.fetype=os.popen(cmd).readlines()
        	if np.size(self.fetype) > 0:
                	# this info exists in parset file
                	self.fetype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.fetype == 'f':
                        	self.fetype = "-"
                	else:
                        	self.fetype = "+"
        	else:
                	self.fetype = "?"

	        # getting info about the pointing
        	cmd="grep 'Beam\[0\].angle1' %s" % (self.parset,)
        	self.rarad=os.popen(cmd).readlines()
        	if np.size(self.rarad)>0:
                	# RA info exists in parset file
                	self.rarad=float(os.popen(cmd).readlines()[0][:-1].split(" = ")[-1])
                	rahours=self.rarad*12./3.1415926
                	rah=int(rahours)
                	ram=int((rahours-rah)*60.)
                	self.rastring="%02d%02d" % (rah, ram)
        	else:
                	self.rastring="????"

        	cmd="grep 'Beam\[0\].angle2' %s" % (self.parset,)
        	self.decrad=os.popen(cmd).readlines()
        	if np.size(self.decrad)>0:
                	# DEC info exists in parset file
                	self.decrad=float(os.popen(cmd).readlines()[0][:-1].split(" = ")[-1])
                	decdeg=self.decrad*180./3.1415926
                	if decdeg>0:
                        	decsign="+"
                	else:
                        	decsign="-"
                	decdeg=abs(decdeg)
                	decd=int(decdeg)
                	decm=int((decdeg-decd)*60.)
                	self.decstring="%c%02d%02d" % (decsign, decd, decm)
        	else:
                	self.decstring="_????"
        	self.pointing="%s%s" % (self.rastring, self.decstring)

	        # getting info about Source name (new addition to the Parset files)
        	cmd="grep 'Observation.Beam\[0\].target' %s" % (self.parset,)
        	self.source=os.popen(cmd).readlines()
        	if np.size(self.source)>0:
                	# Source name exists in parset file
                	self.source=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
			if self.source != "":
				if self.source[0] == "'":
					self.source=self.source.split("'")[1]
				if self.source[0] == "\"":
					self.source=self.source.split("\"")[1]
        	else:
                	self.source=""

        	# Getting the Duration
        	cmd="grep Observation.stopTime %s | tr -d \\'" % (self.parset,)
        	self.stoptime=os.popen(cmd).readlines()
        	if np.size(self.starttime) > 0 and np.size(self.stoptime) > 0:
                	# it means that both start and stop Times exist in parset file
                	self.stoptime=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
                	c1 = time.strptime(self.starttime, "%Y-%m-%d %H:%M:%S")
                	c2 = time.strptime(self.stoptime, "%Y-%m-%d %H:%M:%S")
                	self.dur=time.mktime(c2)-time.mktime(c1)  # difference in seconds
                	if float(self.dur/3600.0) > 1.:
                        	self.duration="%.1fh" % (self.dur/3600.)
                	else:
                        	self.duration="%.1fm" % (self.dur/60.)
        	else:
                	self.duration="?"







# Class (structure) with all fields to output
class outputInfo:
	def __init__(self, id):
		self.id = id
		self.obsyear = self.id.split("_")[0][1:]
		try:
			self.seconds=time.mktime(time.strptime(self.obsyear, "%Y"))
		except StandardError:
			print "Network problems or non-standard obs dir name: %s" % (self.id, )
			sys.exit(1)
		self.storage = {}
		for key in lsenames:
			self.storage[key] = ["x", "0.0"]
	
	def setcomment (self, id, storage_nodes, comment):
		self.id = id
		self.obsyear = self.id.split("_")[0][1:]
		self.seconds=time.mktime(time.strptime(self.obsyear, "%Y"))
		self.comment = comment
		self.totsize = 0.0
		self.processed_dirsize = 0.0
		self.pointing = "????_????"
		self.cs = len(storage_nodes)
		if viewtype == "brief":
			self.colspan = 13
		elif viewtype == "plots":
			self.colspan = 18
		elif viewtype == "mega":
			self.colspan = 22 + self.cs
		else:
			self.colspan = 14 + self.cs

		if self.comment != "":
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=left>%s</td>" % (self.id, self.colspan, self.comment,)

	def Init(self, id, oi, storage_nodes, dirsizes, statusline, redlocation, processed_dirsize, comment, filestem_array, chi_array):
		self.id = id
		self.obsyear = self.id.split("_")[0][1:]
		self.oi = oi
		if self.oi.seconds != 0:
			self.seconds = self.oi.seconds
		else:
			self.seconds=time.mktime(time.strptime(self.obsyear, "%Y"))
		self.pointing = self.oi.pointing
		self.statusline = statusline
		self.redlocation = redlocation
		self.processed_dirsize = processed_dirsize
		self.comment = comment
		self.cs = len(storage_nodes)
		if viewtype == "brief":
			self.colspan = 13
		elif viewtype == "plots":
			self.colspan = 17
		elif viewtype == "mega":
			self.colspan = 21 + self.cs
		else:
			self.colspan = 14 + self.cs
		self.filestem_array = filestem_array
		self.chi_array = chi_array

		# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
		self.totsize=0.0
		self.dirsize_string=""
		for l in storage_nodes:
			self.storage[l]=dirsizes[l]
		for l in storage_nodes:
			self.totsize = self.totsize + float(self.storage[l][1])
			self.dirsize_string = self.dirsize_string + self.storage[l][0] + "\t"
		# converting total size to GB
		self.totsize = "%.1f" % (self.totsize / 1024. / 1024. / 1024.,)
		self.dirsize_string_html = "</td>\n <td align=center>".join(self.dirsize_string.split("\t")[:-1])

		if self.comment == "":
			if viewtype == "brief" or viewtype == "plots":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%s		%-27s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.redlocation, self.statusline)
			elif viewtype == "mega":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%-16s %s	%s%-9s	%s	%s		%-27s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.stations, self.redlocation, self.statusline)
			else: # usual
				self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-27s	%s   %s" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)
			if viewtype == "brief":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.redlocation)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left>%s</td>" % (self.id, (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_start or nedlink_start, self.oi.source.replace("+", "%2B"), (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_end or nedlink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.redlocation)
			elif viewtype == "plots" or viewtype == "mega":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_start or nedlink_start, self.oi.source.replace("+", "%2B"), (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_end or nedlink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				chisize=np.size(self.chi_array)
				filestemsize=np.size(self.filestem_array)
				lmin=np.min([filestemsize, chisize])
				for l in np.arange(lmin):
					profile_string = "\n <td align=center>%s</td>\n <td align=center><a href=\"plots/%s/%s.png\"><img width=200 height=140 src=\"plots/%s/%s.th.png\"></a></td>" % (self.chi_array[l], self.id, self.filestem_array[l], self.id, self.filestem_array[l])
					self.infohtml = self.infohtml + profile_string
				if lmin < 2:
					for l in np.arange(lmin, 2):
						if l < chisize:
							self.infohtml = self.infohtml + "\n <td align=center>%s</td>" % (self.chi_array[l])
						else:
							self.infohtml = self.infohtml + "\n <td align=center></td>"
						if l < filestemsize:
							self.infohtml = self.infohtml + "\n <td align=center><a href=\"plots/%s/%s.png\"><img width=200 height=140 src=\"plots/%s/%s.th.png\"></a></td>" % (self.id, self.filestem_array[l], self.id, self.filestem_array[l])
						else:
							self.infohtml = self.infohtml + "\n <td align=center></td>"
				if viewtype == "plots": self.infohtml = self.infohtml + "\n <td align=left>%s</td>\n <td align=center>%s</td>" % (self.redlocation, self.statusline.replace("-", "&#8211;"))
				if viewtype == "mega":
					self.infohtml = self.infohtml + "\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=left>%s</td>\n <td align=center>%s</td>" % (self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.stations_html, self.redlocation, self.statusline.replace("-", "&#8211;"))
			else: # usual
				self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.statusline.replace("-", "&#8211;"), self.oi.pointing, self.oi.source)
		else:
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=left>%s</td>" % (self.id, self.colspan, self.comment,)

	def update(self, storage_nodes):
		self.cs = len(storage_nodes)
		# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
		self.totsize=0.0
		self.dirsize_string=""
		for l in storage_nodes:
			self.totsize = self.totsize + float(self.storage[l][1])
			self.dirsize_string = self.dirsize_string + self.storage[l][0] + "\t"
		# converting total size to GB
		self.totsize = "%.1f" % (self.totsize / 1024. / 1024. / 1024.,)
		self.dirsize_string_html = "</td>\n <td align=center>".join(self.dirsize_string.split("\t")[:-1])

		if viewtype == "brief":
			self.colspan = 13
		elif viewtype == "plots":
			self.colspan = 18
		elif viewtype == "mega":
			self.colspan = 22 + self.cs
		else:
			self.colspan = 14 + self.cs

		if self.comment == "":
			if viewtype == "brief" or viewtype == "plots":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%s		%-27s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.redlocation, self.statusline)
			elif viewtype == "mega":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%-16s %s	%s%-9s	%s	%s		%-27s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.stations, self.redlocation, self.statusline)
			else: #usual
				self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-27s	%s   %s" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)

			if viewtype == "brief":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.redlocation)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left>%s</td>" % (self.id, (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_start or nedlink_start, self.oi.source.replace("+", "%2B"), (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_end or nedlink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.redlocation)
			elif viewtype == "plots" or viewtype == "mega":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_start or nedlink_start, self.oi.source.replace("+", "%2B"), (self.oi.source[0] == "B" or self.oi.source[0] == "J") and atnflink_end or nedlink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				chisize=np.size(self.chi_array)
				filestemsize=np.size(self.filestem_array)
				lmin=np.min([filestemsize, chisize])
				for l in np.arange(lmin):
					profile_string = "\n <td align=center>%s</td>\n <td align=center><a href=\"plots/%s/%s.png\"><img width=200 height=140 src=\"plots/%s/%s.th.png\"></a></td>" % (self.chi_array[l], self.id, self.filestem_array[l], self.id, self.filestem_array[l])
					self.infohtml = self.infohtml + profile_string
				if lmin < 2:
					for l in np.arange(lmin, 2):
						if l < chisize:
							self.infohtml = self.infohtml + "\n <td align=center>%s</td>" % (self.chi_array[l])
						else:
							self.infohtml = self.infohtml + "\n <td align=center></td>"
						if l < filestemsize:
							self.infohtml = self.infohtml + "\n <td align=center><a href=\"plots/%s/%s.png\"><img width=200 height=140 src=\"plots/%s/%s.th.png\"></a></td>" % (self.id, self.filestem_array[l], self.id, self.filestem_array[l])
						else:
							self.infohtml = self.infohtml + "\n <td align=center></td>"
				if viewtype == "plots": self.infohtml = self.infohtml + "\n <td align=left>%s</td>\n <td align=center>%s</td>" % (self.redlocation, self.statusline.replace("-", "&#8211;"))
				if viewtype == "mega":
					self.infohtml = self.infohtml + "\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=left>%s</td>\n <td align=center>%s</td>" % (self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.stations_html, self.redlocation, self.statusline.replace("-", "&#8211;"))
			else:
				self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.statusline.replace("-", "&#8211;"), self.oi.pointing, self.oi.source)
		else:
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=left>%s</td>" % (self.id, self.colspan, self.comment,)





# Class with functions to write Html obs list
class writeHtmlList:
	def __init__(self, file, stem, fd, td):
		self.htmlfile = file
		self.linkedhtmlstem = stem
		self.fd = fd
		self.td = td
		self.lupd = ""

	def reInit (self, file):
		self.htmlfile = file

	def open(self):
		self.htmlptr = open(self.htmlfile, 'w')
		self.htmlptr.write ("<html>\n\
                	         <head>\n\
                                  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n\
                          	  <meta name=\"Classification\" content=\"public HTML\">\n\
                                  <meta name=\"robots\" content=\"noindex, nofollow\">\n\
				  <base href=\"http://www.astron.nl/~kondratiev/lofar/\" />\n\
                          	  <title>LOFAR pulsar observations</title>\n\
                         	</head>\n\n\
                         	<style type='text/css'>\n\
                          	 tr.d0 td { background-color: #ccffff; color: black; font-size: 80% }\n\
                          	 tr.d1 td { background-color: #99cccc; color: black; font-size: 80% }\n\
                          	 tr.d th { background-color: #99cccc; color: black;}\n\
                         	</style>\n\n\
                         	<body bgcolor='white'>\n\
                          	<h2 align=left>LOFAR pulsar observations</h2>\n\
                        	\n")

	def obsnumber (self, storage_nodes, ndbnodes, nnodes):
		self.nodes_string = ", ".join(storage_nodes)
		self.htmlptr.write("Number of observations in db file: <b>%d</b><br>\n" % (ndbnodes, ))
		self.htmlptr.write("Number of new observations found in %s: <b>%d</b><br>\n" % (self.nodes_string, nnodes))

	def datesrange (self):
		if self.fd == "":
			if self.td != "":
				self.htmlptr.write ("List only observations till %s<br>\n" % (self.td, ))
		else:
			if self.td == "":
				self.htmlptr.write ("List only observations since %s<br>\n" % (self.fd, ))
			else:
				self.htmlptr.write ("List only observations since %s till %s<br>\n" % (self.fd, self.td))

	def header (self, viewtype, storage_nodes_string_html):
		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=3>\n")
		if viewtype == "brief":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=left>Location</th>\n</tr>\n")
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n <th align=left>Location</th>\n <th align=center>Status</th>\n</tr>\n")
		elif viewtype == "mega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n <th align=center>NodesList (lse)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center>Total (GB)</th>\n <th align=left style=\"white-space: nowrap;\">Stations</th>\n <th align=left>Location</th>\n <th align=center>Status</th>\n</tr>\n" % (storage_nodes_string_html,))
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>NodesList (lse)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center>Total (GB)</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Status</th>\n <th align=center>Pointing</th>\n <th align=center>Source</th>\n</tr>\n" % (storage_nodes_string_html,))

	def linkedheader (self, viewtype, storage_nodes_string_html):
		sf=["-obsid.html", "-time.html", "-size.html", "-source.html"]
		sf=["%s%s" % (self.linkedhtmlstem.split("/")[-1], i) for i in sf]
		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=3>\n")
		if viewtype == "brief":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=left>Location</th>\n</tr>\n" % (sf[0], sf[3], sf[1]))
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n <th align=left>Location</th>\n <th align=center>Status</th>\n</tr>\n" % (sf[0], sf[3], sf[1]))
		elif viewtype == "mega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n <th align=center>NodesList (lse)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=left style=\"white-space: nowrap;\">Stations</th>\n <th align=left>Location</th>\n <th align=center>Status</th>\n</tr>\n" % (sf[0], sf[3], sf[1], storage_nodes_string_html, sf[2]))
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>NodesList (lse)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Status</th>\n <th align=center><a href=\"%s\">Pointing</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n</tr>\n" % (sf[0], sf[1], storage_nodes_string_html, sf[2], sf[3], sf[3]))

	def record (self, lineclass, index, line):
		self.htmlptr.write ("\n<tr class='%s' align=left>\n <td>%d</td>\n %s\n</tr>" % (lineclass, index, line))

	def legend (self):
		# getting date & time of last update
		if self.lupd == "":
			cmd="date +'%b %d, %Y %H:%M:%S'"
			self.lupd=os.popen(cmd).readlines()[0][:-1]
		self.htmlptr.write ("\n</table>\n\n<hr width=100%%>\n<address>\nLast Updated: %s\n</address>\n" % (self.lupd, ))
		self.htmlptr.write ("\n</body>\n</html>")

	def close (self):
		self.htmlptr.close()





# help
def usage (prg):
        """ Prints info how to use the script.
        """
        print "Program %s lists info about observations" % (prg, )
	print "Usage: %s [options]\n\
          -f, --from <date>          - list obs only _since_ <date> (inclusive), <date> in format YYYY-MM-DD\n\
          -t, --to <date>            - list obs only _till_ <date> (inclusive), <date> in format YYYY-MM-DD\n\
          --sort <mode>              - sort obs list. Default list is sorted by ObsID. Possible <mode>\n\
                                       is \"time\" to sort by start obs time, \"size\" to sort by total\n\
                                       disk space occupied by _raw_ data, \"source\" to sort by\n\
                                       the pointing coords of the source, and \"obsid\" is to sort by ObsID (default)\n\
                                       All sorting modes are: \"time\", \"size\", \"source\", \"obsid\"\n\
          --lse <lsenodes>           - set lse nodes to search for raw and processed data. Default are lse\n\
                                       nodes in sub5 and sub6, i.e. from lse013 to lse018, or <lsenodes> = 13-18\n\
                                       <lsenodes> should not have any spaces, nodes are specified just by number\n\
                                       without 'lse' prefix; comma and hyphen are allowed to list nodes and their ranges\n\
          --html <file>              - also write the list of obs in html-format to the file <file>\n\
          --linkedhtml <filestem>    - several lists of obs in html format will be created for for all sorted modes,\n\
                                       with basename <filestem>, and they will be linked together\n\
          -v, --view <mode>          - set the format of output data, namely the type of info to be listed.\n\
                                       Possible <mode> are \"usual\" (default) to list the data volume in every specified\n\
                                       lse nodes, total disk volume of raw data, datadir, if data reduced or not\n\
                                       in addition to start time, duration, source, etc. fiels that present in all formats.\n\
                                       Second <mode> is \"brief\" that lists antenna array, band filter, number of stations\n\
                                       used. Third <mode> is \"plots\" which is the same as \"brief\" mode in ascii output,\n\
                                       but in html-format it also provides the profiles (if existed) for RSP0 split and\n\
                                       in the full band (RSPA) together with chi-squared values of profiles.\n\
                                       Fourth <mode> is \"mega\" which is huge table like for \"plots\" but also with\n\
                                       with all other info from \"usual\" mode together with list of all stations\n\
                                       All view modes are: \"usual\" (default), \"brief\", \"plots\", \"mega\"\n\
          -r, --rebuild              - reprocess all observations from scratch (can take a while) rather than to read\n\
                                       the existent database, process obs that do not exist there, and add them to the database\n\
          -u, --update               - update db file only, new observations in /data? won't be added\n\
                                       This option can be used together with --from and --to to update only some observations\n\
          --stats                    - to calculate the statistics of existent observations in the database\n\
                                       can be used together with --from and --to options\n\
          --norsync                  - don't rsync plots to external webserver when \"mega\" or \"plots\" view mode is used\n\
          --dbfile <dbfile>          - database file with stored info about the observations\n\
          --debug                    - debug mode\n\
          -h, --help                 - print this message\n" % (prg, )




# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """
	try:
		opts, args = getopt.getopt (argv, "hf:t:v:ru", ["help", "sort=", "from=", "html=", "to=", "lse=", "view=", "linkedhtml=", "rebuild", "update", "debug", "stats", "dbfile=", "norsync"])
		for opt, arg in opts:
			if opt in ("-h", "--help"):
				usage(prg)
				sys.exit()
			if opt in ("--sort"):
				global sortkind
				sortkind = arg
				if sortkind != "time" and sortkind != "size" and sortkind != "source" and sortkind != "obsid":
					print "Arg for sort option should either be 'time' or 'size' or 'source' or 'obsid'\n"
					sys.exit()
			if opt in ("--html"):
				global is_html
				is_html = True
				global htmlfile
				htmlfile = arg
			if opt in ("--linkedhtml"):
				global is_linkedhtml
				is_linkedhtml = True
				global linkedhtmlstem
				linkedhtmlstem = arg
			if opt in ("--lse"):
				if arg.isspace() == True:
					print "--lse option has spaces that is not allowed"
					sys.exit()
				lsenodes = []
				for s in arg.split(","):
					if s.count("-") == 0:
						lsenodes = np.append(lsenodes, "lse%03d" % (int(s),))
					else:
						for l in np.arange(int(s.split("-")[0]), int(s.split("-")[1])+1):
							lsenodes = np.append(lsenodes, "lse%03d" % (l,))	
				global storage_nodes
				storage_nodes = lsenodes
			if opt in ("-f", "--from"):
				global is_from
				is_from = True
				global fromdate
				fromdate = arg
			if opt in ("-t", "--to"):
				global is_to
				is_to = True
				global todate
				todate = arg
			if opt in ("-v", "--view"):
				global viewtype
				viewtype = arg
			if opt in ("-r", "--rebuild"):
				global is_rebuild
				is_rebuild = True
			if opt in ("-u", "--update"):
				global is_update
				is_update = True
			if opt in ("--debug"):
				global is_debug
				is_debug = True
			if opt in ("--stats"):
				global is_stats
				is_stats = True
			if opt in ("--dbfile"):
				global dumpfile
				dumpfile = arg
			if opt in ("--norsync"):
				global is_torsync
				is_torsync = False

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

###################################################################################################################
#          M A I N                                                                                                #
###################################################################################################################
if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0].split("/")[-1], sys.argv[1:])

	if is_rebuild == True and is_update == True:
		print "Choose what you want to do: rebuild, update or add new observations if any"
		sys.exit()

	# table with obs info
	# key is the ObsId, and value is outputInfo class instance
	obstable={}

	# creating plotsdir directory if it does not exist
	cmd="mkdir -p %s" % (plotsdir, )
	os.system(cmd)

	# just make sure that rebuild and update switches are off
	if is_stats:
		is_rebuild = False
		is_update = False	

	if not is_rebuild:
		if not os.path.exists(dumpfile):
			print "Dumpfile \'%s\' does not exist! Use -r option to rebuild the database." % (dumpfile, )
			sys.exit()
		else:
			dfdescr = open(dumpfile, "r")
			obstable=cPickle.load(dfdescr)
			dfdescr.close()
			# update info and infohtml depending on current command line options and make the sorted list of Db obsids
			dbobsids = np.flipud(np.sort(obstable.keys(), kind='mergesort'))
			for r in dbobsids:
				obstable[r].update(storage_nodes)
			#
			# calculate statistics
			#
			if is_stats:
				obsids = dbobsids
				# we also have to choose only those IDs within the desired time range
				# if --from and/or --to are specified
				if is_from == True:
					fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) >= fromsecs, obsids))
				if is_to == True:
					tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d"))
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) <= tosecs, obsids))

				print
				print "Current pulsar obs statistics:"
				if is_from == True or is_to == True:
					print "[%s%s]" % (is_from and "from " + fromdate or (is_to and " till " + todate or ""), 
                                                                              is_to and (is_from and " till " + todate or "") or "")
				print "---------------------------------------------------------------------------"
				totDuration = 0.0
				processedDuration = 0.0
				IMonlyDuration = 0.0
				Nprocessed = 0
				Nistype = 0
				Nistype_only = 0
				Ncstype = 0
				Ncstype_only = 0
				Nfetype = 0
				Nfetype_only = 0
				Nimtype = 0
				Nimtype_only = 0
				Nbftype = 0
				Nbftype_only = 0
				Nfdtype = 0
				Nfdtype_only = 0
				Niscsim = 0
				Nisim = 0
				Niscs = 0
				Ncsim = 0
				Ncsfe = 0
				Nimfe = 0
				Nisfe = 0
				Niscsfe = 0
				Nbfis = 0
				Nbffe = 0
				Nbfisfe = 0
				Nbfiscsfe = 0
				totRawsize = 0.0   # size in TB of raw data
				IMonlyRawsize = 0.0 # size in TB of IM only raw data
				totProcessedsize = 0.0   # size in TB of processed data

				for r in obsids:
					# getting the numbers and duration
					if obstable[r].comment == "" and obstable[r].oi.duration != "?":
						totDuration += obstable[r].oi.dur	
					if obstable[r].comment == "" and obstable[r].statusline != "x":
						Nprocessed += 1
						if obstable[r].oi.duration != "?":
							processedDuration += obstable[r].oi.dur
					# getting the number of obs of different type
					if obstable[r].comment == "" and obstable[r].oi.istype == "+":
						Nistype += 1
						if obstable[r].oi.cstype != "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.bftype != "+":
							Nistype_only += 1
					if obstable[r].comment == "" and obstable[r].oi.cstype == "+":
						Ncstype += 1
						if obstable[r].oi.istype != "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.bftype != "+":
							Ncstype_only += 1
					if obstable[r].comment == "" and obstable[r].oi.fetype == "+":
						Nfetype += 1
						if obstable[r].oi.istype != "+" and obstable[r].oi.cstype != "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.bftype != "+":
							Nfetype_only += 1
					if obstable[r].comment == "" and obstable[r].oi.imtype == "+":
						Nimtype += 1
						if obstable[r].oi.cstype != "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.istype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.bftype != "+":
							Nimtype_only += 1
							IMonlyRawsize += float(obstable[r].totsize)
							if obstable[r].oi.duration != "?":
								IMonlyDuration += obstable[r].oi.dur
					if obstable[r].comment == "" and obstable[r].oi.bftype == "+":
						Nbftype += 1
						if obstable[r].oi.cstype != "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.istype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.imtype != "+":
							Nbftype_only += 1
					if obstable[r].comment == "" and obstable[r].oi.fdtype == "+":
						Nfdtype += 1
						if obstable[r].oi.cstype != "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.istype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.imtype != "+":
							Nfdtype_only += 1
					# getting the number of some observing types' mixtures
					if obstable[r].comment == "" and obstable[r].oi.istype == "+" and obstable[r].oi.cstype == "+" and obstable[r].oi.imtype == "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+":
						Niscsim += 1
					if obstable[r].comment == "" and obstable[r].oi.istype == "+" and obstable[r].oi.imtype == "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.cstype != "+":
						Nisim += 1
					if obstable[r].comment == "" and obstable[r].oi.istype == "+" and obstable[r].oi.cstype == "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.imtype != "+":
						Niscs += 1
					if obstable[r].comment == "" and obstable[r].oi.cstype == "+" and obstable[r].oi.imtype == "+" and obstable[r].oi.fetype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.istype != "+":
						Ncsim += 1
					if obstable[r].comment == "" and obstable[r].oi.cstype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.istype != "+":
						Ncsfe += 1
					if obstable[r].comment == "" and obstable[r].oi.istype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.cstype != "+":
						Nisfe += 1
					if obstable[r].comment == "" and obstable[r].oi.imtype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.istype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.cstype != "+":
						Nimfe += 1
					if obstable[r].comment == "" and obstable[r].oi.istype == "+" and obstable[r].oi.cstype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.bftype != "+" and obstable[r].oi.fdtype != "+":
						Niscsfe += 1
					if obstable[r].comment == "" and obstable[r].oi.bftype == "+" and obstable[r].oi.istype == "+" and obstable[r].oi.imtype != "+" and obstable[r].oi.cstype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.fetype != "+":
						Nbfis += 1
					if obstable[r].comment == "" and obstable[r].oi.bftype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.istype != "+" and obstable[r].oi.cstype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.imtype != "+":
						Nbffe += 1
					if obstable[r].comment == "" and obstable[r].oi.bftype == "+" and obstable[r].oi.istype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.cstype != "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.imtype != "+":
						Nbfisfe += 1
					if obstable[r].comment == "" and obstable[r].oi.bftype == "+" and obstable[r].oi.istype == "+" and obstable[r].oi.fetype == "+" and obstable[r].oi.cstype == "+" and obstable[r].oi.fdtype != "+" and obstable[r].oi.imtype != "+":
						Nbfiscsfe += 1
					# getting the sizes
					if obstable[r].comment == "":
						totRawsize += float(obstable[r].totsize)
					if obstable[r].comment == "":
						totProcessedsize += float(obstable[r].processed_dirsize)

				totDuration /= 3600.
				processedDuration /= 3600.
				IMonlyDuration /= 3600.
				totRawsize /= 1024.
				totProcessedsize /= 1024.
				IMonlyRawsize /= 1024.

				print "Total number of observations [hours / days]:         %d [%.1f / %.1f]" % (np.size(obsids),totDuration,totDuration/24.)
				print "Number of observations w/o IM only [hours / days]:   %d [%.1f / %.1f]" % (np.size(obsids)-Nimtype_only, totDuration-IMonlyDuration, (totDuration-IMonlyDuration)/24.)
				print "Number of processed observations [hours / days]:     %d [%.1f / %.1f]" % (Nprocessed,processedDuration,processedDuration/24.)
				print
				print "Number of IS observations [only IS]:       %d [%d]" % (Nistype, Nistype_only)
				print "Number of IS+CS observations only:         %d" % (Niscs,)
				print "Number of IS+IM observations only:         %d" % (Nisim,)
				print "Number of IS+CS+IM observations only:      %d" % (Niscsim,)
				print "Number of CS observations [only CS]:       %d [%d]" % (Ncstype, Ncstype_only)
				print "Number of CS+IM observations only:         %d" % (Ncsim,)
				print "Number of FE observations [only FE]:       %d [%d]" % (Nfetype, Nfetype_only)
				print "Number of FE+CS observations only:         %d" % (Ncsfe,)
				print "Number of FE+IS observations only:         %d" % (Nisfe,)
				print "Number of FE+IM observations only:         %d" % (Nimfe,)
				print "Number of FE+IS+CS observations only:      %d" % (Niscsfe,)
				print "Number of IM observations [only IM]:       %d [%d]" % (Nimtype, Nimtype_only)
				print "Number of BF observations [only BF]:       %d [%d]" % (Nbftype, Nbftype_only)
				print "Number of BF+IS observations only:         %d" % (Nbfis,)
				print "Number of BF+FE observations only:         %d" % (Nbffe,)
				print "Number of BF+IS+FE observations only:      %d" % (Nbfisfe,)
				print "Number of BF+IS+CS+FE observations only:   %d" % (Nbfiscsfe,)
				print "Number of FD observations [only FD]:       %d [%d]" % (Nfdtype, Nfdtype_only)
				print
				print "Total size of raw data (TB):               %.1f" % (totRawsize,)
				print "Total size of raw data w/o IM-only (TB):   %.1f" % (totRawsize-IMonlyRawsize,)
				print "Total size of processed data (TB):         %.1f" % (totProcessedsize,)
				print
				sys.exit(0)
				#
				########## end of statistics #############
				# 

	if not is_update:
		# loop over the storage nodes and directories to get the list of all IDs
		for s in storage_nodes:
			for d in data_dirs:
				cmd="cexec %s 'find %s -maxdepth 1 -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[s], d, "?20??_*", cexec_egrep_string)
				indlist=[i.split("/")[-1][:-1] for i in os.popen(cmd).readlines()]
				obsids = np.append(obsids, indlist)

		# also checking the archive directories to extend the list of ObsIDs in case the raw data was removed
		for s in storage_nodes:
			cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | %s" % (cexec_nodes[s], "/data4/LOFAR_PULSAR_ARCHIVE_" + s, cexec_egrep_string)
			if np.size(os.popen(cmd).readlines()) == 0:
				continue
			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[s], "/data4/LOFAR_PULSAR_ARCHIVE_" + s, "?20??_*_red", cexec_egrep_string)
			indlist=[i.split("/")[-1].split("_red")[0] for i in os.popen(cmd).readlines()]
			obsids = np.append(obsids, indlist)

		# getting the unique list of IDs (some of IDs can have entries in many /data? and nodes)
		# and sort in reverse order (most recent obs go first)
		# more recent obs is the obs with higher ID (as it should be)
		obsids = np.flipud(np.sort(np.unique(obsids), kind='mergesort'))

		# Number of ObsIDs
		Nobsids = np.size(obsids)

		# if is_rebuild == False then excluding ObsIDs from obsids list that are already in the database, i.e. in dbobsids list
		# only new ObsIDs will be processed and added to database
		if not is_rebuild:
			# now obsids have only those IDs that are not in the dump file
			obsids=list(set(obsids)-set(obsids).intersection(set(dbobsids)))
	else:   ## --update is set
		obsids = dbobsids
		# for the db update we also have to choose only those IDs within the desired time range
		# if --from and/or --to are specified
		if is_from == True:
			fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
			obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) >= fromsecs, obsids))

		if is_to == True:
			tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d"))
			obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) <= tosecs, obsids))
		# Number of ObsIDs
		Nobsids = np.size(obsids)


	if is_rebuild == True:
		print "Number of observations in %s: %d" % (", ".join(storage_nodes), Nobsids)
	else:
		print "Number of observations in db file: %d" % (np.size(dbobsids), )
		if not is_update:
			print "Number of new observations found in %s: %d" % (", ".join(storage_nodes), np.size(obsids))
		

	if is_from == True or is_to == True:
		print "List only observations%s%s" % (is_from and " since " + fromdate or (is_to and " till " + todate or ""), 
                                                      is_to and (is_from and " till " + todate or "") or "")
	print

	# number of storage nodes
	Nnodes=np.size(storage_nodes)

	# printing out the header of the table
	storage_nodes_string=""
	for i in np.arange(Nnodes-1):
		storage_nodes_string=storage_nodes_string+storage_nodes[i]+"\t"
	storage_nodes_string=storage_nodes_string+storage_nodes[-1]
	storage_nodes_string_html="</th>\n <th align=center>".join(storage_nodes_string.split("\t"))

	if viewtype == "brief" or viewtype == "plots":
		equalstrs=[]
		equalstring_size=163
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		MMDD	Dur	Ant	Band	   #Stations	    BF FD IM IS CS FE	Location		Status"
		print equalstring
	elif viewtype == "mega":
		equalstrs=[]
		equalstring_size=233+8*Nnodes
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		MMDD	Dur	Ant	Band	   #Stations	    BF FD IM IS CS FE	NodesList (lse) Datadir	%s	Total(GB)	Stations		Location                Status" % (storage_nodes_string,)
		print equalstring
	else: # usual
		equalstrs=[]
		equalstring_size=159+8*Nnodes
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		MMDD	Dur	NodesList (lse)	Datadir	%s	Total(GB)	BF FD IM IS CS FE	Status				Pointing    Source" % (storage_nodes_string,)
		print equalstring
		


	# loop for every observation
	for id in obsids:
	
		# class instance with output Info
		out=outputInfo(id)	

		# prefix of ID, like L2010 or L2009
        	id_prefix=id.split("_")[0]   
		# suffix of ID, the sequence number of observation
        	id_suffix=id.split("_")[1]   

		#
		# checking first if the directory with the parset file exists
		#
		# There were several changes in the location of the parset files and its name, so we have to
		# check all of them. There were:
		# (0) OLDEST parset file: /globalhome/lofarsystem/oldlog/id/RTCP.parset.0   <-- parset_oldlogdir + id + parset  
		# (1) OLD parset was here: /globalhome/lofarsystem/log/id/RTCP.parset.0     <-- parset_logdir + id + parset
		# (2) NEW parset as of May 10, 2010 is here: /globalhome/lofarsystem/log/L2010-MM-DD-DATE/RTCP-ID.parset
		# (3) 2nd transpose parset as of Aug 20, 2010 is here: 
		#          /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L2010-MM-DD-DATE/RTCP-ID.parset
		#
		logdir=parset_logdir + id + "/"
		if not os.path.exists(logdir):
			# checking in the oldlog directory
			logdir=parset_oldlogdir + id + "/"
			if not os.path.exists(logdir):
				# Due to new naming convention and location of the parset files, also looking for the parset file
				# in any L2010-??-??_?????? directories	
				cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_logdir, id_suffix)
				logdir=os.popen(cmd).readlines()
				if np.size(logdir) > 0:
					# it means we found the directory with parset file
					logdir=os.popen("dirname %s" % (logdir[0][:-1],)).readlines()[0][:-1]
					logdir = logdir + "/"
				else:
					# now checking the new parset directory
					cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_newlogdir, id_suffix)
					logdir=os.popen(cmd).readlines()
					if np.size(logdir) > 0:
						# it means we found the directory with parset file
						logdir=os.popen("dirname %s" % (logdir[0][:-1],)).readlines()[0][:-1]
						logdir = logdir + "/"
					else:
						# no directory found
						comment = "Oops!.. The log directory or parset file in new naming convention does not exist!"
						out.setcomment(id, storage_nodes, comment)
						obstable[id] = out
						continue

		# get the full path for the parset file for the current ID
		log=logdir + parset
		if not os.path.exists(log):
			# also checking that maybe the parset file has name the same as Obs ID
			log=logdir + id + ".parset"
			if not os.path.exists(log):
				# also checking that maybe the name of parset file has new naming convention, like "RTCP-<id_suffix>.parset"
				# OR like L<id_suffix>.parset
				log=logdir + "RTCP-" + id_suffix + ".parset"
				if not os.path.exists(log):
					log=logdir + "L" + id_suffix + ".parset"
					if not os.path.exists(log):
						comment = "Oops!.. The parset file '%s' does not exist in any possible location!" % (parset,)
						out.setcomment(id, storage_nodes, comment)
						obstable[id] = out
						continue

		# initializing the obsinfo class
		oi=obsinfo(log)

		# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
		dirsizes = {}
		for lse in storage_nodes:
			ddir=oi.datadir + "/" + id
			dirsizes[lse] = ["x", "0.0"]
			cmd="cexec %s 'du -sh %s 2>/dev/null | cut -f 1' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], ddir, cexec_egrep_string)
			dirout=os.popen(cmd).readlines()
			if np.size(dirout) > 0:
				dirsizes[lse][0]=dirout[0][:-1]
				cmd="cexec %s 'du -s -B 1 %s 2>/dev/null | cut -f 1' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], ddir, cexec_egrep_string)
				status=os.popen(cmd).readlines()
				if np.size(status) > 0:
					status=status[0][:-1]
					if status.isdigit() == True:
							dirsizes[lse][1] = status

		# checking if this specific observation was already reduced. Checking for both existence of the *_red directory
		# in LOFAR_PULSAR_ARCHIVE and the existence of *_plots.tar.gz file
		statusline="x"
		redlocation="x"
		processed_dirsize=0.0
		for lse in storage_nodes:
			cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, cexec_egrep_string)
			if np.size(os.popen(cmd).readlines()) == 0:
				continue
			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, id + "_red", cexec_egrep_string)
			redout=os.popen(cmd).readlines()
			if np.size(redout) > 0:
				reddir=redout[0][:-1]
				statusline=lse
				redlocation="%s/%s/%s%s" % ("/net", cexec_nodes[lse].split(":")[0], lse, reddir)
				# getting the size of the processed data
				cmd="cexec %s 'du -s -B 1 %s 2>/dev/null | cut -f 1' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], reddir, cexec_egrep_string)
				status=os.popen(cmd).readlines()
				if np.size(status) > 0:
					status=status[0][:-1]
					if status.isdigit() == True:
							processed_dirsize = float(status) / 1024. / 1024. / 1024.
				# checking if final tar.gz file exists
				cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir, "*_plots.tar.gz", cexec_egrep_string)
				if np.size(os.popen(cmd).readlines()) > 0:
					# tarfile exists
					statusline=statusline+" +tar"	
				else:
					statusline=statusline+" -tar"
				# checking if RSPA exists
				cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", cexec_egrep_string)
                                if np.size(os.popen(cmd).readlines()) > 0:
					statusline=statusline+" +all"
				else:
					statusline=statusline+" -all"
				# checking if rfirep file exists in RSP0
				cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", "*.rfirep", cexec_egrep_string)
                                if np.size(os.popen(cmd).readlines()) > 0:
					statusline=statusline+" +rfi"
				else:
					statusline=statusline+" -rfi"
				# checking if summary rfirep file exists
				cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes", "*.rfirep", cexec_egrep_string)
                                if np.size(os.popen(cmd).readlines()) > 0:
					statusline=statusline+" +rfiA"
				else:
					statusline=statusline+" -rfiA"
				break


		# Collecting info about chi-squared and profile png-files
		profiles_array=[]
		chi_array=[]

		for lse in storage_nodes:
			cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, cexec_egrep_string)
			if np.size(os.popen(cmd).readlines()) == 0:
				continue
			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, id + "_red", cexec_egrep_string)
			redout=os.popen(cmd).readlines()
			if np.size(redout) > 0:
				reddir=redout[0][:-1]
				# getting first the name of the main pulsar (first in the command-line option)
				cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir, "*.log", cexec_egrep_string)
				mainlog=os.popen(cmd).readlines()
				mainpsr="undefined"
				if np.size(mainlog) > 0:
					cmd="cexec %s 'cat %s 2>/dev/null | grep id | head -n 1' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], mainlog[0][:-1], cexec_egrep_string)
					cmdline=os.popen(cmd).readlines()
					if np.size(cmdline) > 0:
						param=cmdline[0][:-1].split(" ")
						for k in np.arange(len(param)):
        						if param[k] == "-p":
                						mainpsr=param[k+1].split(",")[0]
                						break
				# RSP0
				cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", cexec_egrep_string)
				if np.size(os.popen(cmd).readlines()) > 0:
					if mainpsr == "undefined":
						cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", "*.pfd*png", cexec_egrep_string)
					else:
						cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", mainpsr + "*.pfd*png", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# copying png files to local directory
						cmd="mkdir -p %s/%s ; cexec %s 'cp -f %s %s/%s' 2>&1 1>/dev/null" % (plotsdir, id, cexec_nodes[lse], " ".join([ss[:-1] for ss in status]), plotsdir, id)
						os.system(cmd)
						profiles_array = np.append(profiles_array, status[0].split("/")[-1].split(".pfd")[0] + ".pfd")
					# getting chi-squared
					if mainpsr == "undefined":
						cmd="cexec %s 'find %s -name \"%s\" -print -exec cat {} \; 2>/dev/null | grep chi-squared' 2>/dev/null | grep -v Permission | grep -v such | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", "*.prepout", cexec_egrep_string)
					else:
						cmd="cexec %s 'find %s -name \"%s\" -print -exec cat {} \; 2>/dev/null | grep chi-squared' 2>/dev/null | grep -v Permission | grep -v such | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", mainpsr + "*.prepout", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						chi_array = np.append(chi_array, status[0][:-1])
				# RSPA
				cmd="cexec %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", cexec_egrep_string)
				if np.size(os.popen(cmd).readlines()) > 0:
					if mainpsr == "undefined":
						cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", "*.pfd*png", cexec_egrep_string)
					else:
						cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", mainpsr + "*.pfd*png", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# copying png files to local directory
						cmd="mkdir -p %s/%s ; cexec %s 'cp -f %s %s/%s' 2>&1 1>/dev/null" % (plotsdir, id, cexec_nodes[lse], " ".join([ss[:-1] for ss in status]), plotsdir, id)
						os.system(cmd)
						profiles_array = np.append(profiles_array, status[0].split("/")[-1].split(".pfd")[0] + ".pfd")
					# getting chi-squared
					if mainpsr == "undefined":
						cmd="cexec %s 'find %s -name \"%s\" -print -exec cat {} \; 2>/dev/null | grep chi-squared' 2>/dev/null | grep -v Permission | grep -v such | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", "*.prepout", cexec_egrep_string)
					else:
						cmd="cexec %s 'find %s -name \"%s\" -print -exec cat {} \; 2>/dev/null | grep chi-squared' 2>/dev/null | grep -v Permission | grep -v such | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", mainpsr + "*.prepout", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						chi_array = np.append(chi_array, status[0][:-1])
				break

		# combining info
		out.Init(id, oi, storage_nodes, dirsizes, statusline, redlocation, processed_dirsize, "", profiles_array, chi_array)
		obstable[id] = out
		# printing the info line by line in debug mode
		if is_debug:
			print "%d       %s" % (debugcounter, out.info)
			cmd="echo \"%d (%d)\" > %s" % (debugcounter, Nobsids, debugfile)
			os.system(cmd)
			debugcounter += 1


	# dump obs table to the file
	dfdescr = open (dumpfile, "w")
	cPickle.dump(obstable, dfdescr, True)
	dfdescr.close()

	# uploading the png files to webserver
	if is_torsync:
		if viewtype == 'plots' or viewtype == 'mega':
			cmd="ssh %s mkdir -p %s" % (webserver, webplotsdir)	
			os.system(cmd)
			cmd="rsync -a %s/ %s:%s 2>&1 1>/dev/null" % (plotsdir, webserver, webplotsdir)
			os.system(cmd)

	# copying to another list to keep the old one
	obskeys = np.flipud(np.sort(obstable.keys(), kind='mergesort'))

	# if is_from and/or is_to are set, then we have to exclude those records
	# from obstable that do not obey the conditions
	if is_from == True:
		fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
		obskeys=list(np.compress(np.array([obstable[r].seconds for r in obskeys]) >= fromsecs, obskeys))

	if is_to == True:
		tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d"))
		obskeys=list(np.compress(np.array([obstable[r].seconds for r in obskeys]) <= tosecs, obskeys))

	# printing the sorted list
	if sortkind == "size":
		sorted_indices=np.flipud(np.argsort([float(obstable[r].totsize) for r in obskeys], kind='mergesort'))
	elif sortkind == "time":
		sorted_indices=np.flipud(np.argsort([obstable[r].seconds for r in obskeys], kind='mergesort'))
	# sorting by source (pointing coords)
	elif sortkind == "source":
		sorted_indices=np.argsort([obstable[r].pointing for r in obskeys], kind='mergesort')
	# unsorted (i.e. by default sorted by ObsId)
	else:
		sorted_indices=np.arange(np.size(obskeys))

	counter=0
	for i in sorted_indices:
		print "%d	%s" % (counter, obstable[obskeys[i]].info)
		counter += 1


	# writing the html code if chosen
	if is_html == True:
		htmlrep=writeHtmlList(htmlfile, linkedhtmlstem, fromdate, todate)
		htmlrep.open()
		htmlrep.obsnumber(storage_nodes, np.size(dbobsids), np.size(obsids))
		htmlrep.datesrange()
		htmlrep.header(viewtype, storage_nodes_string_html)
		counter = 0
		for i in sorted_indices:
			htmlrep.record(counter%2 == 0 and "d0" or "d1", counter, obstable[obskeys[i]].infohtml)
			counter += 1
		htmlrep.legend()
		htmlrep.close()

	# create html files sorted for different sorting modes, linked together
	if is_linkedhtml == True:
		sf={"obsid": "-obsid.html", "time": "-time.html", "size": "-size.html", "source": "-source.html"}
		for key in sf.keys():
			sf[key] = "%s%s" % (linkedhtmlstem, sf[key])
		htmlrep=writeHtmlList(sf["obsid"], linkedhtmlstem, fromdate, todate)
		for key in sf.keys():
			htmlrep.reInit(sf[key])
			htmlrep.open()
			htmlrep.obsnumber(storage_nodes, np.size(dbobsids), np.size(obsids))
			htmlrep.datesrange()
			htmlrep.linkedheader(viewtype, storage_nodes_string_html)
			if key == "size":
				sorted_indices=np.flipud(np.argsort([float(obstable[r].totsize) for r in obskeys], kind='mergesort'))
			elif key == "time":
				sorted_indices=np.flipud(np.argsort([obstable[r].seconds for r in obskeys], kind='mergesort'))
			elif key == "source":
				sorted_indices=np.argsort([obstable[r].pointing for r in obskeys], kind='mergesort')
			# unsorted (i.e. by default sorted by ObsId)
			else:
				sorted_indices=np.arange(np.size(obskeys))
			counter = 0
			for i in sorted_indices:
				htmlrep.record(counter%2 == 0 and "d0" or "d1", counter, obstable[obskeys[i]].infohtml)
				counter += 1
			htmlrep.legend()
			htmlrep.close()

	if is_debug:
		cmd="rm -f %s" % (debugfile,) 
		os.system(cmd)
