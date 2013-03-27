###################################################################
#
# Class CVUnit - processsing unit for CV data
# and other derivative classes
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
			self.log.exception("Oops... 'run_nodal' function for %s%s has crashed!" % (obs.FE and "FE/" or "", self.code))
			self.kill()
			sys.exit(1)

		# kill all open processes
		self.kill()
		self.procs = []
		# remove reference to PulpLogger class from processing unit
		self.log = None
		# remove references to Popen processes
		self.parent = None

class FE_CVUnit(CVUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		CVUnit.__init__(self, obs, cep2, cmdline, tab, log)
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
