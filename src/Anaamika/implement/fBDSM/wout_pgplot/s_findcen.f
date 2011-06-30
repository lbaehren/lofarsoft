c! stupid prog to find centre of image given one almost symmetric source
c! difficult to find correct centre. tried various combinations of c12 and c21
c! to minimise but doesnt work. so now for c1, find zero for every vertical line
c! and for c2 for horizontal line, and then find intersection point of these two.
c! that shud work even for very non gaussian cases.

        subroutine s_findcen(arr,n,m,hc,nhc,beta,nmax,num,xinter,yinter,
     /             std,sav,runcode)
        implicit none
        integer n,m,nmax,i,j,num,xmax,ymax,xind,yind,nhc
        real*8 arr(n,m),beta,cf12(n,m),cf21(n,m),hc(nhc,nhc)
        real*8 hcx(nhc),hcy(nhc),B_im(n,m)
        real*8 cf1(n,m),cf2(n,m),trans(m,n),trcf21(m,n)
        real*8 std,av,mn,nsig,maxv,magic
        real*8 x1(n+m),y1(n+m),x2(n+m),y2(n+m)
        real*8 xfit1(5),yfit1(5),xfit2(5),yfit2(5),sig(5)
        real*8 a(2),chisq,covar(2,2),b(2),xinter,yinter
        character f1*500,sav*1,runcode*2

        call maxarray(arr,n,m,n,m,xmax,ymax,maxv)  ! peak of emission
        call getcoeffarr(hc,nhc,num,1,hcx) 
        call getcoeffarr(hc,nhc,num,2,hcy) 
        magic=-999.d0
        nsig=3.d0
        do 100 i=1,n
         do 110 j=1,m
          if (arr(i,j).ge.nsig*std) then
           call getcartim(n,m,beta,1,2,hcx,hcy,i*1.d0,j*1.d0,nmax,B_im) ! image of c12 at (i,j)
           call decompcartcoeff(arr,n,m,B_im,cf12(i,j))                 ! value of c12
           call getcartim(n,m,beta,2,1,hcy,hcx,i*1.d0,j*1.d0,nmax,B_im) ! image of c21 at (i,j)
           call decompcartcoeff(arr,n,m,B_im,cf21(i,j))
          else 
           cf12(i,j)=magic
           cf21(i,j)=magic
          end if
          trans(j,i)=arr(i,j)
          trcf21(j,i)=cf21(i,j)
110      continue
100     continue
        
c! in high snr area, get zero crossings for each hori and vert line for c2, c1 resp
        call getzero(arr,cf12,magic,n,m,nsig*std,x1,y1,ymax,xmax)     ! y1 is array of 0-crossings
        call getzero(trans,trcf21,magic,m,n,nsig*std,y2,x2,xmax,ymax) ! so is x2

c! find nominal intersection pt in integers
        do 200 i=1,n
         if (x1(i).eq.xmax) xind=i
200     continue
        do 210 i=1,m
         if (y2(i).eq.ymax) yind=i
210     continue

c! now take 2 before and 2 after fit straight lines, get proper intersection
        do 220 i=1,5
         xfit1(i)=x1(xind+(i-3))
         yfit1(i)=y1(xind+(i-3))
         xfit2(i)=x2(yind+(i-3))
         yfit2(i)=y2(yind+(i-3))
         sig(i)=1.d0
220     continue
        
        call fitstr8line(xfit1,yfit1,sig,5,5,a,chisq,covar)
        call fitstr8line(xfit2,yfit2,sig,5,5,b,chisq,covar)
        
        xinter=(a(1)-b(1))/(b(2)-a(2))
        yinter=a(1)+a(2)*xinter
        
        if (sav.eq.'y') then
         f1='c1'
         call writearray_bin2D(cf12,n,m,n,m,f1,runcode)
         f1='c2'
         call writearray_bin2D(cf21,n,m,n,m,f1,runcode)
        end if

        return
        end


c! get zero of a slice, cud be vertical or horizontal.
c! damn thing works ! amazing ... 
c! for each i, y has value where cf crosses zero.
        subroutine getzero(image,cf,magic,n,m,cutoff,x,y,cen,cenx)
        implicit none
        integer n,m,low,up,i,j,npt,error,npoint,cen,cenx
        real*8 cf(n,m),cutoff,x(n+m),y(n+m),image(n,m),magic
        real*8 root,xfn(n+m),yfn(n+m)
        
        do 100 i=1,n
         x(i)=i*1.d0
         npt=0
         do 110 j=1,m
          if (image(i,j).ge.cutoff) npt=npt+1
110      continue

         if (npt.ge.4.and.cf(i,cen).ne.magic) then !if valid slice then

          do 120 j=cen,1,-1   ! get lower bound of slice
           low=j
           if (image(i,j).lt.cutoff) goto 333
120       continue 
333       continue
          do 130 j=cen,m   ! get upper bound of slice
           up=j
           if (image(i,j).lt.cutoff) goto 334
130       continue 
334       continue
          low=low+1  ! so u dont get one pixel edges.
          up=up-1

          npoint=up-low+1              ! create slice to find root, find it
          do 140 j=1,npoint
           xfn(j)=low*1.d0+(j-1)
           yfn(j)=cf(i,int(xfn(j)))
140       continue
          call getroot(npoint,xfn,yfn,root,error,n+m,x(i),cenx,cen)
          if (error.eq.1) then
           y(i)=0.d0
          else
           y(i)=root
          end if

         else
          y(i)=0.d0
         end if
100     continue

        return
        end



        
