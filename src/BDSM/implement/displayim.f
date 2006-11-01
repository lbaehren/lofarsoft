c! display just one image

        subroutine displayim(f1,scratch)
        implicit none
        character f1*(*),extn*10,scratch*500
        integer n,m

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_displayim(f1,n,m,scratch)

        return
        end

        subroutine sub_displayim(f1,n,m,scratch)
        implicit none
        character f1*(*),strdev*5,lab*500,extn*10,scratch*500
        integer n,m
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        strdev='/xs'
        lab=' '
        call grey(image,n,m,n,m,strdev,lab,1,1,'hn',1,scratch)

        return
        end


