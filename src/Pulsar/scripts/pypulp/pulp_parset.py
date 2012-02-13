###################################################################
#
# Class Parset description similar to obsinfo class used to gather
# info about observation
#

# Class Parset with info from the parset file
class Parset:
	def __init__(self, id):
		self.id = id
		self.parset=""
		self.logdir=""
                self.datestring="????"
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
			return
		# for D20??_ obsids also checking L20??_ directories
		if re.search("D20", self.id):			
			self.logdir=parset_logdir + re.sub("D20", "L20", self.id) + "/"
			if os.path.exists(self.logdir):
				self.get_parset_name(self.logdir, id_suffix)
				return
		# checking in the oldlog directory (0)
		self.logdir=parset_oldlogdir + self.id + "/"
		if os.path.exists(self.logdir):
			self.get_parset_name(self.logdir, id_suffix)
			return
		# Due to new naming convention and location of the parset files, also looking for the parset file
		# in any L2010-??-??_?????? directories	(2)
		cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_logdir, id_suffix)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			# it means we found the directory with parset file
			self.parset=status[0][:-1]
			self.logdir="/".join(status[0][:-1].split("/")[:-1]) + "/"
			return
		# now checking the new parset directory (3)
		cmd="find %s -type f -name '*%s.parset' -print 2>/dev/null | grep -v Permission | grep -v such" % (parset_newlogdir, id_suffix)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			# it means we found the directory with parset file
			self.parset=status[0][:-1]
			self.logdir="/".join(status[0][:-1].split("/")[:-1]) + "/"
			return
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

	# parsing the string with ranges of subbands recorded to get the number of subbands
	def getNrSubbands(self, sblist):
		nsubs = 0
		sbparts=sblist.split(",")
		for ss in sbparts:
			sedges=ss.split("..")
			if len(sedges) == 1: nsubs += 1
			else:
				nsubs += (int(sedges[1]) - int(sedges[0]) + 1)
		return nsubs

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
                	smonth=self.starttime.split("-")[1]
                	sday=self.starttime.split("-")[2].split(" ")[0]
                	self.datestring=smonth+sday

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
			# parsing the string with subband ranges to get total number of subbands recorded
			self.nrSubbands = self.getNrSubbands(self.subbandList)
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

				if self.subbandList != "?" and self.subbandWidth != 0 and (self.nrChanPerSubIS != 0 or self.nrChanPerSubCS != 0):
					try:
						subband_first = int(self.subbandList.split("..")[0].split(",")[0])
						# CS has a priority
						if self.nrChanPerSubIS != 0:
							nchanpersub = self.nrChanPerSubCS
						else:
							nchanpersub = self.nrChanPerSubIS
						lofreq = lower_band_edge + (self.subbandWidth / 1000.) * subband_first - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
						self.cfreq = lofreq + 0.5 * self.bw
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

		# in the new parset files (as of Jan 27, 2012), keywords "enabled" for CS and IS will not
		# work anymore. Not sure if they will stay, but even if they do, they will show wrong 'false'
		# So, we have to check this by other way and correct our values of self.CS and self.IS
		cmd="grep TiedArrayBeam %s | grep coherent | awk '{print $3}' - | grep T" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			self.CS = "+"
		cmd="grep TiedArrayBeam %s | grep coherent | awk '{print $3}' - | grep F" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			self.IS = "+"
			# so, in the array of TiedArrayBeams we have at least one IS beam, so we have to
			# decrease then the number of TiedArrayBeams 
			self.nrTiedArrayBeams -= np.size(status)
		

	# return True if parset file was found, and False otherwise
	def is_parset (self):
		if self.parset == "":
			return False
		else:
			return True
