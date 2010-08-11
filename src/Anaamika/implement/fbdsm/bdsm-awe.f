c! This is the automatised command line runnable version of BDSM, part of NOISE.
c! Copyright (C) 2006  LOFAR Consortium, Leiden Observatory, The Netherlands
c! 
c! This program is free software; you can redistribute it and/or
c! modify it under the terms of the GNU General Public License
c! as published by the Free Software Foundation; either version 2
c! of the License, or (at your option) any later version.
c! 
c! This program is distributed in the hope that it will be useful,
c! but WITHOUT ANY WARRANTY; without even the implied warranty of
c! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
c! GNU General Public License for more details
c!
c! You should have received a copy of the GNU General Public License
c! along with this program; if not, write to the Free Software
c! Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
c! 
c! Correspondance concerning this software should be addressed to :
c!              R. Niruj Mohan -- mohan@strw.leidenuniv.nl

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
c!
c!

        implicit none
        character fitsdir*500,fitsname*500,srldir*500,scratch*500
        character solnname*500,runcode*2,f2*500,filename*500
        character extname*16,ofname*500,writeout*500
        integer nchar,error,i

        write (*,*) 
        write (*,*) '  Starting BDSM (ver f1.1.5)'
        call read_paradefine(error,fitsdir,scratch,srldir,fitsname,
     /       solnname,runcode)

c        open(unit=32,file='wenssfiles')
c        do i=1,493
c         read (32,*) fitsname

         filename=''
         if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /      fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
          filename=fitsname(1:nchar(fitsname)-5)
         else
          filename=fitsname
         end if
         
         f2=filename(1:nchar(filename))//'.'//
     /     solnname(1:nchar(solnname))
         if (error.eq.1) goto 333
         ofname=f2(1:nchar(f2))//'.outputfiles'
 
         call callreadfits(fitsdir,fitsname,filename,scratch,runcode)
 
         call preprocess(filename,runcode,scratch)
         call callrmsd(filename,runcode,scratch)
         call formislands(filename,f2,runcode,scratch)
         call iland_mat2list(filename,f2,runcode,scratch)
         call sourcemeasure(filename,f2,srldir,runcode,scratch)
         call make_src_residim(f2,runcode,srldir)
 
         call callwritefits(f2,runcode,fitsdir,fitsname)
         call callgaul2srl(filename,srldir,scratch,f2)
 
         extname='BDSM_gaullist'
         call callwritegaulfits(srldir,fitsname,f2,extname,fitsdir)
         extname='BDSM_srllist'
         call callwritesrlfits(srldir,fitsname,f2,extname,fitsdir)
         call writeaipsfiles(srldir,fitsname,f2,fitsdir)

         writeout='  List of output products in '//
     /     srldir(1:nchar(srldir))//ofname(1:nchar(ofname))
         write (*,*) writeout(1:nchar(writeout))

333      continue
         call writeofname(srldir,ofname,f2,fitsdir)
         write (*,*) '  Finished BDSM'
         write (*,*) 
c        end do
c        close(32)

        end


        subroutine writeofname(srldir,ofname,f2,fitsdir)
        implicit none
        character srldir*500,ofname*500,f2*500,fitsdir*500
        integer nchar

        open(unit=21,file=srldir(1:nchar(srldir))//
     /       ofname(1:nchar(ofname)))
         write (21,*) '  Residual image        (FITS) = '//
     /     fitsdir(1:nchar(fitsdir))//f2(1:nchar(f2))//'.resid.FITS'
         write (21,*) '  Gaussian catalogue   (ascii) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'    
         write (21,*) '  Source catalogue     (ascii) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'    
         write (21,*) '  Gaussian catalogue    (FITS) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.FITS'
         write (21,*) '  Source catalogue      (FITS) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.FITS'
         write (21,*) '  Gaussian STAR file    (AIPS) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.star'
         write (21,*) '  Source STAR file      (AIPS) = '//
     /     srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.star'
        close(21)
        
        return
        end


