      FUNCTION gammq(a,x)
      REAL*8 a,gammq,x
CU    USES gcf,gser
      REAL*8 gammcf,gamser,gln

      if(x.lt.0.d0.or.a.le.0.d0) then
       write (*,*) 'bad arguments in gammq'
       stop 
      end if  
      if(x.lt.a+1.d0)then
        call gser(gamser,a,x,gln)
        gammq=1.d0-gamser
      else
        call gcf(gammcf,a,x,gln)
        gammq=gammcf
      endif
      return
      END
