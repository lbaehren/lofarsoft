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
from pulp_parset import Observation, radial_distance, find_pulsars
from pulp_usercmd import CMDLine, check_pulsars
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
from pulp_feedback import FeedbackUnit
from pulp_pipeunit import *
from pulp_cvunit import *
from pulp_pipeunitpart import *
from pulp_cvunitpart import *

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
		self.log = None   # to be initialized later when doing summaries (in order to execute, etc. functions to be similar in syntax with those in PipeUnit)

		# initializing Processing Units based on the list of beams to process
                for beam in cmdline.beams:
                        sapid=int(beam.split(":")[0])
                        tabid=int(beam.split(":")[1])
                        tab = obs.saps[sapid].tabs[tabid]

			# checking if location (locus node) of raw data or at least processed data is known
			# If not, then skip this beam
			if len(tab.location) == 0: continue

			# if we do not want to use hoover nodes than we should make first a list of nodes
			# where processing will happen for parts of the band
			if len(tab.location) > 1 and cmdline.opts.is_nohoover:
				target_nodes=[key for key in tab.rawfiles.keys() if any("_S0_" in ff for ff in tab.rawfiles[key])]
				# this "target_nodes" list still can potentially have same nodes for different parts
				already_assigned_nodes=[] # list of nodes that already assigned as host nodes for processing of one of the parts
#				if tab.is_coherent and tab.specificationType != "flyseye":
#					if obs.CV: # can do this for now only for CV data
				for node in target_nodes:
					part = int([ff for ff in tab.rawfiles[node] if re.search("_S0_", ff)][0].split("_P")[1].split("_")[0])
					# first check that we do not start processing parts that we do not want
					if part < cmdline.opts.first_freq_split or part >= cmdline.opts.first_freq_split + cmdline.opts.nsplits: continue
					if node not in already_assigned_nodes:
						if not tab.is_coherent:
							unit = ISUnitPart(obs, cep2, cmdline, tab, log, node, part)
						if tab.is_coherent and tab.specificationType != "flyseye":
							if obs.CS:
								unit = CSUnitPart(obs, cep2, cmdline, tab, log, node, part)
							elif obs.CV:
								unit = CVUnitPart(obs, cep2, cmdline, tab, log, node, part)
							else:
								log.error("Can't initialize processing pipeline unit for SAP=%d TAB=%d PART=%d on node %s" % (sapid, tabid, part, node))
								quit(1)
						if tab.is_coherent and tab.specificationType == "flyseye":
							if obs.stokesCS[0] == "I":  # for I and IQUV
								unit = FE_CSUnitPart(obs, cep2, cmdline, tab, log, node, part)
							elif obs.stokesCS[0] == "X": # for XY (old format) or XXYY
								unit = FE_CVUnitPart(obs, cep2, cmdline, tab, log, node, part)
							else:
								log.error("Can't initialize processing pipeline FE unit for SAP=%d TAB=%d PART=%d on node %s" % (sapid, tabid, part, node))
								quit(1)
						already_assigned_nodes.append(node)

					else:   # i.e. this node is already taken to process some other part, so we have to check if one of
						# other nodes that have this part is not taken yet
						# getting list of nodes that have files that belong to this Part
						part_nodes=[key for key in tab.rawfiles.keys() if any("_P%03d_" % (part) in ff for ff in tab.rawfiles[key])]
						# getting the list of nodes from "part_nodes" that are not in the "already_assigned_nodes"
						good_nodes=list(set(part_nodes)-set(part_nodes).intersection(set(already_assigned_nodes)))
						if len(good_nodes) == 0: # means that there is no separate node available
							log.error("No unique separate node available to process the Part=%d for SAP=%d TAB=%d.\nTry process without --no-hoover option!" % \
								(part, sapid, tabid))
							quit(1)
						else:
							if not tab.is_coherent:
								unit = ISUnitPart(obs, cep2, cmdline, tab, log, good_nodes[0], part)
							if tab.is_coherent and tab.specificationType != "flyseye":
								if obs.CS:
									unit = CSUnitPart(obs, cep2, cmdline, tab, log, good_nodes[0], part)
								elif obs.CV:
									unit = CVUnitPart(obs, cep2, cmdline, tab, log, good_nodes[0], part)
								else:
									log.error("Can't initialize processing pipeline unit for SAP=%d TAB=%d PART=%d on node %s" % (sapid, tabid, part, good_nodes[0]))
									quit(1)
							if tab.is_coherent and tab.specificationType == "flyseye":
								if obs.stokesCS[0] == "I":  # for I and IQUV
									unit = FE_CSUnitPart(obs, cep2, cmdline, tab, log, good_nodes[0], part)
								elif obs.stokesCS[0] == "X": # for XY (old format) or XXYY
									unit = FE_CVUnitPart(obs, cep2, cmdline, tab, log, good_nodes[0], part)
								else:
									log.error("Can't initialize processing pipeline FE unit for SAP=%d TAB=%d PART=%d on node %s" % (sapid, tabid, part, good_nodes[0]))
									quit(1)
							already_assigned_nodes.append(good_nodes[0])

					# adding unit to the list
					self.units.append(unit)

			else: # only one locus node in the beam
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

		# creating main output directory on locus092 for CS data and on locus094 for IS data, and on locus093 for CV data
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
			tabpart=""
			locations="[#locations = %d, #files = %d]" % (len(unit.tab.location), unit.tab.numfiles)
			if len(unit.tab.location) > 1:
				if cmdline.opts.is_nohoover: # are not using hoover nodes
					locus=unit.location
					tabpart="PART=%d " % (unit.part)
					target_nodes=[key for key in unit.tab.rawfiles.keys() if any("_P%03d_" % unit.part in ff for ff in unit.tab.rawfiles[key])]
					nfiles=len([len([val for val in unit.tab.rawfiles[key] if re.search("_P%03d_" % unit.part, val)]) for key in target_nodes])
					locations="[#locations = %d, #files = %d]" % (len(target_nodes), nfiles)
				else: locus=cep2.hoover_nodes[0]
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
			cmd="ssh -t %s '%s/release/share/pulsar/bin/pulp.py --noinit --local --beams %d:%d%s %s'" % \
				(locus, cep2.lofarsoft, unit.sapid, unit.tabid, tabpart != "" and "/%d" % unit.part or "", " ".join(cmdline.options))
			unit.parent = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
			log.info("SAP=%d TAB=%d %s%s(%s%s) on %s (pid=%d)  %s" % \
				(unit.sapid, unit.tabid, tabpart, unit.tab.specificationType == "flyseye" and ", ".join(unit.tab.stationList) + " " or "", \
				unit.tab.specificationType == "flyseye" and "FE/" or "", \
				unit.tab.is_coherent and (obs.CS and "CS" or "CV") or "IS", \
				locus, unit.parent.pid, locations))
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
				log.info("Failed beams [%d]: %s" % (self.number_failed_pipes, ",".join(["%s:%s%s" % (u.sapid, u.tabid, \
					(len(u.tab.location)>1 and cmdline.opts.is_nohoover) and " (P%d)" % u.part or "") for u in failed_units])))
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
               			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
       	        		self.log.process2log(proc)
        	       		proc.communicate()
				status = proc.poll()
			job_end = time.time()
			job_total_time = job_end - job_start
        	       	self.log.info("Finished at UTC %s, status=%s, Total runnung time: %.1f s (%.2f hrs)" % \
					(time.asctime(time.gmtime()), status, job_total_time, job_total_time/3600.))
			self.log.info("")
			# if job is not successful
			if status != 0:
				raise Exception
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (cmd, status))
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
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
                                time.sleep(10)  # waiting 10 secs to see if process crashes right away
                                if process.poll() is not None and process.poll() != 0:
                                        raise Exception
                                else: process.kill()  # if process is still running, it means that cmd is good, so we kill it in order to
                                                        # restart it with proper stdout/stderr and add it to the list
                        process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
                        status=process.returncode
                        self.log.info("Job pid=%d, not waiting for it to finish." % (process.pid))
                        return process
                except Exception:
                        self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (re.sub("\n", "\\\\n", cmd), status))
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
                        raise Exception
	
	# waiting for the command to finish
        def waiting(self, prg, popen):
                """
                Waiting for process to finish
                """
                try:
                        job_start = time.time()
                        self.log.info("Waiting for %s to finish, pid=%d" % (prg, popen.pid))
                        (sout, serr) = popen.communicate()
                        # we pipe serr to sout, so no need to log serr
                        self.log.info(sout)
                        job_end = time.time()
                        job_total_time = job_end - job_start
                        self.log.info("Process pid=%d (%s) has finished at UTC %s, status=%d, Waiting time: %.1f s (%.2f hrs)" % \
                                (popen.pid, prg, time.asctime(time.gmtime()), popen.returncode, job_total_time, job_total_time/3600.))
                        self.log.info("")
                        # if job is not successful
                        if popen.poll() != 0:
                                raise Exception
                except Exception:
                        self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, popen.pid, popen.returncode))
			Popen(shlex.split("stty sane"), stderr=open(os.devnull, 'rb')).wait()
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
                                if len(run_units) > 0: sefl.log.info("Still running [%d]: %s" % (len(run_units), run_units))
                        job_end = time.time()
                        job_total_time = job_end - job_start
                        self.log.info("Processes of %s have finished at UTC %s, Waiting time: %.1f s (%.2f hrs)" % \
                                (prg, time.asctime(time.gmtime()), job_total_time, job_total_time/3600.))
                        self.log.info("")
                except Exception:
                        self.log.exception("Oops... %s has crashed!\npids = %s" % (prg, ",".join(["%d" % (fu.pid) for fu in popen_list if fu.poll() is not None])))
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
		self.log = log
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# moving log-files to corresponding directory
		if self.log != None: self.log.info("Moving log-files...")
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)):
				if not cmdline.opts.is_log_append:	
					cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
						(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
					if self.log != None: self.execute(cmd, workdir=sumdir)
					else:
						proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
						proc.communicate()
				else:
					# appending log from sumdir to the one in corresponing beam directory
					cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
						(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
					if self.log != None: self.execute(cmd, is_os=True)
					else: os.system(cmd)
					# removing log from sumdir
					cmd="rm -f %s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)
					if self.log != None: self.execute(cmd, workdir=sumdir)
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
			if cmdline.opts.is_feedback: 
				self.make_feedback(obs, cep2, cmdline, log)
			else:
				is_to_combine=False
				if cmdline.opts.is_nohoover: # are not using hoover nodes
					for u in self.units: # checking if we actually have data spread across several nodes
						if len(u.tab.location) > 1:
							is_to_combine=True
							break
					if is_to_combine: # if we do have data across many nodes, we first call combine function before doing summaries
						self.combine_parts(obs, cep2, cmdline, log)
				if obs.CV:
					self.make_summary_CV(obs, cep2, cmdline, log)
				else:
					self.make_summary_CS_IS(obs, cep2, cmdline, log)

		except Exception:
			log.exception("Oops... 'make_summary' function on %s has crashed!" % (cep2.get_current_node()))
			quit(1)


	# function that combines the ar-files from separate parts for each of the beams for the ObsID
	# and runs further processing in the combined file before summaries
	def combine_parts(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			start_time=time.time()	
			sumnode=cep2.get_current_node()
			sumdir=self.summary_dirs[sumnode]

			# start logging
			self.log.info("Combining parts for all relevant beams on %s:%s    UTC start time is: %s  @node: %s" % \
					(sumnode, sumdir, time.asctime(time.gmtime()), sumnode))	

			# combining parts and do further processing for one beam at a time
			# first, we make a list of beams that require this combining. We can't use self.units list directly 
			# as it has element for each part
			beams_combine=[]
			for unit in [u for u in self.units if u.summary_node == sumnode]:
				if len(unit.tab.location) > 1 and cmdline.opts.is_nohoover:
					beams_combine.append("%d:%d" % (unit.sapid, unit.tabid))
			beams_combine=np.unique(beams_combine)
			self.log.info("The parts of these beams will be combined: %s" % (", ".join(beams_combine)))

			# main loop for combining parts of the beam
			for beam in beams_combine:
				sapid=int(beam.split(":")[0])
				tabid=int(beam.split(":")[1])
				ref_unit = None # we will use it to access necessary info that is same for all parts (e.g. list of pulsars)
				for unit in self.units:
					if unit.sapid == sapid and unit.tabid == tabid:
						ref_unit = unit
						break
				if ref_unit == None:
					self.log.error("Can't find any processing units for the beam %s!" % (beam))
					quit(1)
				self.log.info("Combining parts for the beam %s..." % (beam))

				curdir = "%s/%s/SAP%d/%s" % (sumdir, ref_unit.beams_root_dir, sapid, ref_unit.procdir)
				output_prefix="%s_SAP%d_%s" % (obs.id, sapid, ref_unit.procdir)
				proc_subs = ref_unit.nrSubsPerFile * cmdline.opts.nsplits
				if cmdline.opts.first_freq_split * ref_unit.nrSubsPerFile + proc_subs > ref_unit.tab.nrSubbands:
					proc_subs -= (cmdline.opts.first_freq_split * ref_unit.nrSubsPerFile + proc_subs - ref_unit.tab.nrSubbands)  
				nsubs_eff = min(ref_unit.tab.nrSubbands, proc_subs)
				total_chan = nsubs_eff * ref_unit.nrChanPerSub

				# loop on pulsars
				if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
					for psr in ref_unit.psrs:
						self.log.info("Combining parts for pulsar %s..." % (psr))
	
        	                                # running psradd to add all freq channels together
                	                        self.log.info("Adding frequency splits together...")
                        	                ar_files=glob.glob("%s/%s_%s_P*.ar" % (curdir, psr, output_prefix))
						if len(ar_files) == 0:
							self.log.info("skipped")
						else:
                                	        	cmd="psradd -R -m time -o %s_%s.ar %s" % (psr, output_prefix, " ".join(ar_files))
                                        		self.execute(cmd, workdir=curdir)

							# running common DSPSR post-processing
							dspsr_postproc(self, ref_unit, cmdline, obs, psr, total_chan, nsubs_eff, curdir, output_prefix)
							
                                	       	        # removing ar-files from dspsr for every frequency split
                                        	       	if not cmdline.opts.is_debug:
                                                	       	remove_list=glob.glob("%s/%s_%s_P*.ar" % (curdir, psr, output_prefix))
                                                        	cmd="rm -f %s" % (" ".join(remove_list))
       	                                                	self.execute(cmd, workdir=curdir)

				# running pav
				if not cmdline.opts.is_feedback:
					make_dspsr_plots(self, ref_unit, cmdline, obs, nsubs_eff, curdir, output_prefix)

				# Running pdmp
				if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and \
				not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
					pdmp_popens = start_pdmp(self, ref_unit, cmdline, obs, nsubs_eff, curdir, output_prefix)
					# waiting for pdmp to finish
					finish_pdmp(self, ref_unit, pdmp_popens, cmdline, obs, curdir, output_prefix)

				# Running spectar.py to calculate pulsar spectra
				if not cmdline.opts.is_feedback:
					calc_psr_spectra(self, ref_unit, cmdline, obs, sapid, tabid, curdir, output_prefix)

				# tarball name
				tarname="%s_sap%03d_tab%04d%s" % (obs.id, sapid, tabid, ref_unit.archive_suffix)
				output_archive="%s/%s" % (sumdir, tarname)

				if not cmdline.opts.is_feedback:
					# copying parset file to output directory
					self.log.info("Copying original parset file to output directory...")
					cmd="cp -f %s %s" % (obs.parset, sumdir)
					self.execute(cmd, workdir=sumdir)
					# Make a tarball of all the plots for this beam
					self.log.info("Making a full archive tarball of all the files with extensions: %s" % (", ".join(ref_unit.full_archive_exts)))
					# create a new full archive tarball
					tar_list=[]
					for ext in ref_unit.full_archive_exts:
						ext_list=rglob(curdir, ext, 3)
						tar_list.extend(ext_list)
					tar_list.extend(glob.glob("%s/*.par" % (sumdir)))
					tar_list.extend(glob.glob("%s/*.parset" % (sumdir)))
					cmd="tar -cvz --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
					try:
						# --ignore-failed-read does not seem to help with tar failing for some beams
						# like file was changed during the tar, though tarball seem to be fine
						proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=sumdir)
						proc.communicate()
					except: pass

				# initializing the Feedback unit
				fbindex=0
				for ss in range(sapid): 
					for sap in obs.saps:
						if sap.sapid == ss:
							fbindex += sap.nrTiedArrayBeams
							break
				fbindex += tabid
				fbunit = FeedbackUnit(fbindex, cep2.current_node, sumdir, obs.id, sapid, tabid)
				fbunit.update("%s/%s" % (sumdir, tarname), ref_unit.code)
				fbunit.flush("%s/.%s_sap%03d_beam%04d.fb" % (cep2.get_logdir(), obs.id, sapid, tabid), cep2, False)

		except Exception:
			self.log.exception("Oops... 'combine_parts' function on %s has crashed" % (cep2.get_current_node()))
			quit(1)

	# run necessary processes to organize summary info on summary nodes for CV data
	# to be run locally on summary node
	def make_summary_CV(self, obs, cep2, cmdline, log):

		self.log = log
		start_time=time.time()	
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# start logging
		self.log.info("Summaries on %s:%s    UTC start time is: %s  @node: %s" % \
				(sumnode, sumdir, time.asctime(time.gmtime()), sumnode))

		# extracting files from archive files for all beams
		# and moving log-files to corresponding directory
		self.log.info("Extracting archives in summary nodes, removing archives, moving log-files...")
		already_extracted=[] # list of archives already extracted (to prevent the same archive to be extracted many times in case multiple parts)
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			result_archive="%s_sap%03d_tab%04d%s" % (obs.id, unit.sapid, unit.tabid, unit.archive_suffix)
                        if result_archive in already_extracted: continue
                        else: already_extracted.append(result_archive)
			if os.path.exists("%s/%s" % (sumdir, result_archive)):
				# extracting archive
				cmd="tar xvfz %s" % (result_archive)
				self.execute(cmd, workdir=sumdir)
				# removing archive
				if cmdline.opts.nsplits < 2:
					cmd="rm -f %s" % (result_archive)
					self.execute(cmd, workdir=sumdir)
				# moving log-file to corresponding SAP/BEAM directory
				if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)) and \
					os.path.exists("%s/%s/SAP%d/%s" % (sumdir, unit.beams_root_dir, unit.sapid, unit.procdir)):
					if not cmdline.opts.is_log_append:	
						cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
							(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
						self.execute(cmd, workdir=sumdir)
					else:
						# appending log from sumdir to the one in corresponing beam directory
						cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
							(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, is_os=True)
						# removing log from sumdir
						cmd="rm -f %s_sap%03d_beam%04d.log" % (obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, workdir=sumdir)
			else:
				if not os.path.exists("%s/%s" % (sumdir, unit.curdir.split(unit.outdir + "/")[1])):
					self.log.warning("Warning! Neither archive file %s nor corresponding directory tree exists in: %s. Summary won't be complete" % (result_archive, sumdir))

                # either "CS", "IS", "CV", ..
                data_code=[u.code for u in self.units if u.summary_node == sumnode][0]

		# create beam_process_node.txt file (this is only if file does not exist or it is empty)
		beam_process_node_file="%s/beam_process_node.txt" % (sumdir)
		if not os.path.exists(beam_process_node_file) or os.path.getsize(beam_process_node_file) == 0:
			self.log.info("Creating the beam_process_node.txt file...")
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
			self.log.info("Creating DSPSR summary diagnostic plots...")
			if len(dspsr_diags) > 1: cmd="montage %s -background none -mode concatenate -tile %dx dspsr_status.png" % (" ".join(dspsr_diags), int(math.sqrt(len(dspsr_diags))))
			else: cmd="cp -f %s dspsr_status.png" % (dspsr_diags[0])
			self.execute(cmd, workdir=sumdir)

		if os.path.exists("%s/dspsr_status.png" % (sumdir)):
			self.log.info("Copying dspsr status file to status.png ...")
			cmd="mv dspsr_status.png status.png"
			self.execute(cmd, workdir=sumdir)

		# creating thumbnail version of status.png if it exists
		if os.path.exists("%s/status.png" % (sumdir)):		
			self.log.info("Making a thumbnail version of status.png file...")
			cmd="convert -scale 200x140-0-0 status.png status.th.png"
			self.execute(cmd, workdir=sumdir)
		else:
			self.log.info("No status.png created")

		# Make a tarball of all the plots (summary archive)
		self.log.info("Making a final summary tarball of all files with extensions: %s" % (", ".join(self.summary_archive_exts)))
		tarname="%s%s%s%s" % (obs.id, self.summary_archive_prefix, data_code, self.summary_archive_suffix)
		tar_list=[]
		for ext in self.summary_archive_exts:
			ext_list=rglob(sumdir, ext, 3)
			tar_list.extend(ext_list)
		cmd="tar -cv --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
		try: # --ignore-failed-read does not seem to help with tar failing for some beams
                     # like file was changed during the tar, though tarball seem to be fine
			self.execute(cmd, workdir=sumdir)
		except: pass

		# finish
		end_time=time.time()
		total_time= end_time- start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (total_time, total_time/3600.))

		# flushing log file and copy it to summary node
		self.log.flush()
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

		self.log = log
		start_time=time.time()	
		sumnode=cep2.get_current_node()
		sumdir=self.summary_dirs[sumnode]

		# start logging
		self.log.info("Summaries on %s:%s    UTC start time is: %s  @node: %s" % \
				(sumnode, sumdir, time.asctime(time.gmtime()), sumnode))

		# extracting files from archive files for all beams
		# and moving log-files to corresponding directory
		self.log.info("Extracting archives in summary nodes, removing archives, moving log-files...")
		already_extracted=[] # list of archives already extracted (to prevent the same archive to be extracted many times in case multiple parts)
		for unit in [u for u in self.units if u.summary_node == sumnode]: 
			result_archive="%s_sap%03d_tab%04d%s" % (obs.id, unit.sapid, unit.tabid, unit.archive_suffix)
			if result_archive in already_extracted: continue
			else: already_extracted.append(result_archive)
			if os.path.exists("%s/%s" % (sumdir, result_archive)):
				# extracting archive
				cmd="tar xvfz %s" % (result_archive)
				self.execute(cmd, workdir=sumdir)
				# removing archive
				if cmdline.opts.nsplits < 2:
					cmd="rm -f %s" % (result_archive)
					self.execute(cmd, workdir=sumdir)
				# moving log-file to corresponding SAP/BEAM directory
				if os.path.exists("%s/%s_sap%03d_beam%04d.log" % (sumdir, obs.id, unit.sapid, unit.tabid)) and \
					os.path.exists("%s/%s/SAP%d/%s" % (sumdir, unit.beams_root_dir, unit.sapid, unit.procdir)):
					if not cmdline.opts.is_log_append:	
						cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
							(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
						self.execute(cmd, workdir=sumdir)
					else:
						# appending log from sumdir to the one in corresponing beam directory
						cmd="cat %s/%s_sap%03d_beam%04d.log >> %s/%s/SAP%d/%s/%s_sap%03d_beam%04d.log" % \
							(sumdir, obs.id, unit.sapid, unit.tabid, sumdir, unit.beams_root_dir, unit.sapid, unit.procdir, obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, is_os=True)
						# removing log from sumdir
						cmd="rm -f %s_sap%03d_beam%04d.log" % (obs.id, unit.sapid, unit.tabid)
						self.execute(cmd, workdir=sumdir)
			else:
				if not os.path.exists("%s/%s" % (sumdir, unit.curdir.split(unit.outdir + "/")[1])):
					self.log.warning("Warning! Neither archive file %s nor corresponding directory tree exists in: %s. Summary won't be complete" % (result_archive, sumdir))

		# either "CS", "IS", "CV", .. 
		data_code=[u.code for u in self.units if u.summary_node == sumnode][0]

		# getting the list of *.pfd.bestprof files and read chi-sq values for all folded pulsars
		if not cmdline.opts.is_nofold:
			self.log.info("Reading chi-squared values and adding to chi-squared.txt...")
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
      	               	        	chi_val = 0.0
                        		cmd="cat %s | grep chi-sqr | cut -d = -f 2" % (bp)
	               	        	chiline=os.popen(cmd).readlines()
                        		if np.size(chiline) > 0:
               	                		chi_val = float(chiline[0].rstrip())
   	               	        	else:
						self.log.warning("Warning: can't read chi-sq value from %s" % (bp))
               	        		# getting current number for SAP and TA beam (or station name for FE)
					try: # we need this try block in case User manually creates sub-directories with some test bestprof files there
					     # which will also be found by rglob function. So, we need to exclude them by catching an Exception
					     # on a wrong-formed string applying int()
                      		  		cursapid=int(thumbfile.split("_SAP")[-1].split("_")[0])
					except: continue
               	                	curprocdir=thumbfile.split("_SAP")[-1].split("_")[1]
					# checking if we have different splits or not and depending on that make label differently
					if thumbfile == thumbfile.split("_PART")[0]: # there is only one part
	                        		chif.write("file=%s obs=%s_SAP%d_%s_%s chi-sq=%g\n" % (thumbfile, data_code, cursapid, curprocdir, psr, chi_val))
						if os.path.exists("%s/%s" % (sumdir, thumbfile)):
        		                		montage_cmd += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, thumbfile)
						if os.path.exists("%s/%s" % (sumdir, bigfile)):
        	        	        		montage_cmd_pdf += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, bigfile)
					else: # we have many parts
						curpart=int(thumbfile.split("_PART")[-1].split("_")[0])
	                        		chif.write("file=%s obs=%s_SAP%d_%s_PART%d_%s chi-sq=%g\n" % (thumbfile, data_code, cursapid, curprocdir, curpart, psr, chi_val))
						if os.path.exists("%s/%s" % (sumdir, thumbfile)):
        		                		montage_cmd += "-label '%s SAP%d %s\nPART%d\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, curpart, psr, chi_val, thumbfile)
						if os.path.exists("%s/%s" % (sumdir, bigfile)):
        	        	        		montage_cmd_pdf += "-label '%s SAP%d %s\nPART%d\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, curpart, psr, chi_val, bigfile)
              	        chif.close()

			# creating combined plots
			if len(psr_bestprofs) > 0:
                        	self.log.info("Combining all pfd.th.png files in a single combined plot...")
        	        	montage_cmd += "combined.png"
				self.execute(montage_cmd, workdir=sumdir)
				# making thumbnail version of the combined plot
				self.log.info("Making a thumbnail version of combined plot...")
               	        	cmd="convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.png combined.th.png"
               			self.execute(cmd, workdir=sumdir)

				# creating combined PDF plot with all prepfold plots - ONLY for FE 
				if data_code == "CS" and obs.FE:
                        		self.log.info("Combining all pfd.pdf files in a single combined multi-page PDF file...")
        	        		montage_cmd_pdf += "combined.pdf"
					self.execute(montage_cmd_pdf, workdir=sumdir)

		# create beam_process_node.txt file (this is only if file does not exist or it is empty)
		beam_process_node_file="%s/beam_process_node.txt" % (sumdir)
		if not os.path.exists(beam_process_node_file) or os.path.getsize(beam_process_node_file) == 0:
			self.log.info("Creating the beam_process_node.txt file...")
        	        bpnf=open(beam_process_node_file, 'w')
			for unit in [u for u in self.units if u.summary_node == sumnode]: 
				for node in unit.tab.location:
					if node in unit.tab.rawfiles:
						for rf in unit.tab.rawfiles[node]:
							bpnf.write("%s %s%s\n" % \
							(node, rf, unit.tab.specificationType == "flyseye" and " [%s]" % (",".join(unit.tab.stationList)) or ""))
			bpnf.close()

		# removing old version of all status png files (if exist)
		self.log.info("Removing previous status png files (if any) ...")
		cmd="rm -f %s %s" % (" ".join(glob.glob("%s/*status*.png" % (sumdir))), " ".join(glob.glob("%s/ta_heatmap_sap*.png" % (sumdir))))
		self.execute(cmd, workdir=sumdir)

		# creating TA heatmaps 
		# only when folding, and only if pulsars are set from the command line, or 'parset' or 'sapfind' or 'sapfind3' or "tabfind+" keywords are used (or
		# nothing is given for --pulsar option)
		# otherwise, different TA beams will be folded for different pulsars, and TA heatmap does not have sense
		if data_code == "CS" and not cmdline.opts.is_nofold and (len(cmdline.psrs) == 0 or (len(cmdline.psrs) != 0 and cmdline.psrs[0] != "tabfind")):
			for sap in obs.saps:
				if sap.nrRings > 0:
					if len(cmdline.psrs) != 0 and cmdline.psrs[0] != "parset" and cmdline.psrs[0] != "sapfind" and \
										cmdline.psrs[0] != "sapfind3" and cmdline.psrs[0] != "tabfind+":
						psrs = cmdline.psrs # getting list of pulsars from command line
					else: 
						if len(cmdline.psrs) == 0:
							if sap.source != "" and check_pulsars(sap.source, cmdline, cep2, None): psrs = [sap.source]
							else:
								if len(sap.psrs) > 0: psrs = [sap.psrs[0]]
								else: psrs = []
						else:
							if cmdline.psrs[0] == "parset":
								if sap.source != "" and check_pulsars(sap.source, cmdline, cep2, None): psrs = [sap.source]
								else: psrs = []
							if cmdline.psrs[0] == "sapfind" or cmdline.psrs[0] == "sapfind3":
								if len(sap.psrs) > 0:
									if cmdline.psrs[0] == "sapfind": psrs = [sap.psrs[0]]
									else: psrs = sap.psrs
								else: psrs = []
							if cmdline.psrs[0] == "tabfind+":
								if sap.source != "" and check_pulsars(sap.source, cmdline, cep2, None): psrs = [sap.source]
								else: psrs = []
								if len(sap.psrs) > 0: psrs.append(sap.psrs[0])
								psrs = list(np.unique(psrs))
					self.log.info("Creating TA heatmap with %d rings for SAP=%d..." % (sap.nrRings, sap.sapid))
					for psr in psrs:
						self.log.info(psr)
						# I need this try/except block here, to avoid situation, when there are more than 1 pulsar in the SAP
						# but processing was done only for 1 - this is the case when pulsar is not specified in the command line
						try:
							cmd="cat %s/chi-squared.txt | grep _SAP%d | grep %s > %s/%s-chi-squared.txt" % (sumdir, sap.sapid, psr, sumdir, psr)
							self.execute(cmd, is_os=True)
							cmd="plot_LOFAR_TA_multibeam3.py --sap %d --chi %s-chi-squared.txt --parset %s.parset --out_logscale %s_SAP%d_%s_TA_heatmap_log.png --out_linscale %s_SAP%d_%s_TA_heatmap_linear.png --target %s" % (sap.sapid, psr, obs.id, obs.id, sap.sapid, psr, obs.id, sap.sapid, psr, psr)
							self.execute(cmd, workdir=sumdir)
							cmd="rm -f %s-chi-squared.txt" % (psr)
							self.execute(cmd, workdir=sumdir)
							# combining TA heatmap log and linear plots
							cmd="convert %s_SAP%d_%s_TA_heatmap_log.png %s_SAP%d_%s_TA_heatmap_linear.png -append ta_heatmap_sap%d_%s.png" % (obs.id, sap.sapid, psr, obs.id, sap.sapid, psr, sap.sapid, psr)
							self.execute(cmd, workdir=sumdir)
						except: 
							self.log.info("Can't make a heatmap plot for pulsar %s" % (psr))
							continue

					# combining TA heatmaps for different pulsars
					heatmaps=glob.glob("%s/ta_heatmap_sap%d_*.png" % (sumdir, sap.sapid))
					if len(heatmaps) > 0:
						if len(heatmaps) > 1: cmd="convert %s +append ta_heatmap_sap%d.png" % (" ".join(heatmaps), sap.sapid)
						else: cmd="mv %s ta_heatmap_sap%d.png" % (heatmaps[0], sap.sapid)
						self.execute(cmd, workdir=sumdir)
						# remove temporary png files
						cmd="rm -f %s" % (" ".join(heatmaps))
						self.execute(cmd, workdir=sumdir)

			# combining TA heatmaps for different SAPs
			heatmaps=glob.glob("%s/ta_heatmap_sap*.png" % (sumdir))
			if len(heatmaps) > 0:
				if len(heatmaps) > 1: cmd="convert %s -append TAheatmap_status.png" % (" ".join(heatmaps))
				else: cmd="mv %s TAheatmap_status.png" % (heatmaps[0])
				self.execute(cmd, workdir=sumdir)
				# remove temporary png files
				cmd="rm -f %s" % (" ".join(heatmaps))
				self.execute(cmd, workdir=sumdir)
				# making a thumbnail version of TA heatmap combined plot
				cmd="convert -scale 200x140-0-0 TAheatmap_status.png TAheatmap_status.th.png"
				self.execute(cmd, workdir=sumdir)

		# creating combined DSPSR plots
		if not cmdline.opts.is_skip_dspsr:
			dspsr_diags=rglob(sumdir, "*_diag.png", 3)
			if len(dspsr_diags) > 0:
				self.log.info("Creating DSPSR summary diagnostic plots...")
				if len(dspsr_diags) > 1: cmd="montage %s -background none -mode concatenate -tile %dx dspsr_status.png" % (" ".join(dspsr_diags), int(math.sqrt(len(dspsr_diags))))
				else: cmd="cp -f %s dspsr_status.png" % (dspsr_diags[0])
				self.execute(cmd, workdir=sumdir)
				# making a thumbnail version of combined DSPSR plot
				cmd="convert -scale 200x140-0-0 dspsr_status.png dspsr_status.th.png"
				self.execute(cmd, workdir=sumdir)

		# creating FE status maps
		# FE combined map if exist - should be called FE_status.png
		if data_code == "CS" and not cmdline.opts.is_nofold and obs.FE:
			self.log.info("Creating FE status maps...")
			cmd="lofar_status_map.py"
			self.execute(cmd, workdir=sumdir)
			femaps=glob.glob("%s/*_core_status.png" % (sumdir))
			femaps.extend(glob.glob("%s/*_remote_status.png" % (sumdir)))
			femaps.extend(glob.glob("%s/*_intl_status.png" % (sumdir)))
			# creating combined plots
			if len(femaps) > 0:
				cmd="convert -append %s FE_status.png" % (" ".join(femaps))
				self.execute(cmd, workdir=sumdir)
				# removing individual maps
				cmd="rm -f %s" % (" ".join(femaps))
				self.execute(cmd, workdir=sumdir)
				# making a thumbnail version of FE status map
				cmd="convert -scale 200x140-0-0 FE_status.png FE_status.th.png"
				self.execute(cmd, workdir=sumdir)

		# Combining different status maps into one 'status.png' to be shown in web-summary page 
		# combining FE maps to general status.png
		if os.path.exists("%s/FE_status.png" % (sumdir)):
			self.log.info("Copying FE status map file to status.png ...")
			cmd="cp -f FE_status.png status.png"
			self.execute(cmd, workdir=sumdir)
			cmd="mv FE_status.th.png status.th.png"
			self.execute(cmd, workdir=sumdir)

		# combining TA heatmaps to general status.png
		if os.path.exists("%s/TAheatmap_status.png" % (sumdir)):
			if os.path.exists("%s/status.png" % (sumdir)): # means that FE maps were created
				self.log.info("Appending TA heatmap map file to status.png ...")
				cmd="montage status.png TAheatmap_status.png -background none -mode concatenate -tile 2x .temp_status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="montage status.th.png TAheatmap_status.th.png -background none -mode concatenate -tile 2x .temp_status.th.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv .temp_status.png status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv .temp_status.th.png status.th.png"
				self.execute(cmd, workdir=sumdir)
				cmd="rm -f TAheatmap_status.th.png"
				self.execute(cmd, workdir=sumdir)
			else:  # if status.png does not exist yet
				self.log.info("Copying TA heatmap map file to status.png ...")
				cmd="cp -f TAheatmap_status.png status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv TAheatmap_status.th.png status.th.png"
				self.execute(cmd, workdir=sumdir)

		# combining dspsr summary plots to general status.png
		if os.path.exists("%s/dspsr_status.png" % (sumdir)):
			if os.path.exists("%s/status.png" % (sumdir)): # means that either FE maps or TA heatmap(s) were created
				self.log.info("Appending dspsr status file to status.png ...")
				cmd="montage status.png dspsr_status.png -background none -mode concatenate -tile 2x .temp_status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="montage status.th.png dspsr_status.th.png -background none -mode concatenate -tile 2x .temp_status.th.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv .temp_status.png status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv .temp_status.th.png status.th.png"
				self.execute(cmd, workdir=sumdir)
				cmd="rm -f dspsr_status.th.png"
				self.execute(cmd, workdir=sumdir)
			else:  # if status.png does not exist yet
				self.log.info("Copying dspsr status file to status.png ...")
				cmd="cp -f dspsr_status.png status.png"
				self.execute(cmd, workdir=sumdir)
				cmd="mv dspsr_status.th.png status.th.png"
				self.execute(cmd, workdir=sumdir)

		# creating thumbnail version of status.png if it exists
		if os.path.exists("%s/status.png" % (sumdir)):		
			self.log.info("Making a thumbnail version of status.png file...")
			cmd="convert -scale 200x140-0-0 status.th.png .temp_status.th.png"
			self.execute(cmd, workdir=sumdir)
			cmd="mv .temp_status.th.png status.th.png"
			self.execute(cmd, workdir=sumdir)
		else:
			self.log.info("No status.png created")
			

		# Make a tarball of all the plots (summary archive)
		self.log.info("Making a final summary tarball of all files with extensions: %s" % (", ".join(self.summary_archive_exts)))
		tarname="%s%s%s%s" % (obs.id, self.summary_archive_prefix, data_code, self.summary_archive_suffix)
		tar_list=[]
		for ext in self.summary_archive_exts:
			ext_list=rglob(sumdir, ext, 3)
			tar_list.extend(ext_list)
		cmd="tar -cv --ignore-failed-read -f %s %s" % (tarname, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
		try: # --ignore-failed-read does not seem to help with tar failing for some beams
                     # like file was changed during the tar, though tarball seem to be fine
			self.execute(cmd, workdir=sumdir)
		except: pass

		# finish
		end_time=time.time()
		total_time= end_time- start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (total_time, total_time/3600.))

		# flushing log file and copy it to summary node
		self.log.flush()
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
