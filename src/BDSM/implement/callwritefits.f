c! calls fileformat in some form

        subroutine callwritefits(fname,runcode,fitsdir,fitsname)
        implicit none
        character fname*500,runcode*2,fitsdir*500,extn*10,f1*500
        integer n,m,nchar
        real*8 keyvalue
        character fg*500,keyword*500,comment*500,dir*500
        character scratch*500,fitsname*500

        f1=fname(1:nchar(fname))//'.resid'
        extn='.img'
        call readarraysize(f1,extn,n,m)
        
        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        call sub_callwritefits(f1,fitsdir,n,m,scratch,fitsname)
        
        return
        end

        subroutine sub_callwritefits(f1,fitsdir,n,m,
     /             scratch,fitsname)
        implicit none
        character fname*500,fitsdir*500,extn*10
        character f1*500,scratch*500,f2*500,fitsname*500
        integer n,m,nchar
        real*8 image(n,m)

        f2=f1(1:nchar(f1))//'.FITS'
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call writefits(image,n,m,n,m,f2,fitsdir,scratch,fitsname)

        return
        end

