#!/usr/bin/env python
#
# This is Python implementation of PULP, Pulsar standard pipeline
# written by A2 in shell
#
# Vlad, Oct 25, 2011 (c)
###################################################################
import os, sys, glob, time
import getopt
import numpy as np
import time
import cPickle
import re
import subprocess, shlex
from subprocess import PIPE, STDOUT, Popen
import logging
from pulp_parset import Observation
from pulp_usercmd import CMDLine
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
from pulp_pipeline import Pipeline

# PLEASE increment the version number when you edit this file!!!
VERSION="5.13"

###  M A I N ###
if __name__ == "__main__":

        # parsing command line
	cmdline = CMDLine(VERSION)

	# creating name of the Logger
	logger_name = "PULP"
	logfile = cmdline.opts.obsid
	if cmdline.opts.is_local and cmdline.opts.beam_str != "":
		if not cmdline.opts.is_summary:
			logger_name += cmdline.opts.beam_str.split(",")[0] 
			### I should add checking if beam_str is given correctly
			sapid = int(cmdline.opts.beam_str.split(",")[0].split(":")[0])
			tabid = int(cmdline.opts.beam_str.split(",")[0].split(":")[1])
			logfile = "%s_sap%03d_beam%04d.log" % (cmdline.opts.obsid, sapid, tabid)
		else:   # when --summary then name of the summary locus node should be given in --beams
			# when run locally
			logger_name += "_summary_%s" % (cmdline.opts.beam_str)
			# in this case summary locus node is given in --beams option
			logfile += "_summary_%s.log" % (cmdline.opts.beam_str)
	else:
		if cmdline.opts.is_summary:
			logfile += "_summary.log"
		else:
			logfile += "_pulp.log"

	# initializing the Logger
	log = PulpLogger(logger_name)

	try:
		# getting info about the CEP2
		cep2 = CEP2Info()
	        # forming the name of the log-file as OBSID_pulp.log in $HOME/.pulp/<OBSID> dir
        	# if obsid is not given than the log-file will be _pulp.log
		cep2.set_logdir(cmdline.opts.obsid)
		cep2.set_logfile(logfile)

		# adding file handler to our Logger
		logfh = logging.FileHandler(cep2.get_logfile(), mode='w')
		log.addHandler(logfh)

		# starting logging...
		start_pipe_time=time.time()
		log.info("UTC time is:  %s" % (time.asctime(time.gmtime())))

		obsconf_file = cep2.get_logdir() + "/" + "obs.b"
		pipeline_file = cep2.get_logdir() + "/" + "pipeline.b"

		# checking validity of the options
		cmdline.check_options(cep2, log)

		if not cmdline.opts.is_noinit:
			# checking connections to locus nodes
			cep2.check_connection(log)
			# print down nodes
			cep2.print_down_nodes(log)

		log.info("\nInitializing...")
		if not cmdline.opts.is_noinit:
			# initializing our Observation, collecting info from parset, etc.
			obs = Observation(cmdline.opts.obsid, cep2, cmdline, log)

			# checking if rawdata available on the cluster for user-specified beams
			cmdline.is_rawdata_available(cep2, obs, log)

			# saving obs configuration to file
                	obsfd = open (obsconf_file, "wb")
	                cPickle.dump(obs, obsfd, True)  # when False, the dumpfile is ascii
        	        obsfd.close()
		else:
			# loading obs setup from the file
			obsfd = open(obsconf_file, "rb")
			obs=cPickle.load(obsfd)
			obsfd.close()

		# printing info about observation
		obs.print_info(log)

		if not cmdline.opts.is_local: 
			# print summary
			cmdline.print_summary(cep2, obs, log)

		# printing info both to STDOUT and logfile
		cep2.print_info(cmdline, log)

		# if --beam option is not set, it means that we start the pipeline from main node
		if not cmdline.opts.is_local:	
			# initializing pulsar pipeline
			psrpipe = Pipeline(obs, cep2, cmdline, log)
			# saving pipeline config to file
                	pipefd = open (pipeline_file, "wb")
              		cPickle.dump(psrpipe, pipefd, True)
			pipefd.close()
			# kick off the pipeline
			if not cmdline.opts.is_summary:
				psrpipe.start(obs, cep2, cmdline, log)
			# wait for all childs to finish and prepare logs, all files in order
			# convert, FE maps, etc.
			try:
				psrpipe.finish(obs, cep2, cmdline, log)
			except KeyboardInterrupt:
				log.exception("User interruption...")
				if psrpipe != None:
					psrpipe.kill(log)
				# after Ctrl-C and when using "ssh -t" terminal gets messed up, so one has to reset it
				# the command "stty sane" allows to reset terminal without clearing it (it puts all esc sequences
				# to its default values)
				os.system("stty sane")
				sys.exit(1)

			# end of the pipeline...
			end_pipe_time=time.time()
			pipe_total_time = end_pipe_time - start_pipe_time
			log.info("Finished")
			log.info("UTC time is:  %s" % (time.asctime(time.gmtime())))
			log.info("Total wall time:  %.1f s (%.2f hrs)" % (pipe_total_time, pipe_total_time/3600.))

	                # flushing log file and copy it to summary nodes
			log.flush()
			for (sumnode, sumdir) in psrpipe.summary_dirs.items():
				cmd="rsync -avxP %s %s:%s" % (cep2.get_logfile(), sumnode, sumdir)
                       		proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
                       		proc.communicate()
		else:
			# loading pipeline config from the file
			if os.path.exists(pipeline_file):
				pipefd = open(pipeline_file, "rb")
				psrpipe=cPickle.load(pipefd)
				pipefd.close()
			else: psrpipe = Pipeline(obs, cep2, cmdline, log)
			if not cmdline.opts.is_summary:
				# running processing for particular beam
				for unit in psrpipe.units:
					if unit.sapid == sapid and unit.tabid == tabid:
						try:
							unit.run(obs, cep2, cmdline, log)
						except KeyboardInterrupt:
							log.exception("User interruption...")
							unit.kill() # killing all open processes
							# after Ctrl-C and when using "ssh -t" terminal gets messed up, so one has to reset it
							# the command "stty sane" allows to reset terminal without clearing it (it puts all esc sequences
							# to its default values)
							os.system("stty sane")
							sys.exit(1)
					
			else:   # running local pulp to make summary actions
				try:
					psrpipe.make_summary(obs, cep2, cmdline, log)
				except KeyboardInterrupt:
					log.exception("User interruption...")
					if psrpipe != None:
						psrpipe.kill(log)
					# after Ctrl-C and when using "ssh -t" terminal gets messed up, so one has to reset it
					# the command "stty sane" allows to reset terminal without clearing it (it puts all esc sequences
					# to its default values)
					os.system("stty sane")
					sys.exit(1)

	except Exception:
		log.exception("Oops... pulp has crashed!")
		os.system("stty sane")
		sys.exit(1)

	log.flush()
	logfh.close()
	log.removeHandler(logfh)
	# removing log file from ~/.pulp/obsid dir if it is for pulp on local node
	if cmdline.opts.is_local:
		cmd="rm -rf %s" % (cep2.get_logfile())
		os.system(cmd)
	logging.shutdown()
