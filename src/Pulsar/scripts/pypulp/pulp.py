#!/usr/bin/env python
#
# This is Python implementation of PULP, Pulsar standard pipeline
# written by A2 in shell
#
# Vlad, Oct 25, 2011 (c)
###################################################################
import os, sys, glob
import getopt
import numpy as np
import time
import cPickle
import re
import subprocess
import logging
from pulp_parset import Parset
from pulp_usercmd import CMDLine
from pulp_sysinfo import CEP2Info

# PLEASE increment the version number when you edit this file!!!
VERSION="3.00"

###  M A I N ###
if __name__ == "__main__":

	# getting info about the CEP2
	cep2 = CEP2Info()

	# initializing the Logger
	log = logging.getLogger()
	log.setLevel(logging.INFO) # our default verbosity level
	logch = logging.StreamHandler()
	log.addHandler(logch)

        # parsing command line
        cmd = CMDLine(cep2, VERSION)

	# adding file handler to our Logger
	logfh = logging.FileHandler(cep2.get_logfile())
	log.addHandler(logfh)

	# checking validity of the options
	cmd.check_options(log)

	# printing info both to STDOUT and logfile
	cep2.print_info(log)
	cmd.print_summary(log)

	os.system('date')
