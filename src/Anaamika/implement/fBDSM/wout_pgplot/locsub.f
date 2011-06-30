
        subroutine readinarray(arr,x,n,fn)
        implicit none
        integer x,n,fn,i,j
        real*8 arr(x,x),vec(n)

        do 100 i=1,n
         read (fn,*) vec
         do 110 j=1,n
          arr(i,j)=vec(j)
110      continue
100     continue

        return  
        end

