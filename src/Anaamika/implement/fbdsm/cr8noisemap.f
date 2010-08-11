c! creates noise map

        subroutine cr8noisemap(filen,n,m,sigmaJy,seed,scratch,srldir)
        implicit none
        integer n,m,seed,nchar
        real*8 sigmaJy,image1(n,m),keyvalue
        character filen*500,strdev*5,lab*500,scratch*500,srldir*500

cf2py   intent(in) filen,n,m,sigmaJy,scratch,srldir
cf2py   intent(in,out) seed

c! *** GENERATE  NOISE  IMAGE  ***
c!
        call xcr8noisemap(n,m,n,m,sigmaJy,image1,seed)

        write (*,'(2x,a26,$)') ' Image of noise ... <RET> '
        read (*,*)
        lab='Noise image with zero correlation'
        strdev='/xs'
        call grey(image1,n,m,n,m,strdev,lab,0,1,'hn',1,scratch,
     /       filen,srldir)

        call writearray_bin2D(image1,n,m,n,m,filen,'mv')

333     continue
        return
        end

