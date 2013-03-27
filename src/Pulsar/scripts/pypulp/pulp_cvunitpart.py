###################################################################
#
# Class CVUnitPart - processsing unit for CV data when there are
# several frequency splits and procesing is done without using
# hoover nodes
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
from pulp_pipeunit import PipeUnit
from pulp_cvunit import CVUnit

# class for processing of part of BW for CV data
class CVUnitPart(CVUnit):
	def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		self.location = node  # processing target node of this bandwidth part
		self.part = part      # bandwidth part index
		CVUnit.__init__(self, obs, cep2, cmdline, tab, log)

	# main run function using dspsr to read directly *.h5 files
	def run_dal(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d PART=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, self.part, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
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
				# first we make links to *.h5 and *.raw files in the current directory for the target node

				# make a soft links in the current dir (in order for processing to be consistent with the case when data are in many nodes)
				self.log.info("Making links to input files in the current directory...")
				for f in [ff for ff in self.tab.rawfiles[cep2.current_node] if re.search("_P%03d_" % self.part, ff)]:
					# links to the *.raw files
					cmd="ln -sf %s ." % (f)
					self.execute(cmd, workdir=self.curdir)
					# copy *.h5 files
					cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
					self.execute(cmd, workdir=self.curdir)

				# second, we need to rsync other files for this part from other locus nodes
				# forming the list of dependent locus nodes that have the rest of the data for this part
				dependent_nodes=[key for key in self.tab.rawfiles.keys() if any("_P%03d_" % self.part in ff for ff in self.tab.rawfiles[key])]
				if cep2.current_node in dependent_nodes: dependent_nodes.remove(cep2.current_node)

				# rsyncing the rest of the data
				self.log.info("Rsync'ing other *.h5 and *.raw files from other locus nodes to the current directory...")
		                verbose=""
                		if cmdline.opts.is_debug: verbose="-v"

				for node in dependent_nodes:
					# get the list of necessary *.raw files to copy from this node
					target_files = [ff for ff in self.tab.rawfiles[node] if re.search("_P%03d_" % self.part, ff)]
					h5_files = ["%s.h5" % (ff.split(".raw")[0]) for ff in target_files]
					target_files.extend(h5_files)
					self.log.info("Node: %s   Files: %s" % (node, ", ".join(target_files)))
					cmd="rsync %s -axP %s:%s ." % (verbose, node, " :".join(target_files))
					self.execute(cmd, workdir=self.curdir)

				input_files=[ff.split("/")[-1] for ff in glob.glob("%s/*.h5" % (self.curdir))]
				self.log.info("Input data: %s" % ("\n".join(input_files)))

			self.output_prefix="%s_SAP%d_%s_PART%d" % (obs.id, self.sapid, self.procdir, self.part)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			if self.nrSubsPerFile * (self.part + 1) > self.tab.nrSubbands:
				nsubs_eff = self.tab.nrSubbands - self.part * self.nrSubsPerFile
			else: nsubs_eff = self.nrSubsPerFile
			total_chan = nsubs_eff * self.nrChanPerSub

			target_summary_dir="%s_%s/%s%s/%s/SAP%d/%s" % (cep2.processed_dir_prefix, self.summary_node, \
				cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, self.summary_node_dir_suffix, self.beams_root_dir, self.sapid, self.procdir)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_nofold and not cmdline.opts.is_nodecode:
					# getting the list of "_S0_" files, the number of which is how many freq splits we have
					# we also sort this list by split number
					s0_file=[f for f in input_files if re.search("_S0_", f) is not None][0]
					verbose="-q"
					if cmdline.opts.is_debug: verbose="-v"

					# running dspsr for every pulsar for this frequency split
					self.log.info("Running dspsr for the frequency split %d ..." % self.part)
					# loop on pulsars
					for psr in self.psrs:
						self.log.info("Running dspsr for pulsar %s..." % (psr))
						psr2=re.sub(r'[BJ]', '', psr)
						dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
						cmd="dspsr -b %d -A -L %d %s -fft-bench -E %s/%s.par -O %s_%s -t %d -U minX%d %s %s" % \
							(dspsr_nbins, cmdline.opts.tsubint, verbose, self.outdir, psr2, psr, self.output_prefix, \
							cmdline.opts.nthreads, cmdline.opts.maxram, cmdline.opts.dspsr_extra_opts, s0_file)
						self.execute(cmd, workdir=self.curdir)

					# removing input .raw files (links or rsynced)
					if not cmdline.opts.is_debug:
						cmd="rm -f %s" % (" ".join(["%s.raw" % (f.split(".h5")[0]) for f in input_files]))
						self.execute(cmd, workdir=self.curdir)

				# creating beam directory on summary node (where to copy ar-files)
				self.log.info("Creating directory %s on summary node %s..." % (target_summary_dir, self.summary_node))
				cmd="ssh -t %s 'mkdir -m 775 -p %s'" % (self.summary_node, target_summary_dir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()

				# rsync'ing the ar-files and h5-files to the summary node to be combined and further processed
				# also copy par-file(s) to have it/them in the summary directory
				verbose=""
				if cmdline.opts.is_debug: verbose="-v"
				self.log.info("Rsync'ing *.ar files and *.h5 files to %s:%s" % (self.summary_node, target_summary_dir))
				ar_list=[]
				for psr in self.psrs:
					ar_list.append("%s/%s_%s.ar" % (self.curdir, psr, self.output_prefix))
				# making list of h5 files
				h5_list = glob.glob("%s/*.h5" % (self.curdir))
				# also making list of par-files
				par_list = glob.glob("%s/*.par" % (self.outdir))
				cmd="rsync %s -axP %s %s %s %s:%s" % (verbose, " ".join(ar_list), " ".join(h5_list), " ".join(par_list), self.summary_node, target_summary_dir)
				self.execute(cmd, workdir=self.curdir)

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
			verbose=""
			if cmdline.opts.is_debug: verbose="-v"
			cmd="rsync %s -axP %s %s:%s" % (verbose, cep2.get_logfile(), self.summary_node, target_summary_dir)
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=self.outdir)
			proc.communicate()

			# changing the file permissions to be re-writable for group
			cmd="chmod -R g+w %s" % (self.outdir)
			os.system(cmd)

		except Exception:
			self.log.exception("Oops... 'run_dal' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
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
			self.log.info("%s SAP=%d TAB=%d PART=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, self.part, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
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
				# first we make links to *.h5 and *.raw files in the current directory for the target node

				# make a soft links in the current dir (in order for processing to be consistent with the case when data are in many nodes)
				self.log.info("Making links to input files in the current directory...")
				for f in [ff for ff in self.tab.rawfiles[cep2.current_node] if re.search("_P%03d_" % self.part, ff)]:
					# links to the *.raw files
					cmd="ln -sf %s ." % (f)
					self.execute(cmd, workdir=self.curdir)
					# copy *.h5 files
					cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
					self.execute(cmd, workdir=self.curdir)

				# second, we need to rsync other files for this part from other locus nodes
				# forming the list of dependent locus nodes that have the rest of the data for this part
				dependent_nodes=[key for key in self.tab.rawfiles.keys() if any("_P%03d_" % self.part in ff for ff in self.tab.rawfiles[key])]
				if cep2.current_node in dependent_nodes: dependent_nodes.remove(cep2.current_node)

				# rsyncing the rest of the data
				self.log.info("Rsync'ing other *.h5 and *.raw files from other locus nodes to the current directory...")
		                verbose=""
                		if cmdline.opts.is_debug: verbose="-v"

				for node in dependent_nodes:
					# get the list of necessary *.raw files to copy from this node
					target_files = [ff for ff in self.tab.rawfiles[node] if re.search("_P%03d_" % self.part, ff)]
					h5_files = ["%s.h5" % (ff.split(".raw")[0]) for ff in target_files]
					target_files.extend(h5_files)
					self.log.info("Node: %s   Files: %s" % (node, ", ".join(target_files)))
					cmd="rsync %s -axP %s:%s ." % (verbose, node, " :".join(target_files))
					self.execute(cmd, workdir=self.curdir)

				input_files=[ff.split("/")[-1] for ff in glob.glob("%s/*.raw" % (self.curdir))]
				self.log.info("Input data: %s" % ("\n".join(input_files)))

			self.output_prefix="%s_SAP%d_%s_PART%d" % (obs.id, self.sapid, self.procdir, self.part)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			if self.nrSubsPerFile * (self.part + 1) > self.tab.nrSubbands:
				nsubs_eff = self.tab.nrSubbands - self.part * self.nrSubsPerFile
			else: nsubs_eff = self.nrSubsPerFile
			total_chan = nsubs_eff * self.nrChanPerSub

			target_summary_dir="%s_%s/%s%s/%s/SAP%d/%s" % (cep2.processed_dir_prefix, self.summary_node, 
				cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, self.summary_node_dir_suffix, self.beams_root_dir, self.sapid, self.procdir)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:
				if not cmdline.opts.is_nodecode:
					# getting the list of "_S0_" files, the number of which is how many freq splits we have
					# we also sort this list by split number
					s0_file=[f for f in input_files if re.search("_S0_", f) is not None][0]
					# checking if extra options for complex voltages processing were set in the pipeline
					nblocks=""
					if cmdline.opts.nblocks != -1: nblocks = "-b %d" % (cmdline.opts.nblocks)
					is_all_for_scaling=""
					if cmdline.opts.is_all_times: is_all_for_scaling="-all_times"
					is_write_ascii=""
					if cmdline.opts.is_write_ascii: is_write_ascii="-t"
					verbose=""
					if cmdline.opts.is_debug: verbose="-v"

					# running bf2puma2 command for this frequency split
					self.log.info("Running bf2puma2 for the frequency split %d ..." % self.part)
					cmd="bf2puma2 -f %s -h %s -p %s -hist_cutoff %f %s %s %s %s" % (s0_file, cep2.puma2header, \
						obs.parset, cmdline.opts.hist_cutoff, verbose, nblocks, is_all_for_scaling, is_write_ascii)
					self.execute(cmd, workdir=self.curdir)

				# removing input .raw files (links or rsynced)
				if not cmdline.opts.is_debug:
					cmd="rm -f %s" % (" ".join(input_files))
					self.execute(cmd, workdir=self.curdir)

				if not cmdline.opts.is_nofold:
					# getting the MJD of the observation
					# for some reason dspsr gets wrong MJD without using -m option
					input_prefix=s0_file.split("_S0_")[0]
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
					self.log.info("Running dspsr for each frequency channel for the frequency split %d ..." % self.part)
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

							# waiting for dspsr to finish
							self.waiting_list("dspsr", dspsr_popens)

						# running psradd to add all freq channels together for this frequency split
						self.log.info("Adding frequency channels together for the frequency split %d ..." % self.part)
						ar_files=glob.glob("%s/%s_%s_SB*_CH*.ar" % (self.curdir, psr, self.output_prefix))
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

				# creating beam directory on summary node (where to copy ar-files)
				self.log.info("Creating directory %s on summary node %s..." % (target_summary_dir, self.summary_node))
				cmd="ssh -t %s 'mkdir -m 775 -p %s'" % (self.summary_node, target_summary_dir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()

				# rsync'ing the ar-files and h5-files to the summary node to be combined and further processed
				# also copy par-file(s) to have it/them in the summary directory
				verbose=""
				if cmdline.opts.is_debug: verbose="-v"
				self.log.info("Rsync'ing *.ar files and *.h5 files to %s:%s" % (self.summary_node, target_summary_dir))
				ar_list=[]
				for psr in self.psrs:
					ar_list.append("%s/%s_%s.ar" % (self.curdir, psr, self.output_prefix))
				# making list of h5 files
				h5_list = glob.glob("%s/*.h5" % (self.curdir))
				# also making list of par-files
				par_list = glob.glob("%s/*.par" % (self.outdir))
				cmd="rsync %s -axP %s %s %s %s:%s" % (verbose, " ".join(ar_list), " ".join(h5_list), " ".join(par_list), self.summary_node, target_summary_dir)
				self.execute(cmd, workdir=self.curdir)

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
			verbose=""
			if cmdline.opts.is_debug: verbose="-v"
			cmd="rsync %s -axP %s %s:%s" % (verbose, cep2.get_logfile(), self.summary_node, target_summary_dir)
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=self.outdir)
			proc.communicate()

			# changing the file permissions to be re-writable for group
			cmd="chmod -R g+w %s" % (self.outdir)
			os.system(cmd)

		except Exception:
			self.log.exception("Oops... 'run_dal' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None
