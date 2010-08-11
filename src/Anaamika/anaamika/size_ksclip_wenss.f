c! adapted from analyse_asrl.f
c! fit sigma/mean =sqrt(c1^2 + c2^2/SNR^2) like in analyse_asrl
c! but use median instead of mean. and so kappa-sigma clipping on this
c! to get kappa-sigma clipped c1 and c2 !! but only twice actually.
c! fit straight line or same fn to median vs snr.
c! clip properly, not just twice.

        subroutine size_ksclip_wenss(plotdir,fullname,ncut,bmaj,
     /     bmin,bpa,peak,im_rms,s_med_f,s_rms_c_f,kappa,cbmaj,
     /     cbmin,cbpa,s_c1,s_c2,s_dm1,s_dm2,f_sclip,over,kappa2,
     /     filename,scratch,srldir,ra,dec,bm_pix,cdelt)
        implicit none
        character plotdir*500,fullname*500,code*3,scratch*500,srldir*500
        character filename*500,dumc*500,code1*1,dumc1*500
        integer ncut,over,ptpbin,nbin,ptplastbin,i,nout,iwksp(ncut)
        integer niter,noutold,f_sclip1(ncut),f_sclip2(ncut)
        integer f_sclip(ncut)
        real*8 bmaj(ncut),bmin(ncut),bpa(ncut),peak(ncut),im_rms(ncut)
        real*8 nmaj(ncut),nmin(ncut),snr(ncut),wksp(ncut),bm_pix(3)
        real*4 nmaj4(ncut),nmin4(ncut)
        real*8 s_c1(2),s_dm1(3),s_c2(2),s_dm2(3),ra(ncut),dec(ncut)
        real*8 s_med_f(3),s_rms_c_f(3),kappa,cbmaj,cbmin,cbpa,kappa2
        real*8 cdelt(3),snrcopy(ncut)
        
        do i=1,ncut
         nmaj(i)=bmaj(i)/(cbmaj*3600.d0)
         nmin(i)=bmin(i)/(cbmin*3600.d0)
         snr(i)=dlog10(peak(i)/im_rms(i))
         snrcopy(i)=snr(i)
         nmaj4(i)=nmaj(i)
         nmin4(i)=nmin(i)
        end do
c         snr(1)=4.0
c         snr(8)=3.7
c         snr(9)=4.1
        call sort3_2(ncut,snrcopy,nmin,wksp,iwksp)
        call sort3_2(ncut,snr,nmaj,wksp,iwksp)

        call call_sub_size(ncut,over,plotdir,fullname,nmaj,snr,s_med_f,
     /       s_rms_c_f,s_c1,s_dm1,kappa,kappa2,f_sclip1)
        call call_sub_size(ncut,over,plotdir,fullname,nmin,snr,s_med_f,
     /       s_rms_c_f,s_c2,s_dm2,kappa,kappa2,f_sclip2)

        dumc='oldmonk'
        dumc1=''
        code1='m'
        call plotqty_as_im(scratch,srldir,plotdir,filename,
     /        dumc,nmaj4,ra,dec,ncut,ncut,code1,bm_pix,cdelt,dumc1)

        do i=1,ncut
         f_sclip(iwksp(i))=f_sclip1(i)+f_sclip2(i)
        end do

        return
        end
c!
c!
c!
        subroutine call_sub_size(ncut,over,plotdir,fullname,y,x,
     /       s_med_f,s_rms_c_f,s_c,s_dm,kappa,kappa2,f_sclip)
        implicit none
        character plotdir*500,fullname*500,code*3
        integer ncut,over,ptpbin,nbin,ptplastbin,i,nout,iwksp(ncut)
        integer niter,noutold,f_sclip(ncut)
        real*8 nqty(ncut),snr(ncut),s_c(2),s_dm(3),wksp(ncut),dumr,med
        real*8 s_med_f(3),s_rms_c_f(3),kappa,kappa2,x(ncut),y(ncut)

        do i=1,ncut
         snr(i)=x(i)
         nqty(i)=y(i)
        end do

        nout=ncut
        niter=0
333     continue
        call get_bins(over,nout,ptpbin,nbin,ptplastbin)
        call sub_size_ksclip(plotdir,fullname,ncut,nout,nqty,snr,
     /    s_med_f,s_rms_c_f,over,ptpbin,nbin,ptplastbin,kappa,s_c,s_dm)
        noutold=nout
        call sub_size_wenss_getnum(ncut,nout,noutold,snr,nqty,s_c,s_dm,
     /       kappa2)
        niter=niter+1
        if (noutold.ne.nout) then
         if (niter.lt.10.and.nout.gt.0.75*ncut) goto 333
        end if
        write (*,*) ' Iterations = ',niter,' Fraction thrown away ',
     /              (ncut-nout)*1.d0/ncut

        do i=1,ncut
         dumr=sqrt(s_c(1)*s_c(1)+s_c(2)*s_c(2)/(10.d0**x(i))/
     /        (10.d0**x(i)))
         med=s_dm(1)+s_dm(2)*x(i)+s_dm(3)*x(i)*x(i)
         if (abs((y(i)-med)/(med*dumr)).le.kappa2) then
          f_sclip(i)=0
         else
          f_sclip(i)=1
         end if
        end do

        return
        end
c!
c!
c!
        subroutine sub_size_ksclip(plotdir,fullname,ncut,nout,yy,
     /      xx,s_med_f,s_rms_c_f,over,ptpbin,nbin,
     /      ptplastbin,kappa,s_c,s_dm)
        implicit none
        character plotdir*500,fullname*500,code*3
        integer ncut,over,ptpbin,nbin,ptplastbin,nout,nc,i,mx
        real*8 qty(nout),dumr,s_c(2),s_dm(3)
        real*8 s_med_f(3),s_rms_c_f(3),kappa,snr(nout),xfit(nbin)
        real*8 mean(nbin),emean(nbin),std(nbin),estd(nbin),s_cm(3)
        real*8 median(nbin),emedian(nbin),xval(nbin),yfit(nbin),med
        real*8 maxsnr,qtty(nout),yy(ncut),xx(ncut)
        real*4 mnx,mxx,mny,mxy

        code='YYN'
        maxsnr=0.d0
        do i=1,nout
         qtty(i)=yy(i)
         snr(i)=xx(i)
         if (snr(i).gt.maxsnr) maxsnr=snr(i)
        end do

c! not perfectly rt for noise estd - assume calculated std is real std
c! and error in median is zero for now.

        call bin_and_stats_ny(snr,qtty,nout,nout,ptpbin,nbin,ptplastbin,
     /       over,mean,emean,std,estd,median,emedian,xval,code,kappa)
c! fudge
        if (std(nbin)/median(nbin).gt.std(nbin-1)/median(nbin-1)) then
         std(nbin)=std(nbin-1)/median(nbin-1)*median(nbin)
        end if

        if (xval(1).lt.xval(nbin)) then
         mx=nbin
        else
         mx=1
        end if
        do i=1,nbin
         xfit(i)=10.d0**xval(i)
         yfit(i)=std(i)/median(i)
         estd(i)=std(i)/sqrt(2.d0*ptpbin)/median(mx)
         estd(i)=1.d0
        end do
        estd(nbin-1)=estd(1)*sqrt(1.d0*ptpbin/ptplastbin)
        estd(nbin)=estd(1)*sqrt(1.d0*ptpbin*over/ptplastbin)

        call pgbegin(0,'/xs',1,1)
        call sub_sub_aasl_1(nout,snr,qtty,1.d0,'L',' ',' ',' ',
     /       nbin,xval,median,std,mnx,mxx,mny,mxy)

        call callfit_aasl_1(nbin,xfit,yfit,estd,2,s_c,2) !  y^2=c1^2+c2^2/x^2
        do i=1,nbin
         estd(i)=1.d0
        end do
        call sub_callfit_aasl_1(nbin,xval,median,estd,1,3,s_dm,1) ! 2nd order
        call sub_callfit_aasl_1(nbin,xval,median,estd,3,2,s_cm,1) ! 1st order

        call plotfit_aasl_1(nbin,xfit,yfit,s_c(1),s_c(2),' ')
        call pgend

c! decide whether to take 1st or 2nd order poly for median vs snr.
        if (ptpbin.lt.75) then 
         s_dm(1)=s_cm(1)
         s_dm(2)=s_cm(2)
         s_dm(3)=0.d0
        end if
        write (*,*) 's_c = ',s_c
        write (*,*) 's_dm = ',s_dm
        write (*,*) 's_cm = ',s_cm

        return
        end
c!
c!
c!
        subroutine sub_size_wenss_getnum(ncut,nout,noutold,snr,qty,s_c,
     /             s_dm,kappa)
        implicit none
        integer ncut,nout,i,noutold
        real*8 snr(ncut),qty(ncut),snrold(noutold),qtyold(noutold)
        real*8 med,s_c(2),kappa,s_dm(3),dumr

        do i=1,noutold
         snrold(i)=snr(i)
         qtyold(i)=qty(i)
        end do

        nout=0
        do i=1,noutold
         dumr=sqrt(s_c(1)*s_c(1)+s_c(2)*s_c(2)/(10.d0**snrold(i))/
     /        (10.d0**snrold(i)))
         med=s_dm(1)+s_dm(2)*snrold(i)+s_dm(3)*snrold(i)*snrold(i)
         if (abs((qtyold(i)-med)/(med*dumr)).le.kappa) then
          nout=nout+1
          snr(nout)=snrold(i)
          qty(nout)=qtyold(i)
         end if
        end do
        
        return  
        end


