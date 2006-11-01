c! to calculate src ct from wenss mini survey above 4.7 Jy at 330 MHz cos thats
c! the limit for roughly no source noise for lofar (see notepad)
c! trivial in octave but fucking program doesnt work after linux update.

	implicit none
	integer i
	real*8 s(100),y(100)

	do 100 i=10,100
	 s(i-9)=i*1.d0
	 y(i-9)=-7047.d0/2.35*((s(i-9)**(-2.35d0))-(4.7d0**(-2.35d0)))
         write (*,*) s(i-9),y(i-9)
100     continue

	end


