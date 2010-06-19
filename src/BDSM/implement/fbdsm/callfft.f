
        subroutine callfft(f1,scratch,runcode)
        implicit none
        character f1*500,scratch*500,runcode*2,extn*20
        integer n,m,l,i,j
        real*8 dumr1,dumr2
        character ch1 

cf2py   intent(in) f1,scratch,runcode

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        dumr1=(dlog10(n*1.d0)/dlog10(2.d0))
        dumr2=(dlog10(m*1.d0)/dlog10(2.d0))
        ch1='n'
        i=n
        j=m 
        if (int(dumr1).ne.dumr1.or.int(dumr2).ne.dumr2) then
         write (*,*) '  Only powers of 2 !!!!'
333      write (*,'(a,$)') '   Shall I pad with zeros (y/q) : '
         read (*,*) ch1
         if (ch1.ne.'y'.and.ch1.ne.'q') goto 333
        end if
        if (ch1.eq.'y') then
         i=2**(int(dumr1)+1)
         j=2**(int(dumr2)+1)
        end if
        if (ch1.ne.'q') 
     /     call do_fft(f1,n,m,l,scratch,runcode,ch1,i,j)

        return
        end
c!
c!
c!
        subroutine do_fft(f1,n,m,l,scratch,runcode,ch1,x,y)
        implicit none
        character f1*500,scratch*500,runcode*2,extn*20,fn*500,ch1
        integer n,m,l,i,j,indx,indy,nchar,x,y
        real*8 image1(n,m),im(x,y)
        real*8 image2(x,y),realim(x,y),imagim(x,y),absim(x,y)
        double complex speq(m)

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)
        call initialiseimage(realim,x,y,x,y,0.d0)
        call initialiseimage(imagim,x,y,x,y,0.d0)
        call initialiseimage(absim,x,y,x,y,0.d0)

        do j=1,m
         do i=1,n
          image2(i,j)=image1(i,j)
         end do
        end do
        if (ch1.eq.'y') then
         do j=m+1,y
          do i=n+1,x
           image2(i,j)=0.d0
          end do
         end do
        end if

        call rlft3(image2,speq,x,y,1,1)

        fn=f1(1:nchar(f1))//'.fft'
        call writearray_bin2D(image2,x,y,x,y,fn,'mv')

        call rlft3(image2,speq,x,y,1,-1)
        call imageop(image2,n,m,im,'mul',2.d0/n/m)
        call copyarray(im,n,m,n,m,1,1,n,m,image2)
        fn=f1(1:nchar(f1))//'.fft.fft'
        call writearray_bin2D(image2,x,y,x,y,fn,'mv')

        do i=1,n,2
         do j=1,m
          indy=j-1 +1
          indx=(i-1)/2  +1
          realim(indx,indy)=image2(i,j)
          imagim(indx,indy)=image2(i+1,j)
          absim(indx,indy)=sqrt(realim(indx,indy)*realim(indx,indy)+
     /            imagim(indx,indy)*imagim(indx,indy))
         end do
        end do
        fn=f1(1:nchar(f1))//'.fft.real'
        call writearray_bin2D(realim,x,y,x,y,fn,'mv')
        fn=f1(1:nchar(f1))//'.fft.imag'
        call writearray_bin2D(imagim,x,y,x,y,fn,'mv')
        fn=f1(1:nchar(f1))//'.fft.abs'
        call writearray_bin2D(absim,x,y,x,y,fn,'mv')

        do 100 j=1,m
         do 110 i=1,n
          absim(i,j)=absim(i,j)/(n*m/2)
110      continue
100     continue

        return
        end

