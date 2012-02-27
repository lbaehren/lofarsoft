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
import logging
from pulp_parset import Observation
from pulp_usercmd import CMDLine
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
from pulp_pipeline import Pipeline

# PLEASE increment the version number when you edit this file!!!
VERSION="5.00"

###  M A I N ###
if __name__ == "__main__":

        # parsing command line
	cmdline = CMDLine(VERSION)

	# creating name of the Logger
	logger_name = "PULP"
	logfile = cmdline.opts.obsid
	if cmdline.opts.beam_str != "":
		logger_name += cmdline.opts.beam_str 
		### I should add checking if beam_str is given correctly
		sapid = int(cmdline.opts.beam_str.split(":")[0])
		tabid = int(cmdline.opts.beam_str.split(":")[1])
		logfile = "%s_sap%03d_beam%04d" % (cmdline.opts.obsid, sapid, tabid)
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
		logfh = logging.FileHandler(cep2.get_logfile(), mode='%c' % (cmdline.opts.is_delete and 'w' or 'a'))
		log.addHandler(logfh)

		# starting logging...
		start_pipe_time=time.time()
		log.info("UTC time is:  %s" % (time.asctime(time.gmtime())))

		# checking validity of the options
		cmdline.check_options(cep2, log)
		if cmdline.opts.beam_str == "": cmdline.print_summary(cep2, log)

		# printing info both to STDOUT and logfile
		cep2.print_info(log)

		obsconf_file = cep2.get_logdir() + "/" + "obs.b"
		pipeline_file = cep2.get_logdir() + "/" + "pipeline.b"

		if not cmdline.opts.is_noinit:
			# checking connections to locus nodes
			cep2.check_connection(log)
			# print down nodes
			cep2.print_down_nodes(log)

			# initializing our Observation, collecting info from parset, etc.
			obs = Observation(cmdline.opts.obsid, cep2, log)

			# saving obs configuration to file
                	obsfd = open (obsconf_file, "wb")
	                cPickle.dump(obs, obsfd, True)  # when False, the dumpfile is ascii
        	        obsfd.close()
		else:
			# loading obs setup from the file
			obsfd = open(obsconf_file, "rb")
			obs=cPickle.load(obsfd)
			obsfd.close()

		# if --beam option is not set, it means that we start the pipeline from main node
		if cmdline.opts.beam_str == "":	
			# initializing pulsar pipeline
			psrpipe = Pipeline(obs, log)
			# saving pipeline config to file
                	pipefd = open (pipeline_file, "wb")
                	cPickle.dump(psrpipe, pipefd, True)
			pipefd.close()
			# kick off the pipeline
			psrpipe.start(cep2, cmdline, log)
			# wait for all childs to finish and prepare logs, all files in order
			# convert, FE maps, etc.
			psrpipe.finish(log)
			# end of the pipeline...
			end_pipe_time=time.time()
			pipe_total_time = end_pipe_time - start_pipe_time
			log.info("Finished")
			log.info("UTC time is:  %s" % (time.asctime(time.gmtime())))
			log.info("Total wall time:  %.1f s (%.2f hrs)" % (pipe_total_time, pipe_total_time/3600.))
		else:
			# loading pipeline config from the file
			pipefd = open(pipeline_file, "rb")
			psrpipe=cPickle.load(pipefd)
			pipefd.close()
			# running processing for particular beam
			for unit in psrpipe.units:
				if unit.sapid == sapid and unit.tabid == tabid:
					unit.run(obs, cep2, cmdline, log)
	except Exception:
		log.exception("Oops... pulp has crashed!")
