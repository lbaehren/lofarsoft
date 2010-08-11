c! subroutines for simulation.f 

        subroutine getsurveypara(area,time,bw,nchan,freq,bl,fn,
     /             dia,beamvary)
        implicit none
        include "constants.inc"
        real*8 area,time,bw,freq,bl,prbm,dia
        integer nchan,dumi
        character fn*500,beamvary,getchar

        write (*,*)
        write (*,'(a13,$)') '   File id : '
        read (*,*) fn
444     continue
        write (*,'(a27,$)') '   Maximum baseline (km) : '
        read (*,*) bl
        bl=bl*1.d3
        if (bl.le.1.d2.or.bl.ge.500.d3) goto 444
445     continue
        write (*,'(a26,$)') '   Antenna diameter (m) : '
        read (*,*) dia
        if (dia.le.1.d0.or.dia.ge.100.d0) goto 445
        write (*,'(a36,$)') '   Integration time/pointing (hr) : '
        read (*,*) time
        time=time*3600.d0
        write (*,'(a21,$)') '   Bandwidth (MHz) : '
        read (*,*) bw
        bw=bw*1.d6
        write (*,'(a21,$)') '   No. of channels : '
        read (*,*) nchan
333     continue
        write (*,'(a21,$)') '   Frequency (MHz) : '
        read (*,*) freq
        freq=freq*1.d6
        if (freq.gt.270.d6.or.freq.le.8.d6) goto 333
        prbm=(c/freq/dia*rad)**2.d0
        write (*,'(a25,f5.1,a7)') '   Primary beam (fwhm) = ',
     /         prbm,' sq deg'
        write (*,'(a24)') '   Survey area (sq deg) '
        write (*,'(a29,$)') '   (Not more than 2 beams) : '
        read (*,*) area
        area=area/(rad*rad)
        write (*,'(a39,$)') '   Constant beam (4) or vary (1/2/3) : '
        read (*,*) dumi
        if (dumi.eq.4) then
         beamvary='n'
        else
         beamvary=getchar(dumi)
        end if
        
        return
        end


! returns sens in Jy (1 hr 1 MHz) for freq in MHz. Add 3rd or 4th for other arrays.
        subroutine getsens(freq,sens)
        implicit none
        integer i,array
        real*8 freq,sens
        real*8 f(6,2),sen(6,2),a,b
        data f/15.d6,30.d6,60.d6,75.d6,120.d6,200.d6,           ! Hz lofar
     /         150.d6,240.d6,330.d6,610.d6,1420.d6,1700.d6/     !    gmrt
        data sen/11.d-3,4.d-3,3.d0,2.6d-3,0.14d-3,0.12d-3,      ! Jy/hr/MHz lofar
     /           0.d0,0.01114d0,0.d0,0.d0,0.d0,0.d0/             !    gmrt

333     write (*,'(a,$)') '   Array (LOFAR=1, GMRT=2) : '
        read (*,*) array
        if (array.ne.1.and.array.ne.2) goto 333

        sens=0.d0
        do i=1,6
         if (freq.eq.f(i,array)) sens=sen(i,array)
        end do
        if (sens.eq.0.d0) then
         do i=1,5
          if (freq.gt.f(i,array).and.freq.lt.f(i+1,array)) then
           a=(sen(i,array)-sen(i+1,array))/(f(i,array)-f(i+1,array)) 
           b=(sen(i+1,array)*f(i,array)-sen(i,array)*f(i+1,array))/
     /        (f(i,array)-f(i+1,array)) 
           sens=a*freq+b
          end if
         end do
        end if
        if (freq.gt.f(6,array)) sens=sen(6,array)
        if (freq.lt.f(1,array)) sens=sen(1,array)

        return
        end


