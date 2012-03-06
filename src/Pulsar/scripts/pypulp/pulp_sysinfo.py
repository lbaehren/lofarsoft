#
# Class CEP2Info defines different parameters, locus names, necessary ENV variables, etc.
# related to pipeline processing
#
import os, sys
import logging
from pulp_logging import PulpLogger
import re

class CEP2Info:
	# setting different attributes
        def __init__(self):
		self.user = os.environ['USER']
		self.home = os.environ['HOME']
		self.lofarsoft = os.environ['LOFARSOFT']
		self.pulp_help = self.lofarsoft + "/release/share/pulsar/data/pulp_help.txt"
		# Directory with existing par-files
		self.parfile_dir = self.lofarsoft + "/release/share/pulsar/data/parfile"
		# db file from Psrcat
		self.psrcatdb = self.lofarsoft + "/release/share/pulsar/data/psrcat.db"
		# ATNF pulsar catalog
		self.psrcatalog = self.lofarsoft + "/release/share/pulsar/data/PSR_catalog.txt"
		# maximum radial distance (in deg) to find pulsars in FOV
		self.fov_lba = 9.
		self.fov_hba = 3.
		self.tabfind = 1. # for TABs pulsars usually sit in the center of the beam, so use the same value for both HBA and LBA
		self.pythonpath = os.environ['PYTHONPATH']
		# get the node where the scripts run
		self.current_node = os.popen('hostname').readlines()[0][:-1]
		# get current dir
		self.current_dir = os.popen('pwd').readlines()[0][:-1]
		# set the logdir, the log-file from it will be copied to corresponding _red dir
		self.logdir = ""
		# the logfile will be initialized later when we get info about the ObsID
		self.logfile = ""
		# directory where all parset files live
		self.parset_dir="/globalhome/lofarsystem/log"
		# directory with raw data (can be changed by user from command line)
		self.rawdir = "/data"
		# prefix for default directory with processed data
		self.processed_dir_prefix="/data/LOFAR_PULSAR_ARCHIVE"
		# data directory prefix on hoover nodes
		self.hoover_data_dir="/cep2"
		# full list of nodes and its cexec corresponding table
		self.locus_nodes=["locus%03d" % (num+1) for num in range(100)]
		self.hoover_nodes=["locus101", "locus102"]   # first is used to process CS data (if files per beam distributed over many nodes)
							     # second - to process IS data
		self.cexec_nodes={}
		for num in range(100): # we have 100 locus nodes
			key="locus%03d" % (num+1)
			self.cexec_nodes[key] = "locus:%d" % (num)
		# adding hoover nodes as well
		self.cexec_nodes["locus101"] = "hoover:0"
		self.cexec_nodes["locus102"] = "hoover:1"
		# cexec command to run. Using this mapfile makes keep mapping of the locus to be always the same
		self.cexeccmd="cexec -f /etc/c3.conf.full"
		# list of alive locus nodes
		self.alive_nodes = []
		# list of down nodes
		self.down_nodes = []	

	# checking connection to all locus nodes, to determine which ones are "alive"
	def check_connection(self, log=None):
		msg="Checking connection to locus nodes..."
		if log != None: log.info(msg)
		else: print msg
	        # forming string with all locus nodes to check in one cexec command
        	cexeclocus=self.cexec_nodes[self.locus_nodes[0]] # there is always at least one locus node
	        if len(self.locus_nodes) > 1:
        	        for s in self.locus_nodes[1:]:
                	        cexeclocus += ",%s" % (self.cexec_nodes[s].split(":")[1])
		# adding hoover nodes to cexeclocus
		cexeclocus += " %s" % (self.cexec_nodes[self.hoover_nodes[0]])
	        if len(self.hoover_nodes) > 1:
        	        for s in self.hoover_nodes[1:]:
                	        cexeclocus += ",%s" % (self.cexec_nodes[s].split(":")[1])
	        cmd="%s %s date | grep -v xauth | grep -v connect | grep -v Permission | egrep -v \'\\*\\*\\*\\*\\*\'" % (self.cexeccmd, cexeclocus)
        	cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
	        # finding all locus nodes that have the dir with raw data
        	for l in range(len(cexec_output)):
                	if re.match("^-----", cexec_output[l]) is None:
                        	self.alive_nodes.append(cexec_output[l-1].split(" ")[1])
                if len(self.alive_nodes) == 0:
			msg="The connection to all locus nodes is down. Try again later"
			if log != None: log.error(msg)
			else: print msg
			sys.exit(1)

	# print nodes that are down
	def print_down_nodes(self, log=None):
		all_nodes = self.cexec_nodes.keys()
		self.down_nodes=list(set(all_nodes)-set(all_nodes).intersection(set(self.alive_nodes)))
		msg="Nodes are down [%d]: %s" % (len(self.down_nodes), ", ".join(self.down_nodes))
		if log != None: log.info(msg)
		else: print msg

	# return list of alive nodes
	def get_alive_nodes(self):
		return self.alive_nodes

	# return file name with the extended help
	def get_pulp_help(self):
		return self.pulp_help

	# return $LOFARSOFT
	def get_lofarsoft(self):
		return self.lofarsoft

	# return current node
	def get_current_node(self):
		return self.current_node

	# return current dir
	def get_current_dir(self):
		return self.current_dir

	# return logfile
	def get_logfile(self):
		return self.logfile

	# return logdir
	def get_logdir(self):
		return self.logdir

	# set the logdir
	def set_logdir(self, id):
		self.logdir = "%s/.pulp/%s" % (self.home, id)
		cmd="mkdir -p %s" % (self.logdir)	
		os.system(cmd)

	# set the logfile
	def set_logfile(self, f):
		self.logfile = self.logdir + "/" + f

	# print info of all set attributes
	def print_info(self, log=None):
		if log != None:
			log.info("")
			log.info("USER = %s" % (self.user))
			log.info("Current node = %s" % (self.current_node))
			log.info("Current directory = %s" % (self.current_dir))
			log.info("LOFARSOFT = %s" % (self.lofarsoft))
			log.info("")
		
