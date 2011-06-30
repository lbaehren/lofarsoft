c! various ways of tesselating
c! if generators=calibrators then u dont need to tesselate, u idiot.
c! just get modified list based on veyr nearby ones etc.
c! if generators=field then

        subroutine tesselate(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /     ngensS,dumi,xgensS,ygensS,snrgensS,tess_method,tess_sc,
     /     tess_fuzzy,generators,gencode,fullname,xtile,ytile,snrtile)
        implicit none
        integer n,m,ngens,ngensmod,i,tess_method,dumi,ngensS,nchar
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 xgensS(dumi),ygensS(dumi),snrgensS(dumi)
        real*8 wts(ngens),tess_fuzzy
        real*8 xtile(ngens),ytile(ngens),snrtile(ngens)
        character code*1,tess_sc*1,gencode*4,generators*500,fn*500
        character runcode*2,fullname*500
       
         i=1
c         if (tess_method.eq.0) call tess_adapbin(n,m,ngens,ngensmod,
c     /     xgens,ygens,snrgens,volrank,tess_sc,tess_fuzzy)
c         if (tess_method.eq.1) call tess_bin_wt4(n,m,ngens,ngensmod,
c     /     xgens,ygens,snrgens,volrank,tess_sc,tess_fuzzy)
         if (tess_method.eq.0) call tess_adapbin(n,m,ngens,ngensmod,
     /     xtile,ytile,snrtile,volrank,tess_sc,tess_fuzzy)
         if (tess_method.eq.1) call tess_bin_wt4(n,m,ngens,ngensmod,
     /     xtile,ytile,snrtile,volrank,tess_sc,tess_fuzzy)
         if (tess_method.eq.2) call tess_bin_wt1(n,m,ngens,ngensmod,
     /     xgens,ygens,snrgens,volrank,tess_sc,tess_fuzzy)
         if (tess_method.eq.3) call tess_bin_wt2(n,m,ngens,ngensmod,
     /     xgens,ygens,snrgens,volrank,tess_sc,tess_fuzzy)

         runcode='av'
         if (i.eq.1) fn=fullname(1:nchar(fullname))//'.vororank0'
         if (i.eq.2) fn=fullname(1:nchar(fullname))//'.vororank1'
         if (i.eq.3) fn=fullname(1:nchar(fullname))//'.vororank2'
         if (i.eq.4) fn=fullname(1:nchar(fullname))//'.vororank3'
         call writearray_bin2D(volrank,n,m,n,m,fn,runcode)

        return
        end
c!
c!
c! just adaptive binning s.t. each pixel belongs to bin of generator it is closest to
        subroutine tess_adapbin(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,code,eps)! just bin
        implicit none
        integer n,m,ngens,ngensmod,i
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 wts(ngens),eps
        character code*1

        do i=1,ngensmod
         wts(i)=1.d0
        end do
        call tess_bin_simple(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,wts,code,eps)

        return
        end
c!
c!
c!      wts=log(SNR)
        subroutine tess_bin_wt1(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,code,eps)! just bin
        implicit none
        integer n,m,ngens,ngensmod,i
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 wts(ngens),eps
        character code*1

        do i=1,ngensmod
         wts(i)=dlog10(snrgens(i))
        end do
        call tess_bin_simple(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,wts,code,eps)

        return
        end
c!
c!
c!      wts=sqrt(SNR)
        subroutine tess_bin_wt2(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,code,eps)! just bin
        implicit none
        integer n,m,ngens,ngensmod,i
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 wts(ngens),eps
        character code*1

        do i=1,ngensmod
         wts(i)=sqrt(dlog10(snrgens(i)))
        end do
        call tess_bin_simple(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,wts,code,eps)

        return
        end
c!
c!
c!      wts=sqrt(SNR)/Area
        subroutine tess_bin_wt3(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,code,eps)! just bin
        implicit none
        integer n,m,ngens,ngensmod,i,areavec(ngens)
        integer areavecold(ngens),niter
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 wts(ngens),std,eps
        character code*1

        call initialise_int_vec(areavec,ngensmod,1)

        niter=0
333     continue
        do i=1,ngensmod
         if (niter.eq.0) then
          wts(i)=1.d0
         else
          wts(i)=sqrt(snrgens(i)/areavec(i))
         end if
        end do
        niter=niter+1
        call tess_bin_simple(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,wts,code,eps)
        do i=1,ngensmod
         areavecold(i)=areavec(i)
        end do
        call calc_area_tess(volrank,n,m,ngensmod,ngens,areavec)
        call int_vec_vec_std(areavec,areavecold,ngensmod,std)
        if (niter.eq.1.or.(std.gt.0.d0.and.niter.lt.2)) then
         goto 333
        end if

        return
        end
c!
c!      wts=roundness
        subroutine tess_bin_wt4(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /             volrank,code,eps)! just bin
        implicit none
        integer n,m,ngens,ngensmod,i,areavec(ngens),j,k
        integer roundfacold(ngensmod),niter,armn,armx
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 wts(n,m,ngens),std,eps,roundfac(ngens)
        real*8 roundpix(ngens),x(ngens),y(ngens)
        character code*1

        call initialise_vec(roundfac,ngens,2.d0/3.d0)
        call initialise_vec(x,ngens,0.d0)
        call initialise_vec(y,ngens,0.d0)
        call initialise_vec(roundpix,ngens,0.d0)
        call initialise_int_vec(areavec,ngens,1)
        call rangeint(areavec,ngens,ngensmod,armn,armx)

        niter=0
333     continue
        call tess_bin_complicated(n,m,ngens,ngensmod,xgens,ygens,
     /        snrgens,volrank,roundpix,x,y,niter,code,eps)
        niter=niter+1
        do i=1,ngensmod
         roundfacold(i)=roundfac(i)
        end do
        call tile_roundness(volrank,n,m,ngens,ngensmod,xgens,
     /             ygens,roundfac,roundpix,x,y)
        call calc_area_tess(volrank,n,m,ngensmod,ngens,areavec)
        call rangeint(areavec,ngens,ngensmod,armn,armx)
c        call int_vec_vec_std(roundfac,roundfacold,ngensmod,std)
c        if (niter.eq.1.or.(std.gt.0.d0.and.niter.lt.2)) then
        if (niter.eq.1.or.(niter.lt.2)) then
         goto 333
        end if

        return
        end
c!
c!
c! if code='s' then each pt belongs to one bin. If not then fuzzy tesselation
        subroutine tess_bin_simple(n,m,ngens,ngensmod,xgens,ygens,
     /             snrgens,volrank,wts,code,eps)! just bin  ! code from arya and malamatos
        implicit none
        integer n,m,ngens,ngensmod,i,j,k,minind(n,m),l
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),dist
        real*8 dumr,snrgens(ngens),wts(ngens),eps,dr,distmin
        character code*1

        do j=1,m
         do i=1,n
          volrank(i,j)=0.d0
          dumr=1.d90
          do k=1,ngensmod
           dist=sqrt((i-xgens(k))*(i-xgens(k))+
     /               (j-ygens(k))*(j-ygens(k)))/wts(k)
           if (dist.lt.dumr) then
            dumr=dist
            minind(i,j)=k
           end if               ! minind(i,j) is number of nearest generator
          end do
         end do
        end do      
c!
        if (code.eq.'s') then   
         do j=1,m
          do i=1,n
           volrank(i,j)=1.d0*minind(i,j)
          end do
         end do
        else
         do j=1,m
          do i=1,n
           do k=1,ngensmod
            l=minind(i,j)
            if (k.ne.l) then
             dist=sqrt((i-xgens(k))*(i-xgens(k))+
     /                 (j-ygens(k))*(j-ygens(k)))/wts(k)
             distmin=sqrt((i-xgens(l))*(i-xgens(l))+
     /                    (j-ygens(l))*(j-ygens(l)))/wts(l)
             if (dist.le.(1.d0+eps)*distmin)
     /           volrank(i,j)=volrank(i,j)+1.d0*(l+k)
            end if           
           end do
          end do
         end do
        end if

        return
        end
c!
c! same as simple but weights are fn of each pixel now rather than bin
c! if code='s' then each pt belongs to one bin. If not then fuzzy tesselation
        subroutine tess_bin_complicated(n,m,ngens,ngensmod,xgens,ygens,
     /             snrgens,volrank,roundpix,x,y,niter,code,eps)! just bin  ! code from arya and matamos
        implicit none
        integer n,m,ngens,ngensmod,i,j,k,minind(n,m),l,niter
        real*8 volrank(n,m),xgens(ngens),ygens(ngens),dist,dist1
        real*8 dumr,snrgens(ngens),eps,roundpix(ngens)
        real*8 x(ngens),y(ngens),dumr1,wts
        character code*1

        do j=1,m
         do i=1,n
          volrank(i,j)=0.d0
          dumr=1.d90
          do k=1,ngensmod
           if (niter.eq.0) then
            wts=1.d0
           else
            dumr1=sqrt((x(k)-i)*(x(k)-i)+(y(k)-j)*(y(k)-j))
            dumr1=dumr1*roundpix(k)
            wts=1.d0/dumr1
           end if
           dist=sqrt((i-xgens(k))*(i-xgens(k))+
     /                 (j-ygens(k))*(j-ygens(k)))/wts
           if (dist.lt.dumr) then
            dumr=dist
            minind(i,j)=k
           end if               ! minind(i,j) is number of nearest generator
          end do
         end do
        end do      
c!
        if (code.eq.'s') then   
         do j=1,m
          do i=1,n
           volrank(i,j)=1.d0*minind(i,j)
          end do
         end do
        else
         do j=1,m
          do i=1,n
           do k=1,ngensmod
            if (k.ne.l) then
             if (niter.eq.0) then
              wts=1.d0
             else
              dumr1=sqrt((x(k)-i)*(x(k)-i)+(y(k)-j)*(y(k)-j))
              dumr1=dumr1*roundpix(k)
              wts=1.d0/dumr1
             end if
             dist=sqrt((i-xgens(k))*(i-xgens(k))+
     /            (j-ygens(k))*(j-ygens(k)))/wts
             dist1=sqrt((i-xgens(minind(i,j)))*(i-xgens(minind(i,j)))+
     /            (j-ygens(minind(i,j)))*(j-ygens(minind(i,j))))/wts
             if (dist.le.(1.d0+eps)*dist1)
     /           volrank(i,j)=volrank(i,j)+1.d0*(minind(i,j)+k)
            end if           
           end do
          end do
         end do
        end if

        return
        end

        subroutine calc_area_tess(volrank,n,m,num,x,areavec)
        implicit none
        integer n,m,num,x,areavec(x),i,j
        real*8 volrank(n,m)
        
        call initialise_int_vec(areavec,x,0)
        do j=1,m
         do i=1,n
          areavec(int(volrank(i,j)))=areavec(int(volrank(i,j)))+1
         end do
        end do

        return
        end
c!
c!
c! check roundness.
c! modify to make roundpix not include dist so that u dont have to
c! define huge 3d arrays which crash.
        subroutine tile_roundness(volrank,n,m,ngens,ngensmod,xgens,
     /             ygens,roundfac,roundpix,x,y)
        implicit none
        include "constants.inc"
        integer n,m,i,j,ngens,ngensmod,ind,npix(ngens),k
        real*8 volrank(n,m),area(ngens),sumrad(ngens),dist
        real*8 xgens(ngens),ygens(ngens),roundfac(ngens)
        real*8 x(ngens),y(ngens),roundpix(ngens)

        call initialise_vec(area,ngens,0.d0)
        call initialise_vec(sumrad,ngens,0.d0)
        call initialise_int_vec(npix,ngens,0)

        call initialise_vec(x,ngens,0.d0)
        call initialise_vec(y,ngens,0.d0)
        do j=1,m
         do i=1,n
          ind=int(volrank(i,j))
          x(ind)=x(ind)+i
          y(ind)=y(ind)+j
          npix(ind)=npix(ind)+1
         end do
        end do
        do i=1,ngensmod
         x(i)=x(i)/npix(i)
         y(i)=y(i)/npix(i)
        end do
        
        call initialise_int_vec(npix,ngens,0)
        do j=1,m
         do i=1,n
          ind=int(volrank(i,j))
          dist=sqrt((xgens(ind)-i)*(xgens(ind)-i)+
     /              (ygens(ind)-j)*(ygens(ind)-j))
          dist=sqrt((x(ind)-i)*(x(ind)-i)+
     /              (y(ind)-j)*(y(ind)-j))
          area(ind)=area(ind)+1
          sumrad(ind)=sumrad(ind)+dist
          npix(ind)=npix(ind)+1
         end do
        end do
        
        do i=1,ngensmod
         roundfac(i)=(sumrad(i)/npix(i))/(sqrt(area(i)/pi))
         write (*,*) 'R ',xgens(i),ygens(i),roundfac(i),
     /      sumrad(i)/npix(i),sqrt(area(i)/pi)
        end do
         write (*,*) 

c        do j=1,m
c         do i=1,n
          do k=1,ngensmod
c           dist=sqrt((x(k)-i)*(x(k)-i)+(y(k)-j)*(y(k)-j))
c           roundpix(i,j,k)=dist/(sumrad(k)/npix(k))
           roundpix(k)=1.d0/(sumrad(k)/npix(k))
          end do
c         end do
c        end do
        
        return
        end





