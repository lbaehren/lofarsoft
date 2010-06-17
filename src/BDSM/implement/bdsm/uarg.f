
        function uarg(why,ex)  ! arg in rad
        implicit none
        include "constants.inc"
        real*8 ex,why,uarg,dumr

        uarg=0.d0  ! for compiler
        dumr=atan(abs(why/ex)) 
        if (ex.gt.0.d0.and.why.gt.0.d0) uarg=dumr
        if (ex.lt.0.d0.and.why.gt.0.d0) uarg=-dumr+180.d0/rad
        if (ex.lt.0.d0.and.why.lt.0.d0) uarg=dumr+180.d0/rad
        if (ex.gt.0.d0.and.why.lt.0.d0) uarg=-dumr+360.d0/rad

        return
        end

