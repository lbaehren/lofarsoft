c! convert coord to pix and back again

        implicit none
        include "constants.inc"
        integer i,j,error
        real*8 ra,dec,ra1,dec1,x,y,x1,y1,hh,mm,ss,dd,ma,sa
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        character hdrfile*500,ctype(3)*8

        ctype(1)='RA---SIN'
        ctype(2)='DEC--SIN'
        ctype(3)='FREQ   '
        crpix(1)=1441.d0
        crpix(2)=1441.d0
        crpix(3)=1.d0
        cdelt(1)=-0.033333d0
        cdelt(2)=0.033333d0
        cdelt(3)=1.d0
        crval(1)=350.85d0
        crval(2)=58.815d0
        crval(3)=1.d0
        crota(1)=0.d0
        crota(2)=0.d0
        crota(3)=0.d0
 
        do i=300,2800,100
         do j=300,1900,100
          x=i*1.d0
          y=j*1.d0
          call xy2radec(x,y,ra1,dec1,error,ctype,crpix,cdelt,
     /         crval,crota)
         call radec2xy(ra1,dec1,x1,y1,error,ctype,crpix,cdelt,
     /         crval,crota)
c          write (*,*) x,y,ra1*rad,dec1*rad,x1,y1
         end do
        end do
        
c         write (*,*) 'enter ra dec'
c         read (*,*) hh,mm,ss,dd,ma,sa
c         ra1=(hh+mm/60.d0+ss/3600.d0)*15.d0/rad
c         dec1=(dd+ma/60.d0+sa/3600.d0)/rad
c         call radec2xy(ra1,dec1,x1,y1,error,ctype,crpix,cdelt,
c     /         crval,crota)
c          call xy2radec(x1,y1,ra,dec,error,ctype,crpix,cdelt,
c     /         crval,crota)
c         if (ra*rad.gt.360.d0) ra=ra-360.d0/rad
c          write (*,*) ra1*rad,dec1*rad,x1,y1,ra*rad,dec*rad

         write (*,*) 'enter x y'
         read (*,*) x,y
          call xy2radec(x,y,ra,dec,error,ctype,crpix,cdelt,
     /         crval,crota)
         call correctrarad(ra)
         call radec2xy(ra,dec,x1,y1,error,ctype,crpix,cdelt,
     /         crval,crota)
          write (*,*) x,y,ra*rad,dec*rad,x1,y1


        end


