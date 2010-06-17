c! calls fileformat in some form

        subroutine callwritefits(scratch,fname,runcode,fitsdir,
     /             fitsname,n,m,l0,l)
        implicit none
        character fname*500,runcode*2,fitsdir*500,extn*20
        integer n,m,nchar,error,l,l0
        real*8 keyvalue
        character scratch*500,fitsname*500
        real*8 image(n,m)

        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        extn='.img'
        call readarray_bin(n,m,image,n,m,fname,extn)
        call writefits(image,n,m,n,m,fname,fitsdir,fitsdir,scratch,
     /       fitsname)

        return
        end

