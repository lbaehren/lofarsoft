c! x8=log(SNR); y8=bmaj; ng=number of samples

        subroutine sub_analysepsf2_med(ngau,ng,x8,y8,x4d,y4d,n4,nbin)
        implicit none
        integer ngau,ng,n4,i,j
        real*8 x8(ngau),y8(ngau),dumr,y8bin(ngau),medianj(ngau)
        real*8 xd(ngau),yd(ngau),av(ngau),std(ngau)
        real*4 x4d(n4),y4d(n4)
        integer npt1,npt2,npt3,fac1,nbin,nsrc_bin(ngau)
        integer srcid_bin(ngau,2)

        do i=1,ng
         xd(i)=x8(i)
         yd(i)=y8(i)
        end do

        call callsort3_2(ngau,ng,xd,yd)  ! ascending order of SNR
        if (ng.ge.200) then 
         fac1=20
        else
         if (ng.ge.150) then
          fac1=10
         else
          fac1=5
         end if
        end if
        npt1=fac1
        npt2=ng/10
        npt3=sqrt(npt1*npt2*1.d0)  ! optimum number of pts per bin. If bin too large, use npt1
        dumr=(xd(ng)-xd(1))/5.d0
        nbin=1
        nsrc_bin(nbin)=1
        srcid_bin(nbin,1)=ng    ! 1 and 2 are end and beginning indices
555     continue
        if (xd(srcid_bin(nbin,1))-xd(srcid_bin(nbin,1)-npt3).le.dumr) 
     /      then
         srcid_bin(nbin,2)=srcid_bin(nbin,1)-npt3
        else
         srcid_bin(nbin,2)=srcid_bin(nbin,1)-npt1
        end if
        nsrc_bin(nbin)=srcid_bin(nbin,1)-srcid_bin(nbin,2)+1
        if (srcid_bin(nbin,2).lt.npt3) then
         srcid_bin(nbin,2)=1
         nsrc_bin(nbin)=srcid_bin(nbin,1)-srcid_bin(nbin,2)+1
        else
         nbin=nbin+1
         nsrc_bin(nbin)=1
         srcid_bin(nbin,1)=srcid_bin(nbin-1,2)-1
         goto 555
        end if

        do i=1,nbin
         do j=1,nsrc_bin(i)
          y8bin(j)=yd(srcid_bin(i,2)+(j-1))
         end do
         call callcalcmedian(y8bin,ngau,nsrc_bin(i),medianj(i))
         x4d(i)=0.5d0*(xd(srcid_bin(i,1))+xd(srcid_bin(i,2)))

         call vec_std(y8bin,ngau,1,nsrc_bin(i),av(i),std(i))
         x4d(i)=0.5d0*(xd(srcid_bin(i,1))+xd(srcid_bin(i,2)))
         y4d(i)=std(i)
         y4d(i)=av(i)
         y4d(i)=medianj(i)
        end do

        return
        end


