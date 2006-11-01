c! to calc P(D)

c                WRONG !!! gives tail on negative side !!! 

c        subroutine pofd(gam)
        implicit none   
        real*8 e,gam,g1,g2,g3,eta1,eta2
        real*8 k,omegae,y,w,pi,D,s
        integer i,j

        k=10.d0
        gam=2.2d0
        pi=3.14159d0
        omegae=1.d-9
        e=2.7182818d0

        call gama(gam/2.d0,g1)
        call gama((gam+1.d0)/2.d0,g2)
        call gama((2.d0-gam)/2.d0,g3)
        
        eta1=(pi**(gam+0.5d0))/(2*g1*g2*dsin(pi/2.d0*(gam-1.d0)))
        eta2=(pi**(gam-0.5d0))*g3/(2.d0*g2)
        k=1.d0!/(omegae*eta1)
        omegae=1.d0
        omegae=1.d0/(eta1*k)

       do 200 j=1,100
        D=-4.d0 + 8.d0/100.d0*j 
        s=0.d0 
         do 100 i=1,30000
          w=(i-1)*1.d-4
          y=2.d0*dexp(-k*omegae*eta1*(w**(gam-1.d0)))*
     /      dcos(k*omegae*eta2*(w**(gam-1.d0))+2.d0*pi*w*D)
          s=s+0.5*(y+y)*(1.d-4)
c         if (j.eq.60) write (*,*) w,y
100      continue
c          if (j.eq.30.or.j.eq.60) write (*,*) j,s
         write (*,*) D,s!D-k*omegae/(gam-2.d0),s
200     continue
       
        end


        
        subroutine gama(g,gammag)
        implicit none
        real*8 g,pi,gammln,dumr,e,gammag
        integer i,n
        
        pi=3.14159d0
        e=2.7182818d0

        if (g.ge.0.d0) then
         gammag=e**gammln(g)
        else
         n=abs(int(g))+1.d0
         dumr=1.d0
         do 100 i=1,n
          dumr=dumr*(g+1.d0*(i-1))
100      continue
         gammag=(e**gammln(g+n*1.d0))/dumr
        end if

        return
        end



        
