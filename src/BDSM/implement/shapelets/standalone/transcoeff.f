c! read hermcoeff and transpose it to use as data statement inside main
c! calls to save time.
c! use as include file
        
        implicit none
        integer i,j
        real*8 vec(31),thc(31,31)

        open(unit=21,file='hermcoef')
        open(unit=22,file='../../includes/trhermcoef.inc')
        do i=1,31
         read (21,700) vec
         do j=1,31
          thc(j,i)=vec(j)
         end do
        end do
        close(21)

        write (22,*)
        write (22,*) '        real*8 hc(31,31)'
        write (22,*) '        integer nhc'
        write (22,*) 
        write (22,*) '        parameter (nhc=31)'
        write (22,*) '        data hc/'

        do i=1,31
         do j=1,31
          vec(j)=thc(i,j)
         end do
         do j=1,30,2
          write (22,710) '     /  ',vec(j),',',vec(j+1),','
         end do
         if (i.ne.31) then
          write (22,710) '     /  ',vec(31),','
         else
          write (22,710) '     /  ',vec(31),'/'
         end if
        end do

        close(21)
        close(22)

700     format(31(1Pe25.18,1x))
710     format(a8,1Pe25.18,a1,1Pe25.18,a1)

        end

        
