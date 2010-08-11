c! fits 2d gaussians. simple stuff for now. fits single gaussians
c! to n regions within nsigma around it. no island etc for now. this is just to
c! compare with shapelet fitted-beta.

        subroutine callfitgaussian(f1)
        implicit none
        character f1*(*),f2*500,extn*20
        integer nchar,n,m,l
        
cf2py   intent(in) f1

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call fitgaussian(f1,n,m)

        return
        end

c! actual subroutine. done this way so i know what n,m is
        subroutine fitgaussian(f1,n,m)
        implicit none
        include "includes/constants.inc"
        integer n,m,nchar,ia(6),i,j,dof,fnum,nnum,ia4(4)
        real*8 image1(n,m),sig(n,m) 
        real*8 im1(n,m),im2(n,m)
        real*8 std,av,fwguess,a(6),cutoff
        character strdev*5,lab*500,f1*(*),ch1*1,f2*500,extn*20,sym*1
        real*4 arr4(n,m),xcur,ycur
        real*8 alamda,covar(6,6),alpha(6,6),chisq,dumr,bpa
        real*8 covar4(4,4),alpha4(4,4),a4(4)

        data ia/1,1,1,1,1,1/
        data ia4/1,1,1,1/

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        strdev='/xs'
        lab=' '
        call sigclip(image1,n,m,1,1,n,m,std,av,3.d0)
        write (*,'(1x,a19,1Pe9.2,a14,1Pe9.2,a4)') 
     /   '  3-sig clip std = ',std*1.d3,' mJy ; mean = ',av*1.d3,' mJy'

        call array8to4(image1,n,m,arr4,n,m)
        lab=' '

        call pgbegin(0,strdev,1,1)

        call pgvport(0.1,0.9,0.1,0.9)
        call basicgrey(arr4,n,m,strdev,lab)

        call pgvport(0.1,1.0,0.1,1.0)
        call pgwindow(0.0,1.0,0.0,1.0)
        call pgtext(0.1,0.97,'Click on peak pixel or '//
     /       ' anywhere else to quit')

        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(0.5,n+0.5,0.5,m+0.5)
        call pgcurs(xcur,ycur,ch1)
        if (xcur.ge.0.0.and.xcur.le.n*1.0+0.5.and.
     /      ycur.ge.0.0.and.ycur.le.m*1.0+0.5) then
         write (*,'(a,$)') '   Approximate FWHM in pixels : '
         read (*,*) fwguess
         write (*,'(a,$)') '   Cutoff in sigma : '
         read (*,*) cutoff
         cutoff=cutoff*std
333      write (*,'(a,$)') '   (f)it all or (s)ymmetric gauss : '
         read (*,*) sym
         if (sym.ne.'f'.and.sym.ne.'s') goto 333
         if (sym.eq.'f') then
          fnum=1
          nnum=6
         end if
         if (sym.eq.'s') then
          fnum=2
          nnum=4
         end if
         
         do 100 j=1,m
          do 110 i=1,n
           sig(i,j)=std
110       continue
100      continue

c! if initial guess bmaj.ne.bmin then converges else doesnt.
         a(1)=image1(int(xcur),int(ycur))
         a(2)=xcur
         a(3)=ycur
         a(4)=fwguess/fwsig
         a(5)=a(4)*0.7
         a(6)=0.d0
c         call get_gaus1_iniparm(image1,n,m,n,m,a)   ! returns initial guess
         if (a(4).eq.a(5)) a(5)=a(4)*0.7d0
         if (sym.eq.'s') then
          a4(1)=a(1)
          a4(2)=a(2)
          a4(3)=a(3)
          a4(4)=a(4)
         end if
         alamda=-1.d0

         if (sym.eq.'s') then
          do 420 i=1,20
           call mrqmin2d(image1,sig,n,a4,ia4,nnum,covar4,alpha4,nnum,
     /         chisq,alamda,fnum,cutoff)
420       continue
          alamda=0.d0
          call mrqmin2d(image1,sig,n,a4,ia4,nnum,covar4,alpha4,nnum,
     /         chisq,alamda,fnum,cutoff)
          a(1)=a4(1)
          a(2)=a4(2)
          a(3)=a4(3)
          a(4)=a4(4)
          a(5)=a4(4)
          a(6)=0.d0
         end if

         if (sym.eq.'f') then
          do 425 i=1,20
           call mrqmin2d(image1,sig,n,a,ia,nnum,covar,alpha,nnum,
     /         chisq,alamda,fnum,cutoff)
425       continue
          alamda=0.d0
          call mrqmin2d(image1,sig,n,a,ia,nnum,covar,alpha,nnum,
     /         chisq,alamda,fnum,cutoff)
         end if

         write (*,*) '  Fitted values are : '
         write (*,'(a10,1Pe10.3,a16,0Pf7.2,a1,0Pf7.2,a1)') 
     /    '   Peak = ',a(1),' Jy; Centre is (',a(2),',',a(3),')'
         write (*,'(a18,0Pf5.2,a3,0Pf7.2,a3,0Pf7.2)') 
     /    '   Fitted fwhm =  ',a(4)*2.35,' X ',a(5)*2.35,' X ',a(6)
         bpa=a(6)*3.14159d0/180.d0

         dof=0
         do 300 j=1,m
          do 310 i=1,n
           dumr=a(1)*dexp(-0.5d0*(
     /    (((i-a(2))*dcos(bpa)+(j-a(3))*dsin(bpa))/a(4))**2.d0+
     /    (((j-a(3))*dcos(bpa)-(i-a(2))*dsin(bpa))/a(5))**2.d0))
           if (dumr.le.std*0.1) dumr=0.d0
           im1(i,j)=image1(i,j)-dumr
           im2(i,j)=image1(i,j)
           if (image1(i,j).lt.cutoff) then 
            im2(i,j)=0.d0
           else
            dof=dof+1
           end if
310       continue
300      continue
         dof=dof-6-1
         write (*,*) '  Red. chi sq = ',chisq/dof,chisq,dof
         write (*,*) 

         f2='Fittedimage'
         lab='Residual'
         call writearray_bin2D(im2,n,m,n,m,f2,'mv')    ! image used for fitting
         call writearray_bin2D(im1,n,m,n,m,lab,'mv')   ! residual
         call displaytwoim(f2,lab)

        end if

        call pgend
        write (*,*) '   #### GET the rms input to mrqmin right'
        
        return
        end



