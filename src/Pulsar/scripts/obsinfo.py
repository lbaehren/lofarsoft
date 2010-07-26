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
# if True then make a list in html format
is_html=False

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

# help
def usage (prg):
        """ Prints info how to use the script.
        """
        print "Program %s lists info about sub5 observations" % (prg, )
	print "Usage: %s [-s, --sorted] [-f, --from <YYYY-MM-DD>]\n\
                  [--html] [-h, --help]\n" % (prg, )

# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """
	try:
		opts, args = getopt.getopt (argv, "hsf:", ["help", "sorted", "from=", "html"])
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
			if opt in ("-f", "--from"):
				global is_from
				is_from = True
				global fromdate
				fromdate = arg

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0].split("/")[-1], sys.argv[1:])

	# writing the html code if chosen
	if is_html == True:
		print "<html>\n<head>\n  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n  <meta name=\"Classification\" content=\"public HTML\">\n  <title>LOFAR pulsar observations</title>\n</head>\n<body><h1 align=left>LOFAR pulsar observations</h1>"

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
	print "  <p align=left>Number of observations in Sub5: %d</p>" % (np.size(obsids), )
else:
	print "Number of observations in Sub5: %d" % (np.size(obsids), )
if is_from == True:
	if is_html == True:
		print "  <p align=left>List only observations since %s</p>" % (fromdate, )
	else:
		print "List only observations since %s" % (fromdate, )
	fromyear = fromdate.split("-")[0]
	fromdate = time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
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
	print "\n<table align=center border=1>"
	print "\n<th align=center>\n </th>\n" % (storage_nodes_string,)
else:
	print "#======================================================================================================================================================================="
	print "# No.	ObsID		MMDD	Dur	NodesList (lse)	Datadir	%s	Total(GB)	BF FD IM IS CS FE	Reduced		Pointing    Source" % (storage_nodes_string,)
	print "#======================================================================================================================================================================="

j=0 # extra index to follow only printed lines
# loop for every observation
for counter in np.arange(np.size(obsids)):
	
	id=obsids[counter]

	# prefix of ID, like L2010 or L2009
        id_prefix=id.split("_")[0]   
	# suffix of ID, the sequence number of observation
        id_suffix=id.split("_")[1]   

	# if request only newer observations, check first the year from the ID
	# if it's less than specified year, then continue with the next ID
	if is_from == True:
		obsyear=id_prefix.split("L")[1]
		if fromyear > obsyear:
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
				totsz[j] = 0.
				if tosort == False:
					print "%d	%s" % (j, comment)
				else:
					obstable=np.append(obstable, comment)
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
				totsz[j] = 0.
				if tosort == False:
					print "%d	%s" % (j, comment)
				else:
					obstable=np.append(obstable, comment)
				j=j+1
				continue

	# Getting the Date of observation
	cmd="grep Observation.startTime %s | tr -d \\'" % (log,)
	datestring1=os.popen(cmd).readlines()
	if np.size(datestring1) > 0:
		# it means that this keyword exist and we can extract the info
		datestring1=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
		c1 = time.strptime(datestring1, "%Y-%m-%d %H:%M:%S")
		smonth=datestring1.split("-")[1]
		sday=datestring1.split("-")[2].split(" ")[0]
		datestring=smonth+sday
	else:
		datestring="????"

	# check if we want to show only newer data and check if the current obs is newer than specified date
	if is_from == True and np.size(datestring1) > 0:
		to_show=time.mktime(c1)-fromdate
		if to_show < 0:   # continue with the next ObsID
			continue

        # reading the parset file
	# getting the info about StorageNodes. Note! For old parsets there seems to be no such a keyword Virtual...
	# However, the old keyword OLAP.storageNodeList has "non-friendly" format, so I just ignore this by now
	cmd="grep Observation.VirtualInstrument.storageNodeList %s | sed -e 's/lse//g'" % (log,)
	nodeslist=os.popen(cmd).readlines()
	if np.size(nodeslist) > 0:
		# it means that this keyword exist and we can extract the info
		nodeslist=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
	# cut the string of nodes if it is too long
	if len(nodeslist)>13:
		nodeslist=nodeslist[:13] + "..."

	# getting the name of /data? where the data are stored
	cmd="grep Observation.MSNameMask %s" % (log,)
	datadir="/" + os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("/")[1]

	# checking if the datadir exists in all sub5 lse nodes and if it does, gets the size of directory
	totsize=0
	dirsize_string=""
	for lse in storage_nodes:
		ddir=storage_prefix + lse + datadir + "/" + id
		dirsize="x"
		if os.path.exists(ddir):
			dirsize=os.popen("du -sh %s | cut -f 1" % (ddir,)).readlines()[0][:-1]
			totsize=totsize + float(os.popen("du -s -B 1 %s | cut -f 1" % (ddir,)).readlines()[0][:-1])
		dirsize_string=dirsize_string+dirsize+"\t"

	# converting total size to GB
	totsz[j] = totsize / 1024. / 1024. / 1024.
	totsize = "%.1f" % (totsz[j],)

	# getting info about the Type of the data (BF, Imaging, etc.)
	# check first if data are beamformed
	cmd="grep outputBeamFormedData %s" % (log,)
	bftype=os.popen(cmd).readlines()
	if np.size(bftype) > 0:
		# this info exists in parset file
		bftype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if bftype == 'f':
			bftype = "-"
		else:
			bftype = "+"
	else:
		bftype = "?"
	# check first if data are filtered
	cmd="grep outputFilteredData %s" % (log,)
	fdtype=os.popen(cmd).readlines()
	if np.size(fdtype) > 0:
		# this info exists in parset file
		fdtype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if fdtype == 'f':
			fdtype = "-"
		else:
			fdtype = "+"
	else:
		fdtype = "?"
	# check if data are imaging
	cmd="grep outputCorrelatedData %s" % (log,)
	imtype=os.popen(cmd).readlines()
	if np.size(imtype) > 0:
		# this info exists in parset file
		imtype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if imtype == 'f':
			imtype = "-"
		else:
			imtype = "+"
	else:
		imtype = "?"
	# check if data are incoherent stokes data
	cmd="grep outputIncoherentStokes %s" % (log,)
	istype=os.popen(cmd).readlines()
	if np.size(istype) > 0:
		# this info exists in parset file
		istype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if istype == 'f':
			istype = "-"
		else:
			istype = "+"
	else:
		istype = "?"
	# check if data are coherent stokes data
	cmd="grep outputCoherentStokes %s" % (log,)
	cstype=os.popen(cmd).readlines()
	if np.size(cstype) > 0:
		# this info exists in parset file
		cstype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if cstype == 'f':
			cstype = "-"
		else:
			cstype = "+"
	else:
		cstype = "?"
	# check if data are fly's eye mode data
	cmd="grep PencilInfo.flysEye %s" % (log,)
	fetype=os.popen(cmd).readlines()
	if np.size(fetype) > 0:
		# this info exists in parset file
		fetype=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].lower()[:1]
		if fetype == 'f':
			fetype = "-"
		else:
			fetype = "+"
	else:
		fetype = "?"

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

	# getting info about the pointing
	cmd="grep 'Beam\[0\].angle1' %s" % (log,)
	rarad=os.popen(cmd).readlines()
	if np.size(rarad)>0:
		# RA info exists in parset file
        	rarad=float(os.popen(cmd).readlines()[0][:-1].split(" = ")[-1])
		rahours=rarad*12./3.1415926
		rah=int(rahours)
		ram=int((rahours-rah)*60.)
		rastring="%02d%02d" % (rah, ram)
	else:
		rastring="????"

	cmd="grep 'Beam\[0\].angle2' %s" % (log,)
	decrad=os.popen(cmd).readlines()
	if np.size(decrad)>0:
		# DEC info exists in parset file
	        decrad=float(os.popen(cmd).readlines()[0][:-1].split(" = ")[-1])
		decdeg=decrad*180./3.1415926
		if decdeg>0:
			decsign="+"
		else:
			decsign="-"
		decdeg=abs(decdeg)
		decd=int(decdeg)
		decm=int((decdeg-decd)*60.)
		decstring="%c%02d%02d" % (decsign, decd, decm)
	else:
		decstring="_????"
	pointing="%s%s" % (rastring, decstring)

	# getting info about Source name (new addition to the Parset files)
	cmd="grep 'Observation.Beam\[0\].target' %s" % (log,)
	source=os.popen(cmd).readlines()
	if np.size(source)>0:
		# Source name exists in parset file
        	source=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
	else:
		source=""

	# Getting the Duration
	cmd="grep Observation.stopTime %s | tr -d \\'" % (log,)
	datestring2=os.popen(cmd).readlines()
	if np.size(datestring1) > 0 and np.size(datestring2) > 0:
		# it means that both start and stop Times exist in parset file
		datestring2=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
		c2 = time.strptime(datestring2, "%Y-%m-%d %H:%M:%S")
		diff=time.mktime(c2)-time.mktime(c1)  # difference in seconds
		if float(diff/3600.0) > 1.:
			duration="%.1fh" % (diff/3600.)
		else:
			duration="%.1fm" % (diff/60.)
	else:
		duration="?"

	# combining info
	# The columns are ObsID   MMDD NodesList   Datadir   Size_in_lse013   Size_in_lse014  Size_in_lse015 TotalSize  Beam-Formed Filtered Imaging IncohStokes Reduced Pointing Source
	info="%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-11s	%s   %s" % (id, datestring, duration, nodeslist, datadir, dirsize_string, totsize, bftype, fdtype, imtype, istype, cstype, fetype, statusline, pointing, source)

	# Printing out the report (if we want unsorted list)
	if tosort == False:
		print "%d	%s" % (j, info)
	else:
		obstable=np.append(obstable, info)

	# increase counter
	j=j+1

Nrecs=j
# printing the sorted list
if tosort == True:
	sorted_indices=np.flipud(np.argsort(totsz[:Nrecs], kind='mergesort'))
	for i in np.arange(Nrecs):
		print "%d	%s" % (i, obstable[sorted_indices[i]])

if is_html == True:
	print "\n</table>\n\n</body>\n</html>"
