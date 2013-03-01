cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     strmatch --- do the strings match?
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      logical function strmatch(s1, s2)
      implicit none
      character*(*) s1, s2
      integer l1, l2 ,i
      integer length

      l1 = length(s1)
      l2 = length(s2)
      
      if (.not. (l1 .eq. l2)) then
         strmatch = .false.
         return
      end if

      do i=1,l1
         if (.not.(s1(i:i) .eq. s2(i:i))) then
c     write (*,'(a3," noteq ", a3, i)')s1,s2,l
            strmatch = .false.
            return
         endif
      enddo

      strmatch = .true.
      return
      end
