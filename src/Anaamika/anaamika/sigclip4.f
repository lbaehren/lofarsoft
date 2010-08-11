c! calculates std and mean by n-sigma clipping. 
c! arr of small size n,m has all the values in the box to be fit
c! for now, compute by direct computation. Maybe final call to fit as well.
c! initial guess centre should be median, width is std about median
c! Had originally converted array to vector and worked with that but 
c! crashes if array is too long so dont do that.
c! so dont use median at all as need to have prog to calc median with array
c! and not vector. do later. rt now with actual mean as initial guess it converges.
c! testsigclip shows it does converge.
c! for real*4 image

        subroutine sigclip4(arr4,x,y,n1,m1,n,m,std4,av4,nsig)
        implicit none
        integer n,m,i,j,num,x,y,ind,ind1(6),n1,m1
        real*4 arr4(x,y),av4,std4
        real*8 arr(x,y),av,s1,std,a(3),ssq,nsig
        real*8 summ,avold,stdold,ind2(2)
       
        call array4to8(arr4,x,y,arr,x,y)
        call sigclip(arr,x,y,n1,m1,n,m,std,av,nsig)
444     continue
        std4=std
        av4=av

        return
        end

