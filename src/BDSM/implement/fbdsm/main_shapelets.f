c! decompose image into cartesian shapelets. prelim prog

        subroutine main_shapelets(f1,scratch,srldir,runcode)
        implicit none
        character f1*500,extn*20,runcode*2,srldir*500,scratch*500
        integer nchar,n,m,l
        real*8 image1(2048,2048)

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        call processmain(f1,n,m,runcode)

        return
        end

c!
c! do all the stuff, easier this way cos u can specify arr(n,m)
c!
        subroutine processmain(f1,n,m,runcode)
        implicit none
        include "includes/trhermcoef.inc"
        integer n,m,nmax,num,i,j,nchar,dumi
        integer i1,j1,dumi2,round
        real*8 image1(n,m),arr(n,m),beta,cf(50,50)
        real*8 rarr(n,m),std,av,stdres,avres,nn,dum(n,m)
        real*8 xc,yc,xcen,ycen,stdr
        character fcf*500,filec*500,fileo*500,f1*500,dir*500,runcode*2
        character extn*20

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        num=20
333     continue
        write (*,'(1x,a18,i2,a4,$)') '  Beta and nmax (<',num+1,') : '
        read (*,*) beta,nmax
        if (nmax.gt.num) goto 333
        
        call sub_processmain(f1,image1,n,m,runcode,beta,nmax,nhc,hc)

        return
        end
c!
c!
c!
        subroutine sub_processmain(f1,image1,n,m,runcode,beta,
     /             nmax,nhc,hc)
        implicit none
        integer n,m,x,y,nmax,num,i,j,nchar,dumi
        integer i1,j1,dumi2,round,nhc
        real*8 image1(n,m),arr(n,m),beta,cf(nmax,nmax)
        real*8 rarr(n,m),std,av,stdres,avres,nn,dum(n,m)
        real*8 xc,yc,xcen,ycen,stdr,hc(nhc,nhc)
        character fcf*500,filec*500,fileo*500,f1*500,dir*500,runcode*2

        do 100 i=1,n
         do 110 j=1,m
          arr(i,j)=image1(i,j)
110      continue
100     continue


        call sigclip(arr,n,m,1,1,n,m,std,av,5.d0)
        call s_findcen(arr,n,m,hc,nhc,beta,nmax,num,xcen,ycen,
     /       std,'y',runcode)
        xcen=round(xcen*1.d2)/1.d2
        ycen=round(ycen*1.d2)/1.d2
        write (*,*) '  Found centre = ',xcen,ycen

        write (*,'(1x,a27,$)') '  Central x and y pixels : '
        read (*,*) xc,yc

        call initialiseimage(cf,nmax,nmax,nmax,nmax,0.d0)
        call decomp_cart(arr,n,m,hc,beta,xc,yc,nmax,nhc,cf)  ! get coeffs by integrating

        call recons_cart(n,m,hc,beta,xc,yc,nmax,cf,rarr,nhc)

        do 300 i=1,n
         do 310 j=1,m
          dum(i,j)=arr(i,j)-rarr(i,j)
310      continue
300     continue

c! rms in fitted region (4 sigma)
        stdres=0.d0
        avres=0.d0
        nn=0.d0
        do 200 i=1,n
         do 210 j=1,m
          if (arr(i,j).ge.2.d0*std) then
           nn=nn+1.d0
           avres=avres+(arr(i,j)-rarr(i,j)) 
           stdres=stdres+((arr(i,j)-rarr(i,j))**2.d0)
          end if
210      continue
200     continue
        avres=avres/nn
        stdres=sqrt(stdres/nn - avres*avres)

        call arrstat(dum,n,m,1,1,n,m,stdr,av)
        write (*,*) '  The Full Image std = ',std,' Jy'
        write (*,*) '  Residual image std = ',stdr,' Jy'
        write (*,*) '  Residual (>2s) std = ',stdres,' Jy'

        filec='coeff'
        fileo=f1(1:nchar(f1))//'.recons'
        call writearray_bin2D(cf,nmax,nmax,nmax,nmax,filec,runcode)
        call writearray_bin2D(rarr,n,m,n,m,fileo,runcode)
        call s_displayshap(f1,fileo,filec)

        return
        end


