! rehash of other progs to give image stats.
c! actual std and mean. and n-sigma clipped std and mean.
c! and maybe median if i write select.f for an array instead of bloody long vector

        subroutine imagestats(f1,kappa,scratch)
        implicit none
        integer i,j,nchar,n,m,mn(2),mx(2),dumi,round,l
        real*8 std,av,mnv,mxv,kappa,stdclip,avclip,blankv
        character f1*(*),ch1*1,strdev*5,lab*500,opt*4,extn*20
        character scratch*500
        logical blanky

cf2py   intent(in) f1,kappa,scratch

c        WRONG TVSTAT
        
        extn='.img'
        blanky=.true. ! no magic blanking
        blankv=-999.d0
        call readarraysize(f1,extn,n,m,l)
        if (l.eq.1) then
         call get_imagestats2D(f1,kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv,blanky,blankv)

         write (*,*) '  Statistics over the entire image ...'
         write (*,'(1x,a19,i5,a3,i5)') '  Image size     = ',n,' X ',m
         write (*,'(1x,a19,1Pe10.3,a14,1Pe10.3,a4)') 
     /    '  Image std      = ',std*1.d3,' mJy ; Mean = ',av*1.d3,' mJy'
         write (*,'(1x,a19,1Pe10.3,a14,i8,a7)') 
     /    '  Total flux     = ',av*n*m,'  Jy ; Area = ',n*m,' pixels'
         write (*,'(1x,i3,a16,1Pe10.3,a14,1Pe10.3,a4)') round(kappa),
     /    '-sig clip std = ',stdclip*1.d3,' mJy ; mean = ',avclip*1.d3
     /    ,' mJy'
         write (*,'(1x,a18,1Pe10.3,a7,i4,a1,i4,a9,0Pf6.1)') 
     /          '  Image maximum = ',mxv,' Jy at ',mx(1),',',mx(2),
     /          '; snr = ',mxv/stdclip
         write (*,'(1x,a18,1Pe10.3,a7,i4,a1,i4,a9,0Pf6.1)') 
     /          '  Image minimum = ',mnv,' Jy at ',mn(1),',',mn(2),
     /          '; snr = ',mnv/stdclip
 
         strdev='/xs'
         lab=' '
         call callplotimage2(f1,n,m,stdclip,avclip)
         call sub_imagestats(f1,n,m)
        else
         call get_imagestats3D(f1,kappa,n,m,l,blanky,blankv,scratch)
        end if

        return
        end

        subroutine plotimage2(array,x,y,n,m,std,av)
        implicit none
        integer n,m,x,y,dumi,i,nbin,id1,pgopen,nchar
        real*8 array(x,y),std,av
        real*4 arr4(n,m),mn1,mx1,mn2,mx2,low,up,x4(100),y4(100)
        character strdev*5

        mn1=av-3.0*std
        mx1=av+7.0*std
        call pgbegin(0,'/NULL',1,1)
        dumi=n*m
        call array8to4(array,x,y,arr4,n,m)
        call pghist(dumi,arr4,mn1,mx1,100,0)
        call pgqwin(mn1,mx1,mn2,mx2)
        call pgend

        strdev='/xs'
        id1=pgopen(strdev(1:nchar(strdev)))   ! image
        if (id1.le.0) stop
        call pgslct(id1)
        call pgpap(9.0,1.0)
        call pgpage
        call pgask(.false.)
        call pgvport(0.1,0.9,0.3,0.9)
        call plotimage(array,x,y,n,m)

        call pgvport(0.1,0.9,0.1,0.3)
        call pgwindow(mn1,mx1,mn2,mx2)
        call pgbox('BNST',0.0,0,'BCVNST',0.0,0)
        call pghist(n*m,arr4,mn1,mx1,100,1)
        nbin=100
        do 100 i=1,nbin
         x4(i)=mn1+(i-1)*(mx1-mn1)/nbin
         y4(i)=mx2*0.8*dexp(-0.5d0*(((x4(i)-av)/std)**2.d0))
100     continue
        call pgline(nbin,x4,y4)
        call pgclos

        return
        end



        subroutine plotimage(array,x,y,n,m)
        implicit none
        integer n,m,x,y
        real*8 array(x,y),nsig
        real*4 arr4(n,m),mn,mx,tr(6),low1,std4,av4

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        nsig=7.d0
        call arr2dnz(array,x,y,n,m,mn,mx)
        call array8to4(array,x,y,arr4,n,m)
        call sigclip4(arr4,n,m,1,1,n,m,std4,av4,nsig)
c        if (mx/mn.gt.50.0) then
c         call converttolog(arr4,n,m,mn,mx,low1)
c        end if
        mn=av4-10.0*std4
        mx=av4+10.0*std4
        call pgsci(3)
        call pgwindow(0.5,n+0.5,0.5,m+0.5)
        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
        call pggray(arr4,n,m,1,n,1,m,mx,mn,tr)
        call pgwedg('RG',2.0,3.0,mx,mn,' ')
        call pgsci(1)

        return
        end




        subroutine setgetoption(opt)
        implicit none
        character opt*4,ch1*1

        call pgvport(0.0,1.0,0.0,1.0)
        call pgwindow(0.0,1.0,0.0,1.0)
        call pgtext(0.3,0.97,'Rect. area    or irregular area    or '//
     /       ' anywhere else to quit')
        call clickop(0.35,0.94,0.5,0.94,0.015,ch1)
        if (ch1.eq.'y') opt='box'
        if (ch1.eq.'n') opt='area'
        if (ch1.eq.'x') opt='quit'

        return
        end


        subroutine boxstats(array,x,y,n,m)
        implicit none
        integer x,y,n,m,area,i,mx(2),j
        real*8 array(x,y)
        real*4 xcur,ycur,xcur1,ycur1,blc(2),trc(2)
        character ch1*1
        real*8 std,av

        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(0.5,n+0.5,0.5,m+0.5)
        call pgcurs(xcur,ycur,ch1)
        call pgsci(6)
        call pgband(2,1,xcur,ycur,xcur1,ycur1,ch1)
        call setzoom(xcur,ycur,xcur1,ycur1,blc,trc,n,m)
        call pgsfs(2)
        call pgrect(blc(1),trc(1),blc(2),trc(2))
        call pgsfs(1)
        call pgsci(1)
        
        call sigclip(array,x,y,int(blc(1)),int(blc(2)),int(trc(1)),
     /               int(trc(2)),std,av,3.d0)

        write (*,'(1x,a19,1Pe10.3,a14,1Pe10.3,a4)') 
     /   '  Clipped std    = ',std*1.d3,' mJy ; Mean = ',av*1.d3,' mJy'
        area=(trc(1)-blc(1)+1)*(trc(2)-blc(2)+1)
        write (*,'(1x,a19,1Pe10.3,a14,i8,a7)') 
     /   '  Total flux     = ',av*area,'  Jy ; Area = ',area,' pixels'

        mx(1)=1
        mx(2)=1
        do 100 i=int(blc(1)),int(trc(1))
         do 110 j=int(blc(2)),int(trc(2))
          if (array(i,j).gt.array(mx(1),mx(2))) then
           mx(1)=i
           mx(2)=j
          end if
110      continue
100     continue
        write (*,'(1x,a18,1Pe10.3,a7,i4,a1,i4,a9,0Pf6.1)') 
     /         '  Image maximum = ',array(mx(1),mx(2)),' Jy at ',
     /         mx(1),',',mx(2)
        return
        end


        subroutine tvstat(array,x,y,n,m)
        implicit none
        integer x,y,n,m,nver,xver(100),yver(100),mask(n,m)
        real*8 array(x,y),sl(100),inter(100),std,av

        write (*,*) 
        write (*,*) 
        write (*,*) '  Sorry, doesnt work yet'
        write (*,*) 
        write (*,*) 
        goto 455
        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(0.5,n+0.5,0.5,m+0.5)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)

        call setvertices(nver,xver,yver,n,m)   ! nver vertices with x,y as xver,yver
        call getmc(nver,xver,yver,sl,inter) ! get nver slopes and intercepts
        call getmask(mask,n,m,nver,xver,yver,sl,inter) ! mask has 1/0 if inside/outside
        call calcstatsmask(array,x,y,n,m,mask,std,av)
        
        write (*,'(1x,a14,1Pe10.3,a14,1Pe10.3,a4)') 
     /     '  Image std = ',std*1.d3,' mJy ; mean = ',av*1.d3,' mJy'
        
455     continue

        return
        end


        subroutine setvertices(nver,xver,yver,n,m)
        implicit none
        integer nver,xver(100),yver(100),n,m,xx,yy,round4
        character ch*1
        real*4 x,y

        write (*,*) '  Click on vertices, click outside to end'
        nver=0
333     continue
        call pgcurs(x,y,ch)
        xx=round4(x)
        yy=round4(y)
        call pgsci(5)
        call pgsfs(1)
        if (xx.ge.1.and.xx.le.n.and.yy.ge.1.and.yy.le.m) then
         nver=nver+1
         xver(nver)=xx
         yver(nver)=yy
         call pgrect(xx-0.5,xx+0.5,yy-0.5,yy+0.5)
         if (nver.gt.1) then
          call pgmove(xver(nver-1)*1.0,yver(nver-1)*1.0)
          call pgdraw(x,y)
         end if
         goto 333
        end if
        call pgmove(xver(1)*1.0,yver(1)*1.0)
        call pgdraw(xver(nver)*1.0,yver(nver)*1.0)
        call pgsci(1)


        return
        end




        subroutine getmc(nver,xver,yver,sl,inter) 
        implicit none
        integer nver,xver(100),yver(100),i
        real*8 sl(100),inter(100),x(nver),y(nver)

        do 200 i=1,nver
         x(i)=xver(i)*1.d0
         y(i)=yver(i)*1.d0
200     continue

        do 100 i=1,nver-1
         if (x(i).ne.x(i+1)) then 
          sl(i)=(y(i)-y(i+1))/(x(i)-x(i+1))
          inter(i)=(y(i)*x(i+1)-y(i+1)*x(i))/
     /            (x(i+1)-x(i))
         else
          sl(i)=99.d9
          inter(i)=x(i)   ! code x=const like this
         end if
100     continue
        if (x(nver).ne.x(1)) then 
         sl(nver)=(y(nver)-y(1))/(x(nver)-x(1))
         inter(nver)=(y(nver)*x(1)-y(1)*x(nver))/
     /            (x(1)-x(nver))
        else
         sl(nver)=99.d9
         inter(nver)=x(1)   ! code x=const like this
        end if

        write (*,*) sl(1),inter(1),1
        write (*,*) sl(2),inter(2),2
        write (*,*) sl(3),inter(3),3
        write (*,*) sl(4),inter(4),4

        return
        end



        subroutine getmask(mask,n,m,nver,xver,yver,sl,inter) 
        implicit none
        integer n,m,nver,xver(100),yver(100),mask(n,m)
        real*4 xuse(100,2),yuse(100,2),xx(100,2),yy(100,2)
        integer nintr(n,m,4),nn,i,j
        real*8 sl(100),inter(100),ex,why,x,y

c! need to do complex idiocy. cos u have to take care of case where pt considered
c! intersects a vertex in one direction. u dont want to count twice so u give gt and le
c! for the two vertices but order shud be preserved blah blah. maha pain.
        call calclines(nver,xver,yver,sl,inter,xuse,yuse)

c! create the two x,y pairs for each line. to take care of per bndry cond
        do 300 i=1,nver
         xx(i,1)=min(xuse(i,1),xuse(i,2))
         xx(i,2)=max(xuse(i,1),xuse(i,2))
         yy(i,1)=min(yuse(i,1),yuse(i,2))
         yy(i,2)=max(yuse(i,1),yuse(i,2))
        write (*,*) xx(i,1),yy(i,1),xx(i,2),yy(i,2),'  @@'
        call pgsci(6)
        call pgrect(xuse(i,1)-0.5,xuse(i,1)+0.5,yuse(i,1)-0.5,
     /         yuse(i,1)+0.5)
        call pgsci(7)
        call pgrect(xuse(i,2)-0.5,xuse(i,2)+0.5,yuse(i,2)-0.5,
     /         yuse(i,2)+0.5)
300     continue
        if (nver.gt.99) write (*,*) '  ###  Too many vertices'

c! now for actual computing
        do 100 i=1,n
         do 110 j=1,m
          x=i*1.0
          y=j*1.0
          do 115 nn=1,4
           nintr(i,j,nn)=0  ! for each side
115       continue
          do 120 nn=1,nver

c! y=y1 
           if (sl(nn).ne.0.d0.and.sl(nn).ne.99.d9) then
            ex=(y-inter(nn))/sl(nn)   
            if (ex.ge.xx(nn,1).and.ex.lt.xx(nn,2)) then
             if (ex.ge.x) nintr(i,j,1)=nintr(i,j,1)+1  ! on right
             if (ex.lt.x) nintr(i,j,2)=nintr(i,j,2)+1  ! on left
            end if
           else
            if (sl(nn).eq.99.d9) then
             if (y.ge.yy(nn,1).and.y.lt.yy(nn,2)) then
              if (xx(nn,1).ge.x) nintr(i,j,1)=nintr(i,j,1)+1  ! on right
              if (xx(nn,1).lt.x) nintr(i,j,2)=nintr(i,j,2)+1  ! on left
             end if
            end if
           end if
         if (i.eq.100.and.y.eq.yver(4)) 
     /      write (*,*) nn,x,y,nintr(i,j,1),nintr(i,j,2),
     /           ex,x,xx(nn,1),xx(nn,2)
          
c! x=x1
           if (sl(nn).ne.0.d0.and.sl(nn).ne.99.d9) then
            why=inter(nn)+sl(nn)*x
            if (why.ge.yy(nn,1).and.why.lt.yy(nn,2)) then
             if (why.ge.y) nintr(i,j,3)=nintr(i,j,3)+1  ! on top
             if (why.lt.y) nintr(i,j,4)=nintr(i,j,4)+1  ! on bottom
            end if
           else
            if (sl(nn).eq.0.d0) then
             if (x.ge.xx(nn,1).and.x.lt.xx(nn,2)) then
              if (why.ge.y) nintr(i,j,3)=nintr(i,j,3)+1  ! on top
              if (why.lt.y) nintr(i,j,4)=nintr(i,j,4)+1  ! on bottom
             end if
            end if
           end if

120       continue

          mask(i,j)=0
          if (mod(nintr(i,j,1),2).eq.1.and.mod(nintr(i,j,2),2).eq.1.
     /     and.mod(nintr(i,j,3),2).eq.1.and.mod(nintr(i,j,4),2).eq.1)
     /    mask(i,j)=1 
          if (mask(i,j).eq.1) then
           call pgsfs(1)
           call pgsci(4)
           call pgrect(i-0.5,i+0.5,j-0.5,j+0.5)
          end if

110      continue
100     continue

        return
        end




        subroutine calcstatsmask(array,x,y,n,m,mask,std,av)
        implicit none
        integer x,y,n,m
        integer mask(n,m)
        real*8 std,av,array(x,y)


        return
        end






c! calculates the lines in order of clicking but second pt (,2) is one pixel less
c! on line.
        subroutine calclines(nver,xver,yver,sl,inter,xuse,yuse)
        implicit none
        integer nver,xver(100),yver(100)
        real*4 xuse(100,2),yuse(100,2)
        integer i
        real*8 sl(100),inter(100)
        
        xver(nver+1)=xver(1)
        yver(nver+1)=yver(1)
        do 100 i=1,nver
         xuse(i,1)=xver(i)
         yuse(i,1)=yver(i)

         if (sl(i).ne.99.d9) then
          if (xver(i+1).gt.xver(i)) xuse(i,2)=xver(i+1)-1
          if (xver(i+1).lt.xver(i)) xuse(i,2)=xver(i+1)+1
         end if
         if (sl(i).ne.0.d0.and.sl(i).ne.99.d9) then
          yuse(i,2)=sl(i)*xuse(i,2)+inter(i)
         end if
         if (sl(i).eq.0.d0) then
          yuse(i,2)=yver(i+1)
         end if

         if (sl(i).eq.99.d9) then
          if (yver(i+1).gt.yver(i)) yuse(i,2)=yver(i+1)-1
          if (yver(i+1).lt.yver(i)) yuse(i,2)=yver(i+1)+1
          xuse(i,2)=xver(i+1)
         end if

100     continue


        return
        end

c! rt now take in blank value (if blanky=true) and create a mask and change arrstat to
c! arrstatmask and same for sigclip. silly, shud be able to pass mask directly. 
        subroutine get_imagestats2D(f1,kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv,blanky,blankv)
        implicit none
        integer i,j,nchar,n,m,mn(2),mx(2),dumi,round,mask(n,m),blankn
        real*8 std,av,mnv,mxv,kappa,stdclip,avclip
        character f1*(*),ch1*1,strdev*5,lab*500,opt*4,extn*20
        real*8 image(n,m),blankv
        logical blanky

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call get_im_agestats2D(kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv,blanky,blankv,blankn,image)
        
        return
        end
c!
c!
c!
        subroutine get_im_agestats2D(kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv,blanky,blankv,blankn,image)
        implicit none
        integer i,j,nchar,n,m,mn(2),mx(2),dumi,round,mask(n,m),blankn
        real*8 std,av,mnv,mxv,kappa,stdclip,avclip
        character ch1*1,strdev*5,lab*500,opt*4,extn*20
        real*8 image(n,m),blankv
        logical blanky

        blankn=0
        if (blanky) then
         do j=1,m
          do i=1,n
           if (image(i,j).eq.blankv) then
            mask(i,j)=0
            blankn=blankn+1
           else
            mask(i,j)=1
           end if
          end do
         end do
        end if

        if (blanky.eqv..true.) then
         if (blankn.gt.0) then
          if (blankn.lt.n*m) then
           call arrstatmask(image,mask,n,m,1,1,n,m,std,av,'v')
           call sigclipmask(image,mask,n,m,1,1,n,m,stdclip,avclip,
     /          kappa)
          else
           std=0.d0
           av=0.d0
           stdclip=0.d0
           avclip=0.d0
          end if
         else
          call arrstat(image,n,m,1,1,n,m,std,av)
          call sigclip(image,n,m,1,1,n,m,stdclip,avclip,kappa)
         end if
        else
         call arrstat(image,n,m,1,1,n,m,std,av)
         call sigclip(image,n,m,1,1,n,m,stdclip,avclip,kappa)
        end if

        mn(1)=n/2
        mx(1)=n/2
        mn(2)=m/2
        mx(2)=m/2

        if (blanky.eqv..false.) then ! to save computing time split this up
         do 110 j=1,m
          do 100 i=1,n
           if (image(i,j).gt.image(mx(1),mx(2))) then
            mx(1)=i
            mx(2)=j
           end if
           if (image(i,j).lt.image(mn(1),mn(2))) then
            mn(1)=i
            mn(2)=j
           end if
100       continue
110      continue
        else
         do 200 j=1,m
          do 210 i=1,n
           if (image(i,j).gt.image(mx(1),mx(2))) then
            if (mask(i,j).eq.1) then
             mx(1)=i
             mx(2)=j
            end if
           end if
           if (image(i,j).lt.image(mn(1),mn(2))) then
            if (mask(i,j).eq.1) then
             mn(1)=i
             mn(2)=j
            end if
           end if
210       continue
200      continue
        end if

        mnv=image(mn(1),mn(2))
        mxv=image(mx(1),mx(2))

        return
        end


        subroutine callplotimage2(f1,n,m,std,av)
        implicit none
        integer n,m
        real*8 image(n,m),std,av
        character f1*(*),extn*20

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call plotimage2(image,n,m,n,m,std,av)

        return
        end

        subroutine sub_imagestats(f1,n,m)
        implicit none
        integer n,m
        real*8 array(n,m)
        character f1*(*),extn*20,ch1,strdev*5,lab*500,opt*4

        extn='.img'
        call readarray_bin(n,m,array,n,m,f1,extn)
344     continue
        write (*,*)
        write (*,'(a39,$)') '   Want statistics over a region (n) ? '
        read (*,'(a1)') ch1
        if (ichar(ch1).eq.32) ch1='n'
        if (ch1.ne.'n') then
         strdev='/xs'
         lab='Image'
         call pgbegin(0,strdev,1,1)
         call pgvport(0.1,0.9,0.1,0.9)
         call plotimage(array,n,m,n,m)
         call setgetoption(opt)
         if (opt.eq.'box') call boxstats(array,n,m,n,m)
         if (opt.eq.'area') call tvstat(array,n,m,n,m)
         call pgend
         goto 344
        end if
        
        return
        end
c!
c!
c!
c! do for 3d cubes. save stuff in a file to read later in QC

        subroutine get_imagestats3D(f1,kappa,n,m,l,blanky,blankv,
     /             scratch)
        implicit none
        integer k,nchar,n,m,l,mn(2),mx(2),dumi,round,mask(n,m),i,j
        integer mnd(2,l),mxd(2,l),mxc(2),mnc(2),blankn(l),npixc
        integer mxind,mnind
        real*8 std(l),av(l),mnv(l),mxv(l),kappa,stdclip(l),avclip(l)
        real*8 stdc,avc,minvc,maxvc,stdclipc,avclipc
        character f1*(*),ch1*1,strdev*5,lab*500,opt*4,extn*20,f2*500
        character scratch*500
        real*8 image(n,m),blankv,cube(n,m,l)
        logical blanky

        extn='.img'
        call readarray_bin3D(n,m,l,cube,n,m,l,f1,extn)

        do k=1,l
         do j=1,m
          do i=1,n
           image(i,j)=cube(i,j,k)
          end do
         end do 
         call get_im_agestats2D(kappa,n,m,std(k),av(k),stdclip(k),
     /     avclip(k),mx,mn,mxv(k),mnv(k),blanky,blankv,blankn(k),image)
         mxd(1,k)=mx(1)
         mxd(2,k)=mx(2)
         mnd(1,k)=mn(1)
         mnd(2,k)=mn(2)
        end do

c!      do max min
        maxvc=mxv(1)
        minvc=mnv(1)
        mxind=1
        mnind=1
        do k=2,l
         if (mxv(k).gt.maxvc) then
          maxvc=mxv(k)
          mxind=k
         end if
         if (mnv(k).lt.minvc) then
          minvc=mnv(k)
          mnind=k
         end if
        end do
        mxc(1)=mxd(1,mxind)
        mxc(2)=mxd(2,mxind)
        mnc(1)=mnd(1,mxind)
        mnc(2)=mnd(2,mxind)

c!      do av, sig
        avc=0.d0
        npixc=0
        do k=1,l
         avc=avc+av(k)*(n*m-blankn(k)) 
         npixc=npixc+(n*m-blankn(k))
        end do
        avc=avc/npixc

        stdc=0.d0
        do k=1,l
         stdc=stdc+(std(k)*std(k)+av(k)*av(k))*(n*m-blankn(k))
        end do
        stdc=sqrt(stdc/npixc-avc*avc)

c!      write out
        f2=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.qc_cc' 
        open(unit=21,file=f2,form='unformatted')
         write (21) n,m,l
         do k=1,l
          write (21) mxv(k),mxd(1,k),mxd(2,k),mnv(k),mnd(1,k),  ! for each channel
     /        mnd(2,k),blankn(k),std(k),av(k),stdclip(k),avclip(k)
         end do
        close(21)
        write (21) maxvc,mxc(1),mxc(2),minvc,mnc(1),mnc(2),  ! for entire image
     /        n*m*l-npixc,stdc,avc,stdc,avc
        
        return
        end


