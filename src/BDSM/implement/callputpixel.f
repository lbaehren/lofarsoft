c! put pixel values directly on image

        subroutine callputpixel(f1,f2,code)
        implicit none
        character f1*(*),f2*(*),code*3,extn*10
        integer n,m

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_callputpixel(f1,f2,code,n,m)

        return
        end
c!
c!
        subroutine sub_callputpixel(f1,f2,code,n,m)
        implicit none
        character f1*(*),f2*(*),code*3,extn*10
        integer n,m,nchar,i,j
        real*8 image1(n,m),image2(n,m)
        character strdev*5,lab*500

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        do 100 i=1,n
         do 110 j=1,m
          image2(i,j)=image1(i,j)
110      continue
100     continue
                
        strdev='/xs'
        call putpixel(image1,n,m,n,m,image2,strdev,code)
        call writearray_bin(image2,n,m,n,m,f2,'mv')

        return
        end




