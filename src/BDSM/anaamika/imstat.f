c! this is to get image stats as called by noise.f
c! img is i/p image, to take n,m from it, chr1 is AB
c! A is q or v for quiet or verbose and B is y/n for plot or not
c! This fits gaussian to all the data. Direct computation of mean and std
c! is done in arrstat.f. There is also sigclip.f

        subroutine imstat(img,x,y,n,m,a,chr1)
        implicit none
        integer n,m,i,j,nbin,i1,ia(3),x,y
        real*8 img(x,y),mid(100),a(3),chisq,std,av
        real*8 mn,mx,mn1,mx1,delta,sig(100),histo(100)
        real*8 low(100),high(100),alamda,covar(3,3),alpha(3,3)
        real*4 mn4,mx4,x4(100),y4(100),image(n,m),y41(100)
        real*4 mn41,mx41,mn42,mx42,dum4
        character chr1*2

        data ia/1,1,1/

        call array8to4(img,x,y,image,n,m)
        call minmaxarr8(img,x,y,1,1,n,m,mn,mx)
        mn4=mn
        mx4=mx

        nbin=int(sqrt(1.0*n*m))
        if (nbin.gt.100) nbin=100
        delta=(mx-mn)/(nbin-1)

        do 140 i=1,nbin
         low(i)=mn+(i-1)*delta
         high(i)=low(i)+delta
         mid(i)=low(i)+0.5d0*delta
         histo(i)=0
140     continue

        a(1)=0
        do 120 j=1,m
         do 130 i=1,n
          i1=int((img(i,j)-mn)/delta)+1
          histo(i1)=histo(i1)+1
          if (a(1).lt.histo(i1)) then
           a(1)=histo(i1)*1.1
           a(2)=img(i,j)
          end if
130      continue
120     continue
        call arrstat(img,x,y,1,1,n,m,std,av)
        a(3)=std*1.1

        do 200 i=1,nbin
         x4(i)=mid(i)
         y4(i)=a(1)*dexp((-1.d0)*((x4(i)-a(2))**2.d0)/
     /         (2.d0*a(3)*a(3)))
         sig(i)=1.d0
200     continue

        if (chr1(2:2).eq.'y') then
         write (*,'(a21,$)') '   Plot of fit <Ret> '
         read (*,*)
         i=n*m
         call pgbegin(0,'/NULL',1,1)
         call pghist(i,image,mn4,mx4,nbin,0)
         call pgqwin(mn41,mx41,mn42,mx42)
         call pgend

         call pgbegin(0,'/xs',1,1)
         call pgvport(0.1,0.9,0.3,0.9)
         dum4=a(1)*1.1
         call pgwindow(mn41,mx41,mn42,dum4)
         call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
         call pghist(i,image,mn4,mx4,nbin,1)
         call pgsls(2)
         call pgline(nbin,x4,y4)
         call pgsls(1)
        end if

        alamda=-1.d0
        do 420 i=1,20
         call mrqmin(mid,histo,sig,nbin,nbin,a,ia,3,covar,alpha,3,
     /       chisq,alamda,1)
420     continue
        alamda=0.d0
        call mrqmin(mid,histo,sig,nbin,nbin,a,ia,3,covar,alpha,3,
     /       chisq,alamda,1)

        do 210 i=1,nbin
         y41(i)=a(1)*dexp((-1.d0)*((x4(i)-a(2))**2.d0)/
     /         (2.d0*a(3)*a(3)))
210     continue

        if (chr1(1:1).eq.'v') then
         write (*,*) '  computing (gaussian) image statistics ... '
         write (*,777) '  Peak = ',a(1),', Histo fitted RMS = ',a(3),
     /               ' Centre = ',a(2)
         write (*,*) '  Max and min are ',(mx-a(2))/a(3),' & ',
     /         (mn-a(2))/a(3),' sigma resp.'
        end if
777     format(1x,a9,e14.6,a20,e14.6,a10,e14.6)

        if (chr1(2:2).eq.'y') then
         call pgline(nbin,x4,y41)

         do 400 i=1,nbin
          y4(i)=histo(i)-y41(i)
400      continue
         call pgvport(0.1,0.9,0.1,0.3)
         call range2(y4,100,nbin,mn41,mx41)
	 call pgwindow(mn4,mx4,mn41,mx41)
	 call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
         call pgline(nbin,x4,y4)
 
         call pgend
        end if

        return
        end



