#!/usr/bin/env python

# $Id: spectrummaker.py 322 2013-02-04 17:29:35Z fkbreitl $

# LOFAR Solar Imaging Pipeline, Frank Breitling, 2012

# DESCRIPTION: Program for saving spectra from BF HDF5 files to JPG files.


"""Produces and saves dynamic spectra from beam formed HDF5 to JPG files."""

import matplotlib as ml; ml.use('Agg')  #disable any Xwindows backend
import h5py, pylab as pl, numpy as np, time, sys, os, datetime, getopt, \
    multiprocessing

print " ".join(sys.argv[:])

def usage(): sys.exit(
    """USAGE: spectrummaker.py [ OPTIONS ] filename.h5 [ starsec~stopsec ]

Produces and saves dynamic spectra from beam formed HDF5 to JPG/PNG files.

OPTIONS:
      -a, --automatic          run in automatic mode and produce spectra of
                               different resolution for the solar data center
      -h, --help
      -l, --logscale           use logarithmic intensity scale
      -m, --max=1.5        set maximum value            
      -o, --overwrite          overwrite existing JPG files (default is skip)
      -v, --verbose
      startsec~stopsec         set start and stop second for spectrum
""")

automatic=False
logscale=False
overwrite=False
verbose=False
vmax=False

options, remainder = getopt.getopt(sys.argv[1:], 'ahlm:ov', [
        'automatic','help','logscale','max','overwrite','verbose'])

for opt, arg in options:
    if opt in ('-a', '--automatic'): automatic = True
    elif opt in ('-h', '--help'): usage()
    elif opt in ('-l', '--logscale'): logscale = True
    elif opt in ('-m', '--max'): vmax = float(arg)
    elif opt in ('-o', '--overwrite'): overwrite = True
    elif opt in ('-v', '--verbose'): verbose = True

if (len(remainder) == 0): sys.exit(usage())

fn=os.path.abspath(remainder[0])
if not os.path.exists(fn): sys.exit(fn+' does not exist')

if not vmax: vmax=0.3 if logscale else 1.3

ticks10=ml.dates.SecondLocator(bysecond=range(0,60,4))
ticks600=ml.dates.MinuteLocator(byminute=range(0,60,4))
ticks6000=ml.dates.MinuteLocator(byminute=range(0,60,20))
ticks36000=ml.dates.HourLocator(byhour=range(0,24,2))


def medians():
    return np.median(Spectrum[:,::samples/1000],axis=1)[:,np.newaxis]


def plot_medians(medians):
    pl.plot(medians); pl.xlim(0,channels); pl.yscale('log');
    pl.ylabel('median(intensity)'); pl.xlabel('channel no.')
    pl.axes().xaxis.set_minor_locator(ml.ticker.MultipleLocator(chpsb))
    pl.grid(which="minor")
    pl.savefig('channels.jpg', dpi=100)


def average_channels(Spectrum):
    return Spectrum.reshape(sbs,chpsb,-1).mean(1)


def rebin_spectrum(nbins):
    nsamples=samples/nbins*nbins
    return Spectrum[:,0:nsamples].reshape(
        np.shape(Spectrum)[0],nsamples/nbins,-1).mean(2)


def save_jpg(sname, f):
    if verbose: ts=time.time()
    jn=sname+'.jpg'
    if automatic:
        pn=sname+'.png'
        pl.savefig(pn, dpi=100)
        os.system('convert '+pn+' '+jn+'; convert '+pn+' -crop '+str(np.ceil(
                    600*f))+'x480+75+60 -geometry 162%x20% tn/'+jn+';rm '+pn)
    else: pl.savefig(jn, dpi=100)
    if verbose: print jn+' saved in ', time.time()-ts, 's'
    elif not automatic: print jn


#params = {'figure.subplot.left': 0.5}
#pl.rcParams.update(params)

def plot_and_save(s1,s2,c,rebin,spec):
    ts=time.time()
#    pl.clf()
    time_win=(s2-s1)*rebin/samples_per_image
    f=1
    if s2 > samples/rebin:
        f=1.*(samples/rebin-s1)/(time_win*samples_per_image/rebin)

    t1=obsstart+datetime.timedelta(seconds=s1*exposure*rebin)
    t2=obsstart+datetime.timedelta(seconds=s2*exposure*rebin)

    sname=t1.strftime("%Y%m%dT%H%M%S.%f")[0:18]+automatic*(
        '-'+'%05d'%(c*time_win+1))
    if automatic:
        print c+1, str(time_win)+'/'+sname,t2.strftime("%H%M%S.%f")[:-4],s1,s2
    else: print 'Time range: '+t1.strftime(
        "%F %T.%f")[:-4], '-', t2.strftime("%T.%f")[:-4]

    nt1,nt2=ml.dates.date2num([t1,t2])

    fig = pl.figure()
    imax=fig.add_axes([0.09,0.06,0.768*f,0.88])
    cb_label="log(intenisty/median(intensity))"
    if logscale:
        sp=imax.imshow(np.log(spec[:,s1:s2]+1e-9), aspect='auto',
                       vmin=0, vmax=vmax, cmap='gist_heat')
    else:
        cb_label="intenisty/median(intensity)"
        sp=imax.imshow(spec[:,s1:s2], aspect='auto', vmin=1, vmax=vmax,
                       cmap='gist_heat') #,interpolation='None')

    imax.set_xticks([]); imax.set_yticks([])
    imax.set_frame_on(False)

    path=ml.path.Path([[0,0],[s2-s1,0],[s2-s1,len(fH)-1],[0,len(fH)-1],[0,0]])
    patch=ml.patches.PathPatch(path, transform=sp.axes.transData)
    sp.set_clip_path(patch)

    ax=fig.add_axes([0.09,0.1,0.814,0.8], xlim=[nt1,nt2], ylim=[fmax,fmin])
    ax.patch.set_alpha(0)
    #ax.set_xticks([]); ax.set_yticks([])

    if automatic:
        if   time_win==10: ax.xaxis.set_major_locator(ticks10)
        elif time_win==600: ax.xaxis.set_major_locator(ticks600)
        elif time_win==6000: ax.xaxis.set_major_locator(ticks6000)
        elif time_win==36000: ax.xaxis.set_major_locator(ticks36000)
    else:
        locator=''
        if time_win<=120: locator=ml.dates.SecondLocator(bysecond=range(0,60))
        elif time_win<=7200:
            locator= ml.dates.MinuteLocator(byminute=range(0,60))
        else: locator= ml.dates.HourLocator(byhour=range(0,24))
        ax.xaxis.set_major_locator(locator)
        tticks=ax.get_xticks()
        while np.size(tticks)>=6: tticks=tticks[::2]
        ax.set_xticks(tticks)

    ax.xaxis.set_major_formatter(ml.dates.DateFormatter('%H:%M:%S'))
    #ax.xaxis_date()
    ax.tick_params(axis='x', direction='out', width=1.4,length=6,which='both')
    ax.tick_params(axis='y', direction='out', width=1.4,length=6)
    #pl.grid(linestyle='-', linewidth=1, color='w')

    cb=pl.colorbar(sp, fraction=0.0368, pad=0.02, ax=ax)
    cb.set_label(cb_label)

    pl.xlabel('time [h:m:s]')
    pl.ylabel('f [MHz]')
    t=pl.title(bn).set_y(1.03) 

    if verbose: print 'plotting:', time.time()-ts, 's'
    p=multiprocessing.Process(target=save_jpg,args=(sname,f,))
    p.start()
    pl.close()


wd=os.getcwd()
bn=os.path.basename(fn)[:-3]

os.chdir(os.path.dirname(fn))

pl.rcParams['font.size']=14
rebin_res=600

f5=h5py.File(fn,'r')
fmin=f5.attrs.get('OBSERVATION_FREQUENCY_MIN')
fmax=f5.attrs.get('OBSERVATION_FREQUENCY_MAX')
obsstart=datetime.datetime.strptime(f5.attrs.get('OBSERVATION_START_UTC')
                                    [:-4], "%Y-%m-%dT%H:%M:%S.%f")
chpsb=f5.get('SUB_ARRAY_POINTING_000/BEAM_000'
             ).attrs.get('CHANNELS_PER_SUBBAND')
if chpsb==None: 
    chpsb=f5.get('SUB_ARRAY_POINTING_000').attrs.get('CHANNELS_PER_SUBBAND')
STOKES_0=f5.get('SUB_ARRAY_POINTING_000/BEAM_000/STOKES_0')
samples,channels=np.shape(STOKES_0.value[:,:])
sbs=channels/chpsb
f_channels=f5.get('SUB_ARRAY_POINTING_000/BEAM_000/COORDINATES/COORDINATE_1'
                  ).attrs.get('AXIS_VALUES_WORLD')
f_sbs=f_channels.reshape(-1,chpsb).mean(axis=1)
exposure=f5.get('SUB_ARRAY_POINTING_000/BEAM_000').attrs.get('SAMPLING_TIME')
samples_per_image=96 if exposure<0.03 else 32
#print samples_per_image
if exposure==None: exposure=f5.attrs.get('TOTAL_INTEGRATION_TIME')/samples
dt_image=datetime.timedelta(seconds=samples_per_image*exposure)
tstart=time.time()

sec1,sec2 = '',''; 
if len(remainder)>1 and not automatic: sec1,sec2 = remainder[1].split('~')
s1=0 if sec1=='' else int(float(sec1)/exposure)
s2=samples if sec2=='' else int(float(sec2)/exposure)
if s2<=s1: sys.exit('Error: startsec >= stopsec')
Spectrum=np.swapaxes(STOKES_0.value[s1:s2,:],0,1)
medians=medians()
Spectrum=Spectrum[:,:]/medians
Spectrum=average_channels(Spectrum)

fedges=np.arange(f_sbs[0]-2.5e6,f_sbs[-1]+5e6,5e6)
fH = np.histogram(f_sbs, bins=fedges)[0]
for n in range(len(fH)):
    Spectrum[n,:]=Spectrum[n:n+fH[n],:].mean(axis=0)
    Spectrum=np.delete(Spectrum,range(1,fH[n]),axis=0)


#Non automatic mode:
if not automatic:
    os.chdir(wd)
    plot_medians(medians)
    obsstart=obsstart+datetime.timedelta(seconds=s1*exposure)
    s2=s2-s1; spec=Spectrum; rebin=1
    if 1.*s2/samples_per_image>=rebin_res:
        s2=1.*s2/samples_per_image
        rebin=samples_per_image
        spec=rebin_spectrum(samples_per_image)
    plot_and_save(0,s2,-1,rebin,spec)
    sys.exit('Total processing time: '+str(time.time()-tstart)+' s')


#Automatic mode:
if not overwrite and os.path.exists(wd+'/'+bn):
    sys.exit('Warning: Directory exists. Use --overwrite to overwrite.')
os.chdir(wd+'/'+bn)
plot_medians(medians)

spectrum=rebin_spectrum(samples_per_image)
time_res=np.array([3600, 600, 60, 1]) #time_res=np.array([60])
for time_win in time_res*10:
    twd=wd+'/'+bn+'/'+str(time_win)
    if not os.path.exists(twd+'/tn'): os.makedirs(twd+'/tn')
    os.chdir(twd)
    spec=Spectrum; rebin=1
    if time_win >= rebin_res: spec=spectrum; rebin=samples_per_image
    c=0
    for s1 in range(0,samples/rebin,time_win*samples_per_image/rebin):
        s2=s1+time_win*samples_per_image/rebin
        plot_and_save(s1,s2,c,rebin,spec)
        c+=1
    print ('Total duration'+str(time.time()-tstart)+' s')

