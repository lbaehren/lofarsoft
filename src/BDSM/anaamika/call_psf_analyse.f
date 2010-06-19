c! for actual psf variation study.
c! book keeping here

      subroutine call_psf_analyse(scratch,srldir,fullname,imagename,
     /           plotdir,runcode,n,m,l,l0,filename)
      implicit none
      character fullname*500,imagename*500,runcode*2,f1*500,ffmt*500
      character scratch*500,srldir*500,plotdir*500,generators*500
      character gencode*4,primarygen*500,filename*500
      integer n,m,dumi,l,l0,nisl,ngau,gpi,nffmt,over,error,error1
      real*8 snrcut,nsig,kappa2,snrtop,snrbot,tess_fuzzy,snrcutstack
      integer tess_method
      character tess_sc*1

      write (*,*) '  Calculating psf variation'
      write (*,*) filename(1:100)
      call sourcelistheaders(fullname,f1,n,m,nisl,ngau,gpi,nffmt,
     /       ffmt,srldir) 
        write (*,*) 'a0'
      call read_psfparadefine(generators,nsig,over,kappa2,snrcut,
     /       snrtop,snrbot,snrcutstack,error,error1,gencode,primarygen,
     /       tess_method,tess_sc,tess_fuzzy)
        
      call psf_analyse(scratch,srldir,fullname,imagename,runcode,n,m,
     /  l,l0,nisl,ngau,gpi,snrcut,snrtop,snrbot,kappa2,over,nsig,
     /  generators,plotdir,gencode,primarygen,tess_method,tess_sc,
     /  tess_fuzzy,filename)

      return
      end
c!
c!
c!
      subroutine psf_analyse(scratch,srldir,fullname,imagename,
     /   runcode,n,m,l,l0,nisl,ngau,gpi,snrcut,snrtop,snrbot,kappa2,
     /   over,nsig,generators,plotdir,gencode,primarygen,tess_method,
     /   tess_sc,tess_fuzzy,filename)
      implicit none
      character fullname*500,imagename*500,runcode*2,f1*500,ffmt*500
      character scratch*500,srldir*500,plotdir*500,generators*500
      integer n,m,dumi,l,l0,nisl,ngau,gpi,nffmt,i,ngaucut,c_flag(ngau)
      integer flag(ngau),srcid(ngau),trc1(ngau),trc2(ngau),over
      integer gaulid(ngau),islid(ngau),blc1(ngau),blc2(ngau),tess_method
      real*8 ra(ngau),dec(ngau),rstd(ngau),rav(ngau),sstd(ngau)
      real*8 sav(ngau),tot(ngau),dtot(ngau),snrtop,snrbot
      real*8 bmaj(ngau),bmin(ngau),bpa(ngau),xpix(ngau)
      real*8 expix(ngau),ypix(ngau),eypix(ngau),tess_fuzzy
      real*8 era(ngau),edec(ngau),totfl(ngau),etotfl(ngau),peak(ngau)
      real*8 epeak(ngau),ebmaj(ngau),ebmin(ngau),ebpa(ngau)
      real*8 dbmaj(ngau),edbmaj(ngau),dbmin(ngau),edbmin(ngau)
      real*8 dbpa(ngau),edbpa(ngau),chisq(ngau),q(ngau),snrcut
      character code(ngau)*4,code4(ngau)*4,names(ngau)*30
      character gencode*4,primarygen*500,tess_sc*1,filename*500
      real*8 im_rms(ngau),dumr2(ngau),dumr3(ngau),dumr4(ngau)
      real*8 dumr5(ngau),dumr6(ngau),kappa2,nsig

c! read in .gaul, send snrcutoff number. annoying. stupid fortran. 
        write (*,*) 'a1'
      call readgaul(fullname,ngau,ngau,srldir,gaulid,islid,flag,tot,
     /   dtot,peak,
     /   epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /   bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /   sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,
     /   trc1,trc2,im_rms,dumr2,dumr3,dumr4,dumr5,dumr6)
        write (*,*) 'a2'
c! get  number of gaussians in culled list
      call get_culled_gaullist(ngau,gaulid,islid,flag,tot,dtot,peak,
     /   epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /   bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /   sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,trc1,trc2,
     /   im_rms,dumr2,dumr3,dumr4,dumr5,dumr6,c_flag,ngaucut,n,m,snrcut)

      call sub_psf_analyse(scratch,srldir,fullname,imagename,runcode,
     /  n,m,l,l0,nisl,ngau,gpi,snrcut,gaulid,islid,flag,tot,dtot,peak,
     /   epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /   bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /   sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,trc1,trc2,
     /   im_rms,dumr2,dumr3,dumr4,dumr5,dumr6,c_flag,ngaucut,plotdir,
     /   kappa2,over,nsig,generators,snrtop,snrbot,gencode,primarygen,
     /   tess_method,tess_sc,tess_fuzzy,filename)

      return
      end
c!
c!
c!
      subroutine sub_psf_analyse(scratch,srldir,fullname,imagename,
     /  runcode,n,m,l,l0,nisl,ngau,gpi,snrcut,rgaulid,rislid,rflag,
     /  rtot,rdtot,rpeak,repeak,rra,rera,rdec,redec,rxpix,rexpix,
     /  rypix,reypix,rbmaj,rebmaj,rbmin,rebmin,rbpa,rebpa,rdbmaj,
     /  redbmaj,rdbmin,redbmin,rdbpa,redbpa,rsstd,rsav,rrstd,rrav,
     /  rchisq,rq,rcode,rnames,rblc1,rblc2,rtrc1,rtrc2,
     /  rim_rms,rdumr2,rdumr3,rdumr4,rdumr5,rdumr6,c_flag,ncut,plotdir,
     /  kappa2,over,nsig,generators,snrtop,snrbot,gencode,primarygen,
     /  tess_method,tess_sc,tess_fuzzy,filename)
      implicit none
      character fullname*500,imagename*500,runcode*2,f1*500,ffmt*500
      character scratch*500,srldir*500,f3*500,tess_sc*1,filename*500
      integer n,m,dumi,l,l0,nisl,ngau,gpi,nffmt,i,ncut,c_flag(ngau)
      integer rflag(ngau),rsrcid(ngau),rtrc1(ngau),rtrc2(ngau),nchar
      integer rgaulid(ngau),rislid(ngau),rblc1(ngau),rblc2(ngau)
      real*8 rra(ngau),rdec(ngau),rrstd(ngau),rrav(ngau),rsstd(ngau)
      real*8 rsav(ngau),rtot(ngau),rdtot(ngau)
      real*8 rbmaj(ngau),rbmin(ngau),rbpa(ngau),rxpix(ngau)
      real*8 rexpix(ngau),rypix(ngau),reypix(ngau),rera(ngau)
      real*8 redec(ngau),rtotfl(ngau),retotfl(ngau),rpeak(ngau)
      real*8 repeak(ngau),rebmaj(ngau),rebmin(ngau),rebpa(ngau)
      real*8 rdbmaj(ngau),redbmaj(ngau),rdbmin(ngau),redbmin(ngau)
      real*8 rdbpa(ngau),redbpa(ngau),rchisq(ngau),rq(ngau),snrcut
      character rcode(ngau)*4,rcode4(ngau)*4,rnames(ngau)*30
      real*8 rim_rms(ngau),rdumr2(ngau),rdumr3(ngau),rdumr4(ngau)
      real*8 rdumr5(ngau),rdumr6(ngau)
      integer flag(ncut),srcid(ncut),trc1(ncut),trc2(ncut)
      integer gaulid(ncut),islid(ncut),blc1(ncut),blc2(ncut)
      real*8 ra(ncut),dec(ncut),rstd(ncut),rav(ncut),sstd(ncut)
      real*8 sav(ncut),tot(ncut),dtot(ncut)
      real*8 bmaj(ncut),bmin(ncut),bpa(ncut),xpix(ncut)
      real*8 expix(ncut),ypix(ncut),eypix(ncut)
      real*8 era(ncut),edec(ncut),totfl(ncut),etotfl(ncut),peak(ncut)
      real*8 epeak(ncut),ebmaj(ncut),ebmin(ncut),ebpa(ncut)
      real*8 dbmaj(ncut),edbmaj(ncut),dbmin(ncut),edbmin(ncut)
      real*8 dbpa(ncut),edbpa(ncut),chisq(ncut),q(ncut)
      character code(ncut)*4,code4(ncut)*4,names(ncut)*30,plotdir*500
      real*8 im_rms(ncut),dumr2(ncut),dumr3(ncut),dumr4(ncut)
      real*8 dumr5(ncut),dumr6(ncut),tess_fuzzy
      character ctype(3)*8,extn*20,keyword*500,keystrng*500
      character comment*500,generators*500,gencode*4,primarygen*500
      real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
      real*8 cbmaj,cbmin,cbpa,nsig,kappa2,snrtop,snrbot
      integer error,over,tess_method

      call sub_psf_cparray(ngau,c_flag,ncut,rgaulid,rislid,rflag,
     /  rtot,rdtot,rpeak,repeak,rra,rera,rdec,redec,rxpix,rexpix,
     /  rypix,reypix,rbmaj,rebmaj,rbmin,rebmin,rbpa,rebpa,rdbmaj,
     /  redbmaj,rdbmin,redbmin,rdbpa,redbpa,rsstd,rsav,rrstd,rrav,
     /  rchisq,rq,rcode,rnames,rblc1,rblc2,rtrc1,rtrc2,
     /  rim_rms,rdumr2,rdumr3,rdumr4,rdumr5,rdumr6,gaulid,islid,flag,
     /  tot,dtot,peak,epeak,ra,era,dec,edec,xpix,expix,
     /  ypix,eypix,bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,
     /  edbmaj,dbmin,edbmin,dbpa,edbpa,sstd,sav,rstd,rav,
     /  chisq,q,code,names,blc1,blc2,trc1,trc2,
     /  im_rms,dumr2,dumr3,dumr4,dumr5,dumr6)

      f3=fullname(1:nchar(fullname))//'.header'
      call read_head_coord(f3(1:nchar(f3)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
      extn='.header'
      keyword='BMAJ'
      call get_keyword(fullname,extn,keyword,keystrng,cbmaj,
     /     comment,'r',scratch,error)
      keyword='BMIN'
      call get_keyword(fullname,extn,keyword,keystrng,cbmin,
     /     comment,'r',scratch,error)
      keyword='BPA'
      call get_keyword(fullname,extn,keyword,keystrng,cbpa,
     /     comment,'r',scratch,error)

      write (*,*) filename(1:100)
      call main_psf_analyse(scratch,srldir,fullname,imagename,
     /  runcode,n,m,l,l0,nisl,ngau,gpi,snrcut,ncut,gaulid,islid,flag,
     /  tot,dtot,peak,epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,
     /  ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /  sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,trc1,trc2,
     /  im_rms,dumr2,dumr3,dumr4,dumr5,dumr6,plotdir,bm_pix,
     /  cbmaj,cbmin,cbpa,kappa2,over,nsig,generators,snrtop,snrbot,
     /  gencode,primarygen,tess_method,tess_sc,tess_fuzzy,filename,
     /  cdelt)

      return
      end
c!
c!
