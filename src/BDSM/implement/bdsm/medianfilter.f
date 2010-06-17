c! median filter an image

        subroutine medianfilter(ft,f1,f2)
        implicit none
        integer n,m,ft,l
        character f1*(*),f2*(*),extn*20

cf2py   intent(in) f1,f2,ft

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_medianfilter(ft,f1,f2,n,m)

        return
        end
c!
c!
        subroutine sub_medianfilter(ft,f1,f2,n,m)
        implicit none
        integer n,m,ft,i,j,i1,j1,nchar,mask(n,m)
        real*8 image2(n,m),filter(ft,ft),med,image1(n,m),vec(ft*ft)
        character lab*500,strdev*5,f1*(*),f2*(*),lab2*500,extn*20

        extn='.img'
        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        write (*,*) '  Median filtering the data ... '
        do 200 i=ft/2+1,n-ft/2
         do 210 j=ft/2+1,m-ft/2   ! i,j is centre of median filter
          do 220 i1=1,ft
           do 230 j1=1,ft
            filter(i1,j1)=image1(i-(ft+1)/2+i1,j-(ft+1)/2+j1)
230        continue
220       continue
          call arr8tovec8(filter,ft,ft,ft,ft,vec,ft*ft)
          call calcmedian(vec,ft*ft,1,ft*ft,med)
          image2(i,j)=med
210      continue
200     continue
        write (*,*) '  CORRECT THIS !!!!'
c! now fill up the boundaries
        call fillbt(image2,n,m,n,m,ft,mask)
        call filllr(image2,n,m,n,m,ft,mask)
        call filledges(image2,n,m,n,m,ft,mask)

        call writearray_bin2D(image2,n,m,n,m,f2,'mv')

        write (*,'(a30,$)') '   Image of median ... <Ret>  '
        read (*,*) 
        strdev='/xs'
        lab='Image of median'
        lab2='Original image'
        call grey2(image2,image1,n,m,n,m,strdev,lab,lab2,1,0)

        return
        end


