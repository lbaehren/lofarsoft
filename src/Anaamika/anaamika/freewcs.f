
        subroutine freewcs(wcs,wcslen)
        implicit none
        include "wcs_bdsm.inc"
        integer wcslen,wcs(wcslen),status
        
        status=wcsfree(wcs)
        
        return  
        end

