c! get gaul para, decide on culling and pass on c_flag and ngaucut

      subroutine get_culled_gaullist(ngau,gaulid,islid,flag,tot,dtot,
     /    peak,epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /    bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /    sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,trc1,trc2,
     /    im_rms,dumr2,dumr3,dumr4,dumr5,dumr6,c_flag,ngaucut,n,m,
     /    snrcut)
      implicit none
      integer n,m,ngau,ngaucut,c_flag(ngau),i
      integer flag(ngau),srcid(ngau),trc1(ngau),trc2(ngau)
      integer gaulid(ngau),islid(ngau),blc1(ngau),blc2(ngau)
      real*8 ra(ngau),dec(ngau),rstd(ngau),rav(ngau),sstd(ngau)
      real*8 sav(ngau),tot(ngau),dtot(ngau)
      real*8 bmaj(ngau),bmin(ngau),bpa(ngau),xpix(ngau)
      real*8 expix(ngau),ypix(ngau),eypix(ngau)
      real*8 era(ngau),edec(ngau),totfl(ngau),etotfl(ngau),peak(ngau)
      real*8 epeak(ngau),ebmaj(ngau),ebmin(ngau),ebpa(ngau)
      real*8 dbmaj(ngau),edbmaj(ngau),dbmin(ngau),edbmin(ngau)
      real*8 dbpa(ngau),edbpa(ngau),chisq(ngau),q(ngau),snrcut
      character code(ngau)*4,code4(ngau)*4,names(ngau)*30
      real*8 im_rms(ngau),dumr2(ngau),dumr3(ngau),dumr4(ngau)
      real*8 dumr5(ngau),dumr6(ngau)

      ngaucut=0
      do i=1,ngau
       c_flag(i)=0
       if (flag(i).ne.0) then
        c_flag(i)=1
       else
        if (peak(i)/im_rms(i).ge.snrcut) then
         ngaucut=ngaucut+1
        else
         c_flag(i)=1
        end if
       end if
      end do

      return
      end

c
