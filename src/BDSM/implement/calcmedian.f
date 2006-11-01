
        subroutine calcmedian(filt,f1,f2,arr)
        implicit none
        integer f1,f2,i,j,k
        real*8 filt(f1,f2),arr,array(f1*f2)
        real*4 arr4(f1*f2),selectnr

        k=0
        do 100 i=1,f1
         do 110 j=1,f2
          k=k+1
          array(k)=filt(i,j)
          arr4(k)=array(k)
110      continue
100     continue
        arr=selectnr((f1*f2+1)/2,f1*f2,arr4)
c        arr=1.0

        return
        end

