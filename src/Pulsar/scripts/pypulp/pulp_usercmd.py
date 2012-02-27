#
# Class CMDLine collects info about user input from the command line
#
import optparse as opt
import os, sys
import numpy as np
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
import logging

class CMDLine:
	# parsing a command line
        def __init__(self, version=""):
		self.prg = sys.argv[0]
		self.options = sys.argv[1:]  # storing original cmd line
		self.version = version
		self.psrs = []  # list of pulsars to fold
        	self.usage = "Usage: %prog <-id ObsID> -p <Pulsar name(s)> [-h|--help] [OPTIONS]"
        	self.cmd = opt.OptionParser(self.usage, version="%prog " + self.version)
        	self.cmd.add_option('--id', dest='obsid', metavar='ObsID',
                           help="Specify the Observation ID (i.e. L30251). This option is required", default="", type='str')
        	self.cmd.add_option('-p', '--pulsar', dest='psr', metavar='PSRS',
                           help="Specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or \
				specify the word 'position' (lower case) find associated known Pulsars in the FOV of observation or \
				specify the word 'NONE' (upper case) when you want to skip the folding step of the processing \
				(i.e. single Pulsar: B2111+46) (i.e. multiple pulsars to fold:  B2111+46,B2106+44) \
				(i.e. up to 3 brights pulsars to fold at location of FOV: position. This option is required", default="", type='str')
        	self.cmd.add_option('-o', '-O', '--output', dest='outdir', metavar='DIR',
                           help="Specify the Output Processing Location relative to /data/LOFAR_PULSAR_ARCHIVE_locus*. \
                                 Default is corresponding *_red or *_redIS directory", default="", type='str')
        	self.cmd.add_option('--raw', dest='rawdir', metavar='DIR',
                           help="when pipeline is not run on CEPI, input raw data locations can be specified; directory structure \
				assumed as: input_raw_data_location/<OBSID> . Wildcards can be used for multiple input locations, \
				but MUST be quoted: (i.e. -raw \"/state/partition2/lofarpwg/RAW?\")", default="", type='str')
        	self.cmd.add_option('--par', '--parset', dest='parset', metavar='FILE',
                           help="when pipeline is not run on CEPI, input parameter file location can be specified; directory structure \
				assumed as: parset_location/<any_path>/LOBSID.parset \
				(i.e. -par /state/partition2/lofarpwg/PARSET)", default="", type='str')
        	self.cmd.add_option('--norfi', action="store_true", dest='is_norfi',
                           help="optional parameter to skip Vlad's RFI checker", default=False)
        	self.cmd.add_option('--nopdmp', action="store_true", dest='is_nopdmp',
                           help="Turn off running pdmp in the pipeline", default=False)
        	self.cmd.add_option('--del', '--delete', action="store_true", dest='is_delete',
                           help="optional parameter to delete the previous ENTIRE Output_Processing_Location if it exists (override \
				previous results!)", default=False)
        	self.cmd.add_option('--incoh_only', action="store_true", dest='is_incoh_only',
                           help="optional parameter to process ONLY Incoherentstokes (even though coherentstokes data exist)", default=False)
        	self.cmd.add_option('--coh_only', action="store_true", dest='is_coh_only',
                           help="optional parameter to process ONLY Coherentstokes  (even though incoherentstokes data exist)", default=False)
#        	self.cmd.add_option('--incoh_redo', action="store_true", dest='is_incoh_redo',
#                           help="optional parameter to redo processing for Incoherentstokes (deletes previous incoh results!)", default=False)
#        	self.cmd.add_option('--coh_redo', action="store_true", dest='is_coh_redo',
#                           help="optional parameter to redo processing for Coherentstokes (deletes previous coh results!)", default=False)
#        	self.cmd.add_option('--nofold', action="store_true", dest='is_nofold',
#                           help="optional parameter to turn off folding of data (prepfold is not run);  multiple pulsar names are not possible", default=False)
        	self.cmd.add_option('--debug', action="store_true", dest='is_debug',
                           help="optional for testing: turns on debug level logging in Python", default=False)
        	self.cmd.add_option('--noinit', action="store_true", dest='is_noinit',
                           help="do not initialize classes but instead read all info from saved config file", default=False)
        	self.cmd.add_option('--beam', dest='beam_str', metavar='SAP#:TAB#',
                           help="beam to process written as station beam number, colon, TA beam number, with no spaces. This \
                                 option assumes that data are on the current node, or current node is hoover node", default="", type='str')
        
		# reading cmd options
		(self.opts, self.args) = self.cmd.parse_args()

		# check if any input parameters are given
		if len(sys.argv[1:]) == 0:
			self.cmd.print_usage()
			sys.exit(0)

		# print extended help
#		if self.opts.is_examples:
#			pulp_file_help = si.get_pulp_help()
#			os.system("cat %s" % (pulp_file_help))
#			sys.exit(0)



	# checks if given arguments are OK and not mutually exclusive, etc.
	def check_options(self, cep2, log=None):

		# changing the logging level for debug if option is given
		if log != None and self.opts.is_debug:
			log.set_loglevel(logging.DEBUG)

		# check if all required options are given
		if self.opts.obsid == "" or self.opts.psr == "":
			msg="It is required to set _all two_ parameters: ObsID and PSRS!"
			if log != None: log.error(msg)
			else: print msg
			sys.exit(1)

		# checking if there are mutually exclusive parameters given
		if self.opts.is_coh_only and self.opts.is_incoh_only:
			msg="Mutually exclusive parameters set coh_only=yes and incoh_only=yes; only one allowed to be turned on!"
			if log != None: log.error(msg)
			else: print msg
			sys.exit(1)

		# checking if given psr(s) names are valid, and these pulsars are in the catalog
		if self.opts.psr == "position":
			pass  #  I should add a proper action when "position" is given
                self.psrs=self.opts.psr.split(",")
		self.psrs=list(np.unique(self.psrs))
		# if NONE is given for pulsar together wiht other pulsar names, then NONE is ignored
		if "NONE" in self.psrs and len(self.psrs)>1: self.psrs.remove("NONE")
		if len(self.psrs) > 3:
			msg="%d pulsars are given, but only first 3 will be used for folding" % (len(self.psrs))
			if log != None: log.warning(msg)
			else: print msg
			self.psrs=self.psrs[:3]
		# reading B1950 and J2000 pulsar names from the catalog and checking if our pulsars are listed there
		psrbs, psrjs = np.loadtxt(cep2.psrcatalog, comments='#', usecols=(1,2), dtype=str, unpack=True)
		for psr in self.psrs:
			if psr == "NONE": continue
			if psr not in psrbs and psr not in psrjs:
				msg="Pulsar %s is not in the catalog: '%s'! Exiting." % (psr, cep2.psrcatalog)
				if log != None: log.error(msg)
				else: print msg
				sys.exit(1)
			

	# print summary of all set input parameters
	def print_summary(self, cep2, log=None):
		if log != None:
			log.info("")
			log.info("Pulsar Pipeline, V%s" % (self.version))
			log.info("Prg: %s" % (self.prg))
			log.info("Cmdline: %s %s" % (self.prg.split("/")[-1], " ".join(self.options)))
			log.info("")
			log.info("ObsID = %s" % (self.opts.obsid))
			log.info("PSR(s) = %s" % (self.opts.psr))
			log.info("Output Dir = %s_*/%s" % (cep2.processed_dir_prefix, self.opts.outdir != "" and self.opts.outdir or self.opts.obsid + "_red*"))
			log.info("Delete previous results = %s" % (self.opts.is_delete and "yes" or "no"))
			log.info("RFI Checking = %s" % (self.opts.is_norfi and "no" or "yes"))
			log.info("pdmp = %s" % (self.opts.is_nopdmp and "no" or "yes"))
			log.info("IS Only = %s" % (self.opts.is_incoh_only and "yes" or "no"))
			log.info("CS Only = %s" % (self.opts.is_coh_only and "yes" or "no"))
#			log.info("          REDO OF INCOHERENTSTOKES PROCESSING = %s" % (self.opts.is_incoh_redo and "yes" or "no"))
#			log.info("            REDO OF COHERENTSTOKES PROCESSING = %s" % (self.opts.is_coh_redo and "yes" or "no"))
#			log.info("                      TURN OF FOLDING OF DATA = %s" % (self.opts.is_nofold and "yes" or "no"))
			if self.opts.rawdir != "":
				log.info("User-specified Raw data directory = %s" % (self.opts.rawdir))
			if self.opts.parset != "":
				log.info("User-specified Parset file = %s" % (self.opts.parset))
			log.info("")
