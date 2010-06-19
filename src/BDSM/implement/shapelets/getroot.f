c! finds root.
c! dont know why found minint by closest to zero. doesn work when u
c! complicated sources where it may go closer to zero but not cross, elsewhere
c! so now check for crossing properly.
c! but u get cases where there is more than one root. 
c! for now, take root closer to 'centre' which is intensity barycentre.

        subroutine getroot(npoint,xfn,yfn,root,error,n,xco,xcen,ycen)
        implicit none
        integer n,npoint,error,ia(2),nfit,minint,up,low,i,xcen,ycen
        integer minintold,dnew,dold
        real*8 xfn(n),yfn(n),root,mn,mx
        real*8 xfit(n),yfit(n),sig(n),xco
        real*8 alamda,chisq,covar(2,2),alpha(2,2),a(2)
        data ia/1,1/

        error=0
        call rangevec8(yfn,n,npoint,mn,mx)
        if (mn*mx.ge.0.d0) error=1

c        minint=1      ! minint is index of pt where y is closest to zero
c        do 100 i=1,npoint
c         if (abs(yfn(minint)).gt.abs(yfn(i))) minint=i
c100     continue

c! second time distance is wrong but is ok since x,y are exchanged for all pairs.
        minintold=0
        minint=0
        do 105 i=2,npoint
         if (yfn(i-1)*yfn(i).lt.0.d0) then
          if (minintold.eq.0) then            ! so take nearest to centre
           if (abs(yfn(i-1)).lt.abs(yfn(i))) then 
            minint=i-1
           else
            minint=i
           end if
          else
           dnew=sqrt((xco-xcen)*(xco-xcen)+(xfn(i)-ycen)*(xfn(i)-ycen))
           dold=sqrt((xco-xcen)*(xco-xcen)+
     /          (xfn(minintold)-ycen)*(xfn(minintold)-ycen))
           if (dnew.le.dold) then
            minint=i
           else
            minint=minintold
           end if
          end if
          minintold=minint
         end if
105     continue

        if (minint.lt.1.or.minint.gt.npoint) error=1
        if (error.ne.1) then 
         low=minint-min(2,minint-1)
         up=minint+min(2,npoint-minint)
         nfit=up-low+1
         do 110 i=1,nfit
          xfit(i)=xfn(low+(i-1))
          yfit(i)=yfn(low+(i-1))
          sig(i)=1.d0
110      continue

         call fitstr8line(xfit,yfit,sig,n,nfit,a,chisq,covar)
         root=-1.d0*a(1)/a(2)
         if (root.lt.xfn(low).or.root.gt.xfn(up)) error=1
        end if

        return
        end


