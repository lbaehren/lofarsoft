c! to populate srcs in an image
c! dont calc cn,gam for every point. cos its difficult a high flux where number << 1
c! split into 5 parts, fit power law, is within 5-7 %, use those.

        subroutine populatesrcs(f1,seed)
        implicit none
        character f1*(*),str1,tscope(5)*7
        integer scope,nband(5),bandn,i,n,m,band(5,5),seed
        real*8 freq,intsec,bw,bmax(5),dia(5),sens(5,5)
        real*8 cdelt,c,area,clnbm,sarr(500),cdfarr(500)
        real*8 cn(500),gam(500),sigma1,cn1(500),gam1(500)
        real*8 pixsc
        
        c=3.d8
        data tscope/'LOFAR','LOFARVC','GMRT','WSRT','VLA'/
        data nband/4,4,5,5,3/
        data bmax/90.d3,2.d3,30.d3,2.7d3,36.d3/
        data band/30,75,120,200,0,     ! first index is freqband 
     /            30,75,120,200,0,     ! second index is scope
     /            150,230,325,610,1400,
     /            120,170,325,610,1400,
     /            75,325,1400,0,0/
        data dia/50.d0,50.d0,45.d0,25.d0,25.d0/
        data sens/4.d-3,2.6d-3,0.14d-3,0.12d-3,0.d0,  ! rms Jy 1hr 1 Mhz
     /            9.6d-3,6.6d-3,0.34d-3,0.30d-3,0.d0,
     /            5.8d-4,2.1d-4,1.2d-4,1.1d-4,1.6d-4,
     /            0.d0,0.d0,0.d0,0.d0,0.d0,
     /            1.9d-2,9.0d-4,1.5d-4,0.d0,0.d0/

333     continue
        write (*,'(3x,a8,$)') 'Array : '
        write (*,'(3x,a52,$)') 
     /           '1. LOFAR  2. LOFAR (VC)  3. GMRT  4. WSRT  5. VLA : '
        read (*,*) scope
        if (scope.gt.5) goto 333
        write (*,'(3x,a18,$)') 'Freq band (MHz) : '
        do 100 i=1,nband(scope)-1
         write (*,'(i1,a1,i4,a3,$)') i,'.',band(i,scope),'   '
100     continue
        write (*,'(i1,a1,i4,a3,$)') i,'.',band(i,scope),' : '
        read (*,*) bandn
        if (bandn.gt.nband(scope)) goto 333
        freq=band(bandn,scope)*1.d6   ! Hz
        write (*,'(3x,a25,$)') 'Integration time (hrs) : '
        read (*,*) intsec
        intsec=intsec*3600.d0         ! secs
        write (*,'(3x,a18,$)') 'Bandwidth (MHz) : '
        read (*,*) bw
        bw=bw*1.d6                    ! Hz

        write (*,'(3x,a32,$)') 'No. pixels/fwhm of clean beam : '
        read (*,*) cdelt
        if (cdelt.lt.1.d0) goto 333
        write (*,'(3x,a15,i4,a8,$)') 'Image size (fw=',
     /           int(4.d0*bmax(scope)/dia(scope)),' pix) : '
        read (*,*) n
        if (n.lt.4.or.n.gt.9048) goto 333
        m=n
        clnbm=(c/freq)/bmax(scope)    ! fwhm of clean beam in radians
        area=(n*clnbm/cdelt)**2.d0    ! area in str
        pixsc=clnbm/cdelt*180.d0/3.14159*3600.d0  ! pixel scale in arcsec
        sigma1=sens(bandn,scope)*sqrt(3600.d0/intsec*1.d6/bw)  ! 1 sigma rms in Jy

        call getcdf(area,freq,sarr,cdfarr)
        if (sigma1.le.sarr(1).or.sigma1.gt.sarr(500)/3.d0) then
         write (*,*) '  ### Need to extrapolate number counts'
         write (*,*) '  ###           Exiting '
         write (*,*) sarr(1),sigma1,sarr(500)
         goto 334
        end if
        write (*,'(a14,1Pe9.2,a4)') '   1 sigma is ',sigma1*1.d3,' mJy'

        call fitcdf(sarr,cdfarr,cn,gam)    ! locally interpolated cn(s)*S^gam(s)
        call fitinparts(sarr,cdfarr,100,cn1,gam1) ! break into 500/nbin bits, fit each
        if (cdfarr(5).lt.100.d0) then
         write (*,*) '  ### Last bin has < 100 srcs'
         write (*,*) '  ###       Exiting '
         goto 334
        end if
        
        call populateinparts(sarr,cdfarr,100,cn1,gam1,n,m,f1,freq,
     /                       pixsc,seed)
        call writeheader(f1,n,m,tscope(scope),freq,bw,intsec,pixsc)

334     continue
        return
        end

