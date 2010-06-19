c! makes slices
c! for now only hori and vert

        subroutine slice(f1)
        implicit none
        character f1*(*),extn*20
        integer nchar,n,m,l

cf2py   intent(in) f1

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_slice(f1,n,m)

        return
        end
c!
c!
        subroutine sub_slice(f1,n,m)
        implicit none
        character f1*(*),extn*20
        integer nchar,n,m
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        call getslice(image,n,m,n,m)
        
        return
        end

        subroutine getslice(image,x,y,n,m)
        implicit none
        character str1*1,lab*500,strdev*5,str12*1
        integer n,m,i,xc,yc,x,y,round4,yr
        real*8 image(x,y),nsig
        real*4 arr4(n,m),xcur,ycur,x4(2*n),y4(2*n),mn,mx
        real*4 low,low1,up,std,av
        
333     continue
        write (*,'(a38,$)') '   (v)ertical or (h)orizontal slice ? '
        read (*,*) str1
        if (str1.ne.'h'.and.str1.ne.'v') goto 333

        call array8to4(image,x,y,arr4,n,m)
        strdev='/xs'
        call pgbegin(0,strdev,1,1)
        lab=' '

        call arr2dnz4(arr4,n,m,low,up)
        if (abs(up/low).gt.50.0) then   ! then convert to LG else keep LN
         call converttolog(arr4,n,m,low,up,low1)
        end if
        nsig=5.d0
        call sigclip4(arr4,n,m,1,1,n,m,std,av,nsig)
444     continue
        call pgvport(0.1,1.0,0.3,1.0)
        call sub_basicgrey(n,m,av,std,low,up,arr4,lab)

        call pgvport(0.1,1.0,0.1,1.0)
        call pgwindow(0.0,1.0,0.0,1.0)
        call pgtext(0.1,0.97,'Click on pixel or '//
     /       ' anywhere else to quit')

        call pgvport(0.1,1.0,0.3,1.0)
        call pgwindow(0.5,n+0.5,0.5,m+0.5)
        call pgcurs(xcur,ycur,str12)

        if (xcur.ge.0.0.and.xcur.le.n*1.0+0.5.and.
     /      ycur.ge.0.0.and.ycur.le.m*1.0+0.5) then
         xc=round4(xcur)
         yc=round4(ycur)

         if (str1.eq.'v') then
          do 100 i=1,m
           x4(i)=i*1.0
           y4(i)=image(xc,i)
100       continue
          yr=m
          call range2xakt(y4,2*n,m,mn,mx)
         else
          do 110 i=1,n
           x4(i)=i*1.0
           y4(i)=image(i,yc)
110       continue
          yr=n
          call range2xakt(y4,2*n,n,mn,mx)
         end if   

         call pgeras
         call pgvport(0.1,1.0,0.1,0.3)
         call pgwindow(0.0,yr*1.0,mn,mx)
         call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
         call pgline(yr,x4,y4)

        else
         goto 555
        end if
        goto 444
        
555     continue
        call pgend

        return
        end
