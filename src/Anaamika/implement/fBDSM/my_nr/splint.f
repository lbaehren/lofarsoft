      SUBROUTINE splint(xa,ya,y2a,n,x,y)
      implicit none
      INTEGER n
      REAL*8 x,y,xa(n),y2a(n),ya(n)
      INTEGER k,khi,klo
      REAL*8 a,b,h

      klo=1
      khi=n
1     if (khi-klo.gt.1) then
        k=(khi+klo)/2
        if(xa(k).gt.x)then
          khi=k
        else
          klo=k
        endif
      goto 1
      endif
      h=xa(khi)-xa(klo)
      if (h.eq.0.d0) then 
       write (*,*) 'bad xa input in splint'
       stop
      end if 
      a=(xa(khi)-x)/h
      b=(x-xa(klo))/h
      y=a*ya(klo)+b*ya(khi)+((a*a*a-a)*y2a(klo)+(b*b*b-b)*y2a(khi))*(h*h
     *)/6.d0
      return
      END
