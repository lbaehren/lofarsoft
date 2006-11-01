
        subroutine grey(array,x,y,i,j,str1,lab,hi,zero,cod1,zm,scratch)
        implicit none
        integer i,j,i1,i2,nchar,x,y,hi,dumi,zero,zm,round4,starc
        real*8 array(x,y),keyvalue
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 blc(2),trc(2),xcur,ycur,xcur1,ycur1,dum4
        real*4 xclk(6),yclk(6),xcur2,uph,lowh
        character str1*5,lab*500,ch1,cod1*2,srlname*500,scratch*500
        character chold*1,srldir*500,fg*500,extn*10,dir*500,keyword*500
        character comment*500

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/
        data xclk/0.1,0.25,0.4,0.55,0.7,0.85/
        data yclk/0.93,0.93,0.93,0.93,0.93,0.93/

        fg="paradefine"
        extn=""
        dir="./"
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir)

        starc=1
        chold='0'
        low=array(1,1)
        up=array(1,1)
        do 100 i1=1,i
         do 110 i2=1,j
          arr4(i1,i2)=array(i1,i2)
          if (low.gt.arr4(i1,i2)) low=arr4(i1,i2)
          if (up.lt.arr4(i1,i2)) up=arr4(i1,i2)
110      continue
100     continue
c        low=low-0.1*abs(up-low)  ! so lowest signal is seeable
        if (zero.eq.1) then
         if (abs(up+1.0-low).gt.50.0) then   ! then convert to LG else keep LN
          call arr2dnz(array,x,y,i,j,low,up)
          call converttolog(arr4,i,j,low,up)
         end if
        end if

        lowh=low
        uph=up
        if (hi.eq.1) then
         dumi=i*j
c         call pgbegin(0,'/NULL',1,1)
c         call pghist(dumi,arr4,lowh,uph,100,0)
c         call pgqwin(mn1,mx1,mn2,mx2)
c         call pgend
        end if

333     continue
        blc(1)=1.0
        blc(2)=1.0
        trc(1)=i*1.0
        trc(2)=j*1.0
444     continue
c        call pgbegin(0,str1(1:nchar(str1)),1,1)
        if (hi.eq.1) dum4=0.3
        if (hi.eq.0) dum4=0.1
c        call pgvport(0.1,0.9,dum4,0.9)
c        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
c        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
c        call pggray(arr4,i,j,1,i,1,j,up,low,tr)
c        call pglabel(' ',' ',lab(1:nchar(lab)))
c        call pgwedg('RG',1.5,3.0,up,low,' ')

        if (hi.eq.1) then
c         call pgvport(0.1,0.9,0.1,0.3)
         if (cod1.eq.'hn') then
c          call pgwindow(mn1,mx1,mn2,mx2)
c          call pgbox('BNST',0.0,0,'BCVNST',0.0,0)
c          call pghist(i*j,arr4,lowh,uph,100,1)
         end if
         if (cod1.eq.'hg') then
          mn2=0.0
          mx2=log10(mx2)
c          call pgwindow(mn1,mx1,mn2,mx2)
c          call pgbox('BNST',0.0,0,'BCVNSTL',0.0,0)
          call plotloghist(arr4,i,j,mn1,mx1,10)
         end if
        end if

        if (zm.eq.0) goto 555
234     continue
c        call pgvport(0.1,1.0,0.1,1.0)
c        call pgwindow(0.0,1.0,0.0,1.0)
c        call pgtext(0.06,0.97,'Zoom in    zoom out       histo'//
 
        call clickopn(xclk,yclk,6,6,0.015,ch1)
        if (ch1.eq.'1') then
c         call pgvport(0.1,0.9,dum4,0.9)
c         call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
c         call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
c         call pgcurs(xcur,ycur,ch1)
c         call pgband(2,1,xcur,ycur,xcur1,ycur1,ch1)
         call setzoom(xcur,ycur,xcur1,ycur1,blc,trc,i,j)
         chold=ch1
         goto 444
        end if
        if (ch1.eq.'2') then
         chold=ch1
         goto 333
        end if
        if (ch1.eq.'3') then
c         call pgvport(0.1,0.9,0.1,0.3)
c         call pgwindow(mn1,mx1,mn2,mx2)
c         call pgband(6,1,mn1,mn2,xcur1,ycur1,ch1)
c         call pgmove(xcur1,mn2)
c         call pgdraw(xcur1,mx2)
c         call pgband(6,1,mn1,mn2,xcur2,ycur1,ch1)
c         call pgmove(xcur2,mn2)
c         call pgdraw(xcur2,mx2)
         low=min(xcur1,xcur2)
         up=max(xcur1,xcur2)
         chold=ch1
         goto 444
        end if
        if (ch1.eq.'4') then
c         call pgvport(0.1,0.9,dum4,0.9)
c         call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
c         call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
c         call pgcurs(xcur,ycur,ch1)
c         call pgband(2,1,xcur,ycur,xcur1,ycur1,ch1)
         call setzoom(xcur,ycur,xcur1,ycur1,blc,trc,i,j)
         call minmaxarr4(arr4,i,j,round4(blc(1)),round4(blc(2)),
     /        round4(trc(1)),round4(trc(2)),low,up)
         chold=ch1
         goto 333
        end if
        if (ch1.eq.'5') then
334      write (*,'(a,$)') '   Enter pixrange : '
         read (*,*) low,up
         if (low.eq.up) goto 334
         low=min(low,up)
         up=max(low,up)
         chold=ch1
         goto 444
        end if
        if (ch1.eq.'6') then
         write (*,'(a,$)') '   Sourcelistname : ' 
         read (*,*) srlname
c         call pgvport(0.1,0.9,dum4,0.9)
c         call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
c         call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
         if (chold.ne.ch1) starc=1 
         starc=starc+1
         call tvstar(srlname,srldir,starc,scratch)
         chold=ch1
         goto 234
        end if
555     continue

c        call pgend

        return
        end

        
c! d1 if 0 and not 1 => zero is magic value, exclude from hist and image
c! d2=0, plot as is. d2=1 ==> do 5-sigma clip and set low and up as -5,5 and no log
        subroutine grey2(array,arr2,x,y,i,j,str1,lab,lab2,d1,d2)
        implicit none
        integer i,j,i1,i2,nchar,x,y,dumi,d1,d2
        real*8 array(x,y),arr2(x,y),std,av
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 mn12,mx12,mn22,mx22
        real*4 arr42(i,j),low2,up2
        character str1*5,lab*500,lab1*500,lab2*500

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/
        std=0.d0
        av=0.d0

        do 100 i1=1,i
         do 110 i2=1,j
          arr4(i1,i2)=array(i1,i2)
          arr42(i1,i2)=arr2(i1,i2)
110      continue
100     continue

        if (d1.eq.1) then
         call range2d(arr4,i,j,i,j,low,up)
         call range2d(arr42,i,j,i,j,low2,up2)
        else
         call arr2dnz(array,x,y,i,j,low,up)
         call arr2dnz(arr2,x,y,i,j,low2,up2)
         if (d2.eq.0) then

          if (up/low.gt.10.0) then   ! then convert to LG else keep LN
           call converttolog(arr4,i,j,low,up)
           call converttolog(arr42,i,j,low2,up2)
          end if
         else     ! if d2=0
          i1=i*j
          i2=3
          call sigclip(array,x,y,1,1,i,j,std,av,i2)
          low=max(av-10.0*std,low) 
c          up=av+5.0*std 
          if (up2/low2.gt.50.0) then   ! then convert to LG else keep LN
           call converttolog(arr42,i,j,low2,up2)
          end if
         end if   ! if d2=0
c         up=up*1.1
c         up2=up2*1.1
        end if

        lab1=lab(1:nchar(lab))
        lab2=lab2(1:nchar(lab2))

        dumi=i*j
c        call pgbegin(0,'/NULL',1,1)
c        call pghist(dumi,arr4,low,up,100,0)
c        call pgqwin(mn1,mx1,mn2,mx2)
c        call pghist(dumi,arr42,low2,up2,100,0)
c        call pgqwin(mn12,mx12,mn22,mx22)
c        call pgend

c        call pgbegin(0,str1(1:nchar(str1)),1,1)
c        call pgvport(0.12,0.52,0.45,0.9)
c        call pgwindow(0.5,i*1.0+0.5,0.5,j*1.0+0.5)
c        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
c        call pggray(arr4,i,j,1,i,1,j,up,low,tr)
c        call pglabel(' ',' ',lab1)

c        call pgvport(0.58,0.98,0.45,0.9)
c        call pgwindow(0.5,i*1.0+0.5,0.5,j*1.0+0.5)
c        call pgbox('BCNIST',0.0,0,'BCNIST',0.0,0)
c        call pggray(arr42,i,j,1,i,1,j,up2*0.5,low2,tr)
c        call pglabel(' ',' ',lab2)

c        call pgvport(0.12,0.52,0.1,0.4)
c        call pgwindow(mn1,mx1,mn2,mx2)
c        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
c        call pghist(i*j,arr4,low,up,100,1)
c        call pgwedg('BG',1.5,3.0,up,low,' ')

c        call pgvport(0.58,0.98,0.1,0.4)
c        call pgwindow(mn12,mx12,mn22,mx22)
c        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
c        call pghist(i*j,arr42,low2,up2,100,1)
c        call pgwedg('BG',1.5,3.0,up2*0.5,low2,' ')

c        call pgend

        return
        end

c! plots curves

        subroutine plotcurves(nc,num,nbin,x,y1,y2,y3,xl,yl1,yl2,
     /             yl3,code)
        implicit none
        integer nc,num,nbin
        real*8 x(num),y1(num),y2(num),y3(num)
        character xl*30,yl1*30,yl2*30,yl3*30,code*6
        real*4 x4(nbin),y41(nbin),y42(nbin),y43(nbin)
        real*4 mnx,mxx,mny,mxy,d41,d42

        call vec8to4(x,num,x4,nbin)
        call range2(x4,nbin,nbin,mnx,mxx)

        call vec8to4(y1,num,y41,nbin)
        call range2(y41,nbin,nbin,mny,mxy)
        call vec8to4(y2,num,y42,nbin)
        call range2(y42,nbin,nbin,d41,d42)
        mny=min(mny,d41)
        mxy=max(mxy,d42)
        call vec8to4(y3,num,y43,nbin)
        call range2(y43,nbin,nbin,d41,d42)
        mny=min(mny,d41)
        mxy=max(mxy,d42)
                
c        call pgbegin(0,'/xs',1,1)
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNST',0.0,0,'BNST',0.0,0)
c        call pgvport(0.1,0.9,0.1,0.9)
c        call pglabel(xl,yl1,' ')
        if (nc.ge.1) then
c         if (code(1:1).eq.'L') call pgline(nbin,x4,y41)
c         if (code(2:2).eq.'P') call pgpoint(nbin,x4,y41,7)
        end if
        if (nc.ge.2) then
         call vec8to4(y2,num,y42,nbin)
         call range2(y42,nbin,nbin,d41,d42)
c         call pgwindow(mnx,mxx,d41,d42)
c         call pgbox('',0.0,0,'CMST',0.0,0)
c         call pgptxt(mxx+0.08*(mxx-mnx),0.5*(d42+d41),90.0,0.5,yl2)
c         call pgsls(2)
c         if (code(3:3).eq.'L') call pgline(nbin,x4,y42)
c         if (code(4:4).eq.'P') call pgpoint(nbin,x4,y42,7)
        end if
        if (nc.eq.3) then
c         call pgsls(3)
c         if (code(5:5).eq.'L') call pgline(nbin,x4,y43)
c         if (code(6:6).eq.'P') call pgpoint(nbin,x4,y43,7)
        end if
c        call pgend
        
        return
        end


        subroutine clickop(x1,y1,x2,y2,rad,ch1)
        implicit none
        real*4 x1,x2,y1,y2,rad,x,y,dum41,dum42
        character ch1*1,ch

c        call pgsfs(2)
c        call pgcirc(x1,y1,rad)
c        call pgcirc(x2,y2,rad)
c        call pgsfs(1)
        x=x2
        y=y2
c        call pgcurs(x,y,ch)      
        dum41=(x-x1)**2.0+(y-y1)**2.0
        dum42=(x-x2)**2.0+(y-y2)**2.0
        if (dum41.le.rad*rad) then 
         ch1='y'
c         call pgcirc(x1,y1,rad)
        else
         if (dum42.le.rad*rad) then
          ch1='n'
c          call pgcirc(x2,y2,rad)
         else
          ch1='x'
         end if
        end if

        return
        end


        subroutine clickopn(xa,ya,n,nmax,rad,ch1)
        implicit none
        integer n,nmax,i
        real*4 xa(nmax),ya(nmax),rad,x,y,dum4
        character ch1*1,ch,getchar*1

        if (n.gt.nmax.or.nmax.gt.9) write (*,*) ' ### ERROR !!!'
c        call pgsfs(2)
        do i=1,n
c         call pgcirc(xa(i),ya(i),rad)
        end do
c        call pgsfs(1)

        x=xa(1)
        y=ya(1)
c        call pgcurs(x,y,ch)      

        ch1='0'
        do i=1,n
         dum4=(x-xa(i))**2.0+(y-ya(i))**2.0
         if (dum4.le.rad*rad) then 
          ch1=getchar(i)
c          call pgcirc(xa(i),ya(i),rad)
         end if
        end do

        return
        end

        subroutine plotloghist(arr4,n,m,mn1,mx1,nbin)
        implicit none
        integer i,j,nbin,n,m,i1
        real*4 arr4(n,m),mn1,mx1,low(nbin),high(nbin),mid(nbin)
        real*4 delta,histo(nbin)

        delta=(mx1-mn1)/(nbin-1)
        do 140 i=1,nbin
         low(i)=mn1+(i-1)*delta
         high(i)=low(i)+delta
         mid(i)=low(i)+0.50*delta
         histo(i)=0.0
140     continue

        do 120 i=1,n
         do 130 j=1,m
          i1=int((arr4(i,j)-mn1)/delta)+1
          histo(i1)=histo(i1)+1.0
130      continue
120     continue

        do i=1,nbin
         if (histo(i).ne.0) then
          histo(i)=log10(histo(i))
         else
          histo(i)=0.0
         end if
        end do

c        call pgbin(nbin,mid,histo,.true.)

        do i=1,nbin-1
c         call pgmove(high(i),0.0)
c         call pgdraw(high(i),histo(i))
        end do

        return
        end


        subroutine pgdrawellipse(x,y,bmaj,bmin,bpa,starc)
        implicit none
        include "constants.inc"
        real*4 x,y,bmaj,bmin,bpa,x1,x2(38),y1,y2(38),a,b,ang
        integer i,s,starc

        a=0.50*bmaj
        b=0.50*bmin
        s=0
        ang=bpa+90.0
        do i=0,370,10
         s=s+1
         x1=a*cos(i/rad)
         y1=b*sin(i/rad)
         x2(s)=x1*cos(ang/rad)-y1*sin(ang/rad)+x
         y2(s)=x1*sin(ang/rad)+y1*cos(ang/rad)+y
        end do
c        call pgsci(starc)
c        call pgline(38,x2,y2)
c        call pgsci(1)

        return
        end

c! does what tvstar does in aips
        subroutine tvstar(f1,srldir,starc,scratch)
        implicit none
        integer n,m,nchar,nsrc,nffmt,nisl,starc,gpi
        character f1*500,fi*500,ffmt*500,srldir*500
        real*8 cdelt(3)
        character extn*10,keyword*500,scratch*500
        character comment*500,keystrng*500

        call sourcelistheaders(f1,fi,n,m,nisl,nsrc,gpi,nffmt,ffmt,
     /       srldir)
        extn='.header'
        keyword='CDELT1'
        call get_keyword(fi,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(fi,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        call sub_tvstar(f1,fi,n,m,nisl,nsrc,nffmt,ffmt,starc,
     /       srldir,cdelt)
        
        return
        end

        subroutine sub_tvstar(f1,fi,n,m,nisl,nsrc,nffmt,ffmt,
     /             starc,srldir,cdelt)
        implicit none
        integer n,m,nchar,nsrc,nffmt,nisl,isrc,i,starc,flag(nsrc)
        character f1*500,fi*500,ffmt*500,srldir*500
        real*8 xsrc(nsrc),ysrc(nsrc),cdelt(3)
        real*8 bmaj(nsrc),bmin(nsrc),bpa(nsrc)
        real*4 x4(nsrc),y4(nsrc),bmaj4(nsrc),bmin4(nsrc),bpa4(nsrc)
        
        call readgaul_getposn(f1,nsrc,xsrc,ysrc,flag,bmaj,bmin,
     /       bpa,srldir)
        call vec8to4(xsrc,nsrc,x4,nsrc)
        call vec8to4(ysrc,nsrc,y4,nsrc)
        call vec8to4(bmaj,nsrc,bmaj4,nsrc)
        call vec8to4(bmin,nsrc,bmin4,nsrc)
        call vec8to4(bpa,nsrc,bpa4,nsrc)
c        call pgsci(starc)
cc        call pgpoint(nsrc,x4,y4,2) 
c        call pgsci(1)
        do i=1,nsrc  ! plot fwhm ellipse
         if (flag(i).eq.0) then
          bmaj4(i)=bmaj4(i)/(cdelt(1)*3600.d0) * 3.d0
          bmin4(i)=bmin4(i)/(cdelt(2)*3600.d0) * 3.d0
          if (x4(i).gt.800.0.and.x4(i).lt.840.0.and.y4(i).gt.1050.0. 
     /          and.y4(i).lt.1200.0) then
          end if
c          call pgdrawellipse(x4(i),y4(i),bmaj4(i),bmin4(i),bpa4(i),
 
         end if
        end do
        
        return
        end


        subroutine gethistpara(nsrc,x,plotx,minx,maxx,mn1,mx1)
        implicit none
        integer nsrc,id1,pgopen,x
        real*4 plotx(x),minx,maxx,mn1,mx1

c        call pgbegin(0,'/NULL',1,1)
c        call pgsci(0)
c        call pghist(nsrc,plotx,minx,maxx,100,0)
c        call pgqwin(minx,maxx,mn1,mx1)
c        call pgsci(1)
c        call pgend

        return
        end

