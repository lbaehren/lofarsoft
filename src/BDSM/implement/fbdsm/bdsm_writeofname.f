c!
        subroutine bdsm_writeofname(srldir,ofname,fullname,imagename,
     /             fitsdir,scratch,gausshap)
        implicit none
        character srldir*500,ofname*500,fullname*500,gausshap*500
        character fitsdir*500,scratch*500,imagename*500
        integer nchar

        open(unit=21,file=srldir(1:nchar(srldir))//
     /       ofname(1:nchar(ofname)))
         write (21,'(a)') 
     /     '  Freq collapsed image               (FITS)  = '
     /     //fitsdir(1:nchar(fitsdir))//imagename(1:nchar(imagename))//
     /     '.FITS'
         write (21,'(a)') 
     /     '  Image statistics                   (ascii) = '
     /     //scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.bstat'
         write (21,'(a)') 
     /     '  BDSM processing parameters         (ascii) = '
     /     //scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.bparms'
         if (gausshap(1:1).eq.'1') then
          write (21,'(a)') 
     /     '  Gaussian subtracted residual image (FITS)  = '
     /      //fitsdir(1:nchar(fitsdir))//fullname(1:nchar(fullname))//
     /      '.resid.gaus.FITS'
         write (21,'(a)') 
     /     '  Gaussian catalogue                 (ascii) = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /       '.gaul'
         write (21,'(a)') 
     /     '  Source catalogue                   (ascii) = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /       '.srl'
         write (21,'(a)') 
     /     '  Gaussian catalogue                 (FITS)  = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.gaul.FITS'
         write (21,'(a)') 
     /     '  Source catalogue                   (FITS)  = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.srl.FITS'
         write (21,'(a)') 
     /     '  Gaussian STAR file                 (AIPS)  = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.gaul.star'
         write (21,'(a)') 
     /     '  Source STAR file                   (AIPS)  = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.srl.star'
         write (21,'(a)') 
     /     '  Gaussian ds9 file                  (ascii) = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.gaul.reg'
         end if
         if (gausshap(2:2).eq.'1') then
          write (21,'(a)') 
     /     '  Shapelet reconstructed source image (FITS) = '
     /      //fitsdir(1:nchar(fitsdir))//fullname(1:nchar(fullname))//
     /      '.srcim.shap.FITS'
          write (21,'(a)') 
     /     '  Shapelet subtracted residual image  (FITS) = '
     /      //fitsdir(1:nchar(fitsdir))//fullname(1:nchar(fullname))//
     /      '.resid.shap.FITS'
         write (21,'(a)') 
     /     '  Shapelet parameter catalogue       (FITS)  = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.shapelet.c.FITS'
         write (21,'(a)') 
     /     '  Shapelet parameter catalogue       (ascii) = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.shapelet.c'
         write (21,'(a)') 
     /     '  Shapelet coefficients              (ascii) = '
     /     //srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.shapcoef.c'
         end if
c         write (21,'(a)') 
c     /     '  Info for debugging   (ascii) = '//
c     /     scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
c     /     '.scratch'
        close(21)
        
        return
        end



