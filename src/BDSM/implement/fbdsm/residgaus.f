c! subtracts ng gaussians from image and makes residual image

        subroutine residgaus(subim,subn,subm,a,nmul6,ng6,resid)
        implicit none
        include "constants.inc"
        integer subn,subm,nmul6,ng6,i,j,i1,d
        real*8 subim(subn,subm),resid(subn,subm),a(nmul6),bpa
        real*8 dumr1,dumr2,amp

        do i=1,subn
         do j=1,subm
          resid(i,j)=0.d0
          do i1=1,ng6/6
           d=6*(i1-1)
c!           bpa=(180.d0-a(6+d))/rad    ! WRONG, U IMBECILE !!!
           bpa=(a(6+d))/rad
           dumr1=(((i-a(2+d))*dcos(bpa)+(j-a(3+d))*dsin(bpa))/a(4+d))
           dumr2=(((j-a(3+d))*dcos(bpa)-(i-a(2+d))*dsin(bpa))/a(5+d))
           amp=a(1+d)*dexp(-0.5d0*(dumr1*dumr1+dumr2*dumr2))
           resid(i,j)=resid(i,j)+amp
          end do
          resid(i,j)=subim(i,j)-resid(i,j)
         end do
        end do

        return
        end


