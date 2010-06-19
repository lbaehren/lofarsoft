c To calculate luminosity distance for arbit cosmology (nonzero lambda).
c Taken from Ue-Li Pen.


        implicit none
	real*8 Ho,red,omega,c,c1,c2,x,x2_8,x2_9,a,s,u
	real*8 eta(2),lumd,lambda,luma
	integer i

        lambda=0.7d0
        Ho=65.d0
        omega=1.d0-lambda

	write (*,*) ' Enter z'
	read (*,*) red

	c=2.99d5
	c1=-0.1540d0
	c2=0.4304d0
	x=0.3566d0
	x2_8=0.0669d0
	x2_9=0.0477d0

        if (omega.lt.0.999) then
         do 100 i=1,2
	  if (i.eq.1) then
	    a=1.d0
          else
	    a=1.d0/(1.d0+red)
	  end if  
 	  s=((1.d0-omega)/omega)**(0.3333d0)
 	  u=a*s
          eta(i)=2.d0*sqrt((s*s*s+1)/s)*(((u**(-4.d0))+c1/(u*u*u)+
     /       c2/(u*u)+4.d0/u*x2_9+x2_8)**(-1.d0/8.d0) )
100	 continue
	 lumd=c/Ho*(1.d0+red)*(eta(1)-eta(2))
        else
	 lumd=2.d0*c/Ho*(red+(1.d0-sqrt(1+red)))
        end if

        lumd=lumd !  Mpc
        luma=lumd/((1.d0+red)*(1.d0+red))

	write (*,*) ' Angular diameter distance is ',luma,' Mpc'

	end

