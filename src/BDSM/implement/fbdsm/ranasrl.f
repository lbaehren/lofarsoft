c! checks if bdsm has been run ... checks if various files exist. not for all.

        function ranasrl(scratch,filename1,solnname1,filename2,
     /           solnname2)
        implicit none
        character scratch*500,filename1*500,solnname1*500,filename2*500
        character solnname2*500
        character filen*500,dir*500,extn*20
        logical ranasrl,existsq
        integer nchar

        ranasrl=.true.
        filen=filename1(1:nchar(filename1))//'.'//
     /         solnname1(1:nchar(solnname1))//'__'//
     /         filename2(1:nchar(filename2))//'.'//
     /         solnname2(1:nchar(solnname2))
        dir=scratch        

        extn='.asrl'
        ranasrl=ranasrl.and.existsq(filen,dir,extn)
        extn='.assf'
        ranasrl=ranasrl.and.existsq(filen,dir,extn)
        extn='.pasf'
        ranasrl=ranasrl.and.existsq(filen,dir,extn)

        return
        end


