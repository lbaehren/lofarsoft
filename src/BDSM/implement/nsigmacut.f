c! create n-sigma clipped image

        subroutine nsigmacut(f1,f2,nsig)
        implicit none
        character f1*(*),f2*(*),extn*10
        integer n,m
        real*8 nsig

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_nsigmacut(f1,f2,nsig,n,m)
        
        return
        end
c!
c!
        subroutine sub_nsigmacut(f1,f2,nsig,n,m)
        implicit none
        character f1*(*),f2*(*),extn*10
        integer i,j,n,m,nchar
        real*8 nsig,image1(n,m),std,av

        call readarray_bin(n,m,image1,n,m,f1,extn)

c! calculate std properly by 5-sigma clip
        call sigclip(image1,n,m,1,1,n,m,std,av,3)

        do 100 i=1,n
         do 110 j=1,m
          if (abs(image1(i,j)-av).lt.nsig*std) image1(i,j)=0.d0 
110      continue
100     continue

        call writearray_bin(image1,n,m,n,m,f2,'mv')

        return
        end

