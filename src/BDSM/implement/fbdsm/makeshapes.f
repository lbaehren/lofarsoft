
        subroutine makeshapes(f1,n,m,scratch)
        implicit none
        include "constants.inc"
        character f1*500,extn*20,scratch*500
        real*8 dumr,peak,sigx,sigy,flux,radi,box,pa
        integer nchar,n,m,l,i,j,choi
        real*8 image(n,m),xc,yc

cf2py   intent(in) f1,scratch,n,m

444     write (*,*) '    1. gaus    2. circle    3. box '
        write (*,'(a,$)') '    What shape do you want ? '
        read (*,*) choi
        if (choi.lt.1.or.choi.gt.3) goto 444

        write (*,'(a,$)') '    Centre of shape : '
        read (*,*) xc,yc

        if (choi.eq.1) then
         write (*,'(a,$)') '    Peak, SigmaX, SigmaY  P.A. : ' 
         read (*,*) peak,sigx,sigy,pa
         pa=pa/rad
         do i=1,n
          do j=1,m
           dumr=((((i-xc)*dcos(pa)+(j-yc)*dsin(pa))/sigx)**2.d0+
     /       (((j-yc)*dcos(pa)-(i-xc)*dsin(pa))/sigy)**2.d0)
           image(i,j)=peak*dexp(-0.5d0*dumr)
          end do
         end do
        end if

        if (choi.eq.2) then
         write (*,'(a,$)') '   Flux and Radius : '
         read (*,*) flux,radi
         call initialiseimage(image,n,m,n,m,0.d0)
         do i=1,n
          do j=1,m
           dumr=sqrt((i-xc)*(i-xc)+(j-yc)*(j-yc))
           if (dumr.le.radi) image(i,j)=flux
          end do
         end do
        end if

        if (choi.eq.3) then
         write (*,'(a,$)') '   Flux and size : '
         read (*,*) flux,box
         call initialiseimage(image,n,m,n,m,0.d0)
         do i=int(xc-box/2.d0),int(xc+box/2.d0)
          do j=int(xc-box/2.d0),int(xc+box/2.d0)
           image(i,j)=flux
          end do
         end do
        end if

        call writearray_bin2D(image,n,m,n,m,f1,'mv')

        return
        end

