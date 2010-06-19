c! corrects sex.cat from pixel to NCP coordinates for WENSS since it doesnt know
c! NCP and converting sex to pixel doesnt work.

        implicit none
        include "constants.inc"
        integer num,int1,i
        character filen*500,dir*500
        real*8 ra1,dec1,r1,r2,r3,r4,r5,r6,r7,r8,r9,ra,dec
        real*8 crval(2),crval1,crval2,l,m,cosd0,sind0,da
        real*8 s,t,sina,cosa,sind,cosd,x,y,cdelt(2),crpix(2)

        data crval/16.7526d0,40.6491d0/
        data cdelt/-5.85937d-3,5.85937d-3/
        data crpix/2.22d3,8.7d2/

        crval1=crval(1)/rad
        crval2=crval(2)/rad
        
        filen='sex.cat'
        dir='./'
        call getline(filen,dir,num) 
        
        open(unit=21,file='sex.cat')
        open(unit=22,file='a')
        do i=1,num
         read (21,*) int1,x,y,r1,r2,r3,r4,r5,r6,r7,r8,r9
c! now do ncp
         x=cdelt(1)*(x-crpix(1))
         y=cdelt(2)*(y-crpix(2))
         l=x/rad
         m=y/rad

         cosd0=dcos(crval2)
         sind0=dsin(crval2)
         da=atan(l/(cosd0-m*sind0))
         dec=acos((cosd0-m*sind0)/dcos(da))
         if (crval2.gt.0.0) then
          dec=dabs(dec)
         else
          dec=dabs(dec)*(-1.d0)
         end if
         ra=crval1+da
         ra=ra*rad
         dec=dec*rad
         write (22,777) int1,ra,dec,r1,r2,r3,r4,r5,r6,r7,r8,r9
        end do
        close(21)
        close(22)
        call system('cp a sex.cat')
777     format(i5,11(1x,f10.6))
        
        end


