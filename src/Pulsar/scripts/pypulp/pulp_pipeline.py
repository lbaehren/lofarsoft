###################################################################
#
# Class Pipeline - main processing class
# Other class, aka Processing Units (per Beam) are defined
#

import os, sys, glob, time, re, os.path
import math
import numpy as np
import cPickle
import subprocess, shlex
from subprocess import PIPE, STDOUT, Popen
import psr_utils as pu
import parfile
from pulp_parset import Observation, radial_distance, find_pulsars
from pulp_usercmd import CMDLine, check_pulsars
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
from pulp_feedback import FeedbackUnit

### define a global function - recursive glob ####

# return list of dirs in the current base directory
def getDirs(base):
	return [x for x in glob.iglob(os.path.join(base, '*')) if os.path.isdir(x)]

# recursive glob
def rglob(base, pattern, maxdepth=0):
	flist=[]
	flist.extend(glob.glob(os.path.join(base, pattern)))
	dirs = getDirs(base)
	if maxdepth <= 0: return flist
	if len(dirs):
		for d in dirs:
			flist.extend(rglob(os.path.join(base, d), pattern, maxdepth-1))
	return flist


# The main processing class
class Pipeline:
	def __init__(self, obs, cep2, cmdline, log):
		self.units = []   # list of processing units
		self.feedbacks = [] # list of feedback units
		self.sum_popens = []  # list of Popen make_summary processes
		self.summary_dirs = {}  # dictionary, key - locus node of the summaries, value - dir
		# extensions of the files to copy to archive in summary (*_nopfd*.tgz)
		self.summary_archive_exts=["*.log", "*.txt", "*.pdf", "*.ps", "*.bestprof", "*.inf", "*.rfirep", "*png", "*parset", "*.par"]
		# prefix and suffix for summary archive name, in between them there will CS, IS, CV code
		self.summary_archive_prefix="_summary"
		self.summary_archive_suffix=".tar"
		self.number_failed_pipes = 0   # number of failed pipelines
		self.number_failed_summaries = 0  # number of failed summaries

		# initializing Processing Units based on the list of beams to process
                for beam in cmdline.beams:
                        sapid=int(beam.split(":")[0])
                        tabid=int(beam.split(":")[1])
                        tab = obs.saps[sapid].tabs[tabid]

			# checking if location (locus node) of raw data or at least processed data is known
			# If not, then skip this beam
			if len(tab.location) == 0: continue

			if not tab.is_coherent:
				unit = ISUnit(obs, cep2, cmdline, tab, log)
			if tab.is_coherent and tab.specificationType != "flyseye":
				if obs.CS:
					unit = CSUnit(obs, cep2, cmdline, tab, log)
				elif obs.CV:
					unit = CVUnit(obs, cep2, cmdline, tab, log)
				else:
					log.error("Can't initialize processing pipeline unit for SAP=%d TAB=%d" % (sapid, tabid))
					quit(1)
			if tab.is_coherent and tab.specificationType == "flyseye":
				if obs.stokesCS[0] == "I":  # for I and IQUV
					unit = FE_CSUnit(obs, cep2, cmdline, tab, log)
				elif obs.stokesCS[0] == "X": # for XY (old format) or XXYY
					unit = FE_CVUnit(obs, cep2, cmdline, tab, log)
				else:
					log.error("Can't initialize processing pipeline FE unit for SAP=%d TAB=%d" % (sapid, tabid))
					quit(1)

			# adding unit to the list
			self.units.append(unit)

		if len(self.units) == 0:
			log.info("None beams to process!")
			quit(0)

		# creating main output directory on locus092 for CS data and on locus094 for IS data
		# before that we also remove this directory if user flag is_del was set
		unique_outdirs=["%s:%s_%s/%s%s" % (unit.summary_node, cep2.processed_dir_prefix, unit.summary_node, \
			cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, \
			unit.summary_node_dir_suffix) for unit in self.units if unit.summary_node != "" and unit.summary_node_dir_suffix != ""]
		unique_outdirs=np.unique(unique_outdirs)
		fbindex = obs.nrTiedArrayBeams  # file index for feedback file, the offset accounts for total number of TABs (each has its own feedback)
		for uo in unique_outdirs:
			node=uo.split(":")[0]
			sumdir=uo.split(":")[1]
			self.summary_dirs[node] = sumdir
			fbunit = FeedbackUnit(fbindex, node, sumdir, cmdline.opts.obsid, -1, -1)
			self.feedbacks.append(fbunit)
			fbindex += 1
			# deleting previous results if option --del was set
			if cmdline.opts.is_delete:
				log.info("Deleting previous summary results on %s: %s" % (node, sumdir))
#				cmd="%s %s 'rm -rf %s'" % (cep2.cexeccmd, cep2.cexec_nodes[node], sumdir)
				# Using ssh in the background instead of cexec in order to get proper status code if job has failed
				# with cexec you always get status=0
				# Also, using "-t" option in ssh allows to send KILL signal to remote command over ssh to finish all processes there
				# in case when pipeline was interrupted
				# with nohup and </dev/null the parent bash process only kiled when interrupted by User..
#				cmd="ssh -t %s 'nohup rm -rf %s </dev/null 2>&1'" % (node, sumdir)
				# stdin=open(os.devnull, 'rb')  should help to avoid suspending pulp.py when run in the background
				cmd="ssh -t %s 'rm -rf %s'" % (node, sumdir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()
			log.info("Creating output summary directory on %s: %s" % (node, sumdir))
#			cmd="%s %s 'mkdir -p %s'" % (cep2.cexeccmd, cep2.cexec_nodes[node], sumdir)
			# Using ssh in the background instead of cexec in order to get proper status code if job has failed
			# with cexec you always get status=0
			# Also, using "-t" option in ssh allows to send KILL signal to remote command over ssh to finish all processes there
			# in case when pipeline was interrupted
			# with nohup and </dev/null the parent bash process only kiled when interrupted by User..
#			cmd="ssh -t %s 'nohup mkdir -p %s </dev/null 2>&1'" % (node, sumdir)
			# stdin=open(os.devnull, 'rb')  should help to avoid suspending pulp.py when run in the background
			cmd="ssh -t %s 'mkdir -m 775 -p %s'" % (node, sumdir)
			p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			p.communicate()
		
		# Defining output directories for all local locus nodes
		# We have to do it here rather than in 'run' function of PipeUnit because there can be more than 1 beam per node
		# and then there can be a clash when we will delete (if -del is set) the output directory 
		unit_outdirs = ["%s:%s" % (unit.outdir.split(cep2.processed_dir_prefix + "_")[1].split("/")[0], unit.outdir) for unit in self.units]
		unit_outdirs=np.unique(unit_outdirs)

		# Deleting all local output directories if --del is set 
		if cmdline.opts.is_delete:
			for uo in unit_outdirs:
				node=uo.split(":")[0]
				outdir=uo.split(":")[1]
				log.info("Deleting previous processed results on %s: %s" % (node, outdir))
				cmd="ssh -t %s 'rm -rf %s'" % (node, outdir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()
				Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
		log.info("")



	# kicking off the pipeline
	def start(self, obs, cep2, cmdline, log):
		# and here we start...
		log.info("Starting PULP processing for:")

		for unit in self.units:
			# if data for this beam are on several nodes, then we have to log in to hoover node...
			if len(unit.tab.location) > 1:
				if unit.tab.is_coherent: locus=cep2.hoover_nodes[0] # we choose one hoover node for CS data
#				else: locus=cep2.hoover_nodes[1]                    # and another for IS data
				else: locus=cep2.hoover_nodes[0]                    # and another for IS data
			else:
				locus=unit.tab.location[0]
#			cmd="%s %s 'pulp.py --noinit --local --beams %d:%d %s'" %  \
#				(cep2.cexeccmd, cep2.cexec_nodes[locus], unit.sapid, unit.tabid, " ".join(cmdline.options))
			# Using ssh in the background instead of cexec in order to get proper status code if job has failed
			# with cexec you always get status=0
			# Also, using "-t" option in ssh allows to send KILL signal to remote command over ssh to finish all processes there
			# in case when pipeline was interrupted
			# with nohup and </dev/null the parent bash process only kiled when interrupted by User..
#			cmd="ssh -t %s 'nohup pulp.py --noinit --local --beams %d:%d %s </dev/null 2>&1'" % \
			cmd="ssh -t %s '%s/release/share/pulsar/bin/pulp.py --noinit --local --beams %d:%d %s'" % \
				(locus, cep2.lofarsoft, unit.sapid, unit.tabid, " ".join(cmdline.options))
			unit.parent = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
			log.info("SAP=%d TAB=%d %s(%s%s) on %s (pid=%d)  [#locations = %d, #files = %d]" % \
				(unit.sapid, unit.tabid, unit.tab.specificationType == "flyseye" and ", ".join(unit.tab.stationList) + " " or "", \
				unit.tab.specificationType == "flyseye" and "FE/" or "", \
				unit.tab.is_coherent and (obs.CS and "CS" or "CV") or "IS", \
				locus, unit.parent.pid, len(unit.tab.location), unit.tab.numfiles))
			time.sleep(1) # wait 1 sec (otherwise terminal output gets messed up often)


	# here we finish the pipeline, creating extra files, convert ps(pdf) to png, etc...
	# create FE status maps, TA heat maps...
	def finish(self, obs, cep2, cmdline, log):

		try:
			# waiting for processing in individual locus nodes to finish
			# unless we want to do just a summary
			if not cmdline.opts.is_summary:
				run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
				Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
				log.info("Still running [%d]: %s" % (len(run_units), run_units))
				for unit in self.units:
					Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
					log.info("waiting...")
					unit.parent.communicate()
					log.info("Process pid=%d has finished, status=%d" % (unit.parent.pid, unit.parent.returncode))
					run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
					if len(run_units) > 0: log.info("Still running [%d]: %s" % (len(run_units), run_units))

				# loop over finished processes to see if they all finished OK
				failed_units = [u for u in self.units if u.parent.returncode > 0]
				Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
				self.number_failed_pipes = len(failed_units)
				log.info("Failed beams [%d]: %s" % (self.number_failed_pipes, ",".join(["%s:%s" % (u.sapid, u.tabid) for u in failed_units])))
				if self.number_failed_pipes > 0:
					log.info("*** Summaries will not be complete! Re-run processing for the failed beams using --beams option. ***")

			self.sum_popens=[]
			log.info("Starting summaries...")
			# starting separate pulp.py on summary nodes just to finish up
			for (sumnode, sumdir) in self.summary_dirs.items():
#				cmd="%s %s 'pulp.py --noinit --summary --local --beams %s %s'" %  \
#					(cep2.cexeccmd, cep2.cexec_nodes[sumnode], sumnode, " ".join(cmdline.options))
				# Using ssh in the background instead of cexec in order to get proper status code if job has failed
				# with cexec you always get status=0
#				cmd="ssh -t %s 'nohup /home/kondratiev/pulp/pulp.py --noinit --summary --local --beams %s %s </dev/null 2>&1'" % \
				cmd="ssh -t %s '%s/release/share/pulsar/bin/pulp.py --noinit --summary --local --beams %s %s'" % \
					(sumnode, cep2.lofarsoft, sumnode, " ".join(cmdline.options))
				sum_popen = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				self.sum_popens.append(sum_popen)
				log.info("Making summaries on %s... (pid=%d)" % (sumnode, sum_popen.pid))

			run_units = [p.pid for p in self.sum_popens if p.poll() is None]
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
			log.info("Still running [%d]: %s" % (len(run_units), run_units))
			for proc in self.sum_popens:
				Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
				log.info("waiting...")
				proc.communicate()
				log.info("Process pid=%d has finished, status=%d" % (proc.pid, proc.returncode))
				run_units = [p.pid for p in self.sum_popens if p.poll() is None]
				finished_units = [p for p in self.sum_popens if p.poll() is not None]
				for fu in finished_units:
					if fu.returncode != 0: raise Exception
					else: self.sum_popens.remove(fu)
				if len(run_units) > 0: log.info("Still running [%d]: %s" % (len(run_units), run_units))

			# loop over finished summaries to see if they all finished OK
			failed_summaries = [s for s in self.sum_popens if s.returncode > 0]
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
			self.number_failed_summaries = len(failed_summaries)
			log.info("%d failed summaries" % (self.number_failed_summaries))

		except Exception:
			log.exception("Oops... 'finish' function of the pipeline has crashed!")
			self.kill(log)
			quit(1)

	# return number of failed pipelines
	def get_number_failed_pipes(self):
		return self.number_failed_pipes

	# return number of failed summaries
	def get_number_failed_summaries(self):
		return self.number_failed_summaries

	# execute command on local node (similar to execute in PipeUnit)
	def execute(self, cmd, log, workdir=None, shell=False, is_os=False):
	    	"""
        	Execute the command 'cmd' after logging the command
            	and the wall-clock amount of time the command took to execute.
		This function waits for process to finish
    		"""
		try:
			log.info(re.sub("\n", "\\\\n", cmd))  # also escaping \n to show it as it is
    			job_start = time.time()
			log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
			status = 1
			if is_os: status = os.system(cmd)
			else:
               			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
       	        		log.process2log(proc)
        	       		proc.communicate()
				status = proc.poll()
			job_end = time.time()
			job_total_time = job_end - job_start
        	       	log.info("Finished at UTC %s, status=%s, Total runnung time: %.1f s (%.2f hrs)" % \
					(time.asctime(time.gmtime()), status, job_total_time, job_total_time/3600.))
			log.info("")
			# if job is not successful
			if status != 0:
				raise Exception
		except Exception:
			log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (cmd, status))
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
			raise Exception

	# function that checks all processes in the list and kill them if they are still running
	def kill(self, log=None):
		if log != None: log.info("Killing all open processes...")
		for unit in self.units:
			if unit.parent != None and unit.parent.poll() is None:
				unit.parent.kill()
				if unit.parent != None: unit.parent.communicate()
				if unit.parent != None: unit.parent.poll()
		self.units = []
		# killing summary processes if open
		for sum_popen in self.sum_popens:
			if sum_popen != None and sum_popen.poll() is None:
				sum_popen.kill()
				if sum_popen != None: sum_popen.communicate()
				if sum_popen != None: sum_popen.poll()
		self.sum_popens=[]

	# make feedback file
	def make_feedback(self, obs, cep2, cmdline, log=None):
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# moving log-files to corresponding directory
		if log != None: log.info("Moving log-files...")
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)):
				if not cmdline.opts.is_log_append:	
					cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
						(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
					if log != None: self.execute(cmd, log, workdir=sumdir)
					else:
						proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
						proc.communicate()
				else:
					# appending log from sumdir to the one in corresponing beam directory
					cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
						(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
					if log != None: self.execute(cmd, log, is_os=True)
					else: os.system(cmd)
					# removing log from sumdir
					cmd="rm -f %s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)
					if log != None: self.execute(cmd, log, workdir=sumdir)
					else: os.system(cmd)

                # either "CS", "IS", "CV", ..
                data_code=[u.code for u in self.units if u.summary_node == sumnode][0]
		tarname="%s%s%s%s" % (obs.id, self.summary_archive_prefix, data_code, self.summary_archive_suffix)
		# updating the Feedback unit
		fbunit=[u for u in self.feedbacks if u.node == sumnode and u.path == sumdir][0]
		fbunit.update("%s/%s.gz" % (sumdir, tarname), data_code, True)
		fbunit.flush("%s/.%s%s%s.fb" % (cep2.get_logdir(), obs.id, self.summary_archive_prefix, data_code), cep2, True)

	# run necessary processes to organize summary info on summary nodes
	# to be run locally on summary node
	def make_summary(self, obs, cep2, cmdline, log):
		try:
			sumnode=cep2.get_current_node()
			data_code=[u.code for u in self.units if u.summary_node == sumnode][0]

			if cmdline.opts.is_feedback: 
				self.make_feedback(obs, cep2, cmdline, log)
			else:
				if data_code == "CS" or data_code == "IS":
					self.make_summary_CS_IS(obs, cep2, cmdline, log)
				if data_code == "CV":
					self.make_summary_CV(obs, cep2, cmdline, log)

		except Exception:
			log.exception("Oops... 'make_summary' function on %s has crashed!" % (cep2.get_current_node()))
			quit(1)

	# run necessary processes to organize summary info on summary nodes for CV data
	# to be run locally on summary node
	def make_summary_CV(self, obs, cep2, cmdline, log):

		start_time=time.time()	
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# start logging
		log.info("Summaries on %s:%s    UTC start time is: %s  @node: %s" % \
				(sumnode, sumdir, time.asctime(time.gmtime()), sumnode))

		# extracting files from archive files for all beams
		# and moving log-files to corresponding directory
		log.info("Extracting archives in summary nodes, removing archives, moving log-files...")
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			result_archive="%s_sap%03d_tab%04d%s" % (obs.id, unit.sapid, unit.tabid, unit.archive_suffix)
			if os.path.exists("%s/%s" % (sumdir, result_archive)):
				# extracting archive
				cmd="tar xvfz %s" % (result_archive)
				self.execute(cmd, log, workdir=sumdir)
				# removing archive
				cmd="rm -f %s" % (result_archive)
				self.execute(cmd, log, workdir=sumdir)
				# moving log-file to corresponding SAP/BEAM directory
				if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)):
					if not cmdline.opts.is_log_append:	
						cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
							(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
						self.execute(cmd, log, workdir=sumdir)
					else:
						# appending log from sumdir to the one in corresponing beam directory
						cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
							(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, log, is_os=True)
						# removing log from sumdir
						cmd="rm -f %s_sap%03d_beam%04d.log" % (obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, log, workdir=sumdir)
			else:
				if not os.path.exists("%s/%s" % (sumdir, unit.curdir.split(unit.outdir + "/")[1])):
					log.warning("Warning! Neither archive file %s nor corresponding directory tree exists in: %s. Summary won't be complete" % (result_archive, sumdir))

                # either "CS", "IS", "CV", ..
                data_code=[u.code for u in self.units if u.summary_node == sumnode][0]

		# create beam_process_node.txt file (this is only if file does not exist or it is empty)
		beam_process_node_file="%s/beam_process_node.txt" % (sumdir)
		if not os.path.exists(beam_process_node_file) or os.path.getsize(beam_process_node_file) == 0:
			log.info("Creating the beam_process_node.txt file...")
        	        bpnf=open(beam_process_node_file, 'w')
			for unit in [u for u in self.units if u.summary_node == sumnode]: 
				for node in unit.tab.location:
					if node in unit.tab.rawfiles:
						for rf in unit.tab.rawfiles[node]:
							bpnf.write("%s %s%s\n" % \
							(node, rf, unit.tab.specificationType == "flyseye" and " [%s]" % (",".join(unit.tab.stationList)) or ""))
			bpnf.close()

		# creating combined DSPSR plots
		dspsr_diags=rglob(sumdir, "*_diag.png", 3)
		if len(dspsr_diags) > 0:
			log.info("Creating DSPSR summary diagnostic plots...")
			if len(dspsr_diags) > 1: cmd="montage %s -background none -mode concatenate -tile %dx dspsr_status.png" % (" ".join(dspsr_diags), int(math.sqrt(len(dspsr_diags))))
			else: cmd="mv %s dspsr_status.png" % (dspsr_diags[0])
			self.execute(cmd, log, workdir=sumdir)

		if os.path.exists("%s/dspsr_status.png" % (sumdir)):
			log.info("Copying dspsr status file to status.png ...")
			cmd="cp -f dspsr_status.png status.png"
			self.execute(cmd, log, workdir=sumdir)

		# creating thumbnail version of status.png if it exists
		if os.path.exists("%s/status.png" % (sumdir)):		
			log.info("Making a thumbnail version of status.png file...")
			cmd="convert -scale 200x140-0-0 status.png status.th.png"
			self.execute(cmd, log, workdir=sumdir)
		else:
			log.info("No status.png created")

		# Make a tarball of all the plots (summary archive)
		log.info("Making a final summary tarball of all files with extensions: %s" % (", ".join(self.summary_archive_exts)))
		tarname="%s%s%s%s" % (obs.id, self.summary_archive_prefix, data_code, self.summary_archive_suffix)
		tar_list=[]
		for ext in self.summary_archive_exts:
			ext_list=rglob(sumdir, ext, 3)
			tar_list.extend(ext_list)
		cmd="tar -cv --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
		try: # --ignore-failed-read does not seem to help with tar failing for some beams
                     # like file was changed during the tar, though tarball seem to be fine
			self.execute(cmd, log, workdir=sumdir)
		except: pass

		# finish
		end_time=time.time()
		total_time= end_time- start_time
		log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		log.info("Total runnung time: %.1f s (%.2f hrs)" % (total_time, total_time/3600.))

		# flushing log file and copy it to summary node
		log.flush()
		if not cmdline.opts.is_log_append: cmd="cp -f %s %s" % (cep2.get_logfile(), sumdir)
		else: cmd="cat %s >> %s/%s" % (cep2.get_logfile(), sumdir, cep2.get_logfile().split("/")[-1])
		os.system(cmd)

		# changing the file permissions to be re-writable for group
		cmd="chmod -R g+w %s" % (sumdir)
		os.system(cmd)

		# delete file from the archive first
		cmd="tar --delete -v --ignore-failed-read -f %s %s" % (tarname, cep2.get_logfile().split("/")[-1])
		try:
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
			proc.communicate()
			# adding log file to the archive and gzip it
			cmd="tar -rv --ignore-failed-read -f %s %s" % (tarname, cep2.get_logfile().split("/")[-1])
			# --ignore-failed-read does not seem to help with tar failing for some beams
			# like file was changed during the tar, though tarball seem to be fine
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
			proc.communicate()
		except: pass
		cmd="gzip -f %s" % (tarname)
		proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
		proc.communicate()

		# updating the Feedback unit
		self.make_feedback(obs, cep2, cmdline)


	# run necessary processes to organize summary info on summary nodes for CS and IS data
	# to be run locally on summary node
	def make_summary_CS_IS(self, obs, cep2, cmdline, log):

		start_time=time.time()	
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# start logging
		log.info("Summaries on %s:%s    UTC start time is: %s  @node: %s" % \
				(sumnode, sumdir, time.asctime(time.gmtime()), sumnode))

		# extracting files from archive files for all beams
		# and moving log-files to corresponding directory
		log.info("Extracting archives in summary nodes, removing archives, moving log-files...")
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			result_archive="%s_sap%03d_tab%04d%s" % (obs.id, unit.sapid, unit.tabid, unit.archive_suffix)
			if os.path.exists("%s/%s" % (sumdir, result_archive)):
				# extracting archive
				cmd="tar xvfz %s" % (result_archive)
				self.execute(cmd, log, workdir=sumdir)
				# removing archive
				cmd="rm -f %s" % (result_archive)
				self.execute(cmd, log, workdir=sumdir)
				# moving log-file to corresponding SAP/BEAM directory
				if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)):
					if not cmdline.opts.is_log_append:	
						cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
							(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
						self.execute(cmd, log, workdir=sumdir)
					else:
						# appending log from sumdir to the one in corresponing beam directory
						cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
							(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, log, is_os=True)
						# removing log from sumdir
						cmd="rm -f %s_sap%03d_beam%04d.log" % (obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, log, workdir=sumdir)
			else:
				if not os.path.exists("%s/%s" % (sumdir, unit.curdir.split(unit.outdir + "/")[1])):
					log.warning("Warning! Neither archive file %s nor corresponding directory tree exists in: %s. Summary won't be complete" % (result_archive, sumdir))

		# either "CS", "IS", "CV", .. 
		data_code=[u.code for u in self.units if u.summary_node == sumnode][0]

		# getting the list of *.pfd.bestprof files and read chi-sq values for all folded pulsars
		if not cmdline.opts.is_nofold:
			log.info("Reading chi-squared values and adding to chi-squared.txt...")
                        # also preparing montage command to create combined plot
	       	        montage_cmd="montage -background none -pointsize 10.2 "
	       	        montage_cmd_pdf="montage -geometry 100% -adjoin -tile 1x1 -pointsize 12 "
                	chif=open("%s/chi-squared.txt" % (sumdir), 'w')
     	       	        psr_bestprofs=rglob(sumdir, "*.pfd.bestprof", 3)
			if len(psr_bestprofs) > 0:
				# check first if all available *bestprof files are those created without mask. If so, then allow to make
				# a diagnostic combined plot using prepfold plots without a mask
				good_bestprofs=[file for file in psr_bestprofs if re.search("_nomask_", file) is None]
				if len(good_bestprofs) == 0:
					good_bestprofs=[file for file in psr_bestprofs]
                       		for bp in good_bestprofs:
               	               		psr=bp.split("/")[-1].split("_")[0]
                	        	thumbfile=bp.split(sumdir+"/")[-1].split(".pfd.bestprof")[0] + ".pfd.th.png"
					# we need it for combined.pdf
					bigfile=thumbfile.split(".th.png")[0] + ".png"
               	        		# getting current number for SAP and TA beam (or station name for FE)
					try: # we need this try block in case User manually creates sub-directories with some test bestprof files there
					     # which will also be found by rglob function. So, we need to exclude them by catching an Exception
					     # on a wrong-formed string applying int()
                      		  		cursapid=int(thumbfile.split("_SAP")[-1].split("_")[0])
					except: continue
               	                	curprocdir=thumbfile.split("_SAP")[-1].split("_")[1]
      	               	        	chi_val = 0.0
                        		cmd="cat %s | grep chi-sqr | cut -d = -f 2" % (bp)
	               	        	chiline=os.popen(cmd).readlines()
                        		if np.size(chiline) > 0:
               	                		chi_val = float(chiline[0].rstrip())
   	               	        	else:
						log.warning("Warning: can't read chi-sq value from %s" % (bp))
	                        	chif.write("file=%s obs=%s_SAP%d_%s_%s chi-sq=%g\n" % (thumbfile, data_code, cursapid, curprocdir, psr, chi_val))
					if os.path.exists("%s/%s" % (sumdir, thumbfile)):
        	                		montage_cmd += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, thumbfile)
					if os.path.exists("%s/%s" % (sumdir, bigfile)):
        	                		montage_cmd_pdf += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, bigfile)
              	        chif.close()

			# creating combined plots
			if len(psr_bestprofs) > 0:
                        	log.info("Combining all pfd.th.png files in a single combined plot...")
        	        	montage_cmd += "combined.png"
				self.execute(montage_cmd, log, workdir=sumdir)
				# making thumbnail version of the combined plot
				log.info("Making a thumbnail version of combined plot...")
               	        	cmd="convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.png combined.th.png"
               			self.execute(cmd, log, workdir=sumdir)

				# creating combined PDF plot with all prepfold plots - ONLY for FE 
				if data_code == "CS" and obs.FE:
                        		log.info("Combining all pfd.pdf files in a single combined multi-page PDF file...")
        	        		montage_cmd_pdf += "combined.pdf"
					self.execute(montage_cmd_pdf, log, workdir=sumdir)

		# create beam_process_node.txt file (this is only if file does not exist or it is empty)
		beam_process_node_file="%s/beam_process_node.txt" % (sumdir)
		if not os.path.exists(beam_process_node_file) or os.path.getsize(beam_process_node_file) == 0:
			log.info("Creating the beam_process_node.txt file...")
        	        bpnf=open(beam_process_node_file, 'w')
			for unit in [u for u in self.units if u.summary_node == sumnode]: 
				for node in unit.tab.location:
					if node in unit.tab.rawfiles:
						for rf in unit.tab.rawfiles[node]:
							bpnf.write("%s %s%s\n" % \
							(node, rf, unit.tab.specificationType == "flyseye" and " [%s]" % (",".join(unit.tab.stationList)) or ""))
			bpnf.close()

		# removing old version of all status png files (if exist)
		log.info("Removing previous status png files (if any) ...")
		cmd="rm -f status.png status.th.png TAheatmap_status.png TAheatmap_status.th.png FE_status.png FE_status.th.png dspsr_status.png dspsr_status.th.png"
		self.execute(cmd, log, workdir=sumdir)

		# creating TA heatmaps 
		# only when folding, and only if pulsars are set from the command line, or 'parset' or 'sapfind' or 'sapfind3' keywords are used (or
		# nothing is given for --pulsar option
		# otherwise, different TA beams will be folded for different pulsars, and TA heatmap does not have sense
		if data_code == "CS" and not cmdline.opts.is_nofold and (len(cmdline.psrs) == 0 or (len(cmdline.psrs) != 0 and cmdline.psrs[0] != "tabfind")):
			for sap in obs.saps:
				if sap.nrRings > 0:
					if len(cmdline.psrs) != 0 and cmdline.psrs[0] != "parset" and cmdline.psrs[0] != "sapfind" and \
										cmdline.psrs[0] != "sapfind3" and cmdline.psrs[0] != "tabfind+":
						psrs = cmdline.psrs # getting list of pulsars from command line
					else: # getting list of pulsars from SAP
						psrs = sap.psrs
					log.info("Creating TA heatmap with %d rings for SAP=%d..." % (sap.nrRings, sap.sapid))
					for psr in psrs:
						log.info(psr)
						cmd="cat %s/chi-squared.txt | grep _SAP%d | grep %s > %s/%s-chi-squared.txt" % (sumdir, sap.sapid, psr, sumdir, psr)
						self.execute(cmd, log, is_os=True)
						cmd="plot_LOFAR_TA_multibeam2.py --sap %d --chi %s-chi-squared.txt --parset %s.parset --out_logscale %s_SAP%d_%s_TA_heatmap_log.png --out_linscale %s_SAP%d_%s_TA_heatmap_linear.png --target %s" % (sap.sapid, psr, obs.id, obs.id, sap.sapid, psr, obs.id, sap.sapid, psr, psr)
						self.execute(cmd, log, workdir=sumdir)
						cmd="rm -f %s-chi-squared.txt" % (psr)
						self.execute(cmd, log, workdir=sumdir)
						# combining TA heatmap log and linear plots
						cmd="convert %s_SAP%d_%s_TA_heatmap_log.png %s_SAP%d_%s_TA_heatmap_linear.png -append ta_heatmap_sap%d_%s.png" % (obs.id, sap.sapid, psr, obs.id, sap.sapid, psr, sap.sapid, psr)
						self.execute(cmd, log, workdir=sumdir)

					# combining TA heatmaps for different pulsars
					heatmaps=glob.glob("%s/ta_heatmap_sap%d_*.png" % (sumdir, sap.sapid))
					if len(heatmaps) > 0:
						if len(heatmaps) > 1: cmd="convert %s +append ta_heatmap_sap%d.png" % (" ".join(heatmaps), sap.sapid)
						else: cmd="mv %s ta_heatmap_sap%d.png" % (heatmaps[0], sap.sapid)
						self.execute(cmd, log, workdir=sumdir)
						# remove temporary png files
						cmd="rm -f ta_heatmap_sap%d_*.png" % (sap.sapid)
						self.execute(cmd, log, workdir=sumdir)
			# combining TA heatmaps for different SAPs
			heatmaps=glob.glob("%s/ta_heatmap_sap*.png" % (sumdir))
			if len(heatmaps) > 0:
				if len(heatmaps) > 1: cmd="convert %s -append TAheatmap_status.png" % (" ".join(heatmaps))
				else: cmd="mv %s TAheatmap_status.png" % (heatmaps[0])
				self.execute(cmd, log, workdir=sumdir)
				# remove temporary png files
				cmd="rm -f ta_heatmap_sap*.png"
				self.execute(cmd, log, workdir=sumdir)
				# making a thumbnail version of TA heatmap combined plot
				cmd="convert -scale 200x140-0-0 TAheatmap_status.png TAheatmap_status.th.png"
				self.execute(cmd, log, workdir=sumdir)

		# creating combined DSPSR plots
		if not cmdline.opts.is_skip_dspsr:
			dspsr_diags=rglob(sumdir, "*_diag.png", 3)
			if len(dspsr_diags) > 0:
				log.info("Creating DSPSR summary diagnostic plots...")
				if len(dspsr_diags) > 1: cmd="montage %s -background none -mode concatenate -tile %dx dspsr_status.png" % (" ".join(dspsr_diags), int(math.sqrt(len(dspsr_diags))))
				else: cmd="mv %s dspsr_status.png" % (dspsr_diags[0])
				self.execute(cmd, log, workdir=sumdir)
				# making a thumbnail version of combined DSPSR plot
				cmd="convert -scale 200x140-0-0 dspsr_status.png dspsr_status.th.png"
				self.execute(cmd, log, workdir=sumdir)

		# creating FE status maps
		# FE combined map if exist - should be called FE_status.png
		if data_code == "CS" and not cmdline.opts.is_nofold and obs.FE:
			log.info("Creating FE status maps...")
			cmd="lofar_status_map.py"
			self.execute(cmd, log, workdir=sumdir)
			femaps=glob.glob("%s/*_core_status.png" % (sumdir))
			femaps.extend(glob.glob("%s/*_remote_status.png" % (sumdir)))
			femaps.extend(glob.glob("%s/*_intl_status.png" % (sumdir)))
			# creating combined plots
			if len(femaps) > 0:
				cmd="convert -append %s FE_status.png" % (" ".join(femaps))
				self.execute(cmd, log, workdir=sumdir)
				# removing individual maps
				cmd="rm -f %s" % (" ".join(femaps))
				self.execute(cmd, log, workdir=sumdir)
				# making a thumbnail version of FE status map
				cmd="convert -scale 200x140-0-0 FE_status.png FE_status.th.png"
				self.execute(cmd, log, workdir=sumdir)

		# Combining different status maps into one 'status.png' to be shown in web-summary page 
		# combining FE maps to general status.png
		if os.path.exists("%s/FE_status.png" % (sumdir)):
			log.info("Copying FE status map file to status.png ...")
			cmd="cp -f FE_status.png status.png"
			self.execute(cmd, log, workdir=sumdir)
			cmd="mv FE_status.th.png status.th.png"
			self.execute(cmd, log, workdir=sumdir)

		# combining TA heatmaps to general status.png
		if os.path.exists("%s/TAheatmap_status.png" % (sumdir)):
			if os.path.exists("%s/status.png" % (sumdir)): # means that FE maps were created
				log.info("Appending TA heatmap map file to status.png ...")
				cmd="montage status.png TAheatmap_status.png -background none -mode concatenate -tile 2x .temp_status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="montage status.th.png TAheatmap_status.th.png -background none -mode concatenate -tile 2x .temp_status.th.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv .temp_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv .temp_status.th.png status.th.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="rm -f TAheatmap_status.th.png"
				self.execute(cmd, log, workdir=sumdir)
			else:  # if status.png does not exist yet
				log.info("Copying TA heatmap map file to status.png ...")
				cmd="cp -f TAheatmap_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv TAheatmap_status.th.png status.th.png"
				self.execute(cmd, log, workdir=sumdir)

		# combining dspsr summary plots to general status.png
		if os.path.exists("%s/dspsr_status.png" % (sumdir)):
			if os.path.exists("%s/status.png" % (sumdir)): # means that either FE maps or TA heatmap(s) were created
				log.info("Appending dspsr status file to status.png ...")
				cmd="montage status.png dspsr_status.png -background none -mode concatenate -tile 2x .temp_status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="montage status.th.png dspsr_status.th.png -background none -mode concatenate -tile 2x .temp_status.th.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv .temp_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv .temp_status.th.png status.th.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="rm -f dspsr_status.th.png"
				self.execute(cmd, log, workdir=sumdir)
			else:  # if status.png does not exist yet
				log.info("Copying dspsr status file to status.png ...")
				cmd="cp -f dspsr_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
				cmd="mv dspsr_status.th.png status.th.png"
				self.execute(cmd, log, workdir=sumdir)

		# creating thumbnail version of status.png if it exists
		if os.path.exists("%s/status.png" % (sumdir)):		
			log.info("Making a thumbnail version of status.png file...")
			cmd="convert -scale 200x140-0-0 status.th.png .temp_status.th.png"
			self.execute(cmd, log, workdir=sumdir)
			cmd="mv .temp_status.th.png status.th.png"
			self.execute(cmd, log, workdir=sumdir)
		else:
			log.info("No status.png created")
			

		# Make a tarball of all the plots (summary archive)
		log.info("Making a final summary tarball of all files with extensions: %s" % (", ".join(self.summary_archive_exts)))
		tarname="%s%s%s%s" % (obs.id, self.summary_archive_prefix, data_code, self.summary_archive_suffix)
		tar_list=[]
		for ext in self.summary_archive_exts:
			ext_list=rglob(sumdir, ext, 3)
			tar_list.extend(ext_list)
		cmd="tar -cv --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
		try: # --ignore-failed-read does not seem to help with tar failing for some beams
                     # like file was changed during the tar, though tarball seem to be fine
			self.execute(cmd, log, workdir=sumdir)
		except: pass

		# finish
		end_time=time.time()
		total_time= end_time- start_time
		log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		log.info("Total runnung time: %.1f s (%.2f hrs)" % (total_time, total_time/3600.))

		# flushing log file and copy it to summary node
		log.flush()
		if not cmdline.opts.is_log_append: cmd="cp -f %s %s" % (cep2.get_logfile(), sumdir)
		else: cmd="cat %s >> %s/%s" % (cep2.get_logfile(), sumdir, cep2.get_logfile().split("/")[-1])
		os.system(cmd)

		# changing the file permissions to be re-writable for group
		cmd="chmod -R g+w %s" % (sumdir)
		os.system(cmd)

		# delete file from the archive first
		cmd="tar --delete -v --ignore-failed-read -f %s %s" % (tarname, cep2.get_logfile().split("/")[-1])
		try:
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
			proc.communicate()
			# adding log file to the archive and gzip it
			cmd="tar -rv --ignore-failed-read -f %s %s" % (tarname, cep2.get_logfile().split("/")[-1])
			# --ignore-failed-read does not seem to help with tar failing for some beams
			# like file was changed during the tar, though tarball seem to be fine
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
			proc.communicate()
		except: pass
		cmd="gzip -f %s" % (tarname)
		proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
		proc.communicate()

		# updating the Feedback unit
		self.make_feedback(obs, cep2, cmdline)


# base class for the single processing (a-ka beam)
class PipeUnit:
	def __init__(self, obs, cep2, cmdline, tab, log):
		self.code = ""  # 2-letter code, CS, IS, CV
		self.stokes = "" # Stokes I, IQUV, or XXYY
		self.sapid = tab.parent_sapid
		self.tabid = tab.tabid
		self.tab = tab
		self.parent = None   # parent Popen project
		self.procs = []      # list of open processes
				     # process.pid - pid, process.returncode - status
                                     # if returncode == None, it means that process is still running
		self.outdir = ""     # root directory with processed data
		self.curdir = ""     # current processing directory
		self.beams_root_dir = ""  # 'stokes' for CS, 'incoherentstokes' for IS
		self.summary_node = "" # summary node, for CS - locus092, for IS - locus094
		self.summary_node_dir_suffix = ""  # for CS this is "_CSplots" and for IS - "_redIS"
		self.archive_suffix = "" # "_plotsCS.tar.gz" for CS and "_plotsIS.tar.gz" for IS
		self.outdir_suffix = "" #  for CS this is "_red" (if user did not specify) and "_redIS" for IS
		self.raw2fits_extra_options = ""  # extra options for 2bf2fits:  -CS -H for CS, -CS -H -IS for IS
		self.nrChanPerSub = 0
		self.sampling = 0
		self.log = None
		self.logfile = ""    # basename of the logfile
		self.start_time = 0  # start time of the processing (in s)
		self.end_time = 0    # end time (in s)
		self.total_time = 0  # total time in s 
		# extensions of the files to copy to summary node
		self.extensions=["*.log", "*.txt", "*.pdf", "*.ps", "*.bestprof", "*.inf", "*.rfirep", "*png"]
		self.procdir = "BEAM%d" % (self.tabid)
		# extensions for the full archive, e.g. LTA
		self.full_archive_exts=["*.log", "*.txt", "*.pdf", "*.ps", "*.pfd", "*.bestprof", "*.polycos", "*.inf", "*.rfirep", "*png", "*.ar", "*.AR", "*pdmp*", "*_rfifind*", "*.dat", "*.singlepulse", "*.h5"]

		# pulsars to fold for this unit
		self.psrs = []
		if not cmdline.opts.is_nofold:
			self.psrs = self.get_pulsars_to_fold(obs, cep2, cmdline, log)
			# to be sure that we have unique list of pulsars (especially relevant for tabfind+ option)
			self.psrs = np.unique(self.psrs)

	# function to set outdir and curdir directories
	def set_outdir(self, obs, cep2, cmdline):
		if len(self.tab.location) > 1 or len(self.tab.location) == 0:  # the last condition is when raw data are erased but still want to run only summaries
			if self.tab.is_coherent: locus=cep2.hoover_nodes[0] # we choose one hoover node for CS data
			else: locus=cep2.hoover_nodes[0]                    # and another for IS data
#			else: locus=cep2.hoover_nodes[1]                    # and another for IS data
		else:
			locus=self.tab.location[0]
		# if user specified output dir (relative to /data/LOFAR_PULSAR_....)
		if cmdline.opts.outdir != "":
			self.outdir = "%s_%s/%s" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir)
		else: # if output dir was not set
			self.outdir = "%s_%s/%s%s" % (cep2.processed_dir_prefix, locus, obs.id, self.outdir_suffix)
		self.curdir = "%s/%s/SAP%d/%s" % (self.outdir, self.beams_root_dir, self.sapid, self.procdir)


	# function to get the list of pulsars to fold for this TAB (unit)
	def get_pulsars_to_fold(self, obs, cep2, cmdline, log):
		try:
			# get pulsar name from the parset
			# if pulsar is not given in the command line, I also have to find pulsar if parset entry is empty
			if len(cmdline.psrs) == 0 or cmdline.psrs[0] == "parset":
				for sap in obs.saps:
					if self.sapid == sap.sapid and sap.source != "" and check_pulsars(sap.source, cmdline, cep2, None):
						self.psrs.append(sap.source)

			# if --pulsar is not given and source field in parset is empty
			if len(cmdline.psrs) == 0 and len(self.psrs) == 0:
				for sap in obs.saps:
					if self.sapid == sap.sapid:
						self.psrs[:] = sap.psrs
						break
				if len(self.psrs)>0: self.psrs = self.psrs[:1]  # leave only one pulsar
	
			# if special word "tabfind" or "tabfind+" is given
			# if it is "tabfind+", then we take pulsar from the parset (if exist), then one pulsar from the SAP (if different from the parset)
			# and then also search for another pulsar in the TAB
			# In case of "tabfind" we only search for pulsars in the TAB
			if len(cmdline.psrs) != 0 and (cmdline.psrs[0] == "tabfind" or cmdline.psrs[0] == "tabfind+"):
				# in the special case of "tabfind+"...
				if cmdline.psrs[0] == "tabfind+":
					for sap in obs.saps:
						if self.sapid == sap.sapid: 
							if sap.source != "" and check_pulsars(sap.source, cmdline, cep2, None):
								self.psrs.append(sap.source)
							if len(sap.psrs) > 0: self.psrs.append(sap.psrs[0])
				log.info("Searching for best pulsar for folding in SAP=%d TAB=%d..." % (self.sapid, self.tabid))
				tabpsrs = find_pulsars(self.tab.rarad, self.tab.decrad, cmdline, cmdline.opts.fwhm_CS/2.)
				if len(tabpsrs) > 0: 
					self.psrs.append(tabpsrs[0]) # use only one pulsar from those found in a TAB
					log.info("%s" % (tabpsrs[0]))

			# using pulsars from SAP
			if len(cmdline.psrs) != 0 and (cmdline.psrs[0] == "sapfind" or cmdline.psrs[0] == "sapfind3"):
				for sap in obs.saps:
					if self.sapid == sap.sapid:
						self.psrs[:] = sap.psrs
						break
				if cmdline.psrs[0] == "sapfind" and len(self.psrs)>0: self.psrs = self.psrs[:1]  # leave only one pulsar

			# if --pulsar is used but no special word
			if len(cmdline.psrs) != 0 and cmdline.psrs[0] != "parset" and cmdline.psrs[0] != "tabfind" and \
					cmdline.psrs[0] != "sapfind" and cmdline.psrs[0] != "sapfind3" and cmdline.psrs[0] != "tabfind+":
				self.psrs[:] = cmdline.psrs # copying all items

			# checking if pulsars are in ATNF catalog, or if not par-files do exist fo them, if not - exit
			for psr in self.psrs:
				if not check_pulsars(psr, cmdline, cep2, log):
					log.info("*** No parfile found for pulsar %s for SAP=%d TAB=%d. Exiting..." % (psr, self.sapid, self.tabid))
					sys.exit(1)

			# if pulsar list is still empty, and we did not set --nofold then set is_nofold flag to True
			if len(self.psrs) == 0:
				log.warning("*** No pulsar found to fold for SAP=%d TAB=%d. Setting --nofold flag for this beam..." % (self.sapid, self.tabid))
			
			return self.psrs

		except Exception:
			self.log.exception("Oops... get_pulsars_to_fold has crashed!")
			self.kill()  # killing all open processes
			sys.exit(1)

	# getting proper parfile in the processing directory
	def get_parfile(self, cmdline, cep2):
		for psr in self.psrs:
			psr2=re.sub(r'[BJ]', '', psr)
			if cmdline.opts.parfile != "":
				if os.path.exists(cmdline.opts.parfile): 
					self.log.info("Copying user-specified parfile '%s' to %s/%s.par" % \
						(cmdline.opts.parfile, self.outdir, psr2))
					cmd="cp -n %s %s/%s.par" % (cmdline.opts.parfile, self.outdir, psr2)
					self.execute(cmd)
					continue
				else: 
					self.log.error("Can't find user-specified parfile '%s'. Exiting..." % (cmdline.opts.parfile))
					self.kill()
					sys.exit(1)
			parfile="%s/%s.par" % (cep2.parfile_dir, psr2)
			if os.path.exists(parfile):
				cmd="cp -n %s %s" % (parfile, self.outdir)
				self.execute(cmd)
				continue
			parfile="%s/%s.par" % (cep2.parfile_dir, psr)
			if os.path.exists(parfile):
				cmd="cp -n %s %s/%s.par" % (parfile, self.outdir, psr2)
				self.execute(cmd)
				continue
			self.log.info("Parfile does not exist. Creating parfile base on pulsar ephemeris from ATNF catalog...")
			# for -e option, we need to use pulsar name with leading B or J, otherwise it is possible (I cama across this!)
			# that there are two pulsars with the same name, one with leading J, another with leading B,
			# in this case psrcat returns records for both pulsars, and output parfile gets messed up
			cmd="psrcat -db_file %s -e %s > %s/%s.par" % (cep2.psrcatdb, psr, self.outdir, psr2)
			self.execute(cmd, is_os=True)

		# Now we check the par-files if they have non-appropriate flags that can cause prepfold to crash
		for psr in self.psrs:
			psr2=re.sub(r'[BJ]', '', psr)
			parfile="%s/%s.par" % (self.outdir, psr2)
			# check first for PSRB name. It should be changed to PSRJ
			cmd="grep 'PSRB' %s" % (parfile,)
                	status=os.popen(cmd).readlines()
			if np.size(status)>0:
				self.log.warning("WARNING: Par-file %s has PSRB keyword that can cause prepfold to crash!\n\
If your pipeline run calls prepfold you might need to change PSRB to PSRJ." % (parfile,))
			cmd="grep 'CLK' %s" % (parfile,)
                	status=os.popen(cmd).readlines()
			if np.size(status)>0:
				self.log.warning("WARNING: Par-file %s has CLK keyword that can cause prepfold to crash!\n\
CLK line will be removed from the parfile!" % (parfile,))
				cmd="sed -i '/^CLK/d' %s" % (parfile,)
				self.execute(cmd, self.log, is_os=True)


	def execute(self, cmd, workdir=None, shell=False, is_os=False):
	    	"""
        	Execute the command 'cmd' after logging the command
            	and the wall-clock amount of time the command took to execute.
		This function waits for process to finish
    		"""
		try:
			self.log.info(re.sub("\n", "\\\\n", cmd))  # also escaping \n to show it as it is
    			job_start = time.time()
			self.log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
			status = 1
			if is_os: status = os.system(cmd)
			else:
               			process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
				self.procs.append(process)
       	        		self.log.process2log(process)
        	       		process.communicate()
				status=process.poll()
				self.procs.remove(process)
			job_end = time.time()
			job_total_time = job_end - job_start
        	       	self.log.info("Finished at UTC %s, status=%s, Total runnung time: %.1f s (%.2f hrs)" % \
					(time.asctime(time.gmtime()), status, job_total_time, job_total_time/3600.))
			self.log.info("")
			# if job is not successful
			if status != 0:
				raise Exception
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (re.sub("\n", "\\\\n", cmd), status))
			raise Exception

	def start_and_go(self, cmd, workdir=None, shell=False, immediate_status_check=False):
	    	"""
        	Execute the command 'cmd' after logging the command
		This function start the cmd and leaves the function
		returning the Popen object, it does not wait for process to finish
    		"""
		status=1
		try:
			self.log.info(re.sub("\n", "\\\\n", cmd))
			self.log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
			if immediate_status_check:
               			process = Popen(shlex.split(cmd), cwd=workdir, shell=shell)
				time.sleep(5)  # waiting 5 secs to see if process crashes right away
				if process.poll() != 0: raise Exception
				else: process.kill()  # if process is still running, it means that cmd is good, so we kill it in order to
							# restart it with proper stdout/stderr and add it to the list
               		process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
			status=process.returncode
			self.procs.append(process)
			self.log.info("Job pid=%d, not waiting for it to finish." % (process.pid))
			return process
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (re.sub("\n", "\\\\n", cmd), status))
			raise Exception

	def waiting(self, prg, popen):
		"""
		Waiting for process to finish
		"""
		try:
			job_start = time.time()
			self.log.info("Waiting for %s to finish, pid=%d" % (prg, popen.pid))
			(sout, serr) = popen.communicate()
			# we pipe serr to sout, so no need to log serr
#			self.log.stdout2log(sout)
			self.log.info(sout)
			job_end = time.time()
			job_total_time = job_end - job_start
			self.log.info("Process pid=%d (%s) has finished at UTC %s, status=%d, Waiting time: %.1f s (%.2f hrs)" % \
				(popen.pid, prg, time.asctime(time.gmtime()), popen.returncode, job_total_time, job_total_time/3600.))
			self.log.info("")
			self.procs.remove(popen)
			# if job is not successful
			if popen.poll() != 0:
				raise Exception
		except Exception:
			self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, popen.pid, popen.returncode))
			raise Exception

	def waiting_list(self, prg, popen_list):
		"""
		Waiting for list of processes to finish
		"""
		try:
			job_start = time.time()
			self.log.info("Waiting for %s processes to finish..." % (prg))
			run_units = [u.pid for u in popen_list if u.poll() is None]
			self.log.info("Still running [%d]: %s" % (len(run_units), run_units))
			for unit in popen_list:
				self.waiting(prg, unit)
				run_units = [u.pid for u in popen_list if u.poll() is None]
				finished_units = [u for u in popen_list if u.poll() is not None]
				for fu in finished_units:
					if fu.returncode != 0:
						self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, fu.pid, fu.returncode))
				if len(run_units) > 0: self.log.info("Still running [%d]: %s" % (len(run_units), run_units))
			job_end = time.time()
			job_total_time = job_end - job_start
			self.log.info("Processes of %s have finished at UTC %s, Waiting time: %.1f s (%.2f hrs)" % \
				(prg, time.asctime(time.gmtime()), job_total_time, job_total_time/3600.))
			self.log.info("")
		except Exception:
			self.log.exception("Oops... %s has crashed!\npids = %s" % (prg, ",".join(["%d" % (fu.pid) for fu in popen_list if fu.poll() is not None])))
			raise Exception

	def power_of_two(self, value):
		"""
		Returns the closest power of two value to the input value (from the high side)
		"""
		return int(math.pow(2, math.ceil(math.log(value)/math.log(2))))

	def get_best_nbins(self, parf):
		"""
		Calculates the best number of bins for folding based on the period value from the parfile (parf)
		and sampling interval (tsamp)
		"""
		try:
			ephem=parfile.psr_par(parf) # there should not be empty lines in the parfile, otherwise psr_par crashes
			nbins=self.power_of_two(int(math.ceil(ephem.P0*1000.0/self.sampling)))
			if nbins > 1024: return 1024
			else: return nbins
		except: return 256

	def get_psr_dm(self, parf):
		"""
		Reads parfile and returns pulsar DM
		"""
		dm=0
		try:
			ephem=parfile.psr_par(parf)
			dm=ephem.DM
		except: pass
		return dm

	def lcd(self, low, high):
		"""
		Calculates the lowest common denominator of 'high' value between 'low' and 'high'
		"""
		for ii in range(low, high+1): 
			if high % ii == 0: return ii
		return high

	def hcd(self, low, high, value):
		"""
		Calculates the highest common denominator of 'value' value between 'low' and 'high'
		"""
		for ii in range(high, low-1, -1): 
			if value % ii == 0: return ii
		return 1

	# function that checks all processes in the list and kill them if they are still running
	def kill(self):
		if self.log != None: self.log.info("Killing all open processes for SAP=%d TAB=%d..." % (self.sapid, self.tabid))
		for proc in self.procs:
			if proc.poll() is None: # process is still running
				proc.kill()
				if proc != None: proc.communicate()
				if proc != None: proc.poll()
		self.procs = []

	# function that does last steps in processing, creating tarball, copyting it, changing ownership, etc...
	def finish_off(self, obs, cep2, cmdline):

		# tarball name
		tarname="%s_sap%03d_tab%04d%s" % (obs.id, self.sapid, self.tabid, self.archive_suffix)
		# variables for rsync'ing the data
		verbose=""
		if cmdline.opts.is_debug: verbose="-v"
		output_dir="%s_%s/%s%s" % \
			(cep2.processed_dir_prefix, self.summary_node, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, self.summary_node_dir_suffix)
		output_archive="%s/%s" % (output_dir, tarname)

		if not cmdline.opts.is_feedback:
			# copying parset file to output directory
			self.log.info("Copying original parset file to output directory...")
			cmd="cp -n %s %s" % (obs.parset, self.outdir)
			self.execute(cmd, workdir=self.outdir)
			# Make a tarball of all the plots for this beam
			self.log.info("Making a tarball of all the files with extensions: %s" % (", ".join(self.extensions)))
			tar_list=[]
			for ext in self.extensions:
				ext_list=rglob(self.curdir, ext, 3)
				tar_list.extend(ext_list)
			tar_list.extend(glob.glob("%s/*.par" % (self.outdir)))
			tar_list.extend(glob.glob("%s/*.parset" % (self.outdir)))
			cmd="tar -cvz --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(self.outdir+"/")[1] for f in tar_list]))
			try: # --ignore-failed-read does not seem to help with tar failing for some beams
        	             # like file was changed during the tar, though tarball seem to be fine
				self.execute(cmd, workdir=self.outdir)
			except: pass

			# copying archive file to summary node
			self.log.info("Copying archive file to %s:%s" % (self.summary_node, output_dir))
			cmd="rsync %s -axP %s %s:%s" % (verbose, tarname, self.summary_node, output_archive)
			self.execute(cmd, workdir=self.outdir)

		# finish
		self.end_time=time.time()
		self.total_time=self.end_time - self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

		# flushing log file and copy it to outdir on local node and summary node
		self.log.flush()
		if not cmdline.opts.is_log_append: cmd="cp -f %s %s" % (cep2.get_logfile(), self.outdir)
		else: cmd="cat %s >> %s/%s" % (cep2.get_logfile(), self.outdir, cep2.get_logfile().split("/")[-1])
		os.system(cmd)
		cmd="rsync %s -axP %s %s:%s" % (verbose, cep2.get_logfile(), self.summary_node, output_dir)
		proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=self.outdir)
		proc.communicate()

		# changing the file permissions to be re-writable for group
		cmd="chmod -R g+w %s" % (self.outdir)
		os.system(cmd)

		if not cmdline.opts.is_feedback:
			# create a new full archive tarball
			tar_list=[]
			for ext in self.full_archive_exts:
				ext_list=rglob(self.curdir, ext, 3)
				tar_list.extend(ext_list)
			tar_list.extend(glob.glob("%s/*.par" % (self.outdir)))
			tar_list.extend(glob.glob("%s/*.parset" % (self.outdir)))
			tar_list.extend(glob.glob("%s/%s" % (self.outdir, cep2.get_logfile().split("/")[-1])))
			cmd="tar -cvz --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(self.outdir+"/")[1] for f in tar_list]))
			try:
				# --ignore-failed-read does not seem to help with tar failing for some beams
				# like file was changed during the tar, though tarball seem to be fine
				proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=self.outdir)
				proc.communicate()
			except: pass

		# initializing the Feedback unit
		fbindex=0
		for ss in range(self.sapid): 
			for sap in obs.saps:
				if sap.sapid == ss:
					fbindex += sap.nrTiedArrayBeams
					break
		fbindex += self.tabid
		fbunit = FeedbackUnit(fbindex, cep2.current_node, self.outdir, obs.id, self.sapid, self.tabid)
		fbunit.update("%s/%s" % (self.outdir, tarname), self.code)
		fbunit.flush("%s/.%s_sap%03d_beam%04d.fb" % (cep2.get_logdir(), obs.id, self.sapid, self.tabid), cep2, False)

	# main processing function
	def run(self, obs, cep2, cmdline, log):
		# if there are no pulsars to fold we set --nofold option to True
		if len(self.psrs) == 0 and cmdline.opts.is_nofold == False:
			cmdline.opts.is_nofold = True
		# if we are not using dspsr directly to read *.h5 files
		if not cmdline.opts.is_with_dal:
			self.run_nodal(obs, cep2, cmdline, log)
		else:
			self.run_dal(obs, cep2, cmdline, log)

	# main run function to use 2bf2fits for conversion
	def run_nodal(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# Re-creating root output directory
			cmd="mkdir -m 775 -p %s" % (self.outdir)
			self.execute(cmd)

			# creating Par-file in the output directory or copying existing one
			if not cmdline.opts.is_nofold: 
				self.get_parfile(cmdline, cep2)

			# Creating curdir dir
			cmd="mkdir -m 775 -p %s" % (self.curdir)
			self.execute(cmd)

			# if we run the whole processing and not just plots
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_nodecode and not cmdline.opts.is_feedback:
				if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
        		        	                          # also we need to moint locus nodes that we need
					self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
					input_files=[]
					for loc in self.tab.location:
						if loc in self.tab.rawfiles:
							# first "mounting" corresponding locus node
							uniqdir="/".join(self.tab.rawfiles[loc][0].split("/")[0:-1]).split("/data/")[-1]
							input_dir="%s/%s_data/%s" % (cep2.hoover_data_dir, loc, uniqdir)
							process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
							process.communicate()
#							input_file="%s/%s_SAP%03d_B%03d_S*_bf.raw" % (input_dir, obs.id, self.sapid, self.tabid)
#							input_files.extend(glob.glob(input_file))
							input_file=["%s/%s" % (input_dir, f.split("/" + obs.id + "/")[-1]) for f in self.tab.rawfiles[loc]]
							input_files.extend(input_file)
							# copy *.h5 files (want to keep them)
							for f in self.tab.rawfiles[loc]:
								cmd="cp -f %s/%s.h5 ." % (input_dir, f.split("/" + obs.id + "/")[-1].split(".raw")[0])
								self.execute(cmd, workdir=self.curdir)
					input_file=" ".join(input_files)
				else:
					if cep2.current_node in self.tab.rawfiles:
						input_file=" ".join(self.tab.rawfiles[cep2.current_node])
						# copy *.h5 files
						for f in self.tab.rawfiles[cep2.current_node]:
							cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
							self.execute(cmd, workdir=self.curdir)
					else: input_file=""
#					input_file=glob.glob("%s/%s/%s_SAP%03d_B%03d_S*_bf.raw" % (cep2.rawdir, obs.id, obs.id, self.sapid, self.tabid))[0]

				self.log.info("Input data: %s" % (input_file))

			self.output_prefix="%s_SAP%d_%s" % (obs.id, self.sapid, self.procdir)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			total_chan = self.tab.nrSubbands*self.nrChanPerSub

			# if we run the whole processing and not just plots
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:

				# running data conversion (2bf2fits)
				if not cmdline.opts.is_nodecode:
					verbose=""
					if cmdline.opts.is_debug: verbose="-v"
					cmd="2bf2fits %s %s -parset %s -append -nbits 8 -A 100 -sigma 3 -nsubs %d -o %s %s" % (verbose, self.raw2fits_extra_options, obs.parset, self.tab.nrSubbands, self.output_prefix, input_file)
					self.execute(cmd, workdir=self.curdir)

				# running RFI excision, checking
				if not cmdline.opts.is_norfi:
					zapstr=""
					# we should zap 1st chan as after 2nd polyphase it has junk
					if self.nrChanPerSub > 1:
						zapstr="-zapchan 0:%d:%d" % (total_chan-1, self.nrChanPerSub)
					self.log.info("Creating RFI mask...")
					cmd="rfifind -o %s -psrfits -noclip -blocks 16 %s %s.fits" % (self.output_prefix, zapstr, self.output_prefix)
					rfifind_popen = self.start_and_go(cmd, workdir=self.curdir)
					if not cmdline.opts.is_skip_subdyn:
						self.log.info("Producing RFI report...")
						samples_to_average=int(10000. / self.sampling) # 10 s worth of data
						cmd="python %s/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n %d %s.fits" % (cep2.lofarsoft, samples_to_average, self.output_prefix)
						subdyn_popen = self.start_and_go(cmd, workdir=self.curdir)

					# waiting for rfifind to finish
					self.waiting("rfifind", rfifind_popen)

				if not cmdline.opts.is_nofold:	
					# running prepfold with and without mask
					if not cmdline.opts.is_skip_prepfold:
						if total_chan >= 256 and total_chan <= 6000:
							prepfold_nsubs = 256
						elif total_chan > 6000:
							prepfold_nsubs = 512
						else: prepfold_nsubs = total_chan
						self.log.info("Getting proper value of nsubs in prepfold between %d and %d..." % (prepfold_nsubs, total_chan))
						# calculating the least common denominator of total_chan starting from prepfold_nsubs
						prepfold_nsubs = self.lcd(prepfold_nsubs, total_chan)
						self.log.info("Number of subbands, -nsubs, for prepfold is %d" % (prepfold_nsubs))
						prepfold_popens=[]  # list of prepfold Popen objects
						for psr in self.psrs:   # pulsar list is empty if --nofold is used
							psr2=re.sub(r'[BJ]', '', psr)
							prepfold_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
							# first running prepfold with mask (if --norfi was not set)
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)):
								# we use ../../../ instead of self.outdir for the full-name of the parfile, because in this case prepfold crashes
								# I suppose it happens because name of the file is TOO long for Tempo
								cmd="prepfold -noscales -nooffsets -noxwin -psr %s -par ../../../%s.par -n %d -nsub %d -fine -nopdsearch -mask %s_rfifind.mask -o %s_%s %s %s.fits" % \
									(psr, psr2, prepfold_nbins, prepfold_nsubs, self.output_prefix, psr, self.output_prefix, cmdline.opts.prepfold_extra_opts, self.output_prefix)
								try: # if prepfold fails right away (sometimes happens with error like this:
									# Read 0 set(s) of polycos for PSR 1022+1001 at 56282.138888888891 (DM = 3.6186e-317)
									# MJD 56282.139 out of range (    0.000 to     0.000)
									# isets = 0
									# I guess something to do with how Tempo treats parfile. When this happens, we try to rerun prepfold with the same
									# command but without using -par option
									prepfold_popen = self.start_and_go(cmd, workdir=self.curdir, immediate_status_check=True)
								except Exception:
									self.log.warning("***** Prepfold failed when using par-file. Will try the same command but without using -par option *****")
									cmd="prepfold -noscales -nooffsets -noxwin -psr %s -n %d -nsub %d -fine -nopdsearch -mask %s_rfifind.mask -o %s_%s %s %s.fits" % \
										(psr, prepfold_nbins, prepfold_nsubs, self.output_prefix, psr, self.output_prefix, cmdline.opts.prepfold_extra_opts, self.output_prefix)
									prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
								prepfold_popens.append(prepfold_popen)
								time.sleep(5) # will sleep for 5 secs, in order to give prepfold enough time to finish 
                                                                              # with temporary files lile resid2.tmp otherwise it can interfere with next prepfold call
							# running prepfold without mask
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)): 
								output_stem="_nomask"
							else: output_stem=""
							# we use ../../../ instead of self.outdir for the full-name of the parfile, because in this case prepfold crashes
							# I suppose it happens because name of the file is TOO long for Tempo
							cmd="prepfold -noscales -nooffsets -noxwin -psr %s -par ../../../%s.par -n %d -nsub %d -fine -nopdsearch -o %s_%s%s %s %s.fits" % \
								(psr, psr2, prepfold_nbins, prepfold_nsubs, psr, self.output_prefix, output_stem, cmdline.opts.prepfold_extra_opts, self.output_prefix)
							try: # same reasoning as above
								prepfold_popen = self.start_and_go(cmd, workdir=self.curdir, immediate_status_check=True)
							except Exception:
								self.log.warning("***** Prepfold failed when using par-file. Will try the same command but without using -par option *****")
								cmd="prepfold -noscales -nooffsets -noxwin -psr %s -n %d -nsub %d -fine -nopdsearch -o %s_%s%s %s %s.fits" % \
									(psr, prepfold_nbins, prepfold_nsubs, psr, self.output_prefix, output_stem, cmdline.opts.prepfold_extra_opts, self.output_prefix)
								prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
							prepfold_popens.append(prepfold_popen)
							time.sleep(5) # again will sleep for 5 secs, in order to give prepfold enough time to finish 
                                                                      # with temporary files like resid2.tmp otherwise it can interfere with next prepfold call

					# now running dspsr stuff...
					if not cmdline.opts.is_skip_dspsr:
						zapstr=""
						if self.nrChanPerSub > 1:
							zapstr="-j 'zap chan %s'" % (",".join([str(ii) for ii in range(0, total_chan, self.nrChanPerSub)]))
						verbose="-q"
						if cmdline.opts.is_debug: verbose="-v"
						dspsr_popens=[] # list of dspsr Popen objects
						for psr in self.psrs: # pulsar list is empty if --nofold is used
							psr2=re.sub(r'[BJ]', '', psr)
							dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
							cmd="dspsr -b %d -E %s/%s.par %s %s -fft-bench -O %s_%s -K -A -L 60 -t %d %s %s.fits" % \
								(dspsr_nbins, self.outdir, psr2, zapstr, verbose, psr, self.output_prefix, cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, self.output_prefix)
							dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
							dspsr_popens.append(dspsr_popen)

						# waiting for dspsr to finish
						self.waiting_list("dspsr", dspsr_popens)

						# zapping channels...
						if not cmdline.opts.is_norfi:
							self.log.info("Zapping channels using median smoothed difference algorithm...")
							for psr in self.psrs:  # pulsar list is empty if --nofold is used
								cmd="paz -r -e paz.ar %s_%s.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)

						# scrunching in frequency
						self.log.info("Scrunching in frequency to have %d channels in the output ar-file..." % (self.tab.nrSubbands))
						if self.nrChanPerSub > 1:
							for psr in self.psrs:  # pulsar list is empty if --nofold is used
								# first, running fscrunch on zapped archive
								if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
									cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (self.tab.nrSubbands, psr, self.output_prefix)
									self.execute(cmd, workdir=self.curdir)
									# remove non-scrunched zapped archive (we will always have unzapped non-scrunched version)
									cmd="rm -f %s_%s.paz.ar" % (psr, self.output_prefix)
									self.execute(cmd, workdir=self.curdir)
								# running fscrunching on non-zapped archive
								cmd="pam --setnchn %d -e fscr.AR %s_%s.ar" % (self.tab.nrSubbands, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
						else: # if number of chans == number of subs, we will just rename *.ar-file to *.fscr.AR and make a link to it for original *.ar-file
							for psr in self.psrs:  # pulsar list is empty if --nofold is used
								if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
									cmd="mv -f %s_%s.paz.ar %s_%s.paz.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
									self.execute(cmd, workdir=self.curdir)
								cmd="mv -f %s_%s.ar %s_%s.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
								cmd="ln -sf %s_%s.fscr.AR %s_%s.ar" % (psr, self.output_prefix, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)

			# running extra Psrchive programs, pam, pav,pdmp, etc... 
			# these programs should be run quick, so run them one by one

			if not cmdline.opts.is_skip_dspsr and not cmdline.opts.is_feedback:
				# first, calculating the proper max divisor for the number of subbands
#				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (self.nrChanPerSub, self.tab.nrSubbands))
				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(self.tab.nrSubbands, 63)))
				# calculating the greatest common denominator of self.tab.nrSubbands starting from 63 down
				pav_nchans = self.hcd(1, min(self.tab.nrSubbands, 63), self.tab.nrSubbands)
				for psr in self.psrs:  # pulsar list is empty if --nofold is used
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
						output_stem=".paz.fscr.AR"
					else: output_stem=".fscr.AR"
					# creating DSPSR diagnostic plots
					cmd="pav -DFTp -g %s_%s_DFTp.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -GTpf%d -g %s_%s_GTpf%d.ps/cps %s_%s%s" % (pav_nchans, psr, self.output_prefix, pav_nchans, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -YFp -g %s_%s_YFp.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -J -g %s_%s_J.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="convert \( %s_%s_GTpf%d.ps %s_%s_J.ps +append \) \( %s_%s_DFTp.ps %s_%s_YFp.ps +append \) \
                	                     -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
                        	             (psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
                                	      psr, self.output_prefix, psr, self.output_prefix)
					self.execute(cmd, workdir=self.curdir)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_skip_dspsr:
					# now running pdmp without waiting...
					if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
						self.log.info("Running pdmp...")
						pdmp_popens=[]  # list of pdmp Popen objects	
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
								output_stem=".paz.fscr.AR"
							else: output_stem=".fscr.AR"
							# getting the number of bins in the ar-file (it can be different from self.get_best_nbins, because
							# we still provide our own number of bins in --dspsr-extra-opts
							try:
								cmd="psredit -q -Q -c nbin %s/%s_%s%s" % (self.curdir, psr, self.output_prefix, output_stem)
								binsline=os.popen(cmd).readlines()
								if np.size(binsline) > 0:
									best_nbins=int(binsline[0][:-1])
									cmd="pdmp -mc %d -mb %d -g %s_%s_pdmp.ps/cps %s_%s%s" % \
										(self.tab.nrSubbands, min(128, best_nbins), psr, self.output_prefix, psr, self.output_prefix, output_stem)
									pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
									pdmp_popens.append(pdmp_popen)
							except Exception: pass
		
				# waiting for prepfold to finish
				try:
					if not cmdline.opts.is_nofold and not cmdline.opts.is_skip_prepfold: self.waiting_list("prepfold", prepfold_popens)
				# we let PULP to continue if prepfold has crashed, as dspsr can run ok, or other instances of prepfold could finish ok as well
				except: pass

			# if we want to run prepdata to create a time-series and make a list of TOAs
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and cmdline.opts.is_single_pulse:	
				self.log.info("Running single-pulse analysis...")
				prepdata_popens=[]  # list of prepdata Popen objects
				for psr in self.psrs:   # pulsar list is empty if --nofold is used
					psr2=re.sub(r'[BJ]', '', psr)
					psrdm=self.get_psr_dm("%s/%s.par" % (self.outdir, psr2))
					# first running prepdata with mask (if --norfi was not set)
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)):
						cmd="prepdata -noscales -nooffsets -noclip -nobary -dm %f -mask %s_rfifind.mask -o %s_%s %s.fits" % \
							(psrdm, self.output_prefix, psr, self.output_prefix, self.output_prefix)
						prepdata_popen = self.start_and_go(cmd, workdir=self.curdir)
						prepdata_popens.append(prepdata_popen)

					# running prepdata without mask
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)): 
						output_stem="_nomask"
					else: output_stem=""
					cmd="prepdata -noscales -nooffsets -noclip -nobary -dm %f -o %s_%s%s %s.fits" % \
						(psrdm, psr, self.output_prefix, output_stem, self.output_prefix)
					prepdata_popen = self.start_and_go(cmd, workdir=self.curdir)
					prepdata_popens.append(prepdata_popen)

			# waiting for prepdata to finish
			try:
				if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and cmdline.opts.is_single_pulse:
					self.waiting_list("prepdata", prepdata_popens)
					# after all instances of prepdata are finished we run single_pulse_search.py on created .dat files
					singlepulse_popens=[]  # list of single_pulse_search.py Popen objects
					for psr in self.psrs:   # pulsar list is empty if --nofold is used
						psr2=re.sub(r'[BJ]', '', psr)
						# first running single_pulse_search.py on .dat file created with the rfifind mask
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)):
							cmd="single_pulse_search.py -p %s_%s.dat" % (psr, self.output_prefix)
							singlepulse_popen = self.start_and_go(cmd, workdir=self.curdir)
							singlepulse_popens.append(singlepulse_popen)

						# running single_pulse_search.py on .dat file created without rfifind mask
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_rfifind.mask" % (self.curdir, self.output_prefix)): 
							output_stem="_nomask"
						else: output_stem=""
						cmd="single_pulse_search.py -p %s_%s%s.dat" % (psr, self.output_prefix, output_stem)
						singlepulse_popen = self.start_and_go(cmd, workdir=self.curdir)
						singlepulse_popens.append(singlepulse_popen)
					# now waiting for all instances of single_pulse_search.py to finish
					self.waiting_list("single_pulse_search.py", singlepulse_popens)
			# we let PULP to continue if prepdata (or single_pulse_search.py) has crashed, as the rest of the pipeline can finish ok
			except: pass

			# running convert on prepfold ps to pdf and png
			if not cmdline.opts.is_nofold and not cmdline.opts.is_feedback:
				self.log.info("Running convert on ps to pdf and png of the plots...")
				prepfold_ps=glob.glob("%s/*.pfd.ps" % (self.curdir))
				for psfile in prepfold_ps:
					base=psfile.split("/")[-1].split(".ps")[0]
					cmd="convert %s.ps %s.pdf" % (base, base)
					# have separate try/except blocks for each convert command to allow to continue processing
					# in case something is wrong with ps-file
					try:
						self.execute(cmd, workdir=self.curdir)
					except: pass
					cmd="convert -rotate 90 %s.ps %s.png" % (base, base)
					try:
						self.execute(cmd, workdir=self.curdir)
					except : pass
					cmd="convert -rotate 90 -crop 200x140-0 %s.ps %s.th.png" % (base, base)
					try:
						self.execute(cmd, workdir=self.curdir)
					except: pass

			# getting the list of *.pfd.bestprof files and read chi-sq values for all folded pulsars
			if not cmdline.opts.is_nofold and not cmdline.opts.is_feedback:
				psr_bestprofs=rglob(self.outdir, "*.pfd.bestprof", 3)
				if len(psr_bestprofs) > 0:
					self.log.info("Reading chi-squared values and adding to chi-squared.txt...")
					# also preparing montage command to create combined plot
					montage_cmd="montage -background none -pointsize 10.2 "
					chif=open("%s/%s_sap%03d_tab%04d_chi-squared.txt" % (self.outdir, obs.id, self.sapid, self.tabid), 'w')
					thumbs=[] # list of thumbnail files
					# check first if all available *bestprof files are those created without mask. If so, then allow to make
					# a diagnostic combined plot using prepfold plots without a mask
					good_bestprofs=[file for file in psr_bestprofs if re.search("_nomask_", file) is None]
					if len(good_bestprofs) == 0:
						good_bestprofs=[file for file in psr_bestprofs]
					for bp in good_bestprofs:
						psr=bp.split("/")[-1].split("_")[0]
						thumbfile=bp.split(self.outdir+"/")[-1].split(".pfd.bestprof")[0] + ".pfd.th.png"
						# getting current number for SAP and TA beam
						try: # we need this try block in case User manually creates sub-directories with some test bestprof files there
						     # which will also be found by rglob function. So, we need to exclude them by catching an Exception
						     # on a wrong-formed string applying int()
							cursapid=int(thumbfile.split("_SAP")[-1].split("_")[0])
						except: continue
						curprocdir=thumbfile.split("_SAP")[-1].split("_")[1]
						chi_val = 0.0
						thumbs.append(thumbfile)
						cmd="cat %s | grep chi-sqr | cut -d = -f 2" % (bp)
						chiline=os.popen(cmd).readlines()
						if np.size(chiline) > 0:
							chi_val = float(chiline[0].rstrip())
						else:
							if self.sapid == cursapid and self.procdir == curprocdir:
								self.log.warning("Warning: can't read chi-sq value from %s" % (bp))
						chif.write("file=%s obs=%s_SAP%d_%s_%s chi-sq=%g\n" % (thumbfile, self.code, cursapid, curprocdir, psr, chi_val))
						montage_cmd += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (self.code, cursapid, curprocdir, psr, chi_val, thumbfile)
					chif.close()
					cmd="mv %s_sap%03d_tab%04d_chi-squared.txt chi-squared.txt" % (obs.id, self.sapid, self.tabid)
					self.execute(cmd, workdir=self.outdir)

					# creating combined plots
					# only creating combined plots when ALL corresponding thumbnail files exist. It is possible, when there are 2+ beams on
					# the same node, that bestprof files do exist, but thumbnails were not created yet at the time when chi-squared.txt is
					# getting created for another beam. And this will cause "montage" command to fail
					# At the end combined plot will eventually be created for this node during the procesing of the last beam of this node
					if len(thumbs) > 0 and len([ff for ff in thumbs if os.path.exists(ff)]) == len(thumbs):
						# creating combined plots
						self.log.info("Combining all pfd.th.png files in a single combined plot...")
						montage_cmd += "combined.png"
						self.execute(montage_cmd, workdir=self.outdir)
						# making thumbnail version of the combined plot
						cmd="convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.png combined.th.png"
						self.execute(cmd, workdir=self.outdir)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and not cmdline.opts.is_norfi and not cmdline.opts.is_skip_subdyn:
				# waiting for subdyn to finish
				self.waiting("subdyn.py", subdyn_popen)

			# waiting for pdmp to finish
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_skip_dspsr:
					if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold: 
						self.waiting_list("pdmp", pdmp_popens)
						# when pdmp is finished do extra actions with files...
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
								output_stem=".paz.fscr.AR"
							else: output_stem=".fscr.AR"
							cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
							self.execute(cmd, is_os=True)
							cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
							self.execute(cmd, is_os=True)
							# reading new DM from the *.per file
							newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
							if np.size(newdm) > 1: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm[-1], psr, self.output_prefix, output_stem)
							else: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)

			# finishing off the processing...
			self.finish_off(obs, cep2, cmdline)

		except Exception:
			self.log.exception("Oops... 'run' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None

	# main run function using dspsr to directly read *.h5 files
	def run_dal(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# Re-creating root output directory
			cmd="mkdir -m 775 -p %s" % (self.outdir)
			self.execute(cmd)

			# creating Par-file in the output directory or copying existing one
			if not cmdline.opts.is_nofold: 
				self.get_parfile(cmdline, cep2)

			# Creating curdir dir
			cmd="mkdir -m 775 -p %s" % (self.curdir)
			self.execute(cmd)

			# if not just making plots...
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and not cmdline.opts.is_nodecode:
				if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
        	        	                          # also we need to moint locus nodes that we need
					self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
					input_files=[]
					for loc in self.tab.location:
						if loc in self.tab.rawfiles:
							# first "mounting" corresponding locus node
							uniqdir="/".join(self.tab.rawfiles[loc][0].split("/")[0:-1]).split("/data/")[-1]
							input_dir="%s/%s_data/%s" % (cep2.hoover_data_dir, loc, uniqdir)
							process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
							process.communicate()
							# dspsr needs all polarizations S* files to be in the current directory together with h5 files,
							# so we have to make soft links to input files
							self.log.info("Making links to input files in the current directory...")
							for f in self.tab.rawfiles[loc]:
								# links to the *.raw files
								cmd="ln -sf %s/%s ." % (input_dir, f.split("/" + obs.id + "/")[-1])
								self.execute(cmd, workdir=self.curdir)
								# copy *.h5 files (want to keep them)
								cmd="cp -f %s/%s.h5 ." % (input_dir, f.split("/" + obs.id + "/")[-1].split(".raw")[0])
								self.execute(cmd, workdir=self.curdir)
							input_file=["%s.h5" % (f.split("/" + obs.id + "/")[-1]).split(".raw")[0] for f in self.tab.rawfiles[loc]]
							input_files.extend(input_file)
					input_file=" ".join(input_files)
				else:
					if cep2.current_node in self.tab.rawfiles:
						# make a soft links in the current dir (in order for processing to be consistent with the case when data are in many nodes)
						self.log.info("Making links to input files in the current directory...")
						for f in self.tab.rawfiles[cep2.current_node]:
							# links to the *.raw files
							cmd="ln -sf %s ." % (f)
							self.execute(cmd, workdir=self.curdir)
							# copy *.h5 files
							cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
							self.execute(cmd, workdir=self.curdir)
						input_files=["%s.h5" % (f.split("/" + obs.id + "/")[-1]).split(".raw")[0] for f in self.tab.rawfiles[cep2.current_node]]
						input_file=" ".join(input_files)
					else: input_file=""

				self.log.info("Input data: %s" % (input_file))

			self.output_prefix="%s_SAP%d_%s" % (obs.id, self.sapid, self.procdir)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			proc_subs = self.nrSubsPerFile * cmdline.opts.nsplits
			if cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs > self.tab.nrSubbands:
				proc_subs -= (cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs - self.tab.nrSubbands)  
			nsubs_eff = min(self.tab.nrSubbands, proc_subs)
			total_chan = nsubs_eff * self.nrChanPerSub

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				# getting the list of "_S0_" files, the number of which is how many freq splits we have
				# we also sort this list by split number
				s0_files=sorted([f for f in input_files if re.search("_S0_", f) is not None], key=lambda input_file: int(input_file.split("_P")[-1].split("_")[0]))
				if not cmdline.opts.is_nofold and not cmdline.opts.is_skip_dspsr:
					verbose="-q"
					if cmdline.opts.is_debug: verbose="-v"

					# running dspsr for every pulsar for all frequency splits
					self.log.info("Running dspsr for all frequency splits...")
					# loop on pulsars
					for psr in self.psrs: # pulsar list is empty if --nofold is used
						self.log.info("Running dspsr for pulsar %s..." % (psr))
						psr2=re.sub(r'[BJ]', '', psr)
						dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
						# loop on frequency splits
						for ii in range(len(s0_files)):
							fpart=int(s0_files[ii].split("_P")[-1].split("_")[0])
							zapstr=""
							if self.nrChanPerSub > 1:
								proc_subs = self.nrSubsPerFile
								if (fpart + 1)*self.nrSubsPerFile > self.tab.nrSubbands: 
									proc_subs = -fpart*self.nrSubsPerFile + self.tab.nrSubbands
								zapstr="-j 'zap chan %s'" % (",".join([str(kk) for kk in range(0, proc_subs * self.nrChanPerSub, self.nrChanPerSub)]))
							cmd="dspsr -b %d -A -L 60 %s %s -fft-bench -E %s/%s.par -O %s_%s_P%d -t %d %s %s" % \
								(dspsr_nbins, verbose, zapstr, self.outdir, psr2, \
								psr, self.output_prefix, fpart, cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, s0_files[ii])
							self.execute(cmd, workdir=self.curdir)

						# running psradd to add all freq channels together
						self.log.info("Adding frequency channels together...")
						ar_files=glob.glob("%s/%s_%s_P*.ar" % (self.curdir, psr, self.output_prefix))
						cmd="psradd -R -o %s_%s.ar %s" % (psr, self.output_prefix, " ".join(ar_files))
						self.execute(cmd, workdir=self.curdir)
						# removing ar-files from dspsr for every frequency split
						if not cmdline.opts.is_debug:
							remove_list=glob.glob("%s/%s_%s_P*.ar" % (self.curdir, psr, self.output_prefix))
							cmd="rm -f %s" % (" ".join(remove_list))
							self.execute(cmd, workdir=self.curdir)

						# zapping rfi
						if not cmdline.opts.is_norfi:
							self.log.info("Zapping channels using median smoothed difference algorithm...")
							cmd="paz -r -e paz.ar %s_%s.ar" % (psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

						# dedispersing
						self.log.info("Dedispersing...")
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
							cmd="pam -D -m %s_%s.paz.ar" % (psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
						cmd="pam -D -e dd %s_%s.ar" % (psr, self.output_prefix)
						self.execute(cmd, workdir=self.curdir)

						# scrunching in frequency
						self.log.info("Scrunching in frequency to have %d channels in the output ar-file..." % (nsubs_eff))
						if self.nrChanPerSub > 1:
							# first, running fscrunch on zapped archive
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (nsubs_eff, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
								# remove non-scrunched zapped archive (we will always have unzapped non-scrunched version)
								cmd="rm -f %s_%s.paz.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							# running fscrunching on non-zapped archive
							cmd="pam --setnchn %d -e fscr.AR %s_%s.dd" % (nsubs_eff, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
							# remove non-scrunched dedispersed archive (we will always have unzapped non-dedispersed non-scrunched version)
							cmd="rm -f %s_%s.dd" % (psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
						else: # if number of chans == number of subs, we will just rename .paz.ar to .paz.fscr.AR and .dd to .fscr.AR
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								cmd="mv -f %s_%s.paz.ar %s_%s.paz.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							cmd="mv -f %s_%s.dd %s_%s.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

					# removing links for input .raw files
					if not cmdline.opts.is_debug:
						cmd="rm -f %s" % (" ".join(["%s.raw" % (f.split(".h5")[0]) for f in input_files]))
						self.execute(cmd, workdir=self.curdir)

			# running extra Psrchive programs, pam, pav,pdmp, etc... 
			# these programs should be run quick, so run them one by one

			if not cmdline.opts.is_skip_dspsr and not cmdline.opts.is_feedback:
				# first, calculating the proper max divisor for the number of subbands
#				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (self.nrChanPerSub, self.tab.nrSubbands))
				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(nsubs_eff, 63)))
				# calculating the greatest common denominator of self.tab.nrSubbands starting from 63 down
				pav_nchans = self.hcd(1, min(nsubs_eff, 63), nsubs_eff)
				for psr in self.psrs:  # pulsar list is empty if --nofold is used
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
						output_stem=".paz.fscr.AR"
					else: output_stem=".fscr.AR"
					# creating DSPSR diagnostic plots
					cmd="pav -DFTp -g %s_%s_DFTp.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -GTpf%d -g %s_%s_GTpf%d.ps/cps %s_%s%s" % (pav_nchans, psr, self.output_prefix, pav_nchans, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -YFp -g %s_%s_YFp.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -J -g %s_%s_J.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="convert \( %s_%s_GTpf%d.ps %s_%s_J.ps +append \) \( %s_%s_DFTp.ps %s_%s_YFp.ps +append \) \
                	                     -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
                        	             (psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
                                	      psr, self.output_prefix, psr, self.output_prefix)
					self.execute(cmd, workdir=self.curdir)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_skip_dspsr:
					# now running pdmp without waiting...
					if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
						self.log.info("Running pdmp...")
						pdmp_popens=[]  # list of pdmp Popen objects	
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
								output_stem=".paz.fscr.AR"
							else: output_stem=".fscr.AR"
							# getting the number of bins in the ar-file (it can be different from self.get_best_nbins, because
							# we still provide our own number of bins in --dspsr-extra-opts
							try:
								cmd="psredit -q -Q -c nbin %s/%s_%s%s" % (self.curdir, psr, self.output_prefix, output_stem)
								binsline=os.popen(cmd).readlines()
								if np.size(binsline) > 0:
									best_nbins=int(binsline[0][:-1])
									cmd="pdmp -mc %d -mb %d -g %s_%s_pdmp.ps/cps %s_%s%s" % \
										(nsubs_eff, min(128, best_nbins), psr, self.output_prefix, psr, self.output_prefix, output_stem)
									pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
									pdmp_popens.append(pdmp_popen)
							except Exception: pass
		
			# waiting for pdmp to finish
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_skip_dspsr:
					if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold: 
						self.waiting_list("pdmp", pdmp_popens)
						# when pdmp is finished do extra actions with files...
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
								output_stem=".paz.fscr.AR"
							else: output_stem=".fscr.AR"
							cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
							self.execute(cmd, is_os=True)
							cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
							self.execute(cmd, is_os=True)
							# reading new DM from the *.per file
							newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
							if np.size(newdm) > 1: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm[-1], psr, self.output_prefix, output_stem)
							else: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)

			# finishing off the processing...
			self.finish_off(obs, cep2, cmdline)

		except Exception:
			self.log.exception("Oops... 'run' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None


class CSUnit(PipeUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "CS"
		self.stokes = obs.stokesCS
		self.beams_root_dir = "stokes"
		self.raw2fits_extra_options="-CS -H"
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.nrSubsPerFile = obs.nrSubsPerFileCS
		self.sampling = obs.samplingCS
		self.summary_node = "locus092"
		self.summary_node_dir_suffix = "_CSplots" # "_CSplots"
		self.archive_suffix = "_pulpCS.tar.gz"
		self.outdir_suffix = "_red" # "_red"
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)

	def run(self, obs, cep2, cmdline, log):
		# currently can only process Stokes I
		if self.stokes == "I":
			PipeUnit.run(self, obs, cep2, cmdline, log)
		else:   # stub for Stokes IQUV
			self.log = log
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d (%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# finish
			self.end_time=time.time()
			self.total_time= self.end_time- self.start_time
			self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

class ISUnit(PipeUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "IS"
		self.stokes = obs.stokesIS
		self.beams_root_dir = "incoherentstokes"
		self.raw2fits_extra_options = "-CS -H -IS"
		self.nrChanPerSub = obs.nrChanPerSubIS
		self.nrSubsPerFile = obs.nrSubsPerFileIS
		self.sampling = obs.samplingIS
		self.summary_node = "locus094"
		self.summary_node_dir_suffix = "_redIS" # "_redIS"
		self.archive_suffix = "_pulpIS.tar.gz"
		self.outdir_suffix = "_redIS" # "_redIS"
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)

	def run(self, obs, cep2, cmdline, log):
		# currently can only process Stokes I
		if self.stokes == "I":
			PipeUnit.run(self, obs, cep2, cmdline, log)
		else:   # stub for Stokes IQUV
			self.log = log
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d (%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# finish
			self.end_time=time.time()
			self.total_time= self.end_time- self.start_time
			self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

class CVUnit(PipeUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "CV"
		self.stokes = obs.stokesCS
		self.beams_root_dir = "rawvoltages"
		self.raw2fits_extra_options=""
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.nrSubsPerFile = obs.nrSubsPerFileCS
		self.sampling = obs.samplingCS
		self.summary_node = "locus093"
		self.summary_node_dir_suffix = "_CVplots"  # "_CVplots"
		self.archive_suffix = "_pulpCV.tar.gz"
		self.outdir_suffix = "_red"  # "_red"
		# extensions of the files to copy to archive (parfile and parset will be also included)
		self.extensions=["*.pdf", "*.ps", "*png", "*.ar", "*.AR", "*pdmp*", "*.rv", "*.out", "*.h5"]
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)

	# refresh NFS mounting of locus node (loc) on hoover node
	# by doing 'ls' command
	def hoover_mounting(self, cep2, firstfile, loc):
		uniqdir="/".join(firstfile.split("/")[0:-1]).split("/data/")[-1]
		input_dir="%s/%s_data/%s" % (cep2.hoover_data_dir, loc, uniqdir)
		process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
		process.communicate()
		return input_dir

	# main CV processing function
	def run(self, obs, cep2, cmdline, log):
		# if there are no pulsars to fold we set --nofold option to True
		if len(self.psrs) == 0 and cmdline.opts.is_nofold == False:
			cmdline.opts.is_nofold = True
		# if we are not using dspsr directly to read *.h5 files
		if cmdline.opts.is_nodal:
			self.run_nodal(obs, cep2, cmdline, log)
		else:
			self.run_dal(obs, cep2, cmdline, log)

	# main run function using dspsr to read directly *.h5 files
	def run_dal(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# Re-creating root output directory
			cmd="mkdir -m 775 -p %s" % (self.outdir)
			self.execute(cmd)

			# creating Par-file in the output directory or copying existing one
			if not cmdline.opts.is_nofold: 
				self.get_parfile(cmdline, cep2)

			# Creating curdir dir
			cmd="mkdir -m 775 -p %s" % (self.curdir)
			self.execute(cmd)

			# if not just making plots...
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
        	        	                          # also we need to moint locus nodes that we need
					self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
					input_files=[]
					for loc in self.tab.location:
						if loc in self.tab.rawfiles:
							# first "mounting" corresponding locus node
							input_dir = self.hoover_mounting(cep2, self.tab.rawfiles[loc][0], loc)
							# dspsr needs all polarizations S* files to be in the current directory together with h5 files,
							# so we have to make soft links to input files
							self.log.info("Making links to input files in the current directory...")
							for f in self.tab.rawfiles[loc]:
								# links to the *.raw files
								cmd="ln -sf %s/%s ." % (input_dir, f.split("/" + obs.id + "/")[-1])
								self.execute(cmd, workdir=self.curdir)
								# copy *.h5 files (want to keep them)
								cmd="cp -f %s/%s.h5 ." % (input_dir, f.split("/" + obs.id + "/")[-1].split(".raw")[0])
								self.execute(cmd, workdir=self.curdir)
							input_file=["%s.h5" % (f.split("/" + obs.id + "/")[-1]).split(".raw")[0] for f in self.tab.rawfiles[loc]]
							input_files.extend(input_file)
				else:
					if cep2.current_node in self.tab.rawfiles:
						# make a soft links in the current dir (in order for processing to be consistent with the case when data are in many nodes)
						self.log.info("Making links to input files in the current directory...")
						for f in self.tab.rawfiles[cep2.current_node]:
							# links to the *.raw files
							cmd="ln -sf %s ." % (f)
							self.execute(cmd, workdir=self.curdir)
							# copy *.h5 files
							cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
							self.execute(cmd, workdir=self.curdir)
						input_files=["%s.h5" % (f.split("/" + obs.id + "/")[-1]).split(".raw")[0] for f in self.tab.rawfiles[cep2.current_node]]

				self.log.info("Input data: %s" % ("\n".join(input_files)))

			self.output_prefix="%s_SAP%d_%s" % (obs.id, self.sapid, self.procdir)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			#total_chan = self.tab.nrSubbands * self.nrChanPerSub
			proc_subs = self.nrSubsPerFile * cmdline.opts.nsplits
			if cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs > self.tab.nrSubbands:
				proc_subs -= (cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs - self.tab.nrSubbands)  
			nsubs_eff = min(self.tab.nrSubbands, proc_subs)
			total_chan = nsubs_eff * self.nrChanPerSub

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				# getting the list of "_S0_" files, the number of which is how many freq splits we have
				# we also sort this list by split number
				s0_files=sorted([f for f in input_files if re.search("_S0_", f) is not None], key=lambda input_file: int(input_file.split("_P")[-1].split("_")[0]))
				if not cmdline.opts.is_nofold and not cmdline.opts.is_nodecode:
					verbose="-q"
					if cmdline.opts.is_debug: verbose="-v"

					# running dspsr for every pulsar for all frequency splits
					self.log.info("Running dspsr for all frequency splits...")
					# loop on pulsars
					for psr in self.psrs:
						self.log.info("Running dspsr for pulsar %s..." % (psr))
						psr2=re.sub(r'[BJ]', '', psr)
						dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
						# loop on frequency splits
						for ii in range(len(s0_files)):
							# refreshing NFS mounting of locus nodes
							for loc in self.tab.location: 
								if loc in self.tab.rawfiles: 
									self.hoover_mounting(cep2, self.tab.rawfiles[loc][0], loc)
							fpart=int(s0_files[ii].split("_P")[-1].split("_")[0])
							cmd="dspsr -b %d -A -L %d %s -fft-bench -E %s/%s.par -O %s_%s_P%d -t %d -U minX%d %s %s" % \
								(dspsr_nbins, cmdline.opts.tsubint, verbose, self.outdir, psr2, \
								psr, self.output_prefix, fpart, cmdline.opts.nthreads, cmdline.opts.maxram, cmdline.opts.dspsr_extra_opts, s0_files[ii])
							self.execute(cmd, workdir=self.curdir)

						# running psradd to add all freq channels together
						self.log.info("Adding frequency channels together...")
						ar_files=glob.glob("%s/%s_%s_P*.ar" % (self.curdir, psr, self.output_prefix))
						cmd="psradd -R -o %s_%s.ar %s" % (psr, self.output_prefix, " ".join(ar_files))
						self.execute(cmd, workdir=self.curdir)
						# removing corrupted freq channels
						if self.nrChanPerSub > 1:
							self.log.info("Zapping every %d channel..." % (self.nrChanPerSub))
							cmd="paz -z \"%s\" -m %s_%s.ar" % \
								(" ".join([str(jj) for jj in range(0, total_chan, self.nrChanPerSub)]), psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

						# rfi zapping
						if not cmdline.opts.is_norfi:
							self.log.info("Zapping channels using median smoothed difference algorithm...")
							cmd="paz -r -e paz.ar %s_%s.ar" % (psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

						# removing ar-files from dspsr for every frequency split
						if not cmdline.opts.is_debug:
							remove_list=glob.glob("%s/%s_%s_P*.ar" % (self.curdir, psr, self.output_prefix))
							cmd="rm -f %s" % (" ".join(remove_list))
							self.execute(cmd, workdir=self.curdir)

					# removing links for input .raw files
					if not cmdline.opts.is_debug:
						cmd="rm -f %s" % (" ".join(["%s.raw" % (f.split(".h5")[0]) for f in input_files]))
						self.execute(cmd, workdir=self.curdir)

					# scrunching in freq
					#self.log.info("Scrunching in frequency to have %d channels in the output AR-file..." % (self.tab.nrSubbands))
					self.log.info("Scrunching in frequency to have %d channels in the output AR-file..." % (nsubs_eff))
					if self.nrChanPerSub > 1:
						for psr in self.psrs:
							# first, running fscrunch on zapped archive
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								#cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (self.tab.nrSubbands, psr, self.output_prefix)
								cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (nsubs_eff, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
								# remove non-scrunched zapped archive (we will always have unzapped non-scrunched version)
								cmd="rm -f %s_%s.paz.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							# running fscrunching on non-zapped archive
							cmd="pam --setnchn %d -e fscr.AR %s_%s.ar" % (nsubs_eff, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
					else: # if number of chans == number of subs, we will just rename *.ar-file to *.fscr.AR and make a link to it for original *.ar-file
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								cmd="mv -f %s_%s.paz.ar %s_%s.paz.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							cmd="mv -f %s_%s.ar %s_%s.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
							cmd="ln -sf %s_%s.fscr.AR %s_%s.ar" % (psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

			if not cmdline.opts.is_feedback:
				# first, calculating the proper min divisir for the number of subbands
#				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (self.nrChanPerSub, self.tab.nrSubbands))
				#self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(self.tab.nrSubbands, 63)))
				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(nsubs_eff, 63)))
				# calculating the greatest common denominator of self.tab.nrSubbands starting from self.nrChanPerSub
				#pav_nchans = self.hcd(1, min(self.tab.nrSubbands, 63), self.tab.nrSubbands)
				pav_nchans = self.hcd(1, min(nsubs_eff, 63), nsubs_eff)
				self.log.info("Creating diagnostic plots...")
				for psr in self.psrs:
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
						output_stem=".paz.fscr.AR"
					else: output_stem=".fscr.AR"
					# creating DSPSR diagnostic plots
					cmd="pav -SFTd -g %s_%s_SFTd.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -GTpdf%d -g %s_%s_GTpdf%d.ps/cps %s_%s%s" % (pav_nchans, psr, self.output_prefix, pav_nchans, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -YFpd -g %s_%s_YFpd.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -J -g %s_%s_J.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					if not cmdline.opts.is_skip_rmfit:
						try:
							# running rmfit for negative and positive RMs
							cmd="rmfit -m -100,0,100 -D -K %s_%s.negRM.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)
							cmd="rmfit -m 0,100,100 -D -K %s_%s.posRM.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)
							cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps %s_%s.posRM.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps %s_%s.negRM.ps +append \) \
        	                                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
								(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, psr, self.output_prefix, \
								psr, self.output_prefix, psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
						except Exception:
							self.log.warning("***** Warning! Rmfit has failed. Diagnostic plots will be made without rmfit plots. *****")
							cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps +append \) \
        	                                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
								(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
								psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
					else:
						cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps +append \) \
                	                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
							(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
							psr, self.output_prefix, psr, self.output_prefix)
						self.execute(cmd, workdir=self.curdir)

			# Running pdmp
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				# now running pdmp without waiting...
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
					self.log.info("Running pdmp...")
					pdmp_popens=[]  # list of pdmp Popen objects	
					for psr in self.psrs:
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
							output_stem=".paz.fscr.AR"
						else: output_stem=".fscr.AR"
						# getting the number of bins in the ar-file (it can be different from self.get_best_nbins, because
						# we still provide our own number of bins in --dspsr-extra-opts
						try:
							cmd="psredit -q -Q -c nbin %s/%s_%s%s" % (self.curdir, psr, self.output_prefix, output_stem)
							binsline=os.popen(cmd).readlines()
							if np.size(binsline) > 0:
								best_nbins=int(binsline[0][:-1])
								cmd="pdmp -mc %d -mb %d -g %s_%s_pdmp.ps/cps %s_%s%s" % \
									(nsubs_eff, min(128, best_nbins), psr, self.output_prefix, psr, self.output_prefix, output_stem)
								pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
								pdmp_popens.append(pdmp_popen)
						except Exception: pass
		
			# waiting for pdmp to finish
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold: 
					self.waiting_list("pdmp", pdmp_popens)
					# when pdmp is finished do extra actions with files...
					for psr in self.psrs:
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
							output_stem=".paz.fscr.AR"
						else: output_stem=".fscr.AR"
						cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
						self.execute(cmd, is_os=True)
						cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
						self.execute(cmd, is_os=True)
						# reading new DM from the *.per file
						newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
						if np.size(newdm) > 1: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm[-1], psr, self.output_prefix, output_stem)
						else: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm, psr, self.output_prefix, output_stem)
						self.execute(cmd, workdir=self.curdir)

			# finishing off the processing...
			self.finish_off(obs, cep2, cmdline)

		except Exception:
			self.log.exception("Oops... 'run' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None

	# main run function using bf2puma2 for conversion
	def run_nodal(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# Re-creating root output directory
			cmd="mkdir -m 775 -p %s" % (self.outdir)
			self.execute(cmd)

			# creating Par-file in the output directory or copying existing one
			if not cmdline.opts.is_nofold: 
				self.get_parfile(cmdline, cep2)

			# Creating curdir dir
			cmd="mkdir -m 775 -p %s" % (self.curdir)
			self.execute(cmd)

			# if not just making plots...
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and not cmdline.opts.is_nodecode:
				if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
        	        	                          # also we need to moint locus nodes that we need
					self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
					input_files=[]
					for loc in self.tab.location:
						if loc in self.tab.rawfiles:
							# first "mounting" corresponding locus node
							input_dir = self.hoover_mounting(cep2, self.tab.rawfiles[loc][0], loc)
							# bf2puma2 assumes all polarizations S* files to be in the current directory, so we have to
							# make soft links to input files
							self.log.info("Making links to input files in the current directory...")
							for f in self.tab.rawfiles[loc]:
								cmd="ln -sf %s/%s ." % (input_dir, f.split("/" + obs.id + "/")[-1])
								self.execute(cmd, workdir=self.curdir)
								# copy *.h5 files (want to keep them)
								cmd="cp -f %s/%s.h5 ." % (input_dir, f.split("/" + obs.id + "/")[-1].split(".raw")[0])
								self.execute(cmd, workdir=self.curdir)
							input_file=["%s" % (f.split("/" + obs.id + "/")[-1]) for f in self.tab.rawfiles[loc]]
							input_files.extend(input_file)
				else:
					if cep2.current_node in self.tab.rawfiles:
						# make a soft links in the current dir (in order for processing to be consistent with the case when data are in many nodes)
						self.log.info("Making links to input files in the current directory...")
						for f in self.tab.rawfiles[cep2.current_node]:
							# links to the *.raw files
							cmd="ln -sf %s ." % (f)
							self.execute(cmd, workdir=self.curdir)
						input_files=["%s" % (f.split("/" + obs.id + "/")[-1]) for f in self.tab.rawfiles[cep2.current_node]]

				self.log.info("Input data: %s" % ("\n".join(input_files)))

			self.output_prefix="%s_SAP%d_%s" % (obs.id, self.sapid, self.procdir)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			#total_chan = self.tab.nrSubbands * self.nrChanPerSub
			proc_subs = self.nrSubsPerFile * cmdline.opts.nsplits
			if cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs > self.tab.nrSubbands:
				proc_subs -= (cmdline.opts.first_freq_split * self.nrSubsPerFile + proc_subs - self.tab.nrSubbands)  
			nsubs_eff = min(self.tab.nrSubbands, proc_subs)
			total_chan = nsubs_eff * self.nrChanPerSub

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:

				# getting the list of "_S0_" files, the number of which is how many freq splits we have
				# we also sort this list by split number
				s0_files=sorted([f for f in input_files if re.search("_S0_", f) is not None], key=lambda input_file: int(input_file.split("_P")[-1].split("_")[0]))
				if not cmdline.opts.is_nodecode:
					# checking if extra options for complex voltages processing were set in the pipeline
					nblocks=""
					if cmdline.opts.nblocks != -1: nblocks = "-b %d" % (cmdline.opts.nblocks)
					is_all_for_scaling=""
					if cmdline.opts.is_all_times: is_all_for_scaling="-all_times"
					is_write_ascii=""
					if cmdline.opts.is_write_ascii: is_write_ascii="-t"
					verbose=""
					if cmdline.opts.is_debug: verbose="-v"

					# running bf2puma2 command for all frequency splits...
					self.log.info("Running bf2puma2 for all frequency splits...")
					# loop on frequency splits
					for ii in range(len(s0_files)):
						# refreshing NFS mounting of locus nodes
						for loc in self.tab.location: 
							if loc in self.tab.rawfiles: 
								self.hoover_mounting(cep2, self.tab.rawfiles[loc][0], loc)
						cmd="bf2puma2 -f %s -h %s -p %s -hist_cutoff %f %s %s %s %s" % (s0_files[ii], cep2.puma2header, obs.parset, cmdline.opts.hist_cutoff, verbose, nblocks, is_all_for_scaling, is_write_ascii)
						self.execute(cmd, workdir=self.curdir)

				# removing links for input files
				if not cmdline.opts.is_debug:
					cmd="rm -f %s" % (" ".join(input_files))
					self.execute(cmd, workdir=self.curdir)

				if not cmdline.opts.is_nofold:
					# getting the MJD of the observation
					# for some reason dspsr gets wrong MJD without using -m option
					input_prefix=s0_files[0].split("_S0_")[0]
					bf2puma_outfiles=glob.glob("%s/%s_SB*_CH*" % (self.curdir, input_prefix))
					self.log.info("Reading MJD of observation from the header of output bf2puma2 files...")
					cmd="head -25 %s | grep MJD" % (bf2puma_outfiles[0])
					mjdline=os.popen(cmd).readlines()
					if np.size(mjdline)>0:
						obsmjd=mjdline[0][:-1].split(" ")[-1]
						self.log.info("MJD = %s" % (obsmjd))
					else:
						self.log.error("Can't read the header of file '%s' to get MJD" % (bf2puma_outfiles[0]))
						self.kill()
						sys.exit(1)

					verbose="-q"
					if cmdline.opts.is_debug: verbose="-v"

					# running dspsr for every frequency channel. We run it in bunches of number of channels in subband
					# usually it is 16 which is less than number of cores in locus nodes. But even if it is 32, then it should be OK (I think...)
					self.log.info("Running dspsr for each frequency channels...")
					# loop on pulsars
					for psr in self.psrs:
						self.log.info("Running dspsr for pulsar %s..." % (psr))
						psr2=re.sub(r'[BJ]', '', psr)
						dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
						for bb in range(0, len(bf2puma_outfiles), self.nrChanPerSub):
							self.log.info("For %s" % (", ".join(bf2puma_outfiles[bb:bb+self.nrChanPerSub])))
							dspsr_popens=[] # list of dspsr Popen objects
							for cc in range(bb, bb+self.nrChanPerSub):
								input_file=bf2puma_outfiles[cc]
								cmd="dspsr -m %s -b %d -A -L %d %s -fft-bench -E %s/%s.par -O %s_%s_SB%s -t %d %s %s" % \
									(obsmjd, dspsr_nbins, cmdline.opts.tsubint, verbose, self.outdir, psr2, psr, self.output_prefix, \
										input_file.split("_SB")[1], cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, input_file)
								dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
								dspsr_popens.append(dspsr_popen)
								# running the single-pulse analysis
								if cmdline.opts.is_single_pulse:
									# getting coordinates string of the pulsar
									psr_ra=pu.coord_to_string(*pu.rad_to_hms(self.tab.rarad))
									psr_dec=pu.coord_to_string(*pu.rad_to_dms(self.tab.decrad))
									if self.tab.decrad < 0: psr_coords=psr_ra+psr_dec
									else: psr_coords=psr_ra+"+"+psr_dec
									cmd="digifil -set site=LOFAR -set name=%s -set coord=%s %s -F 2 -D 0.0 -b 8 -o %s_sp_%s_SB%s.fil %s" % (psr, psr_coords, verbose, psr, self.output_prefix, input_file.split("_SB")[1], input_file)
#									cmd="dspsr -T 59 -b 2097152 -c 59 -D 71.0398 -m %s %s -fft-bench -O %s_sp_%s_SB%s -t %d %s" % \
#										(obsmjd, verbose, psr, self.output_prefix, \
#											input_file.split("_SB")[1], cmdline.opts.nthreads, input_file)
									dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
									dspsr_popens.append(dspsr_popen)

							# waiting for dspsr to finish
							self.waiting_list("dspsr", dspsr_popens)

						# running psradd to add all freq channels together
						self.log.info("Adding frequency channels together...")
						ar_files=glob.glob("%s/%s_%s_SB*_CH*.ar" % (self.curdir, psr, self.output_prefix))
						cmd="psradd -R -o %s_%s.ar %s" % (psr, self.output_prefix, " ".join(ar_files))
						self.execute(cmd, workdir=self.curdir)
						if cmdline.opts.is_single_pulse:
							self.log.info("Adding frequency channels together for single-pulse analysis...")
#							ar_files=glob.glob("%s/%s_sp_%s_SB*_CH*.ar" % (self.curdir, psr, self.output_prefix))
#							cmd="psradd -R -o %s_sp_%s.ar %s" % (psr, self.output_prefix, " ".join(ar_files))
							ar_files=glob.glob("%s/%s_sp_%s_SB*_CH*.fil" % (self.curdir, psr, self.output_prefix))
							cmd="sigproc_splice -o %s_sp_%s.fil %s" % (psr, self.output_prefix, " ".join(ar_files))
							self.execute(cmd, workdir=self.curdir)
						# removing corrupted freq channels
						if self.nrChanPerSub > 1:
							self.log.info("Zapping every %d channel..." % (self.nrChanPerSub))
							cmd="paz -z \"%s\" -m %s_%s.ar" % \
								(" ".join([str(jj) for jj in range(0, total_chan, self.nrChanPerSub)]), psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

						# rfi zapping
						if not cmdline.opts.is_norfi:
							self.log.info("Zapping channels using median smoothed difference algorithm...")
							cmd="paz -r -e paz.ar %s_%s.ar" % (psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

						# removing files created by dspsr for each freq channel
						if not cmdline.opts.is_debug:
							remove_list=glob.glob("%s/%s_%s_SB*_CH*.ar" % (self.curdir, psr, self.output_prefix))
							cmd="rm -f %s" % (" ".join(remove_list))
							self.execute(cmd, workdir=self.curdir)

					# removing files created by bf2puma2 for each freq channel
					if not cmdline.opts.is_debug:
						remove_list=glob.glob("%s/%s_SB*_CH*" % (self.curdir, input_prefix))
						cmd="rm -f %s" % (" ".join(remove_list))
						self.execute(cmd, workdir=self.curdir)

					# scrunching in freq
					#self.log.info("Scrunching in frequency to have %d channels in the output AR-file..." % (self.tab.nrSubbands))
					self.log.info("Scrunching in frequency to have %d channels in the output AR-file..." % (nsubs_eff))
					if self.nrChanPerSub > 1:
						for psr in self.psrs:
							# first, running fscrunch on zapped archive
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								#cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (self.tab.nrSubbands, psr, self.output_prefix)
								cmd="pam --setnchn %d -e fscr.AR %s_%s.paz.ar" % (nsubs_eff, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
								# remove non-scrunched zapped archive (we will always have unzapped non-scrunched version)
								cmd="rm -f %s_%s.paz.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							# running fscrunching on non-zapped archive
							cmd="pam --setnchn %d -e fscr.AR %s_%s.ar" % (nsubs_eff, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
					else: # if number of chans == number of subs, we will just rename *.ar-file to *.fscr.AR and make a link to it for original *.ar-file
						for psr in self.psrs:
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								cmd="mv -f %s_%s.paz.ar %s_%s.paz.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							cmd="mv -f %s_%s.ar %s_%s.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
							cmd="ln -sf %s_%s.fscr.AR %s_%s.ar" % (psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)

			if not cmdline.opts.is_feedback:
				# first, calculating the proper min divisir for the number of subbands
#				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (self.nrChanPerSub, self.tab.nrSubbands))
				#self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(self.tab.nrSubbands, 63)))
				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (1, min(nsubs_eff, 63)))
				# calculating the greatest common denominator of self.tab.nrSubbands starting from self.nrChanPerSub
				#pav_nchans = self.hcd(1, min(self.tab.nrSubbands, 63), self.tab.nrSubbands)
				pav_nchans = self.hcd(1, min(nsubs_eff, 63), nsubs_eff)
				self.log.info("Creating diagnostic plots...")
				for psr in self.psrs:
					if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
						output_stem=".paz.fscr.AR"
					else: output_stem=".fscr.AR"
					# creating DSPSR diagnostic plots
					cmd="pav -SFTd -g %s_%s_SFTd.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -GTpdf%d -g %s_%s_GTpdf%d.ps/cps %s_%s%s" % (pav_nchans, psr, self.output_prefix, pav_nchans, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -YFpd -g %s_%s_YFpd.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					cmd="pav -J -g %s_%s_J.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
					self.execute(cmd, workdir=self.curdir)
					if not cmdline.opts.is_skip_rmfit:
						try:
							# running rmfit for negative and positive RMs
							cmd="rmfit -m -100,0,100 -D -K %s_%s.negRM.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)
							cmd="rmfit -m 0,100,100 -D -K %s_%s.posRM.ps/cps %s_%s%s" % (psr, self.output_prefix, psr, self.output_prefix, output_stem)
							self.execute(cmd, workdir=self.curdir)
							cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps %s_%s.posRM.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps %s_%s.negRM.ps +append \) \
                        	                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
								(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, psr, self.output_prefix, \
								psr, self.output_prefix, psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
						except Exception:
							self.log.warning("***** Warning! Rmfit has failed. Diagnostic plots will be made without rmfit plots. *****")
							cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps +append \) \
                        	                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
								(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
								psr, self.output_prefix, psr, self.output_prefix)
							self.execute(cmd, workdir=self.curdir)
					else:
						cmd="convert \( %s_%s_GTpdf%d.ps %s_%s_J.ps +append \) \( %s_%s_SFTd.ps %s_%s_YFpd.ps +append \) \
                	                             -append -rotate 90 -background white -flatten %s_%s_diag.png" % \
							(psr, self.output_prefix, pav_nchans, psr, self.output_prefix, psr, self.output_prefix, \
							psr, self.output_prefix, psr, self.output_prefix)
						self.execute(cmd, workdir=self.curdir)

			# Running pdmp
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				# now running pdmp without waiting...
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
					self.log.info("Running pdmp...")
					pdmp_popens=[]  # list of pdmp Popen objects	
					for psr in self.psrs:
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
							output_stem=".paz.fscr.AR"
						else: output_stem=".fscr.AR"
						# getting the number of bins in the ar-file (it can be different from self.get_best_nbins, because
						# we still provide our own number of bins in --dspsr-extra-opts
						try:
							cmd="psredit -q -Q -c nbin %s/%s_%s%s" % (self.curdir, psr, self.output_prefix, output_stem)
							binsline=os.popen(cmd).readlines()
							if np.size(binsline) > 0:
								best_nbins=int(binsline[0][:-1])
								cmd="pdmp -mc %d -mb %d -g %s_%s_pdmp.ps/cps %s_%s%s" % \
									(nsubs_eff, min(128, best_nbins), psr, self.output_prefix, psr, self.output_prefix, output_stem)
								pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
								pdmp_popens.append(pdmp_popen)
						except Exception: pass
		
			# waiting for pdmp to finish
			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold: 
					self.waiting_list("pdmp", pdmp_popens)
					# when pdmp is finished do extra actions with files...
					for psr in self.psrs:
						if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.fscr.AR" % (self.curdir, psr, self.output_prefix)):
							output_stem=".paz.fscr.AR"
						else: output_stem=".fscr.AR"
						cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
						self.execute(cmd, is_os=True)
						cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
						self.execute(cmd, is_os=True)
						# reading new DM from the *.per file
						newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
						if np.size(newdm) > 1: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm[-1], psr, self.output_prefix, output_stem)
						else: cmd="pam -e pdmp.AR -d %f -DTp %s_%s%s" % (newdm, psr, self.output_prefix, output_stem)
						self.execute(cmd, workdir=self.curdir)

			# finishing off the processing...
			self.finish_off(obs, cep2, cmdline)

		except Exception:
			self.log.exception("Oops... 'run' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None

class FE_CSUnit(PipeUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "CS"
		self.stokes = obs.stokesCS
		self.beams_root_dir = "stokes"
		self.raw2fits_extra_options="-CS -H"
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.sampling = obs.samplingCS
		self.summary_node = "locus092"
		self.summary_node_dir_suffix = "_CSplots" # "_CSplots"
		self.archive_suffix = "_pulpCS.tar.gz"
		self.outdir_suffix = "_red" # "_red"
		# re-assigning procdir from BEAMN to station name
		if obs.FE and self.tab.stationList[0] != "": 
			self.procdir = self.tab.stationList[0]
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)

	def run(self, obs, cep2, cmdline, log):
		# currently can only process Stokes I
		if self.stokes == "I":
			PipeUnit.run(self, obs, cep2, cmdline, log)
		else:   # stub for Stokes IQUV
			self.log = log
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d (FE/%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# finish
			self.end_time=time.time()
			self.total_time= self.end_time- self.start_time
			self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

class FE_CVUnit(CVUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "CV"
		self.stokes = obs.stokesCS
		self.beams_root_dir = "rawvoltages"
		self.raw2fits_extra_options=""
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.sampling = obs.samplingCS
		self.summary_node = "locus093"
		self.summary_node_dir_suffix = "_CVplots" # "_CVplots"
		self.archive_suffix = "_pulpCV.tar.gz"
		self.outdir_suffix = "_red" # "_red"
		# re-assigning procdir from BEAMN to station name
		if obs.FE and self.tab.stationList[0] != "": 
			self.procdir = self.tab.stationList[0]
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)
		# re-assigning procdir from BEAMN to station name
		if obs.FE and self.tab.stationList[0] != "": 
			self.procdir = self.tab.stationList[0]
		# setting outdir and curdir directories
		self.set_outdir(obs, cep2, cmdline)

	def run(self, obs, cep2, cmdline, log):
		CVUnit.run(self, obs, cep2, cmdline, log)
		"""
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (FE/%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))
		"""
