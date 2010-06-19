c! create image of modulation on noise by adding 3 sine waves

        subroutine cr8modulationimage(filen,n,m,seed,scratch,srldir)
        implicit none
        integer i,n,m,seed,j,nchar
        character filen*(*),strdev*5,lab*500,extn*20,f1*500
        character scratch*500,srldir*500
        real*8 modpk,rand,rad,ct(5),st(5),fac,nwav
        real*8 modl(n,m),dumr

cf2py   intent(in) filen,n,m,scratch,srldir)
cf2py   intent(in,out) seed

        rad=180.d0/3.14159d0
        strdev='/xs'

        write (*,*) 
        write (*,*) '  Now for modulating the noise image'
        write (*,'(a42,$)') ' ... Enter modulation amplitude in Jy : '
        read (*,*) modpk
        write (*,'(a35,$)') '   ... Enter typical wavenumbers : '
        read (*,*) nwav
        write (*,*) '  Using 3 waves at random angles'
c!
c! *** GENERATE  MODULATIONS  ON  NOISE  ***
c!
c         write (*,*) 'enter angle'
c         read (*,*) dumr
        do 105 i=1,3
         call ran1(seed,rand) 
         rand=rand*180.d0/rad  ! bet 0-180 degrees

c!  (@*^#$(*^@(*^$(*@$(*
c         rand=70.d0/rad
c         rand=90.d0/rad
c         rand=dumr/rad
c!  (@*^#$(*^@(*^$(*@$(*

         ct(i)=dcos(rand)
         st(i)=dsin(rand)
         fac=2*3.14159d0/(n/nwav)
105     continue
c!
c! ***  MODULATE  THE  NOISE  IMAGE  ***
c!
        do 120 j=1,m
         do 130 i=1,n
          modl(i,j)=modpk/5.d0*(
     /     dsin(ct(1)*i*fac+ st(1)*j*fac)+
     /     dsin(ct(2)*i*fac+ st(2)*j*fac)+
     /     dsin(ct(3)*i*fac+ st(3)*j*fac))
130      continue
120     continue
        call writearray_bin2D(modl,n,m,n,m,filen,'mv')

        write (*,'(a34,$)') '   Image of modulations ... <RET> '
        read (*,*)
        lab='Image of modulation on noise'
        call grey(modl,n,m,n,m,strdev,lab,0,1,'hn',1,scratch,filen,
     /       srldir)

        return
        end


