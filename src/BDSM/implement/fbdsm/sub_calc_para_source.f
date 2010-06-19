c! CHANGE SRL GAUL FORMAT
        subroutine sub_calc_para_source(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmsisl,scratch,f2,sisl,n1,n2,ctype,crpix,cdelt,
     /    crval,crota,bmar_p,bm_pix,wcs,wcslen,iisl,blc1,blc2,trc1,
     /    trc2,imrms)
        implicit none
        integer cisl,sisl,gpi,n1,n2,h,ntimes
        integer flag(gpi),numsrc,nchar,n,m,nisl,maxmem
        real*8 totfl(gpi),etotfl(gpi),peakfl(gpi),epeakfl(gpi)
        real*8 xpix(gpi),expix(gpi),ypix(gpi),eypix(gpi),bmaj(gpi)
        real*8 ebmaj(gpi),bmin(gpi),ebmin(gpi),bpa(gpi),bmar_p
        real*8 ebpa(gpi),dbmaj(gpi),edbmaj(gpi),dbmin(gpi),imrms(gpi)
        real*8 edbmin(gpi),dbpa(gpi),edbpa(gpi),rmssrc(gpi)
        real*8 avsrc(gpi),rmsisl(gpi),avisl(gpi),chisq(gpi)
        real*8 q(gpi),ra(gpi),dec(gpi),era(gpi),edec(gpi)
        character scratch*500,f2*500,fn*500,ctype(3)*8
        integer islct,islnum(gpi),i,j
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        integer wcslen,iisl(gpi),blc1(gpi),blc2(gpi),trc1(gpi),trc2(gpi)
        integer wcs(wcslen)

c! first get maxmem and nisl. 
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
         read (21) n,m,nisl,maxmem
        close(21)
        call sub_sub_calc(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmsisl,scratch,f2,sisl,n1,n2,nisl,maxmem,
     /    n,m,ctype,crpix,cdelt,crval,crota,bmar_p,bm_pix,wcs,
     /    wcslen,iisl,blc1,blc2,trc1,trc2,imrms)

        return
        end
c!
c!      ==========
c!
c! calc blc, trc and subimage size and pass on.
        subroutine sub_sub_calc(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmsisl,scratch,f2,sisl,n1,n2,nisl,maxmem,
     /    n,m,ctype,crpix,cdelt,crval,crota,bmar_p,bm_pix,wcs,wcslen,
     /    iisl,blc1,blc2,trc1,trc2,imrms)
        implicit none
        integer cisl,sisl,gpi,n1,n2,h,ntimes
        integer flag(gpi),numsrc,n,m,maxmem,nisl,nchar
        real*8 totfl(gpi),etotfl(gpi),peakfl(gpi),epeakfl(gpi)
        real*8 xpix(gpi),expix(gpi),ypix(gpi),eypix(gpi),bmaj(gpi)
        real*8 ebmaj(gpi),bmin(gpi),ebmin(gpi),bpa(gpi),bmar_p
        real*8 ebpa(gpi),dbmaj(gpi),edbmaj(gpi),dbmin(gpi),imrms(gpi)
        real*8 edbmin(gpi),dbpa(gpi),edbpa(gpi),rmssrc(gpi)
        real*8 avsrc(gpi),rmsisl(gpi),avisl(gpi),chisq(gpi)
        real*8 q(gpi),ra(gpi),dec(gpi),era(gpi),edec(gpi)
        character scratch*500,f2*500,fn*500,ctype(3)*8
        integer islct,islnum(gpi),i,j,xpixs(maxmem),ypixs(maxmem)
        integer blc(2),trc(2),subn,subm,npix_isl,delx,dely
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        integer wcslen,wcs(wcslen)
        integer iisl(gpi),blc1(gpi),blc2(gpi),trc1(gpi),trc2(gpi)

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
         read (21) n,m,nisl,maxmem
         if (sisl.gt.nisl) write (*,*) '  $$ ERRORR $$'
         do i=1,2*(sisl-1)
          read (21)
         end do
         read (21) j,npix_isl
         if (j.ne.sisl) write (*,*) '  ##$$ ERRORR $$##'
         call sub_readislpix(21,npix_isl,maxmem,xpixs,ypixs)
         call sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpixs,ypixs,
     /        blc,trc,n,m)
        close(21)
        subn=trc(1)-blc(1)+1
        subm=trc(2)-blc(2)+1
        delx=blc(1)-1
        dely=blc(2)-1       ! coord_im = coord_subim + (delx,dely)

        call sub_sub_sub_calc(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmsisl,scratch,f2,sisl,n1,n2,nisl,maxmem,n,m,
     /    blc,trc,subn,subm,npix_isl,ctype,crpix,cdelt,crval,crota,
     /    delx,dely,xpixs,ypixs,bmar_p,bm_pix,wcs,wcslen,iisl,blc1,
     /    blc2,trc1,trc2,imrms)

        return
        end
c!
c!      ==========
c!
c! calculate the mask so each pixel is identified with a gaussian.
        subroutine sub_sub_sub_calc(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmsisl,scratch,f2,sisl,n1,n2,nisl,maxmem,n,m,
     /    blc,trc,subn,subm,npix_isl,ctype,crpix,cdelt,crval,crota,
     /    delx,dely,xpixs,ypixs,bmar_p,bm_pix,wcs,wcslen,iisl,blc1,
     /    blc2,trc1,trc2,imrms)
        implicit none
        include "includes/constants.inc"
        integer cisl,sisl,gpi,n1,n2,h,ntimes
        integer flag(gpi),numsrc,n,m,maxmem,nisl,delx,dely,sflag
        integer islct,islnum(gpi),i,j,xpixs(maxmem),ypixs(maxmem)
        integer blc(2),trc(2),subn,subm,npix_isl,mask(subn,subm),ngau
        real*8 totfl(gpi),etotfl(gpi),peakfl(gpi),epeakfl(gpi)
        real*8 xpix(gpi),expix(gpi),ypix(gpi),eypix(gpi),bmaj(gpi)
        real*8 ebmaj(gpi),bmin(gpi),ebmin(gpi),bpa(gpi),bmar_p
        real*8 ebpa(gpi),dbmaj(gpi),edbmaj(gpi),dbmin(gpi)
        real*8 edbmin(gpi),dbpa(gpi),edbpa(gpi),rmssrc(gpi)
        real*8 avsrc(gpi),rmsisl(gpi),avisl(gpi),chisq(gpi)
        real*8 q(gpi),ra(gpi),dec(gpi),era(gpi),edec(gpi),imrms(gpi)
        real*8 mompara(6),subim(subn,subm),speak,sra,sdec,total
        character code*4,scratch*500,f2*500,ctype(3)*8,fn*500,rc*2
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),gaus_d(3)
        real*8 maxva,max_peak,mpx,mpy,mra,mdec,dumim(subn,subm)
        integer ii,jj,maxxv,maxyv
        integer wcslen,iisl(gpi),blc1(gpi),blc2(gpi),trc1(gpi),trc2(gpi)
        integer wcs(wcslen)

c! makes mask. value at pixel is SOURCE number to which it belongs (see islnum)
        if (islct.ne.cisl) call make_mask_gausid(cisl,subn,subm,mask,
     /      gpi,peakfl,xpix,ypix,bmaj,bmin,bpa,xpixs,ypixs,maxmem,
     /      npix_isl,cdelt,delx,dely,islct,islnum,subim,flag,sisl)
        if (iisl(1).eq.10)  then
           call imageint2r(mask,subn,subm,subn,subm,dumim)
           fn = 'subim.isl10'
           rc = 'aq'
           call writearray_bin2d(dumim,subn,subm,subn,subm,fn,rc)
        end if

        do i=1,islct    ! for each source in the island
         ntimes=0       ! if single occurence, write out 
         do j=1,cisl    ! cisl is number of gaussians in the island
          if (islnum(j).eq.i) then
           ntimes=ntimes+1
           h=j
          end if
         end do
         if (ntimes.eq.1) then
          code='C  '
          numsrc=numsrc+1
          write (n1,777) numsrc,iisl(h),flag(h),code,totfl(h),etotfl(h),
     /     peakfl(h),epeakfl(h),peakfl(h),epeakfl(h),ra(h),era(h),
     /     dec(h),edec(h),ra(h),era(h),dec(h),edec(h),bmaj(h),
     /     ebmaj(h),bmin(h),ebmin(h),bpa(h),ebpa(h),dbmaj(h),
     /     edbmaj(h),dbmin(h),edbmin(h),dbpa(h),edbpa(h),rmsisl(h),1,
     /     blc1(h),blc2(h),trc1(h),trc2(h),imrms(h)
          write (n2) numsrc,iisl(h),flag(h),code,totfl(h),etotfl(h),
     /     peakfl(h),epeakfl(h),peakfl(h),epeakfl(h),ra(h),era(h),
     /     dec(h),edec(h),ra(h),era(h),dec(h),edec(h),bmaj(h),
     /     ebmaj(h),bmin(h),ebmin(h),bpa(h),ebpa(h),dbmaj(h),
     /     edbmaj(h),dbmin(h),edbmin(h),dbpa(h),edbpa(h),rmsisl(h),1,
     /     blc1(h),blc2(h),trc1(h),trc2(h),imrms(h)
         else
c          maxva=subim(1,1)    ! dont know why this is here. not using it.
c          do ii=1,subn
c           do jj=1,subm
c            if (subim(ii,jj).gt.maxva) then
c             maxva=subim(ii,jj)
c             maxxv=ii
c             maxyv=jj
c           end if
c           end do
c          end do

          call gaul_to_source(islct,mask,subn,subm,subim,bmar_p,
     /     mompara,i,speak,sra,sdec,delx,dely,ctype,crpix,cdelt,
     /     crval,crota,bm_pix,gaus_d,rmsisl(1),max_peak,mpx,
     /     mpy,mra,mdec,sisl,wcs,wcslen)
          code='M  '
          numsrc=numsrc+1
          sflag=0
          ngau=0
          total=0.d0
          do j=1,cisl
           if (islnum(j).eq.i) then
            sflag=sflag+flag(j)
            ngau=ngau+1
            total=total+totfl(j)
           end if
          end do
          write (n1,777) numsrc,iisl(1),sflag,code,total,etotfl(1),
     /     speak,epeakfl(1),max_peak,epeakfl(1),sra,era(1),
     /     sdec,edec(1),mra,era(1),mdec,edec(1),
     /     mompara(4)*abs(cdelt(1))*3600.d0,ebmaj(1),
     /     mompara(5)*abs(cdelt(2))*3600.d0,ebmin(1),
     /     mompara(6)+90.d0,ebpa(1),gaus_d(1)*fwsig*abs(cdelt(1))*
     /     3600.d0,
     /     edbmaj(1),gaus_d(2)*fwsig*abs(cdelt(2))*3600.d0,edbmin(1),
     /     gaus_d(3)+90.d0,edbpa(1),rmsisl(1),ngau,
     /     blc1(1),blc2(1),trc1(1),trc2(1),imrms(1)
          write (n2) numsrc,iisl(1),sflag,code,total,etotfl(1),
     /     speak,epeakfl(1),max_peak,epeakfl(1),sra,era(1),
     /     sdec,edec(1),mra,era(1),mdec,edec(1),
     /     mompara(4)*abs(cdelt(1))*3600.d0,ebmaj(1),
     /     mompara(5)*abs(cdelt(2))*3600.d0,ebmin(1),
     /     mompara(6)+90.d0,ebpa(1),gaus_d(1)*fwsig*abs(cdelt(1))*
     /     3600.d0,
     /     edbmaj(1),gaus_d(2)*fwsig*abs(cdelt(2))*3600.d0,edbmin(1),
     /     gaus_d(3)+90.d0,edbpa(1),rmsisl(1),ngau,
     /     blc1(1),blc2(1),trc1(1),trc2(1),imrms(1)
         end if  ! ntimes=1
        end do  ! i=1,islct

777     format(3(i7,1x),a4,1x,6(1Pe11.3,1x),8(0Pf13.9,1x),
     /         12(0Pf10.5,1x),1(1Pe11.3,1x),i3,4(1x,i5),1Pe11.3)
        return
        end
c!
c!      ==========
c!
