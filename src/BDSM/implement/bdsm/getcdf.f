c! use area and freq (and -0.8) to get array of s,n from 'data'.
c! cdf is cumulative distr fn.

        subroutine getcdf(area,freq,sarr,cdfarr)
        implicit none
        real*8 area,freq,sarr(500),cdfarr(500),alpha,s(500),n(500)
        character filen*500,dir*500
        integer nbin,i

        alpha=-0.8d0
        filen='data'
        dir="./"
        call getline(filen,dir,nbin)
        open(unit=21,file=filen,status='old')
         do 100 i=1,nbin
          read (21,*) s(i),n(i)       !  mJy and N(>S)/str
          s(i)=s(i)*1.0d-3              !   Jy
          sarr(i)=s(i)*((freq/1400.d6)**(alpha))
          cdfarr(i)=n(i)*area
100      continue
        close(21)

        return
        end

