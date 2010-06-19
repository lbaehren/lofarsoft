c! stupid prog to find centre of image given one almost symmetric source
c! difficult to find correct centre. tried various combinations of c12 and c21
c! to minimise but doesnt work. so now for c1, find zero for every vertical line
c! and for c2 for horizontal line, and then find intersection point of these two.
c! that shud work even for very non gaussian cases.

        subroutine findcen_cart(arr,mask,n,m,beta,
     /       nmax,xinter,yinter,sav,runcode)
        implicit none
        include "trhermcoef.inc"
        integer n,m,nmax,i,j,xmax,ymax,xind,yind,n1,n2
        integer maxx,maxy,round,ninter,ii,jj,ind
        integer mask(n,m),trmask(m,n),smask1(5),smask2(5)
        real*8 arr(n,m),beta,cf12(n,m),cf21(n,m)
        real*8 hcx(50),hcy(50),B_im(n,m),m1,m2(2),m3(2)
        real*8 trans(m,n),trcf21(m,n),mim(n,m)
        real*8 av,mn,maxv,magic,dum(n,m)
        real*8 x1(n+m),y1(n+m),x2(n+m),y2(n+m)
        real*8 xft1(5),yft1(5),xft2(5),yft2(5),sig(5)
        real*8 a(2),chisq,covar(2,2),b(2),xinter,yinter
        character f1*500,sav*1,runcode*2

cf2py   intent(in) arr,mask,beta,nmax,sav,runcode
cf2py   intent(out) xinter,yinter

        call maxarraymask(arr,mask,n,m,xmax,ymax,maxv)

c! if source at edge of image and hence island
        if (xmax.eq.1.or.ymax.eq.1.or.xmax.eq.n.or.ymax.eq.m) then
         call momentonlymaskonly(mask,n,m,n,m,m1,m2,m3)  
         maxx=round(m2(1))
         maxy=round(m2(2))
        else
         maxx=xmax
         maxy=ymax
        end if

        call getcoeffarr(hc,31,nhc,1,hcx) 
        call getcoeffarr(hc,31,nhc,2,hcy) 
        magic=-999.d0
        do 100 i=1,n
         do 110 j=1,m
          if (mask(i,j).eq.1) then
c                                     image of c12 at (i,j)
           call getcartim(n,m,beta,1,2,hcx,hcy,i*1.d0,j*1.d0,
     /          nmax,B_im) 
           call decompcartcoeff_mask(arr,mask,n,m,B_im,cf12(i,j))

           if (i.eq.28.and.j.eq.52) then
            do ii=1,n
             do jj=1,m
               dum(ii,jj)=B_im(ii,jj)
             end do
            end do 
           end if

c                                     image of c21 at (i,j)
           call getcartim(n,m,beta,2,1,hcy,hcx,i*1.d0,j*1.d0,
     /          nmax,B_im) 
           call decompcartcoeff_mask(arr,mask,n,m,B_im,cf21(i,j))
          else 
           cf12(i,j)=magic
           cf21(i,j)=magic
          end if
          trans(j,i)=arr(i,j)
          trmask(j,i)=mask(i,j)
          trcf21(j,i)=cf21(i,j)
110      continue
100     continue
        
        if (sav.eq.'y') then
         f1='c1'
         call writearray_bin2D(cf12,n,m,n,m,f1,runcode)
         f1='c2'
         call writearray_bin2D(cf21,n,m,n,m,f1,runcode)
        end if

c! in high snr area, get zero crossings for each hori and vert line for c2, c1 resp
        call getzero_mask(cf12,magic,n,m,mask,x1,y1,maxy,maxx)     ! y1 is array of 0-crossings
        call getzero_mask(trcf21,magic,m,n,trmask,y2,x2,
     /       maxx,maxy)! so is x2

c! find nominal intersection pt in integers
        do 200 i=1,n
         if (x1(i).eq.maxx) xind=i
200     continue
        do 210 i=1,m
         if (y2(i).eq.maxy) yind=i
210     continue

c! now take 2 before and 2 after fit straight lines, get proper intersection
        if (xind.lt.3.or.yind.lt.3.or.xind.gt.n-2.or.yind.gt.m-2) then
         ninter=3
        else
         ninter=5
        end if
        do 220 i=1,ninter
         ind=(ninter-1)/2
         xft1(i)=x1(xind+(i-(ind+1)))
         yft1(i)=y1(xind+(i-(ind+1)))
         xft2(i)=x2(yind+(i-(ind+1)))
         yft2(i)=y2(yind+(i-(ind+1)))
         sig(i)=1.d0
         if (yft1(i).eq.0.d0) then
          smask1(i)=0
         else
          smask1(i)=1
         end if
         if (xft2(i).eq.0.d0) then
          smask2(i)=0
         else
          smask2(i)=1
         end if
220     continue
        
        call vec_int_sum(smask1,5,ninter,n1)
        call vec_int_sum(smask2,5,ninter,n2)
        if (n1.gt.0.and.n2.gt.0) then
         call call_fitstr8line(xft1,yft1,smask1,sig,n1,5,ninter,a,
     /        chisq,covar)
         call call_fitstr8line(xft2,yft2,smask2,sig,n2,5,ninter,b,
     /        chisq,covar)
         xinter=(a(1)-b(1))/(b(2)-a(2))
         yinter=a(1)+a(2)*xinter
        else
         xinter=0.d0
         yinter=0.d0
        end if
        
        return
        end


c! get zero of a slice, cud be vertical or horizontal.
c! damn thing works ! amazing ... 
c! for each i, y has value where cf crosses zero.
        subroutine getzero_mask(cf,magic,n,m,mask,x,y,cen,cenx)
        implicit none
        integer n,m,low,up,i,j,npt,error,npoint,cen,cenx,mask(n,m)
        real*8 cf(n,m),x(n+m),y(n+m),magic
        real*8 root,xfn(n+m),yfn(n+m)
        
        do 100 i=1,n
         x(i)=i*1.d0
         npt=0
         do 110 j=1,m
          if (mask(i,j).eq.1) npt=npt+1
110      continue

         if (npt.ge.4.and.cf(i,cen).ne.magic) then !if valid slice then

          do 120 j=cen,1,-1   ! get lower bound of slice
           low=j
           if (mask(i,j).eq.0) goto 333
120       continue 
333       continue
          do 130 j=cen,m   ! get upper bound of slice
           up=j
           if (mask(i,j).eq.0) goto 334
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
c!
c!
c!
        subroutine call_fitstr8line(xfit,yfit,mask,sig,n1,n,ninter,a,
     /             chisq,covar)
        implicit none
        integer n1,n,mask(n),i,ind,ninter
        real*8 xfit(n),yfit(n),sig(n),x(n1),y(n1),s(n1)
        real*8 x1(n1),y1(n1),s1(n1)
        real*8 a(2),covar(2,2),chisq

        ind=0
        do i=1,ninter
         if (mask(i).eq.1) then
          ind=ind+1
          x(ind)=xfit(i)
          y(ind)=yfit(i)
          s(ind)=sig(i)
         end if
        end do
        if (ind.ne.n1) write (*,*) '  !! SCREAM !! '

        call fitstr8line(x,y,s,n1,n1,a,chisq,covar)

        return
        end


