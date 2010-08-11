c! reconvolves gaus_r with the beam gaus_o to give gaus_d. stolen shamelessly from aips RECONV.FOR
c! gaus_r = R; gaus_o = C; gaus_d = F

        subroutine reconv(gaus_o,gaus_r,gaus_d,e_1,e_2,e_3,
     /             ed_1,ed_2,ed_3,ierr)
        implicit none
        include "constants.inc"
        real*8 gaus_o(3),gaus_r(3),gaus_d(3)
        real*8 e_1,e_2,e_3,ed_1,ed_2,ed_3,fmj2,fmn2
        real*8 cmj2,cmn2,rmj2,rmn2,sigic2,den,det,fmaj,fmin,fpa,rhoc
        integer error,ierr

        cmj2=gaus_o(1)*gaus_o(1)
        cmn2=gaus_o(2)*gaus_o(2)
        rmj2=gaus_r(1)*gaus_r(1)
        rmn2=gaus_r(2)*gaus_r(2)
        sigic2=(gaus_r(3)-gaus_o(3))/28.647888d0
        den=cmj2-cmn2+dcos(sigic2/rad)*(rmj2-rmn2)
        det=dsin(sigic2/rad)*(rmj2-rmn2)
        if (det.eq.0.d0.and.den.eq.0.d0) then
         fpa=0.d0
        else
         fpa=atan(det/den)*rad
        end if


        det=rmj2+rmn2+cmj2+cmn2
        rhoc=dcos(fpa)
        if (rhoc.ne.0.d0) then
         fmj2=(det+den/rhoc)/2.d0
         fmn2=(det-den/rhoc)/2.d0
        else
         fmj2=det/2.d0
         fmn2=fmj2
        end if

        fpa=fpa*28.647888d0+gaus_o(3)
        fmaj=sqrt(abs(fmj2))
        fmin=sqrt(abs(fmn2))
        fpa=mod(fpa+720.d0,180.d0)
        if (fmaj.lt.fmin) then
         det=fmaj
         fmaj=fmin
         fmin=det
         fpa=mod(fpa+450.d0,180.d0)
        end if
        ierr=0
        if (fmj2.le.0.d0.or.fmn2.le.0.d0) ierr=1
        gaus_d(1)=fmaj
        gaus_d(2)=fmin
        gaus_d(3)=fpa

c! error
c        write (*,*) ' incomplete'
c        if (rhoc.eq.0.d0) then
         if (gaus_d(1).ne.0.d0) then
          ed_1=gaus_o(1)/gaus_d(1)*e_1
         else
          ed_1=sqrt(2.d0*e_1*gaus_o(1))
         end if
         if (gaus_d(2).ne.0.d0) then
          ed_2=gaus_o(2)/gaus_d(2)*e_2
         else
          ed_2=sqrt(2.d0*e_2*gaus_o(2))
         end if
         ed_3=e_3
c        else
c         
c        end if

        return
        end

