c! 2-d convolution of input array with specified gaussian (all in pixel units)
c! normalised by area of kernel so that integrated intensity is conserved.

        subroutine conv2(arr,x,y,n,m,sigma,cn,bcon,bcode,fac,s1)
        implicit none
        integer i,j,i1,j1,i2,j2,n,m,wid,nchar,x,y
        real*8 arr(x,y),g(n,m),cn(x,y),s1,dumr
        real*8 sigma(3),s2,fac
        character bcon*10,str1*1,bcode*4

        if (bcode.eq.'full') then
         if (max(sigma(1),sigma(2)).gt.max(n,m)) then
          write (*,*) '  Gaussian bigger than image ! '
          goto 333
         end if

444      continue
         write (*,'(a40,$)') '   Preserve (t)otal flux or (i)ntensity '
         read (*,*) str1
         if (str1.ne.'t'.and.str1.ne.'i') goto 444
        end if
        if (bcode.eq.'bare') str1='t'
        
        call conv2_get_wid_fac(sigma,n,m,s1,fac,wid)
        call conv2_get_gaus(sigma,n,m,wid,g)
        if (bcode.eq.'full') then
         write (*,*) '  Using gaussian image of size ',wid
         write (*,*) '  At edge, condition is ',bcon(1:nchar(bcon))
        end if

c! area under convolved figure is preserved
        if (bcode.ne.'none') then
         do 140 i=1,n
          do 150 j=1,m
 
           cn(i,j)=0.d0
           do 160 i1=1,wid
            do 170 j1=1,wid
             i2=i-int(wid/2)+(i1-1)
             j2=j-int(wid/2)+(j1-1)
             if (i2.lt.1.or.j2.lt.1.or.i2.gt.n.or.j2.gt.m) then
               if (bcon.eq.'periodic') then
                call perbound(arr,x,y,i2,j2,n,m,dumr)  
               else
                dumr=0.d0
               end if
             else
              dumr=arr(i2,j2)
             end if
             cn(i,j)=cn(i,j)+dumr*g(i1,j1)
170         continue
160        continue
           if (str1.eq.'t') cn(i,j)=cn(i,j)/s1

150       continue
140      continue
        else
         call initialiseimage(cn,x,y,n,m,0.d0)
        end if   ! bcode ne none

333     continue

        return
        end

c! put periodic boundary conditions
        subroutine perbound(arr,x,y,i2,j2,n,m,dumr)  ! periodic boundary conditions
        implicit none
        integer i2,j2,n,m,i,j,x,y
        real*8 dumr,arr(x,y)

        i=i2
        j=j2
        if (i2.lt.1) i=n+i2
        if (j2.lt.1) j=m+j2
        if (i2.gt.n) i=i2-n
        if (j2.gt.m) j=j2-m
        dumr=arr(i,j)

        return
        end


