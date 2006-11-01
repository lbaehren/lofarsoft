c! This converts the gaussian list .gaul to a source list .srl (ascii and binary).
c! callwritesrlfits will convert this to fits seperately.
c!
c! Look at notes. basic stuff now. Check inside island only, and if two gaussians are
c! connected at > isl_thresh in reconstructed image or dist is < f1 X fwhms then merge.
c!
c! CHANGE GAUL FORMAT

        subroutine callgaul2srl(fn,srldir,scratch,f2)
        implicit none
        character srldir*500,f2*500,scratch*500,fn*500,ffmt*500
        character f1*500,writeout*500
        integer nsrc,gpi,nisl,nffmt,n,m,nchar
        real*8 dumr

        write (*,*) '  Converting gaul to srl, modifying .gaul file '
        call sourcelistheaders(f2,f1,n,m,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)
        call gaul2srl(fn,srldir,scratch,f2,nisl,nsrc,gpi)

        return
        end
c!
c!      ----------
c!
        subroutine gaul2srl(fn,srldir,scratch,f2,nisl,nsrc,gpi)
        implicit none
        include "constants.inc"
        character srldir*500,f2*500,scratch*500,f1*500,ffmt*500
        character fname*500,sname*500,snameb*500,comment*500
        character fn*500,extn*500,keyword*500,keystrng*500
        character hdrfile*500,ctype(3)*8,srcnf*500,head*1000
        character fnameb*500,ftemp*500,ftempb*500,dums*1000
        integer n,m,nsrc,nffmt,nisl,nchar,gpi,gauln(gpi),headint
        real*8 totfl(gpi),etotfl(gpi),peakfl(gpi),epeakfl(gpi)
        real*8 xpix(gpi),expix(gpi),ypix(gpi),eypix(gpi),bmaj(gpi)
        real*8 ebmaj(gpi),bmin(gpi),ebmin(gpi),bpa(gpi)
        real*8 ebpa(gpi),dbmaj(gpi),edbmaj(gpi),dbmin(gpi)
        real*8 edbmin(gpi),dbpa(gpi),edbpa(gpi),rmssrc(gpi)
        real*8 avsrc(gpi),rmsisl(gpi),avisl(gpi),chisq(gpi)
        real*8 q(gpi),ra(gpi),dec(gpi),era(gpi),edec(gpi)
        real*8 thresh_isl,rms_clip,cbmaj,cbmin,bmar_p,cbpa
        integer iisl(gpi),isrc(gpi),flag(gpi),srcnum(gpi),i,snum,snumt
        integer ctot,cisl,islold,src,isl,numsrc,srcctr1,srcctr2
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)

        extn='.header'
        keyword='CDELT1'
        call get_keyword(fn,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(fn,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        keyword='BMAJ'
        call get_keyword(fn,extn,keyword,keystrng,cbmaj,
     /       comment,'r',scratch)
        keyword='BMIN'
        call get_keyword(fn,extn,keyword,keystrng,cbmin,
     /       comment,'r',scratch)
        keyword='BPA'
        call get_keyword(fn,extn,keyword,keystrng,cbpa,
     /       comment,'r',scratch)
        extn='.bstat'
        keyword='thresh_isl'
        call get_keyword(fn,extn,keyword,keystrng,thresh_isl,
     /       comment,'r',scratch)
        keyword='rms_clip'
        call get_keyword(fn,extn,keyword,keystrng,rms_clip,
     /       comment,'r',scratch)
        bmar_p=2.d0*pi*cbmaj*cbmin/(cdelt(1)*cdelt(2)*fwsig*fwsig)

        hdrfile=fn(1:nchar(fn))//'.header'
        call read_head_coord(hdrfile,3,ctype,crpix,cdelt,
     /        crval,crota,bm_pix)
        bm_pix(1)=cbmaj/cdelt(1)
        bm_pix(2)=cbmin/cdelt(1)
        bm_pix(3)=cbpa

        ctot=0
        cisl=0
        islold=1
        numsrc=0
        srcctr1=0
        srcctr2=0
        fname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'
        fnameb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'
        sname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'
        snameb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.bin'
        srcnf=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.snum'
        open(unit=25,file=fname)
        open(unit=27,file=fnameb,form='unformatted')
        open(unit=22,file=sname)
        open(unit=23,file=snameb,form='unformatted')
        open(unit=24,file=srcnf,form='unformatted')

c! copy headers to .srl
337     read (25,'(a)') head
        if (head(2:5).ne.'gaul') then
         write (22,'(a)') head(1:nchar(head))
         goto 337
        end if
        write (22,*) 'src# flag code tot_Jy err CenPkJy err MaxPkJy err'
     /   //' CenRA err CenDec err MaxRA err MaxDec err bmaj_asec_fw err'
     /   //' bmin_asec_fw err bpa_deg err deconv_bmaj_bmin_bpa_asec_fw '
     /   //' &errors rms_isl num_gaus'
        write (22,*) 'fmt 76 '//'"(2(i4,1x),a4,1x,6(1Pe11.3,1x),'//
     /        '8(0Pf13.9,1x),12(0Pf10.5,1x),1(1Pe11.3,1x),i3)"'
        
c! do actual processing here
333     continue
        if (ctot.eq.nsrc) goto 334
        read (27) src,isl
        ctot=ctot+1
        if (isl.ne.islold) then
         ctot=ctot-1
         call process_g2s(22,23,24,gpi,cisl,islold,flag,totfl,etotfl,
     /    peakfl,epeakfl,ra,era,dec,edec,xpix,expix,ypix,eypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmssrc,avsrc,rmsisl,avisl,chisq,q,numsrc,
     /    thresh_isl,rms_clip,ctype,crpix,cdelt,crval,crota,scratch,
     /    f2,bmar_p,bm_pix,gauln,srcctr1,srcctr2,snumt)
         islold=isl
         cisl=0
         backspace(27)
         goto 333
        else
         cisl=cisl+1
         backspace(27)
         read (27) gauln(cisl),isl,flag(cisl),totfl(cisl),etotfl(cisl),
     /    peakfl(cisl),epeakfl(cisl),ra(cisl),era(cisl),dec(cisl),
     /    edec(cisl),xpix(cisl),expix(cisl),ypix(cisl),eypix(cisl),
     /    bmaj(cisl),ebmaj(cisl),bmin(cisl),ebmin(cisl),bpa(cisl),
     /    ebpa(cisl),dbmaj(cisl),edbmaj(cisl),dbmin(cisl),edbmin(cisl),
     /    dbpa(cisl),edbpa(cisl),rmssrc(cisl),avsrc(cisl),rmsisl(cisl),
     /    avisl(cisl),chisq(cisl),q(cisl),srcnum(cisl)
         goto 333
        end if
334     continue
        call process_g2s(22,23,24,gpi,cisl,isl,flag,totfl,etotfl,
     /   peakfl,epeakfl,ra,era,dec,edec,xpix,expix,ypix,eypix,
     /   bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /   dbpa,edbpa,rmssrc,avsrc,rmsisl,avisl,chisq,q,numsrc, 
     /   thresh_isl,rms_clip,ctype,crpix,cdelt,crval,crota,scratch,
     /   f2,bmar_p,bm_pix,gauln,srcctr1,srcctr2,snumt)
        close(27)
        close(22)
        close(23)
        close(24)

c! now add srcctr to the last column of .gaul
        fname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'
        fnameb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'
        ftemp=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.scr'
        ftempb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.scrb'
        srcnf=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.snum'
        open(unit=22,file=fname)
        open(unit=24,file=ftemp)
        open(unit=27,file=ftempb,form='unformatted')
        open(unit=28,file=srcnf,form='unformatted')

335     read (22,'(a)') head
        write (24,'(a)') head(1:nchar(head))
        if (head(1:3).ne.'fmt') goto 335
        backspace(22)
        read (22,*) head,headint,ffmt
        do i=1,src
         read (28) snum
         read (22,ffmt) gauln(cisl),isl,flag(cisl)
         if (flag(cisl).eq.0) then
          backspace(22)
          read (22,ffmt) gauln(cisl),isl,flag(cisl),
     /     totfl(cisl),etotfl(cisl),
     /     peakfl(cisl),epeakfl(cisl),ra(cisl),era(cisl),dec(cisl),
     /     edec(cisl),xpix(cisl),expix(cisl),ypix(cisl),eypix(cisl),
     /     bmaj(cisl),ebmaj(cisl),bmin(cisl),ebmin(cisl),bpa(cisl),
     /     ebpa(cisl),dbmaj(cisl),edbmaj(cisl),dbmin(cisl),edbmin(cisl),
     /     dbpa(cisl),edbpa(cisl),rmssrc(cisl),avsrc(cisl),rmsisl(cisl),
     /     avisl(cisl),chisq(cisl),q(cisl),srcnum(cisl)
         else  ! terrible terrible hack. shud read anyway and write, as string. do later.
          totfl(cisl)=0.d0
          etotfl(cisl)=0.d0
          peakfl(cisl)=0.d0
          epeakfl(cisl)=0.d0
          ra(cisl)=0.d0
          era(cisl)=0.d0
          dec(cisl)=0.d0
          edec(cisl)=0.d0
          xpix(cisl)=0.d0
          expix(cisl)=0.d0
          ypix(cisl)=0.d0
          eypix(cisl)=0.d0
          bmaj(cisl)=0.d0
          ebmaj(cisl)=0.d0
          bmin(cisl)=0.d0
          ebmin(cisl)=0.d0
          bpa(cisl)=0.d0
          ebpa(cisl)=0.d0
          dbmaj(cisl)=0.d0
          edbmaj(cisl)=0.d0
          dbmin(cisl)=0.d0
          edbmin(cisl)=0.d0
          dbpa(cisl)=0.d0
          edbpa(cisl)=0.d0
          rmssrc(cisl)=0.d0
          avsrc(cisl)=0.d0
          rmsisl(cisl)=0.d0
          avisl(cisl)=0.d0
          chisq(cisl)=0.d0
          q(cisl)=0.d0
          snum=0
         end if
         write (24,ffmt) gauln(cisl),isl,flag(cisl),
     /    totfl(cisl),etotfl(cisl),
     /    peakfl(cisl),epeakfl(cisl),ra(cisl),era(cisl),dec(cisl),
     /    edec(cisl),xpix(cisl),expix(cisl),ypix(cisl),eypix(cisl),
     /    bmaj(cisl),ebmaj(cisl),bmin(cisl),ebmin(cisl),bpa(cisl),
     /    ebpa(cisl),dbmaj(cisl),edbmaj(cisl),dbmin(cisl),edbmin(cisl),
     /    dbpa(cisl),edbpa(cisl),rmssrc(cisl),avsrc(cisl),rmsisl(cisl),
     /    avisl(cisl),chisq(cisl),q(cisl),snum
         write (27) gauln(cisl),isl,flag(cisl),
     /    totfl(cisl),etotfl(cisl),
     /    peakfl(cisl),epeakfl(cisl),ra(cisl),era(cisl),dec(cisl),
     /    edec(cisl),xpix(cisl),expix(cisl),ypix(cisl),eypix(cisl),
     /    bmaj(cisl),ebmaj(cisl),bmin(cisl),ebmin(cisl),bpa(cisl),
     /    ebpa(cisl),dbmaj(cisl),edbmaj(cisl),dbmin(cisl),edbmin(cisl),
     /    dbpa(cisl),edbpa(cisl),rmssrc(cisl),avsrc(cisl),rmsisl(cisl),
     /    avisl(cisl),chisq(cisl),q(cisl),snum
        end do
        close(22)
        close(24)
        close(27)
        close(28)
        call system('mv -f '//ftemp(1:nchar(ftemp))//' '//
     /       fname(1:nchar(fname)))
        call system('mv -f '//ftempb(1:nchar(ftempb))//' '//
     /       fnameb(1:nchar(fnameb)))

c! add number of sources in .srl file header
        fname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'
        call system('rm -f '//scratch(1:nchar(scratch))//'a')
        open(unit=21,file=scratch(1:nchar(scratch))//'a')
        write (21,*) '" Number_of_sources ',snumt,'"'
        close(21)
        open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
        read (21,*) dums
        close(21)
        dums="sed -e '6a\\"//dums(1:nchar(dums))//"' "//
     /    fname(1:nchar(fname))//" > "//scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /    fname(1:nchar(fname)))


        return
        end
c!
c!      ==========
c! cisl is number of gaussians in that island. final islnum of a gaussian is the number of
c! the sub-island/source it belongs to. numsrc keeps count of global number of sources found.
c! islct is number of distinct sources found in that island.
c!
        subroutine process_g2s(n1,n2,n3,gpi,cisl,sisl,flag,totfl,etotfl,
     /    peakfl,epeakfl,ra,era,dec,edec,xpix,expix,ypix,eypix,
     /    bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,
     /    dbpa,edbpa,rmssrc,avsrc,rmsisl,avisl,chisq,q,numsrc,
     /    thresh_isl,rms_clip,ctype,crpix,cdelt,crval,crota,scratch,
     /    f2,bmar_p,bm_pix,gauln,srcctr1,srcctr2,snumt)
        implicit none
        integer ctot,cisl,islold,src,sisl,gpi,n1,n2,h,ntimes,n3
        integer iisl(gpi),isrc(gpi),flag(gpi),numsrc,gauln(gpi)
        real*8 totfl(gpi),etotfl(gpi),peakfl(gpi),epeakfl(gpi)
        real*8 xpix(gpi),expix(gpi),ypix(gpi),eypix(gpi),bmaj(gpi)
        real*8 ebmaj(gpi),bmin(gpi),ebmin(gpi),bpa(gpi)
        real*8 ebpa(gpi),dbmaj(gpi),edbmaj(gpi),dbmin(gpi)
        real*8 edbmin(gpi),dbpa(gpi),edbpa(gpi),rmssrc(gpi)
        real*8 avsrc(gpi),rmsisl(gpi),avisl(gpi),chisq(gpi)
        real*8 q(gpi),ra(gpi),dec(gpi),era(gpi),edec(gpi)
        real*8 rms_clip,thresh_isl,dumr,bmar_p
        character code*4,scratch*500,f2*500,ctype(3)*8
        logical same_island,isint
        integer islct,islnum(gpi),i,j,i1,dumi,dmax,inte,nextint
        integer srcctr1,srcctr2,snumt
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)

        if (cisl.eq.1) then
         islct=cisl
         numsrc=numsrc+1
         code='S   '
         write (n1,777) numsrc,flag(1),code,totfl(1),etotfl(1),
     /    peakfl(1),epeakfl(1),peakfl(1),epeakfl(1),ra(1),era(1),
     /    dec(1),edec(1),ra(1),era(1),dec(1),edec(1),bmaj(1),
     /    ebmaj(1),bmin(1),ebmin(1),bpa(1),ebpa(1),dbmaj(1),
     /    edbmaj(1),dbmin(1),edbmin(1),dbpa(1),edbpa(1),rmsisl(1),1
         write (n2) numsrc,flag(1),code,totfl(1),etotfl(1),
     /    peakfl(1),epeakfl(1),peakfl(1),epeakfl(1),ra(1),era(1),
     /    dec(1),edec(1),ra(1),era(1),dec(1),edec(1),bmaj(1),
     /    ebmaj(1),bmin(1),ebmin(1),bpa(1),ebpa(1),dbmaj(1),
     /    edbmaj(1),dbmin(1),edbmin(1),dbpa(1),edbpa(1),rmsisl(1),1
        else
         islct=cisl
         do i=1,cisl
          islnum(i)=i
         end do
         do i=1,cisl
          do j=i,cisl
           if (i.ne.j) then
            if (same_island(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,thresh_isl,rms_clip,cdelt)) then
             islct=islct-1
             do i1=1,cisl
              if (islnum(i1).eq.max(islnum(i),islnum(j))) 
     /           islnum(i1)=min(islnum(i),islnum(j))
             end do 
            end if
           end if
          end do
         end do
c! calc islct here cos if done inside loop, u count multiple times for eg if relate i-j, i-k and then j-k.
c! islnum has holes so first de-hole-islnum and then calc islct.
         call rangeint(islnum,gpi,cisl,dumi,dmax)
         do inte=1,dmax
          call callisint(islnum,gpi,cisl,inte,isint)
          nextint=inte
444       continue
          if (.not.isint.and.nextint.lt.cisl) then
           nextint=nextint+1
           call callisint(islnum,gpi,cisl,nextint,isint)
           goto 444
          else
           do i=1,cisl
            if (islnum(i).eq.nextint) islnum(i)=inte
           end do
          end if
         end do
         call rangeint(islnum,gpi,cisl,dumi,islct)
c! now to calculate parameters for sources
c! first create mask where each pixel has gaus_id to which it belongs. want to pass it and not store 
         call sub_calc_para_source(gpi,islct,cisl,islnum,numsrc,
     /    flag,totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,ypix,
     /    bmaj,ebmaj,bmin,ebmin,
     /    bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rmsisl,scratch,
     /    f2,sisl,n1,n2,ctype,crpix,cdelt,crval,crota,bmar_p,
     /    bm_pix)
        end if  ! cisl=1
c! now store the srcnum for each gaussian to add back to last column of .gaul
        snumt=0
        do i=1,cisl
         srcctr1=srcctr2+islnum(i)
         write (n3) srcctr1
         if (srcctr1.gt.snumt) snumt=srcctr1
        end do
        srcctr2=srcctr2+islct
        
777     format(2(i4,1x),a4,1x,6(1Pe11.3,1x),8(0Pf13.9,1x),
     /         12(0Pf10.5,1x),1(1Pe11.3,1x),i3)

        return
        end
c!
c!      ==========
c!
        function same_island(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,thresh_isl,rms_clip,cdelt)
        implicit none
        integer i,j,i1,j1,gpi,cisl,maxline
        logical same_island,s_isl1,s_isl2
        real*8 peakfl(gpi),xpix(gpi),ypix(gpi),bmaj(gpi)
        real*8 bmin(gpi),bpa(gpi),cdelt(3),rms_clip,thresh_isl
        
        same_island=.false.
        s_isl1=.false.
        s_isl2=.false.

c! if reconstructed value more than island thresh all along line joining pts then same.
        maxline=max(abs(xpix(i)-xpix(j))+1,abs(ypix(i)-ypix(j))+1)
        call same_isl_min1(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,cdelt,maxline,thresh_isl,rms_clip,s_isl1)
        call same_isl_dist1(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,cdelt,maxline,thresh_isl,rms_clip,s_isl2)

        same_island=s_isl1.and.s_isl2

        return
        end
c!
c!      ==========
c!
        subroutine same_isl_min1(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,cdelt,maxline,thresh_isl,rms_clip,same_island)
        implicit none
        integer i,j,i1,j1,gpi,cisl,maxline,round,dumi
        logical same_island
        real*8 peakfl(gpi),xpix(gpi),ypix(gpi),bmaj(gpi),cdelt(3)
        real*8 bmin(gpi),bpa(gpi),xline(maxline),yline(maxline)
        real*8 rpixval,min_pval,rms_clip,thresh_isl

c! first get the pixels in between
        if (abs(xpix(j)-xpix(i)).gt.abs(ypix(j)-ypix(i))) then
         do i1=1,maxline
          xline(i1)=round(min(xpix(i),xpix(j))+(i1-1))
          yline(i1)=round((ypix(i)-ypix(j))/(xpix(i)-xpix(j))*
     /           (min(xpix(i),xpix(j))+(i1-1)-xpix(i))+ypix(i))
         end do
        else
         do i1=1,maxline
          yline(i1)=round(min(ypix(i),ypix(j))+(i1-1))
          xline(i1)=round((xpix(i)-xpix(j))/(ypix(i)-ypix(j))*
     /           (min(ypix(i),ypix(j))+(i1-1)-ypix(i))+xpix(i))
         end do
        end if
        
        min_pval=99.d99
        do i1=1,maxline
         call get_rpixval(xline(i1),yline(i1),cisl,gpi,peakfl,xpix,
     /        ypix,bmaj,bmin,bpa,cdelt,rpixval)   ! get reconstructed pixel value
         if (min_pval.gt.rpixval) then 
          min_pval=rpixval
          dumi=i1
         end if
        end do
        if (dumi.eq.1.or.dumi.eq.maxline) same_island=.true.  ! no minimum !
        if (abs(min_pval-min(peakfl(i),peakfl(j))).le.
     /      rms_clip*thresh_isl) same_island=.true.

        return
        end
c!
c!      ==========
c!
        subroutine get_rpixval(x,y,cisl,gpi,peakfl,xpix,ypix,bmaj,bmin,
     /         bpa,cdelt,rpixval)   
        implicit none
        include "constants.inc"
        integer i,j,gpi,cisl,round
        logical same_island
        real*8 peakfl(gpi),xpix(gpi),ypix(gpi),bmaj(gpi),cdelt(3)
        real*8 bmin(gpi),bpa(gpi),x,y,spa,cpa
        real*8 a4,a5,a6,dr1,dr2,rpixval

        rpixval=0.d0
        do i=1,cisl
         a4=bmaj(i)/fwsig/abs(cdelt(1)*3600.d0)
         a5=bmin(i)/fwsig/abs(cdelt(2)*3600.d0)
         a6=(bpa(i)+90.d0)/rad
         spa=dsin(a6) 
         cpa=dcos(a6) 
         dr1=(((x-xpix(i))*cpa+(y-ypix(i))*spa)/a4)
         dr2=(((y-ypix(i))*cpa-(x-xpix(i))*spa)/a5)
         rpixval=rpixval+peakfl(i)*dexp(-0.5d0*(dr1*dr1+dr2*dr2))
        end do

        return
        end
c!
c!      ==========
c!
        subroutine same_isl_dist1(i,j,cisl,gpi,peakfl,xpix,ypix,bmaj,
     /           bmin,bpa,cdelt,maxline,thresh_isl,rms_clip,same_island)
        implicit none
        integer i,j,gpi,cisl,maxline,round,dumi
        logical same_island,ysign,xsign
        real*8 peakfl(gpi),xpix(gpi),ypix(gpi),bmaj(gpi),cdelt(3)
        real*8 bmin(gpi),bpa(gpi),xline(maxline),yline(maxline)
        real*8 rms_clip,thresh_isl,dumr1,dumr2,dumr3,fwhm1,psi
        real*8 fwhm2,dist

        call gdist_pa(i,j,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm1) ! fwhm of i & j in asec at PA
        call gdist_pa(j,i,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm2) ! towards each other.
        dist=sqrt((ypix(j)-ypix(i))*(ypix(j)-ypix(i))+(xpix(j)-xpix(i))
     /       *(xpix(j)-xpix(i)))
        dist=dist*abs(cdelt(1)*3600.d0)

        if (dist.le.0.5d0*(fwhm1+fwhm2)*1.d0) same_island=.true.   ! 0.5 for fwhm->hwhm

        return
        end
c!
c!      ==========
c!
        subroutine gdist_pa(i,j,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm)
        implicit none
        include "constants.inc"
        integer i,j,gpi
        real*8 xpix(gpi),ypix(gpi),bmaj(gpi),bmin(gpi),bpa(gpi)
        real*8 dumr1,dumr2,dumr3,fwhm,psi

        dumr1=atan(abs((ypix(j)-ypix(i))/(xpix(j)-xpix(i))))  ! rad, 1st quadrant
        call atanproper(dumr1,ypix(j)-ypix(i),xpix(j)-xpix(i))
        
        psi=dumr1-(bpa(i)+90.d0)/rad
        dumr2=bmaj(i)*dcos(psi)
        dumr3=bmin(i)*dsin(psi)
        fwhm=sqrt(dumr2*dumr2+dumr3*dumr3)

        return
        end
c!
c!      ==========
c!
