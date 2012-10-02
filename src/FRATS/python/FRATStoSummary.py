"""
    
Goal: Make a summary of the most important analysis results. Use the FRATSanalysis library to do so.


"""
# interactive=True: plot in window
# interactive=False: save in pdfFile 
interactive=False
# used to look in different subdirectories
useZeroDM=3


# If run multiple times, close old plots to prevent overflow of memory
if "plt" in dir():
    plt.close('all')
# load Agg interface to save as pdf
if not interactive:
    import matplotlib
    matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import FRATSanalysis as fa   # Library written to read and filter FRATS output
import sys
from matplotlib.backends.backend_pdf import PdfPages
import glob


if useZeroDM==1:
    ZeroDM="ZeroDM"
elif useZeroDM==2:
    ZeroDM="1kpc"
elif useZeroDM==3:
    ZeroDM="coma/multiLength"
else:
    ZeroDM=""


# select directories

# Settings used for analysis and plotting
plotRFIspectrum=False # dynamic spectrum of flagged channels, uses quite some memory
mydirs=fa.dirs14 # select directories from which to analyse the data
window=50 # trigger window in samples
DM_step=0.025 # used for binning / plotting
DM_min=1.0-DM_step/2 # used for binning /plotting
DM_max=21.0+DM_step/2 # used for binning /plotting
Sigma_min=0.1
Sigma_max=12
Sigma_step=0.2
AvgStdPlotDMstep=5
nrbands=10 
subplot_rows=5
subplot_columns=3
subplotmax=subplot_rows*subplot_columns
nrblocks=1300
blocksize=192*2
secondsize=192*4
secondspertimebin=10
time_min=0
time_max=nrblocks*blocksize
time_step=secondsize*secondspertimebin
summarydir='/Users/STV/Astro/Analysis/FRAT/analysis/'+ZeroDM+'/summary/'
thresholdlist=[5.0]  # The data can be analysed for different thresholds, should be sorted.
maxTriggerMsgFileSize=1e8 # not too flood the memory by files that are too big
DMprecision=2 # round DMs to how many digits?
# Make a report per directory
# Definition of band: Data is dedispersed into several bands. The coincidence works between these bands.
# Multiple LOFAR subbands are summed to make such a band.

for mydir in mydirs[12:]: # directories to analyse
    if not interactive:
        plt.close('all')
    print "Analyzing",mydir 
    # obtain obsid (L#####) and pointing (SAP###) from the directory
    dirsplit=mydir.rstrip('/').split('/')
    obsid=dirsplit[-2]
    pointing=dirsplit[-1]
    try:
        # read observation parameters from the analysed beam
        par=fa.obsParameters(mydir,glob.glob(mydir+'/frats_beam*.sh')[0].split('/')[-1])
        par['i']=par['i'].replace('$1',obsid)
        par['i']=par['i'].replace('$2',pointing)
        # Derive some parameters 
        fa.derivedParameters(par)
    except:
        # break the loop if reading of the parameters fails
        print "loading parameters failed",mydir,glob.glob(mydir+'/frats_beam*.sh')[0].split('/')[-1]
        continue

    #load triggermessages

    # Load integration lengths used for triggering (Boxcar profile length)
    # This could have been specified as a single value ('il') or as a range ('ilrange')
    # In the rest of the analysis we us ilrange, so this needs to be specified if it was unknown.
    if 'ilrange' not in par.keys() and 'il' in par.keys():
        par['ilrange']=[par['il']]
    
    msg='temp' # just so we can remove it later
    for triggerlength in par['ilrange']:
        del msg # remove old msg to clear memory
        if os.path.getsize(mydir+'TriggerMsg.log') < maxTriggerMsgFileSize
            try:
                msg=fa.loadTriggerMsg(mydir)
            except:
                print "unable to load trigger msgs for",mydir
                continue
        else:
            print "trigger filesize to big for",mydir
            continue
        # select only messages with the correct triggerlength
        fa.filterDatabase(msg,'length',triggerlength,triggerlength)
        # set integration length to the value used
        par['il']=triggerlength
        # close old plots
        if not interactive:
            plt.close('all')
        # Make a separate analysis for each threshold
        for threshold in thresholdlist:
            if True:
                print "Analyzing",mydir,threshold
                sys.stdout.flush()
                # make pdf file
                pp = PdfPages(summarydir+obsid+'_'+pointing+'_thrsh_'+str(threshold)+'_int_'+str(triggerlength)+'.pdf')
                # filter on threshold
                fa.filterDatabase(msg,'max',threshold,1000000000)
                # if no message are left, continue with the next threshold
                if len(msg.keys())<1:
                    continue
                # Make a dictionary indexed on keyword instead of on trigger messages.
                trval=dict()
                for k in msg.values()[0].keys():
                    trval[k]=fa.filterTriggers(msg,-1,k)
                # Round the DM value to make it more readable.
                trval['DM']=[round(d,DMprecision) for d in trval['DM']]
                DMvalues=list(set(trval['DM']))

# FLAGGING OF CHANNELS AND SAMPLES
                figures=[]
                figures.append(plt.figure())

                # Get two dimensional array of flagged samples and channels
                # dimensions: blocks x samples
                flaggedSamples=fa.getFlaggedSamples(mydir,blocksize=par['sa'],matrixoutput=True)
                # dimensions: blocks x channels
                flaggedChannels=fa.getFlaggedChannels(mydir,channels=par['nrfreqs'],matrixoutput=True)

                # Plot how often each channel is flagged
                plt.plot(np.sum(flaggedChannels,axis=0))
                plt.xlabel('channel number')
                plt.ylabel('# flagged')
                plt.title('channel flagged count')
                plt.vlines(range(0,par['nrfreqs'],par['ch']*par['nrCSB']),0,max(np.sum(flaggedChannels,axis=0)))
                pp.savefig()

                # Plot how many channels are flagged in each block
                figures.append(plt.figure())
                plt.plot(np.sum(flaggedChannels,axis=1))
                plt.xlabel('time (block nr)')
                plt.ylabel('# flagged')
                plt.title('Channels flagged per time block')
                plt.xlim(0,par['n'])
                pp.savefig()
                
                
                # Plot 2-D spectrum of flagged channels to see when each channel is flagged
                if plotRFIspectrum:
                    figures.append(plt.figure())
                    plt.imshow(np.log(flaggedChannels),origin=(0,0))
                    plt.xlabel('channel')
                    plt.ylabel('time (blocknr)')
                    plt.title('Dynamic spectrum of flagged channels')
                    pp.savefig()

                # Plot how many samples are flagged in each block
                figures.append(plt.figure())
                plt.plot(np.sum(flaggedSamples,axis=1))
                plt.xlabel('time (block nr)')
                plt.ylabel('# flagged')
                plt.title('Samples flagged per time block')
                plt.xlim(0,par['n'])
                pp.savefig()

                # Flagging of channels per band
                # copy flagged channels
                fc=flaggedChannels.copy()
                # make an extra axis for each band
                fc.resize(par['n'],par['nrstreams'],par['nrfreqs']/par['nrstreams'])
                # collapse over the frequency axis so we are left with total number per flagged stream
                fc2=np.sum(fc,axis=2)
                figures.append(plt.figure())
                # use multiple plots per page
                subplot_rows=5
                subplot_columns=2
                subplotmax=subplot_rows*subplot_columns
                plt.subplots_adjust(wspace=0.7,hspace=1.2)
                subplotcounter=0
                for i in range(par['nrstreams']):
                    subplotcounter+=1
                    if subplotcounter>subplotmax: #start a new page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.5,hspace=1.2)
                    # set plot position
                    plt.subplot(subplot_rows,subplot_columns,i%subplotmax+1)
                    plt.plot(fc2[:,i]) # plot band i
                    # set limits and ticks
                    ymax=10*round(max(fc2[:,i])/10)
                    yticks=np.arange(0,ymax+1,max(ymax/4,1))
                    # percentage ticks
                    yticksAlt=[100*y/par['nrfreqs']*par['nrstreams'] for y in yticks]
                    yticksAlt=["%0.1f" % y for y in yticksAlt]
                    # set title and labels
                    plt.title('Flagged channels in band '+str(i),fontsize='xx-small')
                    plt.xlabel('time (blocknr)',fontsize='xx-small')
                    plt.ylabel('# flagged',fontsize='xx-small')
                    plt.xlim(0,par['n'])
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(yticks,fontsize='xx-small')
                    plt.ylabel('# flagged',fontsize='xx-small')
                    plt.twinx()
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(yticks,yticksAlt,fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                    plt.ylabel('% flagged',fontsize='xx-small')
                    

                
# ALL CHANNEL TRIGGERS

                # Plot histogram of triggers per DM (for all channels)
                pp.savefig()
                figures.append(plt.figure())
                # NOTE maybe derive DM_min, DM_max, DM_step from observation values
                figDM=plt.hist(trval['DM'],np.arange(DM_min,DM_max,DM_step))
                plt.title('DM histogram, all channels')
                plt.xlabel('DM')
                plt.ylabel('Number of triggers')
                pp.savefig()

                # And the strength of all triggers (for all DMs and channels)
                figures.append(plt.figure())
                # NOTE maybe derive Sigma_min, Sigma_max, Sigma_step from observation values
                figTriggerLevel=plt.hist(trval['max'],np.arange(Sigma_min,Sigma_max,Sigma_step))
                plt.title('Trigger strength, channel trigger, all subbands')
                plt.xlabel('Trigger strength, sigma above mean')
                plt.ylabel('Number of triggers')
                pp.savefig()

# TRIGGERS PER CHANNEL
                # Plot histogram of triggers per DM and the strength of the trigger, but now for each channel.
                figures.append(plt.figure())
                # set multiple plot per page
                subplot_rows=5
                subplot_columns=3
                subplotmax=subplot_rows*subplot_columns
                plt.subplot(subplot_rows,subplot_columns,1)
                plt.subplots_adjust(wspace=0.4,hspace=1.0)
                subplotcounter=0
                # make separate plot per band
                for band in range(nrbands):
                    # START PLOT DM HISTOGRAM
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        # set new page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.5,hspace=1.2)
                    trval[band]=dict()
                    # filter messages per band
                    for k in msg.values()[0].keys():
                        trval[band][k]=fa.filterTriggers(msg,band,k)
                    # round DM values
                    trval[band]['DM']=[round(d,2) for d in trval[band]['DM']]
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    # NOTE: derive DM_min, DM_max, DM_step from data?
                    # plot histogram of data
                    plt.hist(trval[band]['DM'],np.arange(DM_min,DM_max,DM_step))
                    # NOTE: derive ymax from data?
                    plt.ylim(ymax=30)
                    plt.yticks(range(0,ymax+1,5),fontsize='xx-small')
                    plt.title('DM histogram, chan '+str(band),fontsize='xx-small')
                    plt.xlabel('DM',fontsize='xx-small')
                    plt.ylabel('# triggers',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    # END PLOT DM HISTOGRAM
                    # START PLOT STRENGTH HISTOGRAM
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.5,hspace=1.2)
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    figTriggerLevel=plt.hist(trval[band]['max'],np.arange(Sigma_min,Sigma_max,Sigma_step))
                    plt.ylim(ymax=15)
                    plt.yticks(fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.title('Trigger strength, chan '+str(band),fontsize='xx-small')
                    plt.xlabel('strength, sigma above mean',fontsize='xx-small')
                    plt.ylabel('# triggers',fontsize='xx-small')
                    # END PLOT STRENGTH HISTOGRAM
                    # START PLOT TIME HISTOGRAM
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.5,hspace=1.2)
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    figTriggerLevel=plt.hist([t/time_step for t in trval[band]['time']],np.arange(time_min/time_step,time_max/time_step,1))
                    #plt.ylim(ymax=1000)
                    plt.yticks(fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.title('Time histogram, chan '+str(band),fontsize='xx-small')
                    plt.xlabel('Time/ '+str(secondspertimebin)+' seconds',fontsize='xx-small')
                    plt.ylabel('# triggers',fontsize='xx-small')
                    # END PLOT TIME HISTOGRAM
                pp.savefig()
                if not interactive:
                    plt.close('all')


                # COINCIDENCES OVER MULTIPLE CHANNELS
                
                # check multiples of time, subband, max (strength) and DM
                timesSubband=zip(trval['time'],trval['subband'],trval['max'],trval['DM'])
                # sort on time
                timesSubband.sort() 
                timesSubband=np.array(timesSubband)
                alltimesSubband=np.copy(timesSubband)

                # multiple plots per stage
                subplot_columns=3
                subplot_rows=5
                subplotmax=subplot_columns*subplot_rows
                #timesSubband=timesSubband[timesSubband[:,2]>threshold]
                sets=dict()
                figures.append(plt.figure())
                
                plt.suptitle('Threshold '+str(threshold)) # supertitle for all plots
                plt.subplot(subplot_rows,subplot_columns,1)
                plt.subplots_adjust(wspace=0.6,hspace=1.0)
                subplotcounter=0
                sets=dict()
                DMhist=dict()
                DMvalues.sort()
                for DM in DMvalues:
                    # get triggercoincidence sets, make selection on DM
                    sets[DM]=np.array(fa.triggercoincidenceSets2(timesSubband[timesSubband[:,3]==DM],window))
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        # new page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.6,hspace=1.0)
                        plt.suptitle('Threshold '+str(threshold))
                    # set plot position
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    if len(sets[DM])>0:
                        # plot histogram per DM
                        DMhist[DM]=plt.hist(sets[DM][:,1],np.arange(1,nrbands+1),log=True)
                    plt.title('DM '+str(DM),fontsize='xx-small')
                    plt.xlabel('# coincidences ',fontsize='xx-small')
                    plt.ylabel('# triggers',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                    # plot per nr coincidence channels
                pp.savefig()


                # Standard deviation and average
                figures.append(plt.figure())
                plt.suptitle('Threshold '+str(threshold))
                subplot_columns=2
                subplot_rows=2
                subplotmax=subplot_columns*subplot_rows
                subplotcounter=0
                # Load standard deviation and average
                stddev=fa.loadStddev(len(par['DM']),par['nrstreams'],mydir)
                average=fa.loadAverage(len(par['DM']),par['nrstreams'],mydir)
                # Get DMs in observation
                DMvalues2=stddev.keys()
                DMvalues2.sort()
                print "Only plotting average and standard deviation for every",AvgStdPlotDMstep,"DM"
                # Make a plot for each 
                for DM in np.array(DMvalues2)[range(0,len(DMvalues2),AvgStdPlotDMstep)]:
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        # new page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.3,hspace=0.5)
                        plt.suptitle('Threshold '+str(threshold))
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    # offset so plots don't overlap
                    ch_offset=3
                    # plot standard deviation for each band/stream
                    [plt.plot(stddev[DM][i]+ch_offset*i) for i in range(par['nrstreams'])]
                    # set title, label, limits and ticks
                    plt.title('DM '+str(DM),fontsize='xx-small')
                    plt.xlabel('time (blocknr)',fontsize='xx-small')
                    plt.ylabel('stddev+'+str(ch_offset)+'*ch',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                    plt.ylim(ymax=par['nrstreams']*ch_offset + max(stddev[DM][i]))
                    plt.xlim(xmax=par['n'])
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        # next page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.6,hspace=1.0)
                        plt.suptitle('Threshold '+str(threshold))
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    # get limits and spacing between plots
                    avav=np.array([np.average(average[DMvalues2[0]][i][-200:]) for i in range(par['nrstreams'])])
                    avstd=np.array([np.std(average[DMvalues2[0]][i][-200:]) for i in range(par['nrstreams'])])
                    avpredicted=par['ch']*par['nrCSB']*par['tInt']
                    ymin=avav[0]-6*avstd[0]-avpredicted
                    ymax=avav[-1]+6*avstd[-1]-avpredicted+ch_offset*par['nrstreams']
                    ch_offset=round(np.max(avstd))
                    # plot average for each band/stream
                    [plt.plot(average[DM][i]+ch_offset*i-avpredicted) for i in range(par['nrstreams'])] 
                    # set titles,labels,ticks and limits
                    plt.title('DM '+str(DM),fontsize='xx-small')
                    plt.xlabel('time (blocknr)',fontsize='xx-small')
                    plt.ylabel('average+'+str(ch_offset)+'*ch',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                    plt.ylim(ymax=ymax,ymin=ymin)
                    plt.xlim(xmax=par['n'])


                pp.savefig()
                subplot_columns=3
                subplot_rows=5
                subplotmax=subplot_columns*subplot_rows
                figures.append(plt.figure())
                plt.subplots_adjust(wspace=0.6,hspace=1.2)
                subplotcounter=0
                
                pp.savefig()

                # Plot histograms of Triggerlevel difference, standard deviation and averages
                # Make a plot per band
                for st in range(par['nrstreams']):
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.6,hspace=1.2)
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    # plot triggerlevel (this block) - triggerlevel (previous block) in units of 
                    # standard deviation (this block)
                    [plt.hist(np.diff(average[dmm][st]+par['tl']*stddev[dmm][st])/stddev[dmm][st][1:],np.arange(-4,4,0.2),log=True,histtype='step') for dmm in DMvalues2]
                    plt.title('Triggerlevel '+str(par['tl'])+' stream '+str(st),fontsize='xx-small')
                    plt.xlabel('Triggerlevel difference (sigma)',fontsize='xx-small')
                    plt.ylabel('Count (# blocks)',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                    
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        # next page
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.6,hspace=1.2)
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    internoffset=200
                    # plot average - predicted average NOTE this doesn't seem to work too well with the avpredicted for each band for all DMs
                    try:
                        [plt.hist(average[dmm][st][internoffset:]-avpredicted,log=True,histtype='step',bins=np.arange(-10,5,0.1)) for dmm in DMvalues2]
                    except:
                        [plt.hist(average[dmm][st][internoffset:]-avpredicted,log=True,histtype='step',bins=40) for dmm in DMvalues2]
                    plt.title('all DMs, stream '+str(st),fontsize='xx-small')
                    plt.xlabel('Average - Exp average',fontsize='xx-small')
                    plt.ylabel('Count (# blocks)',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')

                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.subplots_adjust(wspace=0.6,hspace=1.2)
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    # plot histogram of standard deviation of each band for all DMs
                    [plt.hist(stddev[dmm][st][internoffset:],log=True,histtype='step',bins=np.arange(0,6,0.1)) for dmm in DMvalues2]
                    plt.title('all DMs, stream '+str(st),fontsize='xx-small')
                    plt.xlabel('Standard deviation',fontsize='xx-small')
                    plt.ylabel('Count (# blocks)',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')

                pp.savefig()
                figures.append(plt.figure())
                plt.suptitle('Threshold '+str(threshold))
                plt.subplot(subplot_rows,subplot_columns,1)
                plt.subplots_adjust(wspace=0.6,hspace=1.0)
                subplotcounter=0
                # calculate histogram of number of coincididences as function of DM
                coinHist=np.array([[(k,v[0][coin-1]) for k,v in DMhist.iteritems()] for coin in range(1,nrbands)])
                subplot_columns=2
                subplot_rows=2
                subplotmax=subplot_columns*subplot_rows
                for coin in range(1,nrbands):
                    subplotcounter+=1
                    if subplotcounter>subplotmax:
                        subplotcounter=1
                        pp.savefig()
                        figures.append(plt.figure())
                        plt.suptitle('Threshold '+str(threshold))
                        plt.subplots_adjust(wspace=0.6,hspace=1.0)
                        plt.suptitle('Threshold '+str(threshold))
                    plt.subplot(subplot_rows,subplot_columns,subplotcounter)
                    if max(coinHist[coin-1,:,1])<1:
                        subplotcounter-=1
                        pass
                    # plot histogram as function of DM for each number of coincidences.
                    plt.bar(coinHist[coin-1,:,0],coinHist[coin-1,:,1],width=0.1,edgecolor=None,align='center')
                    plt.title('Coincidences in '+str(coin)+' chan',fontsize='xx-small')
                    plt.xlabel('DM ',fontsize='xx-small')
                    plt.ylabel('# triggers',fontsize='xx-small')
                    plt.xticks(fontsize='xx-small')
                    plt.yticks(fontsize='xx-small')
                pp.savefig()
                
                pp.close()
                            





                # load average (still need to write this)

                # load stddev ( still need to write this)


# AVERAGE AND STANDARD DEVIATION



# COINCIDENCE TRIGGERS



# MOST NOTABLE TRIGGERS
