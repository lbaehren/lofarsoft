
        subroutine grey(array,x,y,i,j,str1,lab,hi,zero,cod1,zm,
     /             scratch,f1,srldir)
        implicit none
        integer i,j,i1,i2,nchar,x,y,hi,dumi,zero,zm,round4,starc
        integer error,linec,nopts,pgopen,id1,id2,nmaxopts
        integer blackm,blacki   ! image and menu r diff devices so colourmaps are diff
        integer nffmt,gpi,nisl,nsrc,n,m,hiold
        PARAMETER (nmaxopts=16)
        real*8 array(x,y),keyvalue
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2,vport(4)
        real*4 blc(2),trc(2),xcur,ycur,xcur1,ycur1,dum4
        real*4 xcur2,uph,lowh,d1,d2,d3,low1
        character str1*5,lab*500,ch1,cod1*2,srlname*500,scratch*500,ch11
        character chold*1,srldir*500,fg*500,extn*20,dir*500,keyword*500
        character comment*500,opts(nmaxopts)*100,f1*500,f2*500,cmd*1000
        character cs*20,fi*500,ffmt*500,code2*2
        character starname*500,solnname*500,dums*500
        logical exists,colour,logged

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/
        data opts/'Zoom in','Zoom out','Greyscale','Colour',
     /   'Imagescale - histogram',
     /   'Imagescale - area','Imagescale - Pixrange','Get pixel',   ! 8
     /   'Toggle log/linear','Plot gaussians','Name stars',
     /   'Plot islands','Add coordinate grid',
     /   'Plots associatelist','Plots stars','Quit'/

        call setup_grey(starc,linec,chold,array,x,y,i,j,low,up,arr4,
     /       zero,lowh,uph,hi,mn1,mx1,mn2,mx2,low1,logged)

        id1=pgopen(str1(1:nchar(str1)))  ! menu
        if (id1.le.0) stop
        call pgslct(id1)
        call pgpap(4.5,1.0)
        call pgpage
        call pgask(.false.)
        blackm=0

        id2=pgopen(str1(1:nchar(str1)))   ! image
        colour=.false.                    ! start with greyscale
        if (id2.le.0) stop
        call pgslct(id2)
        call pgpap(9.0,1.0)
        call pgpage
        call pgask(.false.)
        starc=1 
        cs='grey'
        code2='u '

333     continue
        blc(1)=1.0
        blc(2)=1.0
        trc(1)=i*1.0
        trc(2)=j*1.0
444     continue

        if (hi.eq.1) dum4=0.3
        if (hi.eq.0) dum4=0.1
        call pgslct(id2)
        call pgpage
        call sub_grey_drawimage(id2,hi,blc,trc,arr4,i,j,up,low,tr,lab,
     /    cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,cs,str1,blacki,logged)
        call pgsci(blacki+1)
        vport(1)=0.1
        vport(2)=0.9
        vport(3)=0.1
        vport(4)=0.9

        if (zm.eq.0) goto 555
234     continue
        call pgslct(id1)
        call pgvport(0.0,1.0,0.0,1.0)
        call clickopn(16,nmaxopts,0.03,ch1,opts,blackm)
        call pgslct(id2)

        if (ch1.eq.'1') then                    !! Zoom in
         vport(3)=dum4
         call pgsub_op1(vport,blc,trc,i,j,ch1,chold)
         goto 444
        end if
        if (ch1.eq.'2') then                    !! Zoom out
         call pgsub_op2(ch1,chold)
         goto 333
        end if
        if (ch1.eq.'3') then                    !! Greyscale
         call pgsub_op3(colour,cs,ch1,chold)
         goto 333
        end if
        if (ch1.eq.'4') then                    !! Colour
         call pgsub_op4(colour,cs,str1,blacki,ch1,chold)
         goto 333
        end if
        if (ch1.eq.'5') then                    !! Imagescale - histogram
         call pgslct(id2)
         call pgpage
         hiold=hi
         hi=1
         dum4=0.3
         call sub_grey_drawimage(id2,hi,blc,trc,arr4,i,j,up,low,tr,lab,
     /    cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,cs,str1,blacki,logged)
         call pgsub_op5(mn1,mx1,mn2,mx2,low,up,ch1,chold)
         hi=hiold
         goto 444
        end if
        if (ch1.eq.'6') then                    !! Imagescale - area
         call pgsub_op6(dum4,blc,trc,i,j,arr4,low,up,ch1,chold)
         goto 333
        end if
        if (ch1.eq.'7') then                    !! Imagescale - pixrange
         call pgsub_op7(low,up,ch1,chold,logged,low1)
         goto 444
        end if
        if (ch1.eq.'8') then                    !! Get pixel
         call pgsub_opE(dum4,blc,trc,ch1,chold,array,x,y)
         goto 333
        end if
        if (ch1.eq.'9') then                    !! Toggle linear/log
         call pgsub_opF(ch1,chold,logged,array,x,y,arr4,i,j,low,up,low1)
         goto 333
        end if
        if (ch1.eq.'A') then                    !! Plot gaussians
         call pgsub_op8(srldir,dum4,blc,trc,ch1,chold,
     /        cs,starc,linec,blacki,scratch,dums,code2)
         goto 234
        end if
        if (ch1.eq.'B') then                    !! Name stars
         call pgsub_op9(srldir,dum4,blc,trc,blacki,scratch,ch1,chold)
         goto 234
        end if
        if (ch1.eq.'C') then                    !! Plot islands
         call pgsub_opA(scratch,dum4,blc,trc,starc,linec,ch1,chold,
     /                  blacki,srldir)
         goto 234
        end if
        if (ch1.eq.'D') then                    !! Plot coordinate grid
         call pgsub_opB(f1,scratch,dum4,blc,trc,ch1,chold)
         goto 234
        end if
        if (ch1.eq.'E') then                    !! Plot associatelist
         call pgsub_opC(f1,scratch,dum4,blc,trc,starc,id1,id2,str1,hi,
     /   arr4,i,j,up,low,tr,lab,cod1,mn1,mx1,mn2,mx2,lowh,uph,cs,blacki)
         goto 234
        end if
        if (ch1.eq.'F') then                    !! Plot gaussians
         call pgsub_opD(srldir,dum4,blc,trc,ch1,chold,
     /        cs,starc,linec,blacki,scratch,dums,code2)
         goto 234
        end if
        if (ch1.eq.'G') then                    !! Quit
        end if
555     continue

        call pgslct(id1)
        call pgclos
        call pgslct(id2)
        call pgclos

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
        real*4 arr42(i,j),low2,up2,low1
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
           call converttolog(arr4,i,j,low,up,low1)
           call converttolog(arr42,i,j,low2,up2,low1)
          end if
         else     ! if d2=0
          i1=i*j
          i2=3
          call sigclip(array,x,y,1,1,i,j,std,av,i2*1.d0)
          low=max(av-10.0*std,low) 
c          up=av+5.0*std 
          if (up2/low2.gt.50.0) then   ! then convert to LG else keep LN
           call converttolog(arr42,i,j,low2,up2,low1)
          end if
         end if   ! if d2=0
c         up=up*1.1
c         up2=up2*1.1
        end if

        lab1=lab(1:nchar(lab))
        lab2=lab2(1:nchar(lab2))

        dumi=i*j
        call pgbegin(0,'/NULL',1,1)
        call pghist(dumi,arr4,low,up,100,0)
        call pgqwin(mn1,mx1,mn2,mx2)
        call pghist(dumi,arr42,low2,up2,100,0)
        call pgqwin(mn12,mx12,mn22,mx22)
        call pgend

        call pgbegin(0,str1(1:nchar(str1)),1,1)
        call pgvport(0.12,0.52,0.45,0.9)
        call pgwindow(0.5,i*1.0+0.5,0.5,j*1.0+0.5)
        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
        call pggray(arr4,i,j,1,i,1,j,up,low,tr)
        call pglabel(' ',' ',lab1)

        call pgvport(0.58,0.98,0.45,0.9)
        call pgwindow(0.5,i*1.0+0.5,0.5,j*1.0+0.5)
        call pgbox('BCNIST',0.0,0,'BCNIST',0.0,0)
        call pggray(arr42,i,j,1,i,1,j,up2*0.5,low2,tr)
        call pglabel(' ',' ',lab2)

        call pgvport(0.12,0.52,0.1,0.4)
        call pgwindow(mn1,mx1,mn2,mx2)
        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
        call pghist(i*j,arr4,low,up,100,1)
        call pgwedg('BG',1.5,3.0,up,low,' ')

        call pgvport(0.58,0.98,0.1,0.4)
        call pgwindow(mn12,mx12,mn22,mx22)
        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
        call pghist(i*j,arr42,low2,up2,100,1)
        call pgwedg('BG',1.5,3.0,up2*0.5,low2,' ')

        call pgend

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
                
        call pgbegin(0,'/xs',1,1)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox('BCNST',0.0,0,'BNST',0.0,0)
        call pgvport(0.1,0.9,0.1,0.9)
        call pglabel(xl,yl1,' ')
        if (nc.ge.1) then
         if (code(1:1).eq.'L') call pgline(nbin,x4,y41)
         if (code(2:2).eq.'P') call pgpoint(nbin,x4,y41,7)
        end if
        if (nc.ge.2) then
         call vec8to4(y2,num,y42,nbin)
         call range2(y42,nbin,nbin,d41,d42)
         call pgwindow(mnx,mxx,d41,d42)
         call pgbox('',0.0,0,'CMST',0.0,0)
         call pgptxt(mxx+0.08*(mxx-mnx),0.5*(d42+d41),90.0,0.5,yl2)
         call pgsls(2)
         if (code(3:3).eq.'L') call pgline(nbin,x4,y42)
         if (code(4:4).eq.'P') call pgpoint(nbin,x4,y42,7)
        end if
        if (nc.eq.3) then
         call pgsls(3)
         if (code(5:5).eq.'L') call pgline(nbin,x4,y43)
         if (code(6:6).eq.'P') call pgpoint(nbin,x4,y43,7)
        end if
        call pgend
        
        return
        end


        subroutine clickop(x1,y1,x2,y2,rad,ch1)
        implicit none
        real*4 x1,x2,y1,y2,rad,x,y,dum41,dum42
        character ch1*1,ch

        call pgsfs(2)
        call pgcirc(x1,y1,rad)
        call pgcirc(x2,y2,rad)
        call pgsfs(1)
        x=x2
        y=y2
        call pgcurs(x,y,ch)      
        dum41=(x-x1)**2.0+(y-y1)**2.0
        dum42=(x-x2)**2.0+(y-y2)**2.0
        if (dum41.le.rad*rad) then 
         ch1='y'
         call pgcirc(x1,y1,rad)
        else
         if (dum42.le.rad*rad) then
          ch1='n'
          call pgcirc(x2,y2,rad)
         else
          ch1='x'
         end if
        end if

        return
        end


        subroutine clickopn(n,nmax,rad,ch1,opts,blackm)
        implicit none
        integer n,nmax,i,nchar,blackm
        real*4 xa(nmax),ya(nmax),rad,x,y,dum4,d1,d2,d3
        character ch1*1,ch,getchar*1,opts(nmax)*100

        call pgpage
        call pgwindow(0.0,1.0,0.0,1.0)
        call pgsch(1.8)
        call pgsci(blackm+8)
        do i=1,n
         xa(i)=0.1
         ya(i)=0.95-(i-1.0)/(n+1.0)
        end do

        if (n.gt.nmax.or.nmax.gt.16) write (*,*) ' ### ERROR !!!'
        call pgsfs(2)
        do i=1,n
         call pgcirc(xa(i),ya(i),rad)
         call pgtext(xa(i)+0.1,ya(i),opts(i)(1:nchar(opts(i))))
        end do
        call pgsfs(1)
        call pgsch(1.0)

        x=xa(1)
        y=ya(1)
        call pgcurs(x,y,ch)      

        ch1='0'
        do i=1,n
         dum4=(x-xa(i))**2.0+(y-ya(i))**2.0
         if (dum4.le.rad*rad) then 
          ch1=getchar(i)
          if (i.eq.10) ch1='A'
          if (i.eq.11) ch1='B'
          if (i.eq.12) ch1='C'
          if (i.eq.13) ch1='D'
          if (i.eq.14) ch1='E'
          if (i.eq.15) ch1='F'
          if (i.eq.16) ch1='G'
          call pgcirc(xa(i),ya(i),rad)
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

        call pgbin(nbin,mid,histo,.true.)

        do i=1,nbin-1
         call pgmove(high(i),0.0)
         call pgdraw(high(i),histo(i))
        end do

        return
        end


        subroutine pgdrawellipse(x,y,bmaj,bmin,bpa,starc,linec,blacki)
        implicit none
        include "constants.inc"
        real*4 x,y,bmaj,bmin,bpa,x1,x2(38),y1,y2(38),a,b,ang,d1,d2,d3
        integer i,s,starc,linec,blacki

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
        call pgsci(blacki+starc)
        call pgslw(2)
c        call pgsls(linec)
        call pgline(38,x2,y2)
        call pgsci(blacki+1)
        call pgslw(1)
        call pgsls(1)

        return
        end

c! does what tvstar does in aips
        subroutine tvstar(f1,srldir,starc,linec,scratch,blacki)
        implicit none
        integer n,m,nchar,nsrc,nffmt,nisl,starc,gpi
        integer error,linec,blacki
        character f1*500,fi*500,ffmt*500,srldir*500
        real*8 cdelt(3)
        character extn*20,keyword*500,scratch*500
        character comment*500,keystrng*500

        call sourcelistheaders(f1,fi,n,m,nisl,nsrc,gpi,nffmt,ffmt,
     /       srldir)
        extn='.header'
        keyword='CDELT1'
        call get_keyword(fi,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(fi,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)
        call sub_tvstar(f1,fi,n,m,nisl,nsrc,nffmt,ffmt,starc,
     /       linec,srldir,cdelt,blacki)
        
        return
        end

        subroutine sub_tvstar(f1,fi,n,m,nisl,nsrc,nffmt,ffmt,
     /             starc,linec,srldir,cdelt,blacki)
        implicit none
        integer n,m,nchar,nsrc,nffmt,nisl,isrc,i,starc,flag(nsrc),linec
        integer blacki
        character f1*500,fi*500,ffmt*500,srldir*500
        real*8 xsrc(nsrc),ysrc(nsrc),cdelt(3)
        real*8 bmaj(nsrc),bmin(nsrc),bpa(nsrc),d2,d1,d3
        real*4 x4(nsrc),y4(nsrc),bmaj4(nsrc),bmin4(nsrc),bpa4(nsrc)
        
        call readgaul_getposn(f1,nsrc,xsrc,ysrc,flag,bmaj,bmin,
     /       bpa,srldir)
        call vec8to4(xsrc,nsrc,x4,nsrc)
        call vec8to4(ysrc,nsrc,y4,nsrc)
        call vec8to4(bmaj,nsrc,bmaj4,nsrc)
        call vec8to4(bmin,nsrc,bmin4,nsrc)
        call vec8to4(bpa,nsrc,bpa4,nsrc)
        do i=1,nsrc  ! plot fwhm ellipse
         if (flag(i).eq.0) then
          bmaj4(i)=bmaj4(i)/(cdelt(1)*3600.d0)
          bmin4(i)=bmin4(i)/(cdelt(2)*3600.d0)
          call pgdrawellipse(x4(i),y4(i),bmaj4(i)*1.0,bmin4(i)*1.0,
     /         bpa4(i),starc,linec,blacki)
         end if
        end do
        
        return
        end


        subroutine gethistpara(nsrc,x,plotx,minx,maxx,mn1,mx1,blacki)
        implicit none
        integer nsrc,id1,pgopen,x,blacki
        real*4 plotx(x),minx,maxx,mn1,mx1

        call pgbegin(0,'/NULL',1,1)
        call pgsci(blacki)
        call pghist(nsrc,plotx,minx,maxx,100,0)
        call pgqwin(minx,maxx,mn1,mx1)
        call pgsci(blacki+1)
        call pgend

        return
        end

c! just draw the image and histo

        subroutine sub_grey_drawimage(id2,hi,blc,trc,arr4,i,j,up,low,tr,
     /       lab,cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,colourscheme,
     /       str1,blacki,logged)
        implicit none
        integer i,j,hi,dumi,nchar,clr1,clr2,id2,blacki,di1,di2
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2,l1,l2
        real*4 blc(2),trc(2),dum4,uph,lowh,d1,d2,d3
        character lab*500,cod1*2,colourscheme*20,str1*5
        logical logged

        call pgslct(id2)
        clr1=1
        if (colourscheme.eq.'grey') then
         clr2=99   
         blacki=0
        else
         clr2=85   ! so for colour leave some for normal colours. 
         blacki=clr2+1
        end if
        call set_colourmap(colourscheme,clr1,clr2)    ! sets colourmap

        call pgvport(0.1,0.9,dum4,0.9)
        call pgsci(blacki+1)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)

        if (colourscheme.eq.'grey') then
         call pggray(arr4,i,j,1,i,1,j,up,low,tr)
         call pgwedg('RG',1.5,3.0,up,low,' ')
        else
         call pgimag(arr4,i,j,1,i,1,j,low,up,tr)
         call pgwedg('RI',1.5,3.0,low,up,' ')
        end if
        call pglabel(' ',' ',lab(1:nchar(lab)))
        call set_colourmap(colourscheme,clr1,clr2)    ! sets colourmap

        if (hi.eq.1) then
         call pgvport(0.1,0.9,0.1,0.3)
         if (cod1.eq.'hn') then
          call pgwindow(mn1,mx1,mn2,mx2)
          call pgbox('BNST',0.0,0,'BCVNST',0.0,0)
          call pghist(i*j,arr4,lowh,uph,100,1)
         end if
         if (cod1.eq.'hg') then
          mn2=0.0
          mx2=log10(mx2)
          call pgwindow(mn1,mx1,mn2,mx2)
          call pgbox('BNST',0.0,0,'BCVNSTL',0.0,0)
          call plotloghist(arr4,i,j,mn1,mx1,10)
         end if
        end if

        return
        end

        subroutine plot_islnumbers(srldir,srlname,starc,linec,scratch,
     /             blacki)
        implicit none
        integer n,m,nchar,nisl,blacki,dumi,starc,linec
        character srlname*500,scratch*500,srldir*500
        character f1*500

        f1=scratch(1:nchar(scratch))//srlname(1:nchar(srlname))//
     /     '.islandlist'
        open(unit=21,file=f1,form='unformatted')
         read (21) n,m,nisl,dumi
        close(21)
        call sub_plot_islnumbers(srldir,srlname,scratch,nisl,n,m,blacki,
     /       starc,linec)

        return
        end

        subroutine sub_plot_islnumbers(srldir,srlname,scratch,nisl,
     /             n,m,blacki,starc,linec)
        implicit none
        integer nisl,iisl,nc,n,m,mask(n,m),i,j,starc,linec
        integer xic(nisl),yic(nisl),blacki
        character srlname*500,scratch*500,srldir*500
        character f1*500,ffmt*500,str6*6

        call readgaul_getisl(srlname,nisl,mask,n,m)
        call pgsci(blacki+starc)
        do i=1,n
         do j=1,m
          if (mask(i,j).eq.1) call pgpoint(1,i*1.0,j*1.0,20)
         end do
        end do

        call readisllist_getisln(srlname,nisl,xic,yic,scratch)
        call pgsci(blacki+3)
        do iisl=1,nisl
         call pgnumb(iisl,0,1,str6,nc)
         call pgtext(xic(iisl)*1.0-0.5,yic(iisl)*1.0-0.5,str6)
        end do
        call pgsci(blacki+1)

        return
        end
c!
c!
c!
        subroutine get_colourscheme(colourscheme,str1,blacki)
        implicit none
        character colourscheme*20,str1*5
        integer id3,pgopen,nchar,nopts,nmaxopts,blacki
        PARAMETER (nmaxopts=9)
        character ch1*1,opts(nmaxopts)*100

        data opts/'Heat','Cool','Rainbow','Sls','Sauron',
     /            ' ',' ',' ',' '/

        id3=pgopen(str1(1:nchar(str1)))  ! menu
        if (id3.le.0) stop
        call pgslct(id3)
        call pgpap(2.5,1.0)
        call pgpage
        call pgask(.false.)
345     continue  
        call pgvport(0.0,1.0,0.0,1.0)
        call clickopn(5,nmaxopts,0.04,ch1,opts,blacki)
        call pgeras
        if (ch1.eq.'1') colourscheme='heat'
        if (ch1.eq.'2') colourscheme='cool'
        if (ch1.eq.'3') colourscheme='rainbow'
        if (ch1.eq.'4') colourscheme='sls'
        if (ch1.eq.'5') colourscheme='sauron'
        call pgclos

        return
        end
c!
c!
c!
        subroutine namestars(starname,solnname,srldir,blacki,scratch,
     /             nsrc,ngau)
        implicit none
        integer blacki,ngau,i,flag,nchar,dumi,nsrc
        character starname*500,solnname*500,scratch*500,srldir*500
        character f1*500,f2*500,nam*30
        real*8 dumr4(4),dumr,x,y,dec
        real*4 x4,y4,dum4

        call pgsci(blacki+1)
        f1=srldir(1:nchar(srldir))//starname(1:nchar(starname))//
     /     '.'//solnname(1:nchar(solnname))//'.gaul.bin'
        f2=srldir(1:nchar(srldir))//starname(1:nchar(starname))//
     /     '.'//solnname(1:nchar(solnname))//'.gaul.star'
        open(unit=21,file=f1,form='unformatted')
        open(unit=22,file=f2,form='formatted')
        call pgsch(0.5)
        do i=1,ngau
         read (21) dumi,dumi,flag,dumr4,dumr4,x,dumr,y
c! changed for 4c.  for bdsm o/p what do i do ?
c         read (22,777) nam
         if (flag.eq.0) then
           read (22,877) dumi,dumi,dumr,dumi,dumi,dumr,dumr,dumr,dumr,
     /                dumi,dumr,nam
          x4=x
          y4=y
          if (nchar(nam).gt.0.and.nam(1:nchar(nam)).ne.'NONE') then
           call pgtext(x4,y4,nam(1:nchar(nam)))
          end if
         end if
        end do
        call pgsch(1.0)
        close(21)
        close(22)
777     format(71x,a)
877     format(i2,1x,i2,1x,f6.3,1x,1x,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)

        return
        end
c!
        subroutine tvwin(f1,n,m,blc,trc)
        implicit none
        character f1*500,extn*20,strdev*5,lab*500,ch1*1,ch2*1
        integer n,m,nchar
        real*8 image(n,m),std,av,nsig
        real*4 low,up,blc(2),trc(2),vport(4)

        extn='.img'
        strdev='/xs'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call arr2dnz(image,n,m,n,m,low,up)
        nsig=5.d0
        call sigclip(image,n,m,1,1,n,m,std,av,nsig)
        low=max(av-10.0*std,low)
        up=min(av+10.0*std,up)
        call pgbegin(0,strdev,1,1)
        lab='Mark a window using blc, trc'
        call plotimage1(n,m,image,up,low,lab,'LG')
        blc(1)=1.0
        blc(2)=1.0
        trc(1)=n*1.0
        trc(2)=m*1.0
        ch1=' '
        ch2=ch1
        vport(1)=0.15
        vport(2)=0.85
        vport(3)=0.15
        vport(4)=0.85
        call pgsub_op1(vport,blc,trc,n,m,ch1,ch2)
        call pgsci(5)
        call pgsfs(2)
        call pgrect(blc(1)*1.0,trc(1)*1.0,blc(2)*1.0,trc(2)*1.0)
        call pgsci(1)
        call pgsfs(1)


        return
        end

