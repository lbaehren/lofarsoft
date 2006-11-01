c! to make mask - id each pixel with one source inside an island. to make
c! srl from gaul. so calc values in all pixels for each gau, group by source, decide
c! which source each pixel belongs to.

        subroutine make_mask_gausid(cisl,subn,subm,mask,gpi,peakfl,
     /        xpix,ypix,bmaj,bmin,bpa,xpixs,ypixs,maxmem,npix_isl,
     /        cdelt,delx,dely,islct,islnum,subim)
        implicit none
        include "constants.inc"
        integer maxmem,npix_isl,xpixs(maxmem),ypixs(maxmem)
        integer subn,subm,blc(2),trc(2),mask(subn,subm),cisl
        integer origmask(subn,subm)
        integer i,j,k,gpi,delx,dely,k1
        integer islct,islnum(gpi)
        real*8 xpix(gpi),ypix(gpi),bmaj(gpi),bmin(gpi),bpa(gpi)
        real*8 dr1,dr2,ang,cdelt(2),rmask(subn,subm),dist
        real*8 g_image(subn,subm,cisl),a4,a5,sang,cang
        real*8 subim(subn,subm)
        real*8 s_image(subn,subm,islct),peakfl(gpi)
        character filen*500

c! set original mask first
        call initialisemask(origmask,subn,subm,subn,subm,0)
        do i=1,npix_isl
         origmask(xpixs(i)-delx,ypixs(i)-dely)=1 
        end do

c! first construct images of each gaussian in full.
        do k=1,cisl
         a4=bmaj(k)/fwsig/abs(cdelt(1)*3600.d0)
         a5=bmin(k)/fwsig/abs(cdelt(2)*3600.d0)
         ang=(bpa(k)+90.d0)/rad
         sang=dsin(ang)
         cang=dcos(ang)
         do i=1,subn
          do j=1,subm
           dr1=peakfl(k)*dexp(-0.5d0*((((i-(xpix(k)-delx))*cang+
     /          (j-(ypix(k)-dely))*sang)/a4)**2.d0+(((j-(ypix(k)-dely))
     /          *cang-(i-(xpix(k)-delx))*sang)/a5)**2.d0))
           g_image(i,j,k)=dr1
          end do
         end do
        end do

c! now construct same thing per source
        do i=1,subn
         do j=1,subm
          do k=1,islct
           s_image(i,j,k)=0.d0
           do k1=1,cisl
            if (islnum(k1).eq.k) 
     /          s_image(i,j,k)=s_image(i,j,k)+g_image(i,j,k1)
           end do
          end do
         end do
        end do 

c! now mark each pixel as belonging to a source (just compare values. shud compare to sigma later.
        do i=1,subn
         do j=1,subm
          mask(i,j)=0
          if (origmask(i,j).eq.1) then
           dr1=s_image(i,j,1)
           mask(i,j)=1
           do k=2,islct
            if (dr1.lt.s_image(i,j,k)) then
             dr1=s_image(i,j,k)
             mask(i,j)=k
            end if
           end do
          end if
         end do
        end do

c! now add s_image for use in moments
        do i=1,subn
         do j=1,subm
          subim(i,j)=0.d0
          do k=1,islct
           subim(i,j)=subim(i,j)+s_image(i,j,k)
          end do
         end do
        end do

c        filen='masking'
c        call imageint2r(mask,subn,subm,subn,subm,rmask)
c        call writearray_bin(rmask,subn,subm,subn,subm,filen,'av')
       
        return
        end



