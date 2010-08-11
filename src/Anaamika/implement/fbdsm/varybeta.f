c! takes an image, does shapelet decomp using values of beta, recons, plots noise as fn of beta.

        subroutine varybeta(f1,scratch,srldir,runcode)
        implicit none
        character f1*500,extn*20,scratch*500,srldir*500,runcode*2
        integer nchar,n,m,l

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        call processvarybeta(f1,n,m,runcode)
        
        return
        end


        subroutine processvarybeta(f1,n,m,rc)
        implicit none
        include "includes/trhermcoef.inc"
        integer n,m,x,y,nmax,num,i,j,nchar,dumi,ibeta
        integer i1,j1,dumi2,nbin
        real*8 arr(n,m),beta(100),cf(50,50)
        real*8 rarr(n,m),std,av,stdres(100),avres,nn,dum(n,m)
        real*8 xc,yc,xcen,ycen,betadum,delta,power(100)
        character fcf*500,filec*500,fileo*500,f1*500,extn*20,dir*500
        character xlab*30,ylab1*30,ylab2*30,ylab3*30,code*6,rc*2

        write (*,'(1x,a17,$)') '  Nominal Beta : '
        read (*,*) betadum

333     continue
        num=20
        write (*,'(1x,a15,i2,a4,$)') '  Enter nmax (<',num+1,') : '
        read (*,*) nmax
        nmax = nmax + 1  ! because thats what i assume in python. Now fortran = python exactly
        if (nmax.gt.num) goto 333
        
        call sub_processvarybeta(f1,n,m,nhc,hc,betadum,nmax,rc)

        return
        end
c!
c!
c!
        subroutine sub_processvarybeta(f1,n,m,nhc,hc,betadum,nmax,rc)
        implicit none
        integer n,m,x,y,nmax,num,i,j,nchar,dumi,ibeta,nhc
        integer i1,j1,dumi2,nbin,minind
        real*8 arr(n,m),beta(100),cf(nmax,nmax),hc(nhc,nhc)
        real*8 rarr(n,m),std,av,stdres(100),avres,nn,dum(n,m)
        real*8 xc,yc,xcen,ycen,betadum,delta,power(100),minbeta
        character fcf*500,filec*500,fileo*500,f1*500,extn*20,dir*500
        character xlab*30,ylab1*30,ylab2*30,ylab3*30,code*6,rc*2,ff*500

        extn='.img'
        call readarray_bin(n,m,arr,n,m,f1,extn)

        call sigclip(arr,n,m,1,1,n,m,std,av,5.d0)
        call s_findcen(arr,n,m,hc,nhc,betadum,nmax,num,xcen,ycen,
     /       std,av,'mv')
        write (*,*) '  Found centre = ',xcen,ycen

        write (*,'(1x,a27,$)') '  Central x and y pixels : '
        read (*,*) xc,yc

        call initialiseimage(cf,nmax,nmax,nmax,nmax,0.d0)
        nbin=100
        delta=(4.0*betadum-betadum/2.0)/nbin
        do 400 ibeta=1,nbin
         beta(ibeta)=betadum/4.0+(ibeta-1)*delta
         call decomp_cart(arr,n,m,hc,beta(ibeta),xc,yc,nmax,nhc,cf)

         call recons_cart(n,m,hc,beta(ibeta),xc,yc,nmax,cf,rarr,nhc)
         call coeffpower(cf,nmax,power(ibeta))
         power(ibeta)=cf(1,1)*cf(1,1)/power(ibeta)

         do 300 i=1,n
          do 310 j=1,m
           dum(i,j)=arr(i,j)-rarr(i,j)
310       continue
300      continue

c! rms in fitted region (4 sigma)
           std = -9.d90
           write (*,*) 'std = ',std
         stdres(ibeta)=0.d0
         avres=0.d0
         nn=0.d0
         do 200 i=1,n
          do 210 j=1,m
           if (rarr(i,j).ge.1.d0*std) then
            nn=nn+1.d0
            avres=avres+dum(i,j)
            stdres(ibeta)=stdres(ibeta)+(dum(i,j)**2.d0)
           end if
210       continue
200      continue
         avres=avres/nn
         stdres(ibeta)=sqrt(stdres(ibeta)/nn - avres*avres)
 
         call arrstat(dum,n,m,1,1,n,m,std,av)
        
c         filec='coeff'
c         fileo=f1(1:nchar(f1))//'.recons'
c         call writearray(cf,50,50,nmax,nmax,filec)
c         call writearray(rarr,n,m,n,m,fileo)
c         call s_displayshap(f1,fileo,filec)
c         pause

400     continue
        xlab='Beta'
        ylab1='rms in residual > 2sigma'
        ylab2='Relative power in (0,0) comp'
        code='LPLP'
        call plotcurves(2,100,nbin,beta,stdres,power,stdres,xlab,
     /       ylab1,ylab2,ylab1,code)

        do ibeta=1,nbin
         write (*,*) beta(ibeta),stdres(ibeta)
        end do

c! get min beta
        minind=1
        do ibeta=2,nbin
         if (stdres(ibeta).lt.stdres(minind)) minind=ibeta
        end do
        minbeta=beta(minind)
        write (*,*) '  Minimum beta = ',minbeta

        return
        end



