c! checks if bdsm has been run ... checks if various files exist. not for all.
c! modified for gausshap

        function ranbdsm1(scratch,srldir,fitsdir,fullname,gausshap)
        implicit none
        character scratch*500,srldir*500,fitsdir*500,fullname*500
        character filen*500,dir*500,extn*20,gausshap*500
        logical ranbdsm1,existsq
        integer nchar,nfimg1,nfimg2,nfsrl,i,nfshap,nfimg2g,nfimg2s
        character attachscr1(5)*500,attachsrl(10)*500,attachscr2(8)*500
        character attachscr2g(8)*500,attachscr2s(8)*500
        character attachshap(10)*500

        data attachscr1/'bstat','header','islandlist','snum',' '/
        data attachscr2/'ch0','mean','rank','rmsd','',
     /          '','',''/
        data attachscr2g/'resid.gaus','','','','',
     /          '','',''/
        data attachscr2s/'resid.shap','','','','',
     /          '','',''/
        data attachsrl/'gaul','gaul.bin','gaul.FITS','gaul.star','srl',
     /          'srl.bin','srl.FITS','srl.star','',''/
        data attachshap/'shapcoef.c','shapelet.c','','','',
     /          '','','','',''/
        nfimg1=4
        nfimg2=4
        nfimg2g=1
        nfimg2s=1
        nfsrl=8
        nfshap=2
       
        ranbdsm1=.true.

        extn=''
        dir=scratch        
        do i=1,nfimg1
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachscr1(i)(1:nchar(attachscr1(i)))
         ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
        end do

        extn='.img'
        dir=scratch        
        do i=1,nfimg2
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachscr2(i)(1:nchar(attachscr2(i)))
         ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
        end do

        extn=''
        dir=srldir        
        do i=1,nfsrl
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachsrl(i)(1:nchar(attachsrl(i)))
         ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
        end do

        if (gausshap(1:1).eq.'1') then
         extn='.img'
         dir=scratch        
         do i=1,nfimg2g
          filen=fullname(1:nchar(fullname))//'.'//
     /          attachscr2g(i)(1:nchar(attachscr2g(i)))
          ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
         end do
        end if

        if (gausshap(2:2).eq.'1') then
         extn='.img'
         dir=scratch        
         do i=1,nfimg2s
          filen=fullname(1:nchar(fullname))//'.'//
     /          attachscr2s(i)(1:nchar(attachscr2s(i)))
          ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
         end do

         extn=''
         dir=scratch        
         do i=1,nfshap
          filen=fullname(1:nchar(fullname))//'.'//
     /          attachshap(i)(1:nchar(attachshap(i)))
          ranbdsm1=ranbdsm1.and.existsq(filen,dir,extn)
         end do
        end if

        return
        end


