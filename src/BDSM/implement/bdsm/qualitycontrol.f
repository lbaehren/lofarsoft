c! initial version of quality control and cross check

        implicit none
        include "constants.inc"
        character fitsdir*500,fitsname*500,srldir*500,scratch*500
        character bdsmsolnname*500,runcode*2,qcfullname*500,filename*500
        character extname*16,ofname*500,writeout*500,imagename*500
        character dumc*500,extn*20,qcsolnname*500,bdsmfullname*500
        character plotdir*500
        integer nchar,error,i,n,m,l,l0
        logical ranbdsm

        write (*,*) 
        write (*,*) '  Starting QC (ver f1.1.8)'
        call read_qcparadefine(error,fitsdir,scratch,srldir,fitsname,
     /       bdsmsolnname,qcsolnname,runcode,plotdir)

         filename=''
         if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /      fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
          filename=fitsname(1:nchar(fitsname)-5)
         else
          filename=fitsname
         end if
         
         bdsmfullname=filename(1:nchar(filename))//'.'//
     /     bdsmsolnname(1:nchar(bdsmsolnname))
         qcfullname=filename(1:nchar(filename))//'.'//
     /     qcsolnname(1:nchar(qcsolnname))
         if (error.eq.1) goto 333
         ofname=qcfullname(1:nchar(qcfullname))//'.qc.outputfiles'

c! fitsname is redefined as name of fits file without the .fits/.FITS
c! bdsmsolnname is id name of that run of bdsm
c! qcsolnname is id name of that run of qc
c! filename is same as fitsname, is .img 
c! bdsmfullname is filename//bdsmsolnname
c! qcfullname is filename//qcsolnname
c! imagename is name of channel collapsed image = filename//solnname//.ch0
 
         call callreadfits(fitsdir,fitsname,filename,bdsmfullname,
     /        scratch,runcode)
         call qc_cc(filename,bdsmfullname,imagename,runcode,scratch)
         call callavspc(filename,bdsmfullname,imagename,runcode,scratch)
         call sub_bdsm_sizes(filename,imagename,n,m,l0,l)

         if (.not.ranbdsm(scratch,srldir,fitsdir,bdsmfullname,
     /       bdsmsolnname)) then
          write (*,*) '   All BDSM products not present. '
          write (*,*) '   Run bdsm-awe again or check qcparadefine.' 
          goto 333
         end if
         call writeofname(srldir,ofname,qcfullname,imagename,
     /        fitsdir,scratch)

c         call call_qc_overlaygaussians(srldir,scratch,runcode,
c     /        filename,bdsmfullname,qcfullname,n,m,l0,l,plotdir,ofname)

         call call_qc_associatelists(scratch,srldir,runcode,filename,
     /        bdsmsolnname,qcsolnname)

         call call_qc_plot_associations(scratch,srldir,runcode,filename,
     /        bdsmsolnname,qcsolnname,plotdir,ofname)

         writeout='  List of output products in '//
     /     srldir(1:nchar(srldir))//ofname(1:nchar(ofname))
         write (*,*) writeout(1:nchar(writeout))

333      continue
         write (*,*) '  Finished QC'
         write (*,*) 

        end


        subroutine writeofname(srldir,ofname,fullname,imagename,
     /             fitsdir,scratch)
        implicit none
        character srldir*500,ofname*500,fullname*500
        character fitsdir*500,scratch*500,imagename*500
        integer nchar

        open(unit=21,file=srldir(1:nchar(srldir))//
     /       ofname(1:nchar(ofname)))
         write (21,*) '  Freq collapsed image  (FITS) = '//
     /     fitsdir(1:nchar(fitsdir))//imagename(1:nchar(imagename))//
     /     '.FITS'
         write (21,*) '  Residual image        (FITS) = '//
     /     fitsdir(1:nchar(fitsdir))//fullname(1:nchar(fullname))//
     /     '.resid.FITS'
        close(21)
        
        return
        end



