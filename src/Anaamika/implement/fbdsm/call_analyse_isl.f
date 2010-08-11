c!
        subroutine call_analyse_isl(scratch,srldir,fullname,solnname,
     /     runcode,plotdir,gausshap,saveplots,filename,fitsdir,fitsname)
        implicit none
        character scratch*500,srldir*500,fullname*500,solnname*500
        character runcode*2,plotdir*500,gausshap*500,saveplots*1
        character filename*500,fitsdir*500,fitsname*500,fn*500,dumc*500
        integer n,m,dumi,nisl,i,nchar,maxnmax
        real*8 dumr

c! get nisl
        fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        read (21) n,m,nisl,dumi
        close(21)

        fn=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.shapelet.c'
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,19,'Max_nmax_per_island',
     /       dumc,maxnmax,dumr,'i')
        close(22)

        call sub_ai(scratch,srldir,fullname,solnname,runcode,plotdir,
     /       gausshap,saveplots,n,m,nisl,filename,fitsdir,fitsname,
     /       maxnmax)

        return
        end
c!
c!
c!
        subroutine sub_ai(scratch,srldir,fullname,solnname,runcode,
     /       plotdir,gausshap,saveplots,n,m,nisl,filename,fitsdir,
     /       fitsname,maxnmax)
        implicit none
        character scratch*500,srldir*500,fullname*500,solnname*500
        character runcode*2,plotdir*500,gausshap*500,saveplots*1
        character fn*500,extn*20,filename*500,plotd*500,fitd*500
        character fitsdir*500,fitsname*500,dumc*500,fitsd*500
        integer n,m,dumi,nisl,i,nchar,nmax(nisl),blc(2,nisl),trc(2,nisl)
        integer iisl,subn,subm,isln,maxnmax
        real*8 beta(nisl),xcen(nisl),ycen(nisl),stds(nisl),avs(nisl)
        real*8 srcim_s(n,m),image(n,m),coef(maxnmax,maxnmax,nisl)

c! read all gaussian and shapelet files as needed
        if (gausshap(1:1).eq.'1') then   ! load in .gaul and .srl -- later

        end if

        if (gausshap(2:2).eq.'1') then   ! load in srcim and shapelet info
         extn='.img'
         fn=fullname(1:nchar(fullname))//'.srcim.shap'
         call readarray_bin(n,m,srcim_s,n,m,fn,extn)
         extn='.img'
         fn=filename(1:nchar(filename))
         call readarray_bin(n,m,image,n,m,fn,extn)

         fn=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))
     /             //'.shapelet.c'
         open(unit=22,file=fn(1:nchar(fn)),form='formatted') 

         fn=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /      '.shapcoef.c'
         open(unit=23,file=fn(1:nchar(fn)),form='formatted') 
334      read (22,*) dumc
         if (dumc.ne.'fmt') goto 334
         call initialiseimage3d(coef,maxnmax,maxnmax,nisl,maxnmax,
     /        maxnmax,nisl,0.d0)
         do iisl=1,nisl
          read (22,*) dumi,nmax(iisl),beta(iisl),xcen(iisl),ycen(iisl),
     /        stds(iisl),avs(iisl),blc(1,iisl),blc(2,iisl),trc(1,iisl),
     /        trc(2,iisl)
          call readin_shapcoef(23,maxnmax,nmax(iisl),nisl,iisl,coef)
         end do
         close(22)
         close(23)
        end if
        
        plotd=plotdir(1:nchar(plotdir))//'/'//
     /       fullname(1:nchar(fullname))//'/'
        call system('rm -fr '//plotd(1:nchar(plotd)))
        call system('mkdir '//plotd(1:nchar(plotd)))
        fitsd=fitsdir(1:nchar(fitsdir))//'/'//
     /       fullname(1:nchar(fullname))//'/'
        call system('rm -fr '//fitsd(1:nchar(fitsd)))
        call system('mkdir '//fitsd(1:nchar(fitsd)))

        if (runcode(1:1).eq.'a') then
         do iisl=1,nisl
          subn=trc(1,iisl)-blc(1,iisl)+1
          subm=trc(2,iisl)-blc(2,iisl)+1
          call show_shap(scratch,fitsd,fitsdir,plotd,saveplots,
     /      fullname,iisl,beta(iisl),xcen(iisl),ycen(iisl),
     /      stds(iisl),avs(iisl),blc(1,iisl),blc(2,iisl),trc(1,iisl),
     /      trc(2,iisl),srcim_s,image,n,m,subn,subm,fitsname,nisl,
     /      maxnmax,nmax(iisl),coef)
         end do
        else
333      write (*,'(a,i5,a,$)') 
     /         '  Enter island number (1 ..',nisl,'; 0->quit) : '
         read (*,*) isln
         if (isln.ge.1.and.isln.le.nisl) then
          subn=trc(1,isln)-blc(1,isln)+1
          subm=trc(2,isln)-blc(2,isln)+1
          call show_shap(scratch,fitsd,fitsdir,plotd,saveplots,
     /     fullname,isln,beta(isln),xcen(isln),ycen(isln),stds(isln),
     /     avs(isln),blc(1,isln),blc(2,isln),trc(1,isln),
     /     trc(2,isln),srcim_s,image,n,m,subn,subm,fitsname,nisl,
     /      maxnmax,nmax(isln),coef) 
          goto 333 
         end if
        end if

        if (saveplots.eq.'y') then
         write (*,*)
         write (*,*) '  All output files in '//fitsd(1:nchar(fitsd))
         write (*,*)
        end if

        return
        end


