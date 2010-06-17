
      SUBROUTINE linfit(x,y,ndata,sig,mwt,a,b,siga,sigb,chi2,q)
      implicit none 
      INTEGER mwt,ndata
      REAL*8 a,b,chi2,q,siga,sigb,sig(ndata),x(ndata),y(ndata)
CU    USES gammq
      INTEGER i
      REAL*8 sigdat,ss,st2,sx,sxoss,sy,t,wt,gammq

      sx=0.d0
      sy=0.d0
      st2=0.d0
      b=0.d0
      if(mwt.ne.0) then
        ss=0.d0
        do 11 i=1,ndata
          wt=1.d0/(sig(i)*sig(i))
          ss=ss+wt
          sx=sx+x(i)*wt
          sy=sy+y(i)*wt
11      continue
      else
        do 12 i=1,ndata
          sx=sx+x(i)
          sy=sy+y(i)
12      continue
        ss=float(ndata)
      endif
      sxoss=sx/ss
      if(mwt.ne.0) then
        do 13 i=1,ndata
          t=(x(i)-sxoss)/sig(i)
          st2=st2+t*t
          b=b+t*y(i)/sig(i)
13      continue
      else
        do 14 i=1,ndata
          t=x(i)-sxoss
          st2=st2+t*t
          b=b+t*y(i)
14      continue
      endif
      b=b/st2
      a=(sy-sx*b)/ss
      siga=sqrt((1.d0+sx*sx/(ss*st2))/ss)
      sigb=sqrt(1.d0/st2)
      chi2=0.d0
      if(mwt.eq.0) then
        do 15 i=1,ndata
          chi2=chi2+(y(i)-a-b*x(i))*(y(i)-a-b*x(i))
15      continue
        q=1.d0
        sigdat=sqrt(chi2/(ndata-2))
        siga=siga*sigdat
        sigb=sigb*sigdat
      else
        do 16 i=1,ndata
          chi2=chi2+((y(i)-a-b*x(i))/sig(i))**2.d0
16      continue
c        q=gammq(0.5d0*(ndata-2),0.5d0*chi2)
      endif
      return
      END
