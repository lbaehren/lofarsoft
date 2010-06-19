c! calls write_gaulsrl to write a catalogue to bdsm format.
c! copy read-file to this name.
c! this is to read the wenss file since it has rms.

        subroutine call_cat2gaulsrl(scratch,srldir,runcode)
        implicit none
        character scratch*500,srldir*500,runcode*2
        integer ngau

        ngau=4935   ! in wenss_sc   which is wenss_all without M srcs
        ngau=9343
        ngau=3434
        ngau=5337
        ngau=217873   ! in wenss_sc   which is wenss_all without M srcs
        ngau=382      ! 3c 61.1 field for rogier

        call sub_cal_cat2gaul(scratch,srldir,runcode,ngau)

        return
        end

        subroutine sub_cal_cat2gaul(scratch,srldir,runcode,ngau)
        implicit none
        character scratch*500,srldir*500,runcode*2,f1*500,fn1*500
        integer ngau,i,hh,mm,dd,ma,nchar,dumi,ierr
        real*8 peak(ngau),epeak(ngau),tot(ngau),etot(ngau),ra(ngau)
        real*8 era(ngau),dec(ngau),edec(ngau),bmaj(ngau),ebmaj(ngau)
        real*8 bmin(ngau),ebmin(ngau),bpa(ngau),ebpa(ngau),dbmaj(ngau)
        real*8 edbmaj(ngau),dbmin(ngau),edbmin(ngau),dbpa(ngau)
        real*8 edbpa(ngau),rstd(ngau),rav(ngau),sstd(ngau),sav(ngau)
        real*8 spin(ngau),espin(ngau),imrms(ngau),dumr2(ngau)
        real*8 dumr3(ngau),dumr4(ngau),dumr5(ngau),dumr6(ngau),dumr
        real*8 cbmaj,cbmin,cbpa,freq,catrms,ss,sa
        real*8 gaus_o(3),gaus_r(3),gaus_d(3)
        real*8 e_1,e_2,e_3,ed_1,ed_2,ed_3
        character nam(ngau)*500,sdec*1,sbmaj,sbmin,code*1
        character dum5a*5,dum5b*5,dum5c*5,second*60

        f1='wp_common'
        f1='wenss_sc_1'
        f1='wenss_3c196'  ! for correlating for py_fbdsm.py
        f1='wenss_sc_3c196'  ! for gsm for busyweek
        f1='wenss_sc'
        f1='wenss_sc_3c61rvl'
        cbmaj=55.d0/3600.d0
        cbmin=55.d0/3600.d0
        cbpa=0.d0
        freq=0.327d9
        catrms=3.0d-3
        gaus_o(1)=cbmaj*3600.d0
        gaus_o(2)=cbmin*3600.d0
        gaus_o(3)=cbpa

        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))
        open(unit=21,file=fn1)
        do i=1,ngau
         read (21,*) nam(i),dumi,dumi,dumr,dumi,dumi,dumr,
     /         hh,mm,ss,dd,ma,sa,code,peak(i),tot(i),
     /         dbmaj(i),dbmin(i),dbpa(i),imrms(i)   ! deconvolved sizes

         if (tot(i).le.0.d0) tot(i)=peak(i) ! two entries in wenss cat have tot=0 and 8 have negative

         if (code.eq.'M') then
          write (*,*) ' ERROR in code ... no M. '
         else

          peak(i)=peak(i)/1.d3
          tot(i)=tot(i)/1.d3
          imrms(i)=imrms(i)/1.d3

          ra(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0  ! deg
          dec(i)=abs(dd+ma/60.d0+sa/3600.d0)
          if (sdec.eq.'-') dec(i)=-dec(i)
          era(i)=cbmaj/(2.d0*peak(i)/imrms(i))
          edec(i)=era(i)
 
          epeak(i)=imrms(i)
          etot(i)=imrms(i)
 
          gaus_r(1)=dbmaj(i)
          gaus_r(2)=dbmin(i)
          gaus_r(3)=dbpa(i)
          call reconv(gaus_o,gaus_r,gaus_d,e_1,e_2,e_3,
     /             ed_1,ed_2,ed_3,ierr)
          bmaj(i)=gaus_d(1)
          bmin(i)=gaus_d(2)
          bpa(i)=gaus_d(3)
          ebmaj(i)=era(i)
          ebmin(i)=era(i)
          ebpa(i)=10.d0
          edbmaj(i)=era(i)
          edbmin(i)=era(i)
          edbpa(i)=10.d0
 
          rstd(i)=imrms(i)
          sstd(i)=imrms(i)
          rav(i)=0.d0
          sav(i)=0.d0
          spin(i)=-99.99d0
          espin(i)=-99.99d0
          dumr2(i)=0.d0
          dumr3(i)=0.d0
          dumr4(i)=0.d0
          dumr5(i)=0.d0
  
         end if
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




