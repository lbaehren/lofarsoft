c! calculates std and mean by n-sigma clipping. 
c! arr of small size n,m has all the values in the box to be fit
c! for now, compute by direct computation. Maybe final call to fit as well.
c! initial guess centre should be median, width is std about median
c! Had originally converted array to vector and worked with that but 
c! crashes if array is too long so dont do that.
c! so dont use median at all as need to have prog to calc median with array
c! and not vector. do later. rt now with actual mean as initial guess it converges.
c! testsigclip shows it does converge.
c! modified dec 06 to take mask. 

        subroutine sigclipmask(arr,mask,x,y,n1,m1,n,m,std,av,nsig)
        implicit none
        integer n,m,i,j,num,x,y,ind,ind1(6),n1,m1,mask(x,y)
        real*8 arr(x,y),av,s1,std,a(3),ssq,nsig
        real*8 summ,avold,stdold,ind2(2)
       
        call arrstatmask(arr,mask,x,y,n1,m1,n,m,std,av,'v')
        call calcssqmask(arr,mask,x,y,n1,m1,n,m,ssq,num)
        std=sqrt(ssq/num-av*av)
        summ=0.d0
        do 100 j=m1,m
         do 110 i=n1,n
          if (mask(i,j).eq.1) summ=summ+arr(i,j)
110      continue
100     continue

        avold=av
        stdold=std*1.d10
        ind=0
333     continue
        do 210 j=m1,m
         do 220 i=n1,n
          if (mask(i,j).eq.1) then
           if (abs(arr(i,j)-av)/std.gt.nsig.and.
     /         abs(arr(i,j)-avold)/stdold.lt.nsig) then
            summ=summ-arr(i,j)
            num=num-1 
            ssq=ssq-arr(i,j)*arr(i,j)
           end if
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
        subroutine calcssqmask(arr,mask,x,y,n1,m1,n,m,std,num)
        implicit none
        integer i,x,y,n,m,j,n1,m1,mask(x,y),num
        real*8 arr(x,y),std,s1

        s1=0.d0
        num=0
        do 200 j=m1,m
         do 210 i=n1,n
          if (mask(i,j).eq.1) then
           s1=s1+arr(i,j)*arr(i,j)
           num=num+1
          end if
210      continue
200     continue
        std=s1

        return
        end

