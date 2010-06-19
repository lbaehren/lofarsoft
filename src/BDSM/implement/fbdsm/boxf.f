c! boxcar smoothing. preserves rms anyway since wt is uniform flat unity 

        subroutine boxf(image1,x,y,n,m,sigma,image2)
        implicit none
        integer x,y,n,m,i,j,i1,j1,i2,j2,siz,round,wid(2)
        real*8 image1(x,y),image2(x,y),sigma(3),dumr,av
        character bcon*8

        wid(1)=round(sigma(1))
        wid(2)=round(sigma(2))
        do i=1,2
         if (mod(wid(i),2).eq.0) then
          wid(i)=wid(i)+1
          write (*,*) '  Increasing axis ',i,' width to ',wid(i)
         end if
        end do

        if (max(wid(1),wid(2)).gt.max(n,m)) then
         write (*,*) '  Boxcar bigger than image ! '
         goto 333
        end if
        bcon='zero pad'
        write (*,*) '  At edge, condition is ',bcon
        siz=wid(1)*wid(2)

c! area under convolved figure is preserved
        do 140 j=1,m
         do 150 i=1,n
          av=0.d0
          do 160 i1=1,wid(1)
           do 170 j1=1,wid(2)
            i2=i-wid(1)/2+(i1-1)
            j2=j-wid(2)/2+(j1-1)
            if (i2.lt.1.or.j2.lt.1.or.i2.gt.n.or.j2.gt.m) then
              if (bcon.eq.'periodic') then
               call perbound(image1,x,y,i2,j2,n,m,dumr)  
              else
               dumr=0.d0
              end if
            else
             dumr=image1(i2,j2)
            end if
            av=av+dumr
170        continue
160       continue
          image2(i,j)=av/siz
150       continue
140      continue

333     continue

        return
        end


        
