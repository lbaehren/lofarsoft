c! calculates area etc as fn of radius


      subroutine fn_radius(fn,scratch)
      implicit none
      character scratch*500,fn*500,extn*20
      integer n,m,l,rnum
      real*8 r1,r2,rstep

cf2py   intent(in) fn,scratch

      extn='.img'
      call readarraysize(fn,extn,n,m,l)

      r1=4.d0
      r2=min(n,m)/2.d0-4.d0
      rstep=12.d0 
      rnum=int((r2-r1)/rstep+1)

      call sub_fn_radius(fn,n,m,scratch,r1,r2,rstep,rnum)

      return
      end

      subroutine sub_fn_radius(fn,n,m,scratch,r1,r2,rstep,rnum)
      implicit none
      character scratch*500,fn*500,extn*20,rc*2,f1*500
      integer n,m,l,opt,rnum,i,ii,jj,id2,pgopen
      real*8 image(n,m),r1,r2,rstep,radi(rnum),xc,yc,summ(rnum),m1,m2
      logical pixinimage,inarea,full
      real*4 arr4(n,m),mnx,mny,mxx,mxy,tr(6),mn,mx,sum4(rnum)
      real*4 radi4(rnum)

      data tr/0.0,1.0,0.0,0.0,0.0,1.0/

      extn='.img'
      call readarray_bin(n,m,image,n,m,fn,extn)
      write (*,*) '  Function of radius : '
      write (*,*) '  1. Area '
c      read (*,*) opt
      opt=1

c! centre is edge of centre pixel and rstep is additive
      if (opt.eq.1) then
       xc=int(min(n,m)/2.d0)+0.5
       yc=int(min(n,m)/2.d0)+1.5 ! trc of that pixel
       do i=1,rnum
        radi(i)=r1+(i-1)*rstep
        summ(i)=0.d0
        do jj=int(yc-radi(i)*2.d0),int(yc+radi(i)*2.d0)  ! cant bother to do exact
         do ii=int(xc-radi(i)*2.d0),int(xc+radi(i)*2.d0)  ! cant bother to do exact
         if (pixinimage(ii,jj,1,n,1,m)) then   ! if pix inside image
           call pixpixdist(ii,jj,xc,yc,radi(i),inarea,full)
           if (i.eq.rnum) inarea=.true.
           if (i.eq.rnum) full=.true.
           if (inarea) then
            if (full) then
             summ(i)=summ(i)+image(ii,jj)
            else
             summ(i)=summ(i)+image(ii,jj)*0.5d0 !!! WWWRRROOONNNGGG !!!!
            end if
           end if
          end if
         end do
        end do
       write (*,*) i,radi(i),summ(i),summ(i)/radi(i)/radi(i)
       summ(i)=summ(i)/radi(i)/radi(i)
       end do
      end if

      call pgbegin(0,'?',1,1)
      call pgvport(0.1,0.9,0.4,0.9)
      call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
      call pgbox('BCINST',0.0,0,'BICNST',0.0,0)

      call array8to4(image,n,m,arr4,n,m)
      call range2d(arr4,n,m,n,m,mn,mx)
      call pggray(arr4,n,m,1,n,1,m,mx,mn,tr)
      call pgwedg('RG',1.5,3.0,mx,mn,' ')

      call vec8to4(summ,rnum,sum4,rnum)
      call vec8to4(radi,rnum,radi4,rnum)
      call pgvport(0.1,0.9,0.1,0.4)
      call rangevec8(summ,rnum,rnum,m1,m2)
      mny=m1
      mxy=m2
      mxx=radi(rnum)
      call pgwindow(0.0,mxx,mny,mxy)
      call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
      call pgline(rnum,radi4,sum4)      
      call pgend



      return
      end

      subroutine pixpixdist(ii,jj,xc,yc,radi,inarea,full)
      implicit none
      logical inarea,full
      integer ii,jj,i,j
      real*8 xc,yc,radi,mindist,maxdist,dist,x,y

      mindist=9.d99
      maxdist=-9.d9
      do i=1,2
       do j=1,2
        x=xc+(2.d0*i-3.d0)/2.d0
        y=yc+(2.d0*i-3.d0)/2.d0
        dist=sqrt((ii-x)*(ii-x)+(jj-y)*(jj-y))
        if (dist.le.mindist) mindist=dist
        if (dist.gt.maxdist) maxdist=dist
       end do
      end do
      if (mindist.le.radi) then
       inarea=.true.
      else
       inarea=.false.
      end if
      if (maxdist.le.radi) then
       full=.true.
      else
       full=.false.
      end if

      return
      end


