c! median filter an image

        subroutine medianfilter(ft,f1,f2)
        implicit none
        integer n,m,ft
        character f1*(*),f2*(*),extn*10

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_medianfilter(ft,f1,f2,n,m)

        return
        end
c!
c!
        subroutine sub_medianfilter(ft,f1,f2,n,m)
        implicit none
        integer n,m,ft,i,j,i1,j1,nchar
        real*8 image2(n,m),filter(ft,ft),med,image1(n,m)
        character lab*500,strdev*5,f1*(*),f2*(*),lab2*500,extn*10

        extn='.img'
        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        write (*,*) ' Now median filtering the data ... '
        do 200 i=ft/2+1,n-ft/2
         do 210 j=ft/2+1,m-ft/2   ! i,j is centre of median filter
          do 220 i1=1,ft
           do 230 j1=1,ft
            filter(i1,j1)=image1(i-(ft+1)/2+i1,j-(ft+1)/2+j1)
230        continue
220       continue
          call calcmedian(filter,ft,ft,med)
          image2(i,j)=med
210      continue
200     continue
        write (*,*) 'CORRECT THIS !!!!'
c! now fill up the boundaries
        call fillbt(image2,n,m,ft)
        call filllr(image2,n,m,ft)
        call filledges(image2,n,m,ft)

        call writearray_bin(image2,n,m,n,m,f2,'mv')

        write (*,'(a28,$)') ' Image of median ... <Ret>  '
        read (*,*) 
        strdev='/xs'
        lab='Image of median'
        lab2='Original image'
        call grey2(image2,image1,n,m,n,m,strdev,lab,lab2,1,0)

        return
        end


