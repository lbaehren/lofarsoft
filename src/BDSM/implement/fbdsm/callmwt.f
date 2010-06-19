
        subroutine callmwt(f1,scratch,runcode,fitsdir)
        implicit none
        character f1*500,scratch*500,extn*20,runcode*2,fitsdir*500
        integer n,m,l,jmax,scalf,n3,n5,opt
        parameter (n3=3)
        parameter (n5=5)
        real*8 atrous_tr(n3,n3),atrous_b3(n5,n5)

cf2py   intent(in) f1,scratch,runcode,fitsdir

        write (*,*) '   This does Multiscale Transforms.'
        write (*,*) '     1. multiscale a trous transform'
        write (*,*) '     2. multiscale median transform'
334     write (*,'(a,$)') '   Choose an option : '
        read (*,*) opt
        if (opt.lt.1.or.opt.gt.2) goto 334

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        write (*,*) '   Size of image is ',n,' X ',m
333     write (*,'(a,$)') '    Enter (isotropic) J_max : '
        read (*,*)  jmax
        if (jmax.lt.1) goto 333

        if (opt.eq.1) then
444      write (*,*) '   Scaling function for the low pass filter is '
         write (*,'(a,$)') 
     /      '     1. Triangle, 2. B3 spline, 3. B5 spline : '
         read (*,*) scalf
         if (scalf.lt.1.or.scalf.gt.3) goto 444
         if (scalf.eq.3) then
          write (*,*) '   B5 spline not yet implemented'
          goto 444
         end if
         call get_atrous_h(atrous_tr,n3,atrous_b3,n5)

         if (scalf.eq.1) call atrous(f1,n,m,jmax,scalf,n3,atrous_tr,
     /       runcode,scratch,fitsdir,opt)
         if (scalf.eq.2) call atrous(f1,n,m,jmax,scalf,n5,atrous_b3,
     /       runcode,scratch,fitsdir,opt)
        else
         if (opt.eq.2) call atrous(f1,n,m,jmax,scalf,n5,atrous_b3,
     /       runcode,scratch,fitsdir,opt)
        end if


        return
        end
c!
c!
c! hmat has the h array of size hsize^2 from atrous_h.inc
        subroutine atrous(f1,n,m,jmax,scalf,hsize,hmat,rc,scratch,
     /             fitsdir,opt)
        implicit none
        integer n,m,jmax,scalf,j,hsize,nchar,ii,jj,opt,meds
        integer mask(n,m),blankn
        real*8 nsig1,nsig
        character f1*500,extn*20,fn*10,f2*500,fname*500,f3*500
        character rc*2,fitsname*500,fitsdir*500,scratch*500
        real*8 image(n,m),hmat(hsize,hsize),std,av,stdw,avw,blankv
        real*8 c_old(n,m),c_new(n,m),w(n,m),clip_im(n,m),clsig
        logical existsq,blanky

c!  c0 is image itself.
        nsig=4.d0
        nsig1=3.d0
        clsig=1.8d0
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)  
        if (opt.eq.1) then
         f2=f1(1:nchar(f1))//'.atrous.c'
         f3=f1(1:nchar(f1))//'.atrous.w'
        else
         f2=f1(1:nchar(f1))//'.medtr.c'
         f3=f1(1:nchar(f1))//'.medtr.w'
        end if

        call copyarray(image,n,m,n,m,1,1,n,m,c_old)
        call initialiseimage(clip_im,n,m,n,m,0.d0)

c! check for blanks
        extn='.img'
        blankv=-999.d0                         ! BDSM value for magic blanking.
        call get_numpix_val(f1,extn,n,m,blankv,'eq',blankn)
        if (blankn.gt.0) then  
         blanky=.true.
        else
         blanky=.false.
         blankn=0
        end if
        if (blanky) call blank2mask(image,n,m,blankv,mask)

c! calculate W for no blank pixels
        do j=1,jmax  ! for level j
         if (blanky) then
          call sigclipmask(c_old,mask,n,m,1,1,n,m,std,av,nsig)
          meds=(2**j)+1
          call calc_atrous_cj_mask(c_old,mask,n,m,c_new,hsize,hmat,j,
     /         std,opt,meds,blankv) 
          do jj=1,m
           do ii=1,n
            w(ii,jj)=c_old(ii,jj)-c_new(ii,jj)
            if (image(ii,jj).eq.blankv) w(ii,jj)=blankv
           end do
          end do
          call sigclipmask(w,mask,n,m,1,1,n,m,stdw,avw,nsig1)
          do jj=1,m
           do ii=1,n
            if (w(ii,jj)-avw.ge.clsig*stdw) 
     /          clip_im(ii,jj)=clip_im(ii,jj)+w(ii,jj)
           end do
          end do
         else
          call sigclip(c_old,n,m,1,1,n,m,std,av,nsig)
          meds=(2**j)+1
          call calc_atrous_cj(c_old,n,m,c_new,hsize,hmat,j,std,opt,
     /         meds) 
          do jj=1,m
           do ii=1,n
            w(ii,jj)=c_old(ii,jj)-c_new(ii,jj)
           end do
          end do
          call sigclip(w,n,m,1,1,n,m,stdw,avw,nsig1)
          do jj=1,m
           do ii=1,n
            if (w(ii,jj)-avw.ge.clsig*stdw) 
     /          clip_im(ii,jj)=clip_im(ii,jj)+w(ii,jj)
           end do
          end do
         end if

         call num2str_fn(j,jmax,fn)
         fitsname=''
         extn='.fits'
         if (existsq(f1,fitsdir,extn)) fitsname=f1(1:nchar(f1))//'.fits'
         extn='.FITS'
         if (existsq(f1,fitsdir,extn)) fitsname=f1(1:nchar(f1))//'.FITS'
         fitsname=' '

cc! write c_j
         fname=f2(1:nchar(f2))//fn(1:nchar(fn))
         call writearray_bin2D(c_new,n,m,n,m,fname,rc)
         call writefits(c_new,n,m,n,m,fname,fitsdir,fitsdir,
     /        scratch,fitsname)

c! write w_j
         fname=f3(1:nchar(f2))//fn(1:nchar(fn))
         call writearray_bin2D(w,n,m,n,m,fname,rc)
         call writefits(w,n,m,n,m,fname,fitsdir,fitsdir,
     /        scratch,fitsname)
         call copyarray(c_new,n,m,n,m,1,1,n,m,c_old)
        end do    ! j
        do jj=1,m
         do ii=1,n
          clip_im(ii,jj)=clip_im(ii,jj)+c_new(ii,jj)
          if (image(ii,jj).eq.blankv) clip_im(ii,jj)=blankv
         end do
        end do

c! write smooth c_J
        fname=f2(1:nchar(f2))//'J'
        call writearray_bin2D(c_new,n,m,n,m,fname,rc)
        call writefits(c_new,n,m,n,m,fname,fitsdir,fitsdir,
     /       scratch,fitsname)

c! write clipped image
        fname=f1(1:nchar(f1))//'.atrous.clip'
        call writearray_bin2D(clip_im,n,m,n,m,fname,rc)
        call writefits(clip_im,n,m,n,m,fname,fitsdir,fitsdir,
     /       scratch,fitsname)

        return
        end



