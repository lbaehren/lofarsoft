import os, sys, glob
import getopt
import numpy as np
import time

# True if we want the output list to be sorted by the TotalSize
tosort=False

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
# table info description
# 0 - id
# 1 - comment (empty if both logdir and parset file exist)
# 2 - nodelist
# 3 - datadir
# 4 - size in lse013
# 5 - size in lse014
# 6 - size in lse015
# 7 - total size
# 8 - "+" if BeamFormed obs
# 9 - "+" if Imaging obs
# 10 - "+" if Incoherent Stokes obs
# 11 - "+" if Coherent Stokes obs
# 12 - "+" if Fly's Eye obs
# 13 - status about reduced data
# 14 - pointing
# 15 - Date (MMDD)
# 16 - duration

# help
def usage (prg):
        """ Prints info how to use the script.
        """
        print "Program %s lists info about sub5 observations" % (prg, )
        print "Usage: %s [-s, --sorted] [-h, --help]\n" % (prg, )

# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """
	try:
		opts, args = getopt.getopt (argv, "hs", ["help", "sorted"])
		for opt, arg in opts:
			if opt in ("-h", "--help"):
				usage(prg)
				sys.exit()
			if opt in ("-s", "--sorted"):
				global tosort
				tosort = True

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0], sys.argv[1:])

	# loop over the storage nodes and directories to get the list of all IDs
	for s in storage_nodes:
		for d in data_dirs:
			mask = storage_prefix + s + d + "/L20??_?????"
			indlist=glob.glob(mask)			
			obsids = np.append(obsids, [el.split("/")[-1] for el in indlist])

# getting the unique list of IDs (some of IDs can have entries in many /data? and nodes)
# and sort in reverse order (most recent obs go first)
# more recent obs is the obs with higher ID (as it should be)
obsids = np.flipud(np.sort(np.unique(obsids), kind='mergesort'))
print "Number of observations in Sub5: %d" % (np.size(obsids), )
print

# array of total sizes for every ObsID
totsz = np.zeros(np.size(obsids))
# 2D table
obstable=[]

# printing out the header of the table
# The columns are ObsID   MMDD	Duration NodesList   Datadir   Size_in_lse013   Size_in_lse014  Size_in_lse015 TotalSize  Beam-Formed Imaging IncohStokes CohStokes Fly'sEye	Reduced Pointing
print "#================================================================================================================================================="
print "# No.	ObsID		MMDD	Dur	NodesList (lse)	Datadir	lse013	lse014	lse015	Total(GB)	BF IM IS CS FE	Reduced		Pointing"
print "#================================================================================================================================================="

# loop for every observation
for counter in np.arange(np.size(obsids)):
	
	obstable.append([])
	id=obsids[counter]
	obstable[counter].append(id)

	# prefix of ID, like L2010 or L2009
        id_prefix=id.split("_")[0]   
	# suffix of ID, the sequence number of observation
        id_suffix=id.split("_")[1]   

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
				obstable[counter].append(comment)
				for k in np.arange(11): obstable[counter].append("")
				totsz[counter] = 0.
				if tosort == False:
					print "%d       %s      %s" % (counter, id, comment)
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
				obstable[counter].append(comment)
				for k in np.arange(11): obstable[counter].append("")
				totsz[counter] = 0.
				if tosort == False:
					print "%d       %s      %s" % (counter, id, comment)
				continue
	obstable[counter].append("")

        # reading the parset file
	# getting the info about StorageNodes. Note! For old parsets there seems to be no such a keyword Virtual...
	# However, the old keyword OLAP.storageNodeList has "non-friendly" format, so I just ignore this by now
	cmd="grep Observation.VirtualInstrument.storageNodeList %s | tr -d lse" % (log,)
	nodeslist=os.popen(cmd).readlines()
	if np.size(nodeslist) > 0:
		# it means that this keyword exist and we can extract the info
		nodeslist=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
	# cut the string of nodes if it is too long
	if len(nodeslist)>13:
		nodeslist=nodeslist[:13] + "..."
	# adding the nodes' list to the table
	obstable[counter].append(nodeslist)

	# getting the name of /data? where the data are stored
	cmd="grep Observation.MSNameMask %s" % (log,)
	datadir="/" + os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("/")[1]
	# adding  the datadir to the table
	obstable[counter].append(datadir)

	# checking if the datadir exists in all sub5 lse nodes and if it does, gets the size of directory
	totsize=0
	for lse in storage_nodes:
		ddir=storage_prefix + lse + datadir + "/" + id
		dirsize="x"
		if os.path.exists(ddir):
			dirsize=os.popen("du -sh %s | cut -f 1" % (ddir,)).readlines()[0][:-1]
			totsize=totsize + float(os.popen("du -s -B 1 %s | cut -f 1" % (ddir,)).readlines()[0][:-1])
		obstable[counter].append(dirsize)

	# converting total size to GB
	totsize = totsize / 1024. / 1024. / 1024.
	obstable[counter].append("%.1f" % (totsize))
	totsz[counter] =  totsize

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
	obstable[counter].append(bftype)
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
	obstable[counter].append(imtype)
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
	obstable[counter].append(istype)
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
	obstable[counter].append(cstype)
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
	obstable[counter].append(fetype)


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
	obstable[counter].append(statusline)

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
	obstable[counter].append(pointing)

	# Getting the Date of observation
	cmd="grep Observation.startTime %s | tr -d \\'" % (log,)
	datestring1=os.popen(cmd).readlines()
	if np.size(datestring1) > 0:
		# it means that this keyword exist and we can extract the info
		datestring1=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
		smonth=datestring1.split("-")[1]
		sday=datestring1.split("-")[2].split(" ")[0]
		datestring=smonth+sday
	else:
		datestring="????"
	obstable[counter].append(datestring)

	# Getting the Duration
	cmd="grep Observation.stopTime %s | tr -d \\'" % (log,)
	datestring2=os.popen(cmd).readlines()
	if np.size(datestring1) > 0 and np.size(datestring2) > 0:
		# it means that both start and stop Times exist in parset file
		datestring2=os.popen(cmd).readlines()[0][:-1].split(" = ")[-1]
		c1 = time.strptime(datestring1, "%Y-%m-%d %H:%M:%S")
		c2 = time.strptime(datestring2, "%Y-%m-%d %H:%M:%S")
		diff=time.mktime(c2)-time.mktime(c1)  # difference in seconds
		if float(diff/3600.0) > 1.:
			duration="%.1fh" % (diff/3600.)
		else:
			duration="%.1fm" % (diff/60.)
	else:
		duration="?"
	obstable[counter].append(duration)


	# Printing out the report (if we want unsorted list)
	# The columns are ObsID   MMDD NodesList   Datadir   Size_in_lse013   Size_in_lse014  Size_in_lse015 TotalSize  Beam-Formed Imaging IncohStokes Reduced Pointing
	if tosort == False:
		print "%d	%s	%s	%s	%-16s %s	%s	%s	%s	%s		%c  %c  %c  %c  %c	%-11s	%s" % (counter, id, datestring, duration, nodeslist, datadir, obstable[counter][4], obstable[counter][5], obstable[counter][6], obstable[counter][7], bftype, imtype, istype, cstype, fetype, statusline, pointing)

# printing the sorted list
if tosort == True:
	sorted_indices=np.argsort(totsz, kind='mergesort')
	j=0
	for i in np.flipud(sorted_indices):
		if obstable[i][1] != "":
			print "%d	%s      %s" % (j, obstable[i][0], obstable[i][1])
		else:
			print "%d	%s	%s	%s	%-16s %s	%s	%s	%s	%s		%c  %c  %c  %c  %c	%-11s	%s" % (j, obstable[i][0], obstable[15], obstable[16], obstable[i][2], obstable[i][3], obstable[i][4], obstable[i][5], obstable[i][6], obstable[i][7], obstable[i][8], obstable[i][9], obstable[i][10], obstable[i][11], obstable[i][12], obstable[i][13], obstable[i][14])
		j=j+1
