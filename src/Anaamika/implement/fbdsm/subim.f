
        subroutine subim(f1,f2,code)
        implicit none
        character f1*(*),f2*(*),extn*20,code*2
        integer n,m,l

cf2py   intent(in) f1,f2,code

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_subim(f1,f2,code,n,m)

        return
        end
c!
c!
        subroutine sub_subim(f1,f2,code,n,m)
        implicit none
        character f1*(*),f2*(*),fn2*500,ch1*1
        character str1*5,lab*500,code*2,extn*20
        integer nchar,i,j,n,m,round4
        real*8 image1(n,m),image2(n,m)
        real*4 xcur,ycur,xcur1,ycur1,rblc(2),rtrc(2)
        integer blc(2),trc(2)

c! read input image
        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

c! display it and get windoww
        if (code.eq.'tv') then
         str1='/xs'
         lab=' '
         write (*,*) '  tvwin first, then correct blc,trc'
         write (*,*) '  Set the window ... '
         call pgbegin(0,str1,1,1)
         call pgvport(0.1,0.9,0.1,0.9)
         call plotimage(image1,n,m,n,m)
         call pgsci(3)
         call pgwindow(0.5,n+0.5,0.5,m+0.5)
         call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
         call pgsci(6)
         call pgcurs(xcur,ycur,ch1)
         call pgband(2,1,xcur,ycur,xcur1,ycur1,ch1)
         call setzoom(xcur,ycur,xcur1,ycur1,rblc,rtrc,n,m)
         call pgsfs(2)
         call pgrect(rblc(1),rtrc(1),rblc(2),rtrc(2))
         call pgsfs(1)
         call pgsci(1)
         blc(1)=round4(rblc(1))
         blc(2)=round4(rblc(2))
         trc(1)=round4(rtrc(1))
         trc(2)=round4(rtrc(2))
 
c! check if thats fine
         write (*,'(a10,i5,1x,i5)') '   BLC is ',blc(1),blc(2)
         write (*,'(a10,i5,1x,i5)') '   TRC is ',trc(1),trc(2)
         write (*,'(a29,$)') '   Enter new blc trc (y/n) ? '
         read (*,*) ch1
        end if
        if (code.eq.'wr') ch1='y'
333     continue
        if (ch1.eq.'y') then
         write (*,'(a30,$)') '   Enter BLCs and then TRCs : '
         read (*,*) blc(1),blc(2),trc(1),trc(2)
         if (trc(1).le.blc(1).or.trc(2).le.blc(2)) goto 333
        end if
        if (code.eq.'tv') then
         call pgsfs(2)
         call pgsci(5)
         call pgrect(blc(1)*1.0,trc(1)*1.0,blc(2)*1.0,trc(2)*1.0)
         call pgsfs(1)
         call pgsci(1)
         call pgend
        end if

c! SUBIM the image and write it out
        do 100 i=blc(1),trc(1)
         do 110 j=blc(2),trc(2)
          image2(i-blc(1)+1,j-blc(2)+1)=image1(i,j)
110      continue
100     continue


        call writearray_bin2D(image2,n,m,trc(1)-blc(1)+1,
     /       trc(2)-blc(2)+1,f2,'mv')

        return
        end


