c! ascending order, two vectors

      subroutine sort3_2(n,ra,rb,wksp,iwksp)
      implicit none  
      integer n,iwksp(n)
      real*8 ra(n),rb(n),wksp(n)
CU    USES indexx
      integer j

      call indexx(n,ra,iwksp)
      do 11 j=1,n
        wksp(j)=ra(j)
11    continue
      do 12 j=1,n
        ra(j)=wksp(iwksp(j))
12    continue

      do 13 j=1,n
        wksp(j)=rb(j)
13    continue
      do 14 j=1,n
        rb(j)=wksp(iwksp(j))
14    continue

      return
      END


