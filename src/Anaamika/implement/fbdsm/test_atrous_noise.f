c! make gaussian noise images with various sizes and restoing beams and
c! calculate noise as a fn of wavelet transform order.

        implicit none
        integer n,nfw,jmax,error
        real*8 fwmin,fwmax,keyvalue
        character fg*500,extn*20,dir*500,keyword*500,comment*500
        character plotdir*500

        write (*,'(a,$)') '   Enter size of image : '
        read (*,*) n
        write (*,'(a,$)') '   Number of fwhms : '
        read (*,*) nfw
        write (*,'(a,$)') '   Min, max of convolving fwhm : '
        read (*,*) fwmin,fwmax
        write (*,'(a,$)') '   Jmax for B2 spline a trous : '
        read (*,*) jmax

        fg="paradefine"
        extn=""
        dir="./"
        keyword="plotdir"
        call get_keyword(fg,extn,keyword,plotdir,keyvalue,
     /    comment,"s",dir,error)
        
        call sub_test_atrous_noise(n,nfw,fwmin,fwmax,jmax,plotdir)

        return
        end

        subroutine sub_test_atrous_noise(n,nfw,fwmin,fwmax,jmax,
     /             plotdir)
        include "includes/constants.inc"
        integer n,nfw,seed,jmax,meds,ii,jj,mm,pp,nc
        real*8 fwmin,fwmax,im(n,n),s1,sigp(3),fac,wid,c_im(n,n)
        real*8 val,std0,av0,atrous_tr(3,3),atrous_b3(5,5),nsig
        real*8 c_old(n,n),c_new(n,n),w(n,n),std(jmax),av(jmax)
        real*4 x(jmax),y(jmax),mn,mx
        character bcon*10,bcode*4,f1*500,runcode*2,code*3,pf*500
        character plotdir*500,rc*2,str1*10,str*2
        
c! create noise map im
        s1=1.d0
        call getseed(seed)
        call xcr8noisemap(n,n,n,n,s1,im,seed)

c! start pg
        pf=plotdir(1:nchar(plotdir))//'test_atrous_noise.ps/CPS'
        call pgbegin(0,pf,1,1)
        mn=0.0
        mx=jmax*1.0
        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(mn,mx,0.0,1.0)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
        call pgslw(3)
        call pgsls(2)
        call pglabel('Order J','RMS',
     /    "rms of a trous transform of convolved noise images")
        call pgsls(1)
        call pgslw(1)

        open(unit=21,file='atrous_noise_stats')
        write (21,'(a5,a5,$)') 'FWHM','     '
        do i=1,jmax
         write (str,*) i
         write (21,'(a3,a2,a5,$)') 'J =',str,'     '
        end do
        write (21,*)

        nsig=4.d0
        do i=1,nfw+1
         if (nfw.gt.1) then
          fw=fwmin+(i-1)*(fwmax-fwmin)/(nfw-1)
         else
          fw=fwmin
         end if
         if (i.gt.nfw) fw=0.d0
         write (21,'(f5.2,a5,$)') fw,'     '


c! get a trous coeff
        call get_atrous_h(atrous_tr,3,atrous_b3,5)

c! do for each convolution -- > c_im
         sigp(1)=fw/fwsig
         sigp(2)=fw/fwsig
         sigp(3)=0.d0
         call conv2_get_wid_fac(sigp,n,n,s1,fac,wid)
         bcon='periodic'
         bcode='bare'                       
         if (i.le.nfw) 
     /    call conv2(im,n,n,n,n,sigp,c_im,bcon,bcode,fac,s1)  ! conv. im

c! get rms of orig image
         call copyarray(c_im,n,n,n,n,1,1,n,n,c_old)
         if (i.eq.nfw+1) call copyarray(im,n,n,n,n,1,1,n,n,c_old)
         call sigclip(c_old,n,n,1,1,n,n,std0,av0,nsig)

c! do a trous
         do j=1,jmax  ! for level j
          call sigclip(c_old,n,n,1,1,n,n,std,av,nsig)
          meds=(2**j)+1
          call calc_atrous_cj(c_old,n,n,c_new,5,atrous_b3,j,std,1,
     /         meds) 
          do jj=1,n
           do ii=1,n
            w(ii,jj)=c_old(ii,jj)-c_new(ii,jj)
           end do
          end do
          call sigclip(w,n,n,1,1,n,n,std(j),av(j),nsig)
          call copyarray(c_new,n,n,n,n,1,1,n,n,c_old)
          x(j)=j*1.d0
          y(j)=std(j)/std0
          write (21,'(f5.3,a5,$)') std(j)/std0,'     '
         end do  ! order j
         write (21,*)
         if (i.eq.nfw+1) then
          call pgslw(4)
          call pgsci(1)
         else
          call pgslw(1)
          call pgsci(i+1)
         end if
         call pgline(jmax,x,y)
         if (i.le.nfw) then
          call get_pgnumb(fw,mm,pp)
          call pgnumb(mm,pp,1,str1,nc)
          call pgtext(jmax*0.6,0.9-i*0.045,
     /         'fwhm='//str1(1:nc)//' pixels')
         end if
         write (*,'(a,$)') '.'

        end do   ! fw of kernel
        write (*,*)
        call get_pgnumb(n*1.0,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(jmax*0.6,0.95,'size='//str1(1:nc)//'X'//str1(1:nc))

        close(21)
        call pgend

        return
        end



