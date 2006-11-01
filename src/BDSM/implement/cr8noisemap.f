c! creates noise map

        subroutine cr8noisemap(filen,n,m,sigmaJy,seed,scratch)
        implicit none
        integer n,m,seed
        real*8 sigmaJy,image1(n,m),keyvalue
        character filen*(*),strdev*5,lab*500,scratch*500

c! *** GENERATE  NOISE  IMAGE  ***
c!
        call xcr8noisemap(n,m,n,m,sigmaJy,image1,seed)

        write (*,'(2x,a26,$)') ' Image of noise ... <RET> '
        read (*,*)
        lab='Noise image with zero correlation'
        strdev='/xs'
        call grey(image1,n,m,n,m,strdev,lab,0,1,'hn',1,scratch)

        call writearray_bin(image1,n,m,n,m,filen,'mv')

333     continue
        return
        end

        subroutine xcr8noisemap(x,y,n,m,sigmaJy,image1,seed)
        implicit none
        integer n,m,seed,i,j,x,y
        real*8 image1(x,y),a,sigmaJy,gasdev

        do 100 i=1,n
         do 110 j=1,m
          a=gasdev(seed)
          image1(i,j)=a*sigmaJy   
110      continue
100     continue
        
        return
        end

