c! plots two images, their difference and ratios

        subroutine displaytwoim(fn1,fn2)
        implicit none
        character fn1*(*),fn2*(*),extn*20
        integer n,m,n1,m1,l,l1

cf2py   intent(in) fn1,fn2

        extn='.img'
        call readarraysize(fn1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call readarraysize(fn2,extn,n1,m1,l1)
        if (l1.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_displaytwoim(fn1,fn2,n,m,n1,m1)
        
        return
        end
c!
c!
        subroutine sub_displaytwoim(fn1,fn2,n,m,n1,m1)
        implicit none
        character fn1*(*),fn2*(*),strdev*5,lab*500,extn*20
        integer n,m,n1,m1,nchar,i,j
        real*8 image1(n,m),image2(n1,m1)
        real*4 low1,up1,low2,up2,lowd,upd,lowr,upr
        real*8 diff(n,m),rat(n,m)

        extn='.img'
        call readarray_bin(n,m,image1,n,m,fn1,extn)
        call readarray_bin(n1,m1,image2,n1,m1,fn2,extn)

        strdev='/xs'
        lab=' '

        call arr2dnz(image1,n,m,n,m,low1,up1)
        call arr2dnz(image2,n1,m1,n,m,low2,up2)

        if (n.eq.n1.and.m.eq.m1) then
         do 350 j=1,m1
          do 360 i=1,n1
           diff(i,j)=image1(i,j)-image2(i,j)
           rat(i,j)=image1(i,j)/image2(i,j)
360       continue
350      continue
        end if

        call arr2dnz(diff,n,m,n,m,lowd,upd)
        call arr2dnz(rat,n,m,n,m,lowr,upr)

        call pgbegin(0,strdev,2,2)

        call pgpanl(1,1)
        lab=fn1(1:nchar(fn1))
        call plotimage1(n,m,image1,up1,low1,lab,'LG')

        call pgpanl(2,1)
        lab=fn2(1:nchar(fn2))
        call plotimage1(n1,m1,image2,up2,low2,lab,'RG')

        if (n.eq.n1.and.m.eq.m1) then
         call pgpanl(1,2)
         lab='Image 1 - Image 2'
         call plotimage1(n,m,diff,upd,lowd,lab,'LG')
 
         call pgpanl(2,2)
         lab='Image 1 / Image 2'
c         lowr=0.0
c         upr=2.0
         call plotimage1(n,m,rat,upr,lowr,lab,'RG')
        end if

        return
        end



        subroutine plotimage1(n,m,arr,up,low,lab,str2)
        implicit none
        integer n,m,i,j
        character lab*500,str2*2
        real*8 arr(n,m)
        real*4 up,low,array(n,m),tr(6),low1

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call array8to4(arr,n,m,array,n,m)
        if (up/low.gt.10.0) then   
         call converttolog(array,n,m,low,up,low1)
        end if
        call pgvport(0.15,0.85,0.15,0.85)
        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
        call pggray(array,n,m,1,n,1,m,up,low,tr)
        call pglabel(' ',' ',lab)
        call pgwedg(str2,2.0,3.0,up,low,' ')

        return
        end

