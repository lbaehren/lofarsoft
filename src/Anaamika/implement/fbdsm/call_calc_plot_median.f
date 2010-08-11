
        subroutine call_calc_plot_median(x4n,y4n,maxngaul,n,label1,
     /         label2,xl,yl,f2,rcode,scrat,title,mny,mxy)
        implicit none
        integer nassoc,sumngaul,nchar,n,maxngaul,i,j,iwksp(n),nn,nct
        integer nbin,isrc
        character gsmsolnname*500,plotdir*500,scratch*500,f1*500,f2*500
        character s1*10,s2*10,scrat*500,fname*500,title*500
        real*4 x4n(maxngaul),y4n(maxngaul),mnx,mxx,mny,mxy
        real*4 y1,y2,y3,y4,x1,x2
        real*8 x(n),y(n),wksp(n),xs(n),ys(n),low,up,l1,l2,r
        real*8 xav,med,medc
        character label1*30,label2*30,chr1*1,xl*6,yl*6,nam*500,rcode*2
        character dev*500

        if (rcode(2:2).eq.'q') then
         dev=f2(1:nchar(f2))
         call pgbegin(0,dev,1,1)
        else
         call pgbegin(0,'?',1,1)
         call pgeras
        end if
        call pgsch(1.1)
        call pgslw(2)

        do i=1,n
         x(i)=x4n(i)
         y(i)=y4n(i)
        end do 
        
        call sort3_2(n,x,y,wksp,iwksp)
        low=x(5)
        up=x(n-5)
        
        nn=n
        nbin=20
        if (n/nbin*1.0.lt.30.0) nbin=int(nbin/30.0)
        if (xl(6:6).eq.'L'.or.xl(6:6).eq.'l') then
         r=(up-low)/nbin*1.0  ! actually log(r)
        else
         r=(up-low)/nbin*1.0  ! actually log(r)
        end if
        

        call pgvport(0.1,0.95,0.15,0.9)
        mnx=low
        mxx=up
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox(xl(1:nchar(xl)),0.0,0,yl(1:nchar(yl)),0.0,0)
        call pglabel(label1(1:nchar(label1)),
     /       label2(1:nchar(label2)),title(1:nchar(title)))

        do i=1,nbin
         l1=low+(i-1)*r
         l2=l1+r
         nct=0
         do isrc=1,n
          if (x(isrc).ge.l1.and.x(isrc).lt.l2) then
           nct=nct+1
           xs(nct)=x(isrc)
           ys(nct)=y(isrc)
          end if
         end do
         if (nct.gt.1) then
          call sub_calc_plot_median(xs,ys,nn,nct,xav,med,medc)
          x1=xav
          y1=med
          y2=medc
          !call pgpoint(1,x1,y1,3)
          call pgpoint(1,x1,y2,17)
          if (i.gt.1) then
          ! call pgmove(x1,y1)
          ! call pgmove(x2,y3)
           call pgdraw(x1,y2)
           call pgdraw(x2,y4)
          end if
          x2=x1
          y3=y1
          y4=y2
         end if
        end do
        call pgend
        
        return
        end
c!
c!
c!
        subroutine sub_calc_plot_median(x,y,nx,n,xav,med,medc)
        implicit none
        integer n,nx,i
        real*8 x(nx),y(nx),med,xav,medc,nsig,std,xx(n),yy(n)

        do i=1,n
         xx(i)=x(i)
         yy(i)=y(i)
        end do

        call calcmedian(yy,n,1,n,med)
        nsig=3.d0
        call calcmedianclip(yy,n,1,n,medc,nsig)
        call vec_std(xx,n,1,n,xav,std)

        return
        end


