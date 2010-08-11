c! ascending order, two vectors
c rb is integer

      subroutine sort3_2i(n,ra,rb,wksp,iwksp)
      implicit none  
      integer n,iwksp(n),rb(n),dumi(n)
      real*8 ra(n),wksp(n)
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
        dumi(j)=rb(j)
13    continue
      do 14 j=1,n
        rb(j)=dumi(iwksp(j))
14    continue

      return
      END


