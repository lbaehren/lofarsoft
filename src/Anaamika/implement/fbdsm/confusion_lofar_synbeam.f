c! this calculates confusion noise for LOFAR assuming the synthesized
c! beam.
c! this is done for various frequencies, various weightings, various
c! max baselines (LOFAR configurations) and averaged over values of
c! nyqfac.
c! Also a few values of gamma and k. 

        implicit none
        integer nparams,i
        parameter(nparams=34)
        real*8 beamj(nparams),freq(nparams),beamn(nparams)
        real*8 cfg(nparams),wt(nparams)
        real*8 beamasec,g,k

        data freq/15.d0, 15.d0, 15.d0, 15.d0,
     /            30.d0, 30.d0, 30.d0, 30.d0, 
     /            60.d0, 60.d0, 60.d0, 60.d0, 
     /            75.d0, 75.d0, 75.d0, 75.d0,
     /           120.d0,120.d0,120.d0,120.d0,
     /           240.d0,240.d0,240.d0,240.d0,
     /            15.d0,30.d0,45.d0,60.d0,75.d0,
     /            120.d0,150.d0,180.d0,210.d0,240.d0/
        data wt/+5.d0,0.d0,-1.d0,-5.d0,
     /           5.d0,0.d0,-1.d0,-5.d0,
     /           5.d0,0.d0,-1.d0,-5.d0,
     /           5.d0,0.d0,-1.d0,-5.d0,
     /           5.d0,0.d0,-1.d0,-5.d0,
     /           5.d0,0.d0,-1.d0,-5.d0,
     /           0.,0.,0.,0.,0.,0.,0.,0.,0.,0./
        data cfg/18.d0,18.d0,18.d0,18.d0,
     /           18.d0,18.d0,18.d0,18.d0,
     /           18.d0,18.d0,18.d0,18.d0,
     /           18.d0,18.d0,18.d0,18.d0,
     /           18.d0,18.d0,18.d0,18.d0,
     /           18.d0,18.d0,18.d0,18.d0,
     /           20.,20.,20.,20.,20.,20.,20.,20.,20.,20./
        data beamj/96.8d0,72.6d0,28.6d0,55.0d0,  ! fwhm asec
     /             48.4d0,36.3d0,14.3d0,27.5d0,
     /             24.2d0,18.2d0,7.15d0,13.75d0,
     /             19.4d0,14.5d0,5.72d0,11.0d0,
     /             12.1d0,9.07d0,3.57d0,6.875d0,
     /             6.05d0,4.54d0,1.79d0,3.44d0,
     /             206.d0,103.d0,68.d0,51.d0,41.d0,
c     /             25.d0,20.d0,17.d0,14.d0,12.d0/
     /             25.d0,7.d0,17.d0,14.d0,12.d0/
        data beamn/82.87d0,57.9d0,44.d0,24.2d0,
     /             41.43d0,28.97d0,22.d0,12.1d0,
     /             20.72d0,14.48d0,11.d0,6.05d0,
     /             16.57d0,11.59d0,8.8d0,4.84d0,
     /             10.36d0,7.24d0,5.5d0,3.025d0,
     /             5.18d0,3.62d0,2.75d0,1.51d0,
     /             206.d0,103.d0,68.d0,51.d0,41.d0,
c     /             25.d0,20.d0,17.d0,14.d0,12.d0/
     /             25.d0,7.d0,17.d0,14.d0,12.d0/

        write (*,*) 
     /      '   Calculates confusion noise for various LOFAR cfg.s'
        write (*,*) 

        g=1.84d0
        g=2.17d0
        k=277.d0
        do i=1,nparams
         beamasec=sqrt(beamj(i)*beamn(i))
         call sub_testconfusionnoise(k,g,freq(i),beamasec,wt(i))
        end do

        return
        end

        subroutine sub_testconfusionnoise(k,gam,freq,beamasec,wt)
        implicit none
        include "constants.inc"
        integer iq
        real*8 k,gam,freq,beamasec,k1,g2,beamrad,omega_e,q,sigma(3)
        real*8 wt,bmpersrc,numbeams,numsrc

        k1=k*((freq/1400.d0)**(-0.7d0))
        g2=1.d0/(gam-1.d0)
        beamrad=beamasec/3600.d0/rad
        omega_e=0.25d0*pi*beamrad*beamrad/(dlog(2.d0)*(gam-1.d0)) ! correct area
c        write (*,*) '  Omega_e = ',omega_e

        do iq=2,2
         q=iq*1.d0+2.d0
         sigma(iq)=(((q**(3.d0-gam))/(3.d0-gam))**g2)*((k1*omega_e)**g2)
         numbeams=1.d0/(0.25d0*pi*beamrad*beamrad/dlog(2.d0))
         numsrc=-k1/(1.d0-gam)*((5*sigma(iq))**(1.d0-gam))
         bmpersrc=numbeams/numsrc
        end do
        write (*,777) freq,' MHz; R = ',wt,' Noise = ',
     /              sigma(2)*1.d3,' mJy; ',bmpersrc,' bmpersrc '
777     format(2x,f5.1,a10,f4.1,a9,f8.5,a6,f8.3,a10)

        return
        end


