c! to run a box on an image and extract noise map out. initial attempt.
c! if nsig is zero, calc std directly else do n-sigma clipping.
c!
c! too large. if u define so many arrays of 2048^2 OS sends SIGENV !!!

        subroutine boxnoiseimage(f1,f2,f3,f4)
        implicit none
        character f1*(*),f2*(*),f3*(*),f4*(*),extn*20
        integer n,m,bsize,stepsize,l
        real*8 nsig

cf2py   intent(in) f1,f2,f3,f4

        write (*,'(a23,$)') '   Boxsize, stepsize : '
        read (*,*) bsize,stepsize
        if (mod(bsize,2).eq.0) then
         bsize=bsize+1
         write (*,*) '  Increasing boxsize to ',bsize
        end if

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        if (bsize+stepsize.gt.min(n,m)) then
         write (*,*) '======================'
         write (*,*) '== Decrease Boxsize =='
         write (*,*) '======================'
         goto 222
        end if

        write (*,'(a39,$)') '   N for n-sigma clipping, 0 for std : '
        read (*,*) nsig

        call sub_boxnoise(f1,f2,f3,f4,bsize,stepsize,n,m,nsig)
        
222     continue
        return
        end

        subroutine sub_boxnoise(f1,f2,f3,f4,bsize,stepsize,n,m,nsig)
        implicit none
        character f1*(*),f2*(*),f3*(*),f4*(*),strdev*5,lab*500
        character lab2*500,extn*20
        integer n,m,bsize,stepsize,nstep1,nstep2,i,j,nchar
        integer cen(n,m,2),cen1,cen2,trc1,trc2
        integer mask(n,m),ft,i1,j1
        real*8 image1(n,m),image2(n,m)  ! image1 is arr and image2 is av
        real*8 std(n,m),mean,rms,rms0,med,median(n,m),nsig
        integer ipblc(2),iptrc(2),di1,di2

c!
c! read in the array
c! mask is 1/0 if pt is center of a box or not. tb updated later
        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)
        do 100 j=1,m
         do 110 i=1,n
          mask(i,j)=0
          image2(i,j)=0.d0
          std(i,j)=0.d0
          median(i,j)=0.d0
110      continue
100     continue

c! nstep boxes to put. remaining can be done with one box at end
        nstep1=float(n-bsize)/float(stepsize)+1 
        nstep2=float(m-bsize)/float(stepsize)+1 

c!
c! now calculate centre of all boxes 
        do 350 i=1,nstep1
         do 360 j=1,nstep2
          cen(i,j,1)=(bsize+1)/2+(i-1)*stepsize
          cen(i,j,2)=(bsize+1)/2+(j-1)*stepsize
360      continue
350     continue

c! including the extra ones at end if needed
        if (mask((n-(bsize+1)/2),(bsize+1)/2).eq.0) then
         do 440 j=1,nstep2
          cen(nstep1+1,j,1)=n-bsize+(bsize+1)/2
          cen(nstep1+1,j,2)=cen(1,j,2)
440      continue
         nstep1=nstep1+1
        end if
        if (mask((bsize+1)/2,(m-(bsize+1)/2)).eq.0) then
         do 450 i=1,nstep1
          cen(i,nstep2+1,1)=cen(i,1,1)
          cen(i,nstep2+1,2)=m-bsize+(bsize+1)/2
450      continue
         nstep2=nstep2+1
        end if

c! put into blc, trc
        ipblc(1)=cen(1,1,1)
        ipblc(2)=cen(1,1,2)
        iptrc(1)=cen(nstep1,nstep2,1)
        iptrc(2)=cen(nstep1,nstep2,2)

c!
c! calculate rms and mean in each of the boxes
c! arrstat calculates mean and standard dev.
c! sigclip does proper n-sigma clipping for rms
        write (*,'(a,$)') '   Computing mean, rms, median '
        call arrstat(image1,n,m,1,1,n,m,rms0,mean)  ! to calc rms of whole image
        do 120 i=1,nstep1
         do 130 j=1,nstep2
          cen1=cen(i,j,1)
          cen2=cen(i,j,2)
          call getstat(image1,n,m,cen1,cen2,bsize,mean,rms,med,nsig) 
          mask(cen1,cen2)=1
          image2(cen1,cen2)=mean
          std(cen1,cen2)=rms
          median(cen1,cen2)=med
130      continue
         write (*,'(a1,$)') '.'
120     continue
        write (*,*) 

c! interpolate if needed
        if (stepsize.ne.1) then
         write (*,*) '  Interpolating the images ... '
         write (*,'(a,$)') '   linear/cubic (1/2) : '
         read (*,*) i
         if (i.eq.3)        
     /   call sub_boxnoise_ip(mask,image2,std,median,n,m,
     /        ipblc,iptrc,nstep1,nstep2,stepsize)
         if (i.eq.2)        
     /   call sub_boxnoise_ip2(mask,image2,std,median,n,m,
     /        ipblc,iptrc,nstep1,nstep2,stepsize)
         if (i.eq.1)        
     /   call sub_boxnoise_ipln(mask,image2,std,median,n,m,
     /        ipblc,iptrc,nstep1,nstep2,stepsize)
        end if

c        strdev='/xs'
c        write (*,'(a,$)') '   Image of mask ... <Ret> '
c        read (*,*) 
c        lab='Image of mask'
c        call plotmask(mask,n,m,strdev)

c!
c! now fill up the boundaries
        call fillbt(image2,n,m,n,m,bsize,mask)
        call fillbt(std,n,m,n,m,bsize,mask)
        call fillbt(median,n,m,n,m,bsize,mask)
        call filllr(image2,n,m,n,m,bsize,mask)
        call filllr(std,n,m,n,m,bsize,mask)
        call filllr(median,n,m,n,m,bsize,mask)
        call filledges(image2,n,m,n,m,bsize,mask)
        call filledges(std,n,m,n,m,bsize,mask)
        call filledges(median,n,m,n,m,bsize,mask)

        strdev='/xs'
        write (*,'(a27,$)') '   Image of mean ... <Ret> '
        read (*,*) 
        lab='Image of mean'
        lab2='Original image'
        call grey2(image2,image1,n,m,n,m,strdev,lab,lab2,1,0)

        write (*,'(a27,$)') '   Image of rms ... <Ret>  '
        read (*,*) 
        lab='Image of rms'
        call grey2(std,image1,n,m,n,m,strdev,lab,lab2,1,0)

        write (*,'(a30,$)') '   Image of median ... <Ret>  '
        read (*,*) 
        lab='Image of median'
        call grey2(median,image1,n,m,n,m,strdev,lab,lab2,1,0)


        call writearray_bin2D(image2,n,m,n,m,f2,'mv')
        call writearray_bin2D(std,n,m,n,m,f3,'mv')
        call writearray_bin2D(median,n,m,n,m,f4,'mv')

        return
        end


        subroutine getstat(arr,n,m,cen1,cen2,bsize,mean,rms,med,nsig)
        implicit none
        integer n,m,bsize,i1,j1,cen1,cen2
        real*8 arr(n,m),mean,rms,tarr(bsize,bsize),med,tvec(bsize*bsize)
        real*8 nsig

        do 130 i1=1,bsize
         do 140 j1=1,bsize
          tarr(i1,j1)=arr(cen1-(bsize+1)/2+i1,cen2-(bsize+1)/2+j1)
140      continue
130     continue
        if (nsig.eq.0.d0) then
         call arrstat(tarr,bsize,bsize,1,1,bsize,bsize,rms,mean)
        else
         call sigclip(tarr,bsize,bsize,1,1,bsize,bsize,rms,mean,nsig)
        end if
        call arr8tovec8(tarr,bsize,bsize,bsize,bsize,tvec,bsize*bsize)
        call calcmedian(tvec,bsize*bsize,1,bsize*bsize,med)

        return
        end


        subroutine plotmask(mask,n,m,str)
        implicit none
        integer n,m,mask(n,m),nchar,i,j
        character str*5
        
        call pgbegin(0,str(1:nchar(str)),1,1)
        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)

        do 100 j=1,m
         do 110 i=1,n
          call pgsfs(2-mask(i,j))
          call pgrect(i*1.0-0.5,i*1.0+0.5,j*1.0-0.5,j*1.0+0.5)
110      continue
100     continue

        call pgend

        return
        end


        subroutine sub_boxnoise_ip(mask,meanim,std,median,x,y,blc,trc,
     /             sn1,sm1,step)
        implicit none
        integer x,y,blc(2),trc(2),mask(x,y)
        integer sn1,sm1,step,i,j
        real*8 meanim(x,y),std(x,y),median(x,y)
        real*8 x1a(sn1),x2a(sm1)
        real*8 submn(sn1,sm1)
        real*8 subrms(sn1,sm1),submed(sn1,sm1)
        real*8 y2amn(sn1,sm1),y2arms(sn1,sm1),y2amed(sn1,sm1)
        
c! Extract image at mask=1 and x and y axes for splie2.f
        do i=1,sn1
         x1a(i)=(blc(1)+(i-1)*step)*1.d0
        end do
        do j=1,sm1
         x2a(j)=(blc(2)+(j-1)*step)*1.d0
        end do
        do i=1,sn1
         do j=1,sm1
          submn(i,j)=meanim(int(x1a(i)),int(x2a(j)))
          subrms(i,j)=std(int(x1a(i)),int(x2a(j)))
          submed(i,j)=median(int(x1a(i)),int(x2a(j)))
         end do
        end do

c! splie2 computes the 2nd der at each pixel in image
        call splie2(x1a,x2a,submn,sn1,sm1,y2amn)
        call splie2(x1a,x2a,subrms,sn1,sm1,y2arms)
        call splie2(x1a,x2a,submed,sn1,sm1,y2amed)

c! now interpolate
        do i=1,x
         do j=1,y
          if (mask(i,j).eq.0) then
           call splin2(x1a,x2a,submn,y2amn,sn1,sm1,i*1.d0,j*1.d0,
     /          meanim(i,j))
           call splin2(x1a,x2a,subrms,y2arms,sn1,sm1,i*1.d0,j*1.d0,
     /          std(i,j))
           call splin2(x1a,x2a,submed,y2amed,sn1,sm1,i*1.d0,j*1.d0,
     /          median(i,j))
          end if
         end do
        end do

        return
        end


        subroutine sub_boxnoise_ip2(mask,meanim,std,median,x,y,blc,trc,
     /             sn1,sm1,step)
        implicit none
        integer x,y,blc(2),trc(2),mask(x,y)
        integer sn1,sm1,step,i,j
        real*8 meanim(x,y),std(x,y),median(x,y)
        real*8 x1a(sn1),x2a(sm1)
        real*8 submn(sn1,sm1)
        real*8 subrms(sn1,sm1),submed(sn1,sm1)
        real*8 y2amn(sn1,sm1),y2arms(sn1,sm1),y2amed(sn1,sm1)
        real*8 y1mn(sn1,y),y1rms(sn1,y),y1med(sn1,y)
        real*8 yytmp1(sn1),yytmp2(sn1),yytmp3(sn1)
        real*8 y2tmp1(sn1),y2tmp2(sn1),y2tmp3(sn1)
        integer i1,j1,hi,low,gx,dumi
        
c! Extract image at mask=1 and x and y axes for splie2.f
        dumi=0
        do i=blc(1),x
         if (mask(i,blc(2)).eq.1) then
          dumi=dumi+1
          x1a(dumi)=i*1.d0 
         end if 
        end do
        dumi=0
        do i=blc(2),y
         if (mask(blc(1),i).eq.1) then
          dumi=dumi+1
          x2a(dumi)=i*1.d0 
         end if 
        end do

        do i=1,sn1
         do j=1,sm1
          submn(i,j)=meanim(int(x1a(i)),int(x2a(j)))
          subrms(i,j)=std(int(x1a(i)),int(x2a(j)))
          submed(i,j)=median(int(x1a(i)),int(x2a(j)))
          y1mn(i,int(x2a(j)))=submn(i,j)
          y1rms(i,int(x2a(j)))=subrms(i,j)
          y1med(i,int(x2a(j)))=submed(i,j)
         end do
        end do

c! splie2 computes the 2nd der at each pixel in image
        call splie2(x1a,x2a,submn,sn1,sm1,y2amn)
        call splie2(x1a,x2a,subrms,sn1,sm1,y2arms)
        call splie2(x1a,x2a,submed,sn1,sm1,y2amed)

c! now interpolate, modified splint routine to do it faster
c! first extimate y in needed pts only if mask at x is 1 
c! sub*** and prev splie2 op has dimensions sn1,sm1 
c! but y1** has dimensions x,y
        do i=1,sn1
         gx=int(x1a(i))
         do j=1,sm1-1
          low=j
          hi=j+1
          do j1=int(x2a(low)+1),int(x2a(hi)-1)
           if (mask(gx,j1).eq.0) then
            call splintmod(x2a(low),x2a(hi),submn(i,low),submn(i,hi)
     /          ,y2amn(i,low),y2amn(i,hi),j1*1.d0,y1mn(i,j1))
            call splintmod(x2a(low),x2a(hi),subrms(i,low),subrms(i,hi)
     /          ,y2arms(i,low),y2arms(i,hi),j1*1.d0,y1rms(i,j1))
            call splintmod(x2a(low),x2a(hi),submed(i,low),submed(i,hi)
     /          ,y2amed(i,low),y2amed(i,hi),j1*1.d0,y1med(i,j1))
           end if
          end do
         end do
        end do
 
c! now calculate 2nd derivative at each of these points for every *row*
        do j=1,sm1
         do i=1,sn1
          yytmp1(i)=y1mn(i,int(x2a(j))) 
          yytmp2(i)=y1rms(i,int(x2a(j))) 
          yytmp3(i)=y1med(i,int(x2a(j))) 
         end do
         call spline(x1a,yytmp1,sn1,2.d30,2.d30,y2tmp1)
         call spline(x1a,yytmp2,sn1,2.d30,2.d30,y2tmp2)
         call spline(x1a,yytmp3,sn1,2.d30,2.d30,y2tmp3)
        end do

c! now do a splint again with correct x value for each row to get value
        do j=int(x2a(1)),int(x2a(sm1))
         do i=1,sn1-1
          low=i
          hi=i+1
          do i1=int(x1a(low)),int(x1a(hi))
           if (mask(i1,j).eq.0) then
            call splintmod(x1a(low),x1a(hi),y1mn(low,j),y1mn(hi,j)
     /          ,y2tmp1(low),y2tmp1(hi),i1*1.d0,meanim(i1,j))
            call splintmod(x1a(low),x1a(hi),y1rms(low,j),y1rms(hi,j)
     /          ,y2tmp1(low),y2tmp1(hi),i1*1.d0,std(i1,j))
            call splintmod(x1a(low),x1a(hi),y1med(low,j),y1med(hi,j)
     /          ,y2tmp1(low),y2tmp1(hi),i1*1.d0,median(i1,j))
           end if
          end do
         end do
        end do

        return
        end

        subroutine sub_boxnoise_ipln(mask,meanim,std,median,x,y,blc,trc,
     /             sn1,sm1,step)
        implicit none
        integer x,y,blc(2),trc(2),mask(x,y)
        integer sn1,sm1,step,i,j
        real*8 meanim(x,y),std(x,y),median(x,y)
        real*8 x1a(sn1),x2a(sm1),t,u
        integer i1,j1,hi,low,gx,dumi
        integer lowx,lowy,hix,hiy
        
        dumi=0
        do i=blc(1),x
         if (mask(i,blc(2)).eq.1) then
          dumi=dumi+1
          x1a(dumi)=i*1.d0 
         end if 
        end do
        dumi=0
        do i=blc(2),y
         if (mask(blc(1),i).eq.1) then
          dumi=dumi+1
          x2a(dumi)=i*1.d0 
         end if 
        end do

        do i=1,sn1-1
         do j=1,sm1-1
          do i1=int(x1a(i)),int(x1a(i+1))
           do j1=int(x2a(j)),int(x2a(j+1))
            lowx=int(x1a(i))
            hix=int(x1a(i+1))
            lowy=int(x2a(j))
            hiy=int(x2a(j+1))
            t=(i1-lowx)*1.d0/(hix-lowx)
            u=(j1-lowy)*1.d0/(hiy-lowy)
            meanim(i1,j1)=(1.d0-t)*(1.d0-u)*meanim(lowx,lowy)+
     /       t*(1.d0-u)*meanim(hix,lowy)+t*u*meanim(hix,hiy)+
     /       (1.d0-t)*u*meanim(lowx,hiy)
            std(i1,j1)=(1.d0-t)*(1.d0-u)*std(lowx,lowy)+
     /       t*(1.d0-u)*std(hix,lowy)+t*u*std(hix,hiy)+
     /       (1.d0-t)*u*std(lowx,hiy)
            median(i1,j1)=(1.d0-t)*(1.d0-u)*median(lowx,lowy)+
     /       t*(1.d0-u)*median(hix,lowy)+t*u*median(hix,hiy)+
     /       (1.d0-t)*u*median(lowx,hiy)
           end do
          end do
         end do
        end do


        return
        end

