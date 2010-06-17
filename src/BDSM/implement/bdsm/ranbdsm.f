c! checks if bdsm has been run ... checks if various files exist. not for all.

        function ranbdsm(scratch,srldir,fitsdir,fullname,solnname)
        implicit none
        character scratch*500,srldir*500,fitsdir*500,fullname*500
        character solnname*500,filen*500,dir*500,extn*20
        logical ranbdsm,existsq
        integer nchar,nfimg1,nfimg2,nfsrl,i
        character attachscr1(5)*500,attachsrl(10)*500,attachscr2(8)*500

        data attachscr1/'bstat','header','islandlist','snum',' '/
        data attachscr2/'ch0','mean','rank','resid.gaus','rmsd',
     /          '','',''/
        data attachsrl/'gaul','gaul.bin','gaul.FITS','gaul.star','srl',
     /          'srl.bin','srl.FITS','srl.star','',''/
        nfimg1=4
        nfimg2=5
        nfsrl=8
       
        ranbdsm=.true.

        extn=''
        dir=scratch        
        do i=1,nfimg1
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachscr1(i)(1:nchar(attachscr1(i)))
         ranbdsm=ranbdsm.and.existsq(filen,dir,extn)
        end do

        extn='.img'
        dir=scratch        
        do i=1,nfimg2
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachscr2(i)(1:nchar(attachscr2(i)))
         ranbdsm=ranbdsm.and.existsq(filen,dir,extn)
        end do

        extn=''
        dir=srldir        
        do i=1,nfsrl
         filen=fullname(1:nchar(fullname))//'.'//
     /         attachsrl(i)(1:nchar(attachsrl(i)))
         ranbdsm=ranbdsm.and.existsq(filen,dir,extn)
        end do

        return
        end


