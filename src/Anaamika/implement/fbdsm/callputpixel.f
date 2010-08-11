c! put pixel values directly on image

        subroutine callputpixel(f1,f2,code,tcode)
        implicit none
        character f1*500,f2*500,code*3,extn*20,tcode*2
        integer n,m,l,nchar

cf2py   intent(in) f1,f2,code

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_callputpixel(f1,f2,code,n,m,tcode)

        return
        end
c!
c!
        subroutine sub_callputpixel(f1,f2,code,n,m,tcode)
        implicit none
        character f1*500,f2*500,code*3,extn*20,tcode*2
        integer n,m,nchar,i,j
        real*8 image1(n,m),image2(n,m)
        character strdev*5,lab*500

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        do 100 j=1,m
         do 110 i=1,n
          image2(i,j)=image1(i,j)
110      continue
100     continue
                
        strdev='/xs'
        call putpixel(image1,n,m,n,m,image2,strdev,code,tcode)
        call writearray_bin2D(image2,n,m,n,m,f2,'mv')

        return
        end


