c! correct for primary beam

        subroutine primarybeam(f1,f2)
        implicit none
        character f1*(*),f2*(*),extn*10
        integer n,m

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_primarybeam(f1,f2,n,m)
        
        return
        end
c!
c!
        subroutine sub_primarybeam(f1,f2,n,m)
        implicit none
        character f1*(*),f2*(*),scop*7,tscope(5)*7,fn*500,extn*10
        integer nchar,n,m,i,j
        real*8 array(n,m),dia(5),c,fwhm,diam
        real*8 freq,bw,intsec,cdelt,sigpix,dist,ncen,mcen
        real*8 pb(n,m),sigma

        data dia/50.d0,50.d0,45.d0,25.d0,25.d0/
        data tscope/'LOFAR','LOFARVC','GMRT','WSRT','VLA'/
        c=3.d8   ! m/s

c! read in image
        extn='.img'
        call readarray_bin(n,m,array,n,m,f1,extn)
        write (*,*) '  scope freq cdelt'
        read (*,*) scop,freq,cdelt

c! get inputs
        write (*,'(3x,a14,i4,a3,i4)') 'Image size is ',n,' X ',m
        write (*,'(3x,a28,$)') 'Pointing centre in pixels : '
        read (*,*) ncen,mcen
        diam=0.d0
        do 100 i=1,5
         if (scop(1:nchar(scop)).eq.tscope(i)) diam=dia(i)
100     continue
        fwhm=(c/freq)/diam*180.d0/3.14159d0  ! in deg
        sigma=fwhm/2.35d0
        sigpix=sigma*3600.d0/cdelt   ! in pixels

        do 200 i=1,n
         do 210 j=1,m
          dist=sqrt((i-ncen)*(i-ncen)+(j-mcen)*(j-mcen))
          pb(i,j)=dexp((-0.5d0)*((dist/sigpix)**2.d0))
210      continue
200     continue

        do 300 i=1,n
         do 310 j=1,m
          array(i,j)=array(i,j)/pb(i,j)
310      continue
300     continue

        call writearray_bin(array,n,m,n,m,f2,'mv')

        return
        end



