#
# Class CEP2Info defines different parameters, locus names, necessary ENV variables, etc.
# related to pipeline processing
#
import os
import logging

class CEP2Info:
	# setting different attributes
        def __init__(self):
		self.lofarsoft = os.environ['LOFARSOFT']
		self.pulp_help = self.lofarsoft + "/release/share/pulsar/data/pulp_help.txt"
		self.pythonpath = os.environ['PYTHONPATH']
		# get the node where the scripts run
		self.current_node = os.popen('hostname').readlines()[0][:-1]
		# get current dir
		self.current_dir = os.popen('pwd').readlines()[0][:-1]
		# the logfile will be initialized later when we get info about the ObsID
		self.logfile = ""

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

	# set the logfile
	def set_logfile(self, f):
		self.logfile = self.current_dir + "/" + f

	# print info of all set attributes
	def print_info(self, log=None):
		if log == None:
			print ""
			print " CURRENT NODE = %s" % (self.current_node)
			print "  CURRENT DIR = %s" % (self.current_dir)
			print "    LOFARSOFT = %s" % (self.lofarsoft)
			print "   PYTHONPATH = %s" % (self.pythonpath)
			print ""
		else:
			log.info("")
			log.info(" CURRENT NODE = %s" % (self.current_node))
			log.info("  CURRENT DIR = %s" % (self.current_dir))
			log.info("    LOFARSOFT = %s" % (self.lofarsoft))
			log.info("   PYTHONPATH = %s" % (self.pythonpath))
			log.info("")
		
