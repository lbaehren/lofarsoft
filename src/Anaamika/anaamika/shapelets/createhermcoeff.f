
        implicit none
        character f1*500
        integer order

        f1='hermcoeftr'
        order=30
        call createhermcoeff(f1,order+1)

        end


c! create an array of coeffs of hermite poly upto a max order n into a file
c! write out just the triangular coeff set upto nmax.
c! hn, h(n-1) and h(n+1)
c! coeffs are real else large integers are crap of course.
c! format(100(1Pe25.18,1x))

        subroutine createhermcoeff(f1,nmax)
        implicit none
        character f1*500
        integer i,n,ind,nmax,order,nchar
        real*8 hn(nmax),hnm1(nmax),hnp1(nmax)

        do 100 i=1,nmax
         hnm1(i)=0.d0
         hn(i)=0.d0
100     continue
        hnm1(1)=1.d0
        hn(1)=0.d0
        hn(2)=2.d0

        call system('rm -f '//f1(1:nchar(f1)))
        open(unit=21,file=f1(1:nchar(f1)),status='unknown')
        write (21,777) hnm1
        write (21,777) hn
777     format(100(1Pe25.18,1x))

        do 110 ind=3,nmax  ! ind is for H(ind-1) is H(n+1) so n=ind-2
         n=ind-2

         do 120 i=1,nmax   ! put -2H_n-1 in
          hnp1(i)=-2.d0*n*hnm1(i)
120      continue
         do 130 i=1,nmax   ! 2xH_n in
          if (i.gt.1) hnp1(i)=hnp1(i)+2.d0*hn(i-1)
130      continue
         write (21,777) hnp1
         do 140 i=1,nmax
          hnm1(i)=hn(i)
          hn(i)=hnp1(i)
140      continue
110     continue
        close(21)
        

        return
        end


