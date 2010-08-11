
        subroutine testfft(f1)
        implicit none
        integer n,m
        character f1*500
        
333     write (*,*) '  Size of image (n,m) : '
        read (*,*) n,m
        if (n.lt.2.or.m.lt.2) goto 333
        
        call call_testfft(f1,n,m)
        
        return
        end

        subroutine call_testfft(f1,n,m)
        implicit none
        include "includes/constants.inc"
        integer n,m,i,j,err,nwav,ix,nchar,l,indx,indy
        real*8 image1(n,m),image(n,m),cimage(n,m)
        real*8 realim(n,m),imagim(n,m),absim(n,m)
        real*8 lx(100),ax(100),dc,px(100)
        character fn*500,f1*500,extn*20
        double complex speq(m)

333     write (*,*) '  Number of sine waves : '
        read (*,*) nwav
        if (nwav.lt.1) goto 333
        write (*,*) '  Wavelength of ',nwav,' waves : '
        call getinputarr8(nwav,100,lx)
        write (*,*) '  Amplitudes of ',nwav,' waves : '
        call getinputarr8(nwav,100,ax)
        write (*,*) '  Angle of ',nwav,' waves (x=0): '
        call getinputarr8(nwav,100,px)
        write (*,*) '  Amplitude of d.c. level : '
        read (*,*) dc
        
        call initialiseimage(image1,n,m,n,m,dc)
        call initialiseimage(realim,n,m,n,m,0.d0)
        call initialiseimage(imagim,n,m,n,m,0.d0)
        call initialiseimage(absim,n,m,n,m,0.d0)
        do ix=1,nwav
         do j=1,m
          do i=1,n
           image1(i,j)=image1(i,j)+ax(ix)*dsin(2.d0*pi*(
     /      dcos(px(ix)/rad)*(i/lx(ix))+dsin(px(ix)/rad)*(j/lx(ix))))
          end do
         end do
        end do

        extn='.img' 
        call writearray_bin2D(image1,n,m,n,m,f1,'mv')

        call copyarray(image1,n,m,n,m,1,1,n,m,image)
        call copyarray(image,n,m,n,m,1,1,n,m,cimage)


        call rlft3(image,speq,n,m,1,-1)

        fn=f1(1:nchar(f1))//'.fft'
        call writearray_bin2D(image,n,m,n,m,fn,'mv')

        do i=1,n,2
         do j=1,m
          indy=j-1 +1
          indx=(i-1)/2  +1
          realim(indx,indy)=image(i,j)
          imagim(indx,indy)=image(i+1,j)
          absim(indx,indy)=sqrt(realim(indx,indy)*realim(indx,indy)+
     /            imagim(indx,indy)*imagim(indx,indy))
         end do
        end do
        fn=f1(1:nchar(f1))//'.fft.real'
        call writearray_bin2D(realim,n,m,n,m,fn,'mv')
        fn=f1(1:nchar(f1))//'.fft.imag'
        call writearray_bin2D(imagim,n,m,n,m,fn,'mv')
        fn=f1(1:nchar(f1))//'.fft.abs'
        call writearray_bin2D(absim,n,m,n,m,fn,'mv')


c        do 100 j=1,m
c         do 110 i=1,n
c          image(i,j)=image(i,j)/(n*m/2)
c110      continue
c100     continue

c        fn='fmod'
c        call writearray_bin2D(image,n,m,n,m,fn,'mv')

        return
        end



