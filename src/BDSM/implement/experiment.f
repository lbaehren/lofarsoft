c! to play with

        subroutine experiment(f1)
        implicit none
        character f1*500,fn*500,f2*500,extn*10
        real*8 nsig
        integer nchar,n,m
c        real*8 image(2048,2048)
c        character sav*1
        
c        fn='image/'//f1
c        open(unit=21,file=fn(1:nchar(fn)),status='old',
c     /       form='unformatted')
c        read (21) n,m
c        call readarray_bin(n,m,image,2048,2048,21)
c        close(21)
        
c        call trial(image,2048,2048,n,m)

c        sav='y'
c        nsig=3.d0
c        call formisland_rank(image,2048,2048,n,m,nsig,sav,f1)

c        extn='.img' 
c        call readarraysize(f1,extn,n,m)
c        call testfft(f1,n,m)
        
c        call islandedit(f1)
        
        call mulsrc(f1)
c         call plotisland(f1)

c        call associatesrl(f1,f2)

c        call preprocess(f1)

        return
        end
        


        subroutine testfft(f1,n,m)
        implicit none
        integer n,m,i,j
        real*8 image1(n,m),image(n,m),cimage(n,m)
        double complex speq(m)
        character fn*500,f1*500,extn*10
        
        extn='.img' 
        call readarray_bin(n,m,image1,n,m,f1,extn)
        call copyarray(image1,n,m,n,m,1,1,n,m,image)
        call copyarray(image,n,m,n,m,1,1,n,m,cimage)

        call rlft3(image,speq,n,m,1,1)
        fn='fftmod'
        call writearray_bin(image,n,m,n,m,fn,'mv')

        call rlft3(image,speq,n,m,1,-1)

        do 100 i=1,n
         do 110 j=1,m
          image(i,j)=image(i,j)/(n*m/2)
110      continue
100     continue

        fn='fmod'
        call writearray_bin(image,n,m,n,m,fn,'mv')

        return
        end
