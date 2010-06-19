c! actually putting values in pixels using cursor

        subroutine putpixel(im1,x,y,n,m,im2,str1,code,tcode)
        implicit none
        integer n,m,x,y,round4,nchar,i,j
        real*8 flux
        character str1*5,lab*500,ch1*1,code*3,tcode*2
        real*4 arr4(n,m),xcur,ycur
        real*8 im1(x,y),im2(x,y)

        if (tcode.eq.'tv') then
         call array8to4(im1,x,y,arr4,n,m)
         call pgbegin(0,str1(1:nchar(str1)),1,1)
         lab=' '
 
444      continue
         call pgvport(0.1,0.9,0.1,0.9)
         call basicgrey(arr4,n,m,str1,lab)
 
         call pgvport(0.1,1.0,0.1,1.0)
         call pgwindow(0.0,1.0,0.0,1.0)
         call pgtext(0.1,0.97,'Click on pixel or '//
     /       ' anywhere else to quit')

         call pgvport(0.1,0.9,0.1,0.9)
         call pgwindow(0.5,n+0.5,0.5,m+0.5)
         call pgcurs(xcur,ycur,ch1)
 
         if (xcur.ge.0.0.and.xcur.le.n*1.0+0.5.and.
     /      ycur.ge.0.0.and.ycur.le.m*1.0+0.5) then
          write (*,'(a28,$)') '  Flux (Jy) at this pixel : '
          read (*,*) flux
          if (code.eq.'put') arr4(round4(xcur),round4(ycur))=flux
          if (code.eq.'add') arr4(round4(xcur),round4(ycur))=
     /       arr4(round4(xcur),round4(ycur))+flux
         else
          goto 333
         end if
         goto 444

333      continue
         call pgend

         do 100 j=1,m
          do 110 i=1,n
           im2(i,j)=arr4(i,j)
110       continue
100      continue

        else
         call copyarray(im1,n,m,n,m,1,1,n,m,im2)
334      write (*,'(a,$)') '  Coordinate x, y (0,0 to end) : '
         read (*,*) i,j
         if (i.lt.0.or.j.lt.0.or.i.gt.n.or.j.gt.m) goto 334
         if (i.ne.0.and.j.ne.0) then
          write (*,'(a,$)') '  Flux (Jy) : '
          read (*,*) flux
          if (code.eq.'put') im2(i,j)=flux
          if (code.eq.'add') im2(i,j)=im2(i,j)+flux
          goto 334
         end if
        end if

        return
        end


