c! vary psf in simulation.f
c! 

c! beamvary=1  =>  simplest -- const + sine wave + slope.
        subroutine set_varypsf(beamvary,bmvaryf,bmvaryn,seed,n,m)
        implicit none
        integer bmvaryn,seed,n,m,i
        real*8 bmvaryf(bmvaryn),rand
        character beamvary

        do i=1,bmvaryn
         bmvaryf(i)=0.d0
        end do

        if (beamvary.eq.'1') then
         call ran1(seed,rand) 
         bmvaryf(3)=rand*0.25d0+0.05d0
         bmvaryf(1)=bmvaryf(3)/2.5d0
         bmvaryf(2)=bmvaryf(1)/sqrt(n*m*1.0) 
         call ran1(seed,rand) 
         bmvaryf(4)=rand*2.d0+0.5d0 
         call ran1(seed,rand) 
         bmvaryf(5)=rand*2.d0+0.5d0 
        end if
        
        return
        end
