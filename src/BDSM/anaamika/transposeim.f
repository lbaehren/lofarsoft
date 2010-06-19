c! transpose image

        subroutine transposeim(f1,f2)
        implicit none
        character f1*(*),f2*(*),extn*20
        integer nchar,n,m,l

cf2py   intent(in) f1,f2

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_transposeim(f1,f2,n,m)
        
        return
        end
c!
c!
        subroutine sub_transposeim(f1,f2,n,m)
        implicit none
        character f1*(*),f2*(*),extn*20
        integer nchar,n,m,i,j
        real*8 image1(n,m),image2(n,m)

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        do 100 i=1,m
         do 110 j=1,n
          image2(i,j)=image1(j,i)
110      continue
100     continue
        
        call writearray_bin2D(image2,n,m,n,m,f2,'mv')
        
        return  
        end


