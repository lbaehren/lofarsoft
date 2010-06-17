c! this is a single line command line running of bdsm, to be used inside AWe, in
c! place of noise.f which is interactive. 
c! the shell script 'create' cuts out the pgplot stuff from the programs.
c! shall read input file name and directory etc from parameter file called paradefine
c!
c!                                                      ------ R. Niruj Mohan
c!                                                      ------ Sterrewacht, Leiden.
c!
c! ver f1.1.1 on July 23 2006
c!     Automatised, runs on a single command line prompt. 
c! ver f1.1.3 on July 31 2006
c!     Outputs gaussianlist as a FITS binary table.
c! ver f1.1.5 on Oct 11 2006
c!     Creates source list from gaussian list, outputs as FITS binary table.
c! ver f1.1.7 on Mar 2007
c!     Debugged, other cool stuff including FDR
c! ver f1.1.8 on May 2007
c!     WCSLIB included
c! ver f1.2 on Oct 2007
c! ver f2.0 on Jan 2008
c!     Cartesian shapelets included
c!

        subroutine fbdsm
        implicit none
        character fitsdir*500,fitsname*500,srldir*500,scratch*500
        character solnname*500,runcode*2,fullname*500,filename*500
        character extname*16,ofname*500,writeout*500,imagename*500
        character dumc*500,extn*20,comment*500,keyword*500,gausshap*500
        character version*68,plotdir*500
        integer nchar,error,i,n,m,l,l0
        real*8 dumr

        write (*,*) 
        write (*,*) '  Starting BDSM (ver f2.0)'
        version='fBDSM 2.0'
        call read_paradefine(error,fitsdir,scratch,srldir,fitsname,
     /       solnname,runcode,plotdir)

         filename=''
         if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /      fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
          filename=fitsname(1:nchar(fitsname)-5)
         else
          filename=fitsname
         end if
         
         fullname=filename(1:nchar(filename))//'.'//
     /     solnname(1:nchar(solnname))
         if (error.eq.1) goto 333
         ofname=fullname(1:nchar(fullname))//'.outputfiles'

c! reorganise naming system.
c! fitsname is redefined as name of fits file without the .fits/.FITS
c! solnname is id name of that run of bdsm-awe
c! filename is same as fitsname, is .img 
c! fullname is filename//solnname
c! imagename is name of channel collapsed image = filename//solnname//.ch0

         call callreadfits(fitsname,filename,fullname,fitsdir,
     /        scratch,runcode)
c         call cr8_wcs(scratch,filename,fullname)
         call qc_cc(filename,fullname,imagename,runcode,scratch)
         call callavspc(filename,fullname,imagename,runcode,scratch)

         call sub_bdsm_sizes(filename,imagename,n,m,l0,l)

         call preprocess(filename,fullname,imagename,runcode,scratch,
     /                   n,m,l0,l)
         call callrmsd(filename,fullname,imagename,runcode,scratch,
     /                 n,m,l0,l)
         call call_threshold(filename,fullname,imagename,runcode,
     /        scratch,n,m,l0,l)
         call formislands(filename,fullname,imagename,runcode,scratch, 
     /                    n,m,l0,l)
         call iland_mat2list(filename,fullname,imagename,
     /        runcode,scratch,n,m,l0,l)
         call sourcemeasure(filename,fullname,srldir,imagename,
     /        runcode,scratch,n,m,l0,l)

         extn='.bparms'
         keyword='gausshap'
         call get_keyword(fullname,extn,keyword,gausshap,dumr,
     /       comment,'s',scratch,error)

        call make_src_residim(fullname,imagename,runcode,srldir,
     /        n,m,l0,l,gausshap,scratch)

        call call_writeresid(fullname,scratch,runcode,fitsdir,
     /       fitsname,n,m,l0,l,gausshap)
        if (l.gt.1)    ! for ch0
     /     call callwritefits(scratch,imagename,runcode,fitsdir,
     /           fitsname,n,m,l0,l)

        if (gausshap(1:1).eq.'1') then
         call callgaul2srl(filename,srldir,imagename,scratch,fullname,
     /        n,m,l0,l)

         call callcalcspin(filename,srldir,imagename,scratch,fullname,
     /        n,m,l0,l)

         extname='BDSM_gaullist'
         call callwritegaulfits(srldir,fitsname,fullname,imagename,
     /        extname,fitsdir,version)
         extname='BDSM_srllist'
         call callwritesrlfits(srldir,fitsname,fullname,imagename,
     /        extname,fitsdir,version)
         call writeaipsfiles(srldir,fitsname,fullname,fitsdir)
        end if
        if (gausshap(2:2).eq.'1') then
         extname='BDSM_shplist'
         call callwriteshapfits(srldir,fitsname,fullname,imagename,
     /        extname,fitsdir,version)
        end if

         writeout='  List of output products in '//
     /     srldir(1:nchar(srldir))//ofname(1:nchar(ofname))
         write (*,*) writeout(1:nchar(writeout))

333      continue
         call bdsm_writeofname(srldir,ofname,fullname,imagename,
     /        fitsdir,scratch,gausshap)
         write (*,*) '  Finished BDSM'
         write (*,*) 

        return
        end


