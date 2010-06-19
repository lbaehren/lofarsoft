c! fits y = sqrt (c1^2 + c2^2/snr^2); uses func3.f
c! rms increases initially before decreasing - because of cutoffs and such, so exclude those.

        subroutine callfit_aasl_1(nbin,xval,std,estd,cod,s_c,ns)
        implicit none
        integer nbin,i,sind,cod,ns,round
        real*8 std(nbin),estd(nbin),xval(nbin),s_c(ns)

        sind=0
        do i=1,nbin-4
         if (std(i).gt.std(i+1).and.std(i+1).gt.std(i+2)
     /       .and.std(i+2).gt.std(i+3)) then   ! to be safe
          sind=i
          goto 100
         end if
        end do
100     continue
        if (sind.eq.0) write (*,*) ' Cant find monotonic part'
        if (sind.eq.0) sind=1
        if (sind.gt.0.25d0*nbin) sind=round(0.25d0*nbin)
        
        call sub_callfit_aasl_1(nbin,xval,std,estd,cod,ns,s_c,sind)
        
        return
        end


        subroutine sub_callfit_aasl_1(nbin,xval,std,estd,cod,ns,
     /             s_c,sind)
        implicit none
        integer nbin,ns,ia(ns),j,sind,cod,code,mx
        real*8 std(nbin),estd(nbin),alamda,chisq,xval(nbin)
        real*8 covar(ns,ns),alpha(ns,ns),a(ns),s_c(ns)
        real*8 xfit(nbin-sind+1),yfit(nbin-sind+1),ey(nbin-sind+1)

        do j=1,ns
         ia(j)=1
        end do

        if (cod.eq.1) code=4  ! a1+a2*x+a3*x*x
        if (cod.eq.2) code=5  ! sqrt(a1^2+a2^2/x^2)
        if (cod.eq.3) code=2  ! a1+a2*x

        do j=sind,nbin
         xfit(j-sind+1)=xval(j)
         yfit(j-sind+1)=std(j)
         ey(j-sind+1)=estd(j)
        end do

        if (xfit(1).gt.xfit(nbin-sind+1)) then 
         mx=1
        else
         mx=nbin-sind+1
        end if
        if (cod.eq.2) then
         a(1)=yfit(mx)
         a(2)=1.d0
        end if
        if (cod.eq.1) then
         a(1)=yfit(mx)
         do j=2,ns
          a(j)=0.d0
         end do
        end if
        if (cod.eq.3) then
         a(1)=yfit(mx)
         a(2)=0.d0
        end if

        alamda=-1.d0
        do 120 j=1,20
         call mrqmin(xfit,yfit,ey,nbin-sind+1,nbin-sind+1,a,ia,ns,
     /        covar,alpha,ns,chisq,alamda,code)  
120     continue
        alamda=0.d0
         call mrqmin(xfit,yfit,ey,nbin-sind+1,nbin-sind+1,a,ia,ns,
     /        covar,alpha,ns,chisq,alamda,code)  

        do j=1,ns
         s_c(j)=a(j)
        end do

        return
        end


