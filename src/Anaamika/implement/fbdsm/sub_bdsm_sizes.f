c!

        subroutine sub_bdsm_sizes(filename,imagename,n,m,l,l1)
        implicit none
        character filename*500,imagename*500,extn*20
        integer n,m,l,n1,m1,l1

        extn='.img'
        call readarraysize(filename,extn,n,m,l)
        call readarraysize(imagename,extn,n1,m1,l1)
        if (n.ne.n1.or.m.ne.m1) write (*,*) '  # problem in sizes'

        return
        end

