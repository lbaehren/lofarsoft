
        subroutine qc_cc(filename,fullname,imagename,runcode,scratch)
        implicit none
        character filename*500,fullname*500,imagename*500,runcode*2
        character scratch*500,extn*20
        integer n,m,l

        write (*,*) '  Quality control and cross-check'
        extn='.img' 
        call readarraysize(filename,extn,n,m,l)
        call get_imagestats3D(filename,3.d0,n,m,l,.true.,-999.d0,
     /       scratch)

        return
        end


