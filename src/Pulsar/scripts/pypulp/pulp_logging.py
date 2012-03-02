#
# Create our own logging class based on logging module
# Problem with Python logging is that it of course can write to multiple streams
# to stdout, and file. But if you have many system calls with Popen, the output
# from these calls won't be duplicated between several streams 
# It is only possible to achieve running subprocess to PIPE and then logging this output
# as implemented in process2log function
#

import logging
import subprocess

# My logger class that emulates somewhat system logging + has extra functionality
class PulpLogger(object):
	"""
	Fake file-like stream that redirects writes to a logger instance
	"""
	def __init__(self, name=None, log_level=logging.INFO):
		self.logger = logging.getLogger(name)
		self.log_level = log_level
		self.logger.setLevel(self.log_level) # our default verbosity level

		# adding stream handler
		self.logger.addHandler(logging.StreamHandler())

	def set_loglevel (self, log_level):
		self.log_level = log_level
		self.logger.setLevel(self.log_level)

	def addHandler(self, handler):
		self.logger.addHandler(handler)

	def removeHandler(self, handler):
		self.logger.removeHandler(handler)

	def write(self, msg):
		self.logger.log(self.log_level, msg)
#		for line in msg.rstrip().splitlines():
#			self.logger.log(self.log_level, line.rstrip())

	def flush(self):
		for handler in self.logger.handlers:
			handler.flush()

	def info(self, msg):
		self.logger.info(msg)

	def error(self, msg):
		self.logger.error(msg)

	def warning(self, msg):
		self.logger.warning(msg)

	def critical(self, msg):
		self.logger.critical(msg)

	def exception(self, msg):
		self.logger.exception(msg)

	# to log the output from subprocess shell process
	# assuming that stderr from the process will be first redirected to stdout
	def process2log(self, process):
		while True:
			line = process.stdout.readline()
			if not line: break
			self.info(line.rstrip())
