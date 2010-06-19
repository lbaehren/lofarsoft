c! to calculate structure function. useful to look at iono effects later 

        subroutine structfn(f1,f2,order)
        implicit none
        integer n,m,order,l
        character f1*(*),f2*(*),extn*20

cf2py   intent(in) f1,f2,order

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_structfn(f1,f2,order,n,m)

        return
        end
c!
c!
        subroutine sub_structfn(f1,f2,order,n,m)
        implicit none
        integer n,m,nchar,i,j,i1,j1,two,n0,order
        character f1*(*),f2*(*),lab*500,strdev*5,str3*3,extn*20
        real*8 image1(n,m),dumr,image2(2*n-1,2*m-1)   ! image1 is main array and image2 is ac
        real*8 ac,std,av

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        two=1
        dumr=log(float(n))/log(2.0)
        if (float(int(dumr)).ne.dumr) two=0
        dumr=log(float(m))/log(2.0)
        if (float(int(dumr)).ne.dumr) two=0
        if (two.eq.0) then
         write (*,*) '   ###  Size of array not power of 2'
         write (*,*) '   ###         Exiting ... '
         goto 333
        end if

        call arrstat(image1,n,m,1,1,n,m,std,av)

        do 820 i=-(n-1),(n-1)
         do 811 j=-(m-1),(m-1)
          image2(i+n,j+m)=0.d0
811      continue
820     continue

c! code for order=0,1,2 seperately to save time
        write (*,'(a,$)') '   '
        if (order.eq.0) then                       ! order = 0
         do 100 i=-(n-1),(n-1)
          do 110 j=-(m-1),(m-1)
           n0=0
           do 120 i1=1,n
            do 130 j1=1,m
             if (i1+i.le.n.and.i1+i.ge.1.and.
     /           j1+j.le.m.and.j1+j.ge.1) then
              dumr=image1(i1,j1)-image1(i1+i,j1+j)
              image2(i+n,j+m)=image2(i+n,j+m)+1.d0
              n0=n0+1
             end if
130         continue
120        continue
           image2(i+n,j+m)=image2(i+n,j+m)/(n0*1.d0)/(av*av)
110       continue
          if (mod(i,10).eq.0) write (*,'(a1,$)') '.'
100      continue
         write (*,*) 
        end if

        if (order.eq.1) then                       ! order = 1
         do 200 i=-(n-1),(n-1)
          do 210 j=-(m-1),(m-1)
           n0=0
           do 220 i1=1,n
            do 230 j1=1,m
             if (i1+i.le.n.and.i1+i.ge.1.and.
     /           j1+j.le.m.and.j1+j.ge.1) then
              dumr=image1(i1,j1)-image1(i1+i,j1+j)
              image2(i+n,j+m)=image2(i+n,j+m)+dumr
              n0=n0+1
             end if
230         continue
220        continue
           image2(i+n,j+m)=image2(i+n,j+m)/(n0*1.d0)/(av*av)
210       continue
          if (mod(i,10).eq.0) write (*,'(a1,$)') '.'
200      continue
         write (*,*) 
        end if

        if (order.eq.2) then                       ! order = 2
         do 300 i=-(n-1),(n-1)
          do 310 j=-(m-1),(m-1)
           n0=0
           do 320 i1=1,n
            do 330 j1=1,m
             if (i1+i.le.n.and.i1+i.ge.1.and.
     /           j1+j.le.m.and.j1+j.ge.1) then
              dumr=image1(i1,j1)-image1(i1+i,j1+j)
              image2(i+n,j+m)=image2(i+n,j+m)+dumr*dumr
              n0=n0+1
             end if
330         continue
320        continue
           image2(i+n,j+m)=image2(i+n,j+m)/(n0*1.d0)/(av*av)
310       continue
          if (mod(i,10).eq.0) write (*,'(a1,$)') '.'
300      continue
         write (*,*) 
        end if


        if (order.ne.0.and.order.ne.1.and.order.ne.2) then 
         do 400 i=-(n-1),(n-1)
          do 410 j=-(m-1),(m-1)
           n0=0
           do 420 i1=1,n
            do 430 j1=1,m
             if (i1+i.le.n.and.i1+i.ge.1.and.
     /           j1+j.le.m.and.j1+j.ge.1) then
              dumr=image1(i1,j1)-image1(i1+i,j1+j)
              image2(i+n,j+m)=image2(i+n,j+m)+(dumr**order)
              n0=n0+1
             end if
430         continue
420        continue
           image2(i+n,j+m)=image2(i+n,j+m)/(n0*1.d0)/(av*av)
410       continue
          if (mod(i,10).eq.0) write (*,'(a1,$)') '.'
400      continue
         write (*,*) 
        end if

        i1=2*n-1
        j1=2*m-1
        call pgnumb(order,0,1,str3,i)
        lab='Structure function of image; order='//str3
        call writearray_bin2D(image2,i1,j1,i1,j1,f2,'mv')
        call greyandslice(image2,i1,j1,i1,j1,lab)

333     continue

        return
        end



