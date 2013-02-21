#!/usr/bin/env python
#
# Script to collect info about LOFAR observations and write out
# the list of obs with different parameters listed, both in ascii
# and Html formats
#
# NOTE: script was tested mainly on BASH, and it's known for
# sure that it fails on CSH, because of many specific cexec and find
# system calls that require shell-specific format. Try using
# another version obsinfo-shell.py
#
# Vlad, Aug 5, 2010 (c)
#
# May  2, 2011 - changed version to work with CEPII layout
#              - new shortened format added - 'smega' (shortened mega)
#                which is the same as 'mega' without data size for each
#                of the data nodes (only the total is shown)
# May 17, 2011 - a few small bugs fixed; 'hoover' nodes are added to
#                the cexec_nodes table. By default, we search for data
#                all available locus nodes now
# May 23, 2011 - implemented new PULSAR_ARCHIVE scheme, with separate
#                directories for CS plots (OBSID_CSplots) on locus101
#                and IS plots (OBSID_redIS) on locus102. Now in the
#                web-page will only show combined plots for IS, CS
#                and BF (for future)
# Jul  8, 2011 - Added IM Location column with the html link to
#                osbervatory page with many diagnostic plot
# Jul 10, 2011 - Added "Obs Setup' column with plenty extra info from
#                parset file.
# Jul 12, 2011 - Added 'Start Time' and 'antenna config' to 'Obs Setup'
#                column
# Jan 30, 2012 - FD is removed from the table, and order has been changed
#                from BF,FD,Im,IS,CS,FE  to FE,Im,IS,CS,CV
#                Many changes were done to parsing parset files to 
#                accomodate new keywords for new OLAP release on Jan 27
#                Old parset format is also supported
###################################################################
import os, sys, glob
import getopt
import numpy as np
import time
import cPickle
import re
import subprocess

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
# if True then deletes only obsids from db file from --obsids option, or ones that meet
# --from and/or --to conditions
is_delete = False
# list of ObsIDs to be updated (only can be used with --update option)
update_obsids = []
# if True, then new obs (the ones not in the db) will be processed and added to db
is_append = False
# if True then obs will be printed one by one (debug mode) (with # = 0 for all)
is_debug = False
debugcounter=0

# if True, then only db file will be read to calculate the statistics of observations
is_stats = False

# if True then we do not check nodeslist from parset file to see if raw data do exist there
# and also do not search all storage nodes if necessary to find raw data
is_no_check_rawdata = False

# View of presenting info (usual (default), brief, plots, mega, and smega)
viewtype="usual"

# The string of conditions to filter out the list of output ObsIDs
# Under development, the only possible condition is "id.is_test" or "id.is_test == True" or "id.is_test == False"
search_string=""

# Setting User name
username=os.environ['USER']
# Lofar software
lofarsoft=os.environ['LOFARSOFT']
hostdir="Lofar"  # dir that contains plots, grid links, db file
plotsdir="/home/%s/%s/plots" % (username, hostdir)
griddir="/home/%s/%s/grid" % (username, hostdir)
# where to copy profile plots (ag004)
#webserver="%s@10.87.15.4" % (username, )  # ag004 - former dop95
webserver="%s@ag004" % (username, )  # ag004 - former dop95
htmltitle="LOFAR pulsar observations "
basehref="http://www.astron.nl"
basehref_dir="lofarpwg"
webplotsdir="/var/www/%s/plots" % (basehref_dir, )
webgriddir="/var/www/%s/grid" % (basehref_dir, )
# if False, then do not rsync plots to external webserver
is_torsync = True
# if True, then add links of number of obs vs. time, and disk volume vs. time
# on the statistics' page
is_statevol_links = False
# db file to dump the obs table
dumpfile="/home/%s/%s/dump.b" % (username, hostdir)

# common large file with archive links in the GRID
archivefile="/home/leeuwen/grid/archived-at-sara.txt"
gridfiles = ()  # list of archive files

# file used in debug mode to write iteration number in main loop
debugfile="/home/%s/.obsinfo_debug" % (username, )

atnflink_start="http://www.atnf.csiro.au/research/pulsar/psrcat/proc_form.php?startUserDefined=true&c1_val=&c2_val=&c3_val=&c4_val=&sort_attr=jname&sort_order=asc&condition=&pulsar_names="
atnflink_end="&ephemeris=long&submit_ephemeris=Get+Ephemeris&coords_unit=raj%2Fdecj&radius=&coords_1=&coords_2=&style=Long+with+last+digit+error&no_value=*&fsize=3&x_axis=&x_scale=linear&y_axis=&y_scale=linear&state=query"
#nedlink_start="http://nedwww.ipac.caltech.edu/cgi-bin/nph-objsearch?objname="
#nedlink_end="&extend=no&hconst=73&omegam=0.27&omegav=0.73&corr_z=1&out_csys=Equatorial&out_equinox=J2000.0&obj_sort=RA+or+Longitude&of=pre_text&zv_breaker=30000.0&list_limit=5&img_stamp=YES"
simbadlinkregion_start="http://simbad.u-strasbg.fr/simbad/sim-sam?Criteria=region%28circle%2c%20icrs%2c%20" # %20 or %20%2b
simbadlinkregion_end="%2c%205d%29%20%26%0d%0a%28otype%20%3d%20%27PSR%27%20%7c%20otype%20%3d%20%27ISM%27%20%7c%20otype%20%3d%20%27mR%27%29&submit=submit%20query&maxObject=100&OutputMode=LIST"
simbadlink_identifier="http://simbad.u-strasbg.fr/simbad/sim-id?Radius=5&Radius.unit=deg&submit=submit%20id&NbIdent=1&Ident="

# cexec corresponding table
cexec_nodes={}
for num in np.arange(100): # we have 100 locus nodes
	key="locus%03d" % (num+1)
	cexec_nodes[key] = "locus:%d" % (num)
cexec_egrep_string="egrep -v \'\\*\\*\\*\\*\\*\' |egrep -v \'\\-\\-\\-\\-\\-\'"
# cexec command to run. Using this mapfile makes keep mapping of the locus to be always the same
cexeccmd="cexec -f /etc/c3.conf.full"
# script that gets the status of processed data and also returns the size of processing directory
process_dir_status_script="%s/release/share/pulsar/bin/cep2_process_dir_status.sh" % (lofarsoft,)

# storage nodes to collect info about Pulsar Observations
# we assume that even for the case of long observations when data were spreaded out
# across many other nodes, these three "pulsar" sub5 nodes were used to record a part
# of the data as well
#storage_nodes=["locus001"]
storage_nodes=cexec_nodes.keys()  # by default, we'll search all available locus nodes except hoover nodes
# adding 'hoover' nodes to the table
#hoover_nodes=["locus101", "locus102"]
hoover_nodes=["locus101"]
cexec_nodes["locus101"] = "hoover:0"
#cexec_nodes["locus102"] = "hoover:1"

# list of lse nodes names for use as keys in the dictionary that keeps the sizes of obs directories in outputInfo class
#lsenames = ["locus%03d" % (n, ) for n in np.arange(1,103,1)]  # we have up to 102 nodes, including hoover's
lsenames = ["locus%03d" % (n, ) for n in np.arange(1,102,1)]  # we have up to 102 nodes, including hoover's

# list of directories with the data
data_dirs=["/data"]
# mask to represent ALL data dirs in one storage nodes
#datadir_mask="/data?"
datadir_mask="/data"
# pulsar archive area stem
psr_archive_dir="/data/LOFAR_PULSAR_ARCHIVE_"
# test dir to keep Ashish's test reduced data (within archive area)
# based on the presence of this dir, dataset will be marked as test
test_dir="PULSAR_SYSTEM_TEST_OBS"

# directories with parset files
parset_logdir="/globalhome/lofarsystem/log/"
parset_oldlogdir="/globalhome/lofarsystem/oldlog/"
parset_newlogdir="/globalhome/lofarsystem/production/lofar-trunk/bgfen/log/"
# name of the parset file
parset="RTCP.parset.0"

# Location of IM diagnostic plots
imserver="http://lcs023.control.lofar/inspect/HTML/" # for each ObsID the ObsID (without leading 'L') will be added to the path

# list of obs ids
obsids=[]
# list of ObsIDs from the dump file 
dbobsids=[]





# Class obsinfo with info from the parset file
class obsinfo:
	def __init__(self, id):
		self.id = id
		self.parset=""
		self.logdir=""
                self.datestring="????-??-??"
		self.project = "" # the name of the campaign
		self.starttime=self.stoptime=""
		self.seconds = 0
                self.antenna=self.antenna_config=self.band=self.stations=self.stations_string="?"
		self.stations_html=""
		self.nstations=self.ncorestations=self.nremotestations=0
		self.nodeslist_string=self.datadir=""
		self.nodeslist=[]
		self.subcluster = 'locus?'
		self.BF=self.FD=self.IM="?"     # BF flag is for CV - complex voltage data
		self.IS=self.CS=self.CV=self.FE="?"
		self.OCD="-"  # online coherent dedispersion
		self.rarad=self.decrad=0
                self.rastring="????"
                self.decstring="_????"
		self.pointing="????_????"
		self.source=""
		self.dur = 0
                self.duration="?"
		self.is_test=False   # if True, this obs is the test one
		self.stokes="?"
		self.nrSubbands = 0        # number of subbands
		self.subbandList="?"       # range of subbands, e.g. 77..320
#		self.subbands=[]           # list of subbands
		self.subbandWidth = 0      # width of subband in kHz
		self.nrChanPerSubIS = 0    # number of channels per subband (for IS)
		self.nrChanPerSubCS = 0    # number of channels per subband (for CS)
		self.sampleClock = 0       # clock in MHz (200 or 160)
		self.integrationSteps = 0  # stokes integration steps
		self.timeresIS = 0           # sampling interval (depends on on integration steps, clock, number of channels)
		self.timeresCS = 0
		self.bw = 0                # bandwidth (in MHz)
		self.cfreq = 0             # central freq (in MHz)
		self.nrBeams = 0           # number of station beams
		self.nrTiedArrayBeams = 0  # number of TA beams (per station beam, can be different for different station beams, we only check central beam)
		self.nrRings = 0           # number of TA rings (if used)
		self.ringSize = 0          # size of TA ring (in deg)

		# search for parset file
		self.find_parset()
		# update info if parset file was found
		if self.parset != "":
			self.update()

	#
	# checking first if the directory with the parset file exists
	#
	# There were several changes in the location of the parset files and its name, so we have to
	# check all of them. There were:
	# (0) OLDEST parset file: /globalhome/lofarsystem/oldlog/id/RTCP.parset.0   <-- parset_oldlogdir + id + parset  
	# (1) OLD parset was here: /globalhome/lofarsystem/log/id/RTCP.parset.0     <-- parset_logdir + id + parset
	#     Here, I found that for some datasets D2010_..., the dir name where parset file is
	#     is actually L2010_...  -> so I am checking it here as well
	# (2) NEW parset as of May 10, 2010 is here: /globalhome/lofarsystem/log/L2010-MM-DD-DATE/RTCP-ID.parset
	# (3) 2nd transpose parset as of Aug 20, 2010 is here: 
	#          /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L2010-MM-DD-DATE/RTCP-ID.parset
	#

	# search for parset file
	def find_parset (self):
		# suffix of ID, the sequence number of observation
		try:
			# because datadir naming convention was changed, namely datadirs can now be named L?????
			# so, this line with throw the Error, and we have to get id_suffix by just dropping the leading "L"
        		id_suffix=self.id.split("_")[1]   
		except:
			id_suffix=self.id[1:]
			

		# checking parset_logdir (1) first
		self.logdir=parset_logdir + self.id + "/"
		if os.path.exists(self.logdir): 
			self.get_parset_name(self.logdir, id_suffix)
			if self.parset != "": return
		# for D20??_ obsids also checking L20??_ directories
		if re.search("D20", self.id):			
			self.logdir=parset_logdir + re.sub("D20", "L20", self.id) + "/"
			if os.path.exists(self.logdir):
				self.get_parset_name(self.logdir, id_suffix)
				if self.parset != "": return
		# checking in the oldlog directory (0)
		self.logdir=parset_oldlogdir + self.id + "/"
		if os.path.exists(self.logdir):
			self.get_parset_name(self.logdir, id_suffix)
			if self.parset != "": return
		# Due to new naming convention and location of the parset files, also looking for the parset file
		# in any L2010-??-??_?????? directories	(2)
		cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_logdir, id_suffix)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			# it means we found the directory with parset file
			self.parset=status[0][:-1]
			self.logdir="/".join(status[0][:-1].split("/")[:-1]) + "/"
			if self.parset != "": return
		# now checking the new parset directory (3)
		cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_newlogdir, id_suffix)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			# it means we found the directory with parset file
			self.parset=status[0][:-1]
			self.logdir="/".join(status[0][:-1].split("/")[:-1]) + "/"
			if self.parset != "": return
                # now checking if parset file is present in pulsar archive summary directories
                summary_nodes={"locus092" : "CSplots", "locus093" : "CVplots", "locus094" : "redIS"}
                for node in summary_nodes.keys():
                        self.logdir=psr_archive_dir + node + "/" + self.id + "_" + summary_nodes[node] + "/"
                        if os.path.exists(self.logdir):
                                self.get_parset_name(self.logdir, id_suffix)
                                if self.parset != "": return
		self.logdir=""
		self.parset=""


	# get parset file from the parset directory
	def get_parset_name (self, dir, id_suffix):
		# get the full path for the parset file
		self.parset=dir + parset
		if os.path.exists(self.parset): return
		# also checking that maybe the parset file has name the same as Obs ID
		self.parset=dir + self.id + ".parset"
		if os.path.exists(self.parset): return
		# also checking that maybe the name of parset file has new naming convention, like "RTCP-<id_suffix>.parset"
		# OR like L<id_suffix>.parset
		self.parset=dir + "RTCP-" + id_suffix + ".parset"
		if os.path.exists(self.parset): return
		# OR like L<id_suffix>.parset
		self.parset=dir + "L" + id_suffix + ".parset"
		if os.path.exists(self.parset): return
		self.parset=""

	# getting the subcluster number where the raw data were taken (e.g. sub4, sub5, etc.)
	# if raw data were spread between several subclusters, then 'subA' is assigned
	# these data are checked if there are more than 3 compute nodes in nodeslist
	# also, even if there are 3 or less storage nodes used, we check if they belong to the
	# same subcluster, if not then 'subA' is assigned
	# is 'nodeslist' is empty, or has old strange node names like 'list...', then we 
	# assign 'sub?' to them. If the data is processed and present in the archive, then 'sub?'
	# will be further corrected
	def get_subcluster (self):
		try:
			snames = np.unique([cexec_nodes[i].split(":")[1] for i in self.nodeslist])
			if np.size(snames) == 0:
				return "locus?"
			elif np.size(snames) > 1:
				return "locusA"
			else:
				return "locus"
		except:
			return "locus?"

	# search for raw data in all datadirs and storage nodes
	# this function is called when no parset file was found
	def rawdata_search (self, storage_nodes, data_dirs, cexec_nodes, cexec_egrep_string):
		iddirs=[]
		self.nodeslist=[]
		# forming string with all locus nodes to check in one cexec command
		cexeclocus=cexec_nodes[storage_nodes[0]] # there is always at least one storage node
		if len(storage_nodes) > 1:
			for s in storage_nodes[1:]:
				cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
		for d in data_dirs:
			cmd="%s %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, d + "/" + self.id)
			cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
			# see if at least one directory was found on all locus nodes
			dirlines=[line for line in cexec_output if re.match("^-----", line) is None]
			if len(dirlines) > 0: iddirs.append(d)
			# finding all locus nodes that have the dir with raw data
			for l in np.arange(len(cexec_output)):
				if re.match("^-----", cexec_output[l]) is None: 
					self.nodeslist.append(cexec_output[l-1].split(" ")[1])

		iddirs=np.unique(iddirs)
		self.nodeslist=np.unique(self.nodeslist)
		self.datadir = ",".join(iddirs)
		self.subcluster = self.get_subcluster()  # getting sub-cluster name
		# forming string of nodes
		self.nodeslist_string=re.sub("locus", "", "[" + ",".join(self.nodeslist) + "]")
        	if len(self.nodeslist_string)>13:
                	self.nodeslist_string=self.nodeslist_string[:13] + "..."


	# check if raw data are indeed exist in datadir and nodeslist from parset file
	# if not, we call rawdata_search to update them
	def rawdata_check (self, storage_nodes, data_dirs, cexec_nodes, cexec_egrep_string):
		# getting the sizes only from the intersection of oi.nodeslist and storage_nodes
		insecnodes=list(set(self.nodeslist).intersection(set(storage_nodes)))
		is_ok = True  # if False, we will update datadir and nodeslist
		if len(insecnodes) == 0: return
		# forming string with all locus nodes needed to check in one cexec command
		cexeclocus=cexec_nodes[insecnodes[0]] # there is always at least one insecnodes nodes (we'd do return above otherwise)
		if len(insecnodes) > 1:
			for s in insecnodes[1:]:
				cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
		for d in data_dirs:
			cmd="%s %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, d + "/" + self.id)
			cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
			# see how many directories with raw data we have. Their number should be the same as number of insecnodes
			dirlines=[line for line in cexec_output if re.match("^-----", line) is None]
			if len(dirlines) < len(insecnodes):
				is_ok = False
				break
		# updating datadir and nodeslist
		if not is_ok:
			self.rawdata_search(storage_nodes, data_dirs, cexec_nodes, cexec_egrep_string)

        # parsing the string with ranges of subbands recorded to get list of subbands
        def getSubbands(self, sblist):
                subs=[]
                sbparts=sblist.split(",")
                for ss in sbparts:
                        sedges=ss.split("..")
                        if len(sedges) == 1: 
				pieces=sedges[0].split("*")
				if len(pieces) > 1: subs.append(int(pieces[1]))
				else: subs.append(int(sedges[0]))
                        else: subs.extend(np.arange(int(sedges[0]), int(sedges[1])+1))
#               subs=np.unique(subs)
                subs.sort() # sorting with smallest being the first
                return subs

	# update info based on parset file
	def update (self):
		# Getting the Date of observation
	        cmd="grep Observation.startTime %s | tr -d \\'" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# it means that this keyword exist and we can extract the info
                	self.starttime=status[0][:-1].split(" = ")[-1]
			# Getting the number of seconds from 1970. Can use this to sort obs out by date/time
			self.seconds=time.mktime(time.strptime(self.starttime, "%Y-%m-%d %H:%M:%S"))
			self.datestring=self.starttime.split(" ")[0]

		# Getting the Duration
		cmd="grep Observation.stopTime %s | tr -d \\'" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(self.starttime) > 0 and np.size(status) > 0:
			# it means that both start and stop Times exist in parset file
			self.stoptime=status[0][:-1].split(" = ")[-1]
			c1 = time.strptime(self.starttime, "%Y-%m-%d %H:%M:%S")
			c2 = time.strptime(self.stoptime, "%Y-%m-%d %H:%M:%S")
			self.dur=time.mktime(c2)-time.mktime(c1)  # difference in seconds
			if float(self.dur/3600.0) > 1.:
				self.duration="%.1fh" % (self.dur/3600.)
			else:
				self.duration="%.1fm" % (self.dur/60.)
		
		# Leaving only time in startTime
		self.starttime = self.starttime.split(" ")[1]

		# Getting the Antenna info (HBA or LBA)
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Antenna array setting exists in parset file
                	self.antenna=status[0][:-1].split(" = ")[-1].split("_")[0]

		# Getting the Antenna config (LBA_OUTER, LBA_INNER, HBA_JOINED, etc.)
        	cmd="grep 'Observation.antennaSet' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Antenna Set setting exists in parset file
                	self.antenna_config=status[0][:-1].split(" = ")[-1]

		# Getting the Filter setting
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# band filter setting exists in parset file
                	self.band=status[0][:-1].split(" = ")[-1].split("A_")[-1]

                # Getting the name of the Campaign
		cmd="grep 'Observation.Campaign.name' %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
		# Campaign name setting exists in parset file
			self.project=status[0][:-1].split(" = ")[-1]

		# Getting the stations and their number (including separately the number of CS and RS)
#        	cmd="grep 'Observation.VirtualInstrument.stationList' %s" % (self.parset,)
        	cmd="grep 'OLAP.storageStationNames' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Stations setting exists in parset file
                	self.stations=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
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

		# checking "locations" keywords first as in the new parset files (as from Jan 27, 2012), 
		# "mountpoints" can give wrong values
		self.nodeslist_string = "[]"
        	cmd="grep locations %s | awk '{print $3}' - | tr -d '[]' | sed -e 's/locus//g'" % (self.parset,)
		try:
			mountpoints=",".join([ss[:-1] for ss in os.popen(cmd).readlines()])
			mountpoints_arr=np.array(mountpoints.split(","))
			mountpoints_arr=mountpoints_arr.compress(mountpoints_arr!="")
			if np.size(mountpoints_arr) > 0:
				self.datadir=",".join(np.unique([m.split(":")[-1] for m in mountpoints_arr]))
				self.nodeslist_string="[" + ",".join(np.unique([m.split(":")[0] for m in mountpoints_arr])) + "]"
		except: pass

		# and now checking "mountpoints" keywords in the parset file to get Nodeslist and Datadir
		# if they are empty or not exist the checking later old keywords for storageNodes and storageDirectory
		if self.nodeslist_string == "[]":
        		cmd="grep mountpoints %s | awk '{print $3}' - | tr -d '[]' | sed -e 's/locus//g'" % (self.parset,)
			try:
				mountpoints=",".join([ss[:-1] for ss in os.popen(cmd).readlines()])
				mountpoints_arr=np.array(mountpoints.split(","))
				mountpoints_arr=mountpoints_arr.compress(mountpoints_arr!="")
				if np.size(mountpoints_arr) > 0:
					self.datadir=",".join(np.unique([m.split(":")[-1] for m in mountpoints_arr]))
					self.nodeslist_string="[" + ",".join(np.unique([m.split(":")[0] for m in mountpoints_arr])) + "]"
			except: pass

	        # reading the parset file
	        # getting the info about StorageNodes. Note! For old parsets there seems to be no such a keyword Virtual...
        	# However, the old keyword OLAP.storageNodeList has "non-friendly" format, so I just ignore this by now
		if self.nodeslist_string == "[]":
        		cmd="grep Observation.VirtualInstrument.storageNodeList %s | sed -e 's/locus//g'" % (self.parset,)
        		status=os.popen(cmd).readlines()
        		if np.size(status) > 0:
                		# it means that this keyword exist and we can extract the info
                		self.nodeslist_string=status[0][:-1].split(" = ")[-1]
			# there were many changes in parset keywords names and datadir naming conventions around May 3, 2011
			# so, I also need to check other keywords to get list proper locus nodes
			if self.nodeslist_string == "[]":
        			cmd="grep OLAP.Storage.hosts %s | sed -e 's/locus//g'" % (self.parset,)
        			status=os.popen(cmd).readlines()
        			if np.size(status) > 0:
                			# it means that this keyword exist and we can extract the info
                			self.nodeslist_string=status[0][:-1].split(" = ")[-1]

		if self.nodeslist_string != "[]":
			self.nodeslist=["locus%s" % (i) for i in self.nodeslist_string.split("[")[1].split("]")[0].split(",")]

		# getting the subcluster
		self.subcluster = self.get_subcluster()

		# After we figured the correct subcluster, then we cut the nodeslist string
        	# cut the string of nodes if it is too long
        	if len(self.nodeslist_string)>13:
                	self.nodeslist_string=self.nodeslist_string[:13] + "..."

		# if datadir is still empty, trying to look for old keywords
	        # getting the name of /data? where the data are stored
		if self.datadir == "":
			try:
        			cmd="grep Observation.MSNameMask %s" % (self.parset,)
        			self.datadir="/" + os.popen(cmd).readlines()[0][:-1].split(" = ")[-1].split("/")[1]
			# it seems that for some of observations, the field Observation.MSNameMask in the parset file is missing
			# instead, there is another field called "OLAP.Storage.targetDirectory"
			except:
        			cmd="grep OLAP.Storage.targetDirectory %s" % (self.parset,)
				status=os.popen(cmd).readlines()
				if np.size(status) > 0:
        				self.datadir="/" + status[0][:-1].split(" = ")[-1].split("/")[1]

		# check the presence in the parset file the keyword for IS 2nd Transpose
		# if such keyword exists then it means that parset format is "new" (as of Jan 27, 2012)
		# so, we can sometimes skip checking other "old" keywords...
		incoh_transposed=False
		cmd="grep OLAP.IncoherentStokesAreTransposed %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
				incoh_transposed=True

	        # check if online coherent dedispersion (OCD) was used
        	cmd="grep OLAP.coherentDedisperseChannels %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	self.OCD=status[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.OCD == 'f':
                        	self.OCD = "-"
                	else:
                        	self.OCD = "+"

		# for new parset files (after Jan 27, 2012)
		# getting info about the Type of the data (BF, Imaging, etc.)
        	cmd="grep Output_CoherentStokes.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
                if np.size(status) > 0:
                        # this info exists in parset file
                        self.CS=status[0][:-1].split(" = ")[-1].lower()[:1]
                        if self.CS == 'f':
                                self.CS = "-"
                                self.BF = "-"
                        else:
                                self.CS = "+"
                                self.BF = "-"
                                cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                                status=os.popen(cmd).readlines()
                                if np.size(status) > 0:
                                        # getting Stokes string
                                        self.stokes=status[0][:-1].split(" = ")[-1]
					# in the transition phase there were some parset with just XY
					# this means just 2 files, one for X, another Y
					# now is always XXYY, i.e. 4 files get written
                                        if self.stokes == "XXYY" or self.stokes == "XY":
                                                self.BF = "+"
                                                self.CS = "-"
		
		# in case keyword 'Output_CoherentStokes.enabled' does not exist, but format is new (after Jan 27, 2012)
		if incoh_transposed and self.CS == "?":
			cmd="grep TiedArrayBeam %s | grep coherent | awk '{print $3}' - | grep 'True\|T\|true\|t\|1'" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status) > 0:
				self.CS = "+"
                                self.BF = "-"
                                cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                                status=os.popen(cmd).readlines()
                                if np.size(status) > 0:
                                        # getting Stokes string
                                        self.stokes=status[0][:-1].split(" = ")[-1]
					# in the transition phase there were some parset with just XY
					# this means just 2 files, one for X, another Y
					# now is always XXYY, i.e. 4 files get written
                                        if self.stokes == "XXYY" or self.stokes == "XY":
                                                self.BF = "+"
                                                self.CS = "-"
			else:
				self.CS = "-"
				self.BF = "-"

	        # check if data are incoherent stokes data
        	cmd="grep Output_IncoherentStokes.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	self.IS=status[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.IS == 'f':
                        	self.IS = "-"
                	else:
                        	self.IS = "+"
				if self.stokes == "?":
					cmd="grep OLAP.CNProc_IncoherentStokes.which %s" % (self.parset,)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# getting Stokes string
						self.stokes=status[0][:-1].split(" = ")[-1]

                # at ~05.07.2012 the logic in the Parset files has changed, and the flag Output_CoherentStokes.enabled = True
                # ONLY for CS data and not for CV data.  For CV data one needs to check Output_Beamformed.enabled flag
                if self.IS != "+" and self.CS != "+":
                        # checking the Output_Beamformed flag
                        cmd="grep Output_Beamformed.enabled %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status) > 0:
                                # this info exists in parset file
                                if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
                                        self.BF = "+"
					if self.stokes == "?":
						cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                                		status=os.popen(cmd).readlines()
                        		        if np.size(status) > 0:
                                        		# getting Stokes string
                                        		self.stokes=status[0][:-1].split(" = ")[-1]

	        # check if data are imaging
        	cmd="grep Output_Correlated.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	self.IM=status[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.IM == 'f':
                        	self.IM = "-"
                	else:
                        	self.IM = "+"

        	# check first if data are filtered
		if self.FD == "?":
        		cmd="grep outputFilteredData %s" % (self.parset,)
        		status=os.popen(cmd).readlines()
        		if np.size(status) > 0:
                		# this info exists in parset file
                		self.FD=status[0][:-1].split(" = ")[-1].lower()[:1]
                		if self.FD == 'f':
                        		self.FD = "-"
                		else:
                        		self.FD = "+"

	        # check if data are imaging
		if self.IM == "?":
        		cmd="grep outputCorrelatedData %s" % (self.parset,)
        		status=os.popen(cmd).readlines()
        		if np.size(status) > 0:
                		# this info exists in parset file
                		self.IM=status[0][:-1].split(" = ")[-1].lower()[:1]
                		if self.IM == 'f':
                        		self.IM = "-"
                		else:
                        		self.IM = "+"

	        # check if data are incoherent stokes data
		if self.IS == "?":
        		cmd="grep outputIncoherentStokes %s" % (self.parset,)
        		status=os.popen(cmd).readlines()
        		if np.size(status) > 0:
                		# this info exists in parset file
                		self.IS=status[0][:-1].split(" = ")[-1].lower()[:1]
                		if self.IS == 'f':
                        		self.IS = "-"
                		else:
                        		self.IS = "+"

	        # check if data are coherent stokes data
		if self.CS == "?":
        		cmd="grep outputCoherentStokes %s" % (self.parset,)
        		status=os.popen(cmd).readlines()
        		if np.size(status) > 0:
                		# this info exists in parset file
                		self.CS=status[0][:-1].split(" = ")[-1].lower()[:1]
                		if self.CS == 'f':
                        		self.CS = "-"
                		else:
                        		self.CS = "+"

                # for parset files with old format
                # getting info about the Type of the data (BF, Imaging, etc.)
                # check first if data are beamformed
		if not incoh_transposed:
                	if self.CS == "+": self.BF = "-"
                	if self.BF == "?":
                        	cmd="grep Output_Beamformed.enabled %s" % (self.parset,)
                        	status=os.popen(cmd).readlines()
                        	if np.size(status) > 0:
                                	# this info exists in parset file
                                	self.BF=status[0][:-1].split(" = ")[-1].lower()[:1]
                                	if self.BF == 'f':
                                        	self.BF = "-"
                                	else:
                                        	self.BF = "+"
                        	else:
                                	cmd="grep outputBeamFormedData %s" % (self.parset,)
                                	status=os.popen(cmd).readlines()
                                	if np.size(status) > 0:
                                        	# this info exists in parset file
                                        	self.BF=status[0][:-1].split(" = ")[-1].lower()[:1]
                                        	if self.BF == 'f':
                                                	self.BF = "-"
                                        	else:
                                                	self.BF = "+"

	        # check if data are fly's eye mode data
        	cmd="grep PencilInfo.flysEye %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	self.FE=status[0][:-1].split(" = ")[-1].lower()[:1]
                	if self.FE == 'f':
                        	self.FE = "-"
                	else:
                        	self.FE = "+"

	        # getting info about the pointing
        	cmd="grep 'Observation.Beam\[0\].angle1' %s | grep -v AnaBeam" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# RA info exists in parset file
			try:
                		self.rarad=float(status[0][:-1].split(" = ")[-1])
			except:
				self.rarad=0.0
                	rahours=self.rarad*12./3.1415926
                	rah=int(rahours)
                	ram=int((rahours-rah)*60.)
                	self.rastring="%02d%02d" % (rah, ram)

        	cmd="grep 'Observation.Beam\[0\].angle2' %s | grep -v AnaBeam" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# DEC info exists in parset file
			try:
                		self.decrad=float(status[0][:-1].split(" = ")[-1])
			except:
				self.decrad=0.0
                	decdeg=self.decrad*180./3.1415926
                	if decdeg>0:
                        	decsign="+"
                	else:
                        	decsign="-"
                	decdeg=abs(decdeg)
                	decd=int(decdeg)
                	decm=int((decdeg-decd)*60.)
                	self.decstring="%c%02d%02d" % (decsign, decd, decm)

        	self.pointing="%s%s" % (self.rastring, self.decstring)

	        # getting info about Source name (new addition to the Parset files)
        	cmd="grep 'Observation.Beam\[0\].target' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Source name exists in parset file
                	self.source=status[0][:-1].split(" = ")[-1]
			if self.source != "":
				if self.source[0] == "'":
					self.source=self.source.split("'")[1]
				if self.source[0] == "\"":
					self.source=self.source.split("\"")[1]

                # if Stokes are still undetermined (e.g. if obs is IM), then
                # rereading default stokes for CS
                if self.stokes == "?" or (self.CS != "+" and self.BF == "+"):
                        cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status) > 0:
                                # getting Stokes string
                                self.stokes=status[0][:-1].split(" = ")[-1]

                # Getting Stokes info (for old parset files)
                if self.stokes == "?":
                        cmd="grep OLAP.Stokes.which %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status)>0:
                                # getting Stokes string
                                self.stokes=status[0][:-1].split(" = ")[-1]

		# Getting the list of subbands
		cmd="grep Observation.subbandList %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting range of subbands
			self.subbandList=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
                        # parsing the string with subband ranges to get list of subbands
#                        self.subbands = self.getSubbands(self.subbandList)
                        subbands = self.getSubbands(self.subbandList)
                        # getting total number of Subbands
#                        self.nrSubbands = np.size(self.subbands)
                        self.nrSubbands = np.size(subbands)
			# in the subbands range, several ranges can be written, and also subbands can be separated
			# just by commas. So, we leave only 8 symbols
			if len(self.subbandList) > 8:
				self.subbandList = self.subbandList[:8] + "..."

		# in new parset files (after Jan 27, 2012) there are new keywords for number of
		# chans per subband and this number can be different for IS and CS
		cmd="grep OLAP.CNProc_IncoherentStokes.channelsPerSubband %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.nrChanPerSubIS=int(status[0][:-1].split(" = ")[-1])
			except: self.nrChanPerSubIS = 0
		cmd="grep OLAP.CNProc_CoherentStokes.channelsPerSubband %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.nrChanPerSubCS=int(status[0][:-1].split(" = ")[-1])
			except: self.nrChanPerSubCS = 0

		# Getting number of channels per subband
		# for old datasets (before Jan 27, 2012) this number is the same for IS and CS
		if self.nrChanPerSubIS == 0 or self.nrChanPerSubCS == 0:
			cmd="grep Observation.channelsPerSubband %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			nchanpersub = 0
			if np.size(status)>0:
				# getting number of channels
				try:
					nchanpersub=int(status[0][:-1].split(" = ")[-1])
				except: nchanpersub = 0
			if nchanpersub == 0: # if for some reason parset file does not have this keyword 'Observation.channelsPerSubband'
				cmd="grep OLAP.Stokes.channelsPerSubband %s" % (self.parset,)
				status=os.popen(cmd).readlines()
				if np.size(status)>0:
					# getting number of channels
					try:
						nchanpersub=int(status[0][:-1].split(" = ")[-1])
					except: nchanpersub = 0
			if self.nrChanPerSubIS == 0: self.nrChanPerSubIS = nchanpersub
			if self.nrChanPerSubCS == 0: self.nrChanPerSubCS = nchanpersub

		# Getting the sample clock
		cmd="grep Observation.sampleClock %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting the clock
			try:
				self.sampleClock=int(status[0][:-1].split(" = ")[-1])
			except: self.sampleClock = 0
		if self.sampleClock == 0: # if keyword 'Observation.sampleClock' is missing in the parset file
			cmd="grep Observation.clockMode %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting the clock
				try:
					self.sampleClock=int(status[0][:-1].split(" = ")[-1].split("Clock")[1])
				except: self.sampleClock = 0

		# Getting width of the subband (in kHz)
		cmd="grep Observation.subbandWidth %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting the width of the subband
			try:
				self.subbandWidth=float(status[0][:-1].split(" = ")[-1])
			except: self.subbandWidth = 0
		if self.subbandWidth == 0 and self.sampleClock != 0:
			self.subbandWidth = ( ( self.sampleClock / 2. ) / 512. ) * 1000.
		
		# Getting the stokes integration steps (in old parset files)
		cmd="grep OLAP.Stokes.integrationSteps %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting integration steps
			try:
				self.integrationSteps=int(status[0][:-1].split(" = ")[-1])
			except: self.integrationSteps = 0
		if self.integrationSteps == 0: # if keyword 'OLAP.Stokes.integrationSteps' is missing in the parset file
			cmd="grep Observation.ObservationControl.onlineControl.OLAP.Stokes.integrationSteps %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting integration steps
				try:
					self.integrationSteps=int(status[0][:-1].split(" = ")[-1])
				except: self.integrationSteps = 0

		# getting timeIntegrationFactors (a la integrationSteps in new parset files) for IS and CS
		# and calculating sampling intervals
		cmd="grep OLAP.CNProc_IncoherentStokes.timeIntegrationFactor %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				tfactor=int(status[0][:-1].split(" = ")[-1])
				if tfactor != 0 and self.sampleClock != 0 and self.nrChanPerSubIS != 0:
					self.timeresIS = tfactor / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubIS) * 1000.
			except: tfactor = 0
		cmd="grep OLAP.CNProc_CoherentStokes.timeIntegrationFactor %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				tfactor=int(status[0][:-1].split(" = ")[-1])
				if tfactor != 0 and self.sampleClock != 0 and self.nrChanPerSubCS != 0:
					self.timeresCS = tfactor / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubCS) * 1000.
			except: tfactor = 0

		# Calculating the sampling interval (in ms)
		# if the values are still undetermined
		if self.timeresIS == 0 or self.timeresCS == 0:
			if self.integrationSteps != 0 and self.sampleClock != 0:
				if self.nrChanPerSubIS != 0 and self.timeresIS == 0:
					self.timeresIS = self.integrationSteps / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubIS) * 1000.
				if self.nrChanPerSubCS != 0 and self.timeresCS == 0:
					self.timeresCS = self.integrationSteps / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubCS) * 1000.

		# Calculating the total BW (in MHz)
		if self.nrSubbands != 0 and self.subbandWidth != 0:
			self.bw = self.subbandWidth * self.nrSubbands / 1000.

		# Calculating the central freq (in MHz)
		if self.band != "?":
			try:
				lower_band_freq = int(self.band.split("_")[0])
				if self.sampleClock == 200:
					if lower_band_freq > 200:
						lower_band_edge = 200.0
					elif lower_band_freq < 200 and lower_band_freq > 100:
						lower_band_edge = 100.0
					else: lower_band_edge = 0.0
				if self.sampleClock == 160:
					if lower_band_freq >= 160:
						lower_band_edge = 160
					elif lower_band_freq < 160 and lower_band_freq >= 80:
						lower_band_edge = 80
					else: lower_band_edge = 0

#				if np.size(self.subbands) > 0 and self.subbandWidth != 0 and (self.nrChanPerSubIS != 0 or self.nrChanPerSubCS != 0):
				if np.size(subbands) > 0 and self.subbandWidth != 0 and (self.nrChanPerSubIS != 0 or self.nrChanPerSubCS != 0):
                                        try:
                                                # CS has a priority but we still have chance to modify it below
						nchanpersub = self.nrChanPerSubCS
						if self.nrChanPerSubIS == self.nrChanPerSubCS or self.nrChanPerSubIS == 0 or self.nrChanPerSubCS == 0:
			                                nchanspersub = (self.nrChanPerSubIS != 0 and self.nrChanPerSubIS or self.nrChanPerSubCS)
			                        elif self.IS == '+' and self.CS != '+' and self.BF != '+':
			                                nchanspersub = self.nrChanPerSubIS
			                        elif self.IS != '+' and (self.CS == '+' or self.BF == '+'):
			                                nchanspersub = self.nrChanPerSubCS
						else:   # still CS has a priority
							nchanspersub = self.nrChanPerSubCS
                                                if nchanpersub > 1: # it means that we ran 2nd polyphase
#                                                        lofreq = lower_band_edge + (self.subbandWidth / 1000.) * self.subbands[0] - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
#                                                        hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (self.subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
                                                        lofreq = lower_band_edge + (self.subbandWidth / 1000.) * subbands[0] - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
                                                        hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
                                                else:
#                                                        lofreq = lower_band_edge + (self.subbandWidth / 1000.) * self.subbands[0] - 0.5 * (self.subbandWidth / 1000.)
#                                                        hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (self.subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.)
                                                        lofreq = lower_band_edge + (self.subbandWidth / 1000.) * subbands[0] - 0.5 * (self.subbandWidth / 1000.)
                                                        hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.)
                                                self.freq_extent = hifreq - lofreq
                                                self.cfreq = lofreq + self.freq_extent/2.
                                        except: pass
			except: pass

		# Getting number of Station beams
		cmd="grep Observation.nrBeams %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of station beams
			try:
				self.nrBeams=int(status[0][:-1].split(" = ")[-1])
			except: self.nrBeams = 0

		# Getting number of TA Beams in central station beam (Beam 0)
		cmd="grep 'Observation.Beam\[0\].nrTiedArrayBeams' %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of TA beams
			try:
				self.nrTiedArrayBeams=int(status[0][:-1].split(" = ")[-1])
			except: self.nrTiedArrayBeams = 0

		# Getting number of TA rings
		cmd="grep OLAP.PencilInfo.nrRings %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of TA rings
			try:
				self.nrRings=int(status[0][:-1].split(" = ")[-1])
			except: self.nrRings = 0
		if self.nrRings == 0: # if keyword 'OLAP.PencilInfo.nrRings' is missing in the parset file
			cmd="grep 'Observation.Beam\[0\].nrTabRings' %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting number of TA rings
				try:
					self.nrRings=int(status[0][:-1].split(" = ")[-1])
				except: self.nrRings = 0

		# Getting the size of the TA ring
		if self.nrRings != 0:
			cmd="grep OLAP.PencilInfo.ringSize %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting size of the TA ring
				try:
					self.ringSize=float(status[0][:-1].split(" = ")[-1])
					self.ringSize = self.ringSize * (180./3.1415926)
				except: self.ringSize = 0
			else: # if keyword 'OLAP.PencilInfo.ringSize' is missing in the parset file
				cmd="grep 'Observation.Beam\[0\].nrTabRingSize' %s" % (self.parset,)
				status=os.popen(cmd).readlines()
				if np.size(status)>0:
					# getting size of the TA ring
					try:
						self.ringSize=float(status[0][:-1].split(" = ")[-1])
						self.ringSize = self.ringSize * (180./3.1415926)
					except: self.ringSize = 0
				else: # if all previous keywords for ringsize are missing (recent change since Jan27, 2012?)
					cmd="grep 'Observation.Beam\[0\].tabRingSize' %s" % (self.parset,)
					status=os.popen(cmd).readlines()
					if np.size(status)>0:
						# getting size of the TA ring
						try:
							self.ringSize=float(status[0][:-1].split(" = ")[-1])
							self.ringSize = self.ringSize * (180./3.1415926)
						except: self.ringSize = 0

		# in case keyword 'Output_IncoherentStokes.enabled' does not exist, but format is new (after Jan 27, 2012)
		# also we need then decrease the number of TA beams as one of them is IS beam now
		if incoh_transposed:
			cmd="grep 'Observation.Beam\[0\].TiedArrayBeam' %s | grep coherent | awk '{print $3}' - | grep 'False\|false\|F\|f\|0'" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status) > 0:
				# so, in the array of TiedArrayBeams we have at least one IS beam, so we have to
				# decrease then the number of TiedArrayBeams 
				self.nrTiedArrayBeams -= np.size(status)
				if self.IS == "?":
                        		self.IS = "+"
					if self.stokes == "?":
						cmd="grep OLAP.CNProc_IncoherentStokes.which %s" % (self.parset,)
						status=os.popen(cmd).readlines()
						if np.size(status) > 0:
							# getting Stokes string
							self.stokes=status[0][:-1].split(" = ")[-1]
		
	# return True if parset file was found, and False otherwise
	def is_parset (self):
		if self.parset == "":
			return False
		else:
			return True





# Class (structure) with all fields to output
class outputInfo:
	def __init__(self, id):
		self.id = id
		try:
			self.obsyear = self.id.split("_")[0][1:]
		except: self.obsyear = 2011
		if self.obsyear > 2100:
			self.obsyear = 2011
		try:
			self.seconds=time.mktime(time.strptime(self.obsyear, "%Y"))
		except: self.seconds = 0
		self.storage = {}
		for key in lsenames:
			self.storage[key] = ["x", "0.0"]

	def get_link (self):
		# if source name starts on "B" or "J" -> it is pulsar
		if self.oi.source[0] == "B" or self.oi.source[0] == "J":
			link = atnflink_start + self.oi.source.split(" ")[0].replace("+", "%2B") + atnflink_end
		# if source name starts on "Pos" -> it's survey pointing -> coordinate search of Simbad
		elif self.oi.source[:3] == "Pos" and re.match("^[\+\-\.\d\s]*$", self.oi.source[3:]) != None:
			rad=self.oi.source.split(" ")[1]
			decd=self.oi.source.split(" ")[2].split("+")[-1]
			link = simbadlinkregion_start + rad
			if float(decd) > 0:
				link = link + "%20%2b" + decd + simbadlinkregion_end   # "%20%2b" is " +"
			else:
				link = link + "%20" + decd + simbadlinkregion_end      # "%20" is " "
		# everything else -> search in NED database (deprecated, Simbad is now)
		else:
			link = simbadlink_identifier + self.oi.source.split("(")[0].replace(" ", "%20")
#			link = nedlink_start + self.oi.source.replace("+", "%2B") + nedlink_end
		return link

	# function transforms project name for the HTML page not to be very long
	# adds spaces before and after 2011, 2012, 2013,...
	def get_html_project (self, project):
		years=range(2011, 2030, 1)
		project_html = project
		for yr in years:
			project_html = (" " + str(yr) + " ").join(project_html.split(str(yr)))
		return project_html

	def Init(self, id, oi, storage_nodes, dirsizes, statusline, CSredlocation, ISredlocation, processed_dirsize, comment, filestem_array, archivestatus, archivesize):
		self.id = id
		try:
			self.obsyear = self.id.split("_")[0][1:]
		except: self.obsyear = 2011
		if self.obsyear > 2100:
			self.obsyear = 2011
		self.oi = oi
		if self.oi.seconds != 0 and re.match("^\d+\.\d+$", str(self.oi.seconds)) is not None:
			self.seconds = self.oi.seconds
		else:
			try:
				self.seconds=time.mktime(time.strptime(self.obsyear, "%Y"))
			except:
				self.seconds=0
		self.pointing = self.oi.pointing
		self.statusline = statusline
		self.subcluster = self.oi.subcluster
		self.CSredlocation = CSredlocation
		self.ISredlocation = ISredlocation
		self.processed_dirsize = processed_dirsize
		self.comment = comment
		self.filestem_array = filestem_array
		self.archivestatus = archivestatus
		self.archivesize = archivesize

		for l in storage_nodes:
			self.storage[l]=dirsizes[l]
		# updating info fields...
		self.update(storage_nodes)


	# update info and infohtml fields
	def update(self, storage_nodes):
		# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
		self.totsize=0.0
		self.dirsize_string=""
		for l in storage_nodes:
			if l in self.storage.keys():
				self.totsize = self.totsize + float(self.storage[l][1])
				self.dirsize_string = self.dirsize_string + self.storage[l][0] + "\t"
		# converting total size to GB
		self.totsize = "%.1f" % (self.totsize / 1000. / 1000. / 1000.,)
		self.dirsize_string_html = "</td>\n <td align=center>".join(self.dirsize_string.split("\t")[:-1])

		# do not keep IMredlocation in the class object
                try:
                        id_suffix=self.id.split("_")[1]
                except:
                        id_suffix=self.id[1:]
		IMredlocation = imserver + id_suffix

		if viewtype == "usual":
			self.colspan = 2
		else:
			self.colspan = 13

		# Compiling obs setup ascii and html strings
		obssetup=""
		obssetup_html=""
		sampling_setup=""
		sampling_setup_html=""
		if self.oi.nrChanPerSubIS == self.oi.nrChanPerSubCS or self.oi.nrChanPerSubIS == 0 or self.oi.nrChanPerSubCS == 0:
			sampling_setup="Channels/Sub:%d|" % (self.oi.nrChanPerSubIS != 0 and self.oi.nrChanPerSubIS or self.oi.nrChanPerSubCS)
			sampling_setup_html="Channels/Sub: %d<br>" % (self.oi.nrChanPerSubIS != 0 and self.oi.nrChanPerSubIS or self.oi.nrChanPerSubCS)
		elif self.oi.IS == '+' and self.oi.CS != '+' and self.oi.BF != '+':
			sampling_setup="Channels/Sub:%d|" % (self.oi.nrChanPerSubIS)
			sampling_setup_html="Channels/Sub: %d<br>" % (self.oi.nrChanPerSubIS)
		elif self.oi.IS != '+' and (self.oi.CS == '+' or self.oi.BF == '+'):
			sampling_setup="Channels/Sub:%d|" % (self.oi.nrChanPerSubCS)
			sampling_setup_html="Channels/Sub: %d<br>" % (self.oi.nrChanPerSubCS)
		else:
			sampling_setup="Channels/Sub:%d(IS),%d(CS)|" % (self.oi.nrChanPerSubIS, self.oi.nrChanPerSubCS)
			sampling_setup_html="Channels/Sub: %d (IS), %d (CS)<br>" % (self.oi.nrChanPerSubIS, self.oi.nrChanPerSubCS)
		if self.oi.timeresIS == self.oi.timeresCS or self.oi.timeresIS == 0 or self.oi.timeresCS == 0:
			sampling_setup+="Sampling:%g_ms" % (self.oi.timeresIS != 0 and self.oi.timeresIS or self.oi.timeresCS)
			sampling_setup_html+="Sampling: %g ms" % (self.oi.timeresIS != 0 and self.oi.timeresIS or self.oi.timeresCS)
		elif self.oi.IS == '+' and self.oi.CS != '+' and self.oi.BF != '+':
			sampling_setup+="Sampling:%g_ms" % (self.oi.timeresIS)
			sampling_setup_html+="Sampling: %g ms" % (self.oi.timeresIS)
		elif self.oi.IS != '+' and (self.oi.CS == '+' or self.oi.BF == '+'):
			sampling_setup+="Sampling:%g_ms" % (self.oi.timeresCS)
			sampling_setup_html+="Sampling: %g ms" % (self.oi.timeresCS)
		else:
			sampling_setup+="Sampling:%g_ms(IS),%g_ms(CS)" % (self.oi.timeresIS, self.oi.timeresCS)
			sampling_setup_html+="Sampling: %g ms (IS), %g ms (CS)" % (self.oi.timeresIS, self.oi.timeresCS)

		if self.comment == "":
			if self.oi.nrRings > 0:
				obssetup="%s|Station_Beams:%d|TA_beams:%d[%d_ring(s),%g_deg]|StartTime:%s|Clock:%d_MHz|CentrFreq:%g_MHz|BW:%g_MHz|Subbands:%d[%s,%g_kHz]|%s|Stokes:%s" % (self.oi.antenna_config, self.oi.nrBeams, self.oi.nrTiedArrayBeams, self.oi.nrRings, self.oi.ringSize, self.oi.starttime, self.oi.sampleClock, self.oi.cfreq, self.oi.bw, self.oi.nrSubbands, self.oi.subbandList, self.oi.subbandWidth, sampling_setup, self.oi.stokes)
				obssetup_html="%s&nbsp;&nbsp;&nbsp;Station Beams: %d<br>TA beams: %d [%d ring(s), %g deg]<br>Start Time: %s<br>Clock: %d MHz<br>Center Freq: %g MHz<br>BW: %g MHz<br>Subbands: %d [%s, %g kHz]<br>%s<br>Stokes: %s" % (self.oi.antenna_config, self.oi.nrBeams, self.oi.nrTiedArrayBeams, self.oi.nrRings, self.oi.ringSize, self.oi.starttime, self.oi.sampleClock, self.oi.cfreq, self.oi.bw, self.oi.nrSubbands, self.oi.subbandList, self.oi.subbandWidth, sampling_setup_html, self.oi.stokes)
			else:
				obssetup="%s|Station_Beams:%d|TA_beams:%d|StartTime:%s|Clock:%d_MHz|CentrFreq:%g_MHz|BW:%g_MHz|Subbands:%d[%s,%g_kHz]|%s|Stokes:%s" % (self.oi.antenna_config, self.oi.nrBeams, self.oi.nrTiedArrayBeams, self.oi.starttime, self.oi.sampleClock, self.oi.cfreq, self.oi.bw, self.oi.nrSubbands, self.oi.subbandList, self.oi.subbandWidth, sampling_setup, self.oi.stokes)
				obssetup_html="%s&nbsp;&nbsp;&nbsp;Station Beams: %d<br>TA beams: %d<br>Start Time: %s<br>Clock: %d MHz<br>Center Freq: %g MHz<br>BW: %g MHz<br>Subbands: %d [%s, %g kHz]<br>%s<br>Stokes: %s" % (self.oi.antenna_config, self.oi.nrBeams, self.oi.nrTiedArrayBeams, self.oi.starttime, self.oi.sampleClock, self.oi.cfreq, self.oi.bw, self.oi.nrSubbands, self.oi.subbandList, self.oi.subbandWidth, sampling_setup_html, self.oi.stokes)

		# forming first Info (not html) string
		if viewtype == "brief":
			if self.comment == "":
				self.info = "%s	%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%s		%s		%-27s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.project, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.CSredlocation, self.ISredlocation, self.statusline)
			else: # no parset file
				self.info = "%s	%s										%s		%s		%-27s" % (self.id, self.comment, self.CSredlocation, self.ISredlocation, self.statusline)
		elif viewtype == "plots":
			if self.comment == "":
				self.info = "%s	%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%s		%s		%-27s   %s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.project, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
			else: # no parset file
				self.info = "%s	%s										%s		%s		%-27s   %s" % (self.id, self.comment, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
		elif viewtype == "mega":
			if self.comment == "":
				self.info = "%s	%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%-16s %s%-9s	%s	%s	%s		%s		%-27s   %s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.project, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.oi.nodeslist_string, self.dirsize_string, self.totsize, obssetup, self.oi.stations, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
			else: # no parset file
				self.info = "%s	%s										%-16s %s%-9s	%s	%s	%s		%s		%-27s   %s" % (self.id, self.comment, self.oi.nodeslist_string, self.dirsize_string, self.totsize, obssetup, self.oi.stations, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
		elif viewtype == "smega":
			if self.comment == "":
				self.info = "%s	%s	%s	%s	%s	%s	%s	   %-15s  %c  %c  %c  %c  %c  %c	%-16s %-9s	%s	%s	%s		%s		%-27s   %s" % (self.id, self.oi.source != "" and self.oi.source or self.oi.pointing, self.oi.datestring, self.oi.project, self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.oi.nodeslist_string, self.totsize, obssetup, self.oi.stations, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
			else: # no parset file
				self.info = "%s	%s										%-16s %-9s	%s	%s	%s		%s		%-27s   %s" % (self.id, self.comment, self.oi.nodeslist_string, self.totsize, obssetup, self.oi.stations, self.CSredlocation, self.ISredlocation, self.statusline, self.archivestatus)
		else: # usual
			if self.comment == "":
				self.info = "%s	%s	%s	%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-27s	%s   %s   %s" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist_string, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.statusline, self.oi.pointing, self.oi.source, self.oi.project)
			else: # no parset file
				self.info = "%s	%s		%-16s %s	%s%s		%c  %c  %c  %c  %c  %c	%-27s	%s   %s   %s" % (self.id, self.comment, self.oi.nodeslist_string, self.oi.datadir, self.dirsize_string, self.totsize, self.oi.FE, self.oi.IM, self.oi.IS, self.oi.CS, self.oi.BF, self.oi.OCD, self.statusline, self.oi.pointing, self.oi.source, self.oi.project)

		# now forming first Info html string
		if viewtype == "brief":
			if self.comment == "":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.get_html_project(self.oi.project), self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE == "-" and "&#8211;" or self.oi.FE, self.oi.IM == "-" and "&#8211;" or (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD, self.CSredlocation, self.ISredlocation)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.get_link(), self.oi.source, self.oi.datestring, self.get_html_project(self.oi.project), self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE == "-" and "&#8211;" or self.oi.FE, self.oi.IM == "-" and "&#8211;" or (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD, self.CSredlocation, self.ISredlocation)
			else: # no parset file
					self.infohtml="<td>%s</td>\n <td colspan=%d align=center><font color=\"brown\"><b>%s</b></font></td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.colspan, self.comment, self.CSredlocation, self.ISredlocation)

		elif viewtype == "plots" or viewtype == "mega" or viewtype == "smega":
			if self.comment == "":
				if self.oi.source == "":
					self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.pointing, self.oi.datestring, self.get_html_project(self.oi.project), self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE == "-" and "&#8211;" or self.oi.FE, self.oi.IM == "-" and "&#8211;" or (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD)
				else:
					self.infohtml="<td>%s</td>\n <td align=center><a href=\"%s\">%s</a></td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.get_link(), self.oi.source, self.oi.datestring, self.get_html_project(self.oi.project), self.oi.duration, self.oi.antenna, self.oi.band, self.oi.stations_string, self.oi.FE == "-" and "&#8211;" or self.oi.FE, self.oi.IM == "-" and "&#8211;" or (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD)
			else: # no parset file
					self.infohtml="<td>%s</td>\n <td colspan=%d align=center><font color=\"brown\"><b>%s</b></font></td>" % (self.id, self.colspan, self.comment)

			# adding CS/CV combined_plot column
			if self.filestem_array[0] == "":
				self.infohtml = self.infohtml + "\n <td align=center></td>"
			else:
				self.infohtml = self.infohtml + "\n <td align=center><a href=\"plots/%s/%s.png\"><img src=\"plots/%s/%s.th.png\"></a></td>" % (self.id, self.filestem_array[0], self.id, self.filestem_array[0])
			# adding IS combined_plot column
			if self.filestem_array[1] == "":
				self.infohtml = self.infohtml + "\n <td align=center></td>"
			else:
				self.infohtml = self.infohtml + "\n <td align=center><a href=\"plots/%s/%s.png\"><img src=\"plots/%s/%s.th.png\"></a></td>" % (self.id, self.filestem_array[1], self.id, self.filestem_array[1])
			# adding FE combined_plot column
			if self.filestem_array[2] == "":
				self.infohtml = self.infohtml + "\n <td align=center></td>"
			else:
				self.infohtml = self.infohtml + "\n <td align=center><a href=\"plots/%s/%s.png\"><img src=\"plots/%s/%s.th.png\"></a></td>" % (self.id, self.filestem_array[2], self.id, self.filestem_array[2])

			# adding the rest (columns) of the table
			if viewtype == "plots": 
				self.infohtml = self.infohtml + "\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.CSredlocation, self.ISredlocation, self.statusline.replace("-", "&#8211;"), self.archivestatus == "x" and self.archivestatus or "<a href=\"grid/%s.txt\">%s</a>" % (self.id, self.archivestatus))
			if viewtype == "mega":
				self.infohtml = self.infohtml + "\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.oi.nodeslist_string, self.dirsize_string_html, self.totsize, obssetup_html, self.oi.stations_html, self.CSredlocation, self.ISredlocation, self.statusline.replace("-", "&#8211;"), self.archivestatus == "x" and self.archivestatus or "<a href=\"grid/%s.txt\">%s</a>" % (self.id, self.archivestatus))
			if viewtype == "smega":
				self.infohtml = self.infohtml + "\n <td>%s</td>\n <td align=center>%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=left style=\"white-space: nowrap;\">%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.oi.nodeslist_string, self.totsize, obssetup_html, self.oi.stations_html, self.CSredlocation, self.ISredlocation, self.statusline.replace("-", "&#8211;"), self.archivestatus == "x" and self.archivestatus or "<a href=\"grid/%s.txt\">%s</a>" % (self.id, self.archivestatus))

		else: # usual
			if self.comment == "":
				self.infohtml="<td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.oi.datestring, self.oi.duration, self.oi.nodeslist_string, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.FE == "-" and "&#8211;" or self.oi.FE, self.oi.IM == "-" and "&#8211;" or (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD, self.statusline.replace("-", "&#8211;"), self.oi.pointing, self.oi.source, self.get_html_project(self.oi.project))
			else: # no parset file
				self.infohtml="<td>%s</td>\n <td colspan=%d align=center><font color=\"brown\"><b>%s</b></font></td>\n <td>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>\n <td align=center>%s</td>" % (self.id, self.colspan, self.comment, self.oi.nodeslist_string, self.oi.datadir, self.dirsize_string_html, self.totsize, self.oi.FE == "-" and "&#8211;" or self.oi.FE, (self.oi.IM == "+" and "<a style=\"text-decoration:none\" href=\"%s\">+</a>" % (IMredlocation) or self.oi.IM), self.oi.IS == "-" and "&#8211;" or self.oi.IS, self.oi.CS == "-" and "&#8211;" or self.oi.CS, self.oi.BF == "-" and "&#8211;" or self.oi.BF, self.oi.OCD == "-" and "&#8211;" or self.oi.OCD, self.statusline.replace("-", "&#8211;"), self.oi.pointing, self.oi.source, self.get_html_project(self.oi.project))



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
				  <base href=\"%s/%s/\" />\n" % (basehref, basehref_dir))
		self.htmlptr.write ("\
                          	  <title>%s</title>\n" % (htmltitle,))
		self.htmlptr.write ("\
                         	</head>\n\n\
                         	<style type='text/css'>\n\
                          	 tr.d0 td { background-color: #ccffff; color: black; font-size: 80% }\n\
                          	 tr.d1 td { background-color: #99cccc; color: black; font-size: 80% }\n\
                          	 tr.d th { background-color: #99cccc; color: black;}\n\
                         	</style>\n\n\
                         	<body bgcolor='white'>\n")
		self.htmlptr.write ("\
                          	<h2 align=left>%s</h2>\n\n" % (htmltitle,))

	def obsnumber (self, storage_nodes, subclusters, ndbnodes, nnodes, is_new):
		self.nodes_string = ", ".join(storage_nodes)
		self.subs_string = ", ".join(subclusters)
		self.htmlptr.write("Number of observations in %s: <b>%d</b><br>\n" % (self.subs_string, ndbnodes, ))
		if is_new:
			self.htmlptr.write("Number of new observations found in %s: <b>%d</b><br>\n" % (self.subs_string, nnodes))

	def datesrange (self):
		if self.fd == "":
			if self.td != "":
				self.htmlptr.write ("List only observations till %s<br>\n" % (self.td, ))
		else:
			if self.td == "":
				self.htmlptr.write ("List only observations since %s<br>\n" % (self.fd, ))
			else:
				self.htmlptr.write ("List only observations since %s till %s<br>\n" % (self.fd, self.td))

	def statistics (self, link):
		self.htmlptr.write ("<a href=\"%s\">Statistics</a>\n" % (link.split("/")[-1]))

	def header (self, viewtype, storage_nodes_string_html):
		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=3>\n")
		if viewtype == "brief":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>YYYY-MM-DD</th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center>Im</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n</tr>\n")
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>YYYY-MM-DD</th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center>Im</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n")
		elif viewtype == "mega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>YYYY-MM-DD</th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center>Im</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>NodesList (locus)</th>\n <th align=center>%s</th>\n <th align=center>Total (GB)</th>\n <th align=center style=\"white-space: nowrap;\">Obs Setup</th>\n <th align=center style=\"white-space: nowrap;\">Stations</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n" % (storage_nodes_string_html,))
		elif viewtype == "smega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>Source</th>\n <th align=center>YYYY-MM-DD</th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center>Im</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>NodesList (locus)</th>\n <th align=center>Total (GB)</th>\n <th align=center style=\"white-space: nowrap;\">Obs Setup</th>\n <th align=center style=\"white-space: nowrap;\">Stations</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n")
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th>ObsID</th>\n <th align=center>YYYY-MM-DD</th>\n <th align=center>Duration</th>\n <th align=center>NodesList (locus)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center>Total (GB)</th>\n <th align=center>FE</th>\n <th align=center>Im</th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>Status</th>\n <th align=center>Pointing</th>\n <th align=center>Source</th>\n <th align=center>Project</th>\n</tr>\n" % (storage_nodes_string_html,))

	def linkedheader (self, viewtype, storage_nodes_string_html):
		sf=["-obsid.html", "-time.html", "-size.html", "-source.html"]
		sf=["%s%s" % (self.linkedhtmlstem.split("/")[-1], i) for i in sf]
		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=3>\n")
		if viewtype == "brief":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">YYYY-MM-DD</a></th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center><a href=\"%s\">Im</a></th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n</tr>\n" % (sf[0], sf[3], sf[1], imserver))
		elif viewtype == "plots":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">YYYY-MM-DD</a></th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center><a href=\"%s\">Im</a></th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n " % (sf[0], sf[3], sf[1], imserver))
		elif viewtype == "mega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">YYYY-MM-DD</a></th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center><a href=\"%s\">Im</a></th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>NodesList (locus)</th>\n <th align=center>%s</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=center style=\"white-space: nowrap;\">Obs Setup</th>\n <th align=center style=\"white-space: nowrap;\">Stations</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n" % (sf[0], sf[3], sf[1], imserver, storage_nodes_string_html, sf[2]))
		elif viewtype == "smega":
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center><a href=\"%s\">YYYY-MM-DD</a></th>\n <th align=center>Project</th>\n <th align=center>Duration</th>\n <th align=center>Antenna</th>\n <th align=center>Band</th>\n <th align=center>#Stations</th>\n <th align=center>FE</th>\n <th align=center><a href=\"%s\">Im</a></th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>CS/CV Summary</th>\n <th align=center>IS Summary</th>\n <th align=center>FE Summary</th>\n <th align=center>NodesList (locus)</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=center style=\"white-space: nowrap;\">Obs Setup</th>\n <th align=center style=\"white-space: nowrap;\">Stations</th>\n <th align=center>CS/CV Location</th>\n <th align=center>IS Location</th>\n <th align=center>Status</th>\n <th align=center>Archive</th>\n</tr>\n" % (sf[0], sf[3], sf[1], imserver, sf[2]))
		else:
			self.htmlptr.write ("\n<tr class='d' align=left>\n <th>No.</th>\n <th><a href=\"%s\">ObsID</a></th>\n <th align=center><a href=\"%s\">YYYY-MM-DD</a></th>\n <th align=center>Duration</th>\n <th align=center>NodesList (locus)</th>\n <th align=center>Raw Datadir</th>\n <th align=center>%s</th>\n <th align=center><a href=\"%s\">Total (GB)</a></th>\n <th align=center>FE</th>\n <th align=center><a href=\"%s\">Im</a></th>\n <th align=center>IS</th>\n <th align=center>CS</th>\n <th align=center>CV</th>\n <th align=center>OCD</th>\n <th align=center>Status</th>\n <th align=center><a href=\"%s\">Pointing</a></th>\n <th align=center><a href=\"%s\">Source</a></th>\n <th align=center>Project</th>\n</tr>\n" % (sf[0], sf[1], storage_nodes_string_html, sf[2], imserver, sf[3], sf[3]))

	def record (self, lineclass, index, line):
		self.htmlptr.write ("\n<tr class='%s' align=left>\n <td>%d</td>\n %s\n</tr>" % (lineclass, index, line))

	def legend (self):
		# getting date & time of last update
		if self.lupd == "":
			cmd="date +'%b %d, %Y %H:%M:%S %Z'"
			self.lupd=os.popen(cmd).readlines()[0][:-1]
		self.htmlptr.write ("\n</table>\n\n<hr width=100%%>\n<address>\nLast Updated: %s\n</address>\n" % (self.lupd, ))
		self.htmlptr.write ("\n</body>\n</html>")

	def close (self):
		self.htmlptr.close()


# Class that keeps the statistics of all data
class obsstat:
	def __init__(self, ids, fd, td, storage_nodes):
		self.ids = ids
		self.fd = fd
		self.td = td
		self.storage_nodes = storage_nodes
		self.lupd = ""

		self.subclusters = np.append(np.unique([cexec_nodes[s].split(":")[0] for s in self.storage_nodes]), ["locusA", "locus?"])
		self.dbinfo = {}
		for sub in np.append(self.subclusters, ["Total"]):
			self.dbinfo[sub] = {"totDuration": 0.0, "processedDuration": 0.0, "IMonlyDuration": 0.0, "Ntotal": 0, 
                                            "Nprocessed": 0, "Narchived": 0, "Narchived_raw": 0, "Narchived_sub": 0, "Narchived_meta": 0,
                                            "Nistype": 0, "Nistype_only": 0, "Ncstype": 0, "Ncstype_only": 0,
                                            "Nfetype": 0, "Nfetype_only": 0, "Nimtype": 0, "Nimtype_only": 0,
                                            "Nbftype": 0, "Nbftype_only": 0, "Nfdtype": 0, "Nfdtype_only": 0,
                                            "Niscsim": 0, "Nisim": 0, "Niscs": 0, "Ncsim": 0, "Ncsfe": 0, "Nimfe": 0, 
                                            "Nisfe": 0, "Niscsfe": 0, "Nbfis": 0, "Nbffe": 0, "Nbfisfe": 0, "Nocd" : 0, 
                                            "totRawsize": 0.0, "IMonlyRawsize": 0.0, "totProcessedsize": 0.0,
                                            "Archivedsize": 0.0, "Archivedsize_raw": 0.0, "Archivedsize_sub": 0.0, 
                                            "Archivedsize_meta": 0.0 } # sizes in TB

		for sub in self.subclusters:
			if len(self.ids) != 0:
				self.subkeys=np.compress(np.array([obstable[r].subcluster for r in self.ids]) == sub, self.ids)
			else:
				self.subkeys = []
			self.dbinfo[sub]["Ntotal"] += np.size(self.subkeys)
			for r in self.subkeys:
				# getting the numbers and duration
				if obstable[r].comment == "" and obstable[r].oi.duration != "?":
					self.dbinfo[sub]["totDuration"] += obstable[r].oi.dur
				if obstable[r].comment == "" and obstable[r].statusline != "x":
					self.dbinfo[sub]["Nprocessed"] += 1
					if obstable[r].oi.duration != "?":
						self.dbinfo[sub]["processedDuration"] += obstable[r].oi.dur
				# getting the number of archived observations (of all types)
				if obstable[r].comment == "" and obstable[r].archivestatus != "x":
					self.dbinfo[sub]["Narchived"] += 1
					if re.search("raw", obstable[r].archivestatus):
						self.dbinfo[sub]["Narchived_raw"] += 1
						self.dbinfo[sub]["Archivedsize_raw"] += obstable[r].archivesize["raw"]
					if re.search("sub", obstable[r].archivestatus):
						self.dbinfo[sub]["Narchived_sub"] += 1
						self.dbinfo[sub]["Archivedsize_sub"] += obstable[r].archivesize["sub"]
					if re.search("meta", obstable[r].archivestatus):
						self.dbinfo[sub]["Narchived_meta"] += 1
						self.dbinfo[sub]["Archivedsize_meta"] += obstable[r].archivesize["meta"]

				# getting the number of obs of different type
				if obstable[r].comment == "" and obstable[r].oi.IS == "+":
					self.dbinfo[sub]["Nistype"] += 1
					if obstable[r].oi.CS == "-" and obstable[r].oi.FE == "-" and obstable[r].oi.IM == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.BF == "-":
						self.dbinfo[sub]["Nistype_only"] += 1
				if obstable[r].comment == "" and obstable[r].oi.CS == "+":
					self.dbinfo[sub]["Ncstype"] += 1
					if obstable[r].oi.IS == "-" and obstable[r].oi.FE == "-" and obstable[r].oi.IM == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.BF == "-":
						self.dbinfo[sub]["Ncstype_only"] += 1
				if obstable[r].comment == "" and obstable[r].oi.FE == "+":
					self.dbinfo[sub]["Nfetype"] += 1
					if obstable[r].oi.IS == "-" and obstable[r].oi.CS == "-" and obstable[r].oi.IM == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.BF == "-":
						self.dbinfo[sub]["Nfetype_only"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IM == "+":
					self.dbinfo[sub]["Nimtype"] += 1
					if obstable[r].oi.CS == "-" and obstable[r].oi.FE == "-" and obstable[r].oi.IS == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.BF == "-":
						self.dbinfo[sub]["Nimtype_only"] += 1
						self.dbinfo[sub]["IMonlyRawsize"] += float(obstable[r].totsize)
						if obstable[r].oi.duration != "?":
							self.dbinfo[sub]["IMonlyDuration"] += obstable[r].oi.dur
				if obstable[r].comment == "" and obstable[r].oi.BF == "+":
					self.dbinfo[sub]["Nbftype"] += 1
					if obstable[r].oi.CS == "-" and obstable[r].oi.FE == "-" and obstable[r].oi.IS == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IM == "-":
						self.dbinfo[sub]["Nbftype_only"] += 1
				if obstable[r].comment == "" and obstable[r].oi.FD == "+":
					self.dbinfo[sub]["Nfdtype"] += 1
					if obstable[r].oi.CS == "-" and obstable[r].oi.FE == "-" and obstable[r].oi.IS == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.IM == "-":
						self.dbinfo[sub]["Nfdtype_only"] += 1
				# getting the number of some observing types' mixtures
				if obstable[r].comment == "" and obstable[r].oi.IS == "+" and obstable[r].oi.CS == "+" and obstable[r].oi.IM == "+" and obstable[r].oi.FE == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-":
					self.dbinfo[sub]["Niscsim"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IS == "+" and obstable[r].oi.IM == "+" and obstable[r].oi.FE == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.CS == "-":
					self.dbinfo[sub]["Nisim"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IS == "+" and obstable[r].oi.CS == "+" and obstable[r].oi.FE == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IM == "-":
					self.dbinfo[sub]["Niscs"] += 1
				if obstable[r].comment == "" and obstable[r].oi.CS == "+" and obstable[r].oi.IM == "+" and obstable[r].oi.FE == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IS == "-":
					self.dbinfo[sub]["Ncsim"] += 1
				if obstable[r].comment == "" and obstable[r].oi.CS == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.IM == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IS == "-":
					self.dbinfo[sub]["Ncsfe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IS == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.IM == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.CS == "-":
					self.dbinfo[sub]["Nisfe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IM == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.IS == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.CS == "-":
					self.dbinfo[sub]["Nimfe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.IS == "+" and obstable[r].oi.CS == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.IM == "-" and obstable[r].oi.BF == "-" and obstable[r].oi.FD == "-":
					self.dbinfo[sub]["Niscsfe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.BF == "+" and obstable[r].oi.IS == "+" and obstable[r].oi.IM == "-" and obstable[r].oi.CS == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.FE == "-":
					self.dbinfo[sub]["Nbfis"] += 1
				if obstable[r].comment == "" and obstable[r].oi.BF == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.IS == "-" and obstable[r].oi.CS == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IM == "-":
					self.dbinfo[sub]["Nbffe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.BF == "+" and obstable[r].oi.IS == "+" and obstable[r].oi.FE == "+" and obstable[r].oi.CS == "-" and obstable[r].oi.FD == "-" and obstable[r].oi.IM == "-":
					self.dbinfo[sub]["Nbfisfe"] += 1
				if obstable[r].comment == "" and obstable[r].oi.OCD == "+":
					self.dbinfo[sub]["Nocd"] += 1

				# getting the sizes
				if obstable[r].comment == "":
					self.dbinfo[sub]["totRawsize"] += float(obstable[r].totsize)
				if obstable[r].comment == "":
					self.dbinfo[sub]["totProcessedsize"] += float(obstable[r].processed_dirsize)

			self.dbinfo[sub]["totDuration"] /= 3600.
			self.dbinfo[sub]["processedDuration"] /= 3600.
			self.dbinfo[sub]["IMonlyDuration"] /= 3600.
			self.dbinfo[sub]["totRawsize"] /= 1000.
			self.dbinfo[sub]["totProcessedsize"] /= 1000.
			self.dbinfo[sub]["IMonlyRawsize"] /= 1000.
			self.dbinfo[sub]["Archivedsize"] = self.dbinfo[sub]["Archivedsize_raw"] + self.dbinfo[sub]["Archivedsize_sub"] + self.dbinfo[sub]["Archivedsize_meta"]
			self.dbinfo[sub]["Archivedsize"] /= 1000.
			self.dbinfo[sub]["Archivedsize_raw"] /= 1000.
			self.dbinfo[sub]["Archivedsize_sub"] /= 1000.
			self.dbinfo[sub]["Archivedsize_meta"] /= 1000.

		for sub in self.subclusters:
			self.dbinfo["Total"]["totDuration"] += self.dbinfo[sub]["totDuration"]
			self.dbinfo["Total"]["processedDuration"] += self.dbinfo[sub]["processedDuration"]
			self.dbinfo["Total"]["IMonlyDuration"] += self.dbinfo[sub]["IMonlyDuration"]
			self.dbinfo["Total"]["totRawsize"] += self.dbinfo[sub]["totRawsize"]
			self.dbinfo["Total"]["totProcessedsize"] += self.dbinfo[sub]["totProcessedsize"]
			self.dbinfo["Total"]["IMonlyRawsize"] += self.dbinfo[sub]["IMonlyRawsize"]
			self.dbinfo["Total"]["Ntotal"] += self.dbinfo[sub]["Ntotal"]
			self.dbinfo["Total"]["Nprocessed"] += self.dbinfo[sub]["Nprocessed"]
			self.dbinfo["Total"]["Narchived"] += self.dbinfo[sub]["Narchived"]
			self.dbinfo["Total"]["Narchived_raw"] += self.dbinfo[sub]["Narchived_raw"]
			self.dbinfo["Total"]["Narchived_sub"] += self.dbinfo[sub]["Narchived_sub"]
			self.dbinfo["Total"]["Narchived_meta"] += self.dbinfo[sub]["Narchived_meta"]
			self.dbinfo["Total"]["Archivedsize"] += self.dbinfo[sub]["Archivedsize"]
			self.dbinfo["Total"]["Archivedsize_raw"] += self.dbinfo[sub]["Archivedsize_raw"]
			self.dbinfo["Total"]["Archivedsize_sub"] += self.dbinfo[sub]["Archivedsize_sub"]
			self.dbinfo["Total"]["Archivedsize_meta"] += self.dbinfo[sub]["Archivedsize_meta"]
			self.dbinfo["Total"]["Nistype"] += self.dbinfo[sub]["Nistype"]
			self.dbinfo["Total"]["Nistype_only"] += self.dbinfo[sub]["Nistype_only"]
			self.dbinfo["Total"]["Ncstype"] += self.dbinfo[sub]["Ncstype"]
			self.dbinfo["Total"]["Ncstype_only"] += self.dbinfo[sub]["Ncstype_only"]
			self.dbinfo["Total"]["Nfetype"] += self.dbinfo[sub]["Nfetype"]
			self.dbinfo["Total"]["Nfetype_only"] += self.dbinfo[sub]["Nfetype_only"]
			self.dbinfo["Total"]["Nimtype"] += self.dbinfo[sub]["Nimtype"]
			self.dbinfo["Total"]["Nimtype_only"] += self.dbinfo[sub]["Nimtype_only"]
			self.dbinfo["Total"]["Nbftype"] += self.dbinfo[sub]["Nbftype"]
			self.dbinfo["Total"]["Nbftype_only"] += self.dbinfo[sub]["Nbftype_only"]
			self.dbinfo["Total"]["Nfdtype"] += self.dbinfo[sub]["Nfdtype"]
			self.dbinfo["Total"]["Nfdtype_only"] += self.dbinfo[sub]["Nfdtype_only"]
			self.dbinfo["Total"]["Niscsim"] += self.dbinfo[sub]["Niscsim"]
			self.dbinfo["Total"]["Nisim"] += self.dbinfo[sub]["Nisim"]
			self.dbinfo["Total"]["Niscs"] += self.dbinfo[sub]["Niscs"]
			self.dbinfo["Total"]["Ncsim"] += self.dbinfo[sub]["Ncsim"]
			self.dbinfo["Total"]["Ncsfe"] += self.dbinfo[sub]["Ncsfe"]
			self.dbinfo["Total"]["Nimfe"] += self.dbinfo[sub]["Nimfe"]
			self.dbinfo["Total"]["Nisfe"] += self.dbinfo[sub]["Nisfe"]
			self.dbinfo["Total"]["Niscsfe"] += self.dbinfo[sub]["Niscsfe"]
			self.dbinfo["Total"]["Nbfis"] += self.dbinfo[sub]["Nbfis"]
			self.dbinfo["Total"]["Nbffe"] += self.dbinfo[sub]["Nbffe"]
			self.dbinfo["Total"]["Nbfisfe"] += self.dbinfo[sub]["Nbfisfe"]
			self.dbinfo["Total"]["Nocd"] += self.dbinfo[sub]["Nocd"]
			

	def printstat (self):
		print
		print "%sstatistics:" % (htmltitle, )
		if self.fd != "" or self.td != "":
			print "[%s%s]" % (self.fd != "" and "from " + self.fd or (self.td != "" and " till " + self.td or ""), 
                                                                    self.td != "" and (self.fd != "" and " till " + self.td or "") or "")
		ruler_size=53+23*(1 + np.size(self.subclusters))
		rulers=[]
		for r in np.arange(ruler_size): rulers = np.append(rulers, "-")
		rulerstring="".join(rulers) + "-"
		print rulerstring
		header_string="                                                     "
		for sub in np.append("Total", self.subclusters):
			symbols=[]
			for r in np.arange(23-len(sub)): symbols = np.append(symbols, " ")
			header_string += "%s%s" % (sub, "".join(symbols))
		print header_string
		print rulerstring

		line="Total number of observations [hours / days]:         "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%.1f / %.1f]" % (self.dbinfo[sub]["Ntotal"], self.dbinfo[sub]["totDuration"], self.dbinfo[sub]["totDuration"]/24.)
			line += "%-23s" % (field)
		print line
		line="Number of observations w/o Im-only [hours / days]:   "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%.1f / %.1f]" % (self.dbinfo[sub]["Ntotal"]-self.dbinfo[sub]["Nimtype_only"], self.dbinfo[sub]["totDuration"]-self.dbinfo[sub]["IMonlyDuration"], (self.dbinfo[sub]["totDuration"]-self.dbinfo[sub]["IMonlyDuration"])/24.)
			line += "%-23s" % (field)
		print line
		line="Number of processed observations [hours / days]:     "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%.1f / %.1f]" % (self.dbinfo[sub]["Nprocessed"], self.dbinfo[sub]["processedDuration"], self.dbinfo[sub]["processedDuration"]/24.)
			line += "%-23s" % (field)
		print line
		line="Number of archived observations [raw / sub / meta]:  "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d / %d / %d]" % (self.dbinfo[sub]["Narchived"], self.dbinfo[sub]["Narchived_raw"], self.dbinfo[sub]["Narchived_sub"], self.dbinfo[sub]["Narchived_meta"])
			line += "%-23s" % (field)
		print line

		print

		line="Number of IS observations [only IS]:                 "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d]" % (self.dbinfo[sub]["Nistype"], self.dbinfo[sub]["Nistype_only"])
			line += "%-23s" % (field)
		print line
		line="Number of IS+CS observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Niscs"])
			line += "%-23s" % (field)
		print line
		line="Number of IS+Im observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nisim"])
			line += "%-23s" % (field)
		print line
		line="Number of IS+CS+Im observations only:                "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Niscsim"])
			line += "%-23s" % (field)
		print line
		line="Number of CS observations [only CS]:                 "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d]" % (self.dbinfo[sub]["Ncstype"], self.dbinfo[sub]["Ncstype_only"])
			line += "%-23s" % (field)
		print line
		line="Number of CS+Im observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Ncsim"])
			line += "%-23s" % (field)
		print line
		line="Number of FE observations [only FE]:                 "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d]" % (self.dbinfo[sub]["Nfetype"], self.dbinfo[sub]["Nfetype_only"])
			line += "%-23s" % (field)
		print line
		line="Number of FE+CS observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Ncsfe"])
			line += "%-23s" % (field)
		print line
		line="Number of FE+IS observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nisfe"])
			line += "%-23s" % (field)
		print line
		line="Number of FE+Im observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nimfe"])
			line += "%-23s" % (field)
		print line
		line="Number of FE+IS+CS observations only:                "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Niscsfe"])
			line += "%-23s" % (field)
		print line
		line="Number of Im observations [only Im]:                 "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d]" % (self.dbinfo[sub]["Nimtype"], self.dbinfo[sub]["Nimtype_only"])
			line += "%-23s" % (field)
		print line
		line="Number of CV observations [only CV]:                 "
		for sub in np.append("Total", self.subclusters):
			field = "%d [%d]" % (self.dbinfo[sub]["Nbftype"], self.dbinfo[sub]["Nbftype_only"])
			line += "%-23s" % (field)
		print line
		line="Number of CV+IS observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nbfis"])
			line += "%-23s" % (field)
		print line
		line="Number of CV+FE observations only:                   "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nbffe"])
			line += "%-23s" % (field)
		print line
		line="Number of CV+IS+FE observations only:                "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nbfisfe"])
			line += "%-23s" % (field)
		print line
		line="Number of observations with OCD:                     "
		for sub in np.append("Total", self.subclusters):
			field = "%d" % (self.dbinfo[sub]["Nocd"])
			line += "%-23s" % (field)
		print line

		print

		line="Total size of raw data (TB):                         "
		for sub in np.append("Total", self.subclusters):
			field = "%.1f" % (self.dbinfo[sub]["totRawsize"])
			line += "%-23s" % (field)
		print line
		line="Total size of raw data w/o Im-only (TB):             "
		for sub in np.append("Total", self.subclusters):
			field = "%.1f" % (self.dbinfo[sub]["totRawsize"]-self.dbinfo[sub]["IMonlyRawsize"])
			line += "%-23s" % (field)
		print line
		line="Total size of processed data (TB):                   "
		for sub in np.append("Total", self.subclusters):
			field = "%.1f" % (self.dbinfo[sub]["totProcessedsize"])
			line += "%-23s" % (field)
		print line
		line="Total size of archived data (TB) [raw / sub / meta]: "
		for sub in np.append("Total", self.subclusters):
			field = "%.1f [%.1f / %.1f / %.1f]" % (self.dbinfo[sub]["Archivedsize"], self.dbinfo[sub]["Archivedsize_raw"], self.dbinfo[sub]["Archivedsize_sub"], self.dbinfo[sub]["Archivedsize_meta"])
			line += "%-23s" % (field)
		print line
		print


	def printhtml (self, htmlfile):
		self.htmlptr = open(htmlfile, 'w')
		self.htmlptr.write ("<html>\n\
                	         <head>\n\
                                  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n\
                          	  <meta name=\"Classification\" content=\"public HTML\">\n\
                                  <meta name=\"robots\" content=\"noindex, nofollow\">\n\
				  <base href=\"%s/%s/\" />\n" % (basehref, basehref_dir))
		self.htmlptr.write ("\
                          	  <title>%s%s</title>\n" % (htmltitle, "statistics"))
		self.htmlptr.write ("\
                         	</head>\n\n\
                         	<style type='text/css'>\n\
                          	 tr.d0 td { background-color: #ccffff; color: black; font-size: 100% }\n\
                          	 tr.d1 td { background-color: #99cccc; color: black; font-size: 100% }\n\
                          	 tr.d th { background-color: #99cccc; color: black;}\n\
                         	</style>\n\n\
                         	<body bgcolor='white'>\n")
		self.htmlptr.write ("\
                                <h2 align=left>%s%s</h2>\n\n" % (htmltitle, "statistics"))
		# adding links for evolution of disk volume and number of obs with time
		if is_statevol_links:
			self.htmlptr.write ("\
                                <a href=\"archive/number.png\">Number of observations over time</a><br>\n\
				<a href=\"archive/volume.png\">Data volume over time</a><br>\n\n")

		if self.fd != "" or self.td != "":
			self.htmlptr.write ("\n<h3>%s%s</h3>" % (self.fd != "" and "From " + self.fd or (self.td != "" and " Till " + self.td or ""), 
                                                                    self.td != "" and (self.fd != "" and " Till " + self.td or "") or ""))

		self.htmlptr.write ("\n<p align=left>\n<table border=0 cellspacing=0 cellpadding=5>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left width=400></td>")
		for sub in np.append("Total", self.subclusters):	
			self.htmlptr.write ("\n <td align=left><b>%s</b></td>" % (sub))	
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font> / <font color=\"green\"><b>%s</b></font>]</td>" % ("Total number of observations", "hours", "days"))
		for sub in np.append("Total", self.subclusters):	
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%.1f</b></font> / <font color=\"green\"><b>%.1f</b></font>]</td>" % (self.dbinfo[sub]["Ntotal"], self.dbinfo[sub]["totDuration"], self.dbinfo[sub]["totDuration"]/24.))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font> / <font color=\"green\"><b>%s</b></font>]</td>" % ("Number of observations w/o Im-only", "hours", "days"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%.1f</b></font> / <font color=\"green\"><b>%.1f</b></font>]</td>" % (self.dbinfo[sub]["Ntotal"]-self.dbinfo[sub]["Nimtype_only"], self.dbinfo[sub]["totDuration"]-self.dbinfo[sub]["IMonlyDuration"], (self.dbinfo[sub]["totDuration"]-self.dbinfo[sub]["IMonlyDuration"])/24.))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font> / <font color=\"green\"><b>%s</b></font>]</td>" % ("Number of processed observations", "hours", "days"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%.1f</b></font> / <font color=\"green\"><b>%.1f</b></font>]</td>" % (self.dbinfo[sub]["Nprocessed"], self.dbinfo[sub]["processedDuration"], self.dbinfo[sub]["processedDuration"]/24.))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font> / <font color=\"green\"><b>%s</b></font> / <font color=\"darkblue\"><b>%s</b></font>]</td>" % ("Number of archived observations", "raw", "sub", "meta"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d</b></font> / <font color=\"green\"><b>%d</b></font> / <font color=\"darkblue\"><b>%d</b></font>]</td>" % (self.dbinfo[sub]["Narchived"], self.dbinfo[sub]["Narchived_raw"], self.dbinfo[sub]["Narchived_sub"], self.dbinfo[sub]["Narchived_meta"]))
		self.htmlptr.write ("\n</tr>")

		self.htmlptr.write ("\n<tr align=left height=25>\n <td align=left></td>")
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left></td>")
		self.htmlptr.write ("\n</tr>")

		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font>]</td>" % ("Number of IS observations", "only IS"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d</b></font>]</td>" % (self.dbinfo[sub]["Nistype"], self.dbinfo[sub]["Nistype_only"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of IS+CS observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Niscs"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Number of IS+Im observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nisim"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of IS+CS+Im observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Niscsim"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font>]</td>" % ("Number of CS observations", "only CS"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d<b></font>]</td>" % (self.dbinfo[sub]["Ncstype"], self.dbinfo[sub]["Ncstype_only"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of CS+Im observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Ncsim"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font>]</td>" % ("Number of FE observations", "only FE"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d</b></font>]</td>" % (self.dbinfo[sub]["Nfetype"], self.dbinfo[sub]["Nfetype_only"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of FE+CS observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Ncsfe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Number of FE+IS observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nisfe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of FE+Im observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nimfe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Number of FE+IS+CS observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Niscsfe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font>]</td>" % ("Number of Im observations", "only Im"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d</b></font>]</td>" % (self.dbinfo[sub]["Nimtype"], self.dbinfo[sub]["Nimtype_only"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font>]</td>" % ("Number of CV observations", "only CV"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b> [<font color=\"brown\"><b>%d</b></font>]</td>" % (self.dbinfo[sub]["Nbftype"], self.dbinfo[sub]["Nbftype_only"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of CV+IS observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nbfis"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Number of CV+FE observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nbffe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Number of CV+IS+FE observations only"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nbfisfe"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Number of observations with OCD"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%d</b></td>" % (self.dbinfo[sub]["Nocd"]))
		self.htmlptr.write ("\n</tr>")

		self.htmlptr.write ("\n<tr align=left height=25>\n <td align=left></td>")
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left></td>")
		self.htmlptr.write ("\n</tr>")

		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Total size of raw data (TB)"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%.1f</b></td>" % (self.dbinfo[sub]["totRawsize"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s</td>" % ("Total size of raw data w/o Im-only (TB)"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%.1f</b></td>" % (self.dbinfo[sub]["totRawsize"]-self.dbinfo[sub]["IMonlyRawsize"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d1' align=left>\n <td align=left>%s</td>" % ("Total size of processed data (TB)"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%.1f</b></td>" % (self.dbinfo[sub]["totProcessedsize"]))
		self.htmlptr.write ("\n</tr>")
		self.htmlptr.write ("\n<tr class='d0' align=left>\n <td align=left>%s [<font color=\"brown\"><b>%s</b></font> / <font color=\"green\"><b>%s</b></font> / <font color=\"darkblue\"><b>%s</b></font>]</td>" % ("Total size of archived data (TB)", "raw", "sub", "meta"))
		for sub in np.append("Total", self.subclusters):
			self.htmlptr.write ("\n <td align=left><b>%.1f</b> [<font color=\"brown\"><b>%.1f</b></font> / <font color=\"green\"><b>%.1f</b></font> / <font color=\"darkblue\"><b>%.1f</b></font>]</td>" % (self.dbinfo[sub]["Archivedsize"], self.dbinfo[sub]["Archivedsize_raw"], self.dbinfo[sub]["Archivedsize_sub"], self.dbinfo[sub]["Archivedsize_meta"]))
		self.htmlptr.write ("\n</tr>")

		self.htmlptr.write ("\n</table>")
                # getting date & time of last update
                if self.lupd == "":
                        cmd="date +'%b %d, %Y %H:%M:%S %Z'"
                        self.lupd=os.popen(cmd).readlines()[0][:-1]
                self.htmlptr.write ("\n\n<hr width=100%%>\n<address>\nLast Updated: %s\n</address>\n" % (self.lupd, ))
		self.htmlptr.write ("\n</body>\n</html>")
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
                                       in addition to start time, duration, source, etc. fields that present in all formats.\n\
                                       Second <mode> is \"brief\" that lists antenna array, band filter, number of stations\n\
                                       used. Third <mode> is \"plots\" which is the same as \"brief\" mode in ascii output,\n\
                                       but in html-format it also provides the profile summaries together with chi-squared\n\
                                       values of profiles. Fourth <mode> is \"mega\" which is huge\n\
                                       table like for \"plots\" but also with all other info from \"usual\" mode together with\n\
                                       list of all stations. Fifth <mode> is \"smega\" which is shortened \"mega\", the same as\n\
                                       \"mega\" without datasize for each of the node, only the total size of data is shown.\n\
                                       All view modes are: \"usual\" (default), \"brief\", \"plots\", \"mega\", and \"smega\"\n\
          -r, --rebuild              - reprocess all observations from scratch (can take a while) rather than to read\n\
                                       the existent database, process obs that do not exist there, and add them to the database\n\
          -u, --update               - update db file only, new observations in /data? won't be added\n\
                                       This option can be used together with --from and --to to update only some observations\n\
          -a, --append               - process only those obs (usually new ones) that are not in the database\n\
          --delete                   - delete ObsIDs from db file listed in --obsids and/or meet --from, --to conditions\n\
          --obsids <ObsIDs>          - set the list of ObsIDs to be updated or accessed. This option can be used only with either\n\
                                       -u or without all of -r, -u, and -a. ObsIDs should be separated by comma with no spaces,\n\
                                       range symbol '-' is not allowed\n\
          --search <expr>            - under development... Conditions can be combined using 'and' and 'or'. Spaces around them are\n\
                                       crucial. Parentheses and 'not' are not allowed. Search parameter should start with 'id.'\n\
                                       For boolean parameters, you should use 'True' and 'False'. Some of the possible parameters are:\n\
                                       id.is_test (boolean, if True then this is system test observation), id.source, id.pointing,\n\
                                       id.id, is.duration, id.antenna, id.band, id.subcluster, id.datadir, id.nodelist_string (string),\n\
                                       id.nstations, id.ncorestations, id.nremotestations (integer), id.dur (float, duration in seconds)\n\
                                       id.rarad, id.decrad (float, RA and DEC in radians), id.IS, id.CS, id.FE, id.IM, id.BF, id.FD\n\
                                       (characters, '+', '-', or '?')\n\
          --stats                    - to calculate the statistics of existent observations in the database\n\
                                       can be used together with --from and --to options, and with --html option\n\
          --hostdir <dir>            - directory relative to which plots dir, grid dir, and dumpfile will be created. Default: Lofar\n\
                                       If you use both --hostdir and --dbfile, make sure that --dbfile stands later in the command line\n\
          --dbfile <dbfile>          - database file with stored info about the observations\n\
          --basehrefdir <dir>        - the base directory on remote webserver where html files, all plots and grid ascii files will be copied\n\
                                       to corresponding plots/ and grid/ directories. This directory is relative to project root directory\n\
                                       on the webserver. Default is 'lofarpwg'\n\
          --htmltitle <str>          - the title for the html page. Default is 'LOFAR pulsar observations '\n\
          --statevol-links           - on the statistics' page adds links to plots of evolution of number of observations\n\
                                       and disk volume with time\n\
          --no_check_rawdata         - do not check nodes list in the parset file to see of raw data do exist there and do not search all given\n\
                                       storage nodes to find raw data if necessary. This will speed up the info gathering\n\
          --norsync                  - don't rsync plots and grid links to external webserver when \"mega\" or \"plots\" view mode is used\n\
          --debug                    - debug mode\n\
          -h, --help                 - print this message\n" % (prg, )




# Parse the command line
def parsecmd(prg, argv):
        """ Parsing the command line
        """

	global dumpfile
	try:
		opts, args = getopt.getopt (argv, "hf:t:v:rua", ["help", "sort=", "from=", "html=", "to=", "lse=", "view=", "linkedhtml=", "rebuild", "update", "delete", "append", "debug", "stats", "dbfile=", "norsync", "obsids=", "search=", "hostdir=", "basehrefdir=", "htmltitle=", "statevol-links", "no_check_rawdata"])
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
						lsenodes = np.append(lsenodes, "locus%03d" % (int(s),))
					else:
						for l in np.arange(int(s.split("-")[0]), int(s.split("-")[1])+1):
							lsenodes = np.append(lsenodes, "locus%03d" % (l,))	
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
			if opt in ("--delete"):
				global is_delete
				is_delete = True
			if opt in ("-a", "--append"):
				global is_append
				is_append = True
			if opt in ("--obsids"):
				if arg.isspace() == True:
					print "--obsids option has spaces that is not allowed"
					sys.exit()
				global update_obsids
				for s in arg.split(","):
					if s.count("-") == 0:
						update_obsids = np.append(update_obsids, s)
					else:
						print "The range in --obsids is not allowed"
						sys.exit()
			if opt in ("--debug"):
				global is_debug
				is_debug = True
			if opt in ("--stats"):
				global is_stats
				is_stats = True
			if opt in ("--dbfile"):
				dumpfile = arg
			if opt in ("--norsync"):
				global is_torsync
				is_torsync = False
			if opt in ("--search"):
				global search_string
				search_string = arg
			if opt in ("--statevol-links"):
				global is_statevol_links
				is_statevol_links = True
			if opt in ("--no_check_rawdata"):
				global is_no_check_rawdata
				is_no_check_rawdata = True
			if opt in ("--basehrefdir"):
				global basehref_dir
				global webplotsdir
				webplotsdir = re.sub("/" + basehref_dir + "/", "/" + arg + "/", webplotsdir)
				global webgriddir
				webgriddir = re.sub("/" + basehref_dir + "/", "/" + arg + "/", webgriddir)
				basehref_dir = arg
			if opt in ("--htmltitle"):
				global htmltitle
				htmltitle = arg
			if opt in ("--hostdir"):
				global hostdir
				global plotsdir
				plotsdir = re.sub("/" + hostdir + "/", "/" + arg + "/", plotsdir)
				global griddir
				griddir = re.sub("/" + hostdir + "/", "/" + arg + "/", griddir)
				dumpfile = re.sub("/" + hostdir + "/", "/" + arg + "/", dumpfile)
				hostdir = arg

	except getopt.GetoptError:
		print "Wrong option!"
		usage(prg)
		sys.exit(2)

# parsing the condition and form the real condition to check
def get_condition (cond):
        """ Parsing the search condition and return the real condition to check
        """
	param=cond.lstrip().split(" ")[0].split("id.")[1]
	if not hasattr (obstable[obskeys[0]].oi, param.split("[")[0]):
		print "Wrong parameter '%s' in the search string!" % (param,)
		sys.exit(1)
	# making the real condition to filter out obsids
	real_cond = re.sub("id", "np.compress(np.array([obstable[r].oi", cond)
	# this is necessary if we want to compare only some characters in the string (e.g. source)
	# and we need to backslash [ and ]
	wparam = re.sub("\]", "\\]", re.sub("\[", "\\[", param))
	real_cond = re.sub(wparam, param + " for r in filtered_obsids])", real_cond)
	real_cond = real_cond + ", filtered_obsids)"
	return real_cond


# checking the internet (cexec) connection to locus nodes
# if node does not respond, exclude it from the storage_nodes
# in debug mode, we print message to stdout that this node is not responding
# the list of available locus nodes is returned
def check_connection (storage_nodes, is_debug):
	up_nodes = []
	if is_debug:
		print "checking network connection to locus nodes..."
	# forming string with all locus nodes to check in one cexec command
	cexeclocus=cexec_nodes[storage_nodes[0]] # there is always at least one storage node
	if len(storage_nodes) > 1:
		for s in storage_nodes[1:]:
			cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
	cmd="%s %s date | grep -v xauth | grep -v connect | grep -v Permission | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus)
	cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
	# finding all locus nodes that have the dir with raw data
	for l in np.arange(len(cexec_output)):
		if re.match("^-----", cexec_output[l]) is None:
			up_nodes.append(cexec_output[l-1].split(" ")[1])

	if is_debug:
		down_nodes=list(set(storage_nodes)-set(storage_nodes).intersection(set(up_nodes)))
		print "Nodes are down: %s" % (", ".join(down_nodes))
	return up_nodes


# getting human-readable size (in GB, MB, or KB) from size in bytes
def human_readable_size (bytesize):
	for s in ['', 'K', 'M', 'G', 'T', 'P']:
		if bytesize < 1000.:
			return "%3.1f%s" % (bytesize, s)
		bytesize /= 1000.
	return "O_O"


# to save db to disk and copy plot and gridarchive files to the webserver
def save_db_copy_files (dumpfile, obstable):
	# saving the database to disk
	if is_rebuild or is_update or is_append or is_delete:
		# dump obs table to the file
		dfdescr = open (dumpfile, "w")
		cPickle.dump(obstable, dfdescr, True)
		dfdescr.close()

		# uploading the png files to webserver
		if is_torsync:
			if viewtype == 'plots' or viewtype == 'mega' or viewtype == 'smega':
				cmd="ssh %s mkdir -p %s" % (webserver, webplotsdir)
				os.system(cmd)
				cmd="rsync -a %s/ %s:%s 2>&1 1>/dev/null" % (plotsdir, webserver, webplotsdir)
				os.system(cmd)
				cmd="ssh %s mkdir -p %s" % (webserver, webgriddir)
				os.system(cmd)
				cmd="rsync -a %s/ %s:%s 2>&1 1>/dev/null" % (griddir, webserver, webgriddir)
				os.system(cmd)
	

###################################################################################################################
#          M A I N                                                                                                #
###################################################################################################################
if __name__ == "__main__":

	# parsing command line
	parsecmd (sys.argv[0].split("/")[-1], sys.argv[1:])

	if (is_rebuild and is_update) or (is_rebuild and is_append) or (is_rebuild and is_delete) or (is_update and is_append) or (is_update and is_delete) or (is_append and is_delete):
		print "Only one action can be done at a time: rebuild (-r), update (-u), append (-a), or delete (--delete)"
		print "To just access the db don't use any of -r, -u, -a, or --delete"
		sys.exit()

	# if only statistics is required, then switiching off all other keys
	if is_stats:
		is_rebuild = is_update = is_append = is_delete = False	

	# checking the internet (cexec) connection to locus nodes
	# if node does not respond, exclude it from the storage_nodes
	# in debug mode, we print message to stdout that this node is not responding
	# only checking connection if we do rebuild, update, or append
	if is_rebuild or is_update or is_append:
		storage_nodes = check_connection (storage_nodes, is_debug)
		if len(storage_nodes) == 0:
			print "The connection to all locus nodes is down. Try again later"
			sys.exit(1)

	# list of subclusters
	subclusters = np.unique([cexec_nodes[s].split(":")[0] for s in storage_nodes])

	# table with obs info
	# key is the ObsId, and value is outputInfo class instance
	obstable={}

	# creating plotsdir directory if it does not exist
	cmd="mkdir -p %s" % (plotsdir, )
	os.system(cmd)
	# creating griddir directory if it does not exist
	cmd="mkdir -p %s" % (griddir, )
	os.system(cmd)

	# reading the grid links from common file to the list
	if os.path.exists(archivefile):
		gridfiles, griddates, gridtimes, gridsizes, gridlinks = np.loadtxt(archivefile, usecols=(0,1,2,3,4), dtype=str, unpack=True, comments='#')
		# converting sizes to float and checking those also that are not yet determined, i.e. have '-'
		gridsizes = [r != '-' and float(r) or 0.0 for r in gridsizes]

	# do not need the db when do --rebuild
	if not is_rebuild:
		if not os.path.exists(dumpfile):
			print "DB file \'%s\' does not exist! Use -r option to rebuild the database." % (dumpfile, )
			sys.exit()
		else:  # reading the DB file
			dfdescr = open(dumpfile, "r")
			obstable=cPickle.load(dfdescr)
			dfdescr.close()
			# update info and infohtml depending on current command line options and make the sorted list of DB obsids
			dbobsids = np.flipud(np.sort(obstable.keys(), kind='mergesort'))
			for r in dbobsids:
				obstable[r].update(storage_nodes)

	###!!!!!!!!
	# Here, for statistics we also have to add all conditions!!!
	#######
	if is_stats:
		obsids = dbobsids
		# we also have to choose only those IDs within the desired time range
		# if --from and/or --to are specified
		if is_from:
			fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
			obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) >= fromsecs, obsids))
		if is_to:
			tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d")) + 86399
			obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) <= tosecs, obsids))

		# initializing statistics' class
		stat = obsstat(obsids, fromdate, todate, storage_nodes)
		stat.printstat()
		if is_html:
			stat.printhtml(htmlfile)
		sys.exit(0)
		########## end of statistics #############

	if is_rebuild or is_update or is_append:
		# loop over the storage nodes and directories to get the list of all IDs
		# Even for update (--update option) we still need to collect ObsIDs to form
		# redonly ObsIDs and rawonly ObsIDs, to more effectively update ObsIDs from the db

		# forming string with all locus nodes to check in one cexec command	
		cexeclocus=cexec_nodes[storage_nodes[0]] # there is always at least one storage node
		if len(storage_nodes) > 1:
			for s in storage_nodes[1:]:
				cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
		for d in data_dirs:
			cmd="%s %s 'find %s -maxdepth 1 -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | grep -v xauth | grep -v connect | %s" % (cexeccmd, cexeclocus, d, "?20??_*", cexec_egrep_string)
			indlist=[i.split("/")[-1][:-1] for i in os.popen(cmd).readlines()]
			obsids = np.append(obsids, indlist)
			# also now (May 4, 2011) we start to search in L????? directories
			# as the datadir naming convention was changed!!! Damn!!!!!!
			cmd="%s %s 'find %s -maxdepth 1 -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | grep -v xauth | grep -v connect | %s" % (cexeccmd, cexeclocus, d, "L?????", cexec_egrep_string)
			indlist=[i.split("/")[-1][:-1] for i in os.popen(cmd).readlines()]
			obsids = np.append(obsids, indlist)

		# getting unique list of ObsIDs based only on raw data dirs
		obsids=np.unique(obsids)

		# also checking the archive directories to extend the list of ObsIDs in case the raw data was removed
		# Here is the way how to combine both _red and _lta in one find request and also how to exclude with grep
		# all the directories that have any other symbols except for 5 or more digits between "_red" or "_lta" and the
		# previous undescore
		# find ./ -type d \( -name "?20??_*_red" -print , -name "?20??_*_lta" -print \) | grep -E '20[0-9][0-9]_[0-9]{5,}_(red|lta)'
		# Can not, however, easily combine _red and _lta in one request, because then I won't be able to run
		# split command to get the list of ObsIDs
		# Decided to allow additional text before _red or _lta
		# On CEPII archive areas with all IS and CS data will be on 'hoover' nodes, so I don't need to search all locus nodes
		obsids_reds=[]
		test_obsids=[] # the list of ObsIDs that have reduced data in 'test_dir' - we mark them as TEST
	
		# checking first locus101 (hoover_nodes[0])
#		lse=hoover_nodes[0]
		lse="locus092"
		# getting the list of *_CSplots directories to get a list of ObsIDs
		cmd="%s %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | grep -v xauth | grep -v connect | %s" % (cexeccmd, cexec_nodes[lse], psr_archive_dir + lse, "*_CSplots", cexec_egrep_string)
		dirlist = os.popen(cmd).readlines()
		obsids_reds = np.append(obsids_reds, [i.split("/")[-1].split("_CSplots")[0] for i in dirlist])
		test_obsids = np.append(test_obsids, [i.split("/")[-1].split("_CSplots")[0] for i in dirlist if re.search(test_dir, i)])

		# then checking locus102 (hoover_nodes[1])
#		lse=hoover_nodes[1]
		lse="locus094"
		# and getting the list of *_redIS directories to get a list of ObsIDs
		cmd="%s %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | grep -v xauth | grep -v connect | %s" % (cexeccmd, cexec_nodes[lse], psr_archive_dir + lse, "*_redIS", cexec_egrep_string)
		dirlist = os.popen(cmd).readlines()
		obsids_reds = np.append(obsids_reds, [i.split("/")[-1].split("_redIS")[0] for i in dirlist])
		test_obsids = np.append(test_obsids, [i.split("/")[-1].split("_redIS")[0] for i in dirlist if re.search(test_dir, i)])

		# then checking for CV data
		lse="locus093"
		# getting the list of *_CSplots directories to get a list of ObsIDs
		cmd="%s %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | grep -v xauth | grep -v connect | %s" % (cexeccmd, cexec_nodes[lse], psr_archive_dir + lse, "*_CVplots", cexec_egrep_string)
		dirlist = os.popen(cmd).readlines()
		obsids_reds = np.append(obsids_reds, [i.split("/")[-1].split("_CVplots")[0] for i in dirlist])
		test_obsids = np.append(test_obsids, [i.split("/")[-1].split("_CVplots")[0] for i in dirlist if re.search(test_dir, i)])

			# also getting the list of *_lta directories to get a list of ObsIDs
			# in case if data was already processed and archived
#### commented by now, because we do not yet archive data on CEPII
#			cmd="cexec %s 'find %s -type d -name \"%s\" -print 2>/dev/null' 2>/dev/null | grep -v Permission | grep -v such | %s" % (cexec_nodes[s], psr_archive_dir + s, "*_lta", cexec_egrep_string)
#			dirlist = os.popen(cmd).readlines()
#			obsids_reds = np.append(obsids_reds, [i.split("/")[-1].split("_lta")[0] for i in dirlist])
#			test_obsids = np.append(test_obsids, [i.split("/")[-1].split("_lta")[0] for i in dirlist if re.search(test_dir, i)])

		# getting unique list of ObsIDs based only on archived data
		obsids_reds=np.unique(obsids_reds)
		test_obsids=np.unique(test_obsids)

		# getting the list of ObsIDs that _DO NOT HAVE_ raw data
		# based on this list we will not check the raw data (sizes) for ObsIDs in this list
		obsids_redonly=list(set(obsids_reds)-set(obsids_reds).intersection(set(obsids)))
		# getting the list of ObsIDs that _DO NOT HAVE_ Reduced data!
		# based on this list we will not look for reduced data, processing status, plots, etc. for ObsIDs in this list
		obsids_rawonly=list(set(obsids)-set(obsids).intersection(set(obsids_reds)))

		# for rebuild and append mode only
		if not is_update:
			# merging obsids and obsids_reds lists together and
			# getting the unique list of IDs (some of IDs can have entries in many /data? and nodes)
			# and sort in reverse order (most recent obs go first)
			# more recent obs is the obs with higher ID (as it should be)
			obsids = np.flipud(np.sort(np.unique(np.append(obsids, obsids_reds)), kind='mergesort'))

			# if is_rebuild == False then excluding ObsIDs from obsids list that are already in the database, i.e. in dbobsids list
			# only new ObsIDs will be processed and added to database
			if not is_rebuild:
				# now obsids have only those IDs that are not in the dump file
				obsids=list(set(obsids)-set(obsids).intersection(set(dbobsids)))
		else:   # update mode
			if np.size(update_obsids) == 0:  # list of ObsIDs to update is not specified
				obsids = dbobsids
				# for the db update we also have to choose only those IDs within the desired time range
				# if --from and/or --to are specified
				if is_from:
					fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) >= fromsecs, obsids))

				if is_to:
					tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d")) + 86399
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) <= tosecs, obsids))
			else:
				obsids = update_obsids


	if is_update:
		# also db can have obsids from all subclusters. However, we need not update all of them, only those
		# from the same subclusters as for lse in the command line option. This is because, the info from the parset
		# files is always the same, and do not need to be updated, and update of the processed data status can only
		# have sense for the same subclusters as in the command line option
		# also, we add 'subA' and 'sub?' as well
		if np.size(obsids) != 0:
			newobsids=[]
			for sub in subclusters:
				newobsids=np.append(newobsids, list(np.compress(np.array([obstable[r].subcluster for r in obsids]) == sub, obsids)))
			newobsids=np.append(newobsids, list(np.compress(np.array([obstable[r].subcluster for r in obsids]) == "locusA", obsids)))
			newobsids=np.append(newobsids, list(np.compress(np.array([obstable[r].subcluster for r in obsids]) == "locus?", obsids)))
			newobsids = np.flipud(np.sort(np.unique(newobsids), kind='mergesort'))
			obsids = newobsids

	# removing selected ObsIDs from db file
	if is_delete:
		try:  # use try block in case some ObsIDs are not in the db file
			if np.size(update_obsids) == 0:  # list of ObsIDs to delete is not specified
				obsids = dbobsids
				if is_from:
					fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) >= fromsecs, obsids))
				if is_to:
					tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d")) + 86399
					obsids=list(np.compress(np.array([obstable[r].seconds for r in obsids]) <= tosecs, obsids))
				for id in obsids:
					del obstable[id]
				obsids = []
			else: # removing specific ObsIDs from the --obsids list
				for id in update_obsids:
					del obstable[id]
		except: pass

	# Number of ObsIDs
	Nobsids = np.size(obsids)

	try:
		# Main loop for every observation
		for id in obsids:
			# i don't know why people make these obsids!!!
			if id == "L?????": continue
		
			# if we do update then we do not need to initialize obsinfo class again, unless
			# parset file does not exist
			if is_update and obstable[id].oi.is_parset(): # update & parset file exists
				oi=obstable[id].oi
			else: # initializing the obsinfo class
				oi=obsinfo(id)

			if not oi.is_parset():
				comment = "NO PARSET FILE FOUND!"
				# search for nodeslist and datadir with raw data
				# we do this search only if ObsID is _NOT_ in obsids_redonly list
				if id not in set(obsids_redonly) and not is_no_check_rawdata:
					oi.rawdata_search(storage_nodes, data_dirs, cexec_nodes, cexec_egrep_string)
			else:
				comment = ""
				# checking if raw data directories exist
				# we only check those ObsIDs that are _NOT_ in obsids_redonly list
				if id not in set(obsids_redonly) and not is_no_check_rawdata:
					oi.rawdata_check(storage_nodes, data_dirs, cexec_nodes, cexec_egrep_string)

			# mark the dataset as test if it has reduced data in test_dir
			if id in set(test_obsids):
				oi.is_test = True
			else: # this "False" part is important in case some ObsID was first the "test" and then it got non-test
				oi.is_test = False

			# class instance with output Info
			out=outputInfo(id)	

			# checking if the datadir exists in all lse nodes and if it does, gets the size of directory
			# if there are data in more than one datadir in one storage node, then Total size will be taken
			dirsizes = {}
			for lse in storage_nodes:
				dirsizes[lse] = ["x", "0.0"]
			# getting the sizes only from the intersection of oi.nodeslist and storage_nodes
			# we do collect sizes only for those ObsIDs that are _NOT_ in obsids_redonly list
			if not is_no_check_rawdata:
				if id not in set(obsids_redonly):
					use_nodes=list(set(oi.nodeslist).intersection(set(storage_nodes)))	
					# forming string with all locus nodes to check in one cexec command
					if len(use_nodes) > 0:
						cexeclocus=cexec_nodes[use_nodes[0]]
						if len(use_nodes) > 1:
							for s in use_nodes[1:]:
								cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
						ddir=datadir_mask + "/" + id   # using mask here to get the Total size of raw data in one storage node
						cmd="%s %s 'du -sc -B 1 %s 2>/dev/null | grep total | cut -f 1' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, ddir)
						cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
						# finding all locus nodes that have the dir with raw data
						for l in np.arange(len(cexec_output)):
							if re.match("^-----", cexec_output[l]) is None:
								if cexec_output[l] == "0": continue
								if cexec_output[l].isdigit() == True:
									lse = cexec_output[l-1].split(" ")[1]
									dirsizes[lse][1] = cexec_output[l]
									# getting human-readable size
									dirsizes[lse][0] = human_readable_size(float(cexec_output[l]))

			# checking if this specific observation was already reduced. 
			# Checking for both existence of the *_CSplots and *_redIS directory
			# Only checking those ObsIDs that are _NOT_ in obsids_rawonly list
			statusline="x"
			CSredlocation="x"
			ISredlocation="x"
			processed_dirsize=0.0
			# Collecting info about combined png plots
			profiles_array=["", "", ""]   # 0 - CS, 1 - IS, 2 - FE

			# On CEPII we only checking the archive area on 'hoover' nodes. Thus, the processed_dirsize does not include
			# the size of CS data on other locus nodes
			# Also getting "combined.png" plot for CS data. Also we have to rename it, because the name for IS is the same
			# CS and BF can't be recorded together, thus in the future I have to check if BF is turned on and then look for BF plot summary correspondingly
			if id not in set(obsids_rawonly):  # only check ObsIDs that do have Reduced dir (_CSplots or _redIS or both) in the Archive area
				# looking for _CSplots first on locus101
				if oi.CS == "+":
#					lse=hoover_nodes[0]  # locus101
					lse="locus092"
					cmd="%s %s '%s -t CS -d %s%s -id \"%s\"' | grep -v xauth | %s" % (cexeccmd, cexec_nodes[lse], process_dir_status_script, psr_archive_dir + lse, oi.is_test and "/" + test_dir or "", id, cexec_egrep_string)
					cmdout=[line[:-1] for line in os.popen(cmd).readlines()]
					if cmdout[0] != "":
						CSredlocation=cmdout[0]
						statusline=cmdout[1]
						# when we are here it means, that CS data is run through standard pipeline, so sub-files should exist and
						# the data can be potentially already ran through SEARCH pipeline. So, here we are checking if search* directories
						# exist in each of the raw nodes with the data
						if id not in set(obsids_redonly):
							use_nodes=list(set(oi.nodeslist).intersection(set(storage_nodes)))	
							# forming string with all locus nodes to check in one cexec command
							if len(use_nodes) > 0:
								cexeclocus=cexec_nodes[use_nodes[0]]
								if len(use_nodes) > 1:
									for s in use_nodes[1:]:
										cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
								cmd="%s %s 'ls -d %s*/%s_red/search* 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, psr_archive_dir, id)
								cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
								# finding all locus nodes that have the search dir and count them
								search_dirs_number=0
								for l in np.arange(len(cexec_output)):
									if re.match("^-----", cexec_output[l]) is None:
										search_dirs_number += 1
								# modifying the statusline
								if search_dirs_number > 0:
									statusline=statusline[:-1] + ",+search[%d])" % (search_dirs_number)

						if cmdout[2].isdigit() == True:
							processed_dirsize + float(cmdout[2]) / 1000. / 1000. / 1000.
						if cmdout[3] == "yes":  # combined plot exists
							# copying combined plots and renaming them
							profiles_array[0]="CScombined"
							if oi.nrBeams > 1 or oi.nrTiedArrayBeams > 1 or oi.nrRings > 0:
								combined="combined"
								cmd="mkdir -p %s/%s ; %s %s 'cp -f %s/%s.png %s/%s.th.png %s/%s' 2>&1 1>/dev/null ; mv -f %s/%s/%s.png %s/%s/%s.png 2>/dev/null ; mv -f %s/%s/%s.th.png %s/%s/%s.th.png 2>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], CSredlocation, combined, CSredlocation, combined, plotsdir, id, plotsdir, id, combined, plotsdir, id, profiles_array[0], plotsdir, id, combined, plotsdir, id, profiles_array[0])
							else:
								thcombined="combined"
								combined=cmdout[5]
								basecombined=combined.split("/")[-1].split(".png")[0]
								cmd="mkdir -p %s/%s ; %s %s 'cp -f %s %s/%s.th.png %s/%s' 2>&1 1>/dev/null ; mv -f %s/%s/%s.png %s/%s/%s.png 2>/dev/null ; mv -f %s/%s/%s.th.png %s/%s/%s.th.png 2>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], combined, CSredlocation, thcombined, plotsdir, id, plotsdir, id, basecombined, plotsdir, id, profiles_array[0], plotsdir, id, thcombined, plotsdir, id, profiles_array[0])
							os.system(cmd)
						# checking if this obs is FE obs. If so, then get status maps
#						if oi.FE == "+" and cmdout[4] == "yes":
						if cmdout[4] == "yes":   # now also checking not just for FE obs (in case there are 'heat' maps for CS data with many TA beams)
							# copying FE status plots
							profiles_array[2]="status"
							combined="status"
							cmd="mkdir -p %s/%s ; %s %s 'cp -f %s/%s.png %s/%s.th.png %s/%s' 2>&1 1>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], CSredlocation, combined, CSredlocation, combined, plotsdir, id)
							os.system(cmd)

				# checking if this obs has BF data (then it should not have CS)
				if oi.BF == "+":
					lse="locus093"
					cmd="%s %s '%s -t CV -d %s%s -id \"%s\"' | grep -v xauth | %s" % (cexeccmd, cexec_nodes[lse], process_dir_status_script, psr_archive_dir + lse, oi.is_test and "/" + test_dir or "", id, cexec_egrep_string)
					cmdout=[line[:-1] for line in os.popen(cmd).readlines()]
					if cmdout[0] != "":
						CSredlocation=cmdout[0]
						statusline=cmdout[1]
						if cmdout[2].isdigit() == True:
							processed_dirsize + float(cmdout[2]) / 1000. / 1000. / 1000.
						if cmdout[4] == "yes":  # status DSPSR plots exist
							# copying combined plots and renaming them
							profiles_array[0]="CScombined"
							combined="status"
							cmd="mkdir -p %s/%s ; %s %s 'cp -f %s/%s.png %s/%s.th.png %s/%s' 2>&1 1>/dev/null ; mv -f %s/%s/%s.png %s/%s/%s.png 2>/dev/null ; mv -f %s/%s/%s.th.png %s/%s/%s.th.png 2>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], CSredlocation, combined, CSredlocation, combined, plotsdir, id, plotsdir, id, combined, plotsdir, id, profiles_array[0], plotsdir, id, combined, plotsdir, id, profiles_array[0])
							os.system(cmd)

				# looking for _redIS first on locus102
				if oi.IS == "+":
#					lse=hoover_nodes[1]  # locus102
					lse="locus094"
					cmd="%s %s '%s -t IS -d %s%s -id \"%s\"' | grep -v xauth | %s" % (cexeccmd, cexec_nodes[lse], process_dir_status_script, psr_archive_dir + lse, oi.is_test and "/" + test_dir or "", id, cexec_egrep_string)
					cmdout=[line[:-1] for line in os.popen(cmd).readlines()]
					if cmdout[0] != "":
						ISredlocation=cmdout[0]
						if statusline == "x":
							statusline=cmdout[1]
						else:
							statusline=statusline+" "+cmdout[1]
						if cmdout[2].isdigit() == True:
							processed_dirsize + float(cmdout[2]) / 1000. / 1000. / 1000.
						if cmdout[3] == "yes":  # combined plot exists
							# copying combined plots and renaming them
							profiles_array[1]="IScombined"
							if oi.nrBeams > 1:
								combined="combined"
								cmd="mkdir -p %s/%s ; %s %s 'cp -f %s/%s.png %s/%s.th.png %s/%s' 2>&1 1>/dev/null ; mv -f %s/%s/%s.png %s/%s/%s.png 2>/dev/null ; mv -f %s/%s/%s.th.png %s/%s/%s.th.png 2>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], ISredlocation, combined, ISredlocation, combined, plotsdir, id, plotsdir, id, combined, plotsdir, id, profiles_array[1], plotsdir, id, combined, plotsdir, id, profiles_array[1])
							else:
								thcombined="combined"
								combined=cmdout[4]
								basecombined=combined.split("/")[-1].split(".png")[0]
								cmd="mkdir -p %s/%s ; %s %s 'cp -f %s %s/%s.th.png %s/%s' 2>&1 1>/dev/null ; mv -f %s/%s/%s.png %s/%s/%s.png 2>/dev/null ; mv -f %s/%s/%s.th.png %s/%s/%s.th.png 2>/dev/null" % (plotsdir, id, cexeccmd, cexec_nodes[lse], combined, ISredlocation, thcombined, plotsdir, id, plotsdir, id, basecombined, plotsdir, id, profiles_array[1], plotsdir, id, thcombined, plotsdir, id, profiles_array[1])
							os.system(cmd)


			# getting the grid links for the current ObsID and putting them to separate ascii file
			# and making the archive status line for the table
			archivestatus = "x"
			archivesize = {"raw": 0.0, "sub": 0.0, "meta": 0.0}  # size of archived files of different kind
			if len(gridfiles) != 0:
				grid_obsid_indices = [i for i in np.arange(len(gridfiles)) if re.search(id, gridfiles[i])]
				if len(grid_obsid_indices) != 0:
					archivestatus = ""
					grid_obsid_files = [gridfiles[i] for i in grid_obsid_indices]
					grid_obsid_dates = [griddates[i] for i in grid_obsid_indices]
					grid_obsid_times = [gridtimes[i] for i in grid_obsid_indices]
					grid_obsid_sizes = [gridsizes[i] for i in grid_obsid_indices]
					grid_obsid_links = [gridlinks[i] for i in grid_obsid_indices]
					# saving the grid links to the separate ascii file
					np.savetxt(griddir + "/%s.txt" % (id), np.transpose((grid_obsid_files, grid_obsid_dates, grid_obsid_times, [str(i) for i in grid_obsid_sizes], grid_obsid_links)), fmt="%s", delimiter="   ")
					rawfiles = [file for file in grid_obsid_files if re.search("raw", file)]
					if len(rawfiles) != 0: 
						archivestatus = archivestatus+" raw"
						archivesize["raw"] = np.sum([grid_obsid_sizes[i] for i in np.arange(len(grid_obsid_files)) if re.search("raw", grid_obsid_files[i])])
					subbandfiles = [file for file in grid_obsid_files if re.search("sub", file)]
					if len(subbandfiles) != 0: 
						archivestatus = archivestatus+" sub"
						archivesize["sub"] = np.sum([grid_obsid_sizes[i] for i in np.arange(len(grid_obsid_files)) if re.search("sub", grid_obsid_files[i])])
					metafiles = [file for file in grid_obsid_files if re.search("meta", file)]
					if len(metafiles) != 0: 
						archivestatus = archivestatus+" meta"
						archivesize["meta"] = np.sum([grid_obsid_sizes[i] for i in np.arange(len(grid_obsid_files)) if re.search("meta", grid_obsid_files[i])])
					archivestatus = " ".join(archivestatus.split(" ")[1:])

			# combining info
			out.Init(id, oi, storage_nodes, dirsizes, statusline, CSredlocation, ISredlocation, processed_dirsize, comment, profiles_array, archivestatus, archivesize)
			obstable[id] = out
			# printing the info line by line in debug mode
			if is_debug:
				print "%d       %s" % (debugcounter, out.info)
				cmd="echo \"%d (%d)\" > %s" % (debugcounter, Nobsids, debugfile)
				os.system(cmd)
				debugcounter += 1

	except: # when the scripts crashes here, before exiting, we do save the current DB to the file
		if is_debug:
			print "Script crashed. Saving db, copying files and exiting..."
		# saving the database to disk
		save_db_copy_files (dumpfile, obstable)
		sys.exit(1)
		
	# saving the database to disk
	save_db_copy_files (dumpfile, obstable)

	# copying to another list to keep the old one
	obskeys = np.flipud(np.sort(obstable.keys(), kind='mergesort'))

	# exclude keys (i.e. ObsIDs) that do not have parset files - Vlad, Feb 20, 2013
	# otherwise, there are too many of them to print on the page, and mostly (if not all) they are not from pulsar obs
	# also, we do not want to exclude them from the database, because every time we want to add new ObsIDs
	# those old without parset files will also be considered as new, and this will take a lot of time to run
	obskeys=[ii for ii in obskeys if obstable[ii].oi.is_parset()]

	if np.size(update_obsids) != 0 and (not is_rebuild) and (not is_update) and (not is_append) and (not is_delete):
		obskeys = update_obsids
	else:
		# if is_from and/or is_to are set, then we have to exclude those records
		# from obstable that do not obey the conditions
		if is_from:
			fromsecs=time.mktime(time.strptime(fromdate, "%Y-%m-%d"))
			obskeys=list(np.compress(np.array([obstable[r].seconds for r in obskeys]) >= fromsecs, obskeys))

		if is_to:
			tosecs=time.mktime(time.strptime(todate, "%Y-%m-%d")) + 86399
			obskeys=list(np.compress(np.array([obstable[r].seconds for r in obskeys]) <= tosecs, obskeys))
		
		# also, here we can potentially add other sorting things, like to exclude ObsIDs with some
		# tags, e.g. TEST, or re-make html on-fly based on request from php-script..
		# Also, I should check the search conditions for validation in the very beginning, because the --search
		# also can be used in advance for a particular --update
		if search_string != "" and len(obskeys) != 0:
			# forming the list of AND conditions
			search_conditions=search_string.split(" and ")
			filtered_obsids=obskeys
			for cond in search_conditions:
				# checking first if this condition is complex condition with OR
				if re.search(" or ", cond):  # complex OR condition
					or_conditions=cond.split(" or ")
					or_filtered=[]
					for orcond in or_conditions:
						# get real condition after parsing
						real_cond = get_condition(orcond)
						# applying this condition to the real list of obsids
						or_filtered=np.append(or_filtered, list(eval(real_cond)))
					# in the end of this OR condition, we form list of unique ObsIDs
					filtered_obsids=np.unique(or_filtered)
				else: # simple condition
					# get real condition after parsing
					real_cond = get_condition(cond)
					# applying this condition to the real list of obsids
					try:
						filtered_obsids=list(eval(real_cond))
					except: pass
			

			# assigning filtered out list of ObsIDs to obsids
			obskeys=filtered_obsids

	# similar to what we do when we are updating db records, here we also only want to show the observations from
	# selected (from the command line) subclusters, because db can have data for all subclusters
	if np.size(obskeys) != 0:
		newobskeys=[]
		for sub in subclusters:
			newobskeys=np.append(newobskeys, list(np.compress(np.array([obstable[r].subcluster for r in obskeys]) == sub, obskeys)))
		newobskeys=np.append(newobskeys, list(np.compress(np.array([obstable[r].subcluster for r in obskeys]) == "locusA", obskeys)))
		newobskeys=np.append(newobskeys, list(np.compress(np.array([obstable[r].subcluster for r in obskeys]) == "locus?", obskeys)))
		newobskeys = np.flipud(np.sort(np.unique(newobskeys), kind='mergesort'))
		obskeys = newobskeys
	
	#
	# Printing the table
	#
	print "Number of observations in %s: %d" % (", ".join(subclusters), np.size(obskeys))
	if is_append:
		print "Number of new observations found in %s: %d" % (", ".join(subclusters), np.size(obsids))

	if (is_rebuild or is_update or is_append or is_delete) or np.size(update_obsids) == 0:
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

	equalstrs=[]
	if viewtype == "brief":
		equalstring_size=216
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		YYYY-MM-DD	Dur	Project		Ant	Band	   #Stations	    FE Im IS CS CV OCD	CSCVLocation		ISLocation		Status"
		print equalstring
	elif viewtype == "plots":
		equalstring_size=225
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		YYYY-MM-DD	Dur	Project		Ant	Band	   #Stations	    FE Im IS CS CV OCD	CSCVLocation		ISLocation		Status      Archive"
		print equalstring
	elif viewtype == "mega":
		equalstring_size=302+8*Nnodes
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		YYYY-MM-DD	Dur	Project		Ant	Band	   #Stations	    FE Im IS CS CV OCD	NodesList (locus)	%s	Total(GB)	Obs Setup	Stations		CSCVLocation		ISLocation                Status      Archive" % (storage_nodes_string,)
		print equalstring
	elif viewtype == "smega":
		equalstring_size=302
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		Source		YYYY-MM-DD	Dur	Project		Ant	Band	   #Stations	    FE Im IS CS CV OCD	NodesList (locus) 	Total(GB)	Obs Setup	Stations		CSCVLocation 		ISLocation               Status      Archive"
		print equalstring
	else: # usual
		equalstring_size=178+8*Nnodes
		for e in np.arange(equalstring_size):
			equalstrs = np.append(equalstrs, "=")
		equalstring="#" + "".join(equalstrs)
		
		print equalstring
		print "# No.	ObsID		YYYY-MM-DD	Dur	NodesList (locus)	Datadir	%s	Total(GB)	FE Im IS CS CV OCD	Status				Pointing    Source   Project" % (storage_nodes_string,)
		print equalstring
		

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
	if is_html:
		htmlrep=writeHtmlList(htmlfile, linkedhtmlstem, fromdate, todate)
		htmlrep.open()
		if is_append:
			htmlrep.obsnumber(storage_nodes, subclusters, np.size(obskeys), np.size(obsids), True)
		else:
			htmlrep.obsnumber(storage_nodes, subclusters, np.size(obskeys), np.size(obsids), False)
		if (is_rebuild or is_update or is_append or is_delete) or np.size(update_obsids) == 0:
			htmlrep.datesrange()
		htmlrep.header(viewtype, storage_nodes_string_html)
		counter = 0
		for i in sorted_indices:
			htmlrep.record(counter%2 == 0 and "d0" or "d1", counter, obstable[obskeys[i]].infohtml)
			counter += 1
		htmlrep.legend()
		htmlrep.close()

	# create html files sorted for different sorting modes, linked together
	if is_linkedhtml:
		sf={"obsid": "-obsid.html", "time": "-time.html", "size": "-size.html", "source": "-source.html"}
		for key in sf.keys():
			sf[key] = "%s%s" % (linkedhtmlstem, sf[key])
		# create html-file with statistics
		htmlstatfile = "%s-stat.html" % (linkedhtmlstem) 
		stat = obsstat(obskeys, fromdate, todate, storage_nodes)
		stat.printhtml(htmlstatfile)

		htmlrep=writeHtmlList(sf["obsid"], linkedhtmlstem, fromdate, todate)
		for key in sf.keys():
			htmlrep.reInit(sf[key])
			htmlrep.open()
			if is_append:
				htmlrep.obsnumber(storage_nodes, subclusters, np.size(obskeys), np.size(obsids), True)
			else:
				htmlrep.obsnumber(storage_nodes, subclusters, np.size(obskeys), np.size(obsids), False)
			if (is_rebuild or is_update or is_append or is_delete) or np.size(update_obsids) == 0:
				htmlrep.datesrange()
			htmlrep.statistics(htmlstatfile)
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
