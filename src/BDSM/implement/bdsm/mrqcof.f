c! From NRP. Converted to real*8
c! fnum : 1 ==> gaussian; 2 ==> 1st order poly; 3 ==> 4th order poly
      SUBROUTINE mrqcof(x,y,sig,siz,ndata,a,ia,ma,alpha,beta,nalp,chisq,
     /                  fnum)
      INTEGER ma,nalp,ndata,ia(ma),MMAX,fnum,siz
      REAL*8 chisq,a(ma),alpha(nalp,nalp),beta(ma),sig(siz),x(siz),
     *y(siz)
      PARAMETER (MMAX=20)
      INTEGER mfit,i,j,k,l,m
      REAL*8 dy,sig2i,wt,ymod,dyda(MMAX)

      mfit=0
      do 11 j=1,ma
        if (ia(j).ne.0) mfit=mfit+1
11    continue
      do 13 j=1,mfit
        do 12 k=1,j
          alpha(j,k)=0.d0
12      continue
        beta(j)=0.d0
13    continue
      chisq=0.d0
      do 16 i=1,ndata
        if (fnum.eq.1)  call fgauss(x(i),a,ymod,dyda,ma)
        if (fnum.eq.2)  call poly2(x(i),a,ymod,dyda,ma)
        if (fnum.eq.3)  call func2(x(i),a,ymod,dyda,ma)
        if (fnum.eq.4)  call poly3(x(i),a,ymod,dyda,ma)
        sig2i=1.d0/(sig(i)*sig(i))
        dy=y(i)-ymod
        j=0
        do 15 l=1,ma
          if(ia(l).ne.0) then
            j=j+1
            wt=dyda(l)*sig2i
            k=0
            do 14 m=1,l
              if(ia(m).ne.0) then
                k=k+1
                alpha(j,k)=alpha(j,k)+wt*dyda(m)
              endif
14          continue
            beta(j)=beta(j)+dy*wt
          endif
15      continue
        chisq=chisq+dy*dy*sig2i
16    continue
      do 18 j=2,mfit
        do 17 k=1,j-1
          alpha(k,j)=alpha(j,k)
17      continue
18    continue
      return
      END
