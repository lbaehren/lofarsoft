
      subroutine ksone(dataa,n,func,d,prob)
      implicit none

      integer n
      real*8 d,dataa(n),func,prob
      external func
CU    USES probks,sort
      integer j
      real*8 dt,en,ff,fn,fo,probks

      call sort(n,dataa)
      en=n
      d=0.d0
      fo=0.d0
      do 11 j=1,n
        fn=j/en
        ff=func(dataa(j))
        dt=max(abs(fo-ff),abs(fn-ff))
        if(dt.gt.d)d=dt
        fo=fn
11    continue
      en=sqrt(en)
      prob=probks((en+0.12d0+0.11d0/en)*d)

      return
      end
