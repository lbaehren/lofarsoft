c! calls write_gaulsrl to write a catalogue to bdsm format.
c! copy read-file to this name.
c! this is to read the nvss file from condon (see emails)
c! NVSScat_condon ===> NVSScondon

        subroutine call_cat2gaulsrl(scratch,srldir,runcode)
        implicit none
        character scratch*500,srldir*500,runcode*2
        integer ngau

        ngau=816331

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
        real*8 cbmaj,cbmin,cbpa,freq,catrms,ss,sa,prob(ngau),rms(ngau)
        character nam(ngau)*500,sdec*3
        character dum5a*5,dum5b*5,dum5c*5,second*60

        f1='FIRSTcatalogue'
        cbmaj=5.d0/3600.d0
        cbmin=5.d0/3600.d0
        cbpa=0.d0
        freq=1.45d9
        catrms=0.15d-3

        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))
        open(unit=21,file=fn1)
        read (21,*)
        read (21,*)
        do i=1,ngau
         read (21,*) hh,mm,ss,sdec,ma,sa,prob(i),peak(i),tot(i),
     /      rms(i),dbmaj(i),dbmin(i),dbpa(i),bmaj(i),bmin(i),bpa(i)
         read(sdec,*) dd
         dd=abs(dd)

         ra(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0  ! deg
         dec(i)=abs(dd+ma/60.d0+sa/3600.d0)
         if (sdec(1:1).eq.'-') dec(i)=-dec(i)
 
         peak(i)=peak(i)/1.d3
         tot(i)=tot(i)/1.d3
         rms(i)=rms(i)/1.d3
         dumr6(i)=tot(i) ! total flux

         epeak(i)=rms(i)
         etot(i)=rms(i)  ! not true
         era(i)=sqrt(bmaj(i)*bmin(i))*(1.d0/20.0+rms(i)/peak(i))
         edec(i)=era(i)
 
         ebmaj(i)=10.d0*(rms(i)/peak(i)+1.d0/75.d0)
         ebmin(i)=ebmaj(i)
         edbmaj(i)=ebmaj(i)
         edbmin(i)=ebmaj(i)
         ebpa(i)=1.d0 ! arbit
         edbpa(i)=1.d0
 
         rstd(i)=rms(i)
         sstd(i)=rms(i)
         rav(i)=0.d0
         sav(i)=0.d0
         spin(i)=-99.99d0
         espin(i)=-99.99d0
         imrms(i)=rms(i)
         dumr2(i)=prob(i)
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




