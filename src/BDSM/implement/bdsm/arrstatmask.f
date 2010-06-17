c! calculates std and mean by computing directly. for fitting gaussian to all of data,
c! look at imstat.f. There is also sigclip.f
c! modified to take mask.

        subroutine arrstatmask(arr,mask,x,y,n1,m1,n,m,std,av,code)
        implicit none
        integer n,m,i,j,x,y,n1,m1,mask(x,y),num
        real*8 arr(x,y),av,s1,std
        character code*1

        s1=0.d0
        num=0
        do 100 j=m1,m
         do 110 i=n1,n
          if (mask(i,j).eq.1) s1=s1+arr(i,j)
          num=num+mask(i,j)
110      continue
100     continue
        av=s1/num
        if (num.eq.0) then
         if (code.eq.'v')
     /       write (*,*) '  ZERO PIXELS IN THIS SOURCE !!!!!'
         av=0.d0
        end if

        s1=0.d0
        do 200 j=m1,m
         do 210 i=n1,n
          if (mask(i,j).eq.1) s1=s1+(arr(i,j)-av)**2.d0
210      continue
200     continue
        if (num.gt.1) std=sqrt(s1/num)
        if (num.eq.1) std=0.d0
        if (num.eq.0) std=0.d0

        return
        end


