c! calculates std and mean by computing directly. for fitting gaussian to all of data,
c! look at imstat.f. There is also sigclip.f

        subroutine arrstat(arr,x,y,n1,m1,n,m,std,av)
        implicit none
        integer n,m,i,j,x,y,n1,m1
        real*8 arr(x,y),av,s1,std

        s1=0.d0
        do 100 j=m1,m
         do 110 i=n1,n
          s1=s1+arr(i,j)
110      continue
100     continue
        av=s1/((n-n1+1)*(m-m1+1))

        s1=0.d0
        do 200 j=m1,m
         do 210 i=n1,n
          s1=s1+(arr(i,j)-av)**2.d0
210      continue
200     continue
        std=sqrt(s1/((n-n1+1)*(m-m1+1)))

        return
        end


