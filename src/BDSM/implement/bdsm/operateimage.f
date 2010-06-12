c! to add two images

        subroutine operateimage(f1,f2)
        implicit none
        integer n,m
        character f1*(*),f2*(*),extn*10

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_operateimage(f1,f2,n,m)

        return
        end
c!
c!
        subroutine sub_operateimage(f1,f2,n,m)
        implicit none
        integer pmax,maxmax,i,j,ip,n,m,nchar
        parameter (maxmax=9)
        character f1*(*),f2*(*),extn*10
        real*8 image1(n,m),image2(n,m)
        real*8 coeff(maxmax),pow

333     continue
        write (*,'(3x,a35,$)') '  Maximum polynomial power (< 8) : '
        read (*,*) pmax
        if (pmax.gt.maxmax-1) goto 333
        write (*,'(3x,a23,i1,a2,$)') '  Coeffs from x^0 to x^',pmax,': '
        call system('rm -f a b')
        call getininp 
        call readininp(pmax+1)
        open(unit=21,file='b',status='old')
        do 100 i=1,pmax+1
         read (21,*) coeff(i)
100     continue
        close(21)

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        call initialiseimage(image2,n,m,n,m,coeff(1))

        do 200 i=1,n
         do 210 j=1,m
          pow=1.d0
          do 220 ip=2,pmax+1
           pow=pow*image1(i,j)
           image2(i,j)=image2(i,j)+pow*coeff(ip)
220       continue
210      continue
200     continue

        call writearray_bin(image2,n,m,n,m,f2,'mv')

        return
        end

