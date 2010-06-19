c! plots sourcelist as well as islands over image (orig or resid)
c! CHANGE GAUL FORMAT

        subroutine plotisland(f2)
        implicit none
        integer n,m,nchar,nsrc,nffmt,nisl,gpi,error
        real*8 keyvalue
        character f1*500,f2*500,ffmt*500
        character extn*20,keyword*500,fg*500,scratch*500
        character comment*500,dir*500,srldir*500

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir,error)

        call sourcelistheaders(f2,f1,n,m,nisl,nsrc,gpi,nffmt,ffmt,
     /       srldir)
        call sub_plotisland(f1,f2,n,m,nisl,nsrc,nffmt,ffmt)

        return
        end

        subroutine sub_plotisland(f1,f2,n,m,nisl,nsrc,nffmt,ffmt)
        implicit none
        include "constants.inc"
        integer n,m,nchar,nsrc,nffmt,nisl,isrc,mask(n,m),i,j
        integer xic(nisl),yic(nisl),nc,iisl,flag(nsrc),error
        character f1*500,fn*500,extn*20,f2*500,ffmt*500,str1*1,fg*500
        character scratch*500
        real*8 image(n,m),xsrc(nsrc),ysrc(nsrc)
        character strdev*5,lab*500,str3*3
        real*4 arr4(n,m),x4(nsrc),y4(nsrc)
        real*8 bmaj(nsrc),bmin(nsrc),bpa(nsrc),keyvalue,cdelt(3)
        real*4 bmaj4(nsrc),bmin4(nsrc),bpa4(nsrc)
        character keyword*500,keystrng*500,comment*500,dir*500
        character srldir*500

        write (*,*)
333     write (*,'(a,$)') '   Display image or resid (i/r) ? '
        read (*,*) str1
        write (*,*) '  Reading the image ... '
        extn='.img'
        if (str1.ne.'i'.and.str1.ne.'r') goto 333
        if (str1.eq.'i') fn=f1
        if (str1.eq.'r') fn=f2(1:nchar(f2))//'.resid'
        call readarray_bin(n,m,image,n,m,fn,extn)

        call array8to4(image,n,m,arr4,n,m)
        lab=' '
        strdev='/xs'
        call pgbegin(0,strdev,1,1)
        call pgvport(0.1,0.9,0.1,0.9)
        call basicgrey(arr4,n,m,strdev,lab)
        
        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir,error)

        extn='.header'
        keyword='CDELT1'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)
334     write (*,'(a,$)') '   Overplot (s)ources/(i)slands/(q)uit ? '
        read (*,*) str1
        if (str1.ne.'i'.and.str1.ne.'s'.and.str1.ne.'q') goto 333
        if (str1.eq.'q') goto 444
        if (str1.eq.'s') then
         write (*,*) '  Plotting ',nsrc,' sources ... '
         call readgaul_getposn(f2,nsrc,xsrc,ysrc,flag,bmaj,bmin,bpa,
     /        srldir)
         call vec8to4(xsrc,nsrc,x4,nsrc)
         call vec8to4(ysrc,nsrc,y4,nsrc)
         call vec8to4(bmaj,nsrc,bmaj4,nsrc)
         call vec8to4(bmin,nsrc,bmin4,nsrc)
         call vec8to4(bpa,nsrc,bpa4,nsrc)
         call pgsci(2)
         call pgpoint(nsrc,x4,y4,2) 
         call pgsci(1)
         do i=1,nsrc  ! plot fwhm ellipse
          if (flag(i).eq.0) then
           call pgdrawellipse(x4(i),y4(i),bmaj4(i)/abs(cdelt(1)),
     /         bmin4(i)/abs(cdelt(2)),bpa4(i),3,0)
          end if
         end do
        end if
        if (str1.eq.'i') then
         write (*,*) '  Plotting ',nisl,' islands ... '
         call readgaul_getisl(f2,nisl,mask,n,m)
         call pgsci(3)
         do j=1,m
          do i=1,n
           if (mask(i,j).eq.1) call pgpoint(1,i*1.0,j*1.0,2)
          end do
         end do
         call pgsci(1)
         call readisllist_getisln(f2,nisl,xic,yic,scratch)
         call pgsci(0)
         do iisl=1,nisl
          call pgnumb(iisl,0,1,str3,nc)
          call pgtext(xic(iisl)*1.0-0.5,yic(iisl)*1.0-0.5,str3)
         end do
         call pgsci(1)
        end if
        
444     continue
        call pgend

        return
        end



        subroutine readgaul_getposn(f2,nsrc,xsrc,ysrc,flag,bmaj,bmin,
     /             bpa,srldir)
        implicit none
        character f2*500,fn*500,head*500,srldir*500
        integer nsrc,nchar,isrc,dumi,flag(nsrc)
        real*8 xsrc(nsrc),ysrc(nsrc),dumr
        real*8 bmaj(nsrc),bmin(nsrc),bpa(nsrc)

        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
333     read (22,*) head
        if (head.ne.'fmt') goto 333
        do 100 isrc=1,nsrc
         read (22,*) dumi,dumi,flag(isrc)
         if (flag(isrc).eq.0) then
          backspace(22)
          read (22,*) dumi,dumi,flag(isrc),dumr,dumr,dumr,dumr,
     /     dumr,dumr,dumr,dumr,xsrc(isrc),dumr,ysrc(isrc),
     /     dumr,bmaj(isrc),dumr,bmin(isrc),dumr,bpa(isrc)
         end if
100     continue
        close(22)
        
        return
        end

        subroutine readgaul_getisl(f2,nisl,mask,n,m)
        implicit none
        character f2*500,fn*500,extn*20,str*10
        integer nisl,n,m,mask(n,m),nchar,mask1(n,m),i,j
        integer ndigit,fac,xsurr(8),ysurr(8),dumi,dumi1,i8
        logical pixinimage

        extn='.nmg'
        fn=f2(1:nchar(f2))//'.rank'
        call readarray_bin_int(n,m,mask1,n,m,fn,extn,str)

        do j=1,m
         do i=1,n
          if (mask1(i,j).ne.0) then 
           mask1(i,j)=1 
          end if
         end do
        end do

        ndigit=int(log10(max(n,m)*1.0))+1      
        fac=int(10**ndigit)                    
        do j=1,m
         do i=1,n 
          if (mask1(i,j).eq.1) then
           call getsurr_8pix(j*fac+i,xsurr,ysurr,fac)
           dumi=0
           dumi1=0
           do i8=1,8
            if (pixinimage(xsurr(i8),ysurr(i8),1,n,1,m)) then 
             dumi=dumi+1
             dumi1=dumi1+mask1(xsurr(i8),ysurr(i8))
            end if
           end do
           if (dumi-dumi1.lt.2) mask(i,j)=0 
           if (dumi-dumi1.ge.2) mask(i,j)=1
           if (dumi-dumi1.lt.0) write (*,*) ' $$$$ AAAHHH!H!!!!'
          else
           mask(i,j)=0
          end if
         end do
        end do

        return
        end

        subroutine readisllist_getisln(f2,nisl,xic,yic,scratch)
        implicit none
        character f2*500,fn*500,scratch*500
        integer nisl,xic(nisl),yic(nisl),nchar,i,dumi,ii
        real*8 dumr

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//
     /     '.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted')
        read (21)
        do i=1,nisl
         read (21) ii,dumi,xic(i),yic(i),dumr
         read (21)
         if (ii.ne.i) write (*,*) '  AIYAIYO !! ERROR !!'
        end do
        close(21)

        return
        end




