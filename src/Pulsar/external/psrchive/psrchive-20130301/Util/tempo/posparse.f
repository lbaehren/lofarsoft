C redwards fortran subroutines I wrote for the ephemeris code
C to parse position strings
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     hmsToDouble and dmsToDouble -- string to double conversion for 
c     hours:minutes[:seconds] and degrees:m[:s] data. Also returns (prec) the
c     value in /*turns*/ _sec_ of a unit in the least significant provided digit.
c     xmsToDouble is a general function used by both, given co-efficients
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      real*8 function xmstodouble(s,f1,f2,f3, prec)
      implicit none
      character*(*) s
      real*8 f1,f2,f3,val, tmp, sgn, prec
      integer pos, pos1, length, slen, i
      logical found 

      slen = length(s)
      xmsToDouble = -1000.0d0   ! in case of error

      found = .false.
      pos = 1
      do while ((pos .le. slen) .and. .not. found)
         if (s(pos:pos) .eq. ':') found = .true.
         pos = pos +1
      enddo
      pos = pos -1
      if (.not. found) return   ! colon missing
      read(s(1:pos-1),*, err=120) tmp
      val = f1*tmp
c     Get the sign of first field since it is actually sign of whole number
      if (s(1:1).eq.'-') then
         sgn = -1.d0
      else
         sgn = 1.d0
      endif
      pos = pos+1
      pos1 = pos
      found = .false.
      do while ((pos .le. slen) .and. .not. found)
         if (s(pos:pos) .eq. ':') found = .true.
         pos = pos + 1
      enddo
      pos = pos-1
      read(s(pos1:pos-1),*,err=120)tmp
      val = val + sgn*f2*tmp
      if (.not. found) then     ! no seconds field. Okay.
c     prec = f2
         prec = f2/f1
         xmsToDouble = val
         return
      endif 
      read(s(pos+1:slen),*,err=120)tmp
      val = val + sgn*f3*tmp
      if (slen-pos .eq. 3) then ! no fractional part: colon, tens, units
c     prec = f3
         prec = 1.0d0
      else
c     prec = 10.0d0**(pos-slen+3.0d0) * f3  ! account for the '.'
c     prec = 10.0d0**(pos-slen+3.0d0)  ! account for the '.'
c     Use a loop to get the precision more accurately and avoid things
c     like 9.99999999999999999D...
         prec = 1.0
         do i=1,slen-pos-3
            prec = prec * 0.1d0
         end do
      end if
      xmsToDouble = val
      return
 120  write(*,130) s(1:slen)    ! Error handler
      return
 130  format ("Error: posparse.f: unable to decipher the following:",a)
      end

      real*8 function hmstoturns(s, prec)
      character*(*) s
      real*8 xmsToDouble, prec
      hmsToTurns = xmsToDouble(s, 1d0/24d0, 1d0/1440d0, 
     +     1d0/86400d0,prec)
      return
      end

      real*8 function dmstoturns(s,prec)
      character*(*) s
      real*8 xmsToDouble, prec
      dmsToTurns = xmsToDouble(s, 1d0/360d0, 1d0/21600d0, 
     +     1d0/1296d3,prec)
      return
      end
