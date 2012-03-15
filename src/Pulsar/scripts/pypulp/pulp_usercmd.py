#
# Class CMDLine collects info about user input from the command line
#
import optparse as opt
import os, sys
import numpy as np
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
import logging
import time
import re

# checks if given pulsar is good ones, i.e. either in ATNF catalog
# or par-file do exis
# return value is True (psr is good), or False (psr is bad)
def check_pulsars(psr, cmdline, cep2, log=None):
	if psr not in cmdline.psrbs and psr not in cmdline.psrjs:
		msg="warning: Pulsar %s is not in the catalog: '%s'! Checking for par-file..." % (psr, cep2.psrcatalog)
		if log != None: log.warning(msg)
		# checking if par-file exist
		parfile="%s/%s.par" % (cep2.parfile_dir, re.sub(r'[BJ]', '', psr))
		if not os.path.exists(parfile):
			# checking another parfile name
			parfile="%s/%s.par" % (cep2.parfile_dir, psr)
			if not os.path.exists(parfile): 
				return False
		msg="Found parfile '%s'. Continue..." % (parfile)
		if log != None: log.info(msg)
	return True

class CMDLine:
	# parsing a command line
        def __init__(self, version=""):
		self.prg = sys.argv[0]
		self.options = sys.argv[1:]  # storing original cmd line
		self.version = version
		self.psrs = []  # list of pulsars to fold
		self.beams = [] # list of beams to process
		self.psrbs = self.psrjs = [] # list of B and J names of pulsars from ATNF catalog
		self.ras = self.decs = self.s400 = [] # lists of RA, DEC, and S400 of catalog pulsars
        	self.usage = "Usage: %prog <--id ObsID> [-h|--help] [OPTIONS]"
        	self.cmd = opt.OptionParser(self.usage, version="%prog " + self.version)
        	self.cmd.add_option('--id', '--obsid', dest='obsid', metavar='ObsID',
                           help="Specify the Observation ID (i.e. L30251). This option is required", default="", type='str')
        	self.cmd.add_option('-p', '--pulsar', dest='psr', metavar='PSRS|word',
                           help="Specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or \
                                give one of the 4 special words: \"parset\" - to take pulsar name from the source field for each SAP \
				separately from the parset file, or \"sapfind\", \"sapfind3\" to find the best (3 best) pulsars in FOV \
				of the particular SAP, or \"tabfind\" to find the brightest pulsar for each TAB individually. \
				If no pulsars are given and no special words used, then pipeline will try to take source names from \
				parset file first, and then look for the best pulsars in SAP's FOV (same as \"sapfind\"). \
                                Word 'NONE' as a pulsar name is ignored", default="", type='str')
        	self.cmd.add_option('-o', '-O', '--output', dest='outdir', metavar='DIR',
                           help="Specify the Output Processing Location relative to /data/LOFAR_PULSAR_ARCHIVE_locus*. \
                                 Default is corresponding *_red or *_redIS directory", default="", type='str')
        	self.cmd.add_option('--norfi', action="store_true", dest='is_norfi',
                           help="optional parameter to skip subdyn.py RFI checker", default=False)
        	self.cmd.add_option('--nopdmp', action="store_true", dest='is_nopdmp',
                           help="Turn off running pdmp in the pipeline", default=False)
        	self.cmd.add_option('--nofold', action="store_true", dest='is_nofold',
                           help="optional parameter to turn off folding of data (prepfold is not run)", default=False)
        	self.cmd.add_option('--skip-dspsr', action="store_true", dest='is_skip_dspsr',
                           help="optional parameter to turn off running dspsr part of the pipeline (including pdmp and creation of corresponding plots)", default=False)
        	self.cmd.add_option('--summary', action="store_true", dest='is_summary', 
                           help="run only summary actions on already processed data", default=False)
        	self.cmd.add_option('--beams', dest='beam_str', metavar='SAP#:TAB#[,SAP#:TAB#,...]',
                           help="user-specified beams to process separated by commas and written as station beam number, colon, \
                                 TA beam number, with no spaces", default="", type='str')
        	self.cmd.add_option('--incoh_only', action="store_true", dest='is_incoh_only',
                           help="optional parameter to process ONLY Incoherentstokes (even though coherentstokes data exist)", default=False)
        	self.cmd.add_option('--coh_only', action="store_true", dest='is_coh_only',
                           help="optional parameter to process ONLY Coherentstokes  (even though incoherentstokes data exist)", default=False)
        	self.cmd.add_option('--del', '--delete', action="store_true", dest='is_delete',
                           help="optional parameter to delete the previous ENTIRE Output_Processing_Location if it exists. \
				Otherwise, the new results will be overwritten/added to existing directory", default=False)
        	self.cmd.add_option('--par', '--parset', dest='parset', metavar='FILE',
                           help="Specify explicitely the input parameter file (parset file). By default, it will be looked for \
                                 in standard system directory", default="", type='str')
        	self.cmd.add_option('--raw', dest='rawdir', metavar='RAWDIR',
                           help="Specify the location of input raw data. Directory structure is assumed as RAWDIR/<ObsID>.", default="/data", type='str')
        	self.cmd.add_option('--debug', action="store_true", dest='is_debug',
                           help="optional for testing: turns on debug level logging in Python", default=False)
        	self.cmd.add_option('--noinit', action="store_true", dest='is_noinit',
                           help="do not initialize classes but instead read all info from saved config file", default=False)
        	self.cmd.add_option('--local', action="store_true", dest='is_local', 
                           help="To process the data locally on current locus node for one beam only. Should only be used together with --beams option \
				and only the first beam will be used if there are several specified in --beams", default=False)
        
		# reading cmd options
		(self.opts, self.args) = self.cmd.parse_args()

		# check if any input parameters are given
		if len(sys.argv[1:]) == 0:
			self.cmd.print_usage()
			os.system("stty sane")
			sys.exit(0)

		# print extended help
#		if self.opts.is_examples:
#			pulp_file_help = si.get_pulp_help()
#			os.system("cat %s" % (pulp_file_help))
#			os.system("stty sane")
#			sys.exit(0)

	# prints countdown (only to terminal)
	def press_controlc(self, fr, cur):
		msg="\bPress Control-C to stop in: %s" % (" ".join(str(i) for i in range(fr, cur-1, -1)))
		print "\b" * int("%d" % (29 + 3*(fr-cur))), msg,
		sys.stdout.flush()

	# waiting for user to make a decision about "Stop or continue"
	def waiting_for_user(self, secs_to_wait, log=None):
		msg="Waiting %d seconds before starting..." % (secs_to_wait)
		if log != None: log.info(msg)
		else: print msg
		for sec in range(secs_to_wait, 0, -1):
			self.press_controlc(secs_to_wait, sec)
			time.sleep(1)
		if log != None: log.info("")
		else: print ""
		

	# checks if given arguments are OK and not mutually exclusive, etc.
	def check_options(self, cep2, log=None):

		# changing the logging level for debug if option is given
		if log != None and self.opts.is_debug:
			log.set_loglevel(logging.DEBUG)

		# checking if --local is used without specified beam
		if self.opts.is_local and self.opts.beam_str == "":
			msg="You have to use --beams option with --local!"
			if log != None: log.error(msg)
			else: print msg
			os.system("stty sane")
			sys.exit(1)

		# check if all required options are given
		if self.opts.obsid == "":
			msg="ObsID is not given. What do you want to process?"
			if log != None: log.error(msg)
			else: print msg
			os.system("stty sane")
			sys.exit(1)

		# checking if there are mutually exclusive parameters given
		if self.opts.is_coh_only and self.opts.is_incoh_only:
			msg="Mutually exclusive parameters set coh_only=yes and incoh_only=yes; only one allowed to be turned on!"
			if log != None: log.error(msg)
			else: print msg
			os.system("stty sane")
			sys.exit(1)

		# checking that if --beams used then beams are specified correctly
		# we have this complicated "if" because we used --beams to pass summary locus node when --summary and --local
		if self.opts.beam_str != "" and (not self.opts.is_summary or (self.opts.is_summary and not self.opts.is_local)):
			if re.search(r'[^\,\:\d]+', self.opts.beam_str) is not None:
				msg="Option --beams can only has digits, colons and commas!"
				if log != None: log.error(msg)
				else: print msg
				os.system("stty sane")
				sys.exit(1)
			elif re.search(r'[\:]+', self.opts.beam_str) is None:
				msg="Option --beams should have at least one colon!"
				if log != None: log.error(msg)
				else: print msg
				os.system("stty sane")
				sys.exit(1)
			else:   # forming array of beams
				self.beams=self.opts.beam_str.split(",")
				# checking if neither SAP or TAB are empty
				for bb in self.beams:
					(sap, tab) = bb.split(":")
					if sap == "" or tab == "":
						msg="Option --beams has at least one empty SAP or TAB value!"
						if log != None: log.error(msg)
						else: print msg
						os.system("stty sane")
						sys.exit(1)

		# warning user that some of the results can still be overwritten, if --del is not used
		if not self.opts.is_delete:
			msg="***\n*** Warning: Some of the previous results still can be overwritten.\n\
*** You may want to use --del to have clean run, or specify new output directory.\n***"
			if log != None: log.warning(msg)
			else: print msg
			self.waiting_for_user(10, log)

		# checking if rawdir is specified
		if self.opts.rawdir != "/data":
			cep2.rawdir = self.opts.rawdir

		# NONE is ignored as pulsar name
		if self.opts.psr != "":
       	        	self.psrs=[psr for psr in self.opts.psr.split(",") if psr != "NONE"]
			self.psrs=list(np.unique(self.psrs))

		# checking --nofold and pulsar list
		if not self.opts.is_nofold and len(self.psrs) == 0:
			msg="***\n*** Warning: Pulsar is not specified and PULP will use source names\n\
*** from the parset file first if given, and then will look for the best\n\
*** pulsar in the SAP's FOV for each SAP separately! You also can use\n\
*** predefined words: \"parset\", \"sapfind\", \"sapfind3\", or \"tabfind\".\n\
*** See help for more details.\n***"
			if log != None: log.warning(msg)
			else: print msg
			self.waiting_for_user(10, log)

		if not self.opts.is_nofold:
			# reading B1950 and J2000 pulsar names from the catalog and checking if our pulsars are listed there
			# also reading coordinates and flux
			self.psrbs, self.psrjs, self.s400 = np.loadtxt(cep2.psrcatalog, comments='#', usecols=(1,2,9), dtype=str, unpack=True)
			self.ras, self.decs = np.loadtxt(cep2.psrcatalog, comments='#', usecols=(5,6), dtype=float, unpack=True)

			# checking if given psr(s) names are valid, and these pulsars are in the catalog
			if len(self.psrs) != 0 and self.psrs[0] != "parset" and self.psrs[0] != "sapfind" and \
				self.psrs[0] != "sapfind3" and self.psrs[0] != "tabfind":
				if len(self.psrs) > 3:
					msg="%d pulsars are given, but only first 3 will be used for folding" % (len(self.psrs))
					if log != None: log.warning(msg)
					else: print msg
					self.psrs=self.psrs[:3]
				# checking if given pulsar names are good
				for psr in self.psrs:
					if not check_pulsars(psr, self, cep2, log):
						msg="No parfile found for pulsar %s. Exiting..." % (psr)
						if log != None: log.error(msg)
						else: print msg
						os.system("stty sane")
                                        	sys.exit(1)
			else:
				msg="No pulsar names are given. PULP will find the proper pulsar(s) to fold..."
				if log != None: log.info(msg)
				else: print msg


	# print summary of all set input parameters
	def print_summary(self, cep2, obs, log=None):
		if log != None:
			log.info("")
			log.info("Pulsar Pipeline, V%s" % (self.version))
			log.info("Prg: %s" % (self.prg))
			log.info("Cmdline: %s %s" % (self.prg.split("/")[-1], " ".join(self.options)))
			log.info("")
			log.info("ObsID = %s" % (self.opts.obsid))
			if self.opts.is_nofold: pulsar_status = "No folding"
			else:
				if len(self.psrs) == 0: pulsar_status = "default:parset -> sapfind"
				else: 
					if self.psrs[0] == "parset" or self.psrs[0] == "sapfind" or self.psrs[0] == "sapfind3" or self.psrs[0] == "tabfind":
						pulsar_status = self.psrs[0]
					else: pulsar_status = ", ".join(self.psrs)
			log.info("PSR(s) = %s" % (pulsar_status))
			if not self.opts.is_nofold:
				if len(self.psrs) == 0:
					for sap in obs.saps:
						if sap.source != "" and check_pulsars(sap.source, self, cep2, None): log.info("SAP=%d   PSR: %s" % (sap.sapid, sap.source))
						else: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, len(sap.psrs)>0 and sap.psrs[0] or ""))
				if len(self.psrs) != 0 and self.psrs[0] == "sapfind":
					for sap in obs.saps: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, len(sap.psrs)>0 and sap.psrs[0] or ""))
				if len(self.psrs) != 0 and self.psrs[0] == "sapfind3":
					for sap in obs.saps: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, ", ".join(sap.psrs)))
				if len(self.psrs) != 0 and self.psrs[0] == "parset":
					for sap in obs.saps: log.info("SAP=%d   PSR: %s" % (sap.sapid, sap.source))
			log.info("Output Dir = %s_*/%s" % (cep2.processed_dir_prefix, self.opts.outdir != "" and self.opts.outdir or self.opts.obsid + "_red*"))
			log.info("Delete previous results = %s" % (self.opts.is_delete and "yes" or "no"))
			log.info("Summaries ONLY = %s" % (self.opts.is_summary and "yes" or "no"))
			log.info("RFI Checking = %s" % (self.opts.is_norfi and "no" or "yes"))
			log.info("DSPSR = %s" % (self.opts.is_skip_dspsr and "no" or "yes"))
			if not self.opts.is_skip_dspsr:
				log.info("pdmp = %s" % ((self.opts.is_nopdmp or self.opts.is_nofold) and "no" or "yes"))
			log.info("IS Only = %s" % (self.opts.is_incoh_only and "yes" or "no"))
			log.info("CS Only = %s" % (self.opts.is_coh_only and "yes" or "no"))
			if self.opts.rawdir != "/data":
				log.info("User-specified Raw data directory = %s" % (self.opts.rawdir))
			if self.opts.parset != "":
				log.info("User-specified Parset file = %s" % (self.opts.parset))
			if len(self.beams) != 0:
				log.info("User-specified BEAMS to process: %s" % (", ".join(self.beams)))
			log.info("")
