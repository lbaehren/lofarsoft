c! From NRP. converted to real*8.
        
        double precision function gasdev(idum)
        implicit none
        integer idum
        real*8 fac,rsq,v1,v2,ans

1       continue
        call ran1(idum,ans)
        v1=2.d0*ans-1.d0
        call ran1(idum,ans)
        v2=2.d0*ans-1.d0
        rsq=v1**2.d0+v2**2.d0
        if(rsq.ge.1.d0.or.rsq.eq.0.d0) goto 1
        fac=sqrt((-2.d0)*log(rsq)/rsq)
        gasdev=v1*fac

        return
        end

