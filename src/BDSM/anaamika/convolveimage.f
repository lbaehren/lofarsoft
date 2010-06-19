c! convolves image with gaussian or boxfilter or anything else u can think of.

        subroutine convolveimage(filen1,filen2,filter,scratch,srldir)
        implicit none
        character filen1*(*),filen2*(*),extn*20,filter*4
        character scratch*500,srldir*500
        integer n,m,err,l
        real*8 sigma(3)

cf2py   intent(in) filen1,filen2,filter,scratch,srldir

c!   *** read in input image ***
        extn='.img'
        call readarraysize(filen1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'

c! filter parameters
        if (filter.eq.'gaus') then
444      write (*,*) '  FWHM (bmaj, bmin, bpa) : '
         call system('rm -f a b')
         call getininp 
         call readininp(3,err)
         if (err.eq.1) goto 444
         open(unit=21,file='b',status='old')
          read (21,*) sigma(1)
          read (21,*) sigma(2)
          read (21,*) sigma(3)
         close(21)
         call system('rm -f a b')
        end if

        if (filter.eq.'boxf') then
445      write (*,*) '  Width (x, y) : '
         call system('rm -f a b')
         call getininp 
         call readininp(2,err)
         if (err.eq.1) goto 445
         open(unit=21,file='b',status='old')
          read (21,*) sigma(1)
          read (21,*) sigma(2)
         close(21)
         call system('rm -f a b')
         sigma(3)=0.d0
        end if
        
        call sub_convolveimage(filen1,filen2,filter,sigma,n,m,
     /       scratch,srldir)
        
        return
        end
c!
c! -----------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_convolveimage(filen1,filen2,filter,sigma,
     /             n,m,scratch,srldir)
        implicit none
        character filen1*(*),filen2*(*),extn*20,filter*4,scratch*500
        character srldir*500 
        integer n,m
        real*8 sigma(3)

        if (filter.eq.'gaus') 
     /   call convolveimage_gaus(filen1,filen2,filter,sigma,n,m,
     /        scratch,scratch)
        if (filter.eq.'boxf') 
     /   call convolveimage_boxf(filen1,filen2,filter,sigma,n,m,
     /        scratch,scratch)

        return
        end
c!
c!      ----------
c!
        subroutine convolveimage_gaus(filen1,filen2,filter,sigma,
     /             n,m,scratch,srldir)
        implicit none
        character filen1*(*),filen2*(*),strdev*5,lab*500,dums*1
        character srldir*500
        character bcon*10,bcode*4,extn*20,filter*4,dir*500,scratch*500
        integer n,m,nchar,i,j
        real*8 sigma(3),image1(n,m),image2(n,m),a(3),fac,s1,keyvalue

        extn='.img'
        call readarray_bin(n,m,image1,n,m,filen1,extn)

        sigma(1)=sigma(1)/2.35d0 ! fwhm --> sigma
        sigma(2)=sigma(2)/2.35d0 ! fwhm --> sigma
        bcon='periodic'
        bcode='full'
        call conv2(image1,n,m,n,m,sigma,image2,bcon,bcode,fac,s1)
        write (*,'(2x,a36,$)') ' Image of convolved noise ... <RET> '
        read (*,*)
        lab='Noise image convolved with gaussian'
        strdev='/xs'
        call grey(image2,n,m,n,m,strdev,lab,0,1,'hn',1,scratch,filen2,
     /       srldir)
        call imstat(image2,n,m,n,m,a,'vy')

        call writearray_bin2D(image2,n,m,n,m,filen2,'mv')

        return
        end
c!
c!      ----------
c!
        subroutine convolveimage_boxf(filen1,filen2,filter,sigma,
     /             n,m,scratch,srldir)
        implicit none
        character filen1*(*),filen2*(*),strdev*5,lab*500,dums*1
        character bcon*10,bcode*4,extn*20,filter*4,scratch*500
        character srldir*500
        integer n,m,nchar,i,j
        real*8 sigma(3),image1(n,m),image2(n,m),a(3),fac,s1,keyvalue

        extn='.img'
        call readarray_bin(n,m,image1,n,m,filen1,extn)

        call boxf(image1,n,m,n,m,sigma,image2)
        write (*,'(2x,a36,$)') ' Image of convolved noise ... <RET> '
        read (*,*)
        lab='Noise image convolved with boxcar'
        strdev='/xs'
        call grey(image2,n,m,n,m,strdev,lab,0,1,'hn',1,scratch,
     /       filen2,srldir)
        call imstat(image2,n,m,n,m,a,'vy')

        call writearray_bin2D(image2,n,m,n,m,filen2,'mv')

        return
        end

