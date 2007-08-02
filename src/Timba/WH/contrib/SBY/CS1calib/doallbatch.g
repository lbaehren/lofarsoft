include 'imager.g'
include 'viewer.g'

spid:=3
fid:=1
startch:=32
#infile:="L2007_02091_SB3-5.MS";
infile:="L2007_02092_SB15-17.MS";
# CygA
myphasecenter:=dm.direction('J2000', '19h59m28.36','40d44m02.42')
# CasA
#myphasecenter:=dm.direction('J2000', '23h23m24','58d48m54')
# flip CasA
#myphasecenter:=dm.direction('J2000', '12h23m24','58d48m54')
msstr:=sprintf("FIELD_ID==%d AND sumsqr(UVW[1:2]) > 100",fid)
while (startch<224) {
my_model:=sprintf("d%d_%d.model",startch,spid)
my_img:=sprintf("d%d_%d.img",startch,spid)
my_res:=sprintf("d%d_%d.residual",startch,spid)
my_img_restored:=sprintf("d%d_%d.img.restored",startch,spid)
my_img_obs:=sprintf("d%d_%d.img.obs",startch,spid)

myimager:=imager(infile)
myimager.setdata(mode='channel', fieldid=fid, spwid=spid,
             nchan=8,
             start=startch, msselect=msstr,
             step=1, async=F);
#myimager.setimage(nx=1024, ny=1024, cellx='120arcsec', celly='120arcsec',  stokes='I', phasecenter=myphasecenter, doshift=T, fieldid=fid, spwid=spid, mode='channel', nchan=1, start=startch, step=8)
#myimager.setimage(nx=1024, ny=1024, cellx='60arcsec', celly='60arcsec',  stokes='IQUV', phasecenter=myphasecenter, doshift=T, fieldid=fid, spwid=spid, mode='channel', nchan=1, start=startch, step=8)
myimager.setimage(nx=2880, ny=2880, cellx='120arcsec', celly='120arcsec',  stokes='I', phasecenter=myphasecenter, doshift=T, fieldid=fid, spwid=spid, mode='channel', nchan=1, start=startch, step=8)
myimager.setoptions(ftmachine='wproject', wprojplanes=128, padding=1.2 , cache=500000000)
#myimager.clean(algorithm="wfclark" , model=my_model , image=my_img , residual=my_res, threshold=[value=0.0, unit="Jy" ], niter=800, interactive=F, async=F, displayprogress=F)
#myimager.restore(model=my_model,image=my_img_restored,residual=my_res,async=F)
myimager.makeimage(type="corrected",image=my_img);
myimager.close()
myimager.done()

startch:=startch+8
}
