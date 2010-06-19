c! correct for primary beam

        subroutine primarybeam(f1,f2)
        implicit none
        character f1*(*),f2*(*),extn*20
        integer n,m,l

cf2py   intent(in) f1,f2

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_primarybeam(f1,f2,n,m)
        
        return
        end
c!
c!
        subroutine sub_primarybeam(f1,f2,n,m)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        character f1*(*),f2*(*),scop*7,tscope(6)*7,fn*500,extn*20
        character muldiv*1,defalt*1,ch1*1
        integer nchar,n,m,i,j,nfw,nfv,nfg,frnear
        real*8 array(n,m),dia(6),fwhm,diam,x
        real*8 freq,bw,intsec,cdelt,sigpix,dist,ncen,mcen
        real*8 pb(n,m),sigma,vlafr(8),vlaG(8,3),wsrtfr(4)
        real*8 wsrtC(4),gmrtfr(5),gmrtG(5,4),g1,g2,g3,g4

        data dia/50.d0,50.d0,45.d0,25.d0,25.d0,0.d0/
        data tscope/'LOFAR','LOFARVC','GMRT','WSRT','VLA','USER'/
        data vlafr/74.d0,327.d0,1465.d0,4885.d0,8435.d0,
     /             14965.d0,22485.d0,43315d0/
        data wsrtfr/327.25d0,608.5d0,1415.d0,4995.d0/
        data gmrtfr/153.d0,235.d0,325.d0,610.d0,1400.d0/
        data vlaG/-0.897d0,-0.935d0,-1.343d0,-1.372d0,
     /             -1.306d0,-1.305d0,-1.417d0,-1.321d0,
     /            2.71d0,3.23d0,6.579d0,6.940d0,6.253d0,
     /             6.155d0,7.332d0,6.185d0,
     /            -0.242d0,-0.378d0,-1.186d0,-1.309d0,
     /              -1.100d0,-1.030d0,-1.352d0,-0.983d0/
        data wsrtC/62.9d0,66.4d0,61.18d0,61.18d0/
        data gmrtG/-4.04d0,-3.366d0,-3.397d0,-3.486d0,-2.27961,
     /             76.2d0,46.159d0,47.192d0,47.749d0,21.4611d0,
     /             -68.8d0,-29.963d0,-30.931d0,-35.203d0,-9.7929d0,
     /             22.03d0,7.529d0,7.803d0,10.399d0,1.80153d0/
        nfw=4
        nfv=8
        nfg=5

c! read in image
        extn='.img'
        call readarray_bin(n,m,array,n,m,f1,extn)
333     write (*,'(a,$)') 
     /            '   Scope (LOFAR LOFARVC GMRT WSRT VLA USER) : '
        read (*,*) scop
        if (scop.ne.'LOFAR'.and.scop.ne.'LOFARVC'.and.scop.ne.'GMRT'
     /   .and.scop.ne.'WSRT'.and.scop.ne.'VLA'.and.scop.ne.'USER') 
     /   goto 333
        if (scop.eq.'USER') then
         write (*,'(a,$)') '   Diameter of antenna (m) : '
         read (*,*) dia(6)
        end if
        write (*,'(a,$)') '   Frequency (MHz) : '
        read (*,*) freq
        freq=freq*1.d6  ! Hz
        write (*,'(a,$)') '   CDELT ("/pix) : '
        read (*,*) cdelt
        if (scop(1:nchar(scop)).ne.'USER') then
335      write (*,'(a,$)') '   (d)efine the beam or take de(f)ault ? : '
         read (*,*) defalt
         if (defalt.ne.'m'.and.defalt.ne.'f') goto 335
        end if
334     write (*,'(a,$)') '   (m)ultiply or (d)ivide by primary beam : '
        read (*,*) muldiv
        if (muldiv.ne.'m'.and.muldiv.ne.'d') goto 334

c! get inputs
        write (*,'(3x,a14,i4,a3,i4)') 'Image size is ',n,' X ',m
        write (*,'(3x,a28,$)') 'Pointing centre in pixels : '
        read (*,*) ncen,mcen

c! get freq
        if (scop(1:nchar(scop)).eq.'WSRT') 
     /      call sub_pb_nearest(wsrtfr,nfw,freq,frnear)
        if (scop(1:nchar(scop)).eq.'VLA') 
     /      call sub_pb_nearest(vlafr,nfv,freq,frnear)
        if (scop(1:nchar(scop)).eq.'GMRT') 
     /      call sub_pb_nearest(gmrtfr,nfg,freq,frnear)

        if (scop(1:5).eq.'LOFAR') then
         if (scop(6:7).eq.'VC') then
          diam=dia(2)
         else
          diam=dia(1)
         end if
         fwhm=(c/freq)/diam*180.d0/3.14159d0  ! in deg
         sigma=fwhm/2.35d0
         sigpix=sigma*3600.d0/cdelt   ! in pixels
         do j=1,m
          do i=1,n
           dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
           pb(i,j)=dexp((-0.5d0)*((dist/sigpix)**2.d0))
          end do
         end do
        end if

        if (scop(1:3).eq.'VLA') then
         diam=dia(5)
         do j=1,m
          do i=1,n
           dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
           dist=dist*cdelt/60.d0 ! in arcmin
           x=dist*dist*freq*freq*1.d-18   ! as in x
           pb(i,j)=1.d0+x*1.d-3*vlaG(frnear,1)+x*x*1.d-7*vlaG(frnear,2)+
     /             x*x*x*1.d-10*vlaG(frnear,3)
          end do
         end do
        end if

        if (scop(1:4).eq.'GMRT') then
         diam=dia(3)
         do j=1,m
          do i=1,n
           dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
           dist=dist*cdelt/60.d0 ! in arcmin
           x=dist*dist*freq*freq*1.d-18   ! as in x
           pb(i,j)=1.d0+x*1.d-3*gmrtG(frnear,1)+x*x*1.d-7*
     /       gmrtG(frnear,2)+x*x*x*1.d-10*gmrtG(frnear,3)+
     /       x*x*x*x*1.d-13*gmrtG(frnear,4)
          end do
         end do
        end if

        if (scop(1:4).eq.'WSRT') then
         diam=dia(4)
         do j=1,m
          do i=1,n
           dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
           dist=dist*cdelt/3600.d0 ! in deg
           dist=dist*freq*1.d-9*wsrtC(frnear)   ! as in cnr
           pb(i,j)=dcos(dist)**6.d0
          end do
         end do
        end if

        if (scop(1:4).eq.'USER') then
         diam=dia(6)
337      write (*,'(a,$)') '   (p)olynomial or (g)aussian beam : '
         read (*,*) ch1 
         if (ch1.ne.'g'.and.ch1.ne.'p') goto 337
         if (ch1.eq.'g') then
          fwhm=(c/freq)/diam*180.d0/3.14159d0  ! in deg
          sigma=fwhm/2.35d0
          sigpix=sigma*3600.d0/cdelt   ! in pixels
          do j=1,m
           do i=1,n
            dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
            pb(i,j)=dexp((-0.5d0)*((dist/sigpix)**2.d0))
           end do
          end do
         else
          write (*,*) '   Polynomial beam as in VLA/GMRT '
          write (*,*) 
     /        '     (b=1+x*G1/1e3+x^2*G2/1e7+x^3*G3/1e10+x^4*G4/1e13)'
          write (*,*) '     where x is (dist in arcmin * freq in GHz)^2'
          write (*,'(a,$)') '    Enter G1..G4 coefficients : '
          read (*,*) g1,g2,g3,g4
          do j=1,m
           do i=1,n
            dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
            dist=dist*cdelt/60.d0 ! in '
            x=dist*dist*freq*freq*1.d-18   ! as in x
            pb(i,j)=1.d0+x*1.d-3*g1+x*x*1.d-7*g2+
     /        x*x*x*1.d-10*g3+ x*x*x*x*1.d-13*g4
           end do
          end do
         end if  ! ch1 is g/p
        end if

        if (muldiv.eq.'m') then
         do j=1,m
          do i=1,n
           array(i,j)=array(i,j)*pb(i,j)
          end do
         end do
        else
         do j=1,m
          do i=1,n
           array(i,j)=array(i,j)/pb(i,j)
          end do
         end do
        end if

        call writearray_bin2D(array,n,m,n,m,f2,'mv')

        return
        end
c
c      
c       frnear is index of wsrtfr array nearest to freq in value
        subroutine sub_pb_nearest(wsrtfr,nfw,freq,frnear)
        implicit none
        integer frnear,nfw,i
        real*8 wsrtfr(nfw),freq,dist

        dist=9.d99
        frnear=0
        do i=1,nfw
         if (abs(freq-wsrtfr(i)).lt.dist) then
          frnear=i
          dist=abs(freq-wsrtfr(i))
         end if
        end do 
        if (frnear.eq.0) write (*,*) ' something wrong'

        return
        end




