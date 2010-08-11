c!
      subroutine sub_psf_cparray(ngau,c_flag,ncut,rgaulid,
     /  rislid,rflag,
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
      implicit none
      integer ngau,ncut,i,nct,c_flag(ngau)
      integer rflag(ngau),rsrcid(ngau),rtrc1(ngau),rtrc2(ngau)
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
      character code(ncut)*4,code4(ncut)*4,names(ncut)*30
      real*8 im_rms(ncut),dumr2(ncut),dumr3(ncut),dumr4(ncut)
      real*8 dumr5(ncut),dumr6(ncut)

      nct=0  
      do i=1,ngau
       if (c_flag(i).eq.0) then
        nct=nct+1
        gaulid(nct)=rgaulid(i)
        islid(nct)=rislid(i)
        flag(nct)=rflag(i)
        tot(nct)=rtot(i)
        dtot(nct)=rdtot(i)
        peak(nct)=rpeak(i)
        epeak(nct)=repeak(i)
        ra(nct)=rra(i)
        era(nct)=rera(i)
        dec(nct)=rdec(i)
        edec(nct)=redec(i)
        xpix(nct)=rxpix(i)
        expix(nct)=rexpix(i)
        ypix(nct)=rypix(i)
        eypix(nct)=reypix(i)
        bmaj(nct)=rbmaj(i)
        ebmaj(nct)=rebmaj(i)
        bmin(nct)=rbmin(i)
        ebmin(nct)=rebmin(i)
        bpa(nct)=rbpa(i)
        ebpa(nct)=rebpa(i)
        dbmaj(nct)=rdbmaj(i)
        edbmaj(nct)=redbmaj(i)
        dbmin(nct)=rdbmin(i)
        edbmin(nct)=redbmin(i)
        dbpa(nct)=rdbpa(i)
        edbpa(nct)=redbpa(i)
        sstd(nct)=rsstd(i)
        sav(nct)=rsav(i)
        rstd(nct)=rrstd(i)
        rav(nct)=rrav(i)
        chisq(nct)=rchisq(i)
        q(nct)=rq(i)
        code(nct)=rcode(i)
        names(nct)=rnames(i)
        blc1(nct)=rblc1(i)
        blc2(nct)=rblc2(i)
        trc1(nct)=rtrc1(i)
        trc2(nct)=rtrc2(i)
        im_rms(nct)=rim_rms(i)
        dumr2(nct)=rdumr2(i)
        dumr3(nct)=rdumr3(i)
        dumr4(nct)=rdumr4(i)
        dumr5(nct)=rdumr5(i)
        dumr6(nct)=rdumr6(i)
       end if
      end do
      if (nct.ne.ncut) write (*,*) ' gaul cp wrong !!!!!'
          
      return
      end

