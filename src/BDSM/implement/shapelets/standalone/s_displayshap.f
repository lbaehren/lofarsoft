c! plots two images, their difference the third as well.
        subroutine s_displayshap(fn1,fn2,fn3)
        implicit none
        character fn1*(*),fn2*(*),strdev*5,lab*500,fn3*(*),fn*500
        character extn*20
        real*8 image1(2048,2048),image2(2048,2048)
        integer n,m,n1,m1,nchar,i,j,n2,m2
        real*4 low1,up1,low2,up2,lowd,upd,low3,up3
        real*8 diff(2048,2048),im3(100,100)  ! im3 is shapelet coeff matrix
        COMMON /comblk1/ image1    
        COMMON /comblk2/ image2    

        extn='.img'
        call readarray_bin(n,m,image1,2048,2048,fn1,extn)

        extn='.img'
        call readarray_bin(n1,m1,image2,2048,2048,fn2,extn)

        extn='.img'
        call readarray_bin(n2,m2,im3,100,100,fn3,extn)

        strdev='/xs'
        lab=' '

        call arr2dnz(image1,2048,2048,n,m,low1,up1)
        call arr2dnz(image2,2048,2048,n,m,low2,up2)
        low2=low1
        up2=up1

        if (n.eq.n1.and.m.eq.m1) then
         do 350 i=1,n1
          do 360 j=1,m1
           diff(i,j)=image1(i,j)-image2(i,j)
360       continue
350      continue
        end if

        call arr2dnz(diff,2048,2048,n,m,lowd,upd)
        call arr2dnz(im3,100,100,n2,m2,low3,up3)
        low3=im3(n2,1)
        up3=im3(n2,1)
        do 400 i=1,n2
         do 410 j=1,m2-i
          if (i*j.ne.1) then
           if (im3(i,j).gt.up3) up3=im3(i,j)
           if (im3(i,j).lt.low3) low3=im3(i,j)
          end if
410      continue
400     continue
        up3=sqrt(up3*im3(1,1))

cc        call pgbegin(0,strdev,2,2)

cc        call pgpanl(1,1)
        lab='Image 1'
        call s_plotimage2(2048,2048,n,m,image1,up1,low1,lab,'LG')

cc        call pgpanl(2,1)
        lab='Image 2'
        call s_plotimage2(2048,2048,n1,m1,image2,up2,low2,lab,'RG')

        if (n.eq.n1.and.m.eq.m1) then
cc         call pgpanl(1,2)
         lab='Image 1 - Image 2'
         call s_plotimage2(2048,2048,n,m,diff,upd,lowd,lab,'LG')
 
cc         call pgpanl(2,2)
         lab='Shapelet coeffs'
         call s_plotimage2(100,100,n2,m2,im3,up3,low3,lab,'RG')
        end if
cc        call pgend

        return
        end



        subroutine s_plotimage2(x,y,n,m,arr,up,low,lab,str2)
        implicit none
        integer n,m,i,j,x,y
        character lab*500,str2*2
        real*8 arr(x,y)
        real*4 up,low,array(n,m),tr(6)

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call array8to4(arr,x,y,array,n,m)
        if (up/low.gt.10.0) then   
         call converttolog(array,n,m,low,up)
        end if
cc        call pgvport(0.1,0.9,0.1,0.9)
cc        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
cc        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
cc        call pggray(array,n,m,1,n,1,m,up,low,tr)
cc        call pglabel(' ',' ',lab)
cc        call pgwedg(str2,2.0,3.0,up,low,' ')

        return
        end


