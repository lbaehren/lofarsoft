c! to add two images

        subroutine operateimage(f1,f2)
        implicit none
        integer n,m,l
        character f1*500,f2*500,extn*20

cf2py   intent(in) f1,f2
       
        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_operateimage(f1,f2,n,m)

        return
        end
c!
c!
        subroutine sub_operateimage(f1,f2,n,m)
        implicit none
        integer pmax,maxmax,i,j,ip,n,m,nchar,err
        parameter (maxmax=9)
        character f1*(*),f2*(*),extn*20,choi*1
        real*8 image1(n,m),image2(n,m)
        real*8 coeff(maxmax),pow,aa,bb

333     continue
        write (*,'(3x,a,$)') '  (p)olynomial, (a)bsval po(w)er : '
        read (*,*) choi
        if (choi.ne.'p'.and.choi.ne.'a'.and.choi.ne.'w') goto 333
        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        if (choi.eq.'p') then
         write (*,'(3x,a35,$)') '  Maximum polynomial power (< 8) : '
         read (*,*) pmax
         if (pmax.gt.maxmax-1) goto 333
         write (*,'(3x,a23,i1,a2,$)')'  Coeffs from x^0 to x^',pmax,': '
         call system('rm -f a b')
         call getininp 
         call readininp(pmax+1,err)
         open(unit=21,file='b',status='old')
         do 100 i=1,pmax+1
          read (21,*) coeff(i)
100      continue
         close(21)
         call initialiseimage(image2,n,m,n,m,coeff(1))
         do 200 j=1,m
          do 210 i=1,n
           pow=1.d0
           do 220 ip=2,pmax+1
            pow=pow*image1(i,j)
            image2(i,j)=image2(i,j)+pow*coeff(ip)
220        continue
210       continue
200      continue
        end if
        if (choi.eq.'a') then
         do j=1,m
          do i=1,n
           if (image1(i,j).gt.0.d0) then
            image2(i,j)=image1(i,j)
           else
            image2(i,j)=-image1(i,j)
           end if  
          end do
         end do
        end if
        if (choi.eq.'w') then
         write (*,'(3x,a,$)') '  a*(x^b); a,b : '
         call system('rm -f a b')
         call getininp 
         call readininp(2,err)
         open(unit=21,file='b',status='old')
         read (21,*) aa
         read (21,*) bb
         close(21)
         do j=1,m
          do i=1,n
           image2(i,j)=aa*(image1(i,j)**bb)
          end do
         end do
        end if
        call writearray_bin2D(image2,n,m,n,m,f2,'mv')

        return
        end

