
      subroutine vec_stats(vec,x,n1,n,med,cmed,rms,av,crms,cav,nsig)
      implicit none
      integer x,n1,n,i
      real*8 vec(x),svec(n-n1+1),med,cmed,rms,av,crms,cav,nsig

Cf2py intent(in) vec,x,n1,n,nsig
Cf2py intent(out) av,rms,med,cav,crms,cmed

      do i=n1,n
       svec(i-n1+1)=vec(i)
      end do
      call c_calcmedian(svec,x,med)
      call calcmedianclip(svec,x,1,n,cmed,nsig)
      call vec_std(svec,x,1,n,av,rms)
      call sigclip1d(svec,x,1,n,crms,cav,nsig)

      return
      end
