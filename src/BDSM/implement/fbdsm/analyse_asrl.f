
        subroutine analyse_asrl(scrat,scratch,srldir)
        implicit none
        character scrat*500,scratch*500,srldir*500
        character fn*500,extn*20,cmd*500,keyword*500,comment*500
        character keystrng*500,master*500,sec*500,dums*500
        integer nchar,nlines,i,ind,error,nbin,npairs,ptpbin
        integer ptplastbin,ind1,dumi,over
        real*8 rmsclip,thresh_pix,thresh_pix_ini,bmaj,bmin

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        cmd='wc -l '//fn(1:nchar(fn))//' > '//
     /       scratch(1:nchar(scratch))//'aa'
        call system(cmd)
        open(unit=31,file=scratch(1:nchar(scratch))//'aa',status='old')
        read(31,*) nlines
        close(31)
        nlines=nlines-1 ! first line is of freqs
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')

c! get rms_clip from sec file header. do this properly later.
        do i=nchar(scrat),1,-1
         if (scrat(i:i).eq.'.') then
          ind=i
          goto 100
         end if
        end do 
100     continue
        do i=1,ind-2
         if (scrat(i:i+1).eq.'__') then
          ind1=i
          goto 110
         end if
        end do 
110     continue
        sec=scrat(ind1+2:nchar(scrat))
        master=scrat(1:ind1-1)
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(sec,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch,error)
        extn='.bparms'
        keyword='thresh_pix'
        call get_keyword(sec,extn,keyword,keystrng,thresh_pix,
     /       comment,'r',scratch,error)

        extn='.header'
        keyword='BMAJ'
        call get_keyword(sec,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch,error)
        keyword='BMIN'
        call get_keyword(sec,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch,error)
        fn=srldir(1:nchar(srldir))//master(1:nchar(master))//'.gaul'
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,16,'Detect_threshold',dums,
     /       dumi,thresh_pix_ini,'r')
        close(22)

        if (mod(nlines,2).ne.0) then
         write (*,*) ' !!!!!  ERROR !!!'
        else
c! Bin for now. fix number of pts per bin first (is a constant). over is supernyquistrate*2
c! that is, over=2 is nyquist.
         over=2
         npairs=nlines/2
         call get_bins(over,npairs,ptpbin,nbin,ptplastbin)

         call sub_analyse_asrl1(scrat,npairs,scratch,rmsclip,
     /        ptpbin,nbin,ptplastbin,over,thresh_pix,thresh_pix_ini,
     /        bmaj,bmin)
        end if
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_analyse_asrl1(scrat,npairs,scratch,rmsclip,
     /        ptpbin,nbin,ptplastbin,over,thresh_pix,thresh_pix_ini,
     /        bmaj,bmin)
        implicit none
        include "constants.inc"
        character scrat*500,fn*500,extn*20,label(40)*30,scratch*500
        character chr1*1,xlab*30,ylab*30,dumc*50
        integer nchar,npairs,i,idm(npairs),ids(npairs),id(2),over
        integer iquit,ptpbin,nbin,ptplastbin,iwksp(npairs)
        real*8 var(40,npairs),rmsclip,wksp(npairs),xfit(nbin)
        real*8 xplot(npairs),yplot(npairs),kappa,c1,c2
        real*8 thresh_pix_ini,yfit(nbin),bmaj,bmin,tol
        real*8 mean(nbin),emean(nbin),std(nbin),estd(nbin)
        real*8 median(nbin),emedian(nbin),xval(nbin),thresh_pix
        real*4 mnx,mxx,mny,mxy
        
        data label/'Peak (srl 1) ','R.A. (srl 1)','Dec (srl 1)',
     /    'Bmaj (srl 1)','Bmin (srl 1)','Bpa (srl 1)','Peak (srl 2)',
     /    'R.A. (srl 2)','Dec (srl 2)','Bmaj (srl 2)','Bmin (srl 2)',  ! 11
     /    'Bpa (srl 2)','Distance ','Peak ratio (srl 1/2)',
     /    'Bpa diff (srl 1-2)','Bmaj ratio (srl 1/2)',
     /    'Bmin ratio (srl 1/2)','Src id (srl 1)','Src id (srl 2)',    ! 19
     /    'src residrms','isl residrms','Total (srl 1)','eTot (srl 1)',
     /    'ePeak (srl 1)','Total (srl 2)','eTot (srl 2)',
     /    'ePeak (srl 2)','Total ratio (srl 1/2)','RAdiff (srl 1-2)',
     /    'DECdiff (srl 1-2)','','','','','','','','','',''/

        extn='.assf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        dumc=''
        open(unit=21,file=fn(1:nchar(fn)),status='old')
333     read (21,*) dumc
        if (dumc.eq.'Tolerance') then
         backspace(21)
         read (21,*) dumc,dumc,dumc,dumc,tol
        else
         goto 333
        end if

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old')
        read (21,*)
        do i=1,npairs
         read (21,*) var(18,i),var(22,i),var(23,i),var(1,i),var(24,i),
     /    var(2,i),var(3,i),var(4,i),
     /    var(5,i),var(6,i),var(20,i),var(21,i)
         read (21,*) var(19,i),var(25,i),var(26,i),var(7,i),var(27,i),
     /    var(8,i),var(9,i),var(10,i),var(11,i),var(12,i),var(13,i)

         var(15,i)=var(6,i)-var(12,i)
         var(16,i)=var(10,i)/var(4,i)
         var(17,i)=var(11,i)/var(5,i)
         var(14,i)=var(7,i)/var(1,i)
         var(31,i)=var(1,i)-var(7,i)
         var(28,i)=var(25,i)/var(22,i)
         var(29,i)=(var(2,i)-var(8,i))*3600.d0
         var(30,i)=(var(3,i)-var(9,i))*3600.d0
         var(32,i)=var(10,i)*var(11,i)/(var(4,i)*var(5,i))*
     /             cos(var(15,i)/rad)
        end do
        close(21)

c!
c! Now : true flux-detected flux vs detected SNR

        do i=1,npairs
         xplot(i)=dlog10(var(7,i)/rmsclip)
         yplot(i)=var(31,i)
        end do 
        call sort3_2(npairs,xplot,yplot,wksp,iwksp)
        xlab='Detected SNR'
        ylab='(1\\ust\\d-2\\und\\d) peak flux'
c        ylab='(True-detected) peak flux'
        kappa=3.0d0

        call pgbegin(0,'?',1,1)
        call sub_analyse_aasl_1(xplot,yplot,npairs,xlab,ylab,'L',' ',
     /       0.d0,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,std,estd,
     /       median,emedian,xval,mnx,mxx,mny,mxy)
        call sub_analyse_asrl_notes(scrat,tol)
        do i=1,nbin
         estd(i)=1.d0
         xfit(i)=10.d0**xval(i)
         yfit(i)=std(i)/xfit(i)
        end do
        xfit(nbin)=1.d3
        call callfit_aasl_1(nbin,xfit,yfit,estd,c1,c2)
        call plotfit_aasl_1(nbin,xfit,yfit,c1,c2,xlab)
        call plotfit_aasl_1_2(nbin,xfit,mean,rmsclip,xlab)
        call pgend
        write (*,*) '  Press any key for next plot ... '
        read (*,*)

c!
c! Now : peak flux ratio vs detected SNR

        do i=1,npairs
         xplot(i)=dlog10(var(7,i)/rmsclip)
         yplot(i)=var(14,i)
        end do 
        call sort3_2(npairs,xplot,yplot,wksp,iwksp)
        xlab='Detected SNR'
        ylab='Peak flux ratio'
        kappa=3.0d0

        call pgbegin(0,'?',1,1)
        call sub_analyse_aasl_1(xplot,yplot,npairs,xlab,ylab,'L',' ',
     /       0.d0,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,std,estd,
     /       median,emedian,xval,mnx,mxx,mny,mxy)
        call plot_thlimit_aasl_2_1(thresh_pix_ini,thresh_pix,
     /       mnx,mxx,mny,mxy,'r')
        do i=1,nbin
         estd(i)=1.d0
         xfit(i)=10.d0**xval(i)
        end do
        xfit(nbin)=1.d3
        call callfit_aasl_1(nbin,xfit,std,estd,c1,c2)
        call plotfit_aasl_1(nbin,xfit,std,c1,c2,xlab)
        call plotfit_aasl_1_2(nbin,xfit,mean,rmsclip,xlab)
        call pgend
        write (*,*) '  Press any key for next plot ... '
        read (*,*)

c! 
c! Now : peak flux ratio vs input SNR
        do i=1,npairs
         xplot(i)=dlog10(var(1,i)/rmsclip)
         yplot(i)=var(14,i)
        end do 
        call sort3_2(npairs,xplot,yplot,wksp,iwksp)
        xlab='Input SNR'
        ylab='Peak flux ratio'
        kappa=3.0d0

        call pgbegin(0,'?',1,1)
        call sub_analyse_aasl_1(xplot,yplot,npairs,xlab,ylab,'L',' ',
     /       0.d0,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,std,estd,
     /       median,emedian,xval,mnx,mxx,mny,mxy)
c! plot limit
        call plot_thlimit_aasl_2_1(thresh_pix,thresh_pix_ini,
     /       mnx,mxx,mny,mxy,' ')
        do i=1,nbin
         estd(i)=1.d0
         xfit(i)=10.d0**xval(i)
        end do
        xfit(nbin)=1.d3
        call callfit_aasl_1(nbin,xfit,std,estd,c1,c2)
        call plotfit_aasl_1(nbin,xfit,std,c1,c2,xlab)
        call plotfit_aasl_1_2(nbin,xfit,mean,rmsclip,xlab)
        call pgend
        write (*,*) '  Press any key for next plot ... '
        read (*,*)

c! Now : true bmaj-detected bmaj vs detected SNR

        do i=1,npairs
         xplot(i)=dlog10(var(7,i)/rmsclip)
         yplot(i)=(var(10,i)/var(4,i))
        end do 
        call sort3_2(npairs,xplot,yplot,wksp,iwksp)
        xlab='Detected SNR'
        ylab='Detected/True Major axis FWHM'
        kappa=3.0d0

        call pgbegin(0,'?',1,1)
        call sub_analyse_aasl_1(xplot,yplot,npairs,xlab,ylab,'L',' ',
     /       1.d0,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,std,estd,
     /       median,emedian,xval,mnx,mxx,mny,mxy)
        call sub_analyse_asrl_notes(scrat,tol)
        do i=1,nbin
         estd(i)=1.d0
         xfit(i)=10.d0**xval(i)
         yfit(i)=std(i)/mean(i)
        end do
        xfit(nbin)=1.d3
        call callfit_aasl_1(nbin,xfit,yfit,estd,c1,c2)
        call plotfit_aasl_1(nbin,xfit,yfit,c1,c2,xlab)
        call plotfit_aasl_1_2(nbin,xfit,mean,rmsclip,xlab)
        call pgend
        write (*,*) '  Press any key for next plot ... '
        read (*,*)


c! Now : true RA-detected RA vs detected SNR

        do i=1,npairs
         xplot(i)=dlog10(var(7,i)/rmsclip)
         yplot(i)=(var(2,i)-var(8,i))/bmaj
        end do 
        call sort3_2(npairs,xplot,yplot,wksp,iwksp)
        xlab='Detected SNR'
        ylab='True-Detected RA /beam'
        kappa=3.0d0

        call pgbegin(0,'?',1,1)
        call sub_analyse_aasl_1(xplot,yplot,npairs,xlab,ylab,'L',' ',
     /       1.d0,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,std,estd,
     /       median,emedian,xval,mnx,mxx,mny,mxy)
        call sub_analyse_asrl_notes(scrat,tol)
        do i=1,nbin
         estd(i)=1.d0
         xfit(i)=10.d0**xval(i)
         yfit(i)=std(i)
        end do
        xfit(nbin)=1.d3
        call callfit_aasl_1(nbin,xfit,yfit,estd,c1,c2)
        call plotfit_aasl_1(nbin,xfit,yfit,c1,c2,xlab)
        call plotfit_aasl_1_2(nbin,xfit,mean,rmsclip,xlab)
        call pgend
        write (*,*) '  Press any key for next plot ... '
        read (*,*)


        return
        end
c!
c! ----------------------------------------------------------------------------------------
c!
c!
c!  plots rms vs snr, and overplots the 'wenss' fits
c!
        subroutine plotfit_aasl_1(nbin,xval,std,c1,c2,xlab)
        implicit none
        integer nbin,i,nc,round,nchar,dumi
        real*8 xval(nbin),std(nbin),c1,c2
        real*4 x4(nbin),y4(nbin),mnx,mxx,mny,mxy
        character sc1*16,sc2*16,xlab*30,stuff*500

        call vec8to4(xval,nbin,x4,nbin)
        call vec8to4(std,nbin,y4,nbin)
        call range_vec4mxmn(x4,nbin,nbin,mnx,mxx)
        call range_vec4mxmn(y4,nbin,nbin,mny,mxy)
        mnx=log10(x4(1)*0.8)
        mxx=log10(mxx)
        
        call pgvport(0.1,0.50,0.1,0.4)
c        mny=-0.00759
c        mxy=0.139
        call pgwindow(mnx,mxx,mny,mxy)
c         write (*,*) 'rms win ',mny,mxy
        call pgbox('BCNSTL',0.0,0,'BCNST',0.0,0)
        do i=1,nbin
         x4(i)=log10(x4(i))
        end do
        call pglabel(xlab(1:nchar(xlab)),'Error/Mean',' ')
        call pgpoint(nbin,x4,y4,17)

        do i=1,nbin
         y4(i)=sqrt(c1*c1+c2*c2/xval(i)/xval(i))
        end do
        call pgline(nbin,x4,y4)

        call pgnumb(abs(round(c1*1.d4)),-4,1,sc1,nc)
        call pgnumb(abs(round(c2*1.d4)),-4,1,sc2,nc)
        call pgsch(0.8)
        call pgtext(mnx+0.3*(mxx-mnx),mny+0.7*(mxy-mny),
     /    'c1 = '//sc1(1:nchar(sc1))//'; c2 = '//sc2(1:nchar(sc2)))
        stuff='(\\gs\\dS\\u/S=(c\\d1\\u\\u2\\d+c\\d2\\u\\u2'//  ! for peak
     /        '\\d\\gs\\drms\\u\\u2\\d/S\\u2\\d)\\u0.5\\d)'
        stuff='(\\gs\\dRA\\u=(c\\d1\\u\\u2\\d+c\\d2\\u\\u2'//  ! for RA
     /        '\\d\\gs\\drms\\u\\u2\\d/S\\u2\\d)\\u0.5\\d)'
        stuff='(\\gs\\db\\u/b=(c\\d1\\u\\u2\\d+c\\d2\\u\\u2'//  ! for bmaj
     /        '\\d\\gs\\drms\\u\\u2\\d/S\\u2\\d)\\u0.5\\d)'
        call pgtext(mnx+0.3*(mxx-mnx),mny+0.6*(mxy-mny),stuff)
        call pgsch(1.0)

        return
        end
c!
c!  plots mean vs snr and does things later
c!
        subroutine plotfit_aasl_1_2(nbin,xval,mean,rmsclip,xlab)
        implicit none
        integer nbin,i,nc,round,nchar
        real*8 xval(nbin),mean(nbin),c1,c2,rmsclip
        real*4 x4(nbin),y4(nbin),mnx,mxx,mny,mxy,x41(100),y41(100)
        character xlab*30

        do i=1,nbin
         x4(i)=log10(xval(i))
         y4(i)=mean(i)/xval(i)*100.0
         y4(i)=mean(i)
        end do
        call range_vec4mxmn(x4,nbin,nbin,mnx,mxx)
        call range_vec4mxmn(y4,nbin,nbin,mny,mxy)
        mnx=mnx-0.1
        
        call pgvport(0.59,0.99,0.1,0.4)
c        mny=-10.061
c        mxy=1.315 ! for sad to be same as bdsm 
c        mny=-0.01759
c        mxy=0.0115
        call pgwindow(mnx,mxx,mny,mxy)
c         write (*,*) 'mean win ',mny,mxy
        call pgbox('BCNSTL',0.0,0,'BCNST',0.0,0)
        call pglabel(xlab(1:nchar(xlab)),'Mean',' ')
        call pgpoint(nbin,x4,y4,17)

        return
        end
c!
c!  overplots theoretical limit of peak ratio vs input snr on 1st plot
c!
        subroutine plot_thlimit_aasl_2_1(thresh_pix,tpi,mnx,mxx,
     /             mny,mxy,code)
        implicit none
        real*4 mnx,mxx,mny,mxy,x4(100),y4(100),dumr4
        real*8 thresh_pix,tpi
        integer i
        character code*1

        do i=1,100
         x4(i)=mnx+(i-1)*0.01*(mxx-mnx)
         y4(i)=thresh_pix/(10.0**x4(i))
         if (code.eq.'r') y4(i)=1.0/y4(i)
        end do
        call pgsci(5)
        call pgline(100,x4,y4)
        dumr4=log10(tpi)
        call pgmove(dumr4,mny)
        call pgdraw(dumr4,mxy)
        call pgsci(1)
        
        return
        end

        subroutine sub_analyse_asrl_notes(scrat,tol)
        implicit none
        character scrat*500,str1*50
        integer nchar,mm,pp,nc
        real*8 tol
        real*4 tol4

        call pgvport(0.65,0.95,0.51,0.99)
        call pgwindow(0.0,1.0,0.0,1.0)
        call pgsch(1.1)
        call pgtext(0.0,0.9,'File : '//scrat(1:nchar(scrat)))
        call pgtext(0.0,0.8,'Pink/green : Mean; '//
     /       'Yellow/blue : '//char(177)//'1\\gs')
        tol4=tol
        call get_pgnumb(tol4,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.0,0.7,'Association search radius : '//
     /       str1(1:nc)//'"')
        call pgsch(1.0)
       
        return
        end




