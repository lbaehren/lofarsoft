c! fit one gaussian. inner subroutine also used by shapelets to get beta right.
c! write a different one from callfg to make it simple.
c! fits 1 gaussian, writes out residual image and returns residual rms.

        subroutine get_totrms_gaus1(image,mask,x,y,n,m,tot,rms,
     /             sav,fname,q)
        implicit none
        include "constants.inc"
        integer x,y,n,m,i,j,mask(x,y),nchar
        real*8 image(x,y),tot,rms,a(6),chisq,av
        real*8 bpa,gaus,resid(x,y),q
        character fn*500,sav*1,fname*500,f2*500,runcode*2
        
        f2='ZZZ'
        runcode='aq'
        call writearray_bin2D(image,n,m,n,m,f2,runcode)
        call fit1gauss(image,mask,x,y,n,m,a,chisq,q)  ! returns solns a
        tot=a(1)*2.d0*pi*a(4)*a(5)
        
        bpa=a(6)*3.14159d0/180.d0
        do 100 i=1,n
         do 110 j=1,m
          gaus=a(1)*dexp(-0.5d0*(
     /    (((i-a(2))*dcos(bpa)+(j-a(3))*dsin(bpa))/a(4))**2.d0+
     /    (((j-a(3))*dcos(bpa)-(i-a(2))*dsin(bpa))/a(5))**2.d0))
          resid(i,j)=image(i,j)-gaus
110      continue
100     continue
        call arrstatmask(resid,mask,x,y,1,1,n,m,rms,av)
        
        if (sav.eq.'y') then
         fn=fname(1:nchar(fname))//'.g_resid'
         call writearray_bin2D(resid,x,y,n,m,fn,'aq')
        end if

        return
        end


        subroutine fit1gauss(image,mask,x,y,n,m,a,chisq,q)
        implicit none
        integer x,y,n,m,i,j,mask(x,y),ia(6),dof
        real*8 image(x,y),std,av,a(6),redchi
        real*8 alamda,covar(6,6),alpha(6,6),chisq
        real*8 sig(x,y),gammq,q

        data ia/1,1,1,1,1,1/
        
        call get_gaus1_iniparm_mask(image,mask,x,y,n,m,a)   ! returns initial guess
        if (a(4).eq.a(5)) a(5)=a(4)*0.7d0
        call sigclip(image,x,y,1,1,n,m,std,av,3)

        dof=0
        do 100 i=1,n
         do 110 j=1,m
          sig(i,j)=std
          dof=dof+mask(i,j)
110      continue
100     continue

        alamda=-1.d0
        do 420 i=1,20
         call mrqmin2dm(image,mask,sig,n,m,a,ia,6,covar,alpha,6,
     /       chisq,alamda,1)
420     continue
        alamda=0.d0
        call mrqmin2dm(image,mask,sig,n,m,a,ia,6,covar,alpha,6,
     /       chisq,alamda,1)
        write (*,*) 'fits = ',a
       
        return
        end


