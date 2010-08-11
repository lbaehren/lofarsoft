c! does wcs here and writes wcs array out cos i have lots of memory
c! problems if i do wcsini often

        subroutine cr8_wcs(scratch,filename,fullname)
        implicit none
        character scratch*500,filename*500,fullname*500,fn*500
        character ctype(3)*8
        integer wcslen,nchar
        parameter (wcslen=450)
        integer wcs(wcslen)
        logical exists
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)

        fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /      '.header'
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        fn=filename(1:nchar(filename))//'.header'
        if (exists) call read_head_coord(fn(1:nchar(fn)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix,scratch)

        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,
     /    wcslen)
        
        fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.wcs'
        open(unit=21,file=fn,form='unformatted')
         write (21) wcslen
         write (21) wcs
        close(21)

        call freewcs(wcs,wcslen)

        return
        end


