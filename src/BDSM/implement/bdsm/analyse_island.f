c! analyse island --- gat gaussian and shapelet reconstruction

        implicit none
        integer error1,nchar
        character fitsdir*500,scratch*500,srldir*500,fitsname*500
        character solnname*500,runcode*2,plotdir*500,fullname*500
        character gausshap*500,filename*500,saveplots*1,f_ai*500
        logical ranbdsm1

        write (*,*) 
        write (*,*) '  Analysing islands '
        call read_aiparadefine(error1,fitsdir,scratch,srldir,
     /       fitsname,solnname,runcode,plotdir,gausshap,saveplots)

        filename=''
        if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /     fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
         filename=fitsname(1:nchar(fitsname)-5)
        else
         filename=fitsname
        end if
        fullname=filename(1:nchar(filename))//'.'//
     /    solnname(1:nchar(solnname))
        if (.not.ranbdsm1(scratch,srldir,fitsdir,fullname,
     /       gausshap)) then
         write (*,*) '  Run BDSM else change aiparadefine'
         stop
        end if

        call call_analyse_isl(scratch,srldir,fullname,solnname,runcode,
     /       plotdir,gausshap,saveplots,filename,fitsdir,fitsname)

        return
        end


