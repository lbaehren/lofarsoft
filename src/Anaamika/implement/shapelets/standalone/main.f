c! decompose image into cartesian shapelets. prelim prog


        implicit none
        character f1*40,extn*10
        integer nchar,n,m,nhc
        real*8 image1(2048,2048)

        write (*,'(a19,$)') '  Input filename : '
        read (*,*) f1
        extn='.img'
        nhc=31
        call readarray_bin(n,m,image1,2048,2048,f1,extn)
        call processmain(f1,image1,2048,2048,n,m,nhc)

        end

c!
c! do all the stuff, easier this way cos u can specify arr(n,m)
c!
        subroutine processmain(f1,image1,x,y,n,m,nhc)
        implicit none
        integer n,m,x,y,nmax,num,i,j,nchar,dumi,nhc
        integer i1,j1,dumi2,round
        real*8 image1(x,y),arr(n,m),beta,hc(nhc,nhc),cf(nhc,nhc)
        real*8 rarr(n,m),std,av,stdres,avres,nn,dum(n,m)
        real*8 xc,yc,xcen,ycen
        character fcf*8,filec*40,fileo*40,f1*40

        do 100 i=1,n
         do 110 j=1,m
          arr(i,j)=image1(i,j)
110      continue
100     continue

c! read cartesian hermite coeffs (upto num, currently 31)
        do 105 i=1,nhc
         do 108 j=1,nhc
          hc(i,j)=0.d0
108      continue
105     continue

        fcf='hermcoef'
        call getline(fcf,num)
        open(unit=21,file=fcf,status='old')
        call readinarray(hc,nhc,num,21)
        close(21)

333     continue

        write (*,'(a18,i2,a4,$)') '  Beta and nmax (<',num+1,') : '
        read (*,*) beta,nmax
        if (nmax.gt.num) goto 333

        call sigclip(arr,n,m,n,m,std,av,5)
        call s_findcen(arr,n,m,hc,beta,nmax,num,xcen,ycen,std,'y')
        xcen=round(xcen*1.d2)/1.d2
        ycen=round(ycen*1.d2)/1.d2
        write (*,*) ' Found centre = ',xcen,ycen

        write (*,'(a27,$)') '  Central x and y pixels : '
        read (*,*) xc,yc

        call initialiseimage(cf,nhc,nhc,nhc,nhc)
        call decomp_cart(arr,n,m,hc,beta,xc,yc,nmax,num,cf)

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

        call arrstat(dum,n,m,1,1,n,m,std,av)
        write (*,*) '  The Full Image std = ',std,' Jy'
        write (*,*) '  Residual (>2s) std = ',stdres,' Jy'

        filec='coeff'
        fileo=f1(1:nchar(f1))//'.recons'
        call writearray_bin2D(cf,nhc,nhc,nmax,nmax,filec)
        call writearray_bin2D(rarr,n,m,n,m,fileo)
        call s_displayshap(f1,fileo,filec)

        return
        end


