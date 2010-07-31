#!/usr/bin/env python
#
import os, sys, glob
import getopt
import numpy as np
import time

# True if we want the output list to be sorted by the TotalSize
tosort=False
sortkind=""  # three kinds of sorting currently: by start time ("time"), by volume ("size"), and by source pointing ("source")

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

# View of presenting info (usual (defaul), brief, and plots)
viewtype="usual"
# where to copy profile plots (dop95)
webserver="kondratiev@10.87.2.95"
plotsdir="/home/kondratiev/Lofar/plots"
webplotsdir="public_html/lofar/plots"

atnflink_start="http://www.atnf.csiro.au/research/pulsar/psrcat/proc_form.php?startUserDefined=true&c1_val=&c2_val=&c3_val=&c4_val=&sort_attr=jname&sort_order=asc&condition=&pulsar_names="
atnflink_end="&ephemeris=long&submit_ephemeris=Get+Ephemeris&coords_unit=raj%2Fdecj&radius=&coords_1=&coords_2=&style=Long+with+last+digit+error&no_value=*&fsize=3&x_axis=&x_scale=linear&y_axis=&y_scale=linear&state=query"

# storage nodes to collect info about Pulsar Observations
# we assume that even for the case of long observations when data were spreaded out
# across many other nodes, these three "pulsar" sub5 nodes were used to record a part
# of the data as well
storage_nodes=["lse013", "lse014", "lse015"]
# list of directories withe the data
data_dirs=["/data1", "/data2", "/data3", "/data4"]

# cexec corresponding table
cexec_nodes={'lse013': 'sub5:9', 'lse014': 'sub5:10', 'lse015': 'sub5:11',
             'lse016': 'sub6:9', 'lse017': 'sub6:10', 'lse018': 'sub6:11'}
cexec_egrep_string="egrep -v \'\\*\\*\\*\\*\\*\' |egrep -v \'\\-\\-\\-\\-\\-\'"

# directories with parset files
parset_logdir="/globalhome/lofarsystem/log/"
parset_oldlogdir="/globalhome/lofarsystem/oldlog/"
# name of the parset file
parset="RTCP.parset.0"

# list of obs ids
obsids=[]


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
			stations_array = self.stations.split(",")
			self.nstations = np.size(stations_array)
			self.ncorestations = self.stations.count("CS")
			self.nremotestations = self.stations.count("RS")
			self.stations_string = "%d [%dCS, %dRS]" % (self.nstations, self.ncorestations, self.nremotestations) 
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
	
	def setcomment (self, id, cs, comment):
		self.id = id
		self.comment = comment
		self.seconds = 0
		self.totsize = 0
		self.pointing = "????_????"
		if viewtype == "brief":
			self.colspan = 12
		elif viewtype == "plots":
			self.colspan = 16
		else:
			self.colspan = 15 + cs - 1

		if self.comment != "":
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=center>%s</td>" % (self.id, self.colspan, self.comment,)

	def Init(self, id, oi, dirsize_string, totsize, statusline, comment, filestem_array, chi_array):
		self.id = id
		self.oi = oi
		self.seconds = self.oi.seconds
		self.pointing = self.oi.pointing
		self.dirsize_string = dirsize_string
		self.totsize = totsize
		self.statusline = statusline
		self.comment = comment
		if viewtype == "brief":
			self.colspan = 12
		elif viewtype == "plots":
			self.colspan = 16
		else:
			self.colspan = 15 + len(self.dirsize_string.split("\t")) - 1
		self.filestem_array = filestem_array
		self.chi_array = chi_array

		self.dirsize_string_html = "</td>\n <td align=center>".join(self.dirsize_string.split("\t")[:-1])
		
		if self.comment == "":
			if viewtype == "brief" or viewtype == "plots":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype)
			else:
				self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-11s	%s   %s" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)
			if viewtype == "brief":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, atnflink_start, self.oi.source.replace("+", "%2B"), atnflink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
			elif viewtype == "plots":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, atnflink_start, self.oi.source.replace("+", "%2B"), atnflink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
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
			else:
				self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)
		else:
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=center>%s</td>" % (self.id, self.colspan, self.comment,)

	def update(self):
		if viewtype == "brief":
			self.colspan = 12
		elif viewtype == "plots":
			self.colspan = 16
		else:
			self.colspan = 15 + len(self.dirsize_string.split("\t")) - 1

		if self.comment == "":
			if viewtype == "brief" or viewtype == "plots":
				self.info = "%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype)
			else:
				self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-11s	%s   %s" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.bftype, self.oi.fdtype, self.oi.imtype, self.oi.istype, self.oi.cstype, self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)

			if viewtype == "brief":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, atnflink_start, self.oi.source.replace("+", "%2B"), atnflink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
			elif viewtype == "plots":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s%s%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, atnflink_start, self.oi.source.replace("+", "%2B"), atnflink_end, self.oi.source, self.oi.datestring, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype)
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
			else:
				self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.bftype == "-" and "&#8211;" or self.oi.bftype, self.oi.fdtype == "-" and "&#8211;" or self.oi.fdtype, self.oi.imtype == "-" and "&#8211;" or self.oi.imtype, self.oi.istype == "-" and "&#8211;" or self.oi.istype, self.oi.cstype == "-" and "&#8211;" or self.oi.cstype, self.oi.fetype == "-" and "&#8211;" or self.oi.fetype, self.statusline, self.oi.pointing, self.oi.source)
		else:
			self.info = self.comment
			self.infohtml = "<td>%s</td>\n <td colspan=%d align=center>%s</td>" % (self.id, self.colspan, self.comment,)


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

	def obsnumber (self, storage_nodes, nnodes):
		self.nodes_string = ", ".join(storage_nodes)
		self.nnodes = nnodes
		self.htmlptr.write("Number of observations in %s: <b>%d</b><br>\n" % (self.nodes_string, self.nnodes))

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
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n</tr>\n")
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n</tr>\n")
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>MMDD</th>\n <th align=center>Duration</th>\n <th>NodesList (lse)</th>\n <th align=center>Datadir</th>\n <th align=center>%s</th>\n <th align=center>Total (GB)</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Reduced</th>\n <th align=center>Pointing</th>\n <th align=center>Source</th>\n</tr>\n" % (storage_nodes_string_html,))

	def linkedheader (self, viewtype, storage_nodes_string_html):
		sf=["-id.html", "-time.html", "-size.html", "-source.html"]
		sf=["%s%s" % (self.linkedhtmlstem, i) for i in sf]
		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=3>\n")
		if viewtype == "brief":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n</tr>\n" % (sf[0], sf[3], sf[1]))
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Chi-squared (RSP0)</th>\n <th align=center>Profile (RSP0)</th>\n <th align=center>Chi-squared (RSPA)</th>\n <th align=center>Profile (RSPA)</th>\n</tr>\n" % (sf[0], sf[3], sf[1]))
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">MMDD</a></th>\n <th align=center>Duration</th>\n <th>NodesList (lse)</th>\n <th align=center>Datadir</th>\n <th align=center>%s</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=center>BF</th>\n <th align=center>FD</th>\n <th align=center>IM</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>FE</th>\n <th align=center>Reduced</th>\n <th align=center><a href=\"%s\">Pointing</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n</tr>\n" % (sf[0], sf[1], storage_nodes_string_html, sf[2], sf[3], sf[3]))

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
	print "Usage: %s [-s, --sorted <mode>] [-f, --from <date>] [-t, --to <date>]\n\
                  [--html <file>] [--lse <lsenodes>] [-v, --view <mode>] [--linkedhtml <filestem>] [-h, --help]\n\
          -f, --from <date>          - list obs only _since_ <date> (inclusive), <date> in format YYYY-MM-DD\n\
          -t, --to <date>            - list obs only _till_ <date> (inclusive), <date> in format YYYY-MM-DD\n\
          -s, --sorted <mode>        - sort obs list. Default list is sorted by ObsID. Possible <mode>\n\
                                       is \"time\" to sort by start obs time, \"size\" to sort by total\n\
                                       disk space occupied by _raw_ data, and \"source\" is to sort by\n\
                                       the pointing coords of the source\n\
                                       All sorting modes are: \"time\", \"size\", \"source\", without this option\n\
                                       obs will be sorted by ObsId\n\
          --lse <lsenodes>           - set lse nodes to search for raw and processed data. Default are lse\n\
                                       nodes in sub5, i.e. lse013, lse014,lse015, or <lsenodes> = 13-15\n\
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
                                       All view modes are: \"usual\" (default), \"brief\", \"plots\"\n\
          -h, --help                 - print this message\n" % (prg, )

# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """
	try:
		opts, args = getopt.getopt (argv, "hs:f:t:v:", ["help", "sorted=", "from=", "html=", "to=", "lse=", "view=", "linkedhtml="])
		for opt, arg in opts:
			if opt in ("-h", "--help"):
				usage(prg)
				sys.exit()
			if opt in ("-s", "--sorted"):
				global tosort
				tosort = True
				global sortkind
				sortkind = arg
				if sortkind != "time" and sortkind != "size" and sortkind != "source":
					print "Arg for sort option should either be 'time' or 'size' or 'source'\n"
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
						lsenodes = np.append(lsenodes, "lse0" + s)
					else:
						for l in np.arange(int(s.split("-")[0]), int(s.split("-")[1])+1):
							lsenodes = np.append(lsenodes, "lse0" + str(l))	
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

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0].split("/")[-1], sys.argv[1:])

	# writing the html code if chosen
	if is_html == True:
		htmlrep=writeHtmlList(htmlfile, linkedhtmlstem, fromdate, todate)
		htmlrep.open()

	# loop over the storage nodes and directories to get the list of all IDs
	for s in storage_nodes:
		for d in data_dirs:
			cmd="cexec %s 'find %s -maxdepth 1 -type d -name \"%s\" -print 2>&1 | grep -v Permission' | %s" % (cexec_nodes[s], d, "?20??_*", cexec_egrep_string)
			indlist=[i.split("/")[-1][:-1] for i in os.popen(cmd).readlines()]
			obsids = np.append(obsids, indlist)

	# also checking the archive directories to extend the list of ObsIDs in case the raw data was removed
	for s in storage_nodes:
		cmd="cexec %s 'ls -d %s 2>/dev/null' | %s" % (cexec_nodes[s], "/data4/LOFAR_PULSAR_ARCHIVE_" + s, cexec_egrep_string)
		if np.size(os.popen(cmd).readlines()) == 0:
			continue
		cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>&1 | grep -v Permission' | %s" % (cexec_nodes[s], "/data4/LOFAR_PULSAR_ARCHIVE_" + s, "?20??_*_red", cexec_egrep_string)
		indlist=[i.split("/")[-1].split("_red")[0] for i in os.popen(cmd).readlines()]
		obsids = np.append(obsids, indlist)

	# number of storage nodes
	Nnodes=np.size(storage_nodes)

	# getting the unique list of IDs (some of IDs can have entries in many /data? and nodes)
	# and sort in reverse order (most recent obs go first)
	# more recent obs is the obs with higher ID (as it should be)
	obsids = np.flipud(np.sort(np.unique(obsids), kind='mergesort'))

	if is_html == True:
		htmlrep.obsnumber(storage_nodes, np.size(obsids))
		htmlrep.datesrange()
	print "Number of observations in %s: %d" % (", ".join(storage_nodes), np.size(obsids), )

	if is_from == True and is_to == True:
		print "List only observations since %s till %s" % (fromdate, todate)
		fromyear = fromdate.split("-")[0]
		fromdate = time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
		toyear = todate.split("-")[0]
		todate = time.mktime(time.strptime(todate, "%Y-%m-%d"))

	if is_from == True and is_to == False:
		print "List only observations since %s" % (fromdate, )
		fromyear = fromdate.split("-")[0]
		fromdate = time.mktime(time.strptime(fromdate, "%Y-%m-%d"))

	if is_from == False and is_to == True:
		print "List only observations till %s" % (todate, )
		toyear = todate.split("-")[0]
		todate = time.mktime(time.strptime(todate, "%Y-%m-%d"))

	print

	# array of total sizes for every ObsID
	totsz = np.zeros(np.size(obsids))

	# table with obs info
	obstable=[]

	# printing out the header of the table
	storage_nodes_string=""
	for i in np.arange(Nnodes-1):
		storage_nodes_string=storage_nodes_string+storage_nodes[i]+"\t"
	storage_nodes_string=storage_nodes_string+storage_nodes[-1]
	storage_nodes_string_html="</th>\n <th align=center>".join(storage_nodes_string.split("\t"))

	if is_html == True:
		htmlrep.header(viewtype, storage_nodes_string_html)

	if viewtype == "brief" or viewtype == "plots":
		equalstrs=[]
		equalstring_size=109
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		MMDD	Dur	Ant	Band	   #Stations	    BF FD IM IS CS FE"
		print equalstring
	else:
		equalstrs=[]
		equalstring_size=143+8*Nnodes
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		MMDD	Dur	NodesList (lse)	Datadir	%s	Total(GB)	BF FD IM IS CS FE	Reduced		Pointing    Source" % (storage_nodes_string,)
		print equalstring

	j=0 # extra index to follow only printed lines
	# loop for every observation
	for counter in np.arange(np.size(obsids)):
	
		id=obsids[counter]
		# class instance with output Info
		out=outputInfo(id)	

		# prefix of ID, like L2010 or L2009
        	id_prefix=id.split("_")[0]   
		# suffix of ID, the sequence number of observation
        	id_suffix=id.split("_")[1]   

		# if request only newer observations, check first the year from the ID
		# if it's less than specified year, then continue with the next ID
		if is_from == True:
			obsyear=id_prefix[1:]
			if fromyear > obsyear:
				continue
		if is_to == True:
			obsyear=id_prefix[1:]
			if toyear < obsyear:
				continue

		# checking first if the directory with the parset file exists
		logdir=parset_logdir + id + "/"
		if not os.path.exists(logdir):
			# checking in the oldlog directory
			logdir=parset_oldlogdir + id + "/"
			if not os.path.exists(logdir):
				# Due to new naming convention and location of the parset files, also looking for the parset file
				# in any L2010-??-??_?????? directories	
				cmd="find %s -type f -name 'RTCP-%s.parset' -print" % (parset_logdir, id_suffix)
				logdir=os.popen(cmd).readlines()
				if np.size(logdir) > 0:
					# it means we found the directory with parset file
					logdir=os.popen(cmd).readlines()[0][:-1].split("RTCP-%s.parset" % (id_suffix,))[0]
				else:
					# no directory found
					comment = "Oops!.. The log directory or parset file in new naming convention does not exist!"
					out.setcomment(id, len(storage_nodes), comment)
					totsz[j] = 0.
					obstable=np.append(obstable, out)
					if tosort == False:
						if is_html == True:
							# making alternating colors in the table
							htmlrep.record(j%2 == 0 and "d0" or "d1", j, out.infohtml)
						print "%d	%s %s" % (j, id, comment)

					j=j+1
					continue

		# get the full path for the parset file for the current ID
		log=logdir + parset
		if not os.path.exists(log):
			# also checking that maybe the parset file has name the same as Obs ID
			log=logdir + id + ".parset"
			if not os.path.exists(log):
				# also checking that maybe the name of parset file has new naming convention, like "RTCP-<id_suffix>.parset"
				log=logdir + "RTCP-" + id_suffix + ".parset"
				if not os.path.exists(log):
					comment = "Oops!.. The parset file '%s' does not exist in any possible location!" % (parset,)
					out.setcomment(id, len(storage_nodes), comment)
					totsz[j] = 0.
					obstable=np.append(obstable, out)
					if tosort == False:
						if is_html == True:
							# making alternating colors in the table
							htmlrep.record(j%2 == 0 and "d0" or "d1", j, out.infohtml)
						print "%d	%s %s" % (j, id, comment)

					j=j+1
					continue

		# initializing the obsinfo class
		oi=obsinfo(log)

		# check if we want to show only newer data and check if the current obs is newer than specified date
		if is_from == True and np.size(oi.starttime) > 0:
			to_show=time.mktime(time.strptime(oi.starttime, "%Y-%m-%d %H:%M:%S"))-fromdate
			if to_show < 0:   # continue with the next ObsID
				continue
		if is_to == True and np.size(oi.starttime) > 0:
			to_show=time.mktime(time.strptime(oi.starttime, "%Y-%m-%d %H:%M:%S"))-todate
			if to_show > 0:   # continue with the next ObsID
				continue

		# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
		totsize=0
		dirsize_string=""
		for lse in storage_nodes:
			ddir=oi.datadir + "/" + id
			dirsize="x"
			cmd="cexec %s 'du -sh %s 2>&1 | cut -f 1 | grep -v such' | %s" % (cexec_nodes[lse], ddir, cexec_egrep_string)
			dirout=os.popen(cmd).readlines()
			if np.size(dirout) > 0:
				dirsize=dirout[0][:-1]
				cmd="cexec %s 'du -s -B 1 %s 2>&1 | cut -f 1 | grep -v such' | %s" % (cexec_nodes[lse], ddir, cexec_egrep_string)
				totsize=totsize + float(os.popen(cmd).readlines()[0][:-1])
			dirsize_string=dirsize_string+dirsize+"\t"

		# converting total size to GB
		totsz[j] = totsize / 1024. / 1024. / 1024.
		totsize = "%.1f" % (totsz[j],)

		# checking if this specific observation was already reduced. Checking for both existence of the *_red directory
		# in LOFAR_PULSAR_ARCHIVE and the existence of *_plots.tar.gz file
		statusline="x"
		for lse in storage_nodes:
			cmd="cexec %s 'ls -d %s 2>/dev/null' | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, cexec_egrep_string)
			if np.size(os.popen(cmd).readlines()) == 0:
				continue
			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>&1 | grep -v Permission' | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, id + "_red", cexec_egrep_string)
			redout=os.popen(cmd).readlines()
			if np.size(redout) > 0:
				reddir=redout[0][:-1]
				statusline=lse
				cmd="cexec %s 'find %s -name \"%s\" -print 2>&1 | grep -v Permission' | %s" % (cexec_nodes[lse], reddir, "*_plots.tar.gz", cexec_egrep_string)
				status=os.popen(cmd).readlines()
				if np.size(status) > 0:
					# tarfile exists
					statusline=statusline+" +tar"	
				else:
					statusline=statusline+" x"
				break

		profiles_array=[]
		chi_array=[]

		for lse in storage_nodes:
			cmd="cexec %s 'ls -d %s 2>/dev/null' | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, cexec_egrep_string)
			if np.size(os.popen(cmd).readlines()) == 0:
				continue
			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>&1 | grep -v Permission' | %s" % (cexec_nodes[lse], "/data4/LOFAR_PULSAR_ARCHIVE_" + lse, id + "_red", cexec_egrep_string)
			redout=os.popen(cmd).readlines()
			if np.size(redout) > 0:
				reddir=redout[0][:-1]
				# RSP0
				cmd="cexec %s 'ls -d %s 2>/dev/null' | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", cexec_egrep_string)
				if np.size(os.popen(cmd).readlines()) > 0:
					cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", "*.pfd*png", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# copying png files to local directory
						cmd="mkdir -p %s/%s ; cexec %s 'cp -f %s %s/%s' 2>&1 1>/dev/null" % (plotsdir, id, cexec_nodes[lse], " ".join([ss[:-1] for ss in status]), plotsdir, id)
						os.system(cmd)
						profiles_array = np.append(profiles_array, status[0].split("/")[-1].split(".pfd")[0] + ".pfd")
					# getting chi-squared
					cmd="cexec %s \"find %s -name \"%s\" -print -exec cat {} \; | grep chi-squared\" | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSP0", "*.prepout", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						chi_array = np.append(chi_array, status[0][:-1])
				# RSPA
				cmd="cexec %s 'ls -d %s 2>/dev/null' | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", cexec_egrep_string)
				if np.size(os.popen(cmd).readlines()) > 0:
					cmd="cexec %s 'find %s -name \"%s\" -print 2>/dev/null' | %s" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", "*.pfd*png", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# copying png files to local directory
						cmd="mkdir -p %s/%s ; cexec %s 'cp -f %s %s/%s' 2>&1 1>/dev/null" % (plotsdir, id, cexec_nodes[lse], " ".join([ss[:-1] for ss in status]), plotsdir, id)
						os.system(cmd)
						profiles_array = np.append(profiles_array, status[0].split("/")[-1].split(".pfd")[0] + ".pfd")
					# getting chi-squared
					cmd="cexec %s \"find %s -name \"%s\" -print -exec cat {} \; | grep chi-squared\" | %s | awk '{print $6}' -" % (cexec_nodes[lse], reddir + "/incoherentstokes/RSPA", "*.prepout", cexec_egrep_string)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						chi_array = np.append(chi_array, status[0][:-1])
				break

		# combining info
		out.Init(id, oi, dirsize_string, totsize, statusline, "", profiles_array, chi_array)

		obstable=np.append(obstable, out)
		# Printing out the report (if we want unsorted list)
		if tosort == False:
			if is_html == True:
				# making alternating colors in the table
				htmlrep.record(j%2 == 0 and "d0" or "d1", j, out.infohtml)
			print "%d	%s" % (j, out.info)

		# increase counter
		j=j+1

	# uploading the png files to webserver
	if viewtype == 'plots':
		cmd="rsync -a %s/ %s:%s 2>&1 1>/dev/null" % (plotsdir, webserver, webplotsdir)
		os.system(cmd)

	Nrecs=j
	# printing the sorted list
	if tosort == True:
		if sortkind == "size":
			sorted_indices=np.flipud(np.argsort([obstable[j].totsize for j in np.arange(Nrecs)], kind='mergesort'))
		elif sortkind == "time":
			sorted_indices=np.flipud(np.argsort([obstable[j].seconds for j in np.arange(Nrecs)], kind='mergesort'))
		# sorting by source (pointing coords)
		else:
			sorted_indices=np.argsort([obstable[j].pointing for j in np.arange(Nrecs)], kind='mergesort')

		for i in np.arange(Nrecs):
			print "%d	%s" % (i, obstable[sorted_indices[i]].info)
		if is_html == True:
			for i in np.arange(Nrecs):
				htmlrep.record(i%2 == 0 and "d0" or "d1", i, obstable[sorted_indices[i]].infohtml)

	if is_html == True:
		htmlrep.legend()
		htmlrep.close()

	# create html files sorted for different sorting modes, linked together
	if is_linkedhtml == True:
		sf={"id": "-id.html", "time": "-time.html", "size": "-size.html", "source": "-source.html"}
		for key in sf.keys():
			sf[key] = "%s%s" % (linkedhtmlstem, sf[key])
		htmlrep=writeHtmlList(sf["id"], linkedhtmlstem, fromdate, todate)
		for key in sf.keys():
			htmlrep.reInit(sf[key])
			htmlrep.open()
			htmlrep.obsnumber(storage_nodes, np.size(obsids))
			htmlrep.datesrange()
			htmlrep.linkedheader(viewtype, storage_nodes_string_html)
			if key == "size":
				sorted_indices=np.flipud(np.argsort([obstable[j].totsize for j in np.arange(Nrecs)], kind='mergesort'))
			elif key == "time":
				sorted_indices=np.flipud(np.argsort([obstable[j].seconds for j in np.arange(Nrecs)], kind='mergesort'))
			elif key == "source":
				sorted_indices=np.argsort([obstable[j].pointing for j in np.arange(Nrecs)], kind='mergesort')
			# unsorted (i.e. by default sorted by ObsId)
			else:
				sorted_indices=np.arange(Nrecs)
			for i in np.arange(Nrecs):
				htmlrep.record(i%2 == 0 and "d0" or "d1", i, obstable[sorted_indices[i]].infohtml)
			htmlrep.legend()
			htmlrep.close()
