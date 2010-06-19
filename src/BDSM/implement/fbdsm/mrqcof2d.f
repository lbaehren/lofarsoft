c! fnum : 1 ==> gaussian; 2 ==> 1st order poly; 3 ==> 4th order poly
c! modified for 2d arrays
      SUBROUTINE mrqcof2d(y,sig,ndata,a,ia,ma,alpha,beta,nalp,chisq,
     /                  fnum,cutoff)
      INTEGER ma,nalp,ndata,ia(ma),MMAX,fnum
      REAL*8 chisq,a(ma),alpha(nalp,nalp),beta(ma),sig(ndata,ndata),
     /  y(ndata,ndata)
      PARAMETER (MMAX=20)
      INTEGER mfit,i,j,k,l,m,i1
      REAL*8 dy,sig2i,wt,ymod,dyda(MMAX),cutoff

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
       do 33 i1=1,ndata
        if (y(i,i1).ge.cutoff) then
         if (fnum.eq.1) call fgauss2de(i,i1,a,ymod,dyda,ma)
         if (fnum.eq.2) call fgauss2dse(i,i1,a,ymod,dyda,ma)
         sig2i=1.d0/(sig(i,i1)*sig(i,i1))
         dy=y(i,i1)-ymod
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
14           continue
             beta(j)=beta(j)+dy*wt
           endif
15       continue
         chisq=chisq+dy*dy*sig2i
        end if ! y > cutoff
33     continue
16    continue
      do 18 j=2,mfit
        do 17 k=1,j-1
          alpha(k,j)=alpha(j,k)
17      continue
18    continue
      return
      END
