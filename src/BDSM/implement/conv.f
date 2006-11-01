c to see if i can write a decent convolution program (the answer is yes)

        implicit none
        integer i,j,i1,j1,i2,j2,n,m,wid,indx1(20000),indx2(20000)
        real*8 arr(1024,1024),g(1021,1021),cn(1024,1024),s1,dumr
        real*8 sigma

        n=128
        m=128
        do 100 i=1,n
         do 110 j=1,m
          arr(i,j)=0.0d0
110      continue
100     continue
        arr(8,6)=20.d0
        arr(5,4)=10.d0
        arr(6,10)=5.d0

        j=0
        do 800 i=1,128
         do 810 i1=1,128
          j=j+1
          indx1(j)=i
          indx2(j)=i1
810      continue
800     continue
        open(unit=21,file='mrcos',status='old')
        do 820 i=1,16384
          read (21,*) arr(indx1(i),indx2(i))
820     continue
        close(21)

        open(unit=21,file='z0',status='unknown')
        sigma=3.0d0
        wid=int(7*sigma)
        if (int(wid/2.d0)/(wid/2.d0).eq.1.d0) wid=wid+1
        if (wid.lt.7) wid=7
c! gaussian has unit peak. sigma is in pixel width. wid is pixel extent of gaussian.
        s1=0.d0
        do 120 i=1,wid
         do 130 j=1,wid
          i1=(wid+1)/2
          g(i,j)=dexp(-((i-i1)**2.d0+(j-i1)**2.d0)/(2.d0*sigma*sigma))
          s1=s1+g(i,j)
          if (j.eq.wid) write (21,'(f7.4)') g(i,j)
          if (j.lt.wid) write (21,'(f7.4,a1,$)') g(i,j),' '
130      continue
120     continue
        close(21)

        open(unit=21,file='z1',status='unknown')
        do 240 i=1,n
         do 250 j=1,m
          if (j.eq.m) write (21,'(f7.4)') arr(i,j)
          if (j.lt.m) write (21,'(f7.4,a1,$)') arr(i,j),' '
250       continue
240       continue
          close(21)

c! area under convolved figure is preserved
        open(unit=21,file='z2',status='unknown')
        do 140 i=1,n
         do 150 j=1,m

          cn(i,j)=0.d0
          do 160 i1=1,wid
           do 170 j1=1,wid
            i2=i-int(wid/2)+(i1-1)
            j2=j-int(wid/2)+(j1-1)
            if (i2.lt.1.or.j2.lt.1.or.i2.gt.n.or.j2.gt.m) then
             call perbound(arr,i2,j2,n,m,dumr)  ! periodic boundary conditions
            else
             dumr=arr(i2,j2)
            end if
            cn(i,j)=cn(i,j)+dumr*g(i1,j1)
170        continue
160       continue
          cn(i,j)=cn(i,j)/s1
          if (j.eq.m) write (21,'(f7.4)') cn(i,j)
          if (j.lt.m) write (21,'(f7.4,a1,$)') cn(i,j),' '

150      continue
140     continue
        close(21)

        end

c! put periodic boundary conditions
        subroutine perbound(arr,i2,j2,n,m,dumr)  ! periodic boundary conditions
        implicit none
        integer i2,j2,n,m,i,j
        real*8 dumr,arr(1024,1024)

        i=i2
        j=j2
        if (i2.lt.1) i=n+i2
        if (j2.lt.1) j=m+j2
        if (i2.gt.n) i=i2-n
        if (j2.gt.m) j=j2-m
        dumr=arr(i,j)

        return
        end


