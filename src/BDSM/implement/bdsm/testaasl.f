
        implicit none
        integer i,nbin
        parameter (nbin=39)
        real*8 med(nbin),std(nbin),estd(nbin),xval(nbin)
        real*8 xfit(nbin),yfit(nbin),s_c(2)
        integer nc,round,nchar,dumi
        real*4 x4(nbin),y4(nbin),mnx,mxx,mny,mxy
        character sc1*16,sc2*16,xlab*30,stuff*500


        open(unit=21,file='k')
        do i=1,nbin
         read (21,*) xval(i),med(i),std(i),estd(i)
         xfit(i)=10.d0**xval(i)
         yfit(i)=std(i)/med(i)
         estd(i)=1.d0
         estd(i)=std(i)/sqrt(2.d0*462)/med(nbin)
        end do
        close(21)
        call callfit_aasl_1(nbin,xfit,yfit,estd,2,s_c,2)
        write (*,*) s_c

        call vec8to4(xfit,nbin,x4,nbin)
        call vec8to4(std,nbin,y4,nbin)
        call range_vec4mxmn(x4,nbin,nbin,mnx,mxx)
        call range_vec4mxmn(y4,nbin,nbin,mny,mxy)
        mnx=log10(x4(1)*0.8)
        mxx=log10(mxx)
        
        call pgbegin(0,'/xs',1,1)

        call pgvport(0.1,0.90,0.1,0.9)
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
        call pgline(nbin,x4,y4)

        do i=1,nbin
         y4(i)=sqrt(s_c(1)*s_c(1)+s_c(2)*s_c(2)/xfit(i)/xfit(i))
        end do
        call pgline(nbin,x4,y4)

        call pgnumb(abs(round(s_c(1)*1.d4)),-4,1,sc1,nc)
        call pgnumb(abs(round(s_c(2)*1.d4)),-4,1,sc2,nc)
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

        call pgend

        end
