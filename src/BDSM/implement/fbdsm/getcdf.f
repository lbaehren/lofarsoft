c! use area and freq (and -0.8) to get array of s,n from 'data'.
c! cdf is cumulative distr fn.

        subroutine getcdf(area,freq,sarr,cdfarr)
        implicit none
<<<<<<< HEAD
        include "sourcecounts.inc"
=======
        include "includes/sourcecounts.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        real*8 area,freq,sarr(nbin),cdfarr(nbin),alpha
        character filen*500,dir*500
        integer i

        alpha=-0.8d0
         do 100 i=1,nbin                  !  mJy and N(>S)/str
          ss(i)=ss(i)*1.0d-3              !   Jy
          sarr(i)=ss(i)*((freq/1400.d6)**(alpha))
          cdfarr(i)=nn(i)*area
100      continue

        return
        end

