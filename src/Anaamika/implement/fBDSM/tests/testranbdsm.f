        
                
        implicit none
        character scratch*500,srldir*500,fitsdir*500,fullname*500
        character solnname*500,fitsname*500,runcode*2,plotdir*500
        integer error,nchar
        logical ranbdsm,yesno

        call read_paradefine(error,fitsdir,scratch,srldir,fitsname,
     /       solnname,runcode,plotdir)
        write (*,*) 
        write (*,*) '  Tests if BDSM has been run or not '
        write (*,'(a,$)') '   Name of image and name of solution : '
        read (*,*) fitsname,solnname
        fullname=fitsname(1:nchar(fitsname))//'.'//
     /           solnname(1:nchar(solnname))
        
        yesno=ranbdsm(scratch,srldir,fitsdir,fullname,solnname)
        write (*,*) 
        if (yesno.eqv..true.) then
         write (*,*) '  BDSM has been run.'
        else
         write (*,*) '  BDSM has not been run.'
        end if

        return
        end


