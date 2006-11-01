c! fits 2d gaussians. simple stuff for now. fits single gaussians
c! to n regions within nsigma around it. no island etc for now. this is just to
c! compare with shapelet fitted-beta.

        subroutine callfitgaussian(f1)
        implicit none
        character f1*(*),f2*500,extn*10
        integer nchar,n,m
        
        extn='.img'
        call readarraysize(f1,extn,n,m)
        call fitgaussian(f1,n,m)

        return
        end

c! actual subroutine. done this way so i know what n,m is
        subroutine fitgaussian(f1,n,m)
        implicit none
	include "constants.inc"
        integer n,m,nchar,ia(6),i,j,dof
        real*8 image1(n,m),sig(n,m) 
        real*8 im1(n,m),im2(n,m)
        real*8 std,av,fwguess,a(6),cutoff
        character strdev*5,lab*500,f1*(*),ch1*1,f2*500,extn*10
        real*4 arr4(n,m),xcur,ycur
        real*8 alamda,covar(6,6),alpha(6,6),chisq,dumr,bpa

        data ia/1,1,1,1,1,1/

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        strdev='/xs'
        lab=' '
        call sigclip(image1,n,m,1,1,n,m,std,av,3)
        write (*,'(1x,a19,1Pe9.2,a14,1Pe9.2,a4)') 
     /   '  3-sig clip std = ',std*1.d3,' mJy ; mean = ',av*1.d3,' mJy'

        call array8to4(image1,n,m,arr4,n,m)
        lab=' '

c        call pgbegin(0,strdev,1,1)

c        call pgvport(0.1,0.9,0.1,0.9)
        call basicgrey(arr4,n,m,strdev,lab)

c        call pgvport(0.1,1.0,0.1,1.0)
c        call pgwindow(0.0,1.0,0.0,1.0)
c        call pgtext(0.1,0.97,'Click on peak pixel or '//
 

c        call pgvport(0.1,0.9,0.1,0.9)
c        call pgwindow(0.5,n+0.5,0.5,m+0.5)
c        call pgcurs(xcur,ycur,ch1)
        if (xcur.ge.0.0.and.xcur.le.n*1.0+0.5.and.
     /      ycur.ge.0.0.and.ycur.le.m*1.0+0.5) then
         write (*,'(a31,$)') '  Approximate FWHM in pixels : '
         read (*,*) fwguess
         write (*,'(a17,$)') '  Cutoff in Jy : '
         read (*,*) cutoff
         
         do 100 i=1,n
          do 110 j=1,m
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

c        call get_gaus1_iniparm(image1,n,m,n,m,a)   ! returns initial guess
        if (a(4).eq.a(5)) a(5)=a(4)*0.7d0
	write (*,*) a

         alamda=-1.d0
         do 420 i=1,20
          call mrqmin2d(image1,sig,n,a,ia,6,covar,alpha,6,
     /        chisq,alamda,1,cutoff)
	write (*,*) a,chisq
420      continue
         alamda=0.d0
         call mrqmin2d(image1,sig,n,a,ia,6,covar,alpha,6,
     /        chisq,alamda,1,cutoff)
	write (*,*) a,chisq
        a(6)=180.d0-a(6)       ! cos used clockwise rotation matrix by mistake

         write (*,*) '  Fitted values are : '
         write (*,'(a9,1Pe10.3,a16,0Pf6.2,a1,0Pf6.2,a1)') 
     /    '  Peak = ',a(1),' Jy; Centre is (',a(2),',',a(3),')'
         write (*,'(a17,0Pf5.2,a3,0Pf5.2,a3,0Pf6.2)') 
     /    '  Fitted size =  ',a(4)*2.35,' X ',a(5)*2.35,' X ',a(6)
         bpa=a(6)*3.14159d0/180.d0

         dof=0
         do 300 i=1,n
          do 310 j=1,m
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

         f2='input'
         call writearray_bin(im2,n,m,n,m,f2,'mv')
         call writearray_bin(im1,n,m,n,m,lab,'mv')
         call displaytwoim(f2,lab)

        end if

c        call pgend
        write (*,*) '   #### GET the rms input to mrqmin right'
        
        return
        end



