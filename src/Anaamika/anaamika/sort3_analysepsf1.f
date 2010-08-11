
      subroutine sort3_analysepsf1(n,ra,flag,peak,xpix,ypix,bmaj,
     /       ebmaj,bmin,ebmin,bpa,ebpa,wksp,iwksp)
      implicit none
      integer n,iwksp(n)
      real*8 ra(n),peak(n),xpix(n),ypix(n),bmaj(n)
      real*8 ebmaj(n),bmin(n),ebmin(n),bpa(n),ebpa(n)
      real*8 wksp(n),wksp1(n),wksp2(n),wksp3(n),wksp4(n)
      integer j,flag(n),wkspi(n)

      call indexx(n,ra,iwksp)
      do 11 j=1,n
        wksp(j)=ra(j)
11    continue
      do 12 j=1,n
        ra(j)=wksp(iwksp(j))
12    continue

      do 13 j=1,n
        wksp1(j)=peak(j)
        wksp2(j)=xpix(j)
        wksp3(j)=ypix(j)
        wksp4(j)=bmaj(j)
        wkspi(j)=flag(j)
13    continue
      do 14 j=1,n
        peak(j)=wksp1(iwksp(j))
        xpix(j)=wksp2(iwksp(j))
        ypix(j)=wksp3(iwksp(j))
        bmaj(j)=wksp4(iwksp(j))
        flag(j)=wkspi(iwksp(j))
14    continue

      do 15 j=1,n
        wksp1(j)=ebmaj(j)
        wksp2(j)=bmin(j)
        wksp3(j)=ebmin(j)
        wksp4(j)=bpa(j)
        wksp(j)=ebpa(j)
15    continue
      do 16 j=1,n
        ebmaj(j)=wksp1(iwksp(j))
        bmin(j)=wksp2(iwksp(j))
        ebmin(j)=wksp3(iwksp(j))
        bpa(j)=wksp4(iwksp(j))
        ebpa(j)=wksp(iwksp(j))
16    continue
      return
      end


