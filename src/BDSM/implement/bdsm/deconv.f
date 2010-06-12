c! deconvolves gaus_r from gaus_o to give gaus_d. stolen shamelessly from aips DECONV.FOR

        subroutine deconv(gaus_o,gaus_r,gaus_d,error)
        implicit none
        include "constants.inc"
        real*8 gaus_o(3),gaus_r(3),gaus_d(3)
        real*8 phi_o,phi_r,maj2_r,min2_r,maj2_o,min2_o,theta,cost,sint
        real*8 rhoc,sigic,rhoa,dumr
        integer error

        phi_o=mod(gaus_o(3)+900.d0,180.d0)
        phi_r=mod(gaus_r(3)+900.d0,180.d0)
        maj2_r=gaus_r(1)*gaus_r(1)
        min2_r=gaus_r(2)*gaus_r(2)
        maj2_o=gaus_o(1)*gaus_o(1)
        min2_o=gaus_o(2)*gaus_o(2)
        theta=2.d0*(phi_o-phi_r)/rad
        cost=dcos(theta)
        sint=dsin(theta)

        rhoc=(maj2_o-min2_o)*cost-(maj2_r-min2_r)
        if (rhoc.eq.0.0d0) then
         sigic=0.0d0
         rhoa=0.0d0
        else
         sigic=atan((maj2_o-min2_o)*sint/rhoc)   ! in radians
         rhoa=((maj2_r-min2_r)-(maj2_o-min2_o)*cost)/(2.d0*dcos(sigic))
        end if

        gaus_d(3)=sigic*rad/2.d0+phi_r
        dumr=((maj2_o+min2_o)-(maj2_r+min2_r))/2.d0
        gaus_d(1)=dumr-rhoa
        gaus_d(2)=dumr+rhoa
        error=0
        if (gaus_d(1).lt.0.d0) error=error+1
        if (gaus_d(2).lt.0.d0) error=error+1

        gaus_d(1)=max(0.d0,gaus_d(1))
        gaus_d(2)=max(0.d0,gaus_d(2))
        gaus_d(1)=sqrt(abs(gaus_d(1)))
        gaus_d(2)=sqrt(abs(gaus_d(2)))
        if (gaus_d(1).lt.gaus_d(2)) then
         sint=gaus_d(1)
         gaus_d(1)=gaus_d(2)
         gaus_d(2)=sint
         gaus_d(3)=gaus_d(3)+90.d0
        end if

        gaus_d(3)=mod(gaus_d(3)+900.d0,180.d0)
        if (gaus_d(1).eq.0.0) then
         gaus_d(3) = 0.0
        else
         if (gaus_d(2).eq.0.0) then
          if ((abs(gaus_d(3)-phi_o).gt.45.d0).and.
     /          (abs(gaus_d(3)-phi_o).lt.135.d0)) then
           gaus_d(3)=mod(gaus_d(3)+450.d0,180.d0)
          end if
         end if
        end if

        return
        end

