
FC=g77 -fbounds-check
PGLIB = -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L$PGPLOT_DIR -lpgplot
CFITS = -L/strw1/mohan/fitsio_dir/lib -lcfitsio
FFLAG= -c 

OBJNOISE = fit_island.o fit_mulgauss.o bdsm.o sourcemeasure.o readhead_srclist.o \
 sourcelistheaders.o mulsrc.o plotisland.o sub_prog.o imstat.o covsrt.o pgsub.o splint.o \
 spline.o splie2.o splin2.o getsurr_8pix.o mrqmin.o mrqmin2dm.o mrqcof2dm.o fgauss2dne.o \
 mrqmin2d.o mrqcof.o fourn.o mrqcof2d.o fgauss2d.o fgauss2de.o ran1.o gasdev.o fgauss.o \
 conv2.o conv2_get_wid_fac.o conv2_get_gaus.o cr8noisemap.o cr8constantim.o convolveimage.o \
 make_src_residim.o callfitgaussian.o detectdiscont.o medianfilter.o acf.o structfn.o \
 readarray.o writearray.o fileformat.o iland_mat2list.o islandedit.o \
 formislands.o getlistnsigpt.o readinimage.o arrstat.o arrstatmask.o selectnr.o sort2_4.o \
 sort3.o displaytwoim.o check.o displayim.o boxnoiseimage.o calcmedian.o \
 cr8modulationimage.o modulate.o subim.o transposeim.o get_gaus1_iniparm.o combineimage.o \
 callgetpixel.o callputpixel.o slice.o putpixel.o populatesrcs.o getcdf.o fitcdf.o \
 fitinparts.o imagestats.o nsigmacut.o experiment.o getco_ringwidth.o populateinparts.o \
 writeheader.o readheader.o nbit.o callmoment.o operateimage.o copyheader.o puthead.o \
 primarybeam.o filling.o func2.o gaussj.o gser.o gcf.o gammln.o gammq.o poly2.o poly3.o \
 getininp.o readininp.o basicgrey.o formisland_rank.o writefits.o printerror.o deletefile.o \
 splintmod.o read_head_coord.o xy2radec.o callreadfits.o readfitshead.o cr8sources.o \
 get_subim_xsrc.o clip.o associatesrl.o readgaul.o make_associatelist1.o write_associatelist1.o \
 plot_associatelist1.o boxf.o preprocess.o get_keyword.o rlft3.o get_numpix_val.o gammp.o erf.o\
 callrmsd.o bdsm_boxnoiseimage.o put_keyword.o writefitshead.o get_rmsmap.o deconv.o \
 soln_quality.o callwritefits.o read_paradefine.o callwritegaulfits.o callgaul2srl.o sigclip.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o sub_calc_para_source.o \
 make_mask_gausid.o gaul_to_source.o calltest.o testmomentanal.o callwritesrlfits.o \
 writeaipsfiles.o

OBJNOISE2 = sub_simulation.o conv2_get_wid_fac.o cr8sky.o sub_cr8sky.o \
            sim_addnoisrc.o get_subim_ptsrc.o get_subim_xsrc.o get_subim_dubsrc.o \
            get_fitskeywords.o get_mosaicpara.o primarybeam3D_many.o sim_cr8noise.o \
            subim3D.o writearray.o writefits3D.o arrstat.o sigclip.o \
            readarray.o check.o sub_prog.o getcdf.o fitinparts.o \
            gasdev.o mrqmin.o mrqcof.o covsrt.o fgauss.o gaussj.o poly2.o poly3.o \
            func2.o ran1.o getsurr_8pix.o sourcelistheaders.o readhead_srclist.o\
            get_keyword.o read_simparadefine.o set_varypsf.o

OBJNOISE3 = makelists1.o sub_prog.o 

OBJNOISE4 = sub_simulation.o conv2_get_wid_fac.o cr8sky_ilse.o sub_cr8sky.o \
            sim_addnoisrc_ilse.o get_subim_ptsrc.o get_subim_xsrc.o get_subim_dubsrc.o \
            get_fitskeywords.o get_mosaicpara.o primarybeam3D_many.o sim_cr8noise.o \
            subim3D.o writearray.o writefits3D.o arrstat.o sigclip.o \
            readarray.o check.o sub_prog.o getcdf.o fitinparts.o \
            gasdev.o mrqmin.o mrqcof.o covsrt.o fgauss.o gaussj.o poly2.o poly3.o \
            func2.o ran1.o getsurr_8pix.o sourcelistheaders.o readhead_srclist.o\
            get_keyword.o read_simparadefine.o set_varypsf.o

all: noise

noise : noise.f noise.o $(OBJNOISE)
	$(FC) noise.o $(OBJNOISE)   $(CFITS) -o noise

noise.o : noise.f $(OBJNOISE)

simulation : simulation.f $(OBJNOISE2)
	$(FC) simulation.f $(OBJNOISE2) $(CFITS) -o simulation

simulation_ilse : simulation_ilse.f $(OBJNOISE4)
	$(FC) simulation_ilse.f $(OBJNOISE4) $(CFITS) -o simulation_ilse

bdsm-awe : bdsm-awe.f $(OBJNOISE)
	$(FC) bdsm-awe.f $(OBJNOISE)   $(CFITS) -o bdsm-awe

srccomp1 : srccomp1.f $(OBJNOISE3)
	$(FC) srccomp1.f $(OBJNOISE3)   -o srccomp1

a : a.f combineimage.o readarray.o writearray.o sub_prog.o check.o
	$(FC) -o a a.f combineimage.o readarray.o writearray.o sub_prog.o check.o

makelists1.o : makelists1.f sub_prog.o 
sub_simulation.o : sub_simulation.f 
conv2_get_wid_fac.o : conv2_get_wid_fac.f
cr8sky.o : cr8sky.f sub_cr8sky.o sub_prog.o ran1.o sigclip.o writearray.o
cr8sky_ilse.o : cr8sky_ilse.f sub_cr8sky.o sub_prog.o ran1.o sigclip.o writearray.o
sub_cr8sky.o : sub_cr8sky.f ran1.o 
sim_addnoisrc.o : sim_addnoisrc.f primarybeam3D_many.o subim3D.o writearray.o 
sim_addnoisrc_ilse.o : sim_addnoisrc_ilse.f primarybeam3D_many.o subim3D.o writearray.o 
get_subim_ptsrc.o : get_subim_ptsrc.f
get_subim_xsrc.o : get_subim_xsrc.f 
get_subim_dubsrc.o : get_subim_dubsrc.f 
get_fitskeywords.o : get_fitskeywords.f
get_mosaicpara.o : get_mosaicpara.f 
primarybeam3D_many.o : primarybeam3D_many.f readarray.o writearray.o 
sim_cr8noise.o : sim_cr8noise.f conv2_get_wid_fac.o writearray.o gasdev.o
subim3D.o : subim3D.f
writefits3D.o : writefits3D.f 
printerror.o : printerror.f
deletefile.o : deletefile.f 
imagestats.o : imagestats.f
arrstat.o : arrstat.f
arrstatmask.o : arrstatmask.f
pgsub.o : pgsub.f sub_prog.o sourcelistheaders.o plotisland.o get_keyword.o
getcdf.o : getcdf.f
fitcdf.o : fitcdf.f
fitinparts.o : fitinparts.f mrqmin.o
gasdev.o : gasdev.f
mrqmin.o : mrqmin.f mrqcof.o covsrt.o gaussj.o
mrqmin2d.o : mrqmin2d.f mrqcof2d.o covsrt.o gaussj.o
mrqmin2dm.o : mrqmin2dm.f mrqcof2dm.o covsrt.o gaussj.o
mrqcof.o : mrqcof.f fgauss.o poly2.o func2.o poly3.o
mrqcof2d.o : mrqcof2d.f fgauss2de.o
mrqcof2dm.o : mrqcof2dm.f fgauss2de.o fgauss2dne.o
covsrt.o : covsrt.f
splint.o : splint.f
spline.o : spline.f
splintmod.o : splintmod.f
fgauss.o : fgauss.f
gaussj.o : gaussj.f
poly2.o : poly2.f
poly3.o : poly3.f
func2.o : func2.f
ran1.o: ran1.f
erf.o : erf.f gammp.o
gammp.o : gammp.f gcf.o gser.o 
gcf.o : gcf.f gammln.o 
gser.o : gser.f gammln.o 
gammln.o : gammln.f 

combineimage.o : combineimage.f readarray.o writearray.o 
readarray.o : readarray.f sub_prog.o
writearray.o : writearray.f sub_prog.o check.o get_keyword.o
sub_prog.o : sub_prog.f
check.o : check.f

acf.o : acf.f readarray.o writearray.o callfitgaussian.o sub_prog.o sort2_4.o
boxnoiseimage.o : boxnoiseimage.f readarray.o arrstat.o filling.o pgsub.o writearray.o \
      calcmedian.o splie2.o splin2.o sigclip.o

bdsm.o : bdsm.f formislands.o islandedit.o getininp.o iland_mat2list.o plotisland.o \
         readininp.o sourcemeasure.o make_src_residim.o associatesrl.o plot_associatelist1.o \
         preprocess.o callrmsd.o get_keyword.o

plotisland.o : plotisland.f sourcelistheaders.o readarray.o sub_prog.o basicgrey.o \
   getsurr_8pix.o get_keyword.o
basicgrey.o : basicgrey.f sub_prog.o
sourcelistheaders.o : sourcelistheaders.f readhead_srclist.o
readhead_srclist.o : readhead_srclist.f
sourcemeasure.o : sourcemeasure.f readarray.o pgsub.o fit_island.o sub_prog.o get_keyword.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o
sub_sourcemeasure_getblctrc.o : sub_sourcemeasure_getblctrc.f
sub_readislpix.o : sub_readislpix.f
fit_island.o : fit_island.f sub_prog.o fit_mulgauss.o xy2radec.o get_keyword.o
fit_mulgauss.o : fit_mulgauss.f mrqmin2dm.o sub_prog.o arrstatmask.o deconv.o soln_quality.o
soln_quality : soln_quality.f
callgetpixel.o : callgetpixel.f readarray.o sub_prog.o basicgrey.o 
callfitgaussian.o : callfitgaussian.f readarray.o pgsub.o sub_prog.o basicgrey.o \
      get_gaus1_iniparm.o mrqmin2d.o writearray.o displaytwoim.o sigclip.o
callmoment.o : callmoment.f readarray.o pgsub.o  sigclip.o
callputpixel.o : callputpixel.f readarray.o writearray.o putpixel.o
convolveimage.o : convolveimage.f readarray.o pgsub.o conv2.o imstat.o writearray.o \
getininp.o readininp.o boxf.o
box.o : boxf.f conv2.o
copyheader.o : copyheader.f
conv2.o : conv2.f conv2_get_wid_fac.o conv2_get_gaus.o sub_prog.o 
conv2_get_gaus.o : conv2_get_gaus.f
cr8constantim.o : cr8constantim.f writearray.o
cr8modulationimage.o : cr8modulationimage.f ran1.o writearray.o pgsub.o
cr8noisemap.o : cr8noisemap.f writearray.o pgsub.o gasdev.o
detectdiscont.o : detectdiscont.f readarray.o writearray.o
displayim.o : displayim.f readarray.o pgsub.o
displaytwoim.o : displaytwoim.f readarray.o sub_prog.o 
experiment.o : experiment.f rlft3.o fourn.o mulsrc.o
rlft3.o : rlft3.f fourn.o
fourn.o : fourn.f
fileformat.o : fileformat.f readarray.o writearray.o writefits.o 
formislands.o : formislands.f readarray.o pgsub.o sub_prog.o get_keyword.o \
       getlistnsigpt.o getsurr_8pix.o writearray.o get_rmsmap.o sigclip.o
getininp.o : getininp.f
getsurr_8pix.o : getsurr_8pix.f
getlistnsigpt.o : getlistnsigpt.f sort3.o get_keyword.o
iland_mat2list.o : iland_mat2list.f readarray.o sourcemeasure.o
imstat.o : imstat.f sub_prog.o arrstat.o mrqmin.o
imagestats.o : imagestats.f readarray.o arrstat.o pgsub.o sub_prog.o  sigclip.o
islandedit.o : islandedit.f 
medianfilter.o : medianfilter.f readarray.o calcmedian.o filling.o writearray.o pgsub.o
calcmedian.o : calcmedian.f
modulate.o : modulate.f readarray.o imstat.o writearray.o pgsub.o
make_src_residim.o : make_src_residim.f sourcelistheaders.o readarray.o sub_prog.o \
   writearray.o get_keyword.o sigclip.o
nbit.o : nbit.f readarray.o pgsub.o getininp.o readininp.o writearray.o
nsigmacut.o : nsigmacut.f readarray.o pgsub.o writearray.o sigclip.o
operateimage.o : operateimage.f getininp.o readininp.o readarray.o sub_prog.o \
   writearray.o 

populatesrcs.o : populatesrcs.f populateinparts.o fitcdf.o getcdf.o writeheader.o \
   fitinparts.o sigclip.o
populateinparts.o : populateinparts.f sub_prog.o ran1.o arrstat.o pgsub.o writearray.o 
primarybeam.o : primarybeam.f readarray.o writearray.o
putpixel.o : putpixel.f sub_prog.o 
puthead.o : puthead.f 
readheader.o : readheader.f
readinimage.o : readinimage.f 
readininp.o : readininp.f
slice.o : slice.f readarray.o sub_prog.o basicgrey.o 
sort2_4.o : sort2_4.f
sort3.o : sort3.f
structfn.o : structfn.f readarray.o arrstat.o writearray.o acf.o 
subim.o : subim.f readarray.o imagestats.o writearray.o sub_prog.o 
transposeim.o : transposeim.f readarray.o writearray.o
xy2radec.o : xy2radec.f read_head_coord.o
read_head_coord.o : read_head_coord.f get_keyword.o
writefits.o : writefits.f printerror.o deletefile.o sub_prog.o xy2radec.o
callreadfits.o : callreadfits.f printerror.o deletefile.o writearray.o readfitshead.o
readfitshead.o : readfitshead.f printerror.o deletefile.o
writeheader.o : writeheader.f get_keyword.o
cr8sources.o : cr8sources.f sub_prog.o ran1.o get_subim_xsrc.o sourcemeasure.o xy2radec.o \
fit_island.o sub_cr8sky.o cr8noisemap.o writearray.o conv2.o conv2_get_wid_fac.o \
writefitshead.o sigclip.o put_keyword.o
clip.o : clip.f readarray.o imagestats.o writearray.o 
associatesrl.o : associatesrl.f sourcelistheaders.o readgaul.o selectnr.o make_associatelist1.o \
write_associatelist1.o plot_associatelist1.o get_keyword.o
readgaul.o : readgaul.f sub_prog.o
make_associatelist1.o : make_associatelist1.f sub_prog.o
write_associatelist1.o : write_associatelist1.f sub_prog.o
plot_associatelist1.o : plot_associatelist1.f sub_prog.o 
preprocess.o : preprocess.f xy2radec.o read_head_coord.o readarray.o imagestats.o get_keyword.o\
get_numpix_val.o erf.o put_keyword.o writefitshead.o
writefitshead.o : writefitshead.f sub_prog.o
get_numpix_val.o : get_numpix_val.f readarray.o
callrmsd.o : callrmsd.f readarray.o get_keyword.o bdsm_boxnoiseimage.o pgsub.o put_keyword.o
get_keyword.o : get_keyword.f sub_prog.o
bdsm_boxnoise.f : bdsm_boxnoise.o readarray.o arrstat.o filling.o pgsub.o writearray.o \
      calcmedian.o splie2.o splin2.o sigclip.o
put_keyword.o : put_keyword.f
get_rmsmap.o : get_rmsmap.f get_keyword.o sub_prog.o readarray.o
deconv.o : deconv.f
mulsrc.o : mulsrc.f sub_prog.f getsurr_8pix.o readarray.o pgsub.o sigclip.o
callwritefits.o : callwritefits.f sub_prog.o readarray.o get_keyword.o writefits.o
read_paradefine.o : read_paradefine.f sub_prog.o get_keyword.o
read_simparadefine.o : read_simparadefine.f sub_prog.o get_keyword.o
callwritegaulfits.o : callwritegaulfits.f sourcelistheaders.o sub_prog.o printerror.o \
      deletefile.o
callwritesrlfits.o : callwritesrlfits.f sub_prog.o printerror.o \
      deletefile.o readhead_srclist.o
callgaul2srl.o : callgaul2srl.f sourcelistheaders.o sub_prog.o gaul_to_source.o \
      sub_calc_para_source.o
gaul_to_source.o : gaul_to_source.f sub_prog.o sourcemeasure.o
sub_calc_para_source.o : sub_calc_para_source.f sub_prog.o make_mask_gausid.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o
make_mask_gausid.o : make_mask_gausid.f 
sigclip.o : sigclip.f arrstat.o
calltest.o : calltest.f get_keyword.o sub_prog.o getininp.o readininp.o testmomentanal.o
testmomentanal.o : testmomentanal.f ran1.o conv2_get_wid_fac.o cr8noisemap.o conv2.o sigclip.o
set_varypsf.o : set_varypsf.f
writeaipsfiles.o : writeaipsfiles.f sub_prog.o readhead_srclist.o
a.o : a.f

b.o : b.f

clean :
	rm -fr *.o bdsm bdsm-awe noise simulation

# DO NOT DELETE

