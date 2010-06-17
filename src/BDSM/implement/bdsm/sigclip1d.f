c! calculates std and mean by n-sigma clipping. 
c! arr of small size n,m has all the values in the box to be fit
c! for now, compute by direct computation. Maybe final call to fit as well.
c! initial guess centre should be median, width is std about median
c! Had originally converted array to vector and worked with that but 
c! crashes if array is too long so dont do that.
c! so dont use median at all as need to have prog to calc median with array
c! and not vector. do later. rt now with actual mean as initial guess it converges.
c! testsigclip shows it does converge.

c! stupid, but modifying this for 1d

        subroutine sigclip1d(vec,x,n1,n,std,av,nsig)
        implicit none
        integer n,i,num,x,ind,n1,m1
        real*8 vec(x),av,s1,std,ssq
        real*8 summ,avold,stdold,nsig
       
        call vec_std(vec,x,1,n,av,std)
        call calcssq1d(vec,x,n1,n,ssq)
        std=sqrt(ssq/(n-n1+1)-av*av)
        summ=0.d0
        do 110 i=n1,n
         summ=summ+vec(i)
110     continue
        num=(n-n1+1)

        avold=av
        stdold=std*1.d10
        ind=0
333     continue
        do 220 i=n1,n
         if (abs(vec(i)-av)/std.gt.nsig.and.
     /       abs(vec(i)-avold)/stdold.lt.nsig) then
          summ=summ-vec(i)
          num=num-1 
          ssq=ssq-vec(i)*vec(i)
         end if
220     continue
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


c! calculates the sum of squares of vec
        subroutine calcssq1d(vec,x,n1,n,std)
        implicit none
        integer i,x,n,j,n1
        real*8 vec(x),std,s1

        s1=0.d0
        do 210 i=n1,n
         s1=s1+vec(i)*vec(i)
210     continue
        std=s1

        return
        end

