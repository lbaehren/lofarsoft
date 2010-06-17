c! create image constant value

        subroutine cr8constantim(filen,n,m,val)
        implicit none
        integer n,m,i,j
        real*8 image1(n,m),val,keyvalue
        character filen*(*),f1*500,extn*20

cf2py   intent(in) filen,n,m,val

        do 100 j=1,m
         do 110 i=1,n
          image1(i,j)=val
110      continue
100     continue

        call writearray_bin2D(image1,n,m,n,m,filen,'mv')

        return
        end
        
