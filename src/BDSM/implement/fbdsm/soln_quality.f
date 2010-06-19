c! bad solution list. coded. quality for each source inside an island.
c! coded in powers or 2 (error1 *2^0 + error2 * 2^1 .. ) to facilitate
c! unique decoding. 
c! 1 - a(1) lower than threshold
c! 2 - a(1) higher than twice max in island
c! 3 - a(2) outside island-npix (n=1)
c! 4 - a(3) outside island-npix (n=1)
c! 5 - a(4) very large (?)
c! 6 - a(4) very small (?)
c! 7 - a(5) very large (?)
c! 8 - a(5) very small (?)
c! 9 - centre is outside universe or bad coordinate
c! whatever. cant be bothered to convert back. using log is stupid, no easy bit operations
c! in f77. 

        subroutine soln_quality(a,nmulsrc,nmul6,quality,subn,subm,
     /      max_msrc,thresh_pix,std,maxv,bm_pix,avclip,flag_s,
     /      wcs,wcslen,delx,dely,maxsize_beam,flagsmallsrc)
        implicit none
        include "constants.inc"
        integer nmulsrc,nmul6,quality(nmulsrc),i,d,npix,subn,subm
        integer max_msrc,wcslen,wcs(wcslen),delx,dely
        real*8 a(nmul6),thresh_pix,std,maxv,bm_pix(3),avclip
        real*8 maxsize_beam
        character flag_s(max_msrc)*5,flagsmallsrc*500
        logical isgoodp

        npix=1
        do i=1,nmulsrc
         d=6*(i-1)
         quality(i)=0
         flag_s(i)='00000'
         if (a(1+d)-avclip.lt.0.8*thresh_pix*std) then
          quality(i)=quality(i)+1
          flag_s(i)(1:1)='2'
         end if
         if (a(1+d).gt.2.d0*maxv) then
          quality(i)=quality(i)+2
          flag_s(i)(1:1)='1'
         end if
         if (a(2+d).gt.subn-npix.or.a(2+d).lt.1+npix) then
          write (*,*) a(2+d)+delx,a(3+d)+dely,a(2+d),a(3+d),npix,subn
          quality(i)=quality(i)+4
          if (a(2+d).gt.subn-npix) flag_s(i)(2:2)='1'
          if (a(2+d).lt.1+npix) flag_s(i)(2:2)='2'
         end if
         if (a(3+d).gt.subm-npix.or.a(3+d).lt.1+npix) then
          quality(i)=quality(i)+8
          if (a(3+d).gt.subm-npix) flag_s(i)(3:3)='1'
          if (a(3+d).lt.1+npix) flag_s(i)(3:3)='2'
         end if
         if (a(4+d).gt.subn.or.
     /     a(4+d).gt.sqrt(maxsize_beam*bm_pix(1)*bm_pix(2)/fwsig)) then
          quality(i)=quality(i)+16
          flag_s(i)(4:4)='1'
         end if
         if (flagsmallsrc(1:4).eq.'true') then
          if (a(4+d).lt.0.5*sqrt(bm_pix(1)*bm_pix(2))/fwsig) then
           quality(i)=quality(i)+32
           flag_s(i)(4:4)='2'
          end if
         end if
         if (a(5+d).gt.subm.or.
     /     a(5+d).gt.sqrt(maxsize_beam*bm_pix(1)*bm_pix(2)/fwsig)) then
          quality(i)=quality(i)+64
          flag_s(i)(5:5)='1'
         end if
         if (flagsmallsrc(1:4).eq.'true') then
          if (a(5+d).lt.0.5*sqrt(bm_pix(1)*bm_pix(2))/fwsig) then
           quality(i)=quality(i)+128
           flag_s(i)(5:5)='2'
          end if
         end if
         call wcs_isgoodpix(a(2+d)+delx,a(3+d)+dely,wcs,wcslen,isgoodp)
         if (.not.isgoodp) then
          quality(i)=quality(i)+256
          flag_s(i)(2:2)='3'
          flag_s(i)(3:3)='3'
         end if
        end do
                
        return
        end


