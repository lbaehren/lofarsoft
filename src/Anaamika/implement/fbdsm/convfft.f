
      subroutine convfft(scratch)
      implicit none
      character f1*500,f2*500,extn*20,scratch*500
      integer n,m,l
      

      write (*,*) '     file1  file2 '
      read (*,*) f1,f2

      extn='.img'
      call readarraysize(f1,extn,n,m,l)
      call readarraysize(f2,extn,n,m,l)

      call sub_convfft(scratch,f1,f2,n,m,n,m)

      return
      end

c! if i dont pass n,m but take as parameter then (1) equivalence wont
c! work and (2) rlft3 doesnt work either !
      subroutine sub_convfft(scratch,f1,f2,n,m,n1,m1)
      implicit none
      character f1*500,f2*500,extn*20,scratch*500,fn*500
      integer n,m,n1,m1,i,j,nchar,indx,indy
      real*8 image1(n,m),image2(n,m),conv(n,m)
      real*8 realim1(n,m),imagim1(n,m),realim2(n,m),imagim2(n,m)
      real*8 realc(n,m),imagc(n,m),absim(n,m)
      double complex speq1(m),speq2(m),speq(m)

      extn='.img'
      call readarray_bin(n,m,image1,n,m,f1,extn)
      call rlft3(image1,speq1,n,m,1,1)
      call readarray_bin(n,m,image2,n,m,f2,extn)
      call rlft3(image2,speq2,n,m,1,1)

c      call imageop(image1,n,m,conv,'mul',2.d0/n/m)
c      call copyarray(conv,n,m,n,m,1,1,n,m,image1)
c      call imageop(image2,n,m,conv,'mul',2.d0/n/m)
c      call copyarray(conv,n,m,n,m,1,1,n,m,image2)
      do j=1,m
c       speq(j)=2.d0/(n*m)*speq1(j)*speq2(j)
       speq(j)=speq1(j)*speq2(j)
      end do

      call initialiseimage(conv,n,m,n,m,0.d0)
      do i=1,n,2
       do j=1,m
        indy=j
        indx=(i-1)/2+1
        realim1(indx,indy)=image1(i,j)
        imagim1(indx,indy)=image1(i+1,j)
        realim2(indx,indy)=image2(i,j)
        imagim2(indx,indy)=image2(i+1,j)
        realc(indx,indy)=realim1(indx,indy)*realim2(indx,indy)-
     /                   imagim1(indx,indy)*imagim2(indx,indy)
        imagc(indx,indy)=realim1(indx,indy)*imagim2(indx,indy)+
     /                   realim2(indx,indy)*imagim1(indx,indy)
        conv(i,j)=realc(indx,indy)
        conv(i+1,j)=imagc(indx,indy)
       end do
      end do

      fn=f1(1:nchar(f1))//'.conv'
      call writearray_bin2D(conv,n,m,n,m,fn,'mv')
      call rlft3(conv,speq,n,m,1,-1)

      do i=1,n,2
       do j=1,m
        indy=j
        indx=(i-1)/2+1
        realc(indx,indy)=conv(i,j)
        imagc(indx,indy)=conv(i+1,j)
        absim(indx,indy)=sqrt(realc(indx,indy)*realc(indx,indy)+
     /            imagc(indx,indy)*imagc(indx,indy))

       end do
      end do

      fn=f1(1:nchar(f1))//'.convfft'
      call writearray_bin2D(conv,n,m,n,m,fn,'mv')
      fn=f1(1:nchar(f1))//'.fft.real'
      call writearray_bin2D(realc,n,m,n,m,fn,'mv')
      fn=f1(1:nchar(f1))//'.fft.imag'
      call writearray_bin2D(imagc,n,m,n,m,fn,'mv')
      fn=f1(1:nchar(f1))//'.fft.abs'
      call writearray_bin2D(absim,n,m,n,m,fn,'mv')


      return
      end


