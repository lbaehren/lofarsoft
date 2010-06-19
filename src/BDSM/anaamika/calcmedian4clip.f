c! calc median of vector after nsigma clipping. 

        subroutine calcmedian4clip(wvec,x,n1,n,med4,nsig4)
        implicit none
        integer n,i,x,n1,n2,n3,nout,niter
        real*4 wvec(x),med4,nsig4
        real*8 nsig,vec(n-n1+1),medstd,med

        nsig=nsig4
        niter=0
        n2=n-n1+1
        do i=n1,n
         vec(i-n1+1)=wvec(i)
        end do
        n3=n2
333     continue
        call c_calcmedianclip4(vec,n2,n3,med)
        call calcmedianstd(vec,n2,n3,med,medstd)
        call get_medianclip_vec(vec,n2,n3,med,medstd,
     /       nsig,nout)
        n3=n2-nout
        niter=niter+1
        if (nout.gt.0.and.niter.lt.6) goto 333
        med4=med

        return
        end

        subroutine c_calcmedianclip4(vec,x,n,med)
        implicit none
        integer n,i,x
        real*8 med,selectnr,dumr,dvec(n)
        real*8 vec(x)

        do i=1,n
         dvec(i)=vec(i)
        end do

        if (mod(n,2).eq.1) then
         med=selectnr((n+1)/2,n,dvec)
        else
         med=selectnr(n/2,n,dvec)
         dumr=selectnr(n/2+1,n,dvec)
         med=0.5d0*(med+dumr)
        end if

        return
        end

