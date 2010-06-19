c! put pixel values directly on image

        subroutine callgetpixel(f1,code)
        implicit none
        character f1*500,code*2,extn*20
        integer n,m,l

cf2py   intent(in) f1,code

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call getpixel(f1,code,n,m)

        return
        end

c! -------------------------------------------------------------------

        subroutine getpixel(f1,code,n,m)
        implicit none
        integer n,m,x,y,round4,nchar,i,j
        real*8 flux
        character str1*5,lab*500,ch1*1,code*2,f1*500,extn*20
        real*4 arr4(n,m),xcur,ycur
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        call array8to4(image,n,m,arr4,n,m)
        str1='/xs'
        call pgbegin(0,str1(1:nchar(str1)),1,1)
        lab=' '

        call pgvport(0.1,0.9,0.1,0.9)
        call basicgrey(arr4,n,m,str1,lab)

        if (code.eq.'wr') then
555      write (*,'(a,$)') '   Enter coordinates (0,0 to quit) : '
         read (*,*) x,y
         if (x.ge.1.and.x.le.n.and.y.ge.1.and.y.le.m) then
          write (*,'(a18,i5,1x,i5,a22,1Pe13.6,a)') 
     /      '   Coordinates  : ',x,y,' ;     Flux density : ',
     /          image(x,y),' Jy/beam'
          goto 555
         end if
        end if

        if (code.eq.'tv') then
444     continue
         call pgvport(0.1,1.0,0.1,1.0)
         call pgwindow(0.0,1.0,0.0,1.0)
         call pgtext(0.1,0.97,'Click on pixel or '//
     /        ' anywhere else to quit')
         call pgvport(0.1,0.9,0.1,0.9)
         call pgwindow(0.5,n+0.5,0.5,m+0.5)
         call pgcurs(xcur,ycur,ch1)
         if (xcur.ge.0.0.and.xcur.le.n*1.0+0.5.and.
     /       ycur.ge.0.0.and.ycur.le.m*1.0+0.5) then
          write (*,'(a18,i5,1x,i5,a22,1Pe13.6,a)') 
     /      '   Coordinates  : ',int(xcur),int(ycur),
     /      ' ;     Flux density : ',image(int(xcur),int(ycur))*1.d3,
     /      ' mJy/beam'
         else
          goto 333
         end if
         goto 444
333      continue
         call pgend
        end if
        write (*,*) 

        return
        end





