#!/usr/bin/env python
#
import numpy as np
import os, os.path, stat, glob, sys, getopt
import infodata as inf

is_phase = False   # if True then calculate the phase of the pulses
inffile=""   # inf-file
spfile=""    # .singlepulse file
polycofile="polyco.dat"  # polyco-file
obscode="1"  # GBT
psrname=""

def usage (prg):
        """ prints the usage info about the current program
        """
        print "Program %s converts the .singlepulse file to tim-file\n" % (prg,)
        print "Usage: %s [-p, --phase] [-o, --obs <obscode>]\n\
                         [-h, --help] <inf-file> <singlepulse-file>\n\
         -o, --obs <obscode>        - set the observatory code (default = 1 for GBT)\n\
         -s, --source <psrname>     - pulsar name to be used for polyco and output tim-file\n\
         -p, --polyco <polyco-file> - use polyco-file to calculate the phase of the pulses\n\
         -h, --help                 - print this message\n" % (prg,)

def parsecmdline (prg, argv):
        """ parse the command line arguments
        """
        if not argv:
                usage (prg)
                sys.exit()
        else:
                try:
                        opts, args = getopt.getopt (argv, "hs:p:o:", ["help", "source=", "polyco=", "obs="])
                        for opt, arg in opts:
                                if opt in ("-h", "--help"):
                                        usage (prg)
                                        sys.exit()

                                if opt in ("-p", "--polyco"):
                                        global is_phase
					is_phase = True
					global polycofile
					polycofile = arg

                                if opt in ("-s", "--source"):
                                        global psrname
                                        psrname = arg

                                if opt in ("-o", "--obs"):
                                        global obscode
                                        obscode = arg

                        if not args:
                                print "inf-file and .singlepulse files are not given!\n"
                                sys.exit(2)
                        else:
				if len(args) < 2:
					print ".singlepulse file is not given!\n"
					sys.exit(2)
				else:
					global inffile
					inffile = args[0]
                                	global spfile
                                	spfile = args[1]

                except getopt.GetoptError:
                        print "Wrong option!"
                        usage (prg)
                        sys.exit(2)

if __name__=="__main__":
        parsecmdline (sys.argv[0].split("/")[-1], sys.argv[1:])
	# reading inf-file
	id = inf.infodata(inffile)

	if is_phase == True:
		import polycos as poly

	startmjd=id.epoch
	tres = id.dt
	unc = "%9f" % (tres * 1000000., )
	if psrname != "":
		source = psrname
	else:
		source = id.object
	cfreq = id.lofreq
	totbw = id.BW
	chanbw = id.chan_width
	freq = cfreq + totbw - chanbw     # lowest freq of the highest channel
                                          # prepdata is dedispersing to higher freq (the highest freq channel gets zero delay)

	# reading .singlepulse file
	dm, sigma, secs = np.loadtxt(spfile, usecols=(0,1,2), comments='#', dtype=float, unpack=True)
	offset, downfact = np.loadtxt(spfile, usecols=(3,4), comments='#', dtype=int, unpack=True)
	toa = ["%.13f" % (startmjd + (offset[i] * tres)/86400.,) for i in np.arange(np.size(offset))]

	# calculating the phases of pulses 
	if is_phase == True:
		pid=poly.polycos(source, polycofile)
		phase=[pid.get_phs_and_freq(float(t.split(".")[0]), float("0." + t.split(".")[1]))[0] for t in toa]

	# writing the tim-file
	# Princeton format (+ additional extra field is for sigma)
	timfile=inffile.split(".inf")[0] + ".tim"
	if is_phase == True:
		lines=["%1s %-12s %8.3f %-20s%9s%10s   %s   %f" % (obscode, source, freq, str(toa[i]), str(unc), "0", str(sigma[i]), phase[i]) for i in np.arange(np.size(offset))]
	else:
		lines=["%1s %-12s %8.3f %-20s%9s%10s   %s" % (obscode, source, freq, str(toa[i]), str(unc), "0", str(sigma[i])) for i in np.arange(np.size(offset))]
	np.savetxt(timfile, np.transpose((lines)), fmt="%s")
