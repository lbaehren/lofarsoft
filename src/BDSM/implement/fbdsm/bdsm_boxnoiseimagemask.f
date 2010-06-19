c! cut pasted from boxnoiseimage.f without std i/o stuff.
c! modified for mask (cos of blanked pixels). going to be a major pain to modify.

        subroutine bdsm_boxnoisemask(f1,f2,bsize,stepsize,n,m,
     /             runcode,blankv,outsideuniv_num,wcslen,wcs)
        implicit none
        character f1*500,strdev*5,lab*500,f2*500,f3*500
        character lab2*500,extn*20,runcode*2
        integer n,m,bsize,stepsize,nstep1,nstep2,i,j,nchar
        integer cen(n,m,2),cen1,cen2,trc1,trc2
        integer mask(n,m),ft,i1,j1,round,wcslen,wcs(wcslen)
        real*8 image(n,m),av(n,m),nsig,blankv,outsideuniv_num
        real*8 std(n,m),mean,rms,rms0,keyvalue
        integer ipblc(2),iptrc(2),di1,di2

c! read in the array
c! mask is 1/0 if pt is center of a box or not. tb updated later
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call initialiseimage(av,n,m,n,m,0.d0)
        call initialiseimage(std,n,m,n,m,0.d0)
        call initialisemask(mask,n,m,n,m,0)

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
        nsig=3.d0
c        call arrstat(image,n,m,1,1,n,m,rms0,mean)  ! to calc rms of whole image
        do 120 i=1,nstep1
         do 130 j=1,nstep2
          cen1=cen(i,j,1)
          cen2=cen(i,j,2)
          call bdsm_getstatmask(image,n,m,cen1,cen2,bsize,mean,rms,
     /         nsig,blankv,outsideuniv_num,wcslen,wcs) ! this mask is for blanked and outofuniv pixel
          mask(cen1,cen2)=1
          av(cen1,cen2)=mean
          std(cen1,cen2)=rms
130      continue
120     continue

c! interpolate if needed
        if (stepsize.ne.1) then
         i=1
         if (i.eq.1)        ! using this
     /   call bdsm_boxnoise_iplnblank(image,mask,av,std,n,m,
     /     ipblc,iptrc,nstep1,nstep2,stepsize,blankv,outsideuniv_num,
     /     wcslen,wcs)
        end if
c!
c! now fill up the boundaries
        call fillbt(av,n,m,n,m,bsize,mask)
        call fillbt(std,n,m,n,m,bsize,mask)
        call filllr(av,n,m,n,m,bsize,mask)
        call filllr(std,n,m,n,m,bsize,mask)
        call filledges(av,n,m,n,m,bsize,mask)
        call filledges(std,n,m,n,m,bsize,mask)

        f3=f2(1:nchar(f2))//'.mean'
        call writearray_bin2D(av,n,m,n,m,f3,runcode)
        f3=f2(1:nchar(f2))//'.rmsd'
        call writearray_bin2D(std,n,m,n,m,f3,runcode)

        return
        end

        subroutine bdsm_getstatmask(arr,n,m,cen1,cen2,bsize,mean,
     /             rms,nsig,blankv,outsideuniv_num,wcslen,wcs)
        implicit none
        integer n,m,bsize,i1,j1,cen1,cen2,mask(bsize,bsize)
        integer num1,wcslen,wcs(wcslen)
        real*8 arr(n,m),mean,rms,tarr(bsize,bsize),blankv
        real*8 outsideuniv_num,nsig
        logical isgoodpix

         num1=0
         do 130 i1=1,bsize
          do 140 j1=1,bsize
           tarr(i1,j1)=arr(cen1-(bsize+1)/2+i1,cen2-(bsize+1)/2+j1)
           if (tarr(i1,j1).eq.blankv) then
            mask(i1,j1)=0
           else
            mask(i1,j1)=1
            num1=num1+1
           end if
140       continue
130      continue
        if (outsideuniv_num.gt.0.d0) then
         do 230 i1=1,bsize
          do 240 j1=1,bsize
           call wcs_isgoodpix((cen1-(bsize+1)/2+i1)*1.d0,
     /         (cen2-(bsize+1)/2+j1)*1.d0,wcs,wcslen,isgoodpix)
           if (.not.isgoodpix) then
            mask(i1,j1)=0
            num1=num1-1
           end if
240       continue
230      continue
        end if

        if (num1.eq.0) then
         rms=0.d0
         mean=0.d0
        else
         if (num1.le.20) then   ! 20 is arbit number. hack.
          call arrstatmask(tarr,mask,bsize,bsize,1,1,bsize,bsize,
     /         rms,mean,'q')
         else
          if (nsig.ne.0.d0) then
           call sigclipmask(tarr,mask,bsize,bsize,1,1,bsize,bsize,
     /          rms,mean,nsig)
          else
           call arrstatmask(tarr,mask,bsize,bsize,1,1,bsize,bsize,
     /          rms,mean,'q')
          end if
         end if
        end if
        if (num1.le.5) rms=0.d0  ! so test for this as well.

        return
        end
c!
c!
c!
        subroutine bdsm_boxnoise_iplnblank(image,mask,meanim,std,
     /     x,y,blc,trc,sn1,sm1,step,blankv,outsideuniv_num,
     /     wcslen,wcs)
        implicit none
        integer x,y,blc(2),trc(2),mask(x,y)
        integer sn1,sm1,step,i,j,wcslen,wcs(wcslen)
        real*8 meanim(x,y),std(x,y),mmean,mstd,stdbox,meanbox,blankv
        real*8 x1a(sn1),x2a(sm1),t,u,image(x,y),outsideuniv_num
        integer i1,j1,hi,low,gx,dumi,nb,nb1
        integer lowx,lowy,hix,hiy,npixgood
        logical isgood1,isgood2,isgood3,isgood4,isgood
        
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
c         write (*,*) dumi,x2a(dumi)
         end if 
        end do

        do i=1,sn1-1
         do j=1,sm1-1
c          write (*,*) i,j,sn1-1,sm1-1
          lowx=int(x1a(i))
          hix=int(x1a(i+1))
          lowy=int(x2a(j))
          hiy=int(x2a(j+1))
          call wcs_isgoodpix(lowx*1.d0,lowy*1.d0,wcs,wcslen,isgood1)
          call wcs_isgoodpix(lowx*1.d0,hiy*1.d0,wcs,wcslen,isgood2)
          call wcs_isgoodpix(hix*1.d0,lowy*1.d0,wcs,wcslen,isgood3)
          call wcs_isgoodpix(hix*1.d0,hiy*1.d0,wcs,wcslen,isgood4)
c          write (*,*) x1a(i),x1a(i+1),x2a(j),x2a(j+1)
c          write (*,*) lowx,hix,lowy,hiy
        
          nb=0
          if ((image(lowx,lowy).ne.blankv).and.isgood1) nb=nb+1
          if ((image(lowx,hiy).ne.blankv).and.isgood2) nb=nb+1
          if ((image(hix,lowy).ne.blankv).and.isgood3) nb=nb+1
          if ((image(hix,hiy).ne.blankv).and.isgood4) nb=nb+1

          if (nb.eq.0) then ! if all 4 corners are from blanked boxes
           npixgood=0
           do i1=int(x1a(i)),int(x1a(i+1))  ! then all pts inside are 0 as well - MAYBE
            do j1=int(x2a(j)),int(x2a(j+1))
             meanim(i1,j1)=0.d0
             std(i1,j1)=0.d0
             call wcs_isgoodpix(i1*1.d0,j1*1.d0,wcs,wcslen,isgood)
             if ((image(i1,j1).ne.blankv).and.isgood) 
     /            npixgood=npixgood+1
            end do
           end do
          end if
c          write (*,*) 
       
          if (nb.eq.0) then
           if (npixgood.gt.0) then   ! u can have 4 corners blanked and image inside not.
                                                ! then just take mean and av and put them everywhere.
                                                ! if all 4 corners are inside univ, so will interior usually 
            call callstat_subbox(image,lowx,hix,lowy,hiy,x,y,
     /           blankv,npixgood,15,meanbox,stdbox)
            do i1=int(x1a(i)),int(x1a(i+1))
             do j1=int(x2a(j)),int(x2a(j+1))
              meanim(i1,j1)=meanbox
              std(i1,j1)=stdbox
             end do
            end do
           end if
          end if

          if (nb.eq.4) then  ! all 4 are ok
           do i1=int(x1a(i)),int(x1a(i+1))
            do j1=int(x2a(j)),int(x2a(j+1))
             t=(i1-lowx)*1.d0/(hix-lowx)
             u=(j1-lowy)*1.d0/(hiy-lowy)
             meanim(i1,j1)=(1.d0-t)*(1.d0-u)*meanim(lowx,lowy)+
     /        t*(1.d0-u)*meanim(hix,lowy)+t*u*meanim(hix,hiy)+
     /        (1.d0-t)*u*meanim(lowx,hiy)
             std(i1,j1)=(1.d0-t)*(1.d0-u)*std(lowx,lowy)+
     /        t*(1.d0-u)*std(hix,lowy)+t*u*std(hix,hiy)+
     /        (1.d0-t)*u*std(lowx,hiy)
            end do
           end do
          end if

          if (nb.gt.0.and.nb.lt.4) then  ! if 1,2 or 3 corners are from blanked boxes
           mmean=0.d0
           if ((image(lowx,lowy).ne.blankv).and.isgood1) 
     /          mmean=mmean+meanim(lowx,lowy)
           if ((image(lowx,hiy).ne.blankv).and.isgood2) 
     /          mmean=mmean+meanim(lowx,hiy)
           if ((image(hix,lowy).ne.blankv).and.isgood3) 
     /          mmean=mmean+meanim(hix,lowy)
           if ((image(hix,hiy).ne.blankv).and.isgood4) 
     /          mmean=mmean+meanim(hix,hiy)
           mmean=mmean/nb
           if ((image(lowx,lowy).eq.blankv).and.isgood1) 
     /          meanim(lowx,lowy)=mmean
           if ((image(lowx,hiy).eq.blankv).and.isgood2) 
     /          meanim(lowx,hiy)=mmean
           if ((image(hix,lowy).eq.blankv).and.isgood3) 
     /          meanim(hix,lowy)=mmean
           if ((image(hix,hiy).eq.blankv).and.isgood4) 
     /          meanim(hix,hiy)=mmean

           nb1=0
           mstd=0.d0  ! put the average of the good corners onto the bad ones.
           if ((image(lowx,lowy).ne.blankv).and.(std(lowx,lowy).ne.0.d0)
     /          .and.isgood1) then
             mstd=mstd+std(lowx,lowy)
             nb1=nb1+1
           end if
           if ((image(lowx,hiy).ne.blankv).and.(std(lowx,hiy).ne.0.d0)
     /          .and.isgood2) then
             mstd=mstd+std(lowx,hiy)
             nb1=nb1+1
           end if
           if ((image(hix,lowy).ne.blankv).and.(std(hix,lowy).ne.0.d0)
     /          .and.isgood3) then
             mstd=mstd+std(hix,lowy)
             nb1=nb1+1
           end if
           if ((image(hix,hiy).ne.blankv).and.(std(hix,hiy).ne.0.d0)
     /          .and.isgood4) then
             mstd=mstd+std(hix,hiy)
             nb1=nb1+1
           end if
           if (nb1.eq.0) then
            mstd=0.d0
           else
            mstd=mstd/nb1
           end if
           if ((image(lowx,lowy).eq.blankv).or.(std(lowx,lowy).eq.0.d0)
     /          .and.isgood1) std(lowx,lowy)=mstd
           if ((image(lowx,hiy).eq.blankv).or.(std(lowx,hiy).eq.0.d0)
     /          .and.isgood2) std(lowx,hiy)=mstd
           if ((image(hix,lowy).eq.blankv).or.(std(hix,lowy).eq.0.d0)
     /          .and.isgood3) std(hix,lowy)=mstd
           if ((image(hix,hiy).eq.blankv).or.(std(hix,hiy).eq.0.d0)
     /          .and.isgood4) std(hix,hiy)=mstd

           do i1=int(x1a(i)),int(x1a(i+1))  ! and then do the usual thing.
            do j1=int(x2a(j)),int(x2a(j+1))
             t=(i1-lowx)*1.d0/(hix-lowx)
             u=(j1-lowy)*1.d0/(hiy-lowy)
             meanim(i1,j1)=(1.d0-t)*(1.d0-u)*meanim(lowx,lowy)+
     /        t*(1.d0-u)*meanim(hix,lowy)+t*u*meanim(hix,hiy)+
     /        (1.d0-t)*u*meanim(lowx,hiy)
             std(i1,j1)=(1.d0-t)*(1.d0-u)*std(lowx,lowy)+
     /        t*(1.d0-u)*std(hix,lowy)+t*u*std(hix,hiy)+
     /        (1.d0-t)*u*std(lowx,hiy)
            end do
           end do
          end if
         end do
        end do
c! since mean and rms images overstep a bit into blanked regions. easier to do it here.
        do i=1,x
         do j=1,y
          if (image(i,j).eq.blankv) then
           meanim(i,j)=blankv
           std(i,j)=blankv
          end if
         end do
        end do

        return
        end
c!
c!
c!
        subroutine callstat_subbox(image,lowx,hix,lowy,hiy,x,y,
     /          blankv,npixgood,num,meanbox,stdbox)
        implicit none
        integer x,y,num,lowx,hix,lowy,hiy,i,j,npixgood
        real*8 image(x,y),blankv,meanbox,stdbox
        real*8 subim(hix-lowx+1,hiy-lowy+1)
        integer mask(hix-lowx+1,hiy-lowy+1),n,m,nn

        nn=0
        n=hix-lowx+1
        m=hiy-lowy+1
        do i=lowx,hix   
         do j=lowy,hiy
          subim(i-lowx+1,j-lowy+1)=image(i,j)
          if (image(i,j).eq.blankv) then
           mask(i-lowx+1,j-lowy+1)=0
          else
           mask(i-lowx+1,j-lowy+1)=1
           nn=nn+1
          end if
         end do
        end do
        if (npixgood.gt.num) then 
         call sigclipmask(subim,mask,n,m,1,1,n,m,stdbox,meanbox,4.d0)
        else
         call arrstatmask(subim,mask,n,m,1,1,n,m,stdbox,meanbox,'q')
        end if
        if (npixgood.eq.1) stdbox=meanbox

        return
        end


