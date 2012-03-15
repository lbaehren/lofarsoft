###################################################################
#
# Class Pipeline - main processing class
# Other class, aka Processing Units (per Beam) are defined
#

import os, sys, glob, time, re, os.path
import numpy as np
import cPickle
import logging
import subprocess, shlex
from subprocess import PIPE, STDOUT, Popen
from pulp_parset import Observation, radial_distance, find_pulsars
from pulp_usercmd import CMDLine, check_pulsars
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger

### define a global function - recursive glob ####

# return list of dirs in the current base directory
def getDirs(base):
	return [x for x in glob.iglob(os.path.join(base, '*')) if os.path.isdir(x)]

# recursive glob
def rglob(base, pattern):
	flist=[]
	flist.extend(glob.glob(os.path.join(base, pattern)))
	dirs = getDirs(base)
	if len(dirs):
		for d in dirs:
			flist.extend(rglob(os.path.join(base, d), pattern))
	return flist


# The main processing class
class Pipeline:
	def __init__(self, obs, cep2, cmdline, log):
		self.units = []   # list of processing units
		self.sum_popens = []  # list of Popen make_summary processes
		self.summary_dirs = {}  # dictionary, key - locus node of the summaries, value - dir
		# extensions of the files to copy to archive in summary (*_nopfd*.tgz)
		self.archive_exts=["*.log", "*.txt", "*.pdf", "*.ps", "*.bestprof", "*.inf", "*.rfirep", "*png", "*parset", "*.par"]
		# prefix and suffix for summary archive name, in between them there will CS, IS, CV code
		self.archive_prefix="_combined"
		self.archive_suffix="_nopfd.tar.gz"

		for sap in obs.saps:
			for tab in sap.tabs:
				if not tab.is_coherent:
					unit = ISUnit(obs, cep2, cmdline, tab, log)
				if tab.is_coherent and tab.specificationType != "flyseye":
					if obs.CS:
						unit = CSUnit(obs, cep2, cmdline, tab, log)
					elif obs.CV:
						unit = CVUnit(obs, cep2, cmdline, tab, log)
					else:
						log.error("Can't initialize processing pipeline unit for SAP=%d TAB=%d" % (sap.sapid, tab.tabid))
						os.system("stty sane")
						sys.exit(1)
				if tab.is_coherent and tab.specificationType == "flyseye":
					if obs.CS:
						unit = FE_CSUnit(obs, cep2, cmdline, tab, log)
					elif obs.CV:
						unit = FE_CVUnit(obs, cep2, cmdline, tab, log)
					else:
						log.error("Can't initialize processing pipeline FE unit for SAP=%d TAB=%d" % (sap.sapid, tab.tabid))
						os.system("stty sane")
						sys.exit(1)

				# adding unit to the list
				self.units.append(unit)

		# check first the user flag if we only want to process CS or IS data
		# and remove those processing units that we will not process from the list
		toremove = set()
		for uid in range(len(self.units)):
			if cmdline.opts.is_coh_only == True and not self.units[uid].tab.is_coherent: toremove.add(uid)
			if cmdline.opts.is_incoh_only == True and self.units[uid].tab.is_coherent: toremove.add(uid)
		toremove = sorted(toremove, reverse=True)
		for uid in toremove: del(self.units[uid])

		# if User specified particular beams to process, then we delete all other units from the list
		toremove = set()
		if len(cmdline.beams) != 0:
			for uid in range(len(self.units)):
				uidbeam="%d:%d" % (self.units[uid].sapid, self.units[uid].tabid)
				if uidbeam not in cmdline.beams: toremove.add(uid)
			toremove = sorted(toremove, reverse=True)
			for uid in toremove: del(self.units[uid])

		if len(self.units) == 0:
			log.info("None beams to process!")
			os.system("stty sane")
			sys.exit(0)	

		# creating main output directory on locus092 for CS data and on locus094 for IS data
		# before that we also remove this directory if user flag is_del was set
		unique_outdirs=["%s:%s_%s/%s%s" % (unit.summary_node, cep2.processed_dir_prefix, unit.summary_node, \
			cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, \
			unit.summary_node_dir_suffix) for unit in self.units if unit.summary_node != "" and unit.summary_node_dir_suffix != ""]
		unique_outdirs=np.unique(unique_outdirs)
		for uo in unique_outdirs:
			node=uo.split(":")[0]
			sumdir=uo.split(":")[1]
			self.summary_dirs[node] = sumdir
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
				cmd="ssh -t %s 'rm -rf %s 2>&1'" % (node, sumdir)
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
			cmd="ssh -t %s 'mkdir -p %s 2>&1'" % (node, sumdir)
			p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			p.communicate()
		
		# Defining output directories for all local locus nodes
		# We have to do it here rather than in 'run' function of PipeUnit because there can be more than 1 beam per node
		# and then there can be a clash when we will delete (if -del is set) the output directory 
		unit_outdirs = []
		for unit in self.units:
			if len(unit.tab.location) > 1:
				if unit.tab.is_coherent: locus=cep2.hoover_nodes[0] # we choose one hoover node for CS data
				else: locus=cep2.hoover_nodes[1]                    # and another for IS data
			else:
				locus=unit.tab.location[0]
			# if user specified output dir (relative to /data/LOFAR_PULSAR_....)
			if cmdline.opts.outdir != "":
				unit.outdir = "%s_%s/%s" % (cep2.processed_dir_prefix, locus, cmdline.opts.outdir)
			else: # if output dir was not set
				unit.outdir = "%s_%s/%s%s" % (cep2.processed_dir_prefix, locus, obs.id, unit.outdir_suffix)
			unit_outdirs.append("%s:%s" % (locus, unit.outdir))
		unit_outdirs=np.unique(unit_outdirs)

		# Deleting all local output directories if --del is set 
		if cmdline.opts.is_delete:
			for uo in unit_outdirs:
				node=uo.split(":")[0]
				outdir=uo.split(":")[1]
				log.info("Deleting previous processed results on %s: %s" % (node, outdir))
				cmd="ssh -t %s 'rm -rf %s 2>&1'" % (node, outdir)
				p = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				p.communicate()
				os.system("stty sane")
		log.info("")



	# kicking off the pipeline
	def start(self, obs, cep2, cmdline, log):
		# and here we start...
		log.info("Starting PULP processing for:")

		for unit in self.units:
			# if data for this beam are on several nodes, then we have to log in to hoover node...
			if len(unit.tab.location) > 1:
				if unit.tab.is_coherent: locus=cep2.hoover_nodes[0] # we choose one hoover node for CS data
				else: locus=cep2.hoover_nodes[1]                    # and another for IS data
			else:
				locus=unit.tab.location[0]
#			cmd="%s %s '/home/kondratiev/pulp/pulp.py --noinit --local --beams %d:%d %s'" %  \
#				(cep2.cexeccmd, cep2.cexec_nodes[locus], unit.sapid, unit.tabid, " ".join(cmdline.options))
			# Using ssh in the background instead of cexec in order to get proper status code if job has failed
			# with cexec you always get status=0
			# Also, using "-t" option in ssh allows to send KILL signal to remote command over ssh to finish all processes there
			# in case when pipeline was interrupted
			# with nohup and </dev/null the parent bash process only kiled when interrupted by User..
#			cmd="ssh -t %s 'nohup /home/kondratiev/pulp/pulp.py --noinit --local --beams %d:%d %s </dev/null 2>&1'" % \
			cmd="ssh -t %s '/home/kondratiev/pulp/pulp.py --noinit --local --beams %d:%d %s 2>&1'" % \
				(locus, unit.sapid, unit.tabid, " ".join(cmdline.options))
			unit.parent = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
			os.system("stty sane")
			log.info("SAP=%d TAB=%d %s(%s%s) on %s (pid=%d)  [number of locations = %d]" % \
				(unit.sapid, unit.tabid, unit.tab.specificationType == "flyseye" and ", ".join(unit.tab.stationList) + " " or "", \
				unit.tab.specificationType == "flyseye" and "FE/" or "", \
				unit.tab.is_coherent and (obs.CS and "CS" or "CV") or "IS", \
				locus, unit.parent.pid, len(unit.tab.location)))
			time.sleep(1) # wait 1 sec (otherwise terminal output gets messed up often)


	# here we finish the pipeline, creating extra files, convert ps(pdf) to png, etc...
	# create FE status maps, TA heat maps...
	def finish(self, obs, cep2, cmdline, log):

		try:
			# waiting for processing in individual locus nodes to finish
			# unless we want to do just a summary
			if not cmdline.opts.is_summary:
				run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
				os.system("stty sane")
				log.info("Still running [%d]: %s" % (len(run_units), run_units))
				for unit in self.units:
					os.system("stty sane")
					log.info("waiting...")
					unit.parent.communicate()
					log.info("Process pid=%d has finished, status=%d" % (unit.parent.pid, unit.parent.returncode))
					run_units = [u.parent.pid for u in self.units if u.parent.poll() is None]
					if len(run_units) > 0: log.info("Still running [%d]: %s" % (len(run_units), run_units))

				# loop over finished processes to see if they all finished OK
				failed_units = [u for u in self.units if u.parent.returncode > 0]
				os.system("stty sane")
				log.info("Failed beams [%d]: %s" % (len(failed_units), ", ".join(["%s:%s" % (u.sapid, u.tabid) for u in failed_units])))
				if len(failed_units) > 0:
					log.info("*** Summaries will not be complete! Re-run processing for the failed beams using --beams option. ***")

			self.sum_popens=[]
			log.info("Starting summaries...")
			# starting separate pulp.py on summary nodes just to finish up
			for (sumnode, sumdir) in self.summary_dirs.items():
#				cmd="%s %s '/home/kondratiev/pulp/pulp.py --noinit --summary --local --beams %s %s'" %  \
#					(cep2.cexeccmd, cep2.cexec_nodes[sumnode], sumnode, " ".join(cmdline.options))
				# Using ssh in the background instead of cexec in order to get proper status code if job has failed
				# with cexec you always get status=0
#				cmd="ssh -t %s 'nohup /home/kondratiev/pulp/pulp.py --noinit --summary --local --beams %s %s </dev/null 2>&1'" % \
				cmd="ssh -t %s '/home/kondratiev/pulp/pulp.py --noinit --summary --local --beams %s %s 2>&1'" % \
					(sumnode, sumnode, " ".join(cmdline.options))
				sum_popen = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT)
				self.sum_popens.append(sum_popen)
				log.info("Making summaries on %s... (pid=%d)" % (sumnode, sum_popen.pid))

			run_units = [p.pid for p in self.sum_popens if p.poll() is None]
			os.system("stty sane")
			log.info("Still running [%d]: %s" % (len(run_units), run_units))
			for proc in self.sum_popens:
				os.system("stty sane")
				log.info("waiting...")
				proc.communicate()
				log.info("Process pid=%d has finished, status=%d" % (proc.pid, proc.returncode))
				run_units = [p.pid for p in self.sum_popens if p.poll() is None]
				finished_units = [p for p in self.sum_popens if p.poll() is not None]
				for fu in finished_units:
					if fu.poll() != 0:
						raise Exception
					else: self.sum_popens.remove(fu)
				if len(run_units) > 0: log.info("Still running [%d]: %s" % (len(run_units), run_units))

			# loop over finished summaries to see if they all finished OK
			failed_summaries = [s for s in self.sum_popens if s.returncode > 0]
			log.info("%d failed summaries" % (len(failed_summaries)))

		except Exception:
			log.exception("Oops... 'finish' function of the pipeline has crashed!")
			self.kill(log)
			os.system("stty sane")
			sys.exit(1)

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
			os.system("stty sane")
			sys.exit(1)

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


	# run necessary processes to organize summary info on summary nodes
	# to be run locally on summary node
	def make_summary(self, obs, cep2, cmdline, log):

		try:
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
						cmd="mv -f %s_sap%03d_beam%04d.log %s/SAP%d/%s" % \
							(obs.id, unit.sapid, unit.tabid, unit.beams_root_dir, unit.sapid, unit.procdir)
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
	        	        montage_cmd_pdf="montage -geometry 100% -rotate 90 -adjoin -tile 1x1 -pointsize 12 "
        	        	chif=open("%s/chi-squared.txt" % (sumdir), 'w')
     	          	        psr_bestprofs=rglob(sumdir, "*.pfd.bestprof")
        	               	for bp in [file for file in psr_bestprofs if re.search("_nomask_", file) is None]:
                	               	psr=bp.split("/")[-1].split("_")[0]
        	        	        thumbfile=bp.split(sumdir+"/")[-1].split(".pfd.bestprof")[0] + ".pfd.th.png"
                	        	# getting current number for SAP and TA beam (or station name for FE)
        	                	cursapid=int(thumbfile.split("_SAP")[-1].split("_")[0])
                	                curprocdir=thumbfile.split("_SAP")[-1].split("_")[1]
      	                	        chi_val = 0.0
        	                	cmd="cat %s | grep chi-sqr | cut -d = -f 2" % (bp)
	                	        chiline=os.popen(cmd).readlines()
        	                	if np.size(chiline) > 0:
                	                	chi_val = float(chiline[0].rstrip())
   	                	        else:
						log.warning("Warning: can't read chi-sq value from %s" % (bp))
		                        chif.write("file=%s obs=%s_SAP%d_%s_%s chi-sq=%g\n" % (thumbfile, data_code, cursapid, curprocdir, psr, chi_val))
        		                montage_cmd += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, thumbfile)
        		                montage_cmd_pdf += "-label '%s SAP%d %s\n%s\nChiSq = %g' %s " % (data_code, cursapid, curprocdir, psr, chi_val, thumbfile)
              		        chif.close()

				# creating combined plots
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

			# create beam_process_node.txt file
			log.info("Creating the beam_process_node.txt file...")
        	        bpnf=open("%s/beam_process_node.txt" % (sumdir), 'w')
			for unit in [u for u in self.units if u.summary_node == sumnode]: 
				for node in unit.tab.location:
					for rf in unit.tab.rawfiles[node]:
						bpnf.write("%s %s%s\n" % \
						(node, rf, unit.tab.specificationType == "flyseye" and " [%s]" % (",".join(unit.tab.stationList)) or ""))
              	        bpnf.close()

			# creating TA heatmaps 
			# only when folding, and only if pulsars are set from the command line, or 'parset' or 'sapfind' or 'sapfind3' keywords are used (or
			# nothing is given for --pulsar option
			# otherwise, different TA beams will be folded for different pulsars, and TA heatmap does not have sense
			if data_code == "CS" and not cmdline.opts.is_nofold and (len(cmdline.psrs) == 0 or (len(cmdline.psrs) != 0 and cmdline.psrs[0] != "tabfind")):
				for sap in obs.saps:
					if sap.nrRings > 0:
						if len(cmdline.psrs) != 0 and cmdline.psrs[0] != "parset" and cmdline.psrs[0] != "sapfind" and cmdline.psrs[0] != "sapfind3":
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

			# creating combined DSPSR plots
			if not cmdline.opts.is_skip_dspsr:
				dspsr_diags=rglob(sumdir, "*_diag.png")
				if len(dspsr_diags) > 0:
					log.info("Creating DSPSR summary diagnostic plots...")
					if len(dspsr_diags) > 1: cmd="convert %s -append dspsr_status.png" % (" ".join(dspsr_diags))
					else: cmd="mv %s dspsr_status.png" % (dspsr_diags[0])
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

			# Combining different status maps into one 'status.png' to be shown in web-summary page 
			if os.path.exists("%s/FE_status.png" % (sumdir)):
				log.info("Renaming FE status map file to status.png ...")
				cmd="mv FE_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
			elif os.path.exists("%s/TAheatmap_status.png" % (sumdir)):
				log.info("Renaming TA heatmap map file to status.png ...")
				cmd="mv TAheatmap_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
			elif os.path.exists("%s/dspsr_status.png" % (sumdir)):
				log.info("Renaming dspsr status file to status.png ...")
				cmd="mv dspsr_status.png status.png"
				self.execute(cmd, log, workdir=sumdir)
			else: log.info("No status.png created")
			# creating thumbnail version of status.png if it exists
			if os.path.exists("%s/status.png" % (sumdir)):		
				cmd="convert -scale 200x140-0-0 status.png status.th.png"
				self.execute(cmd, log, workdir=sumdir)

			# Make a tarball of all the plots (summary archive)
			log.info("Making a final tarball of all files with extensions: %s" % (", ".join(self.archive_exts)))
			tar_list=[]
			for ext in self.archive_exts:
				ext_list=rglob(sumdir, ext)
				tar_list.extend(ext_list)
			cmd="tar cvfz %s%s%s%s %s" % (obs.id, self.archive_prefix, data_code, self.archive_suffix, " ".join([f.split(sumdir+"/")[1] for f in tar_list]))
			self.execute(cmd, log, workdir=sumdir)

			# finish
			end_time=time.time()
			total_time= end_time- start_time
			log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			log.info("Total runnung time: %.1f s (%.2f hrs)" % (total_time, total_time/3600.))

			# flushing log file and copy it to summary node
			log.flush()
			cmd="cp -f %s %s" % (cep2.get_logfile(), sumdir)
			os.system(cmd)

		except Exception:
			log.exception("Oops... 'make_summary' function on %s has crashed!" % (cep2.get_current_node()))
			os.system("stty sane")
			sys.exit(1)

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
		# extensions of the files to copy to archive (parfile and parset will be also included)
		self.extensions=["*.pdf", "*.ps", "*.pfd", "*.bestprof", "*.inf", "*.rfirep", "*png", "*.ar", "*.AR", "*pdmp*"]
		self.procdir = "BEAM%d" % (self.tabid)

		# pulsars to fold for this unit
		self.psrs = []
		if not cmdline.opts.is_nofold:
			self.psrs = self.get_pulsars_to_fold(obs, cep2, cmdline, log)

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
	
			# if special word "tabfind" is given
			if len(cmdline.psrs) != 0 and cmdline.psrs[0] == "tabfind":
				log.info("Searching for best pulsar for folding in SAP=%d TAB=%d..." % (self.sapid, self.tabid))
				self.psrs = find_pulsars(self.tab.rarad, self.tab.decrad, cmdline, cep2.tabfind)
				if len(self.psrs) > 0: 
					self.psrs = self.psrs[:1] # leave only one pulsar
					log.info("%s" % (" ".join(self.psrs)))

			# using pulsars from SAP
			if len(cmdline.psrs) != 0 and (cmdline.psrs[0] == "sapfind" or cmdline.psrs[0] == "sapfind3"):
				for sap in obs.saps:
					if self.sapid == sap.sapid:
						self.psrs[:] = sap.psrs
						break
				if cmdline.psrs[0] == "sapfind" and len(self.psrs)>0: self.psrs = self.psrs[:1]  # leave only one pulsar

			# if --pulsar is used but no special word
			if len(cmdline.psrs) != 0 and cmdline.psrs[0] != "parset" and cmdline.psrs[0] != "tabfind" and \
					cmdline.psrs[0] != "sapfind" and cmdline.psrs[0] != "sapfind3":
				self.psrs[:] = cmdline.psrs # copying all items

			# if pulsar list is still empty, and we did not set --nofold then exit
			if len(self.psrs) == 0:
				log.error("*** No pulsar found to fold and --nofold is not used for SAP=%d TAB=%d. Exiting..." % (self.sapid, self.tabid))
				sys.exit(1)
			
			# checking if pulsars are in ATNF catalog, or if not par-files do exist fo them, if not - exit
			for psr in self.psrs:
				if not check_pulsars(psr, cmdline, cep2, log):
					log.info("*** No parfile found for pulsar %s for SAP=%d TAB=%d. Exiting..." % (psr, self.sapid, self.tabid))
					sys.exit(1)
			return self.psrs

		except Exception:
			self.log.exception("Oops... get_pulsars_to_fold has crashed!")
			self.kill()  # killing all open processes
			sys.exit(1)


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
			self.kill()  # killing all open processes
			sys.exit(1)

	def start_and_go(self, cmd, workdir=None, shell=False):
	    	"""
        	Execute the command 'cmd' after logging the command
		This function start the cmd and leaves the fucntion
		returning the Popen object, it does not wait for process to finish
    		"""
		status=1
		try:
			self.log.info(re.sub("\n", "\\\\n", cmd))
			self.log.info("Start at UTC %s" % (time.asctime(time.gmtime())))
               		process = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=workdir, shell=shell)
			status=process.returncode
			self.procs.append(process)
			self.log.info("Job pid=%d, not waiting for it to finish." % (process.pid))
			return process
		except Exception:
			self.log.exception("Oops... job has crashed!\n%s\nStatus=%s" % (re.sub("\n", "\\\\n", cmd), status))
			self.kill()  # killing all open processes
			sys.exit(1)

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
			self.kill()  # killing all open processes
			sys.exit(1)

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
					if fu.returncode != 0: raise Exception
				if len(run_units) > 0: self.log.info("Still running [%d]: %s" % (len(run_units), run_units))
			job_end = time.time()
			job_total_time = job_end - job_start
			self.log.info("Processes of %s have finished at UTC %s, Waiting time: %.1f s (%.2f hrs)" % \
				(prg, time.asctime(time.gmtime()), job_total_time, job_total_time/3600.))
			self.log.info("")
		except Exception:
			fu = [u for u in popen_list if u.poll() is not None][0]
			self.log.exception("Oops... %s has crashed!\npid=%d, Status=%s" % (prg, fu.pid, fu.returncode))
			self.kill()
			sys.exit(1)

	def lcd(self, low, high):
		"""
		Calculates the lowest common denominator of 'high' value between 'low' and 'high'
		"""
		for ii in range(low, high+1): 
			if high % ii == 0: return ii
		return high

	# function that checks all processes in the list and kill them if they are still running
	def kill(self):
		if self.log != None: self.log.info("Killing all open processes for SAP=%d TAB=%d..." % (self.sapid, self.tabid))
		for proc in self.procs:
			if proc.poll() is None: # process is still running
				proc.kill()
				if proc != None: proc.communicate()
				if proc != None: proc.poll()
		self.procs = []
				

	# main processing function
	def run(self, obs, cep2, cmdline, log):
		try:
			self.log = log
			self.logfile = cep2.get_logfile().split("/")[-1]		
			self.start_time=time.time()	

			# start logging
			self.log.info("%s SAP=%d TAB=%d %s(%s%s Stokes: %s)    UTC start time is: %s  @node: %s" % \
				(obs.id, self.sapid, self.tabid, obs.FE and ", ".join(self.tab.stationList) + " " or "", obs.FE and "FE/" or "", self.code, \
				self.stokes, time.asctime(time.gmtime()), cep2.current_node))

			# Re-creating root output directory
			cmd="mkdir -p %s" % (self.outdir)
			self.execute(cmd)

			# creating Par-file in the output directory or copying existing one
			if not cmdline.opts.is_nofold:
				for psr in self.psrs:
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
					self.log.info("Parfile does not exist. Creating parfile base on pulsar ephemeris from ATNF catalog...")
					cmd="psrcat -db_file %s -e %s > %s/%s.par" % (cep2.psrcatdb, psr2, self.outdir, psr2)
					self.execute(cmd, is_os=True)

			# Creating output dir
			self.curdir = "%s/%s/SAP%d/%s" % (self.outdir, self.beams_root_dir, self.sapid, self.procdir)
			cmd="mkdir -p %s" % (self.curdir)
			self.execute(cmd)

			if len(self.tab.location) > 1: # it means we are on hoover nodes, so dir with the input data is different
                	                          # also we need to moint locus nodes that we need
				self.log.info("Re-mounting locus nodes on 'hoover' node %s: %s" % (cep2.current_node, " ".join(self.tab.location)))
				input_files=[]
				for loc in self.tab.location:
					# first "mounting" corresponding locus node
					input_dir="%s/%s_data/%d" % (cep2.hoover_data_dir, loc, obs.id)
					process = Popen(shlex.split("ls %s" % (input_dir)), stdout=PIPE, stderr=STDOUT)
					process.communicate()
#					input_file="%s/%s_SAP%03d_B%03d_S*_bf.raw" % (input_dir, obs.id, self.sapid, self.tabid)
#					input_files.extend(glob.glob(input_file))
					input_file=["%s/%s/%s" % (input_dir, obs.id, f.split("/" + obs.id + "/")[-1]) for f in self.tab.rawfiles[loc]]
					input_files.extend(input_file)
				input_file=" ".join(input_files)
			else:
				for key in self.tab.location:
					self.log.info("%s  - %s" % (key, " ".join(self.tab.rawfiles[cep2.current_node])))
				input_file=" ".join(self.tab.rawfiles[cep2.current_node])
#				input_file=glob.glob("%s/%s/%s_SAP%03d_B%03d_S*_bf.raw" % (cep2.rawdir, obs.id, obs.id, self.sapid, self.tabid))[0]
			self.log.info("Input data: %s" % (input_file))
			self.output_prefix="%s_SAP%d_%s" % (obs.id, self.sapid, self.procdir)
			self.log.info("Output file(s) prefix: %s" % (self.output_prefix))

			# running data conversion (2bf2fits)
			cmd="2bf2fits %s -parset %s -append -nbits 8 -A 100 -sigma 3 -v -nsubs %d -o %s %s" % (self.raw2fits_extra_options, obs.parset, self.tab.nrSubbands, self.output_prefix, input_file)
			self.execute(cmd, workdir=self.curdir)

			# running RFI excision, checking
			total_chan = obs.nrSubbands*self.nrChanPerSub
			if not cmdline.opts.is_norfi:
				zapstr=""
				# we should zap 1st chan as after 2nd polyphase it has junk
				if self.nrChanPerSub > 1:
					zapstr="-zapchan 0:%d:%d" % (total_chan-1, self.nrChanPerSub)
				self.log.info("Creating RFI mask...")
				cmd="rfifind -o %s -psrfits -noclip -blocks 16 %s %s.fits" % (self.output_prefix, zapstr, self.output_prefix)
				rfifind_popen = self.start_and_go(cmd, workdir=self.curdir)
				self.log.info("Producing RFI report...")
				samples_to_average=int(10000. / self.sampling) # 10 s worth of data
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
			for psr in self.psrs:   # pulsar list is empty if --nofold is used
				# first running prepfold with mask (if --norfi was not set)
				if not cmdline.opts.is_norfi:
					cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -mask %s_rfifind.mask -o %s_%s %s.fits" % \
						(psr, prepfold_nsubs, self.output_prefix, psr, self.output_prefix, self.output_prefix)
					prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
					prepfold_popens.append(prepfold_popen)
					time.sleep(5) # will sleep for 5 secs, in order to give prepfold enough time to finish with temporary files lile resid2.tmp
						      # otherwise it can interfere with next prepfold call
				# running prepfold without mask
				if not cmdline.opts.is_norfi: output_stem="_nomask"
				else: output_stem=""
				cmd="prepfold -noscales -nooffsets -noxwin -psr %s -nsub %d -n 256 -fine -nopdsearch -o %s_%s%s %s.fits" % \
					(psr, prepfold_nsubs, psr, self.output_prefix, output_stem, self.output_prefix)
				prepfold_popen = self.start_and_go(cmd, workdir=self.curdir)
				prepfold_popens.append(prepfold_popen)
				time.sleep(5) # again will sleep for 5 secs, in order to give prepfold enough time to finish with temporary files like resid2.tmp
					      # otherwise it can interfere with next prepfold call

			# now running dspsr stuff...
			if not cmdline.opts.is_skip_dspsr:
				zapstr=""
				if self.nrChanPerSub > 1:
					zapstr="-j 'zap chan %s'" % (" ".join([str(ii) for ii in range(0, total_chan, self.nrChanPerSub)]))
				dspsr_popens=[] # list of dspsr Popen objects
				for psr in self.psrs: # pulsar list is empty if --nofold is used
					psr2=re.sub(r'[BJ]', '', psr)
					cmd="dspsr -E %s/%s.par %s -q -b 256 -fft-bench -O %s_%s -K -A -L 60 %s.fits" % \
						(self.outdir, psr2, zapstr, psr, self.output_prefix, self.output_prefix)
					dspsr_popen = self.start_and_go(cmd, workdir=self.curdir)
					dspsr_popens.append(dspsr_popen)

				# waiting for dspsr to finish
				if not cmdline.opts.is_nofold: self.waiting_list("dspsr", dspsr_popens)

				# running extra Psrchive programs, pam, pav,pdmp, etc... 
				# these programs should be run quick, so run them one by one

				# first, calculating the proper min divisir for the number of subbands
				self.log.info("Getting proper value of nchans in pav -f between %d and %d..." % (self.nrChanPerSub, obs.nrSubbands))
				# calculating the least common denominator of obs.nrSubbands starting from self.nrChanPerSub
				pav_nchans = self.lcd(self.nrChanPerSub, obs.nrSubbands)
				for psr in self.psrs:  # pulsar list is empty if --nofold is used
					cmd="pam --setnchn %d -m %s_%s.ar" % (obs.nrSubbands, psr, self.output_prefix)
					self.execute(cmd, workdir=self.curdir)
					# creating DSPSR diagnostic plot
					cmd="dspsr_ar_plots.sh %s_%s %d" % (psr, self.output_prefix, pav_nchans)
					self.execute(cmd, workdir=self.curdir)

				# now running pdmp without waiting...
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold:
					self.log.info("Running pdmp...")
					pdmp_popens=[]  # list of pdmp Popen objects	
					for psr in self.psrs:
						cmd="pdmp -mc %d -mb 128 -g %s_%s_pdmp.ps/cps %s_%s.ar" % \
							(obs.nrSubbands, psr, self.output_prefix, psr, self.output_prefix)
						pdmp_popen = self.start_and_go(cmd, workdir=self.curdir)
						pdmp_popens.append(pdmp_popen)
		
			# waiting for prepfold to finish
			if not cmdline.opts.is_nofold: self.waiting_list("prepfold", prepfold_popens)

			# running convert on prepfold ps to pdf and png
			if not cmdline.opts.is_nofold:
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

			# getting the list of *.pfd.bestprof files and read chi-sq values for all folded pulsars
			if not cmdline.opts.is_nofold:
				self.log.info("Reading chi-squared values and adding to chi-squared.txt...")
				# also preparing montage command to create combined plot
				montage_cmd="montage -background none -pointsize 10.2 "
				chif=open("%s/%s_sap%03d_tab%04d_chi-squared.txt" % (self.outdir, obs.id, self.sapid, self.tabid), 'w')
				psr_bestprofs=rglob(self.outdir, "*.pfd.bestprof")
				thumbs=[] # list of thumbnail files
				for bp in [file for file in psr_bestprofs if re.search("_nomask_", file) is None]:
					psr=bp.split("/")[-1].split("_")[0]
					thumbfile=bp.split(self.outdir+"/")[-1].split(".pfd.bestprof")[0] + ".pfd.th.png"
					thumbs.append(thumbfile)
					# getting current number for SAP and TA beam
					cursapid=int(thumbfile.split("_SAP")[-1].split("_")[0])
					curprocdir=thumbfile.split("_SAP")[-1].split("_")[1]
					chi_val = 0.0
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
				if len([ff for ff in thumbs if os.path.exists(ff)]) == len(thumbs):
					# creating combined plots
					self.log.info("Combining all pfd.th.png files in a single combined plot...")
					montage_cmd += "combined.png"
					self.execute(montage_cmd, workdir=self.outdir)
					# making thumbnail version of the combined plot
					cmd="convert -resize 200x140 -bordercolor none -border 150 -gravity center -crop 200x140-0-0 +repage combined.png combined.th.png"
					self.execute(cmd, workdir=self.outdir)

			# waiting for subdyn to finish
			self.waiting("subdyn.py", subdyn_popen)

			# waiting for pdmp to finish
			if not cmdline.opts.is_skip_dspsr:
				if not cmdline.opts.is_nopdmp and not cmdline.opts.is_nofold: 
					self.waiting_list("pdmp", pdmp_popens)
					# when pdmp is finished do extra actions with files...
					for psr in self.psrs:
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
			# Make a tarball of all the plots for this beam
			self.log.info("Making a tarball of all the files with extensions: %s" % (", ".join(self.extensions)))
			tarname="%s_sap%03d_tab%04d%s" % (obs.id, self.sapid, self.tabid, self.archive_suffix)
			tar_list=[]
			for ext in self.extensions:
				ext_list=rglob(self.curdir, ext)
				tar_list.extend(ext_list)
			tar_list.extend(glob.glob("%s/*.par" % (self.outdir)))
			tar_list.extend(glob.glob("%s/*.parset" % (self.outdir)))
			cmd="tar cvfz %s %s" % (tarname, " ".join([f.split(self.outdir+"/")[1] for f in tar_list]))
			self.execute(cmd, workdir=self.outdir)

			# copying archive file to summary node
			output_dir="%s_%s/%s%s" % \
				(cep2.processed_dir_prefix, self.summary_node, cmdline.opts.outdir == "" and cmdline.opts.obsid or cmdline.opts.outdir, self.summary_node_dir_suffix)
			output_archive="%s/%s" % (output_dir, tarname)
			self.log.info("Copying archive file to %s:%s" % (self.summary_node, output_dir))
			cmd="rsync -avxP %s %s:%s" % (tarname, self.summary_node, output_archive)
			self.execute(cmd, workdir=self.outdir)

			# finish
			self.end_time=time.time()
			self.total_time= self.end_time- self.start_time
			self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
			self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

			# flushing log file and copy it to outdir on local node and summary node
			self.log.flush()
			cmd="cp -f %s %s" % (cep2.get_logfile(), self.outdir)
			os.system(cmd)
			cmd="rsync -avxP %s %s:%s" % (cep2.get_logfile(), self.summary_node, output_dir)
			proc = Popen(shlex.split(cmd), stdout=PIPE, stderr=STDOUT, cwd=self.outdir)
			proc.communicate()

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
		self.sampling = obs.samplingCS
		self.summary_node = "locus092"
		self.summary_node_dir_suffix = "_CSplots_py" # "_CSplots"
		self.archive_suffix = "_plotsCS.tar.gz"
		self.outdir_suffix = "_red_py" # "_red"

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
		self.sampling = obs.samplingIS
		self.summary_node = "locus094"
		self.summary_node_dir_suffix = "_redIS_py" # "_redIS"
		self.archive_suffix = "_plotsIS.tar.gz"
		self.outdir_suffix = "_redIS_py" # "_redIS"

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
		self.beams_root_dir = ""
		self.raw2fits_extra_options=""
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.sampling = obs.samplingCS
		self.summary_node = ""
		self.summary_node_dir_suffix = ""
		self.archive_suffix = ""
		self.outdir_suffix = ""

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))

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
		self.summary_node_dir_suffix = "_CSplots_py" # "_CSplots"
		self.archive_suffix = "_plotsCS.tar.gz"
		self.outdir_suffix = "_red_py" # "_red"
		# re-assigning procdir from BEAMN to station name
		if obs.FE and self.tab.stationList[0] != "": 
			self.procdir = self.tab.stationList[0]

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

class FE_CVUnit(PipeUnit):
	def __init__(self, obs, cep2, cmdline, tab, log):
		PipeUnit.__init__(self, obs, cep2, cmdline, tab, log)
		self.code = "CV"
		self.stokes = obs.stokesCS
		self.beams_root_dir = ""
		self.raw2fits_extra_options=""
		self.nrChanPerSub = obs.nrChanPerSubCS
		self.sampling = obs.samplingCS
		self.summary_node = ""
		self.summary_node_dir_suffix = ""
		self.archive_suffix = ""
		self.outdir_suffix = ""
		# re-assigning procdir from BEAMN to station name
		if obs.FE and self.tab.stationList[0] != "": 
			self.procdir = self.tab.stationList[0]

	def run(self, obs, cep2, cmdline, log):
		self.log = log
		self.start_time=time.time()	

		# start logging
		self.log.info("%s SAP=%d TAB=%d (FE/%s Stokes: %s)    UTC start time is: %s  @node: %s" % (obs.id, self.sapid, self.tabid, self.code, self.stokes, time.asctime(time.gmtime()), cep2.current_node))

		# finish
		self.end_time=time.time()
		self.total_time= self.end_time- self.start_time
		self.log.info("UTC stop time is: %s" % (time.asctime(time.gmtime())))
		self.log.info("Total runnung time: %.1f s (%.2f hrs)" % (self.total_time, self.total_time/3600.))
