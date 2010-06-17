c!
        subroutine main_psf_analyse(scratch,srldir,fullname,imagename,
     /    runcode,n,m,l,l0,nisl,ngau,gpi,snrcut,ncut,gaulid,islid,flag,
     /    tot,dtot,peak,epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,
     /    edbpa,sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,trc1,
     /    trc2,im_rms,dumr2,dumr3,dumr4,dumr5,dumr6,plotdir,bm_pix,
     /    cbmaj,cbmin,cbpa,kappa2,over,nsig,generators,snrtop,snrbot,
     /    gencode,primarygen,tess_method,tess_sc,tess_fuzzy,filename,
     /    cdelt)
        implicit none
        character fullname*500,imagename*500,runcode*2,f1*500,ffmt*500
        character scratch*500,srldir*500,plotdir*500,generators*500
        integer n,m,dumi,l,l0,nisl,ngau,gpi,nffmt,i,ncut,f_sclip(ncut)
        integer flag(ncut),srcid(ncut),trc1(ncut),trc2(ncut),ngens
        integer gaulid(ncut),islid(ncut),blc1(ncut),blc2(ncut),ngensS
        integer volnum(ncut),ntiles,error,error1,over,tess_method
        real*8 ra(ncut),dec(ncut),rstd(ncut),rav(ncut),sstd(ncut)
        real*8 sav(ncut),tot(ncut),dtot(ncut),snrcut,kappa2
        real*8 bmaj(ncut),bmin(ncut),bpa(ncut),xpix(ncut)
        real*8 expix(ncut),ypix(ncut),eypix(ncut),cdelt(3)
        real*8 era(ncut),edec(ncut),totfl(ncut),etotfl(ncut),peak(ncut)
        real*8 epeak(ncut),ebmaj(ncut),ebmin(ncut),ebpa(ncut)
        real*8 dbmaj(ncut),edbmaj(ncut),dbmin(ncut),edbmin(ncut)
        real*8 dbpa(ncut),edbpa(ncut),chisq(ncut),q(ncut)
        character code(ncut)*4,code4(ncut)*4,names(ncut)*30
        character gencode*4,primarygen*500,tess_sc*1,filename*500
        real*8 im_rms(ncut),dumr2(ncut),dumr3(ncut),dumr4(ncut)
        real*8 dumr5(ncut),dumr6(ncut),bm_pix(3),cbmaj,cbmin,cbpa
        real*8 s_med_f(3),s_av_f(3),s_av_c_f(3),s_rms_f(3),tess_fuzzy
        real*8 s_med_c_f(3),s_rms_c_f(3),nsig,wtavbm(3),wtstdbm(3)
        real*8 s_c1(2),s_c2(2),s_dm1(3),s_dm2(3),snrtop,snrbot
        
c! first, get median and mean psf size for whole catalogue (gaussian size) and see if ok,
c! compare with beam, plot statistics etc. s=stats, f=full cata, r=rms, c=clip
        call psfvary_fullstat(plotdir,scratch,ncut,bmaj,bmin,bpa,ebmaj,
     /    ebmin,ebpa,peak,bm_pix,cbmaj,cbmin,cbpa,im_rms,s_med_f,
     /    s_av_f,s_av_c_f,s_rms_f,s_rms_c_f,s_med_c_f,fullname,nsig)
      write (*,*) s_av_f
      write (*,*) s_av_c_f
      write (*,*) s_rms_f
      write (*,*) s_rms_c_f

c! next, fit sigma/mean =sqrt(c1^2 + c2^2/SNR^2) like in analyse_asrl
c! but use median instead of mean. and so kappa-sigma clipping on this
c! to get kappa-sigma clipped c1 and c2 !!
        call size_ksclip_wenss(plotdir,fullname,ncut,bmaj,bmin,bpa,
     /    peak,im_rms,s_med_f,s_rms_c_f,nsig,cbmaj,cbmin,cbpa,
     /    s_c1,s_c2,s_dm1,s_dm2,f_sclip,over,kappa2,filename,
     /    scratch,srldir,ra,dec,bm_pix,cdelt)

c! analyse for psf not equal to synthesized beam
        call av_psf(ncut,bmaj,bmin,bpa,cbmaj,cbmin,cbpa,peak,im_rms,
     /       wtavbm,wtstdbm,f_sclip)

c! divide image into facets and get sources which are kappa-sigma clipped 
        call get_voronoi_generators(scratch,n,m,peak,im_rms,imagename,
     /   fullname,snrcut,snrtop,snrbot,ngens,ngensS,f_sclip,ncut,
     /   xpix,ypix,generators,gencode,primarygen)
        if (ngensS.gt.0) then ! for index of xgensS etc
         dumi=ngensS
        else
         dumi=1
        end if
        call do_voronoi(n,m,imagename,ncut,ngens,ngensS,dumi,peak,
     /    im_rms,ntiles,volnum,xpix,ypix,scratch,fullname,generators,
     /    gencode,primarygen,tess_method,tess_sc,tess_fuzzy)

c! get the beam para for each of these facets and also median shapelet decomposition
c        call get_beam_tile()

c! see if there is statistically significant variation  

        return
        end


