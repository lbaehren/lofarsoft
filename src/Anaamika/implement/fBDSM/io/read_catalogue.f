c! calls write_gaulsrl to write a catalogue to bdsm format.
c! copy read-file to this name.
c! this is to read the nvss file from condon (see emails)
c! NVSScat_condon ===> NVSScondon

        subroutine call_cat2gaulsrl(scratch,srldir,runcode)
        implicit none
        character scratch*500,srldir*500,runcode*2
        integer ngau

        ngau=1773488
        ngau=74244
        ngau=561434
        ngau=13857
        ngau=6197

        call sub_cal_cat2gaul(scratch,srldir,runcode,ngau)

        return
        end

        subroutine sub_cal_cat2gaul(scratch,srldir,runcode,ngau)
        implicit none
        character scratch*500,srldir*500,runcode*2,f1*500,fn1*500
        integer ngau,i,hh,mm,dd,ma,nchar
        real*8 peak(ngau),epeak(ngau),tot(ngau),etot(ngau),ra(ngau)
        real*8 era(ngau),dec(ngau),edec(ngau),bmaj(ngau),ebmaj(ngau)
        real*8 bmin(ngau),ebmin(ngau),bpa(ngau),ebpa(ngau),dbmaj(ngau)
        real*8 edbmaj(ngau),dbmin(ngau),edbmin(ngau),dbpa(ngau)
        real*8 edbpa(ngau),rstd(ngau),rav(ngau),sstd(ngau),sav(ngau)
        real*8 spin(ngau),espin(ngau),imrms(ngau),dumr2(ngau)
        real*8 dumr3(ngau),dumr4(ngau),dumr5(ngau),dumr6(ngau)
        real*8 cbmaj,cbmin,cbpa,freq,catrms,ss,sa
        character nam(ngau)*500,sdec*1,sbmaj,sbmin
        character dum5a*5,dum5b*5,dum5c*5,second*60

        f1='NVSScondon'
        f1='NVSScondon_1'
        f1='NVSScondon_N'   ! for srcs > 28.5 dec soit matches for wenss and vlss so i can get source counts consistently
        f1='NVSScondon_3c196' ! for srcs > 28.5 dec soit matches for wenss and vlss so i can get source counts consistently
        f1='NVSScondon_rvl'   ! for 3c61 field for rogier
        cbmaj=45.d0/3600.d0
        cbmin=45.d0/3600.d0
        cbpa=0.d0
        freq=1.4d9
        catrms=0.45d-3

        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))
        open(unit=21,file=fn1)
        do i=1,ngau
         read (21,777) hh,mm,ss,sdec,dd,ma,sa,peak(i),dum5a,dum5b,dum5c

         sbmaj=dum5a(1:1)
         sbmin=dum5b(1:1)
         if (sbmaj.eq.'<') dum5a(1:1)=' '
         read (dum5a,*) bmaj(i)
         if (sbmin.eq.'<') dum5b(1:1)=' '
         read (dum5b,*) bmin(i)
         if (sbmaj.eq.' ') read (dum5c,*) bpa(i)

         read (21,'(a60)') second
         read (second,778) era(i),edec(i),epeak(i)
         ebmaj(i)=0.d0
         ebmin(i)=0.d0
         ebpa(i)=0.d0
         if (sbmaj.eq.' ') then
          read (second,'(37x,f5.1)') ebmaj(i)
          if (sbmin.eq.' ') then
           read (second,'(43x,f5.1)') ebmin(i)
          end if
          read (second,'(49x,f5.1)') ebpa(i)
         end if

        ra(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0  ! deg
        dec(i)=abs(dd+ma/60.d0+sa/3600.d0)
        if (sdec.eq.'-') dec(i)=-dec(i)
        peak(i)=peak(i)/1.d3
        tot(i)=peak(i)
        dumr6(i)=peak(i) ! total flux
        epeak(i)=epeak(i)/1.d3
        etot(i)=epeak(i)
        era(i)=era(i)/3600.d0*15.d0
        edec(i)=edec(i)/3600.d0

        bmaj(i)=bmaj(i)
        bmin(i)=bmin(i)
        ebmaj(i)=ebmaj(i)
        ebmin(i)=ebmin(i)
        if (sbmaj.eq.' ') then
         dbmaj(i)=bmaj(i)
         edbmaj(i)=ebmaj(i)
         dbpa(i)=bpa(i)
         edbpa(i)=ebpa(i)
        else
         bmaj(i)=45.d0
         ebmaj(i)=3.d0
         dbmaj(i)=0.d0
         edbmaj(i)=-99.99d0
         dbpa(i)=0.d0
         edbpa(i)=-99.99d0
        end if
        if (sbmin.eq.' ') then
         dbmin(i)=bmin(i)
         edbmin(i)=ebmin(i)
        else
         bmin(i)=45.d0
         ebmin(i)=3.d0
         dbmin(i)=0.d0
         edbmin(i)=-99.99d0
        end if

        rstd(i)=epeak(i)
        sstd(i)=epeak(i)
        rav(i)=0.d0
        sav(i)=0.d0
        spin(i)=-99.99d0
        espin(i)=-99.99d0
        imrms(i)=epeak(i)
        dumr2(i)=0.d0
        dumr3(i)=0.d0
        dumr4(i)=0.d0
        dumr5(i)=0.d0
        nam(i)='  '

        if (mod(i,100000).eq.0) write (*,*) i
        end do
        close(21)
777     format(i2,1x,i2,1x,f5.2,1x,a1,i2,1x,i2,1x,f4.1,6x,f7.1,
     /         1x,a5,1x,a5,1x,a5)
778     format(6x,f5.2,8x,f4.1,6x,f7.1)
      
        call write_gaulsrl(f1,runcode,scratch,srldir,ngau,
     /   peak,epeak,tot,etot,ra,era,dec,edec,bmaj,ebmaj,bmin,ebmin,bpa,
     /   ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rstd,rav,sstd,sav,
     /   spin,espin,imrms,dumr2,dumr3,dumr4,dumr5,dumr6,cbmaj,cbmin,
     /   cbpa,freq,catrms,nam)


        return
        end




