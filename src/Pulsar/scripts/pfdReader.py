#!/usr/bin/python  
from Tkinter import *  
import matplotlib.pyplot as plt
import numpy as Num
import struct
import psr_utils

class pfd:
    def __init__(self, filename):
        self.pfd_filename = filename
        infile = open(filename, "rb")
        # See if the .bestprof file is around
        swapchar = '<' # this is little-endian
        data = infile.read(5*4)
        testswap = struct.unpack(swapchar+"i"*5, data)
        # This is a hack to try and test the endianness of the data.
        # None of the 5 values should be a large positive number.
        if (Num.fabs(Num.asarray(testswap))).max() > 100000:
            swapchar = '>' # this is big-endian
        (self.numdms, self.numperiods, self.numpdots, self.nsub, self.npart) = \
                      struct.unpack(swapchar+"i"*5, data)
        (self.proflen, self.numchan, self.pstep, self.pdstep, self.dmstep, \
         self.ndmfact, self.npfact) = struct.unpack(swapchar+"i"*7, infile.read(7*4))
        self.filenm = infile.read(struct.unpack(swapchar+"i", infile.read(4))[0])
        self.candnm = infile.read(struct.unpack(swapchar+"i", infile.read(4))[0])
        self.telescope = infile.read(struct.unpack(swapchar+"i", infile.read(4))[0])
        self.pgdev = infile.read(struct.unpack(swapchar+"i", infile.read(4))[0])
        test = infile.read(16)
        has_posn = 1
        for ii in range(16):
            if test[ii] not in '0123456789:.-\0':
                has_posn = 0
                break
        if has_posn:
            self.rastr = test[:test.find('\0')]
            test = infile.read(16)
            self.decstr = test[:test.find('\0')]
            (self.dt, self.startT) = struct.unpack(swapchar+"dd", infile.read(2*8))
        else:
            self.rastr = "Unknown"
            self.decstr = "Unknown"
            (self.dt, self.startT) = struct.unpack(swapchar+"dd", test)
        (self.endT, self.tepoch, self.bepoch, self.avgvoverc, self.lofreq, \
         self.chan_wid, self.bestdm) = struct.unpack(swapchar+"d"*7, infile.read(7*8))
        # The following "fixes" (we think) the observing frequency of the Spigot
        # based on tests done by Ingrid on 0737 (comparing it to GASP)
        # The same sorts of corrections should be made to WAPP data as well...
        # The tepoch corrections are empirically determined timing corrections
        # Note that epoch is only double precision and so the floating
        # point accuracy is ~1 us!
        if self.telescope=='GBT':
            if Num.fabs(Num.fmod(self.dt, 8.192e-05) < 1e-12) and \
               ("spigot" in filename.lower() or "guppi" not in filename.lower()):
                if self.chan_wid==800.0/1024: # Spigot 800 MHz mode 2
                    self.lofreq -= 0.5 * self.chan_wid
                    # original values
                    #if self.tepoch > 0.0: self.tepoch += 0.039334/86400.0
                    #if self.bestprof: self.bestprof.epochf += 0.039334/86400.0
                    # values measured with 1713+0747 wrt BCPM2 on 13 Sept 2007
                    if self.tepoch > 0.0: self.tepoch += 0.039365/86400.0
                    if self.bestprof: self.bestprof.epochf += 0.039365/86400.0
                elif self.chan_wid==800.0/2048:
                    self.lofreq -= 0.5 * self.chan_wid 
                    if self.tepoch < 53700.0:  # Spigot 800 MHz mode 16 (downsampled)
                        if self.tepoch > 0.0: self.tepoch += 0.039352/86400.0
                        if self.bestprof: self.bestprof.epochf += 0.039352/86400.0
                    else:  # Spigot 800 MHz mode 14 
                        # values measured with 1713+0747 wrt BCPM2 on 13 Sept 2007
                        if self.tepoch > 0.0: self.tepoch += 0.039365/86400.0
                        if self.bestprof: self.bestprof.epochf += 0.039365/86400.0
                elif self.chan_wid==50.0/1024 or self.chan_wid==50.0/2048: # Spigot 50 MHz modes
                    self.lofreq += 0.5 * self.chan_wid
                    # Note: the offset has _not_ been measured for the 2048-lag mode
                    if self.tepoch > 0.0: self.tepoch += 0.039450/86400.0
                    if self.bestprof: self.bestprof.epochf += 0.039450/86400.0

        (self.topo_pow, tmp) = struct.unpack(swapchar+"f"*2, infile.read(2*4))
        (self.topo_p1, self.topo_p2, self.topo_p3) = struct.unpack(swapchar+"d"*3, \
                                                                   infile.read(3*8))
        (self.bary_pow, tmp) = struct.unpack(swapchar+"f"*2, infile.read(2*4))
        (self.bary_p1, self.bary_p2, self.bary_p3) = struct.unpack(swapchar+"d"*3, \
                                                                   infile.read(3*8))
        (self.fold_pow, tmp) = struct.unpack(swapchar+"f"*2, infile.read(2*4))
        (self.fold_p1, self.fold_p2, self.fold_p3) = struct.unpack(swapchar+"d"*3, \
                                                                   infile.read(3*8))

        (self.orb_p, self.orb_e, self.orb_x, self.orb_w, self.orb_t, self.orb_pd, \
         self.orb_wd) = struct.unpack(swapchar+"d"*7, infile.read(7*8))
        self.dms = Num.asarray(struct.unpack(swapchar+"d"*self.numdms, \
                                             infile.read(self.numdms*8)))
        if self.numdms==1:
            self.dms = self.dms[0]
        self.periods = Num.asarray(struct.unpack(swapchar+"d"*self.numperiods, \
                                                 infile.read(self.numperiods*8)))
        self.pdots = Num.asarray(struct.unpack(swapchar+"d"*self.numpdots, \
                                               infile.read(self.numpdots*8)))
        self.numprofs = self.nsub*self.npart
        if (swapchar=='<'):  # little endian
            self.profs = Num.zeros((self.npart, self.nsub, self.proflen), dtype='d')
            for ii in range(self.npart):
                for jj in range(self.nsub):
                    self.profs[ii,jj,:] = Num.fromfile(infile, Num.float64, self.proflen)
        else:
            self.profs = Num.asarray(struct.unpack(swapchar+"d"*self.numprofs*self.proflen, \
                                                   infile.read(self.numprofs*self.proflen*8)))
            self.profs = Num.reshape(self.profs, (self.npart, self.nsub, self.proflen))
        if (self.numchan==1):
            try:
                idata = infodata.infodata(self.filenm[:self.filenm.rfind('.')]+".inf")
                if idata.waveband=="Radio":
                    self.bestdm = idata.DM
                    self.numchan = idata.numchan
                else: # i.e. for events
                    self.bestdm = 0.0
                    self.numchan = 1
            except IOError:
                print "Warning!  Can't open the .inf file for "+filename+"!"
        self.binspersec = self.fold_p1*self.proflen
        self.chanpersub = self.numchan/self.nsub
        self.subdeltafreq = self.chan_wid*self.chanpersub
        self.hifreq = self.lofreq + (self.numchan-1)*self.chan_wid
        self.losubfreq = self.lofreq + self.subdeltafreq - self.chan_wid
        self.subfreqs = Num.arange(self.nsub, dtype='d')*self.subdeltafreq + \
                        self.losubfreq
        self.subdelays_bins = Num.zeros(self.nsub, dtype='d')
        self.killed_subbands = []
        self.killed_intervals = []
        self.pts_per_fold = []
        # Note: a foldstats struct is read in as a group of 7 doubles
        # the correspond to, in order: 
        #    numdata, data_avg, data_var, numprof, prof_avg, prof_var, redchi
        self.stats = Num.zeros((self.npart, self.nsub, 7), dtype='d')
        for ii in range(self.npart):
            currentstats = self.stats[ii]
            for jj in range(self.nsub):
                if (swapchar=='<'):  # little endian
                    currentstats[jj] = Num.fromfile(infile, Num.float64, 7)
                else:
                    currentstats[jj] = Num.asarray(struct.unpack(swapchar+"d"*7, \
                                                                 infile.read(7*8)))
            self.pts_per_fold.append(self.stats[ii][0][0])  # numdata from foldstats
        self.start_secs = Num.add.accumulate([0]+self.pts_per_fold[:-1])*self.dt
        self.pts_per_fold = Num.asarray(self.pts_per_fold)
        self.mid_secs = self.start_secs + 0.5*self.dt*self.pts_per_fold
        if (not self.tepoch==0.0):
            self.start_topo_MJDs = self.start_secs/86400.0 + self.tepoch
            self.mid_topo_MJDs = self.mid_secs/86400.0 + self.tepoch
        if (not self.bepoch==0.0):
            self.start_bary_MJDs = self.start_secs/86400.0 + self.bepoch
            self.mid_bary_MJDs = self.mid_secs/86400.0 + self.bepoch
        self.Nfolded = Num.add.reduce(self.pts_per_fold)
        self.T = self.Nfolded*self.dt
        self.avgprof = (self.profs/self.proflen).sum()
        infile.close()
        self.barysubfreqs = None
        if self.avgvoverc==0:
            if self.candnm.startswith("PSR_"):
                # If this doesn't work, we should try to use the barycentering calcs
                # in the presto module.
                try:
                    self.polycos = polycos.polycos(self.candnm[4:],
                                                   filenm=self.pfd_filename+".polycos")
                    midMJD = self.tepoch + 0.5*self.T/86400.0
                    self.avgvoverc = self.polycos.get_voverc(int(midMJD), midMJD-int(midMJD))
                    #sys.stderr.write("Approximate Doppler velocity (in c) is:  %.4g\n"%self.avgvoverc)
                    # Make the Doppler correction
                    self.barysubfreqs = self.subfreqs*(1.0+self.avgvoverc)
                except IOError:
                    self.polycos = 0
        if self.barysubfreqs is None:
            self.barysubfreqs = self.subfreqs
        self.fold_p1 = 1.0/self.fold_p1

    def __str__(self):
        out = ""
        for k, v in self.__dict__.items():
            if k[:2]!="__":
                if type(self.__dict__[k]) is StringType:
                    out += "%10s = '%s'\n" % (k, v)
                elif type(self.__dict__[k]) is IntType:
                    out += "%10s = %d\n" % (k, v)
                elif type(self.__dict__[k]) is FloatType:
                    out += "%10s = %-20.15g\n" % (k, v)
        return out

    def dedisperse(self, DM=None, interp=0):
        """
        dedisperse(DM=self.bestdm, interp=0):
            Rotate (internally) the profiles so that they are de-dispersed
                at a dispersion measure of DM.  Use FFT-based interpolation if
                'interp' is non-zero (NOTE: It is off by default!).
        """
        if DM is None:
            DM = self.bestdm
        # Note:  Since TEMPO Doppler corrects observing frequencies, for
        #        TOAs, at least, we need to de-disperse using topocentric
        #        observing frequencies.
        self.subdelays = psr_utils.delay_from_DM(DM, self.subfreqs)
        self.hifreqdelay = self.subdelays[-1]
        self.hifreqdelay = psr_utils.delay_from_DM(DM, 200.0)
        self.subdelays = self.subdelays-self.hifreqdelay
        delaybins = self.subdelays*self.binspersec - self.subdelays_bins
        if interp:
            new_subdelays_bins = delaybins
            for ii in range(self.npart):
                for jj in range(self.nsub):
                    tmp_prof = self.profs[ii,jj,:]
                    self.profs[ii,jj] = psr_utils.fft_rotate(tmp_prof, delaybins[jj])
            # Note: Since the rotation process slightly changes the values of the
            # profs, we need to re-calculate the average profile value
            self.avgprof = (self.profs/self.proflen).sum()
        else:
            new_subdelays_bins = Num.floor(delaybins+0.5)
            for ii in range(self.nsub):
                rotbins = int(new_subdelays_bins[ii])%self.proflen
                if rotbins:  # i.e. if not zero
                    subdata = self.profs[:,ii,:]
                    self.profs[:,ii] = Num.concatenate((subdata[:,rotbins:],
                                                        subdata[:,:rotbins]), 1)
        self.subdelays_bins += new_subdelays_bins
        self.sumprof = self.profs.sum(0).sum(0)
        if Num.fabs((self.sumprof/self.proflen).sum() - self.avgprof) > 1.0:
            print "self.avgprof is not the correct value!"
            
    def fold( self, period ):
        fold_p1 = self.fold_p1
        bindrift_per_int = ((period - fold_p1)/fold_p1) * self.proflen
        int_per_part = self.T/(self.npart*fold_p1)
        binstep =  bindrift_per_int * int_per_part
        ii = 0
        self.fold_p1 = period
        while ii  < self.npart:
        	rotbins = int((binstep*ii) % self.proflen)
        	if rotbins:
        		subdata = self.profs[ii,:,:]
        		self.profs[ii,:] = Num.concatenate((subdata[:,rotbins:], 
        										  subdata[:,:rotbins]), 1)
        	ii+= 1


    def combine_profs(self, new_npart, new_nsub):
        """
        combine_profs(self, new_npart, new_nsub):
            Combine intervals and/or subbands together and return a new
                array of profiles.
        """
        if (self.npart % new_npart):
            print "Warning!  The new number of intervals (%d) is not a" % new_npart
            print "          divisor of the original number of intervals (%d)!"  % self.npart
            print "Doing nothing."
            print "Factors are ", factors(self.npart)
            return None
        if (self.nsub % new_nsub):
            print "Warning!  The new number of subbands (%d) is not a" % new_nsub
            print "          divisor of the original number of subbands (%d)!"  % self.nsub
            print "Doing nothing."
            print "Factors are:\n", factors(self.nsub)
            return None

        dp = self.npart/new_npart
        ds = self.nsub/new_nsub

        newprofs = Num.zeros((new_npart, new_nsub, self.proflen), 'd')
        for ii in range(new_npart):
            # Combine the subbands if required
            if (self.nsub > 1):
                for jj in range(new_nsub):
                    subprofs = Num.add.reduce(self.profs[:,jj*ds:(jj+1)*ds], 1)
                    # Combine the time intervals
                    newprofs[ii][jj] = Num.add.reduce(subprofs[ii*dp:(ii+1)*dp])
            else:
                newprofs[ii][0] = Num.add.reduce(self.profs[ii*dp:(ii+1)*dp,0])
        return newprofs

    def plot_intervals(self, phasebins='All'):
        """
        plot_intervals(self, phasebins='All', device='/xwin'):
            Plot the subband-summed profiles vs time.  Restrict
                the bins in the plot to the (low:high) slice defined
                by the phasebins option if it is a tuple (low,high)
                instead of the string 'All'. 
        """
        if phasebins is not 'All':
            lo, hi = phasebins
            profs = self.profs[:,:,lo:hi].sum(1)
        else:
            lo, hi = 0.0, self.proflen
            profs = self.profs.sum(1)
        return profs

    def plot_subbands(self, phasebins='All'):
        """
        plot_subbands(self, phasebins='All', device='/xwin'):
            Plot the interval-summed profiles vs subband.  Restrict
                the bins in the plot to the (low:high) slice defined
                by the phasebins option if it is a tuple (low,high)
                instead of the string 'All'. 
        """
        if phasebins is not 'All':
            lo, hi = phasebins
            profs = self.profs[:,:,lo:hi].sum(0)
        else:
            lo, hi = 0.0, self.proflen
            profs = self.profs.sum(0)
        lof = self.lofreq - 0.5*self.chan_wid
        hif = lof + self.chan_wid*self.numchan
        return profs

    def adjust_fold_frequency(self, phasebins, profs=None, shiftsubs=False):
        """
        adjust_fold_frequency(phasebins, profs=None, shiftsubs=False):
            Linearly shift the intervals by phasebins over the course of
                the observation in order to change the apparent folding
                frequency.  Return a 2D array containing the de-dispersed
                profiles as a function of time (i.e. shape = (npart, proflen)),
				and the reduced chi^2 of the resulting summed profile.
                If profs is not None, then use profs instead of self.profs.
				If shiftsubs is not False, then actually correct the subbands
				instead of a 2D projection of them.
        """
        if not self.__dict__.has_key('subdelays'):
            print "Dedispersing first..."
            self.dedisperse()
        if shiftsubs:
            print "Shifting all the subbands..."
            if profs is None:
                profs = self.profs
            for ii in range(self.npart):
                bins_to_shift = float(ii)/self.npart - (ii)/self.npart
                for jj in range(self.nsub):
                    profs[ii,jj] = psr_utils.rotate(profs[ii,jj], bins_to_shift)
            redchi = self.calc_redchi2(prof=profs.sum(0).sum(0))
        else:
            print "Shifting just the projected intervals (not individual subbands)..."
            if profs is None:
                profs = self.profs.sum(1)
            for ii in range(self.npart):
                bins_to_shift = int(round(float(ii)/self.npart * phasebins))
                profs[ii] = psr_utils.rotate(profs[ii], bins_to_shift)
            redchi = self.calc_redchi2(prof=profs.sum(0))
        print "New reduced-chi^2 =", redchi
        return profs, redchi
                

class Application(Frame):              
    def __init__(self, master=None):
        Frame.__init__(self, master)   
        self.grid()                    
        self.createWidgets()

    def createWidgets(self):
    	Label(self, text='DM').grid(row=1,column=0)
        self.dmInput = Entry ( self, textvar=DM)
        self.dmInput.insert(0,DM)
        self.dmInput.grid(row=1,column=1)
        Label(self, text='Period').grid(row=2, column=0)
        self.periodInput = Entry ( self, textvar=Period)
        self.periodInput.insert(0,Period)
        self.periodInput.grid(row=2,column=1)
        Label(self, text='Subbands').grid(row=3, column=0)
        self.nsubInput = Entry ( self, textvar=N_sub)
        self.nsubInput.insert(0,N_sub)
        self.nsubInput.grid(row=3,column=1)
        Label(self, text='Subints').grid(row=4, column=0)
        self.nintInput = Entry ( self, textvar=N_int)
        self.nintInput.insert(0,N_int)
        self.nintInput.grid(row=4,column=1)
        Label(self, text='x_min').grid(row=5, column=0)
        self.xminInput = Entry ( self, textvar=X_min)
        self.xminInput.insert(0,X_min)
        self.xminInput.grid(row=5,column=1)
        Label(self, text='x_max').grid(row=6, column=0)
        self.xmaxInput = Entry ( self, textvar=X_max)
        self.xmaxInput.insert(0,X_max)
        self.xmaxInput.grid(row=6,column=1)
        Label(self, text='Rotate by').grid(row=7, column=0)
        self.rotInput = Entry ( self, textvar=Rot)
        self.rotInput.insert(0,Rot)
        self.rotInput.grid(row=7,column=1)
        
        self.plotButton = Button ( self, text='Plot', command = self.plotter, width=10 )
        self.plotButton.grid(row=1, column=2)        
        self.lplotButton = Button ( self, text='Line Plot', command = self.linePlotter, width=10 )
        self.lplotButton.grid(row=2, column=2)
        self.baryButton = Button (self, text='Bary', command = self.bary, width=10)
        self.baryButton.grid(row=3, column=2)
        self.topoButton = Button (self, text='Topo', command = self.topo, width=10)
        self.topoButton.grid(row=4, column=2)

        self.quitButton = Button ( self, text='Quit', command=self.quit, width=10 )        
        self.quitButton.grid(row=6,column=2) 
  
        
        
    def bary(self):
    	self.periodInput.delete(0,END)
    	self.periodInput.insert(0,tp.bary_p1)
	
    def topo(self):
		self.periodInput.delete(0,END)
		self.periodInput.insert(0,tp.topo_p1)
		
    def plotter(self):
    	DM = float(self.dmInput.get())
    	Period = float(self.periodInput.get())
    	N_sub = int(self.nsubInput.get())
    	N_int = int(self.nintInput.get())
    	X_min = float(self.xminInput.get())
    	X_max = float(self.xmaxInput.get())
        Rot = float(self.rotInput.get())
    	tp.dedisperse(DM)
    	tp.fold(Period)
    	profs = tp.combine_profs(N_int,N_sub)
    	subbands = profs.sum(axis=0)
        i = 0
        while i < len(subbands):
            subbands[i] = psr_utils.fft_rotate(subbands[i],Rot*len(subbands[i]))
            subbands[i] = subbands[i] - min(subbands[i])
            subbands[i] = subbands[i]/max(subbands[i])
            i+=1
    	subints = profs.sum(axis=1)
        i = 0
        while i < len(subints):
            subints[i] = psr_utils.fft_rotate(subints[i], Rot*len(subbands[i]))
            subints[i] = subints[i] - min(subints[i])
            subints[i] = subints[i] - min(subints[i])
            i+=1
    	profile = subbands.sum(axis=0)
    	fig = plt.figure(figsize=(8,8))
    	ax1 = fig.add_axes([0.1, 0.05,0.35, 0.7])
    	ax1.imshow(Num.array(subbands), aspect='auto', extent=(0,1,tp.lofreq,tp.hifreq), interpolation='Nearest', origin='Lower', cmap='binary')
    	ax1.set_xlabel('Pulse Phase')
    	ax1.set_ylabel('Frequency (MHz)')
    	ax1.set_xlim((X_min, X_max))
    	ax2 = fig.add_axes([0.6, 0.05,0.35, 0.7])
    	ax2.imshow(Num.array(subints), aspect='auto', extent=(0,1,0,tp.T), interpolation='Nearest', origin='Lower', cmap='binary')
    	ax2.set_xlabel('Pulse Phase')
    	ax2.set_ylabel('Time (s)')
    	ax2.set_xlim((X_min, X_max))
    	ax3 = fig.add_axes([0.1, 0.75,0.35, 0.23])
    	phase=Num.linspace(0,1,num=len(profile))
    	ax3.plot(phase, profile, color='k')
    	ax3.set_xticks(())
    	ax3.set_yticks(())
    	ax3.set_xlim((X_min, X_max))
    	plt.figtext(0.6, 0.90, 'Source:\t'+tp.candnm)
    	plt.figtext(0.6, 0.87, 'RA:\t\t'+tp.rastr)
    	plt.figtext(0.6, 0.84, 'Dec:\t\t'+tp.decstr)    	
    	plt.figtext(0.6, 0.81, 'Period:\t'+str(Period))
    	plt.figtext(0.6, 0.78, 'DM:\t\t'+str(DM))
    	plt.show()
    
    def linePlotter(self):
    	DM = float(self.dmInput.get())
    	Period = float(self.periodInput.get())
    	N_sub = int(self.nsubInput.get())
    	N_int = int(self.nintInput.get())
    	X_min = float(self.xminInput.get())
    	X_max = float(self.xmaxInput.get())
        Rot = float(self.rotInput.get())
    	tp.dedisperse(DM)
    	tp.fold(Period)
    	profs = tp.combine_profs(N_int,N_sub)
    	subbands = profs.sum(axis=0)
        i = 0
        while i < len(subbands):
            subbands[i] = psr_utils.fft_rotate(subbands[i], Rot*len(subbands[i]))
            i+=1
    	profile = subbands.sum(axis=0)
    	phase=Num.linspace(0,1,num=len(profile))
    	i = 0
        fig = plt.figure(figsize=(4,8))
        ax = fig.add_axes([0.1, 0.05, 0.85, 0.9])
    	for sub in subbands:
			freq = tp.lofreq + (i+0.5)*((tp.hifreq - tp.lofreq)/N_sub)
			sub = sub - min(sub)
			sub = sub/max(sub)
			plt.plot(phase, sub+i, color='k')
			plt.text(X_min+0.01*(X_max-X_min), i+0.5, str(round(freq))+" MHz")
			plt.yticks(())
			plt.ylim((-0.2,i+1.1))
			plt.xlim((X_min,X_max))
			plt.xlabel('Pulse Phase')
			plt.ylabel('Frequency')
			i+=1
        plt.show()
		
def factors(n):
    a=[]
    for i in range(1,int(n/2+1)):
        if n % i == 0: 
            a.append(i)
	return a
	
testpfd = sys.argv[1]
tp = pfd(testpfd)	
DM = tp.bestdm
Period = tp.topo_p1
N_sub = tp.nsub
N_int = tp.npart
X_min = 0
X_max = 1
Rot = 0.0
app = Application()                    
app.master.title("pfdReader") 
app.mainloop()                         
