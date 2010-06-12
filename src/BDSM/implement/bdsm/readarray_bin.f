
        subroutine readarray_bin(n,m,arr,x,y,fn)
        implicit none
        integer n,m,i,j,fn,x,y
        real*8 dumr(m),arr(x,y)

        do 100 i=1,n
         read (fn) dumr
         do 110 j=1,m
          arr(i,j)=dumr(j)
110      continue
100     continue

        return
        end

