

        subroutine xcr8noisemap(x,y,n,m,sigmaJy,image1,seed)
        implicit none
        integer n,m,seed,i,j,x,y
        real*8 image1(x,y),a,sigmaJy,gasdev

        do 100 j=1,m
         do 110 i=1,n
          a=gasdev(seed)
          image1(i,j)=a*sigmaJy   
110      continue
100     continue
        
        return
        end

