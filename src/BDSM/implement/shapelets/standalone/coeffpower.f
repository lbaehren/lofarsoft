c! to calculate total power in the coeffecients

        subroutine coeffpower(cf,nmax,sumsq)
        implicit none
        real*8 cf(50,50),sumsq
        integer i,j,nmax 

        sumsq=0.d0
        do 100 i=1,nmax
         do 110 j=1,nmax-i
          sumsq=sumsq+cf(i,j)*cf(i,j)
110      continue
100     continue

        return
        end
