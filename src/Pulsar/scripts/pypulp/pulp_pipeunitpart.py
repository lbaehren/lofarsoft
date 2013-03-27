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
import parfile
from pulp_parset import Observation, radial_distance, find_pulsars
from pulp_usercmd import CMDLine, check_pulsars
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
from pulp_feedback import FeedbackUnit
from pulp_pipeunit import PipeUnit, CSUnit, ISUnit, rglob

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
					# fixing the coordinates
					cmd="fix_fits_coordinates.py %s.fits" % (self.output_prefix)
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
							cmd="dspsr -b %d -E %s/%s.par %s %s -fft-bench -O %s_%s -A -L 60 -t %d %s %s.fits" % \
								(dspsr_nbins, self.outdir, psr2, zapstr, verbose, psr, self.output_prefix, cmdline.opts.nthreads, cmdline.opts.dspsr_extra_opts, self.output_prefix)
							dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
							dspsr_popens.append(dspsr_popen)

						# waiting for dspsr to finish
						self.waiting_list("dspsr", dspsr_popens)

						# zapping rfi
						if not cmdline.opts.is_norfi:
							self.log.info("Zapping channels using median smoothed difference algorithm...")
							for psr in self.psrs:  # pulsar list is empty if --nofold is used
								cmd="paz -r -e paz.ar %s_%s.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)

						# dedispersing
						self.log.info("Dedispersing...")
						for psr in self.psrs:  # pulsar list is empty if --nofold is used
							if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
								cmd="pam -D -m %s_%s.paz.ar" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
							cmd="pam -D -e dd %s_%s.ar" % (psr, self.output_prefix)
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
								cmd="pam --setnchn %d -e fscr.AR %s_%s.dd" % (self.tab.nrSubbands, psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
								# remove non-scrunched dedispersed archive (we will always have unzapped non-dedispersed non-scrunched version)
								cmd="rm -f %s_%s.dd" % (psr, self.output_prefix)
								self.execute(cmd, workdir=self.curdir)
						else: # if number of chans == number of subs, we will just rename .paz.ar to .paz.fscr.AR and .dd to .fscr.AR
							for psr in self.psrs:  # pulsar list is empty if --nofold is used
								if not cmdline.opts.is_norfi or os.path.exists("%s/%s_%s.paz.ar" % (self.curdir, psr, self.output_prefix)):
									cmd="mv -f %s_%s.paz.ar %s_%s.paz.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
									self.execute(cmd, workdir=self.curdir)
								cmd="mv -f %s_%s.dd %s_%s.fscr.AR" % (psr, self.output_prefix, psr, self.output_prefix)
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


class CSUnitPart(PipeUnitPart, CSUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log)
                CSUnit.__init__(self, obs, cep2, cmdline, tab, log)

class ISUnitPart(PipeUnitPart, ISUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log)
                ISUnit.__init__(self, obs, cep2, cmdline, tab, log)

class FE_CSUnitPart(PipeUnitPart, FE_CSUnit):
        def __init__(self, obs, cep2, cmdline, tab, log, node, part):
		PipeUnitPart.__init__(self, obs, cep2, cmdline, tab, log)
                FE_CSUnit.__init__(self, obs, cep2, cmdline, tab, log)
