c! vary psf in simulation.f
c! 

c! beamvary=1  =>  simplest -- const + sine wave + slope.
        subroutine set_varypsf(fn,beamvary,bmvaryf,bmvaryn,seed,
     /             n,m,bmpix,cdelt1,cdelt2)
        implicit none
        include "includes/constants.inc"
        integer bmvaryn,seed,n,m,i,j,nchar
        real*8 bmvaryf(bmvaryn),rand,bmpix,widx(n,m),widy(n,m)
        real*8 cdelt1,cdelt2,sigx(n,m),sigy(n,m),dumr
        character beamvary,fn*500,fn1*500

        do i=1,bmvaryn
         bmvaryf(i)=0.d0
        end do

        if (beamvary.eq.'1') then
         call ran1(seed,rand) 
         bmvaryf(3)=rand*0.25d0+0.05d0
         bmvaryf(1)=bmvaryf(3)/0.5d0
         bmvaryf(2)=bmvaryf(1)/sqrt(n*m*1.0) 
         call ran1(seed,rand) 
         bmvaryf(4)=rand*2.d0+0.5d0 
         call ran1(seed,rand) 
         bmvaryf(5)=rand*2.d0+0.5d0 
         call ran1(seed,rand) 

c        bmvaryf(1)=0.d0
c        bmvaryf(2)=1.d-4
        
         bmvaryf(3)=1.1d0*(bmpix+2.d0*bmvaryf(1)+bmvaryf(2)*m)/
     /     (bmpix+bmvaryf(2))   ! so that major > minor

        end if

        do j=1,m
         do i=1,n
          widx(i,j)=bmvaryf(3)*bmpix+bmvaryf(1)*bmvaryf(3)*
     /           (1.d0+sin(2.d0*pi*bmvaryf(4)*i/
     /           n))+bmvaryf(2)*bmvaryf(3)*i
          widy(i,j)=bmpix+bmvaryf(1)*(1.d0+sin(2.d0*pi*bmvaryf(5)*j/
     /           m))+bmvaryf(2)*j
          sigx(i,j)=widx(i,j)*cdelt1
          sigy(i,j)=widy(i,j)*cdelt2
         end do 
        end do 

        fn1=fn(1:nchar(fn))//'.bmajor'
        call writearray_bin2D(sigx,n,m,n,m,fn1,'av')
        fn1=fn(1:nchar(fn))//'.bminor'
        call writearray_bin2D(sigy,n,m,n,m,fn1,'av')
        
        return
        end




