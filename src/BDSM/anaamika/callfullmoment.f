c! moments above 3 sigma. also mask. also add fwhm and PA from moments2d in gypsy

        subroutine callfullmoment(f1,scratch)
        implicit none
        character f1*(*),extn*20,clipmask*1,maskopt*1,maskfile*500
        character scratch*500,doinvers*1
        real*8 m1,m2(2),nsig,bmaj,bmin,bpa
        real*4 blc(2), trc(2)
        integer nchar,n,m,l,nn,mm,ll,round4
        logical exists

cf2py   intent(in) f1

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'

333     write (*,'(a,$)') '  Use (c)lipped pixels or use a (m)ask : '
        read (*,*) clipmask
        if (clipmask.ne.'c'.and.clipmask.ne.'m') goto 333

        nsig=0.d0
        maskfile=''
        if (clipmask.eq.'c') then
          write (*,'(a,$)') '  Clip above how many sigma : '
          read (*,*) nsig
        end if
        if (clipmask.eq.'m') then
444      write (*,'(a,$)') '  Use existing (f)ile or (c)reate : '
         read (*,*) maskopt
         if (maskopt.ne.'f'.and.maskopt.ne.'c') goto 444
         if (maskopt.eq.'f') then
555       write (*,'(a,$)') '  Name of mask file (name.img) : '
          read (*,*) maskfile
          extn='.img'
          if (.not.exists(maskfile,scratch,extn)) then
           write (*,*) scratch(1:nchar(scratch))//
     /       maskfile(1:nchar(maskfile))//extn(1:nchar(extn))//
     /       ' doesnt exist'
           goto 555
           call readarraysize(maskfile,extn,nn,mm,ll)
           if (n.ne.nn.or.m.ne.mm) then
            write (*,*) '  Maskfile not the same size as image.'
            goto 444
           end if
          end if
         end if
         if (maskopt.eq.'c') then
          call tvwin(f1,n,m,blc,trc)
666       write (*,'(a,$)') '  Valid pixels (i)nside/(o)utside box : '
          read (*,*) doinvers
          if (doinvers.ne.'i'.and.doinvers.ne.'o') goto 666
         end if
        end if

        call fullmoment(f1,n,m,clipmask,nsig,maskopt,maskfile,
     /       round4(blc(1)),round4(blc(2)),round4(trc(1)),
     /       round4(trc(2)),doinvers,m1,m2,bmaj,bmin,bpa,'y')

        write (*,'(5x,a11,1Pe9.2,a3)') 'moment 0 = ',m1,' Jy'
        write (*,'(5x,a11,a1,0Pf8.2,a1,0Pf8.2,a1)') 
     /   'moment 1 = ','(',m2(1),',',m2(2),')'
        write (*,'(5x,a11,0pf8.2,a1,0pf8.2,a9,0pf8.2,a4)') 
     /   'moment 2 = ',bmaj,',',bmin,' pixels; ',bpa,' deg'

        return
        end

c!
        subroutine fullmoment(f1,n,m,clipmask,nsig,
     /         maskopt,maskfile,blc1,blc2,trc1,trc2,doinvers,
     /         m1, m2, axisX, axisY, theta,plot)
        implicit none
        integer n,m,i,j,nchar,blc1,blc2,trc1,trc2,mask(n,m),outin
        character f1*500,extn*20,f2*500,clipmask*1,maskopt*1
        character maskfile*500,doinvers*1,plot*1,lab*500,strdev*5
        real*8 image(n,m),m1,m2(2),m3(3),std,av,a,b,c,p,theta
        real*8 axisX,axisY,s0,sx,sy,sxx,sxy,syy,denom,nsig,mim(n,m)
        real*4 param(5),low,up
        
        s0=0.d0
        sx=0.d0
        sy=0.d0
        sxx=0.d0
        sxy=0.d0
        syy=0.d0

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        if (clipmask.eq.'c') then
         call sigclip(image,n,m,1,1,n,m,std,av,nsig)
         do j=1,m
          do i=1,n
           if (image(i,j)-av.ge.nsig*std) then
            mask(i,j)=1
           else
            mask(i,j)=0
           end if
          end do
         end do
        else
         if (maskopt.eq.'f') then
          extn='.img'
          call readarray_bin(n,m,mim,n,m,maskfile,extn)
          call imager2int(mim,n,m,mask)
         else
          if (doinvers.eq.'i') then
           outin=1
          else
           outin=0
          end if
          call initialisemask(mask,n,m,n,m,1-outin)
          do j=1,m
           do i=1,n
            if (i.ge.blc1.and.i.le.trc1.and.j.ge.blc2
     /         .and.j.le.trc2) mask(i,j)=outin
           end do
          end do
         end if
        end if

        do 100 j=1,m
         do 110 i=1,n
          if (mask(i,j).eq.1) then
           s0=s0+image(i,j)
           sx=sx+image(i,j)*i
           sy=sy+image(i,j)*j
           sxx=sxx+image(i,j)*i*i
           syy=syy+image(i,j)*j*j
           sxy=sxy+image(i,j)*i*j
           mim(i,j)=image(i,j)*mask(i,j)
          end if
110      continue
100     continue
        m1=s0
        m2(1)=sx/m1
        m2(2)=sy/m1
        m3(1)=sxx/m1-m2(1)*m2(1)
        m3(2)=syy/m1-m2(2)*m2(2)
        m3(3)=sxy/m1-m2(1)*m2(2)

        denom=2.d0*(m3(1)*m3(2)-m3(3)*m3(3))
        a=m3(2)/denom
        b=m3(1)/denom
        c=-1.d0*m3(3)/denom
        
        if ((a*b-c*c).le.0.d0) then
         write (*,*) '  Cant make an ellipse out of the moments'
        else
         if (a.eq.b) then
          theta=0.d0
         else
          theta=0.5d0*atan(2.d0*c/(a-b))*180.d0/3.14159d0
         end if
         p=sqrt((a-b)*(a-b)+4.d0*c*c)
         if (a.gt.b) then
          axisX=2.d0*sqrt(2.d0*dlog(2.d0)/(a+b+p))
          axisY=2.d0*sqrt(2.d0*dlog(2.d0)/(a+b-p))
         else
          axisX=2.d0*sqrt(2.d0*dlog(2.d0)/(a+b-p))
          axisY=2.d0*sqrt(2.d0*dlog(2.d0)/(a+b+p))
         end if
        end if

c        f2=f1(1:nchar(f1))//'.mask'
c        call writearray_bin2d(mim,n,m,n,m,f2,'mv')

        if (plot.eq.'y') then
         strdev='/xs'
         call arr2dnz(image,n,m,n,m,low,up)
         nsig=5.d0
         call sigclip(image,n,m,1,1,n,m,std,av,nsig)
         low=max(av-10.0*std,low)
         up=min(av+10.0*std,up)
         call pgbegin(0,strdev,1,1)
         lab='Mark a window using blc, trc'
         call plotimage1(n,m,image,up,low,lab,'LG')
         param(1)=m2(1)
         param(2)=m2(2)
         param(3)=axisX
         param(4)=axisY
         param(5)=theta
         call pgdrawellipse(param(1),param(2),param(3),param(4),
     /         param(5)+90.0,1,2,1)
        end if

        return
        end
c!
        


