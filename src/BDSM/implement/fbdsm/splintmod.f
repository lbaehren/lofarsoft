
      SUBROUTINE splintmod(xal,xah,yal,yah,y2al,y2ah,x,y)
      implicit none
      REAL*8 x,y,xal,xah,y2al,y2ah,yal,yah
      INTEGER k,khi,klo
      REAL*8 a,b,h

      h=xah-xal
      if (h.eq.0.d0) then 
       write (*,*) 'bad xa input in splint'
       stop 
      end if 
      a=(xah-x)/h
      b=(x-xal)/h
      y=a*yal+b*yah+((a*a*a-a)*y2al+(b*b*b-b)*y2ah)*(h*h
     *)/6.d0
      return
      END
