c! takes image and sourcelist and makes resid image and source image.
c! CHANGE GAUL FORMAT

        subroutine make_src_residim(f2,runcode,srldir)
        implicit none
        integer n,m,nsrc,nffmt,nisl,gpi
        character f1*500,f2*500,ffmt*500,runcode*2,srldir*500

        if (runcode(2:2).eq.'q') write (*,*) '  Making residual image'
        call sourcelistheaders(f2,f1,n,m,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)
        call sub_make_src_residim(f1,f2,n,m,nsrc,nffmt,ffmt,runcode)

        return
        end

c! -----------------------------------------------------------------

        subroutine sub_make_src_residim(f1,f2,n,m,nsrc,nffmt,
     /             ffmt,runcode)
        implicit none
        include "constants.inc"
        integer n,m,nsrc,nffmt,isrc,dumi,nchar,nisl,round,i,j
        character f1*500,fn*500,extn*10,f2*500,ffmt*500,head*100
        real*8 a1,a2,a3,a4,a5,a6,chi,q,rstd,rav,sstd,sav
        real*8 image(n,m),resid(n,m),srcim(n,m),std,av,dbmaj
        real*8 level,bmaj,hsize,bpa,dumr,bmin,dbmin,dbpa,ra,dec
        real*8 cdelt(3),keyvalue,dumr1
        integer flag,srcn
        character keyword*500,keystrng*500,comment*500,runcode*2
        character dir*500,fg*500,scratch*500,srldir*500

        level=3.d0  ! go down to sigma/3 to put in source
        
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call sigclip(image,n,m,1,1,n,m,std,av,3)
        call initialiseimage(srcim,n,m,n,m,0.d0)

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir)

        extn='.header'
        keyword='CDELT1'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
333     read (22,*) head
        if (head.ne.'fmt') goto 333
        do 100 isrc=1,nsrc
         read (22,*) dumi,nisl,flag
         if (flag.eq.0) then
          backspace(22)
          read (22,*) dumi,nisl,flag,dumr,dumr1,a1,dumr,ra,dumr,dec,dumr
     /     ,a2,dumr,a3,dumr,bmaj,dumr,bmin,dumr,bpa,dumr,
     /     dbmaj,dumr,dbmin,dumr,dbpa,dumr,sstd,sav,rstd,rav,chi,q,srcn ! fw_asec
          a4=bmaj/fwsig/abs(cdelt(1)*3600.d0)
          a5=bmin/fwsig/abs(cdelt(2)*3600.d0)
          bpa=(bpa+90.d0)/rad
          hsize=sqrt(-2.d0*dlog(std/a1/level))*a4
          do i=max(round(a2-hsize),1),min(round(a2+hsize),n)
           do j=max(round(a3-hsize),1),min(round(a3+hsize),m)
            dumr=a1*dexp(-0.5d0*(
     /       (((i-a2)*dcos(bpa)+(j-a3)*dsin(bpa))/a4)**2.d0+
     /       (((j-a3)*dcos(bpa)-(i-a2)*dsin(bpa))/a5)**2.d0))
            srcim(i,j)=srcim(i,j)+dumr 
           end do
          end do
         end if  ! if flag=0
100     continue
        close(22)

        do i=1,n
         do j=1,m
          resid(i,j)=image(i,j)-srcim(i,j)
         end do
        end do
        
        fn=f2(1:nchar(f2))//'.srcim'
        call writearray_bin(srcim,n,m,n,m,fn,runcode)
        fn=f2(1:nchar(f2))//'.resid'
        call writearray_bin(resid,n,m,n,m,fn,runcode)

        return
        end

