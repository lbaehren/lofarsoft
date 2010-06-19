c! called by aasl. plots the points, calculates running mean, rms, median. checks these.
c! then fits the wenss formula.

        subroutine sub_analyse_aasl_1(xplot,yplot,n,xlab,ylab,
     /       xl,yl,base,kappa,ptpbin,nbin,ptplastbin,over,mean,emean,
     /       std,estd,median,emedian,xval,mnx,mxx,mny,mxy)
        implicit none
        integer n,nchar,ptpbin,nbin,i,ptplastbin,over
        real*8 xplot(n),yplot(n),kappa
        character xlab*30,ylab*30,xl,yl,boxx*6,boxy*6
        real*8 base,mean(nbin),emean(nbin),std(nbin),estd(nbin)
        real*8 median(nbin),emedian(nbin),xval(nbin)
        real*4 mnx,mxx,mny,mxy
        
        call bin_and_stats_ny(xplot,yplot,n,n,ptpbin,nbin,ptplastbin,
     /     over,mean,emean,std,estd,median,emedian,xval,'YYN',kappa)

        call sub_sub_aasl_1(n,xplot,yplot,base,xl,yl,xlab,ylab,
     /       nbin,xval,mean,std,mnx,mxx,mny,mxy)

        return
        end
c!
c!
c!
        subroutine sub_sub_aasl_1(n,xplot,yplot,base,xl,yl,xlab,ylab,
     /       nbin,xval,mean,std,mnx,mxx,mny,mxy)
        implicit none
        integer n,nchar,ptpbin,nbin,i,ptplastbin,over
        real*8 xplot(n),yplot(n),kappa
        real*4 xplot4(n),yplot4(n),mnx,mxx,mny,mxy,base4
        character xlab*30,ylab*30,xl,yl,boxx*6,boxy*6
        real*8 base,mean(nbin),emean(nbin),std(nbin),estd(nbin)
        real*8 median(nbin),emedian(nbin),xval(nbin)
        real*4 x4(nbin),y4(nbin),y41(nbin),y42(nbin)

        do i=1,n
         xplot4(i)=xplot(i)
         yplot4(i)=yplot(i)
        end do
        call range_vec4mxmn(xplot4,n,n,mnx,mxx)
        call range_vec4mxmn(yplot4,n,n,mny,mxy)
        base4=base
        boxx='BCNST'
        boxy='BCNST'
        if (xl.eq.'L') boxx(6:6)='L'
        if (yl.eq.'L') boxy(6:6)='L'
        call pgvport(0.10,0.60,0.51,0.99)
c         mny=-5.22
c         mxy=9.82
c        mny= 0.1
c        mxy= 4.0
        call pgwindow(mnx,mxx,mny,mxy)
c         write (*,*) 'main win ',mny,mxy
        call pgbox(boxx(1:nchar(boxx)),0.0,0,boxy(1:nchar(boxy)),0.0,0)
        call pglabel(xlab(1:nchar(xlab)),ylab(1:nchar(ylab)),' ')
        call pgpoint(n,xplot4,yplot4,3)
        call pgsci(2)
        call pgmove(mnx,base4)
        call pgdraw(mxx,base4)
        call pgsci(1)

        do i=1,nbin
         x4(i)=xval(i)
         y4(i)=mean(i)
         y41(i)=mean(i)-std(i)
         y42(i)=mean(i)+std(i)
        end do
        call pgsci(3)
        call pgpoint(nbin,x4,y4,3)
        call pgline(nbin,x4,y4)
        call pgsci(4)
        call pgpoint(nbin,x4,y41,3)
        call pgline(nbin,x4,y41)
        call pgpoint(nbin,x4,y42,3)
        call pgline(nbin,x4,y42)
        call pgsci(1)
        
        return
        end


