
      function probks(alam)
      implicit none

      real*8 probks,alam,EPS1,EPS2
      parameter (EPS1=0.001d0, EPS2=1.d-8)
      integer j
      real*8 a2,fac,term,termbf

      a2=-2.d0*alam**2
      fac=2.d0
      probks=0.d0
      termbf=0.d0
      do 11 j=1,100
        term=fac*exp(a2*j**2.d0)
        probks=probks+term
        if(abs(term).le.EPS1*termbf.or.abs(term).le.EPS2*probks)return
        fac=-fac
        termbf=abs(term)
11    continue
      probks=1.d0

      return
      END
