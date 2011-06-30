c! mean, std, median. xval is middle fo each bin. 
c! modify so that u take median instead of mean depending on code(3) (Y=mean, N=median)

        subroutine bin_and_stats_ny(xp,yp,x,n,ptpbin,nbin,
     /       ptplastbin,over,mean,emean,std,estd,median,emedian,xval,   
     /       code,kappa)
        implicit none
        integer n,nchar,ptpbin,nbin,i,ptplastbin,j,over,x
        real*8 mean(nbin),emean(nbin),std(nbin),estd(nbin),dumr,kappa
        real*8 median(nbin),emedian(nbin),xval(nbin),xp(x),yp(x)
        real*8 xplot(n),yplot(n)
        character code*3

        do i=1,n
         xplot(i)=xp(i)
         yplot(i)=yp(i)
        end do

c! do the regular bins
        dumr=ptpbin*1.d0
        do i=1,nbin-2
         call sub_bin_and_stats_ny(i,ptpbin,dumr,over,xplot,yplot,n,
     /        code,xval(i),mean(i),std(i),median(i),kappa)
        end do

c! now the 2nd last
        call sub_bin_and_stats_ny(nbin-1,ptplastbin,dumr,over,xplot,
     /       yplot,n,code,xval(nbin-1),mean(nbin-1),std(nbin-1),
     /       median(nbin-1),kappa)

c! now the last bin which has ptpbin/2 points only
        dumr=over*1.d0*(n-ptpbin/2)/(nbin-1)
        call sub_bin_and_stats_ny(nbin,ptpbin/over,dumr,over,xplot,
     /   yplot,n,code,xval(nbin),mean(nbin),std(nbin),median(nbin),
     /   kappa)

        return
        end

c!
c! do twice only, once full vec and second time after kappa-clipping.
c!
        subroutine sub_bin_and_stats_ny(i,npts,npts_t,over,xplot,yplot,
     /        n,code,xval1,mean1,std1,median1,kappa)
        implicit none
        integer n,nchar,npts,nbin,i,j,round,nkappa,over
        real*8 mean1,emean1,std1,estd1,median1,emedian1,xval1
        real*8 mean2,std2,median2
        real*8 xplot(n),yplot(n),xbox(npts),ybox(npts),npts_t
        real*8 xboxk(npts),yboxk(npts),kappa,avmed(2)  ! avmed(1) if mean and (2) is median
        character code*3

        do j=1,npts
         xbox(j)=xplot(round(1+(i-1)*npts_t/over+(j-1)))
         ybox(j)=yplot(round(1+(i-1)*npts_t/over+(j-1)))
        end do
        if (code(3:3).eq.'Y') then
         call vec_mean(xbox,npts,1,npts,xval1)
         if (code(2:2).eq.'Y') 
     /      call vec_std(ybox,npts,1,npts,mean1,std1)
         if (code(1:1).eq.'Y') 
     /      call vec_mean(ybox,npts,1,npts,mean1)
        end if
        
        if (code(3:3).eq.'Y') then
         if (kappa.gt.0.d0) then
          nkappa=0
          do j=1,npts
           if ((ybox(j)-mean1).le.std1*kappa) then
            nkappa=nkappa+1
            xboxk(nkappa)=xbox(j)
            yboxk(nkappa)=ybox(j)
           end if
          end do 
          if (nkappa.gt.1) then
           call vec_mean(xboxk,npts,1,nkappa,xval1)
           if (code(2:2).eq.'Y') 
     /        call vec_std(yboxk,npts,1,nkappa,mean1,std1)
           if (code(1:1).eq.'Y') 
     /        call vec_mean(yboxk,npts,1,nkappa,mean1)
          end if
         end if
        else
         call calcmedianclip2vec(ybox,xbox,npts,1,npts,mean1,median1,
     /        std1,mean2,std2,median2,kappa) 
         xval1=median2  ! mid pt of bin is median
        end if

        return
        end


