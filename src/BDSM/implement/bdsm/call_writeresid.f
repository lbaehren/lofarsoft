
        subroutine call_writeresid(fullname,scratch,runcode,
     /             fitsdir,fitsname,n,m,l0,l,gausshap)
        implicit none
        character fname*500,runcode*2,fitsdir*500,fullname*500
        integer n,m,nchar,l,l0
        character scratch*500,fitsname*500,gausshap*500

        if (runcode(2:2).eq.'q') 
     /      write (*,*) '  Writing FITS resid images'
        if (gausshap(1:1).eq.'1') then
         fname=fullname(1:nchar(fullname))//'.resid.gaus'
         call callwritefits(scratch,fname,runcode,fitsdir,fitsname,
     /        n,m,l0,l)
        end if
        if (gausshap(2:2).eq.'1') then
         fname=fullname(1:nchar(fullname))//'.resid.shap'
         call callwritefits(scratch,fname,runcode,fitsdir,fitsname,
     /        n,m,l0,l)
         fname=fullname(1:nchar(fullname))//'.srcim.shap'
         call callwritefits(scratch,fname,runcode,fitsdir,fitsname,
     /        n,m,l0,l)
        end if

        return
        end


