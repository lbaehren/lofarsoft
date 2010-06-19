c! display just one image

        subroutine displayim(f1,scratch,srldir)
        implicit none
        character f1*500,extn*20,scratch*500,srldir*500
        integer n,m,l

cf2py   intent(in) f1,scratch,srldir

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_displayim(f1,n,m,scratch,srldir)

        return
        end

        subroutine sub_displayim(f1,n,m,scratch,srldir)
        implicit none
        character f1*(*),strdev*5,lab*500,extn*20,scratch*500,srldir*500
        integer n,m,hi
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        strdev='/xs'
        lab=' '
        hi=0
        call grey(image,n,m,n,m,strdev,lab,hi,1,'hn',1,scratch,f1,
     /       srldir)

        return
        end


