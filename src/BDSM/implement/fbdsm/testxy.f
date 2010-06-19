
        implicit none
        include "includes/constants.inc"
        integer i,j,error,hh,mm,dd,ma
        real*8 ra,dec,ss,sa,dra,ddec
        character fn*80,s

        fn='image/subim.header'

c	write (*,*) ' x y : '
c	read (*,*) i,j
c        call xy2radec(i*1.d0,j*1.d0,ra,dec,fn,error)
c        dra=ra*rad
c        ddec=dec*rad
c        call convertra(dra,hh,mm,ss)
c        call convertdec(ddec,s,dd,ma,sa)
c        write (*,*) i,j,hh,mm,ss,s,dd,ma,sa

        do i=1,15
         do j=1,13
          call xy2radec(i*100.d0,j*100.d0,ra,dec,fn,error)
          dra=ra*rad
          ddec=dec*rad
          call convertra(dra,hh,mm,ss)
          call convertdec(ddec,s,dd,ma,sa)
          write (*,*) dra/15.d0,ddec,i*100.d0,j*100.d0
 
         end do
        end do

	end

