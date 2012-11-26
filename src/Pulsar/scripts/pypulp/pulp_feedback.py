###################################################################
#
# Class FeedbackUnit - collects feedback info for every output
#                      tarball file from the processing + log file
#

import os, sys, os.path, tarfile
#import logging
#from pulp_sysinfo import CEP2Info
#from pulp_logging import PulpLogger


# class for keeping feedback info for processed data type (CS, IS, or CV)
class FeedbackUnit:
	def __init__(self, index, node, path):
		self.index = index
		self.node = node
		self.path = path

		self._data = {
		 'fileFormat' : "PULP",
		 'filename' : "",
		 'location' : self.node + ":" + self.path,
		 'percentageWritten' : 0,
		 'filesize' : 0,
		 'datatype' : "", # Log, CoherentStokes, ...
		 'fileContent' : []
		}

	# function to update the feedback unit when the actual file exists
	def update(self, filename, code, log):
		self._data['filename'] = filename.split("/")[-1]
		self._data['percentageWritten'] = 100
		# file size in bytes
		try:
			self._data['filesize'] = os.path.getsize(self.path + "/" + self._data['filename'])
		except Exception:
			log.warning("Can't get the size of the file: %s" % (self._data['location'] + "/" + self._data['filename']))
			self._data['filesize'] = 0

		# updating the data type
		if code == "CS": self._data['datatype'] = "CoherentStokes"
		elif code == "IS": self._data['datatype'] = "IncoherentStokes"
		elif code == "CV": self._data['datatype'] = "ComplexVoltages"
		elif code == "LOG": self._data['datatype'] = "Log" # in case file is general log-file
		else: self._data['datatype'] == "Unknown"

		# getting contents of the tarball file
		try:
			tar=tarfile.open(self.path + "/" + self._data['filename'])
			self._data['fileContent']=[ii.name for ii in tar.getmembers()]
		except Exception: pass

	# function to flush the feedback info to the file
	def flush(self, cep2):
#		lock = lockfile.FileLock(cep2.get_feedbackfile()) # prevent file to be open for writing from other process
#		lock.acquire()
		fb=open(cep2.get_feedbackfile(), 'a')
		for key, value in self._data.items():
			fb.write("%s[%d].%s = %s\n" % (cep2.feedback_prefix, self.index, key, value))
		fb.close()
#		lock.release() # unlocking file
