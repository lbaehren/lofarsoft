c! actually putting values in pixels using cursor

        subroutine putpixel(im1,x,y,n,m,im2,str1,code)
        implicit none
        integer n,m,x,y,round4,nchar,i,j
        real*8 flux
        character str1*5,lab*500,ch1*1,code*3
        real*4 arr4(n,m),xcur,ycur
        real*8 im1(x,y),im2(x,y)

        call array8to4(im1,x,y,arr4,n,m)
c        call pgbegin(0,str1(1:nchar(str1)),1,1)
        lab=' '

444     continue
c        call pgvport(0.1,0.9,0.1,0.9)
        call basicgrey(arr4,n,m,str1,lab)

c        call pgvport(0.1,1.0,0.1,1.0)
c        call pgwindow(0.0,1.0,0.0,1.0)
c        call pgtext(0.1,0.97,'Click on pixel or '//
 

c        call pgvport(0.1,0.9,0.1,0.9)
c        call pgwindow(0.5,n+0.5,0.5,m+0.5)
c        call pgcurs(xcur,ycur,ch1)

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

333     continue
c        call pgend

        do 100 i=1,n
         do 110 j=1,m
          im2(i,j)=arr4(i,j)
110      continue
100     continue

        return
        end


