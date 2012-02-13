#
# Class CMDLine collects info about user input from the command line
#
import optparse as opt
import os, sys
from pulp_sysinfo import CEP2Info
import logging

class CMDLine:
	# parsing a command line
	# si - class object that has CEP2 info
        def __init__(self, si, version=""):
        	self.usage = "Usage: %prog <-id ObsID> -p <Pulsar name(s)> -o <Output Dir> [-h|--help] [OPTIONS]"
        	self.cmd = opt.OptionParser(self.usage, version="%prog " + version)
        	self.cmd.add_option('--id', dest='obsid', metavar='ObsID',
                           help="Specify the Observation ID (i.e. L30251). This option is required", default="", type='str')
        	self.cmd.add_option('-p', '--pulsar', dest='psr', metavar='PSRS',
                           help="Specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or \
				specify the word 'position' (lower case) find associated known Pulsars in the FOV of observation or \
				specify the word 'NONE' (upper case) when you want to skip the folding step of the processing \
				(i.e. single Pulsar: B2111+46) (i.e. multiple pulsars to fold:  B2111+46,B2106+44) \
				(i.e. up to 3 brights pulsars to fold at location of FOV: position. This option is required", default="", type='str')
        	self.cmd.add_option('-o', '-O', '--output', dest='outdir', metavar='DIR',
                           help="Specify the Output Processing Location \
				(i.e. /net/sub5/lse013/data4/LOFAR_PULSAR_ARCHIVE_lse013/L2010_06296_red). This option is required", default="", type='str')
        	self.cmd.add_option('--raw', dest='rawdir', metavar='DIR',
                           help="when pipeline is not run on CEPI, input raw data locations can be specified; directory structure \
				assumed as: input_raw_data_location/L2011_OBSID . Wildcards can be used for multiple input locations, \
				but MUST be quoted: (i.e. -raw \"/state/partition2/lofarpwg/RAW?\")", default="", type='str')
        	self.cmd.add_option('--par', '--parset', dest='parfile', metavar='FILE',
                           help="when pipeline is not run on CEPI, input parameter file location can be specified; directory structure \
				assumed as: parset_location/<any_path>/LOBSID.parset \
				(i.e. -par /state/partition2/lofarpwg/PARSET)", default="", type='str')
        	self.cmd.add_option('--all', action="store_true", dest='is_all',
                           help="optional parameter perform folding on entire subband set in addition to N-splits (takes 11 extra min)", default=False)
        	self.cmd.add_option('--all_pproc', action="store_true", dest='is_all_pproc',
                           help="Post-Processing optional parameter to ONLY perform folding on entire subband set based on \
				already-processed N-splits", default=False)
        	self.cmd.add_option('--rfi', action="store_true", dest='is_rfi',
                           help="optional parameter perform Vlad's RFI checker and only use clean results (takes 7 extra min)", default=False)
        	self.cmd.add_option('--rfi_pproc', action="store_true", dest='is_rfi_pproc',
                           help="Post-Processing optional parameter to perform Vlad\'s RFI checker on already-processed N-splits", default=False)
        	self.cmd.add_option('-c', '-C', '--collapse', action="store_true", dest='is_collapse',
                           help="optional parameter to switch on bf2presto COLLAPSE (Collapse all channels in MS to a single .sub file)", default=False)
        	self.cmd.add_option('--del', '--delete', action="store_true", dest='is_delete',
                           help="optional parameter to delete the previous ENTIRE Output_Processing_Location if it exists (override \
				previous results!)", default=False)
        	self.cmd.add_option('--core', dest='ncores', metavar='N',
                           help="optional parameter to change the number of cores (splits) used for processing (default: %default)", default=8, type='int')
        	self.cmd.add_option('--incoh_only', action="store_true", dest='is_incoh_only',
                           help="optional parameter to process ONLY Incoherentstokes (even though coherentstokes data exist)", default=False)
        	self.cmd.add_option('--coh_only', action="store_true", dest='is_coh_only',
                           help="optional parameter to process ONLY Coherentstokes  (even though incoherentstokes data exist)", default=False)
        	self.cmd.add_option('--incoh_redo', action="store_true", dest='is_incoh_redo',
                           help="optional parameter to redo processing for Incoherentstokes (deletes previous incoh results!)", default=False)
        	self.cmd.add_option('--coh_redo', action="store_true", dest='is_coh_redo',
                           help="optional parameter to redo processing for Coherentstokes (deletes previous coh results!)", default=False)
        	self.cmd.add_option('--transpose', action="store_true", dest='is_transpose',
                           help="optional parameter to indicate the input data were run through the TAB 2nd transpose", default=False)
        	self.cmd.add_option('--nofold', action="store_true", dest='is_nofold',
                           help="optional parameter to turn off folding of data (prepfold is not run);  multiple pulsar names are \
				not possible", default=False)
        	self.cmd.add_option('--subs', action="store_true", dest='is_subs',
                           help="optional parameter to process raw files into presto .subXXXX files instead of the default psrfits", default=False)
        	self.cmd.add_option('--test', action="store_true", dest='is_test',
                           help="optional for testing: runs bf2presto and bypasses prepfold and rfi processing but echo\'s all commands", default=False)
        	self.cmd.add_option('--debug', action="store_true", dest='is_debug',
                           help="optional for testing: turns on debugging in ksh (tons of STDOUT messages)", default=False)
        	self.cmd.add_option('--examples', action="store_true", dest='is_examples',
                           help="Gives extended help and usage examples", default=False)
        
		# reading cmd options
		(self.opts, self.args) = self.cmd.parse_args()

		# check if any input parameters are given
		if len(sys.argv[1:]) == 0:
			self.cmd.print_usage()
			sys.exit(0)

		# print extended help
		if self.opts.is_examples:
			pulp_file_help = si.get_pulp_help()
			os.system("cat %s" % (pulp_file_help))
			sys.exit(0)

		# forming the name of the log-file as OBSID_pulp.log
		# if obsid is not given than the log-file will be _pulp.log
		si.set_logfile(self.opts.obsid + "_pulp.log")



	# checks if given arguments are OK and not mutually exclusive, etc.
	def check_options(self, log=None):

		# changing the logging level for debug if option is given
		if log != None and self.opts.is_debug:
			log.setLevel(logging.DEBUG)

		# check if all required options are given
		if self.opts.obsid == "" or self.opts.psr == "" or self.opts.outdir == "":
			if log == None:
				print "It is required to set _all three_ parameters: ObsID, PSRS and Output directory!"
			else:
				log.error("It is required to set _all three_ parameters: ObsID, PSRS and Output directory!")
			sys.exit(1)

		# checking if number of cores is too large/small or not
		if self.opts.ncores < 1 or self.opts.ncores > 128:
			if log == None:
				print "ERROR: Number of cores must be 1 <= N <= 128 ;  currently requested %d cores!" % (self.opts.ncores)
			else:
				log.error("ERROR: Number of cores must be 1 <= N <= 128 ;  currently requested %d cores!" % (self.opts.ncores))
			sys.exit(1)

		# checking if there are mutually exclusive parameters given
		if self.opts.is_coh_only and self.opts.is_incoh_only:
			if log == None:
				print "ERROR: Mutually exclusive parameters set coh_only=yes and incoh_only=yes; only one allowed to be turned on!"
			else:
				log.error("ERROR: Mutually exclusive parameters set coh_only=yes and incoh_only=yes; only one allowed to be turned on!")
			sys.exit(1)



	# print summary of all set input parameters
	def print_summary(self, log=None):
		if log == None:
			print ""
			print "Running pulp.py with the following input arguments:"
			print ""
			print "              OBSID = %s" % (self.opts.obsid)
			print "             PULSAR = %s" % (self.opts.psr)
			print "   OUTPUT DIRECTORY = %s" % (self.opts.outdir)
			print "           COLLAPSE = %s" % (self.opts.is_collapse and "yes" or "no")
			print "    NUMBER OF CORES = %d" % (self.opts.ncores)
			print ""
			print "    ADDITIONAL PROCESSING ON ALL THE SUBBANDS = %s" % (self.opts.is_all and "yes" or "no")
			print "     ONLY POST-PROCESSING OF ALL THE SUBBANDS = %s" % (self.opts.is_all_pproc and "yes" or "no")
			print "                    ADDITIONAL RFI PROCESSING = %s" % (self.opts.is_rfi and "yes" or "no")
			print "         ONLY RFI CHECKING AS POST-PROCESSING = %s" % (self.opts.is_rfi_pproc and "yes" or "no")
			print "          REDO OF INCOHERENTSTOKES PROCESSING = %s" % (self.opts.is_incoh_redo and "yes" or "no")
			print "            REDO OF COHERENTSTOKES PROCESSING = %s" % (self.opts.is_coh_redo and "yes" or "no")
			print "     PROCESSING OF INCOHERENTSTOKES DATA ONLY = %s" % (self.opts.is_incoh_only and "yes" or "no")
			print "       PROCESSING OF COHERENTSTOKES DATA ONLY = %s" % (self.opts.is_coh_only and "yes" or "no")
			print "INPUT DATA WERE RUN THROUGH THE 2ND TRANSPOSE = %s" % (self.opts.is_transpose and "yes" or "no")
			print "                      TURN OF FOLDING OF DATA = %s" % (self.opts.is_nofold and "yes" or "no")
			print "      PROCESSING USING PRESTO *.SUB???? FILES = %s" % (self.opts.is_subs and "yes" or "no")
			if self.opts.rawdir != "":
				print "            USER-SPECIFIED RAW DATA DIRECTORY = %s" % (self.opts.rawdir)
			if self.opts.parfile != "":
				print "                   USER-SPECIFIED PARSET FILE = %s" % (self.opts.parfile)
			print ""
		else:
			log.info("")
			log.info("Running pulp.py with the following input arguments:")
			log.info("")
			log.info("              OBSID = %s" % (self.opts.obsid))
			log.info("             PULSAR = %s" % (self.opts.psr))
			log.info("   OUTPUT DIRECTORY = %s" % (self.opts.outdir))
			log.info("           COLLAPSE = %s" % (self.opts.is_collapse and "yes" or "no"))
			log.info("    NUMBER OF CORES = %d" % (self.opts.ncores))
			log.info("")
			log.info("    ADDITIONAL PROCESSING ON ALL THE SUBBANDS = %s" % (self.opts.is_all and "yes" or "no"))
			log.info("     ONLY POST-PROCESSING OF ALL THE SUBBANDS = %s" % (self.opts.is_all_pproc and "yes" or "no"))
			log.info("                    ADDITIONAL RFI PROCESSING = %s" % (self.opts.is_rfi and "yes" or "no"))
			log.info("         ONLY RFI CHECKING AS POST-PROCESSING = %s" % (self.opts.is_rfi_pproc and "yes" or "no"))
			log.info("          REDO OF INCOHERENTSTOKES PROCESSING = %s" % (self.opts.is_incoh_redo and "yes" or "no"))
			log.info("            REDO OF COHERENTSTOKES PROCESSING = %s" % (self.opts.is_coh_redo and "yes" or "no"))
			log.info("     PROCESSING OF INCOHERENTSTOKES DATA ONLY = %s" % (self.opts.is_incoh_only and "yes" or "no"))
			log.info("       PROCESSING OF COHERENTSTOKES DATA ONLY = %s" % (self.opts.is_coh_only and "yes" or "no"))
			log.info("INPUT DATA WERE RUN THROUGH THE 2ND TRANSPOSE = %s" % (self.opts.is_transpose and "yes" or "no"))
			log.info("                      TURN OF FOLDING OF DATA = %s" % (self.opts.is_nofold and "yes" or "no"))
			log.info("      PROCESSING USING PRESTO *.SUB???? FILES = %s" % (self.opts.is_subs and "yes" or "no"))
			if self.opts.rawdir != "":
				log.info("            USER-SPECIFIED RAW DATA DIRECTORY = %s" % (self.opts.rawdir))
			if self.opts.parfile != "":
				log.info("                   USER-SPECIFIED PARSET FILE = %s" % (self.opts.parfile))
			log.info("")
