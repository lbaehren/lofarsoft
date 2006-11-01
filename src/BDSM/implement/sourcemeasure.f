c! first attempt at fitting etc. takes each island, finds multiple sources within
c! by some simple algo, fits gaussians. first try, low funda code.
c! bdsm ----> blob detection and source measurement
c!
c! I call a fitting routine from here, which need not be gaussians. Can also
c! call shapelet routine. 
c!

        subroutine sourcemeasure(f1,f2,srldir,runcode,scratch)
        implicit none
        character f1*500,fn*500,extn*10,f2*(*),ch1*1,runcode*2
        character srldir*500,scratch*500,writeout*500
        integer nchar,n,m,nisl,maxmem,n1,m1

        if (runcode(2:2).eq.'q') write (*,*) '  Fitting islands'
        extn='.img'
        call readarraysize(f1,extn,n,m)
        
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        read (21) n1,m1,nisl,maxmem
        if (n1.ne.n.or.m1.ne.m) write (*,*) '   #### Mismatch !!!! '
        close(21)

        call sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,scratch)
c! next step
        if (runcode(1:1).eq.'m') then
         write (*,*) 
         write (*,'(a,$)') '   Continue (residual image : resi) (y)/n ?'
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         if (ch1.eq.'y') call make_src_residim(f2,runcode,srldir)
        end if

        return  
        end

c! ---------------------------------------------------------------------------

c! blc,trc is 1 pixel out from actual island pixels. take these unmasked if < 3sig
        subroutine sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,scratch)
        implicit none
        character f1*(*),fn*500,extn*10,f2*(*),scratch*500
        integer nchar,n,m,nisl,maxmem,subsize(2)

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=23,file=fn(1:nchar(fn)),form='unformatted') 
        read (23) n,m,nisl,maxmem
        call getmax_subsize(23,nisl,maxmem,n,m,subsize) ! avoid pass full im to fit_ each time
        close(23)

        call sub_sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,subsize)

        return
        end
c!
c!
        subroutine sub_sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,subsize)
        implicit none
        integer maxs,maxs6
        parameter (maxs=200)
        parameter (maxs6=200*6)
        character f1*(*),fn*500,extn*10,f2*(*),scratch*500,fn1*500
        integer nchar,n,m,nisl,maxmem,i,j,npix_isl,nsrc,subsize(2)
        integer xpix(maxmem),ypix(maxmem),blc(2),trc(2),subn,subm
        integer delx,dely,iisl,gauperisl
        real*8 image(n,m),std,av,rstd,rav,rmsim(n,m)
        real*8 submaxim(subsize(1),subsize(2)),thresh_pix
        real*8 submaxrms(subsize(1),subsize(2)),keyvalue
        character dums*100,ffmt*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),avclip
        character ctype(3)*8,prog*20,dir*500
        logical exists
        character comment*500,keyword*500,keystrng*500,f3*500,fg*500
        character srldir*500

        prog='bdsm.f'

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,std,
     /       comment,'r',scratch)
        keyword='mean_clip'
        call get_keyword(f1,extn,keyword,keystrng,av,
     /       comment,'r',scratch)
        keyword='thresh_pix'
        call get_keyword(f1,extn,keyword,keystrng,thresh_pix,
     /       comment,'r',scratch)
        keyword='mean_clip'
        call get_keyword(f1,extn,keyword,keystrng,avclip,
     /       comment,'r',scratch)

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=25,file=fn(1:nchar(fn)),form='unformatted') 
        read (25) 

        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'    ! sourcelist==srl
        fn1=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'    ! sourcelist==srl bin
        call system('rm -f '//fn(1:nchar(fn)))
        call system('rm -f '//fn1(1:nchar(fn1)))
        open(unit=26,file=fn(1:nchar(fn)),form='formatted') 
        open(unit=27,file=fn1(1:nchar(fn1)),form='unformatted') 
        call sub_sourcemeasure_writehead(26,f1,f2,n,m,nisl,std,
     /       prog)
c!							get headers for pixel-->coord
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.header'
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        f3=f1(1:nchar(f1))//'.header'
        if (exists) call read_head_coord(f3(1:nchar(f3)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix)

        call get_rmsmap(f1,n,m,rmsim)

        nsrc=0
        gauperisl=0
        do 100 iisl=1,nisl
         read (25) j,npix_isl
         call sub_readislpix(25,npix_isl,maxmem,xpix,ypix)
         call sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpix,ypix,
     /        blc,trc,n,m)
         subn=trc(1)-blc(1)+1
         subm=trc(2)-blc(2)+1
c! now put subimage in submaxim and pass on
         delx=blc(1)-1
         dely=blc(2)-1       ! coord_im = coord_subim + (delx,dely)
c! read in image and corresponding rms map
         do i=1,subn
          do j=1,subm
           submaxim(i,j)=image(i+delx,j+dely)
           submaxrms(i,j)=rmsim(i+delx,j+dely)
          end do
         end do

         call call_fit_island(submaxim,submaxrms,subsize,f1,xpix,ypix,
     /    npix_isl,maxmem,blc,trc,av,subn,subm,rstd,rav,maxs,maxs6,
     /    26,27,iisl,nsrc,ffmt,ctype,crpix,cdelt,crval,crota,bm_pix,
     /    delx,dely,gauperisl,scratch,thresh_pix,avclip)

100     continue
        do i=25,27
         close(i)
        end do
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'    ! sourcelist==srl
        call sub_sourcemeasure_writeheadmore(nsrc,gauperisl,fn,ffmt,
     /       scratch)

        return  
        end


        subroutine sub_sourcemeasure_writehead(nn,f1,f2,n,m,nisl,
     /             std,prog)
        implicit none
        integer nn,n,m,nisl,nchar
        real*8 std,keyvalue,thresh_pix
        character f1*500,f2*(*),prog*20,extn*10,comment*500,keyword*500
        character keystrng*500,rmsmap*500,dir*500,fg*500,scratch*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='rms_map'
        call get_keyword(f1,extn,keyword,rmsmap,keyvalue,
     /       comment,'s',scratch)
        keyword='thresh_pix'
        call get_keyword(f1,extn,keyword,keystrng,thresh_pix,
     /       comment,'r',scratch)

        write (nn,'(a,a)') ' Gaussian list made by '//
     /                     prog(1:nchar(prog))
        write (nn,'(a,a)') ' Image_name ',f1(1:nchar(f1))
        write (nn,*) 'Image_size_x ',n
        write (nn,*) 'Image_size_y ',m
        write (nn,'(a,a)') ' Island_list_name ',f2(1:nchar(f2))
        write (nn,*) 'Number_of_islands ',nisl
        write (nn,'(a,a)') ' RMS_map ',rmsmap(1:nchar(rmsmap))
        write (nn,*) 'Sigma ',std
        write (nn,*) 'Detect_threshold ',thresh_pix
        write (nn,*) 'gaul# island# flag tot_Jy err peak_Jy err   RA '//
     /    'err DEC err  xpos_pix err '//
     /    'ypos_pix err bmaj_asec_fw err bmin_asec_fw err '//
     /    'bpa_deg err deconv_bmaj_bmin_bpa_asec_fw &errors '//
     /    'src_rms src_av isl_rms isl_av chisq q src#'

        return  
        end

        subroutine sub_sourcemeasure_writeheadmore(nsrc,gpi,fn,ffmt,
     /             scratch)
        implicit none
        integer nsrc,line,nchar,nffmt,gpi
        character fn*500,dums*5000,ffmt*(*),str1*10,scratch*500

        call system('rm -f '//scratch(1:nchar(scratch))//'a')
        open(unit=21,file=scratch(1:nchar(scratch))//'a')
        write (21,*) '" Number_of_gaussians ',nsrc,'"'
        close(21)
        open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
        read (21,*) dums
        close(21)
        dums="sed -e '6a\\"//dums(1:nchar(dums))//"' "//
     /       fn(1:nchar(fn))//" > "//scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn)))

        call system('rm -f '//scratch(1:nchar(scratch))//'a')
        open(unit=21,file=scratch(1:nchar(scratch))//'a')
        write (21,*) '" Max_gaussians_per_island ',gpi,'"'
        close(21)
        open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
        read (21,*) dums
        close(21)
        dums="sed -e '7a\\"//dums(1:nchar(dums))//"' "//
     /       fn(1:nchar(fn))//" > "//scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn)))

        nffmt=nchar(ffmt)
        call int2str(nffmt,str1)
        str1='fmt '//str1(1:nchar(str1))
        dums="sed -e '12a\\"//str1(1:nchar(str1))//' "'//
     /       ffmt(1:nchar(ffmt))//'" '//"' "//fn(1:nchar(fn))//" >"//
     /       scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn)))

        return  
        end
c!
c!      ==========
        subroutine call_fit_island(submaxim,submaxrms,subsize,f1,
     /     xpix,ypix,npix_isl,maxmem,blc,trc,av,subn,subm,rstd,
     /     rav,maxs,maxs6,nn,nnb,iisl,nsrc,ffmt,ctype
     /     ,crpix,cdelt,crval,crota,bm_pix,delx,dely,gauperisl,
     /     scratch,thresh_pix,avclip)
        implicit none
        integer maxs,maxs6,nn,gauperisl
        character f1*(*),fn*500,extn*10
        integer nchar,n,m,nisl,maxmem,i,j,npix_isl,nsrc,subsize(2)
        integer xpix(maxmem),ypix(maxmem),blc(2),trc(2),subn,subm
        integer delx,dely,iisl,nnb
        real*8 std,av,rstd,rav,subim(subn,subm),subrms(subn,subm)
        real*8 submaxim(subsize(1),subsize(2)),avclip
        real*8 submaxrms(subsize(1),subsize(2)),thresh_pix
        character dums*100,ffmt*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        character ctype(3)*8,prog*20,scratch*500
        logical exists

        do i=1,subn
         do j=1,subm
          subim(i,j)=submaxim(i,j)
          subrms(i,j)=submaxrms(i,j)
         end do
        end do

        call fit_island(subim,subrms,f1,xpix,ypix,npix_isl,maxmem,
     /   blc,trc,av,subn,subm,rstd,rav,maxs,maxs6,nn,nnb,iisl,nsrc,ffmt,
     /   ctype,crpix,cdelt,crval,crota,bm_pix,delx,dely,gauperisl,
     /   scratch,thresh_pix,avclip)

        return  
        end
c!
c! avoid passing full image to fit_island each time, saving time.
        subroutine getmax_subsize(nn,nisl,maxmem,n,m,subsize) 
        implicit none
        integer nn,nisl,maxmem,n,m,subsize(2),i,j,npix_isl
        integer xpix(maxmem),ypix(maxmem)
        integer blc(2),trc(2),subn,subm

        subsize(1)=0
        subsize(2)=0
        do 100 i=1,nisl
         read (nn) j,npix_isl
         call sub_readislpix(nn,npix_isl,maxmem,xpix,ypix)
         call sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpix,ypix,
     /        blc,trc,n,m)
         subn=trc(1)-blc(1)+1
         subm=trc(2)-blc(2)+1
         if (subn.gt.subsize(1)) subsize(1)=subn
         if (subm.gt.subsize(2)) subsize(2)=subm
100     continue

        return
        end

        


