###################################################################
#
# Class Pipeline - main processing class
# Other class, aka Processing Units (per Beam) are defined
#

import os, sys, glob, time, re, os.path
import numpy as np
import logging
import subprocess, shlex
from subprocess import PIPE, STDOUT, Popen
from pulp_parset import Observation
from pulp_usercmd import CMDLine
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger


# The main processing class
class Pipeline:
	def __init__(self, obs, log):
		self.units = []   # list of processing units

		for sap in obs.saps:
			for tab in sap.tabs:
				if not tab.is_coherent:
					unit = ISUnit(tab)
				if tab.is_coherent and tab.specificationType != "flyseye":
					if obs.CS:
						unit = CSUnit(tab)
					elif obs.CV:
						unit = CVUnit(tab)
					else:
						log.error("Can't initialize processing pipeline unit for SAP=%d TAB=%d" % (sap.sapid, tab.tabid))
						sys.exit(1)
				if tab.is_coherent and tab.specificationType == "flyseye":
					if obs.CS:
						unit = FE_CSUnit(tab)
					elif obs.CV:
						unit = FE_CVUnit(tab)
					else:
						log.error("Can't initialize processing pipeline FE unit for SAP=%d TAB=%d" % (sap.sapid, tab.tabid))
						sys.exit(1)

				# adding unit to the list
				self.units.append(unit)

	# kicking off the pipeline
	def start(self, cep2, cmdline, log):
		# and here we start...
		log.info("Starting PULP processing for:")
		toremove = set()
		# check first the user flag if we only want to process CS or IS data
		# and remove those processing units that we will not process from the list
		for uid in range(len(self.units)):
			if cmdline.opts.is_coh_only == True and not self.units[uid].tab.is_coherent: toremove.add(uid)
			if cmdline.opts.is_incoh_only == True and self.units[uid].tab.is_coherent: toremove.add(uid)
		toremove = sorted(toremove, reverse=True)
		for uid in toremove: del(self.units[uid])

		# creating main output directory on locus092 for CS data and on locus094 for IS data
		for unit in self.units:
			if unit.tab.is_coherent:
				locus="locus092"
				output_dir="%s_%s/%s_CSplots" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir)
				log.info("Creating output directory on %s: %s" % (locus, output_dir))
				cmd="%s %s 'mkdir -p %s'" % (cep2.cexeccmd, cep2.cexec_nodes[locus], output_dir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()
				break

		for unit in self.units:
			if not unit.tab.is_coherent:
				locus="locus094"
				output_dir="%s_%s/%s_redIS" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir)
				log.info("Creating output directory on %s: %s" % (locus, output_dir))
				cmd="%s %s 'mkdir -p %s'" % (cep2.cexeccmd, cep2.cexec_nodes[locus], output_dir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()
				break

		for unit in self.units:
			# if data for this beam are on several nodes, then we have to log in to hoover node...
			if len(unit.tab.location) > 1:
				if unit.tab.is_coherent: locus="locus101" # we choose one hoover node for CS data
				else: locus="locus102"                    # and another for IS data
			else:
				locus=unit.tab.location[0]
			cmd="%s %s '/home/kondratiev/pulp/pulp.py --noinit --beam %d:%d %s'" %  \
				(cep2.cexeccmd, cep2.cexec_nodes[locus], unit.sapid, unit.tabid, " ".join(cmdline.options))
			unit.parent = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			log.info("SAP=%d TAB=%d (%s) on %s (pid=%d)  [number of locations = %d]" % \
				(unit.sapid, unit.tabid, unit.tab.is_coherent and "CS" or "IS", locus, unit.parent.pid, len(unit.tab.location)))


	# here we finish the pipeline, creating extra files, convert ps(pdf) to png, etc...
	# create FE status maps, TA heat maps...
	def finish(self, log):
		run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
		log.info("Still running [%d]: %s" % (len(run_units), run_units))
		for unit in self.units:
			log.info("waiting...")
			unit.parent.communicate()
			log.info("Process pid=%d has finished, status=%d" % (unit.parent.pid, unit.parent.returncode))
			run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
			if len(run_units) > 0: log.info("Still running [%d]: %s" % (len(run_units), run_units))

		# combining all *.th.png files into one (for summary)
#		self.log.info("Combining all th.png files into one...")
#		cmd="thumbnail_combine.sh"
#		self.execute(cmd, workdir=self.outdir)

# base class for the single processing (a-ka beam)
class PipeUnit:
	def __init__(self, tab):
		self.sapid = tab.parent_sapid
		self.tabid = tab.tabid
		self.tab = tab
		self.parent = None   # parent Popen project
		self.process = None  # own process, process.pid - pid, process.returncode - status
                                     # if returncode == None, it means that process is still running
		self.outdir = ""     # root directory with processed data
		self.curdir = ""     # current processing directory
		self.log = None
		self.start_time = 0  # start time of the processing (in s)
		self.end_time = 0    # end time (in s)
		self.total_time = 0  # total time in s 
		# extensions of the files to copy to archive
		self.extensions=["*.pdf", "*.ps", "*.pfd", "*.inf", "*.rfirep", "*png", "*parset", "*.par", "*.ar", "*.AR", "*pdmp*"]

	def execute(self, cmd, workdir=None, shell=False, is_os=False):
	    	"""
        	Execute the command 'cmd' after logging the command
            	and the wall-clock amount of time the command took to execute.
		This function waits for process to finish
    		"""
		try:
			self.log.info(cmd)
    			job_start = time.time()
			self.log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
			if is_os: status = os.system(cmd)
			else:
               			self.process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
       	        		self.log.process2log(self.process)
        	       		self.process.communicate()
			job_end = time.time()
			job_total_time = job_end - job_start
			if is_os:
        	       		self.log.info("Finished at UTC %s, status=%s, Total runnung time: %.1f s (%.2f hrs)" % \
						(time.asctime(time.gmtime()), status, job_total_time, job_total_time/3600.))
			else:
				status=self.process.poll()
        	       		self.log.info("Finished at UTC %s, status=%s, Total runnung time: %.1f s (%.2f hrs)" % \
						(time.asctime(time.gmtime()), status, job_total_time, job_total_time/3600.))
			self.log.info("")
			# if job is not successful
			if status != 0:
				raise Exception
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (cmd, status))
			sys.exit(1)

	def start_and_go(self, cmd, workdir=None, shell=False):
	    	"""
        	Execute the command 'cmd' after logging the command
		This function start the cmd and leaves the fucntion
		returning the Popen object, it does not wait for process to finish
    		"""
		try:
			self.log.info(cmd)
			self.log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
               		self.process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
			self.log.info("Job pid=%d, not waiting for it to finish." % (self.process.pid))
			return self.process
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (cmd, self.process.returncode))
			sys.exit(1)

	def waiting(self, prg, popen):
		"""
		Waiting for process to finish
		"""
		try:
			self.log.info("Waiting for %s to finish, pid=%d" % (prg, popen.pid))
			popen.communicate()
			self.log.info("Process pid=%d (%s) has finished, status=%d" % (popen.pid, prg, popen.returncode))
			self.log.info("")
			# if job is not successful
			if popen.poll() != 0:
				raise Exception
		except Exception:
			self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, popen.pid, popen.returncode))
			sys.exit(1)

	def waiting_list(self, prg, popen_list):
		"""
		Waiting for list of processes to finish
		"""
		try:
			self.log.info("Waiting for %s processes to finish..." % (prg))
			run_units = [u.pid for u in popen_list if u.poll() is None]
			self.log.info("Still running [%d]: %s" % (len(run_units), run_units))
			for unit in popen_list:
				self.waiting(prg, unit)
				run_units = [u.pid for u in popen_list if u.poll() is None]
				finished_units = [u for u in popen_list if u.poll() is not None]
				for fu in finished_units:
					if fu.poll() != 0:
						raise Exception
				if len(run_units) > 0: self.log.info("Still running [%d]: %s" % (len(run_units), run_units))
		except Exception:
			fu = [u for u in popen_list if u.poll() is not None][0]
			self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, fu.pid, fu.returncode))
			sys.exit(1)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		return

	def lcd(self, low, high):
		"""
		Calculates the lowest common denominator of 'high' value between 'low' and 'high'
		"""
		for ii in range(low, high+1): 
			if high % ii == 0: return ii
		return high

	# return list of dirs in the current base directory
	def _getDirs(self, base):
		return [x for x in glob.iglob(os.path.join(base, '*')) if os.path.isdir(x)]

	# recursive glob
	def rglob(self, base, pattern):
		flist=[]
		flist.extend(glob.glob(os.path.join(base, pattern)))
		dirs = self._getDirs(base)
		if len(dirs):
			for d in dirs:
				flist.extend(self.rglob(os.path.join(base, d), pattern))
		return flist

class CSUnit(PipeUnit):
	def __init__(self, tab):
		PipeUnit.__init__(self, tab)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (CS)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, time.asctime(time.gmtime()), cep2.current_node))

		# if user specified output dir (relative to /data/LOFAR_PULSAR_....)
		if cmdline.opts.outdir != "":
			self.outdir = "%s_%s/%s" % (cep2.processed_dir_prefix, cep2.current_node, cmdline.opts.outdir)
		else: # if output dir was not set
			self.outdir = "%s_%s/%s_red" % (cep2.processed_dir_prefix, cep2.current_node, obs.id)
		# deleting previous results if option --del was set
		if cmdline.opts.is_delete:
			self.log.info("Deleting previous processed results in %s" % (self.outdir))
			cmd="rm -rf %s" % (self.outdir)
			self.execute(cmd)
		# Re-creating root output directory
		cmd="mkdir -p %s" % (self.outdir)
		self.execute(cmd)

		# creating Par-file in the output directory or copying existing one
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			psr2=re.sub(r'[BJ]', '', psr)
			parfile="%s/%s.par" % (cep2.parfile_dir, psr2)
			if os.path.exists(parfile):
				cmd="cp -f %s %s" % (parfile, self.outdir)
				self.execute(cmd)
				continue
			parfile="%s/%s.par" % (cep2.parfile_dir, psr)
			if os.path.exists(parfile):
				cmd="cp -f %s %s/%s.par" % (parfile, self.outdir, psr2)
				self.execute(cmd)
				continue
			cmd="psrcat -db_file %s -e %s > %s/%s.par" % (cep2.psrcatdb, psr2, self.outdir, psr2)
			self.execute(cmd, is_os=True)

		# Creating output dir
		self.curdir = "%s/stokes/BEAM%d" % (self.outdir, self.tabid)
		cmd="mkdir -p %s" % (self.curdir)
		self.execute(cmd)

		if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
                                          # also we need to moint locus nodes that we need
			self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
			input_files=[]
			for loc in self.tab.location:
				# first "mounting" corresponding locus node
				input_dir="%s/%s_data/%d" % (cep2.hoover_data_dir, loc, obs.id)
				self.process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
				self.process.communicate()
				input_file="%s/%s_SAP%03d_B%03d_S*_bf.raw" % (input_dir, obs.id, self.sapid, self.tabid)
				input_files.extend(glob.glob(input_file))
			input_file=" ".join(input_files)
		else:
			input_file=glob.glob("/data/%s/%s_SAP%03d_B%03d_S*_bf.raw" % (obs.id, obs.id, self.sapid, self.tabid))[0]
		self.log.info("Input data: %s" % (input_file))
		self.output_prefix="%s_BEAM%d" % (obs.id, self.tabid)
		self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

		# running data conversion (2bf2fits)
		cmd="2bf2fits -CS -H -parset %s -append -nbits 8 -A 100 -sigma 3 -v -nsubs %d -o %s %s" % (obs.parset, self.tab.nrSubbands, self.output_prefix, input_file)
		self.execute(cmd, workdir=self.curdir)

		# running RFI excision, checking
		total_chan = obs.nrSubbands*obs.nrChanPerSubCS
		if not cmdline.opts.is_norfi:
			zapstr=""
			# we should zap 1st chan as after 2nd polyphase it has junk
			if obs.nrChanPerSubCS > 1:
				zapstr="-zapchan 0:%d:%d" % (total_chan-1, obs.nrChanPerSubCS)
			self.log.info("Creating RFI mask...")
			cmd="rfifind -o %s -psrfits -noclip -blocks 16 %s %s.fits" % (self.output_prefix, zapstr, self.output_prefix)
			rfifind_popen = self.start_and_go(cmd, workdir=self.curdir)
			self.log.info("Producing RFI report...")
			samples_to_average=int(10000. / obs.samplingCS) # 10 s worth of data
			cmd="python %s/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n %d %s.fits" % (cep2.lofarsoft, samples_to_average, self.output_prefix)
			subdyn_popen = self.start_and_go(cmd, workdir=self.curdir)

			# waiting for rfifind to finish
			self.waiting("rfifind", rfifind_popen)

		# running prepfold with and without mask
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
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			# first running prepfold with mask (if --norfi was not set)
			if not cmdline.opts.is_norfi:
				cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -mask %s_rfifind.mask -o %s_%s %s.fits" % \
					(psr, prepfold_nsubs, self.output_prefix, psr, self.output_prefix, self.output_prefix)
				prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
				prepfold_popens.append(prepfold_popen)
			# running prepfold without mask
			if not cmdline.opts.is_norfi: output_stem="_nomask"
			else: output_stem=""
			cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -o %s_%s%s %s.fits" % \
				(psr, prepfold_nsubs, psr, self.output_prefix, output_stem, self.output_prefix)
			prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
			prepfold_popens.append(prepfold_popen)

		# now running dspsr stuff...
		zapstr=""
		if obs.nrChanPerSubCS > 1:
			zapstr="-j 'zap chan %s'" % (" ".join([str(ii) for ii in range(0, total_chan, obs.nrChanPerSubCS)]))
		dspsr_popens=[] # list of dspsr Popen objects
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			psr2=re.sub(r'[BJ]', '', psr)
			cmd="dspsr -E %s/%s.par %s -q -b 256 -fft-bench -O %s_%s -K -A -L 60 %s.fits" % \
				(self.outdir, psr2, zapstr, psr, self.output_prefix, self.output_prefix)
			dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
			dspsr_popens.append(dspsr_popen)

		# waiting for dspsr to finish
		self.waiting_list("dspsr", dspsr_popens)

		# running extra Psrchive programs, pam, pav,pdmp, etc... 
		# these programs should be run quick, so run them one by one

		# first, calculating the proper min divisir for the number of subbands
		self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (obs.nrChanPerSubCS, obs.nrSubbands))
		# calculating the least common denominator of obs.nrSubbands starting from obs.nrChanPerSubCS
		pav_nchans = self.lcd(obs.nrChanPerSubCS, obs.nrSubbands)
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			cmd="pam --setnchn %d -m %s_%s.ar" % (obs.nrSubbands, psr, self.output_prefix)
			self.execute(cmd, workdir=self.curdir)
			cmd="dspsr_ar_plots.sh %s_%s %d" % (psr, self.output_prefix, pav_nchans)
			self.execute(cmd, workdir=self.curdir)

		# now running pdmp without waiting...
		if not cmdline.opts.is_nopdmp:
			self.log.info("Running pdmp...")
			pdmp_popens=[]  # list of pdmp Popen objects	
			for psr in cmdline.psrs:
				if psr == "NONE": continue
				cmd="pdmp -mc %d -mb 128 -g %s_%s_pdmp.ps/cps %s_%s.ar" % \
					(obs.nrSubbands, psr, self.output_prefix, psr, self.output_prefix)
				pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
				pdmp_popens.append(pdmp_popen)
		
		# waiting for prepfold to finish
		self.waiting_list("prepfold", prepfold_popens)

		# running convert on prepfold ps to pdf and png
		if cmdline.psrs[0] != "NONE":
			self.log.info("Running convert on ps to pdf and png of the plots...")
			prepfold_ps=glob.glob("%s/*.pfd.ps" % (self.curdir))
			for psfile in prepfold_ps:
				base=psfile.split("/")[-1].split(".ps")[0]
				cmd="convert %s.ps %s.pdf" % (base, base)
				self.execute(cmd, workdir=self.curdir)
				cmd="convert -rotate 90 %s.ps %s.png" % (base, base)
				self.execute(cmd, workdir=self.curdir)
				cmd="convert -rotate 90 -crop 200x140-0 %s.ps %s.th.png" % (base, base)
				self.execute(cmd, workdir=self.curdir)

		# waiting for subdyn to finish
		self.waiting("subdyn.py", subdyn_popen)

		# Here, I should create RFI summary files, for the case of 1 and several beams...
		self.log.info("Creating RFI Summary files... (not implemented yet)")

		# waiting for pdmp to finish
		self.waiting_list("pdmp", pdmp_popens)
		# when pdmp is finished do extra actions with files...
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
			self.execute(cmd, is_os=True)
			cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
			self.execute(cmd, is_os=True)
			# reading new DM from the *.per file
			newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
			cmd="pam -e AR -d %f -DTp %s_%s.ar" % (newdm, psr, self.output_prefix)
			self.execute(cmd, workdir=self.curdir)

		# copying parset file to output directory
		self.log.info("Copying original parset file to output directory...")
		cmd="cp -f %s %s" % (obs.parset, self.outdir)
		self.execute(cmd, workdir=self.outdir)
		# Make a tarball of all the plots
		self.log.info("Make a tarball of all the plots...")
		tar_list=[]
		for ext in self.extensions:
			ext_list=self.rglob(self.outdir, ext)
			tar_list.extend(ext_list)
		cmd="tar cvfz %s_plotsCS.tar.gz %s" % (obs.id, " ".join([f.split(self.outdir+"/")[1] for f in tar_list]))
		self.execute(cmd, workdir=self.outdir)

		# copying archive file to locus092
		locus="locus092"
		output_dir="%s_%s/%s_CSplots" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir)
		output_archive="%s/%s_sap%03d_tab%04d_plotsCS.tar.gz" % (output_dir, obs.id, self.sapid, self.tabid)
		self.log.info("Copying archive file to %s:%s" % (locus, output_dir))
		cmd="rsync -avxP %s_plotsCS.tar.gz %s:%s" % (obs.id, locus, output_archive)
		self.execute(cmd, workdir=self.outdir)

		# combining all *.th.png files into one (for summary)
		self.log.info("Combining all th.png files into one...")
		cmd="thumbnail_combine.sh"
		self.execute(cmd, workdir=self.outdir)
			
		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

		# flushing log file and copy it to outdir on local node and locus092
		self.log.flush()
		cmd="cp -f %s %s" % (cep2.get_logfile(), self.outdir)
		os.system(cmd)
		cmd="rsync -avxP %s %s:%s" % (cep2.get_logfile(), locus, output_dir)
		os.system(cmd)

class ISUnit(PipeUnit):
	def __init__(self, tab):
		PipeUnit.__init__(self, tab)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (IS)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, time.asctime(time.gmtime()), cep2.current_node))

		# if user specified output dir (relative to /data/LOFAR_PULSAR_....)
		if cmdline.opts.outdir != "":
			self.outdir = "%s_%s/%s" % (cep2.processed_dir_prefix, cep2.current_node, cmdline.opts.outdir)
		else: # if output dir was not set
			self.outdir = "%s_%s/%s_redIS" % (cep2.processed_dir_prefix, cep2.current_node, obs.id)
		# deleting previous results if option --del was set
		if cmdline.opts.is_delete:
			self.log.info("Deleting previous processed results in %s" % (self.outdir))
			cmd="rm -rf %s" % (self.outdir)
			self.execute(cmd)
		# Re-creating root output directory
		cmd="mkdir -p %s" % (self.outdir)
		self.execute(cmd)

		# creating Par-file in the output directory or copying existing one
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			psr2=re.sub(r'[BJ]', '', psr)
			parfile="%s/%s.par" % (cep2.parfile_dir, psr2)
			if os.path.exists(parfile):
				cmd="cp -f %s %s" % (parfile, self.outdir)
				self.execute(cmd)
				continue
			parfile="%s/%s.par" % (cep2.parfile_dir, psr)
			if os.path.exists(parfile):
				cmd="cp -f %s %s/%s.par" % (parfile, self.outdir, psr2)
				self.execute(cmd)
				continue
			cmd="psrcat -db_file %s -e %s > %s/%s.par" % (cep2.psrcatdb, psr2, self.outdir, psr2)
			self.execute(cmd, is_os=True)

		# Creating output dir
		self.curdir = "%s/incoherentstokes/BEAM%d" % (self.outdir, self.tabid)
		cmd="mkdir -p %s" % (self.curdir)
		self.execute(cmd)

		if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
                                          # also we need to moint locus nodes that we need
			self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
			input_files=[]
			for loc in self.tab.location:
				# first "mounting" corresponding locus node
				input_dir="%s/%s_data/%d" % (cep2.hoover_data_dir, loc, obs.id)
				self.process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
				self.process.communicate()
				input_file="%s/%s_SAP%03d_B%03d_S*_bf.raw" % (input_dir, obs.id, self.sapid, self.tabid)
				input_files.extend(glob.glob(input_file))
			input_file=" ".join(input_files)
		else:
			input_file=glob.glob("/data/%s/%s_SAP%03d_B%03d_S*_bf.raw" % (obs.id, obs.id, self.sapid, self.tabid))[0]
		self.log.info("Input data: %s" % (input_file))
		self.output_prefix="%s_BEAM%d" % (obs.id, self.tabid)
		self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

		# running data conversion (2bf2fits)
		cmd="2bf2fits -CS -H -IS -parset %s -append -nbits 8 -A 100 -sigma 3 -v -nsubs %d -o %s %s" % (obs.parset, self.tab.nrSubbands, self.output_prefix, input_file)
		self.execute(cmd, workdir=self.curdir)

		# running RFI excision, checking
		total_chan = obs.nrSubbands*obs.nrChanPerSubIS
		if not cmdline.opts.is_norfi:
			zapstr=""
			# we should zap 1st chan as after 2nd polyphase it has junk
			if obs.nrChanPerSubIS > 1:
				zapstr="-zapchan 0:%d:%d" % (total_chan-1, obs.nrChanPerSubIS)
			self.log.info("Creating RFI mask...")
			cmd="rfifind -o %s -psrfits -noclip -blocks 16 %s %s.fits" % (self.output_prefix, zapstr, self.output_prefix)
			rfifind_popen = self.start_and_go(cmd, workdir=self.curdir)
			self.log.info("Producing RFI report...")
			samples_to_average=int(10000. / obs.samplingIS) # 10 s worth of data
			cmd="python %s/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n %d %s.fits" % (cep2.lofarsoft, samples_to_average, self.output_prefix)
			subdyn_popen = self.start_and_go(cmd, workdir=self.curdir)

			# waiting for rfifind to finish
			self.waiting("rfifind", rfifind_popen)

		# running prepfold with and without mask
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
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			# first running prepfold with mask (if --norfi was not set)
			if not cmdline.opts.is_norfi:
				cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -mask %s_rfifind.mask -o %s_%s %s.fits" % \
					(psr, prepfold_nsubs, self.output_prefix, psr, self.output_prefix, self.output_prefix)
				prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
				prepfold_popens.append(prepfold_popen)
			# running prepfold without mask
			if not cmdline.opts.is_norfi: output_stem="_nomask"
			else: output_stem=""
			cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -o %s_%s%s %s.fits" % \
				(psr, prepfold_nsubs, psr, self.output_prefix, output_stem, self.output_prefix)
			prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
			prepfold_popens.append(prepfold_popen)

		# now running dspsr stuff...
		zapstr=""
		if obs.nrChanPerSubIS > 1:
			zapstr="-j 'zap chan %s'" % (" ".join([str(ii) for ii in range(0, total_chan, obs.nrChanPerSubIS)]))
		dspsr_popens=[] # list of dspsr Popen objects
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			psr2=re.sub(r'[BJ]', '', psr)
			cmd="dspsr -E %s/%s.par %s -q -b 256 -fft-bench -O %s_%s -K -A -L 60 %s.fits" % \
				(self.outdir, psr2, zapstr, psr, self.output_prefix, self.output_prefix)
			dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
			dspsr_popens.append(dspsr_popen)

		# waiting for dspsr to finish
		self.waiting_list("dspsr", dspsr_popens)

		# running extra Psrchive programs, pam, pav,pdmp, etc... 
		# these programs should be run quick, so run them one by one

		# first, calculating the proper min divisir for the number of subbands
		self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (obs.nrChanPerSubIS, obs.nrSubbands))
		# calculating the least common denominator of obs.nrSubbands starting from obs.nrChanPerSubCS
		pav_nchans = self.lcd(obs.nrChanPerSubIS, obs.nrSubbands)
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			cmd="pam --setnchn %d -m %s_%s.ar" % (obs.nrSubbands, psr, self.output_prefix)
			self.execute(cmd, workdir=self.curdir)
			cmd="dspsr_ar_plots.sh %s_%s %d" % (psr, self.output_prefix, pav_nchans)
			self.execute(cmd, workdir=self.curdir)

		# now running pdmp without waiting...
		if not cmdline.opts.is_nopdmp:
			self.log.info("Running pdmp...")
			pdmp_popens=[]  # list of pdmp Popen objects	
			for psr in cmdline.psrs:
				if psr == "NONE": continue
				cmd="pdmp -mc %d -mb 128 -g %s_%s_pdmp.ps/cps %s_%s.ar" % \
					(obs.nrSubbands, psr, self.output_prefix, psr, self.output_prefix)
				pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
				pdmp_popens.append(pdmp_popen)
		
		# waiting for prepfold to finish
		self.waiting_list("prepfold", prepfold_popens)

		# running convert on prepfold ps to pdf and png
		if cmdline.psrs[0] != "NONE":
			self.log.info("Running convert on ps to pdf and png of the plots...")
			prepfold_ps=glob.glob("%s/*.pfd.ps" % (self.curdir))
			for psfile in prepfold_ps:
				base=psfile.split("/")[-1].split(".ps")[0]
				cmd="convert %s.ps %s.pdf" % (base, base)
				self.execute(cmd, workdir=self.curdir)
				cmd="convert -rotate 90 %s.ps %s.png" % (base, base)
				self.execute(cmd, workdir=self.curdir)
				cmd="convert -rotate 90 -crop 200x140-0 %s.ps %s.th.png" % (base, base)
				self.execute(cmd, workdir=self.curdir)

		# waiting for subdyn to finish
		self.waiting("subdyn.py", subdyn_popen)

		# Here, I should create RFI summary files, for the case of 1 and several beams...
		self.log.info("Creating RFI Summary files... (not implemented yet)")

		# waiting for pdmp to finish
		self.waiting_list("pdmp", pdmp_popens)
		# when pdmp is finished do extra actions with files...
		for psr in cmdline.psrs:
			if psr == "NONE": continue
			cmd="grep %s %s/pdmp.per > %s/%s_%s_pdmp.per" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
			self.execute(cmd, is_os=True)
			cmd="grep %s %s/pdmp.posn > %s/%s_%s_pdmp.posn" % (psr, self.curdir, self.curdir, psr, self.output_prefix)
			self.execute(cmd, is_os=True)
			# reading new DM from the *.per file
			newdm = np.loadtxt("%s/%s_%s_pdmp.per" % (self.curdir, psr, self.output_prefix), comments='#', usecols=(3,3), dtype=float, unpack=True)[0]
			cmd="pam -e AR -d %f -DTp %s_%s.ar" % (newdm, psr, self.output_prefix)
			self.execute(cmd, workdir=self.curdir)

		# copying parset file to output directory
		self.log.info("Copying original parset file to output directory...")
		cmd="cp -f %s %s" % (obs.parset, self.outdir)
		self.execute(cmd, workdir=self.outdir)
		# Make a tarball of all the plots
		self.log.info("Make a tarball of all the plots...")
		tar_list=[]
		for ext in self.extensions:
			ext_list=self.rglob(self.outdir, ext)
			tar_list.extend(ext_list)
		cmd="tar cvfz %s_plotsIS.tar.gz %s" % (obs.id, " ".join([f.split(self.outdir+"/")[1] for f in tar_list]))
		self.execute(cmd, workdir=self.outdir)

		# copying archive file to locus094
		locus="locus094"
		output_dir="%s_%s/%s_redIS" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir)
		output_archive="%s/%s_sap%03d_tab%04d_plotsIS.tar.gz" % (output_dir, obs.id, self.sapid, self.tabid)
		self.log.info("Copying archive file to %s:%s" % (locus, output_dir))
		cmd="rsync -avxP %s_plotsIS.tar.gz %s:%s" % (obs.id, locus, output_archive)
		self.execute(cmd, workdir=self.outdir)

		# combining all *.th.png files into one (for summary)
		self.log.info("Combining all th.png files into one...")
		cmd="thumbnail_combine.sh"
		self.execute(cmd, workdir=self.outdir)
			
		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

		# flushing log file and copy it to outdir on local node and locus092
		self.log.flush()
		cmd="cp -f %s %s" % (cep2.get_logfile(), self.outdir)
		os.system(cmd)
		cmd="rsync -avxP %s %s:%s" % (cep2.get_logfile(), locus, output_dir)
		os.system(cmd)

class CVUnit(PipeUnit):
	def __init__(self, tab):
		PipeUnit.__init__(self, tab)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (CV)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

class FE_CSUnit(PipeUnit):
	def __init__(self, tab):
		PipeUnit.__init__(self, tab)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (CS/FE)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

class FE_CVUnit(PipeUnit):
	def __init__(self, tab):
		PipeUnit.__init__(self, tab)

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (CV/FE)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))
