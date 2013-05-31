###################################################################
#
# Class PipeUnitPart - aka Processing unit per Beam when there are
# several frequency splits and procesing is done without using
# hoover nodes; and other derivative classes
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
from pulp_pipeunit import PipeUnit, CSUnit, ISUnit, FE_CSUnit, rglob

class PipeUnitPart(PipeUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
                self.location = node  # processing target node of this bandwidth part
                self.part = part      # bandwidth part index
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)

	# main processing function
	def run(self, obs, cep2, cmdline, log):
		# if there are no pulsars to fold we set --nofold option to True
		if len(self.psrs) == 0 and cmdline.opts.is_nofold == False:
			cmdline.opts.is_nofold = True

		# currently can only process Stokes I
		if self.stokes == "I":
			# if we are not using dspsr directly to read *.h5 files
			if not cmdline.opts.is_with_dal:
				self.run_nodal(obs, cep2, cmdline, log)
			else:
				self.run_dal(obs, cep2, cmdline, log)
		else:   # stub for Stokes IQUV
			self.log = log
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d PART=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, self.part, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# finish
			self.end_time=time.time()
			self.total_time= self.end_time- self.start_time
			self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

	# main run function using dspsr to directly read *.h5 files
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
                                                cmd="dspsr -b %d -A -L %d %s -fft-bench -E %s/%s.par -O %s_%s -t %d %s %s" % \
                                                        (dspsr_nbins, cmdline.opts.tsubint, verbose, self.outdir, psr2, psr, self.output_prefix, \
                                                        cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, s0_file)
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


        # main run function to use 2bf2fits for conversion
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

                                self.log.info("Copying .h5 file to the current directory...")
                                for f in [ff for ff in self.tab.rawfiles[cep2.current_node] if re.search("_P%03d_" % self.part, ff)]:
                                        # forming the input file string
					input_file=f  # should be only one file for a part
                                        # copy *.h5 files
                                        cmd="cp -f %s.h5 ." % (f.split(".raw")[0])
                                        self.execute(cmd, workdir=self.curdir)

                                self.log.info("Input data: %s" % (input_file))

                        self.output_prefix="%s_SAP%d_%s_PART%d" % (obs.id, self.sapid, self.procdir, self.part)
                        self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

                        target_summary_dir="%s_%s/%s%s/%s/SAP%d/%s" % (cep2.processed_dir_prefix, self.summary_node, \
                                cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, self.summary_node_dir_suffix, self.beams_root_dir, self.sapid, self.procdir)

			proc_subs = self.nrSubsPerFile
                        if self.nrSubsPerFile * (self.part + 1) > self.tab.nrSubbands:
                                proc_subs -= (self.nrSubsPerFile * (self.part + 1) - self.tab.nrSubbands)
                        nsubs_eff = min(self.tab.nrSubbands, proc_subs)
                        total_chan = nsubs_eff * self.nrChanPerSub

                        # if we run the whole processing and not just plots
                        if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:

                                # running data conversion (2bf2fits)
                                if not cmdline.opts.is_nodecode:
                                        verbose=""
                                        if cmdline.opts.is_debug: verbose="-v"
                                        cmd="2bf2fits %s %s -parset %s -append -nbits 8 -A %d -sigma %d -nsubs %d -sap %d -tab %d -part %d -o %s %s %s" % (verbose, self.raw2fits_extra_options, obs.parset, cmdline.opts.decode_nblocks, cmdline.opts.decode_sigma, nsubs_eff, self.sapid, self.tabid, self.part, self.output_prefix, cmdline.opts.bf2fits_extra_opts, input_file)
                                        self.execute(cmd, workdir=self.curdir)
                                        # fixing the coordinates
                                        #cmd="fix_fits_coordinates.py %s.fits" % (self.output_prefix)
                                        #self.execute(cmd, workdir=self.curdir)

                                # running RFI excision, checking
                                if not cmdline.opts.is_norfi:
                                        zapstr=""
                                        # we should zap 1st chan as after 2nd polyphase it has junk
                                        if self.nrChanPerSub > 1:
                                                zapstr="-zapchan 0:%d:%d" % (total_chan-1, self.nrChanPerSub)
                                        self.log.info("Creating RFI mask...")
                                        cmd="rfifind -o %s -psrfits -noclip -blocks 16 %s %s.fits" % (self.output_prefix, zapstr, self.output_prefix)
                                        rfifind_popen = self.start_and_go(cmd, workdir=self.curdir)

                                        # start subdyn.py
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
                                                prepfold_popens = self.start_prepfold(cmdline, total_chan)

                                        # now running dspsr stuff...
                                        if not cmdline.opts.is_skip_dspsr:

                                                verbose="-q"
                                                if cmdline.opts.is_debug: verbose="-v"
                                                dspsr_popens=[] # list of dspsr Popen objects
                                                for psr in self.psrs: # pulsar list is empty if --nofold is used
                                                        psr2=re.sub(r'[BJ]', '', psr)
                                                        dspsr_nbins=self.get_best_nbins("%s/%s.par" % (self.outdir, psr2))
                                                        cmd="dspsr -b %d -A -L %d -E %s/%s.par %s -fft-bench -O %s_%s -t %d %s %s.fits" % \
                                                                (dspsr_nbins, cmdline.opts.tsubint, self.outdir, psr2, verbose, psr, \
                                                                self.output_prefix, cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, self.output_prefix)
                                                        dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
                                                        dspsr_popens.append(dspsr_popen)

                                                # waiting for dspsr to finish
                                                self.waiting_list("dspsr", dspsr_popens)

                                # waiting for prepfold to finish
                                try:
                                        if not cmdline.opts.is_nofold and not cmdline.opts.is_skip_prepfold:
                                                self.waiting_list("prepfold", prepfold_popens)
                                # we let PULP to continue if prepfold has crashed, as dspsr can run ok, or other instances of prepfold could finish ok as well
                                except: pass

                        # if we want to run prepdata to create a time-series and make a list of TOAs
                        try:
                                if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and cmdline.opts.is_single_pulse:
                                        self.run_prepdata(cmdline)
                        # we let PULP to continue if prepdata (or single_pulse_search.py) has crashed, as the rest of the pipeline can finish ok
                        except: pass

                        # making prepfold diagnostic plots
                        if not cmdline.opts.is_nofold and not cmdline.opts.is_feedback:
                                self.make_prepfold_plots(obs)

                        # Running "RRATs" analysis (prepsubband + single_pulse_search.py for a range of DMs)
                        try:
                                if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and cmdline.opts.is_rrats:
                                        self.run_rrats_analysis(cmdline)
                        except: pass

                        # waiting for subdyn to finish
                        if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback and \
                        not cmdline.opts.is_norfi and not cmdline.opts.is_skip_subdyn:
                                self.waiting("subdyn.py", subdyn_popen)

			if not cmdline.opts.is_plots_only and not cmdline.opts.is_feedback:

                                # creating beam directory on summary node (where to copy ar-files)
                                self.log.info("Creating directory %s on summary node %s..." % (target_summary_dir, self.summary_node))
                                cmd="ssh -t %s 'mkdir -m 775 -p %s'" % (self.summary_node, target_summary_dir)
                                p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
                                p.communicate()

                                # rsync'ing all relevant files to the summary node to be combined and further processed
                                # also copy par-file(s) to have it/them in the summary directory
                                verbose=""
                                if cmdline.opts.is_debug: verbose="-v"
                                self.log.info("Rsync'ing all relevant files to %s:%s" % (self.summary_node, target_summary_dir))
				copy_list=[]
                        	for ext in self.full_archive_exts:
                                	ext_list=rglob(self.curdir, ext, 3)
					copy_list.extend(ext_list)
                     	   	copy_list.extend(glob.glob("%s/*.par" % (self.outdir)))
                                cmd="rsync %s -axP %s %s:%s" % (verbose, " ".join(copy_list), self.summary_node, target_summary_dir)
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


class CSUnitPart(PipeUnitPart, CSUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log, node, part)
                CSUnit.__init__(self, obs, cep2, cmdline, tab, log)

class ISUnitPart(PipeUnitPart, ISUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log, node, part)
                ISUnit.__init__(self, obs, cep2, cmdline, tab, log)

class FE_CSUnitPart(PipeUnitPart, FE_CSUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log, node, part)
                FE_CSUnit.__init__(self, obs, cep2, cmdline, tab, log)
