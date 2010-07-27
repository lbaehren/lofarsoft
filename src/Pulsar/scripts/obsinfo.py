#!/usr/bin/env python
#
import os, sys, glob
import getopt
import numpy as np
import time

# True if we want the output list to be sorted by the TotalSize
tosort=False
# if True then will show only those observations newer than some date
is_from=False
fromdate=""
# if True then will show only those observations older than some date
is_to=False
todate=""
# if True then make a list in html format
is_html=False
htmlfile=""  # name of the html file in case is_html == True

# storage nodes to collect info about Pulsar Observations
# we assume that even for the case of long observations when data were spreaded out
# across many other nodes, these three "pulsar" sub5 nodes were used to record a part
# of the data as well
storage_nodes=["lse013", "lse014", "lse015"]
# storage path
storage_prefix = "/net/sub5/"
# list of directories withe the data
data_dirs=["/data1", "/data2", "/data3", "/data4"]

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
                	smonth=self.starttime.split("-")[1]
                	sday=self.starttime.split("-")[2].split(" ")[0]
                	self.datestring=smonth+sday
        	else:
                	self.datestring="????"

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
                	rahours=rarad*12./3.1415926
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
                	decdeg=decrad*180./3.1415926
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
		self.comment = ""
	
	def setcomment (self, comment):
		self.comment = comment

	def Init(self, id, datestring, duration, nodelist, datadir, dirsize_string, totsize, bftype, fdtype, imtype, istype, cstype, fetype, statusline, pointing, source):
		self.id = id
		self.datestring = datestring
		self.duration = duration
		self.nodelist = nodelist
		self.datadir = datadir
		self.dirsize_string = dirsize_string
		self.totsize = totsize
		self.bftype = bftype
		self.fdtype = fdtype
		self.imtype = imtype
		self.istype = istype
		self.cstype = cstype
		self.fetype = fetype
		self.statusline = statusline
		self.pointing = pointing
		self.source = source
		
		if comment == "":
			self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-11s	%s   %s" % (self.id, self.datestring, self.duration, self.nodeslist, self.datadir, self.dirsize_string, self.totsize, self.bftype, self.fdtype, self.imtype, self.istype, self.cstype, self.fetype, self.statusline, self.pointing, self.source)
			self.infohtml="<td>%s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%-16s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%s</td>\n <td>%c  %c  %c  %c  %c  %c</td>\n <td>%-11s</td>\n <td>%s</td>\n <td>%s</td>\n</tr>" % (self.id, self.datestring, self.duration, self.nodeslist, self.datadir, self.dirsize_string, self.totsize, self.bftype, self.fdtype, self.imtype, self.istype, self.cstype, self.fetype, self.statusline, self.pointing, self.source)
		else:
			self.info = comment
			self.infohtml = "<td colspan=11>%s</td>" % (comment, )


# help
def usage (prg):
        """ Prints info how to use the script.
        """
        print "Program %s lists info about sub5 observations" % (prg, )
	print "Usage: %s [-s, --sorted] [-f, --from <YYYY-MM-DD>] [-t, --to <YYYY-MM-DD>]\n\
                  [--html <html-file>] [-h, --help]\n" % (prg, )

# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """
	try:
		opts, args = getopt.getopt (argv, "hsf:t:", ["help", "sorted", "from=", "html=", "to="])
		for opt, arg in opts:
			if opt in ("-h", "--help"):
				usage(prg)
				sys.exit()
			if opt in ("-s", "--sorted"):
				global tosort
				tosort = True
			if opt in ("--html"):
				global is_html
				is_html = True
				global htmlfile
				htmlfile = arg
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

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0].split("/")[-1], sys.argv[1:])

# writing the html code if chosen
if is_html == True:
	htmlptr = open(htmlfile, 'w')
	htmlptr.write ("<html>\n<head>\n  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n  <meta name=\"Classification\" content=\"public HTML\">\n  <title>LOFAR pulsar observations</title>\n</head>\n<body>\n<h2 align=left>LOFAR pulsar observations</h2>\n")

# loop over the storage nodes and directories to get the list of all IDs
for s in storage_nodes:
	for d in data_dirs:
		mask = storage_prefix + s + d + "/?20??_?????"
		indlist=glob.glob(mask)			
		indlist = np.append(indlist, glob.glob(mask + "?"))
		indlist = np.append(indlist, glob.glob(mask + "??"))
		indlist = np.append(indlist, glob.glob(mask + "???"))
		obsids = np.append(obsids, [el.split("/")[-1] for el in indlist])

# number of storage nodes
Nnodes=np.size(storage_nodes)

# getting the unique list of IDs (some of IDs can have entries in many /data? and nodes)
# and sort in reverse order (most recent obs go first)
# more recent obs is the obs with higher ID (as it should be)
obsids = np.flipud(np.sort(np.unique(obsids), kind='mergesort'))

if is_html == True:
	htmlptr.write("Number of observations in Sub5: %d<br>\n" % (np.size(obsids), ))
print "Number of observations in Sub5: %d" % (np.size(obsids), )

if is_from == True and is_to == True:
	if is_html == True:
		htmlptr.write ("List only observations since %s till %s<br>\n" % (fromdate, todate))

	print "List only observations since %s till %s" % (fromdate, todate)
	fromyear = fromdate.split("-")[0]
	fromdate = time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
	toyear = todate.split("-")[0]
	todate = time.mktime(time.strptime(todate, "%Y-%m-%d"))

if is_from == True and is_to == False:
	if is_html == True:
		htmlptr.write ("List only observations since %s<br>\n" % (fromdate, ))

	print "List only observations since %s" % (fromdate, )
	fromyear = fromdate.split("-")[0]
	fromdate = time.mktime(time.strptime(fromdate, "%Y-%m-%d"))

if is_from == False and is_to == True:
	if is_html == True:
		htmlptr.write ("List only observations till %s<br>\n" % (todate, ))

	print "List only observations till %s" % (todate, )
	toyear = todate.split("-")[0]
	todate = time.mktime(time.strptime(todate, "%Y-%m-%d"))

print

# array of total sizes for every ObsID
totsz = np.zeros(np.size(obsids))
# table with obs info
if tosort == True:
	obstable=[]

# printing out the header of the table
# The columns are ObsID   MMDD	Duration NodesList   Datadir   Size_in_lse013   Size_in_lse014  Size_in_lse015 TotalSize  Beam-Formed FilteredData Imaging IncohStokes CohStokes Fly'sEye	Reduced Pointing Source

storage_nodes_string=""
for i in np.arange(Nnodes-1):
	storage_nodes_string=storage_nodes_string+storage_nodes[i]+"\t"
storage_nodes_string=storage_nodes_string+storage_nodes[-1]

if is_html == True:
	htmlptr.write ("\n<p align=left>\n<table align=center border=0>\n")
	htmlptr.write ("\n<tr align=center>\n <td>No.</td><td>ObsID</td><td>MMDD</td><td>Dur</td><td>NodesList (lse)</td><td>Datadir</td><td>%s</td><td>Total(GB)</td><td>BF FD IM IS CS FE</td><td>Reduced</td><td>Pointing</td><td>Source</td>\n</tr>\n" % (storage_nodes_string,))

print "#======================================================================================================================================================================="
print "# No.	ObsID		MMDD	Dur	NodesList (lse)	Datadir	%s	Total(GB)	BF FD IM IS CS FE	Reduced		Pointing    Source" % (storage_nodes_string,)
print "#======================================================================================================================================================================="

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
				comment = "%s	Oops!.. The log directory or parset file in new naming convention does not exist!" % (id, )
				out.setcomment(comment)
				totsz[j] = 0.
				if tosort == False:
					print "%d	%s" % (j, comment)
				else:
					obstable=np.append(obstable, out)
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
				comment = "%s	Oops!.. The parset file '%s' does not exist in any possible location!" % (id, parset)
				out.setcomment(comment)
				totsz[j] = 0.
				if tosort == False:
					print "%d	%s" % (j, comment)
				else:
					obstable=np.append(obstable, out)
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

	# checking if the datadir exists in all sub5 lse nodes and if it does, gets the size of directory
	totsize=0
	dirsize_string=""
	for lse in storage_nodes:
		ddir=storage_prefix + lse + oi.datadir + "/" + id
		dirsize="x"
		if os.path.exists(ddir):
			dirsize=os.popen("du -sh %s | cut -f 1" % (ddir,)).readlines()[0][:-1]
			totsize=totsize + float(os.popen("du -s -B 1 %s | cut -f 1" % (ddir,)).readlines()[0][:-1])
		dirsize_string=dirsize_string+dirsize+"\t"

	# converting total size to GB
	totsz[j] = totsize / 1024. / 1024. / 1024.
	totsize = "%.1f" % (totsz[j],)

	# checking if this specific observation was already reduced. Checking for both existence of the *_red directory
	# in LOFAR_PULSAR_ARCHIVE and the existence of *_plots.tar.gz file in ./incoherentstokes/ directory
	statusline="x"
	for lse in storage_nodes:
                reddir=storage_prefix + lse + "/data4/LOFAR_PULSAR_ARCHIVE_" + lse + "/" + id + "_red"
                if os.path.exists(reddir):
			statusline=lse
			tarball=reddir + "/incoherentstokes/*_plots.tar.gz"
			status=glob.glob(tarball)
			if np.size(status) > 0:
				# tarfile exists
				statusline=statusline+" +tar"	
			else:
				statusline=statusline+" x"
			break

	# combining info
	out.Init(id, oi.datestring, oi.duration, oi.nodelist, oi.datadir, dirsize_string, totsize, oi.bftype, oi.fdtype, oi.imtype, oi.istype, oi.cstype, oi.fetype, statusline, oi.pointing, oi.source)

	# Printing out the report (if we want unsorted list)
	if tosort == False:
		if is_html == True:
			htmlptr.write ("\n<tr align=center>\n <td>%d</td>\n %s" % (j, out.infohtml))
		print "%d	%s" % (j, out.info)
	else:
		obstable=np.append(obstable, out)

	# increase counter
	j=j+1

Nrecs=j
# printing the sorted list
if tosort == True:
	sorted_indices=np.flipud(np.argsort(totsz[:Nrecs], kind='mergesort'))
	for i in np.arange(Nrecs):
		print "%d	%s" % (i, obstable[sorted_indices[i]].info)
	if is_html == True:
		for i in np.arange(Nrecs):
			htmlptr.write ("\n<tr align=center>\n <td>%d</td>\n %s" % (i, obstable[sorted_indices[i]].infohtml))

if is_html == True:
	# getting date & time of last update
	cmd="date +'%b %d, %Y %H:%M:%S'"
	lupd=os.popen(cmd).readlines()[0][:-1]
	htmlptr.write ("\n<hr width=100%%>\n<address>\nLast Updated: %s\n</address>\n" % (lupd, ))
	htmlptr.write ("\n</table>\n\n</body>\n</html>")
	htmlptr.close()
