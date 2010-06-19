c! Subroutines for createsky.f

        subroutine getnsrcbin(nbin,sarr,cn1,gam1,low,high,nsrc)
        implicit none
        integer i,nbin
        real*8 sarr(500),cn1(500),gam1(500)
        real*8 low(500/nbin),high(500/nbin),nsrc(500/nbin)

        do 100 i=1,500/nbin 
        
         low(i)=cn1(i)*(sarr((i-1)*nbin+1)**gam1(i))
         if ((i*nbin+1).le.500) then 
          high(i)=cn1(i+1)*(sarr(i*nbin+1)**gam1(i+1))
         else
          high(i)=cn1(500/nbin)*(sarr(500)**gam1(500/nbin))
         end if
         nsrc(i)=low(i)-high(i)
         if (i.eq.500/nbin.and.high(i).gt.0.1d0) then
          low(i)=low(i)+high(i)
          high(i)=0.d0
          nsrc(i)=low(i)-high(i)
         end if
100     continue

        return  
        end


        subroutine getrandsize(flux,freq,ssize,seed)
        implicit none
        integer seed
        real*8 flux,freq,ssize,medsize,rand
        
        medsize=2.d0*(flux*1.d3*((1.4d9/freq)**(-0.8d0)))**0.3d0  ! median size in arcsec
        call ran1(seed,rand)
        call calcsize(seed,medsize,ssize)  ! sizeas is actual size in arcsec

        return
        end



        subroutine getrandflux(n_ga,n_gb,cn,gam,rand,func,seed)
        implicit none
        integer seed
        real*8 n_ga,n_gb,cn,gam,rand,func

        call ran1(seed,rand)
        func=((n_ga-(n_ga-n_gb)*rand)/cn)**(1.d0/gam)
        
        return
        end



        subroutine getrandposn(n,m,x,y,seed)
        implicit none
        integer n,m,seed
        real*8 rand,x,y

        call ran1(seed,rand)
        x=rand*(n-1)+1
        call ran1(seed,rand)
        y=rand*(m-1)+1
        
        return
        end



        subroutine calcsize(seed,medsize,sizeas)  ! sizeas is actual size in arcsec
        implicit none
        integer seed
        real*8 medsize,sizeas,a,b,rand,h,ha,hb

        a=min(medsize*0.2d0,0.5d0)   ! min arcsec
        b=40.d0 ! max arcsec

        call ran1(seed,rand)

        ha=h(medsize,a)
        hb=h(medsize,b)
        sizeas=-dlog(rand*(hb-ha)+ha)
        sizeas=medsize*((sizeas/dlog(2.d0))**(1.d0/0.62d0))

        return
        end



        function h(med,val)
        implicit none
        real*8 med,val,h
        
        h=dexp((-1.d0)*dlog(2.d0)*((val/med)**(0.62d0)))

        return
        end


        subroutine writesrclist(nn,xp,yp,fl,bmaj,bmin,bpa)
        implicit none
        integer nn
        real*8 xp,yp,fl,bmaj,bmin,bpa
        real*4 x4,y4,f4,s1,s2,s3
        
        x4=xp
        y4=yp
        f4=fl
        s1=bmaj
        s2=bmin
        s3=bpa
        write (nn,*) x4,y4,f4,s1,s2,s3
        
        return
        end

c! get size for convolved subimage of each source here. is an arbit hack, no fundae
        subroutine get_subim_imsize(beam,flux,sens,simsize,bmvaryn,
     /             bmvaryf,beamvary,imsize)
        implicit none
        include "includes/constants.inc"
        real*8 beam,flux,sens,dx,largest
        integer simsize,round
        integer bmvaryn,imsize
        real*8 bmvaryf(bmvaryn)
        character beamvary

        if (beamvary.eq.'n') then
         if (flux.lt.sens) simsize=round(beam)
         if (flux.ge.sens) then
          dx=sqrt(-2.d0*beam*beam/fwsig/fwsig*dlog(sens/3.5d0/flux))
          simsize=round(2.d0*dx)
         end if
         simsize=simsize+1
        end if

        if (beamvary.eq.'1') then
         largest=beam+2.d0*bmvaryf(1)+bmvaryf(2)*imsize
         if (flux.lt.sens) simsize=round(largest)
         if (flux.ge.sens) then
          dx=sqrt(-2.d0*largest*largest/fwsig/fwsig*
     /       dlog(sens/3.5d0/flux))
          simsize=round(2.d0*dx)
         end if
         simsize=simsize+1
        end if

        return
        end


