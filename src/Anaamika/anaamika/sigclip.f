c! calculates std and mean by n-sigma clipping. 
c! arr of small size n,m has all the values in the box to be fit
c! for now, compute by direct computation. Maybe final call to fit as well.
c! initial guess centre should be median, width is std about median
c! Had originally converted array to vector and worked with that but 
c! crashes if array is too long so dont do that.
c! so dont use median at all as need to have prog to calc median with array
c! and not vector. do later. rt now with actual mean as initial guess it converges.
c! testsigclip shows it does converge.

        subroutine sigclip(arr,x,y,n1,m1,n,m,std,av,nsig)
        implicit none
        integer n,m,i,j,num,x,y,ind,ind1(6),n1,m1
        real*8 arr(x,y),av,s1,std,a(3),ssq,nsig
        real*8 summ,avold,stdold,ind2(2)
       
        call arrstat(arr,x,y,n1,m1,n,m,std,av)
        call calcssq(arr,x,y,n1,m1,n,m,ssq)
        std=sqrt(ssq/((n-n1+1)*(m-m1+1))-av*av)
        summ=0.d0
        do 100 j=m1,m
         do 110 i=n1,n
          summ=summ+arr(i,j)
110      continue
100     continue
        num=(n-n1+1)*(m-m1+1)

        avold=av
        stdold=std*1.d10
        ind=0
333     continue
        do 210 j=m1,m
         do 220 i=n1,n
          if (abs(arr(i,j)-av)*1.d0/std.gt.nsig.and.
     /        abs(arr(i,j)-avold)*1.d0/stdold.lt.nsig) then
           summ=summ-arr(i,j)
           num=num-1 
           ssq=ssq-arr(i,j)*arr(i,j)
          end if
220      continue
210     continue
        avold=av
        stdold=std
        av=summ/num
        std=sqrt(ssq/num-av*av)
        ind=ind+1
        if (ind.gt.100) then
         write (*,*) '  ### Sigma-clip not converging'
         write (*,*) '  ###        Exiting .. '
         goto 444
        end if
        if (std.ne.stdold) goto 333

444     continue

        return
        end


c! calculates the sum of squares of array arr
        subroutine calcssq(arr,x,y,n1,m1,n,m,std)
        implicit none
        integer i,x,y,n,m,j,n1,m1
        real*8 arr(x,y),std,s1

        s1=0.d0
        do 200 j=m1,m
         do 210 i=n1,n
          s1=s1+arr(i,j)*arr(i,j)
210      continue
200     continue
        std=s1

        return
        end

