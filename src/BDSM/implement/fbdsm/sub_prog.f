

c! HELP1 subroutine range1(array,num,num1,mx)
c! HELP 	(Increased) real*4 max of real*8 vector upto num
        subroutine range1(array,num,num1,mx)
        implicit none
        integer num,num1,i
        real*8 array(num)
        real*4 mx

        mx=array(1)
        do 100 i=2,num1
         if (mx.lt.array(i)) then
          mx=array(i)
         end if
100     continue

        mx=mx*1.1
        return
        end

c! HELP1 subroutine rangeint(vec,x,n,mn,mx)
c! HELP 	Exact integer max and min of integer vector upto n
        subroutine rangeint(vec,x,n,mn,mx)
        implicit none
        integer i,x,n
        integer vec(x),mx,mn

        mx=vec(1)
        mn=vec(1)
        do 100 i=1,n
         if (mx.lt.vec(i)) mx=vec(i)
         if (mn.gt.vec(i)) mn=vec(i)
100     continue

        return
        end

c! HELP1 subroutine rangevec8(vec,x,n,mn,mx)
c! HELP 	Exact real*8 max min of real*8 vector upto n	
        subroutine rangevec8(vec,x,n,mn,mx)
        implicit none
        integer i,x,n
        real*8 vec(x),mx,mn

        mx=vec(1)
        mn=vec(1)
        do 100 i=1,n
         if (mx.lt.vec(i)) mx=vec(i)
         if (mn.gt.vec(i)) mn=vec(i)
100     continue

        return
        end

c! HELP1 subroutine range2d(array,x,y,n,m,mn,mx)
c! HELP 	Exact max min of real*4 matrix upto n,m
        subroutine range2d(array,x,y,n,m,mn,mx)
        implicit none
        integer i,j,x,y,n,m
        real*4 array(x,y),mx,mn

        mx=array(1,1)
        mn=array(1,1)
        do 100 j=1,m
         do 110 i=1,n
          if (mx.lt.array(i,j)) mx=array(i,j)
          if (mn.gt.array(i,j)) mn=array(i,j)
110      continue
100     continue

        return
        end

c! HELP1 subroutine range_vec4mxmn(vec,num,num1,mn,mx)
c! HELP 	(Increased) max min of real*4 vector upto num1
        subroutine range_vec4mxmn(vec,num,num1,mn,mx)
        implicit none
        integer num,num1,i
        real*4 vec(num),mx,mn,dum

        mx=vec(1)
        mn=vec(1)
        do 100 i=2,num1
         if (mx.lt.vec(i)) then
          mx=vec(i)
         end if
         if (mn.gt.vec(i)) then
          mn=vec(i)
         end if
100     continue
        dum=abs(mx-mn)
        mx=mx+0.1*dum
        mn=mn-0.1*dum
        if (dum.eq.0.0) then
         if (mx.eq.0) then
          mx=1.0
          mn=-1.0 
         else
          mx=mx*1.5
          mn=mn/1.5
         end if
        end if

        return
        end

c! HELP1 subroutine range_vec4mxmn_mask(vec,mask,num,num1,mn,mx)
c! HELP 	(Increased) max min of real*4 vector upto num1, with mask
        subroutine range_vec4mxmn_mask(vec,mask,num,num1,mn,mx)
        implicit none
        integer num,num1,i,mask(num),dumi
        real*4 vec(num),mx,mn,dum

        do i=1,num1
         if (mask(i).eq.1) then
          dumi=i
          goto 333 
         end if
        end do
333     continue

        mx=vec(dumi)
        mn=vec(dumi)
        do 100 i=2,num1
         if (mask(i).eq.1) then
          if (mx.lt.vec(i)) then
           mx=vec(i)
          end if
          if (mn.gt.vec(i)) then
           mn=vec(i)
          end if
         end if
100     continue
        dum=abs(mx-mn)
        mx=mx+0.1*dum
        mn=mn-0.1*dum
        if (dum.eq.0.0) then
         if (mx.eq.0) then
          mx=1.0
          mn=-1.0 
         else
          mx=mx*1.5
          mn=mn/1.5
         end if
        end if

        return
        end

c! HELP1 subroutine range2xakt(vec,num,num1,mn,mx)
c! HELP1 subroutine range2xakt(vec,num,num1,mn,mx)
c! HELP 	Exact max min of real*4 vector upto num1
        subroutine range2xakt(vec,num,num1,mn,mx)
        implicit none
        integer num,num1,i
        real*4 vec(num),mx,mn

        mx=vec(1)
        mn=vec(1)
        do 100 i=2,num1
         if (mx.lt.vec(i)) then
          mx=vec(i)
         end if
         if (mn.gt.vec(i)) then
          mn=vec(i)
         end if
100     continue

        return
        end

c! HELP1 subroutine range2(array,num,num1,mn,mx)
c! HELP 	(Increased) max min of real*4 vector upto num1
        subroutine range2(array,num,num1,mn,mx)
        implicit none
        integer num,num1,i
        real*4 array(num),mx,mn,dum

        mx=array(1)
        mn=array(1)
        do 100 i=2,num1
         if (mx.lt.array(i)) then
          mx=array(i)
         end if
         if (mn.gt.array(i)) then
          mn=array(i)
         end if
100     continue

        dum=0.1*abs(mx-mn)
        mx=mx+dum
        mn=mn-dum

        return
        end

c! HELP1 subroutine range3(iarr,n1,n2,nm1,nm2,mn,mx,lg,nzero)
c! HELP 	(Increased) real*4 max min of real*8 matrix excluding zero, and in log if needed
        subroutine range3(iarr,n1,n2,nm1,nm2,mn,mx,lg,nzero)
        implicit none
        integer n1,n2,nm1,nm2,i,j,lg,nzero
        real*8 iarr(n1,n2),arr(n1,n2),nzelem
        real*4 mx,mn,dum

        nzelem=-999.d0
        if (nzero.eq.1) then   ! dont take zero
         do 250 j=1,nm2
          do 260 i=1,nm1
           if (iarr(i,j).ne.0.d0) then
            nzelem=iarr(i,j)     ! first nonzero element
            goto 333
           end if
260       continue
250      continue
        end if
333     continue
        if (nzelem.eq.-999.d0) write (*,*) ' NO nonzero elements!!!!! '

        if (lg.eq.1) then
         do 200 j=1,nm2
          do 210 i=1,nm1
           if (nzero.eq.1.and.iarr(i,j).eq.0.d0) then
            arr(i,j)=dlog10(nzelem)
           else
            arr(i,j)=dlog10(iarr(i,j))
           end if
210       continue
200      continue
        else
         do 300 j=1,nm2
          do 310 i=1,nm1
           if (nzero.eq.1.and.iarr(i,j).eq.0.d0) then
            arr(i,j)=nzelem
           else
            arr(i,j)=iarr(i,j)
           end if
310       continue
300      continue
        end if

        mx=arr(1,1)
        mn=arr(1,1)
        do 100 j=1,nm2
         do 110 i=1,nm1
          if (mx.lt.arr(i,j)) then
           mx=arr(i,j)
          end if
          if (mn.gt.arr(i,j)) then
           mn=arr(i,j)
          end if
110      continue
100     continue

        dum=0.1*abs(mx-mn)
        mx=mx+dum
        mn=mn-dum

        return
        end

c! HELP1 subroutine readnum(f1,n)
c! HELP 	Read an integer from file f1
        subroutine readnum(f1,n)
        implicit none
        character f1*500
        integer n,nchar

        open(unit=41,file=f1(1:nchar(f1)),status='old')
        read(41,*) n
        close(41)
        
        return
        end

c! HELP1 subroutine getline(file,n)
c! HELP 	Compute number of lines in file 
        subroutine getline(filen,dir,n)
        implicit none
        character filen*500,dir*500
        integer n,nchar

        n=0
        open(unit=41,file=dir(1:nchar(dir))//filen(1:nchar(filen)))
200      read (41,*,END=100) 
         n=n+1
         goto 200
100     continue
        close(41)

        return
        end

c! HELP1 Integer function NCHAR(string)
c! HELP 	Prog from Desh to compute number of valid characters in a string
        Integer function NCHAR(string)
C
C  Routne to count the number of characters in the
C  input string. Looks for the last occurrence of
C  non-(null, blank or tab character)
C
C  from desh thro reks
C
        Implicit none
        integer*4 i,ipos
        character*(*)  string
        character blank,tab,null,c

c        data blank,tab,null/' ',9,0/
        data blank/' '/
        tab=char(9)
        null=char(0)

        ipos = 0
        i    = len(string)
        do while (i.gt.0.and.ipos.eq.0)
         c = string(i:i)
         if (c.ne.blank.and.c.ne.tab.and.c.ne.null) ipos = i
         i = i - 1
        end do

        NCHAR = ipos
        return
        end

        Integer function bnchar(string)
C
C  modify nchar to get index of first nonblank etc character
C
        Implicit none
        integer*4 i,ipos
        character*(*)  string
        character blank,tab,null,c

c        data blank,tab,null/' ',9,0/
        data blank/' '/
        tab=char(9)
        null=char(0)

        ipos = 0
        i    = 1 
        do while (i.le.len(string).and.ipos.eq.0)
         c = string(i:i)
         if (c.ne.blank.and.c.ne.tab.and.c.ne.null) ipos = i
         i = i + 1
        end do

        bnchar = ipos
        return
        end

c! HELP1 subroutine convertra(ra,hh,mm,ss)
c! HELP 	Converts ra in degrees to 'hh mm ss'
        subroutine convertra(ra,hh,mm,ss)
        implicit none
        integer hh,mm
        real*8 ss,ra,dumr

        dumr=ra/15.d0
        hh=int(dumr)
        dumr=(ra/15.0-hh)*60.d0
        mm=int(dumr)
        dumr=(ra/15.0-hh)*60.d0 - mm
        ss=dumr*60.d0

        return
        end

c! HELP1 subroutine convertdec(dec,s,dd,ma,sa)
c! HELP 	Converts dec in degrees to 'sdd ma sa'
        subroutine convertdec(dec,s,dd,ma,sa)
        implicit none
        integer dd,ma
        real*8 sa,dec,dumr,ddec
        character s*1

        if (dec.lt.0.0) then
         s='-'
        else
         s='+'
        end if
        ddec=abs(dec)
        dd=int(ddec)
        dumr=ddec-dd
        dumr=dumr*60.d0
        ma=int(dumr)
        dumr=dumr-ma
        sa=dumr*60.d0

        return
        end

c! HELP1 subroutine convertdecinv(dd,ma,sa,dec)
c! HELP 	Converts 'dd ma sa' to dec in degrees
        subroutine convertdecinv(dd,ma,sa,dec)
        implicit none
        integer dd,ma
        real*8 sa,dec

        dec=abs(dd) + abs(ma)/60.d0 + abs(sa)/3600.d0
        if (dd.lt.0.or.ma.lt.0.or.sa.lt.0.d0) then
         dec=-dec
        end if

        return
        end
c!
c!
c!
        subroutine dec_rmsign(s,dd,ma,sa)
        implicit none
        integer dd,ma
        real*8 sa
        character s*1
        
        if (s.eq.'-') then
         if (dd.ne.0) then
          dd=-dd
         else
          if (ma.ne.0) then
           ma=-ma
          else
           sa=-sa
          end if
         end if
        end if
        
        return
        end

c! HELP1 subroutine xchangi(i,j)
c! HELP 	Exchanges integers i and j
        subroutine xchangi(i,j)
        implicit none
        integer i,j,k

        k=i
        i=j
        j=k

        return
        end

c! HELP1 subroutine xchangr8(i,j)
c! HELP 	Exchanges real*8 i and j
        subroutine xchangr8(i,j)
        implicit none
        real*8 i,j,k

        k=i
        i=j
        j=k

        return
        end

c! HELP1 subroutine justdist(ra1,ra2,dec1,dec2,dist)
c! HELP 	Computes distance in arcsec between (ra1,dec1) and (ra2,dec2) in degrees
        subroutine justdist(ra1,ra2,dec1,dec2,dist)
        implicit none
        real*8 ra1,ra2,dec1,dec2,dist,rad
        real*8 r1,r2,d1,d2

cf2py   intent(in) ra1, ra2, dec1, dec2
cf2py   intent(out) dist

        rad=3.14159d0/180.d0

        r1=(ra1-180.d0)*rad
        r2=(ra2-180.d0)*rad
        d1=dec1*rad
        d2=dec2*rad

        dist=acos(dsin(d1)*dsin(d2)+dcos(d1)*dcos(d2)*dcos(r1-r2))
        dist=dist*3600.d0/rad  ! in arcsec

        return
        end
 
c! HELP1 subroutine gdist_pa(i,j,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm)
c! HELP 	Computes distance in arcsec in direction of PA
        subroutine gdist_pa(i,j,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer i,j,gpi
        real*8 xpix(gpi),ypix(gpi),bmaj(gpi),bmin(gpi),bpa(gpi)
        real*8 dumr1,dumr2,dumr3,fwhm,psi

        dumr1=atan(abs((ypix(j)-ypix(i))/(xpix(j)-xpix(i))))  ! rad, 1st quadrant
        call atanproper(dumr1,ypix(j)-ypix(i),xpix(j)-xpix(i))
        
        psi=dumr1-(bpa(i)+90.d0)/rad
c!                       convert angle to eccentric anomaly
        psi=atan(bmaj(i)/bmin(i)*tan(psi))  
        dumr2=bmaj(i)*dcos(psi)/2.d0
        dumr3=bmin(i)*dsin(psi)/2.d0
        fwhm=sqrt(dumr2*dumr2+dumr3*dumr3)

        return
        end

c! HELP1 subroutine justangle(ra2,ra1,dec2,dec1,ang)
c! HELP 	Computes P.A. of line joining (ra1,dec1) and (ra2,dec2) in degrees
        subroutine justangle(ra2,ra1,dec2,dec1,ang)
        implicit none
        real*8 ra1,ra2,dec1,dec2,ang,rad,d

        call justdist(ra1,ra2,dec1,dec2,d)
        ang=acos((dec2-dec1)*3600.d0/d)*180.d0/3.14159  ! in deg
        if (ra2-ra1.le.0.d0) ang=360.d0-ang      ! in deg

        return
        end

c! HELP1 subroutine getseed(seed)
c! HELP 	Computes seed for ran1 from current time
        subroutine getseed(seed)
        implicit none
        integer seed
        character str1*80
         
        call system('rm -f aa')
        str1="date | sed 's/:/ /g' | awk '{print -($5*100+$6)}' > aa"
        call system(str1)
        open(unit=21,file='aa',status='unknown')
        read (21,*) seed
        close(21)
        call system('rm -f aa')

        return
        end

        subroutine vec8to4(vec8,n1,vec4,n)
        implicit none
        integer n,i,n1
        real*8 vec8(n1)
        real*4 vec4(n)

        do 100 i=1,n
         vec4(i)=vec8(i)
100     continue

        return
        end

        subroutine vec4to8(vec4,n1,vec8,n)
        implicit none
        integer n,i,n1
        real*4 vec4(n1)
        real*8 vec8(n)

        do 100 i=1,n
         vec8(i)=vec4(i)
100     continue

        return
        end

        subroutine array8to4(arr8,n1,m1,arr4,n,m)
        implicit none
        integer n,m,i,j,n1,m1
        real*8 arr8(n1,m1)
        real*4 arr4(n,m)

        do 100 j=1,m
         do 110 i=1,n
          arr4(i,j)=arr8(i,j)
110      continue
100     continue

        return
        end

        subroutine array4to8(arr4,n1,m1,arr8,n,m)
        implicit none
        integer n,m,i,j,n1,m1
        real*4 arr4(n1,m1)
        real*8 arr8(n,m)

        do 100 j=1,m
         do 110 i=1,n
          arr8(i,j)=arr4(i,j)
110      continue
100     continue

        return
        end
        
        subroutine minmaxarr8(arr8,n1,m1,nb,mb,n,m,mn,mx)
        implicit none
        integer n,m,i,j,n1,m1,nb,mb
        real*8 arr8(n1,m1),mn,mx

        mx=arr8(nb,mb)
        mn=arr8(nb,mb)
        do 100 j=mb,m
         do 110 i=nb,n
          if (mx.lt.arr8(i,j)) mx=arr8(i,j)
          if (mn.gt.arr8(i,j)) mn=arr8(i,j)
110      continue
100     continue

        return
        end


        subroutine minmaxarr4(arr4,n1,m1,nb,mb,n,m,mn,mx)
        implicit none
        integer n,m,i,j,n1,m1,nb,mb
        real*4 arr4(n1,m1),mn,mx

        mx=arr4(nb,mb)
        mn=arr4(nb,mb)
        do 100 j=mb,m
         do 110 i=nb,n
          if (mx.lt.arr4(i,j)) mx=arr4(i,j)
          if (mn.gt.arr4(i,j)) mn=arr4(i,j)
110      continue
100     continue

        return
        end


        subroutine arr2dnz4(arr,n,m,mn,mx)
        implicit none
        integer n,m,k,i,j
        real*4 arr(n,m),nzero,mn,mx
        
        nzero=-999.0
        do 100 j=1,m
         do 110 i=1,n
          if (arr(i,j).ne.0.0) then
           nzero=arr(i,j)
           goto 333
          end if
110      continue
100     continue
333     continue
        if (nzero.eq.-999.0) write (*,*) ' ALL zero !!!'

        mn=nzero
        mx=nzero
        do 200 j=1,m
         do 210 i=1,n
          if (arr(i,j).ne.0.0) then
           if (mn.gt.arr(i,j)) mn=arr(i,j)
           if (mx.lt.arr(i,j)) mx=arr(i,j)
          end if
210      continue
200     continue

        return
        end

        subroutine arr2dnz(arr,x,y,n,m,mn,mx)
        implicit none
        integer x,y,n,m,k,i,j
        real*8 arr(x,y),nzero
        real*4 mn,mx
        
        nzero=-999.d0
        do 100 j=1,m
         do 110 i=1,n
          if (arr(i,j).ne.0.d0) then
           nzero=arr(i,j)
           goto 333
          end if
110      continue
100     continue
333     continue
        if (nzero.eq.-999.0) write (*,*) ' ALL zero !!!'

        mn=nzero
        mx=nzero
        do 200 j=1,m
         do 210 i=1,n
          if (arr(i,j).ne.0.d0) then
           if (mn.gt.arr(i,j)) mn=arr(i,j)
           if (mx.lt.arr(i,j)) mx=arr(i,j)
          end if
210      continue
200     continue

        return
        end

        subroutine converttolog(arr4,i,j,low,up,low1)
        implicit none
        integer i,j,i1,i2
        real*4 arr4(i,j),low1,low,up
        
        if (low.lt.0.0) then
         low1=abs(2.0*low)
        else
         low1=0.d0
        end if
        low=log10(low+low1)
        up=log10(up+low1)
        do 400 i2=1,j
         do 410 i1=1,i
          if (arr4(i1,i2).ne.0.0) then
           arr4(i1,i2)=log10(arr4(i1,i2)+low1)
          else
           arr4(i1,i2)=log10(low1)   
          end if
410      continue
400     continue

        return
        end

        subroutine arr8tovec8_bl(arr,x,y,n,m,vec,nm,blv)
        implicit none
        integer x,y,n,m,i,j,k,nm
        real*8 arr(x,y),vec(nm),blv
        
        k=0
        do 100 j=1,m
         do 110 i=1,n
          if (arr(i,j).ne.blv) then
           k=k+1
           vec(k)=arr(i,j)
          end if
110      continue
100     continue
        if (k.ne.nm) write (*,*) ' ### arr8->vec8_bl wrong !!!'

        return
        end

        subroutine arr8tovec8(arr,x,y,n,m,vec,nm)
        implicit none
        integer x,y,n,m,i,j,k,nm
        real*8 arr(x,y),vec(nm)
        
        k=0
        do 100 j=1,m
         do 110 i=1,n
          k=k+1
          vec(k)=arr(i,j)
110      continue
100     continue

        return
        end


        subroutine arr4tovec4(arr,x,y,n,m,vec,nm)
        implicit none
        integer x,y,n,m,i,j,k,nm
        real*4 arr(x,y),vec(nm)
        
        k=0
        do 100 j=1,m
         do 110 i=1,n
          k=k+1
          vec(k)=arr(i,j)
110      continue
100     continue

        return
        end


        subroutine setzoom(xcur,ycur,xcur1,ycur1,blc,trc,n,m)
        implicit none
        integer n,m
        real*4 xcur,ycur,xcur1,ycur1,blc(2),trc(2),n1,m1

        n1=n*1.0
        m1=m*1.0 

        blc(1)=min(xcur,xcur1)
        blc(2)=min(ycur,ycur1)
        trc(1)=max(xcur,xcur1)
        trc(2)=max(ycur,ycur1)

        blc(1)=max(blc(1),1.0)
        blc(2)=max(blc(2),1.0)
        trc(1)=min(trc(1),n1)
        trc(2)=min(trc(2),m1)

        return
        end

        function round(i)
        integer round
        real*8 i,a
        
        round=int(i)
        a=i-int(i)
        if (a.gt.0.5) round=round+1

        return
        end


        function round4(i)
        integer round4
        real*4 i,a
        
        round4=int(i)
        a=i-int(i)
        if (a.gt.0.5) round4=round4+1

        return
        end

        function rfact(n)
        implicit none
        real*8 rfact
        integer i,n

        rfact=1.d0
        if (n.gt.1) then
         do i=1,n
          rfact=rfact*i
         end do
        end if
        
        return
        end

        function fact(n)
        implicit none
        integer fact
        integer n,i

        fact=1
        if (n.gt.12) then
         write (*,*) ' N too large !! Use rfact instead !! '
        else
         if (n.gt.1) then
          do 100 i=1,n
           fact=fact*i
100       continue
         end if
        end if
        
        return
        end

        subroutine initialisemask(array,x,y,n,m,val)
        implicit none
        integer n,m,i,j,x,y
        integer array(x,y),val

        do 100 j=1,m
         do 110 i=1,n
          array(i,j)=val
110      continue
100     continue

        return
        end

        subroutine initialiseimage(array,x,y,n,m,val)
        implicit none
        integer n,m,i,j,x,y
        real*8 array(x,y),val

        do 100 j=1,m
         do 110 i=1,n
          array(i,j)=val
110      continue
100     continue

        return
        end

        subroutine initialiseimage3d(array,x,y,z,n,m,l,val)
        implicit none
        integer n,m,i,j,x,y,z,l,k
        real*8 array(x,y,z),val

        do 120 k=1,l
         do 100 j=1,m
          do 110 i=1,n
           array(i,j,k)=val
110       continue
100      continue
120     continue

        return
        end

        subroutine initialise_int_vec(vec,n,val)
        implicit none
        integer n,vec(n),val,i

        do 100 i=1,n
         vec(i)=val
100     continue

        return
        end

        subroutine initialise_vec(vec,n,val)
        implicit none
        integer n,i
        real*8 vec(n),val

        do 100 i=1,n
         vec(i)=val
100     continue

        return
        end


        subroutine getmax(vec,x,n,dumi)
        implicit none
        integer x,n,i,dumi
        real*8 vec(x),mx

        mx=vec(1)
        do 100 i=1,n
         if (mx.lt.vec(i)) then
          mx=vec(i) 
          dumi=i
         end if
100     continue
        
        return
        end


        subroutine vec_int_num_range(vec,x,n,low,hi,s1)
        implicit none
        integer x,n,i,vec(x),s1,low,hi

        s1=0
        do 100 i=1,n
         if (vec(i).ge.low.and.vec(i).le.hi) s1=s1+1
100     continue
        
        return
        end


        subroutine vec_int_nzero(vec,x,n,s1)
        implicit none
        integer x,n,i,vec(x),s1

        s1=0
        do 100 i=1,n
         if (vec(i).eq.0) s1=s1+1
100     continue
        
        return
        end

        subroutine vec_int_sum(vec,x,n,vsum)
        implicit none
        integer x,n,i,vec(x),vsum

        vsum=0
        do 100 i=1,n
         vsum=vsum+vec(i)
100     continue
        
        return
        end


        subroutine vec_mean(vec,x,n1,n,av)
        implicit none
        integer x,n,i,n1
        real*8 vec(x),av

        av=0.d0
        do 100 i=n1,n
         av=av+vec(i)
100     continue
        av=av/(n-n1+1)
        
        return
        end

        subroutine vec_std4(vec,x,n1,n,av,std)
        implicit none
        integer x,n,i,n1
        real*4 vec(x),av,std

        av=0.0
        std=0.0
        if (n-n1+1.gt.0) then
         do 100 i=n1,n
          av=av+vec(i)
100      continue
         av=av/(n-n1+1)

         do 200 i=n1,n
          std=std+(vec(i)-av)*(vec(i)-av)
200      continue
         std=sqrt(std/(n-n1+1-1))
        end if

        return
        end

        subroutine vec_std(vec,x,n1,n,av,std)
        implicit none
        integer x,n,i,n1
        real*8 vec(x),av,std

        av=0.d0
        std=0.d0
        if (n-n1+1.gt.0) then
         do 100 i=n1,n
          av=av+vec(i)
100      continue
         av=av/(n-n1+1)

         do 200 i=n1,n
          std=std+(vec(i)-av)*(vec(i)-av)
200      continue
         std=sqrt(std/(n-n1+1-1))
        end if

        return
        end

        subroutine matavmask(image,mask,x,y,n,m,av)
        implicit none
        integer x,y,m,n,i,j,mask(x,y),num
        real*8 image(x,y),av

        av=0.d0
        num=0
        do 100 j=1,m
         do 110 i=1,n
          if (mask(i,j).eq.1) then
           av=av+image(i,j)
           num=num+1
          end if
110      continue
100     continue
        av=av/num
        
        return
        end

        subroutine matav(image,x,y,n,m,av)
        implicit none
        integer x,y,m,n,i,j
        real*8 image(x,y),av

        av=0.d0
        do 100 i=1,n
         do 110 j=1,m
          av=av+image(i,j)
110      continue
100     continue
        av=av/(n*m)
        
        return
        end

        subroutine intmatsum(image,x,y,n,m,av)
        implicit none
        integer x,y,m,n,i,j,image(x,y),av

        av=0
        do 100 j=1,m
         do 110 i=1,n
          av=av+image(i,j)
110      continue
100     continue
        
        return
        end

c        subroutine meanc(cvec,x,n,av)
c        implicit none
c        integer x,n,i
c        double complex cvec(x),av
c
c        av=0.d0
c        do 100 i=1,n
c         av=av+cvec(i)
c100     continue
c        av=av/n
c        
c        return
c        end
c
c        subroutine makecomplex(real,imag,cn,x,n)
c        implicit none
c        integer x,n,i
c        double complex cn(x)
c        real*8 real(x),imag(x)
c
c        do 100 i=1,n
c         cn(i)=dcmplx(real(i),imag(i))
c100     continue
c
c        return
c        end


        subroutine maxarray(image,x,y,n,m,xmax,ymax,maxv)
        implicit none
        integer x,y,n,m,i,j,xmax,ymax
        real*8 image(x,y),maxv

        xmax=1
        ymax=1
        maxv=image(xmax,ymax)
        do 100 j=1,m
         do 110 i=1,n
          if (image(i,j).gt.maxv) then
           xmax=i
           ymax=j
           maxv=image(i,j)
          end if
110      continue
100     continue

        return
        end

        subroutine maxarraymask(image,mask,n,m,xmax,ymax,maxv)
        implicit none
        integer n,m,i,j,xmax,ymax,mask(n,m)
        real*8 image(n,m),maxv

        do j=1,m
         do i=1,n
          if (mask(i,j).eq.1) then
           xmax=i
           ymax=j
           maxv=image(i,j)
          end if
         end do
        end do

        do 100 j=1,m
         do 110 i=1,n
          if (mask(i,j).eq.1.and.image(i,j).gt.maxv) then
           xmax=i
           ymax=j
           maxv=image(i,j)
          end if
110      continue
100     continue

        return
        end

        subroutine imager2int(image,n,m,mask)
        implicit none
        integer n,m,i,j,mask(n,m)
        real*8 image(n,m)
        
        do 100 j=1,m
         do 110 i=1,n
          mask(i,j)=int(image(i,j))
110      continue
100     continue

        return
        end


        subroutine imageint2r(mask,x,y,n,m,image)
        implicit none
        integer x,y,n,m,i,j,mask(x,y)
        real*8 image(x,y)
        
        do 100 j=1,m
         do 110 i=1,n
          image(i,j)=mask(i,j)*1.d0
110      continue
100     continue

        return
        end


        subroutine copyarray(im1,x,y,x1,y1,n1,m1,n,m,im2)
        implicit none
        integer x,y,n,m,i,j,x1,y1,n1,m1
        real*8 im1(x,y),im2(x1,y1)
        
        do 100 j=m1,m
         do 110 i=n1,n
          im2(i-(n1-1),j-(m1-1))=im1(i,j)
110      continue
100     continue

        return
        end

        subroutine copyarray4(im1,x,y,x1,y1,n1,m1,n,m,im2)
        implicit none
        integer x,y,n,m,i,j,x1,y1,n1,m1
        real*4 im1(x,y),im2(x1,y1)
        
        do 100 j=m1,m
         do 110 i=n1,n
          im2(i-(n1-1),j-(m1-1))=im1(i,j)
110      continue
100     continue

        return
        end

        subroutine copyarrayint(im1,x,y,x1,y1,n1,m1,n,m,im2)
        implicit none
        integer x,y,n,m,i,j,x1,y1,n1,m1
        integer im1(x,y),im2(x1,y1)
        
        do 100 j=m1,m
         do 110 i=n1,n
          im2(i-(n1-1),j-(m1-1))=im1(i,j)
110      continue
100     continue

        return
        end


        subroutine int2str(nisl,str)
        implicit none
        integer nisl,ndigit,num,j,rem
        character str*10,getchar*1

        str=' '
        ndigit=int(log10(nisl*1.0))+1
        num=nisl
        do 100 j=1,ndigit
         rem=num-int(num*0.1)*10
         str(ndigit-j+1:ndigit-j+1)=getchar(rem)
         num=int(num*0.1)
100     continue

        return
        end

        function pixinimage(x,y,n1,n2,m1,m2)
        implicit none
        integer x,y,n1,n2,m1,m2
        logical pixinimage

        if (x.ge.n1.and.x.le.n2.and.y.ge.m1.and.y.le.m2) then
         pixinimage=.true. 
        else
         pixinimage=.false. 
        end if

        return
        end

        subroutine convertchar(cha,num)
        implicit none
        character cha*1,nums(10)*1
        integer num,i,j

        data nums/'0','1','2','3','4','5','6','7','8','9'/

        do i=1,10
         if (cha.eq.nums(i)) num=i-1
        end do

        return
        end

        subroutine get_fmt_int(n,fmti)
        implicit none
        integer n,ndigit
        character fmti*2,getchar*1

        fmti(1:1)='i'
        ndigit=int(log10(n*1.0))+1
        if (ndigit.gt.9) write (*,*) ' !!! ERROR !!!!'
        fmti(2:2)=getchar(ndigit)
        
        return
        end

        function existsq(filen,dir,extn)   
        implicit none
        logical existsq,ex
        character filen*500,extn*20,fn*500,dir*500
        integer nchar
        
        fn=dir(1:nchar(dir))//filen(1:nchar(filen))//extn(1:nchar(extn))
        inquire(file=fn,exist=ex)
        existsq=ex
        
        return
        end

        function exists(filen,dir,extn)
        implicit none
        logical exists,ex
        character filen*500,extn*20,fn*500,dir*500
        integer nchar
        
        fn=''
        fn=dir(1:nchar(dir))//filen(1:nchar(filen))//extn(1:nchar(extn))
        inquire(file=fn,exist=ex)
        exists=ex
        if (.not.ex) then
         write (*,*) '    #### ',fn(1:nchar(fn)),' not found.'
         write (*,*)
        end if
        
        return
        end

        subroutine callisint(arr,x,n,i,isint)
        implicit none
        logical isint
        integer x,n,i,arr(x),j

        isint=.false.
        do j=1,n
         if (arr(j).eq.i) isint=.true. 
        end do

        return
        end

        subroutine masksum(mask,x,y,n,m,tot)
        implicit none
        integer x,y,n,m,tot,mask(x,y),i,j
        
        tot=0
        do j=1,m
         do i=1,n
          tot=tot+mask(i,j)
         end do
        end do
        
        return
        end

        subroutine multimasksum(mask,x,y,n,m,k,tot)
        implicit none
        integer x,y,n,m,tot,mask(x,y),i,j,k
        
        tot=0
        do j=1,m
         do i=1,n
          if (mask(i,j).eq.k) tot=tot+1
         end do
        end do
        
        return
        end

        subroutine atanproper(dumr1,dy,dx)
        implicit none
        include "includes/constants.inc"
        real*8 dumr1,dy,dx
        logical ysign,xsign

        ysign=(dy.ge.0.d0)
        xsign=(dx.ge.0.d0)
        if (ysign.and..not.xsign) dumr1=pi-dumr1
        if (.not.ysign.and..not.xsign) dumr1=pi+dumr1
        if (.not.ysign.and.xsign) dumr1=2.d0*pi-dumr1          ! rad, all qdrnts

        return
        end


c! precession from B1950.0 to J2000.0 from http://www.stargazing.net/kepler/b1950.html
c! mistake mentioned there is real.
c! also from smith c.a. et al 1989.
c!       !!!   is wrong -- within few " ! use Aoki !!!

        subroutine B1950toJ2000(ra,dec,raj,decj)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        real*8 ra,dec,raj,decj,ra1,dec1,deltac,deltad
        real*8 x,y,z,x1,y1,z1,dumr
        character s*1

        write (*,*) '  Dont use Smith et al, use Aoki et al instead !!'
        deltac=-(0.065838/3600.d0)  ! deg
        deltad=0.335299/3600.d0  ! deg 
        ra1=ra-(deltac*dcos(ra/rad)+deltad*dsin(ra/rad))/dcos(dec/rad)
        dec1=dec-(deltad*dcos(ra/rad)-deltac*dsin(ra/rad))
     /       *dsin(dec/rad)+0.028553/3600.d0*dcos(dec/rad)

        x=dcos(ra1/rad)*dcos(dec1/rad)
        y=dsin(ra1/rad)*dcos(dec1/rad)
        z=dsin(dec1/rad)
        
        x1=0.999926d0*x-0.011179d0*y-0.004859d0*z  
        y1=0.011179d0*x+0.999938d0*y-0.000027d0*z 
        z1=0.004859d0*x-0.000027d0*y+0.999988d0*z  

        raj=atan(y1/x1)*rad
        if (x1.lt.0.d0) raj=raj+180.d0
        if (y1.lt.0.d0.and.x1.gt.0.d0) raj=raj+360.d0
        decj=asin(z1)*rad
                
        return
        end

c! precess from smith ea in sub_prog.f seems to be wrong by few " so
c! do Aoki  ea 83 instead. all ra,dec in deg
c! seems to be same as NED to ~0.2".

        subroutine Aoki_B1950toJ2000(ra,dec,raj,decj)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        real*8 ra,dec,raj,decj,A(3),r0(3),r1(3),r0A,M(3,3)
        real*8 r(3),rscal,d1,d2
        integer i

        data A/-1.62557d-6,-0.31919d-6,-0.13843d-6/
        data M/0.9999256782d0,0.0111820609d0,0.0048579479d0,  ! 11 21 31
     /         -0.0111820610,0.9999374784,-0.0000271474,      ! 12 22 32
     /         -0.0048579477,-0.0000271765,0.9999881997/      ! 13 23 33

        r0(1)=dcos(dec/rad)*dcos(ra/rad)
        r0(2)=dcos(dec/rad)*dsin(ra/rad)
        r0(3)=dsin(dec/rad)

        r0A=r0(1)*A(1)+r0(2)*A(2)+r0(3)*A(3)

        do i=1,3
         r1(i)=r0(i)-A(i)+r0A*r0(i)
        end do

        do i=1,3
         r(i)=M(i,1)*r1(1)+M(i,2)*r1(2)+M(i,3)*r1(3)
        end do

        rscal=sqrt(r(1)*r(1)+r(2)*r(2)+r(3)*r(3))
        decj=asin(r(3)/rscal)*rad   ! in deg

        d1=r(1)/rscal/dcos(decj/rad)   ! acos
        d2=r(2)/rscal/dcos(decj/rad)   ! asin
        raj=acos(d1)*rad ! in deg
        if (d2.lt.0.d0) raj=360.d0-raj

        return  
        end

        subroutine correctrarad(ra)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        real*8 ra

        if (ra*rad.gt.360.d0) ra=ra-360.d0/rad
        if (ra*rad.lt.0.d0) ra=ra+360.d0/rad

        return
        end

        function cartdist(x1,y1,x2,y2)
        implicit none
        real*4 x1,y1,x2,y2,cartdist
        
        cartdist=abs(sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1)))
        
        return
        end

c! HELP1 subroutine num_num(str,num)
c! HELP 	number of numbers (or strings) in string str 
        subroutine num_num(str,num)
        implicit none
        character str*500
        integer num,nchar,i,nstr,n1

        n1=500 ! for compiler
        nstr=nchar(str)
        do i=1,nstr
         if (str(i:i).ne.' ') then
          n1=i
          goto 333
         end if
        end do
333     continue  !  n1 is first non-space character in str
        num=0
        do i=n1,nstr-1
         if (str(i:i).eq.' '.and.str(i+1:i+1).ne.' ') then
          num=num+1
         end if
        end do
        num=num+1

        return
        end
c!
c!
c!
        subroutine calcmedianstd(vec,x,n,med,medstd)
        implicit none
        integer n,x,i
        real*8 vec(x),med,medstd,s1

        s1=0.d0
        do i=1,n
         s1=s1+(vec(i)-med)*(vec(i)-med)
        end do
        medstd=sqrt(s1/n)
        
        return
        end
c!
c!
c!
        subroutine calcmedianstd4(vec,x,n,med,medstd)
        implicit none
        integer n,x,i
        real*4 vec(x),med,medstd,s1

        s1=0.0
        do i=1,n
         s1=s1+(vec(i)-med)*(vec(i)-med)
        end do
        medstd=sqrt(s1/n)
        
        return
        end


        subroutine getcoeffarr(hc,x,num,i,hca)
        implicit none   
        integer x,num,i,j
        real*8 hc(x,x),hca(x)

        do 100 j=1,x!num
         hca(j)=hc(i,j)
100     continue 

        return
        end

        subroutine numE2str(n,str)
        implicit none
        real*8 n
        character str*10
        
        if (n.lt.0.d0) then
         write (str,'(1Pe9.2)') n
        else
         write (str,'(1Pe8.2)') n
        end if

        return
        end

        subroutine num2str_fn(n,x,str1)
        implicit none
        integer n,x,nd0,nd,j,nchar,bnchar
        character str*500,dumc*10,str1*10
        
        nd0=int(log10(x*1.0))+1
        write (str,*) n
        nd=int(log10(n*1.0))+1
        dumc=''
        do j=1,nd0-nd
         dumc=dumc(1:nchar(dumc))//'0'
        end do
        str1=dumc(1:nchar(dumc))//str(bnchar(str):nchar(str))

        return
        end


        subroutine int_vec_vec_std(vec1,vec2,n,std)
        implicit none
        integer n,vec1(n),vec2(n),i,round
        real*8 std
         
        std=0.d0
        do i=1,n
         std=std+1.d0*(vec1(i)-vec2(i))*(vec1(i)-vec2(i))
        end do
        std=sqrt(std)
        
        return
        end

        subroutine imageop(im,n,m,oim,code,val)
        implicit none
        integer n,m,i,j
        real*8 im(n,m),oim(n,m),val
        character code*3

        if (code.eq.'add') then
         do j=1,m
          do i=1,n
           oim(i,j)=im(i,j)+val
          end do
         end do 
        end if

        if (code.eq.'mul') then
         do j=1,m
          do i=1,n
           oim(i,j)=im(i,j)*val
          end do
         end do 
        end if

        return
        end

        subroutine blank2mask(image,n,m,blankv,mask)
        implicit none
        integer n,m,mask(n,m),i,j
        real*8 image(n,m),blankv

        do j=1,m
         do i=1,n
          if (image(i,j).eq.blankv) then
           mask(i,j)=0
          else
           mask(i,j)=1
          end if
         end do
        end do

        return
        end

        subroutine invert_sortindex(iwksp,n,ind)
        implicit none
        integer n,i,iwksp(n),ind(n)

        do i=1,n
         ind(iwksp(i))=i
         if (i.eq.7) write (*,*) ' ####### ',i,iwksp(i)
        end do

        return
        end

        subroutine cart2polar(x,y,xcen,ycen,r,th)
        implicit none
        real*8 x,y,xcen,ycen,r,th
 
        r=sqrt((x-xcen)*(x-xcen)+(y-ycen)*(y-ycen))
        th=atan2(y-ycen,x-xcen)
        
        return
        end

        subroutine argmax(x,n,ind)
        implicit none
        integer n,ind,i
        real*8 x(n),dum

        ind=1
        dum = x(1)
        do i=1,n
         if (x(i).gt.dum) then
          dum=x(i)
          ind=i
         end if
        end do

        return
        end

        function getchar(i)
        implicit none
        character getchar*1,nums(10)*1
        integer i,j

        data nums/'0','1','2','3','4','5','6','7','8','9'/

        do 100 j=1,10
         if (i.eq.(j-1)) getchar=nums(j)
100     continue

        return
        end

        subroutine argmin(x,n,ind)
        implicit none
        integer n,ind,i
        real*8 x(n),dum

        ind=1
        dum = x(1)
        do i=1,n
         if (x(i).lt.dum) then
          dum=x(i)
          ind=i
         end if
        end do

        return
        end






