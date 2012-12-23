###################################################################
#
# Class FeedbackUnit - collects feedback info for every output
#                      tarball file from the processing + log file
#

import os, sys, os.path, tarfile

# class for keeping feedback info for processed data type (CS, IS, or CV)
class FeedbackUnit:
	def __init__(self, index, node, path, obsid, sapid, tabid):
		self.index = index
		self.node = node
		self.path = path

		self._data = {
		 'fileFormat' : "PULP",
		 'filename' : "",
		 'location' : self.node + ":" + self.path,
		 'percentageWritten' : 0,
		 'filesize' : 0,
		 'datatype' : "", # CoherentStokes, IncoherentStoks, SummaryCoherentStokes, ...
		 'fileContent' : [],
		 'ObsID' : obsid,
		 'SAP' : sapid,
		 'TAB' : tabid
		}

	# function to update the feedback unit when the actual file exists
	def update(self, filename, code, is_summary=False):
		self._data['filename'] = filename.split("/")[-1]
		self._data['percentageWritten'] = 100
		# file size in bytes
		try:
			self._data['filesize'] = os.path.getsize(self.path + "/" + self._data['filename'])
		except Exception: self._data['filesize'] = 0

		# updating the data type
		if code == "CS": self._data['datatype'] = "CoherentStokes"
		elif code == "IS": self._data['datatype'] = "IncoherentStokes"
		elif code == "CV": self._data['datatype'] = "ComplexVoltages"
		else: self._data['datatype'] == "Unknown"
		if is_summary and self._data['datatype'] != "Unknown":
			self._data['datatype'] = "Summary" + self._data['datatype']

		# getting contents of the tarball file
		try:
			tar=tarfile.open(self.path + "/" + self._data['filename'])
			self._data['fileContent']=[ii.name for ii in tar.getmembers()]
		except Exception: pass

	# function to flush the feedback info to the file
	def flush(self, fbfile, cep2, is_summary, is_log=False):
		fb=open(fbfile, 'w')
		# if it is PULP log-file, then just write 
		if is_log: fb.write("%s = %s/%s\n" % (cep2.feedback_log_prefix, self._data['location'], self._data['filename']))
		else:
			for key, value in self._data.items():
				if is_summary and (key == "SAP" or key == "TAB"): continue
				fb.write("%s[%d].%s = %s\n" % (cep2.feedback_prefix, self.index, key, value))
		fb.close()
