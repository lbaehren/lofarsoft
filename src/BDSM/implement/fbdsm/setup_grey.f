c! initial part of grey.f

        subroutine setup_grey(starc,linec,chold,array,x,y,i,j,low,up,
     /        arr4,zero,lowh,uph,hi,mn1,mx1,mn2,mx2,low1,logged)
        implicit none
        integer i,j,i1,i2,x,y,hi,dumi,zero,starc
        integer linec,pgopen,id1
        real*8 array(x,y)
        real*4 arr4(i,j),low,up,mn1,mx1,mn2,mx2,uph,lowh,low1
        character chold*1
        logical logged

        starc=1
        linec=0
        chold='0'
        do i2=1,j
         do i1=1,i   ! get 1st nonblank pixel value
          if (array(i1,i2).ne.-999.d0) then
           low=array(i1,i2)
           up=array(i1,i2)
           goto 122
          end if
         end do
        end do
122     continue
         
        do 110 i2=1,j
         do 100 i1=1,i  ! get min and max
          arr4(i1,i2)=array(i1,i2)
          if (arr4(i1,i2).ne.-999.d0) then
           if (low.gt.arr4(i1,i2)) low=arr4(i1,i2)
           if (up.lt.arr4(i1,i2)) up=arr4(i1,i2)
          end if
100      continue
110     continue
c        low=low-0.1*abs(up-low)  ! so lowest signal is seeable

        logged=.false.
        if (zero.eq.1) then
         if (low.ne.0.0.and.abs(up/low).gt.45.0) logged=.true. ! then convert to LG else keep LN
         if (low.eq.0.0.and.up.gt.45.0) logged=.true. ! then convert to LG else keep LN
         if (logged) then
          call arr2dnz(array,x,y,i,j,low,up)
          call converttolog(arr4,i,j,low,up,low1)
         end if
        end if

        lowh=low
        uph=up
c        if (hi.eq.1) then
         dumi=i*j
         id1=pgopen('/NULL')
         call pgslct(id1)
         call pghist(dumi,arr4,lowh,uph,100,0)
         call pgqwin(mn1,mx1,mn2,mx2)
         call pgclos
c        end if

        return
        end


