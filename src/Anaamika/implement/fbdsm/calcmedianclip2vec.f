c! calc median of vector after nsigma clipping. 
c! modify for another vector which is also clipped off in parallel

        subroutine calcmedianclip2vec(wvec,wvec2,x,n1,n,av1,med1,std1,
     /             av2,std2,med2,nsig)
        implicit none
        integer n,i,x,n1,n2,n3,nout,niter
        real*8 wvec(x),med1,nsig,vec(n-n1+1),medstd,wvec2(x)
        real*8 av1,av2,std1,std2,med2,vec2(n-n1+1)

        niter=0
        n2=n-n1+1
        do i=n1,n
         vec(i-n1+1)=wvec(i)
         vec2(i-n1+1)=wvec2(i)
        end do
        n3=n2
333     continue
        call c_calcmedianclip(vec,n2,n3,med1)
        call c_calcmedianclip(vec2,n2,n3,med2)
        call calcmedianstd(vec,n2,n3,med1,medstd)
        call vec_std(vec,n2,1,n3,av1,std1)
        call vec_std(vec2,n2,1,n3,av2,std2)
        call get_medianclip_vec2(vec,vec2,n2,n3,med1,medstd,
     /       nsig,nout)
        n3=n2-nout
        niter=niter+1
        if (nout.gt.0.and.niter.lt.6) goto 333

        return
        end

