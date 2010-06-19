c! callgaul2srl identifies a group of gaussians to be made into a source and here we actually
c! construct the source.
c! We go back to islandlist file, identify each pixel with a gaussian upto fwhm and the rest 
c! depending on which (set of) gaussian(s) gives it higher value. stored in mask.
c! then calc source properties by moment (as well as shapelet ?). 

        subroutine gaul_to_source(islct,mask,subn,subm,subim,bmar_p,
     /   mompara,k,speak,sra,sdec,delx,dely,ctype,crpix,cdelt,crval,
     /   crota,bm_pix,gaus_d,rms_isl,maxpeak,mpx,mpy,mra,mdec,sisl,
     /   wcs,wcslen)
        implicit none
        include "constants.inc"
        integer islct,subn,subm,k,mask(subn,subm),error1,sisl
        integer hix,lowx,lowy,hiy,delx,dely,round,ssubimsize
        integer maxx,maxy,i,j,blc(2),trc(2),ssubimsizex,ssubimsizey
        real*8 subim(subn,subm),mom1(2),mom2(2),xpix,ypix,m11
        real*8 tot,mompara(6),bmar_p,t,u,speak,sra,sdec
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        real*8 gaus_o(3),gaus_r(3),gaus_d(3),error,maxv,rms_isl
        real*8 maxpeak,mpx,mpy,mra,mdec
        real*8 e1,e2,e3,ed1,ed2,ed3
        character ctype(3)*8
        integer wcslen,wcs(wcslen)

c! do very basic moment analysis now. sort out later.
        call momanalmask(subim,subn,subm,mask,mompara,k,bmar_p)

c! calculate peak position of maximum by fitting gaussian around max pixel
        maxv=subim(1,1)
        do i=1,subn
         do j=1,subm
          if (subim(i,j).gt.maxv) then
           maxv=subim(i,j)
           maxx=i
           maxy=j
          end if
         end do
        end do
c! initial guess is sensible but not from solutions since u can have overlapping gaus.s
c! maxpeak, mpx and mpy are value and location of fitted gaussian.
        ssubimsize=round(max(bm_pix(1),bm_pix(2))*2)+1
        if (ssubimsize.lt.5) ssubimsize=5
        blc(1)=max(1,maxx-(ssubimsize-1)/2)
        blc(2)=max(1,maxy-(ssubimsize-1)/2)
        trc(1)=min(subn,maxx+(ssubimsize-1)/2)
        trc(2)=min(subm,maxy+(ssubimsize-1)/2)
        ssubimsizex=trc(1)-blc(1)+1
        ssubimsizey=trc(2)-blc(2)+1
        call sub_g2s_fitmaxgaus(subn,subm,subim,mask,maxv,maxx,
     /       maxy,maxpeak,blc,trc,ssubimsizex,ssubimsizey,bm_pix,
     /       rms_isl,k,delx,dely,mpx,mpy)
        
c! calculate peak by bilinear interpolation around centroid.
        lowx=int(mompara(2))
        hix=int(mompara(2))+1
        lowy=int(mompara(3))
        hiy=int(mompara(3))+1
c         write (*,*) sisl,mompara(2),mompara(3),k,lowx+delx,hix+delx,
c     /     lowy+dely,hiy+dely
c        do i=1,subn
c         do j=1,subm
c         if (mask(i,j).eq.k) write (*,*) i+delx,j+dely
c         end do
c        end do

        if (mask(lowx,lowy).ne.k.or.mask(lowx,hiy).ne.k.or.
     /      mask(hix,lowy).ne.k.or.mask(hix,hiy).ne.k) 
     /    write (*,*) ' !!! ERRORR !!! ',k,lowx,hix,lowy,hiy
        t=(mompara(2)-lowx)/(hix-lowx)
        u=(mompara(3)-lowy)/(hiy-lowy)
        speak=(1.d0-t)*(1.d0-u)*subim(lowx,lowy)+t*(1.d0-u)*
     /    subim(hix,lowy)+t*u*subim(hix,hiy)+(1.d0-t)*u*subim(lowx,hiy)

c! convert x0,y0 to ra,dec
         call wcs_xy2radec(mompara(2)+delx,mompara(3)+dely,sra,sdec,
     /        error1,wcs,wcslen)
         if (error1.ne.0) write (*,*) '  ### ERROR1 ###'
         sra=sra*rad
         sdec=sdec*rad
         call wcs_xy2radec(mpx,mpy,mra,mdec,error1,wcs,wcslen)
         if (error1.ne.0) write (*,*) '  ### ERROR1 ###'
         mra=mra*rad
         mdec=mdec*rad

c! 'deconvolve'
         gaus_o(1)=mompara(4)/(cdelt(1)*3600.d0*fwsig)          ! in sig_pix, sig_pix, deg
         gaus_o(2)=mompara(5)/(cdelt(2)*3600.d0*fwsig)
         gaus_o(3)=mompara(6)
         gaus_r(1)=bm_pix(1)/fwsig     ! in sig_pix, sig_pix, deg
         gaus_r(2)=bm_pix(2)/fwsig
         gaus_r(3)=bm_pix(3)
         call deconv(gaus_o,gaus_r,gaus_d,e1,e2,e3,ed1,ed2,ed3,error)
                
        return
        end
c!
c!      ==========
c!
        subroutine momanalmask(subim,subn,subm,mask,mompara,k,bmar_p)
        implicit none
        include "constants.inc"
        integer subn,subm,k,mask(subn,subm),i,j
        real*8 subim(subn,subm),tot,mom1(2),mom2(2),xpix,ypix,m11
        real*8 mompara(6),bmar_p,dumr

        mom1(1)=0.d0
        mom1(2)=0.d0
        mom2(1)=0.d0
        mom2(2)=0.d0
        m11=0.d0
        tot=0.d0
        do i=1,subn
         do j=1,subm
          if (mask(i,j).eq.k) then
           tot=tot+subim(i,j)
           mom1(1)=mom1(1)+i*subim(i,j)
           mom1(2)=mom1(2)+j*subim(i,j)
          end if
         end do
        end do
        mompara(1)=tot/bmar_p
        mompara(2)=mom1(1)/tot
        mompara(3)=mom1(2)/tot
        do i=1,subn
         do j=1,subm
          if (mask(i,j).eq.k) then
           mom2(1)=mom2(1)+(i-mompara(2))*(i-mompara(2))*subim(i,j)
           mom2(2)=mom2(2)+(j-mompara(3))*(j-mompara(3))*subim(i,j)
           m11=m11+(i-mompara(2))*(j-mompara(3))*subim(i,j)
          end if
         end do
        end do
        mompara(4)=sqrt((mom2(1)+mom2(2)+sqrt((mom2(1)-mom2(2))*
     /    (mom2(1)-mom2(2))+4.d0*m11*m11))/(2.0d0*tot))*fwsig
        mompara(5)=sqrt((mom2(1)+mom2(2)-sqrt((mom2(1)-mom2(2))*
     /    (mom2(1)-mom2(2))+4.d0*m11*m11))/(2.0d0*tot))*fwsig
        dumr=atan(abs(2.d0*m11/(mom2(1)-mom2(2))))
        call atanproper(dumr,2.d0*m11,mom2(1)-mom2(2))
        mompara(6)=0.5d0*dumr*rad-90.d0
        if (mompara(6).lt.0.d0) mompara(6)=mompara(6)+180.d0
                
        return
        end
c!
c!      ==========
c!
        subroutine sub_g2s_fitmaxgaus(subn,subm,subim,mask,maxv,maxx,
     /         maxy,maxpeak,blc,trc,ssubimsizex,ssubimsizey,bm_pix,
     /         rms_isl,k,delx,dely,dumr1,dumr2)
        implicit none
        include "constants.inc"
        integer maxx,maxy,ssubimsizex,ssubimsizey,subn,subm,i,j
        integer dumi1,dumi2,mask(subn,subm),delx,dely
        real*8 maxv,maxpeak,subim(subn,subm)
        real*8 ssig(ssubimsizex,ssubimsizey)
        real*8 ssubim(ssubimsizex,ssubimsizey),bm_pix(3)
        integer ia(6),k,smask(ssubimsizex,ssubimsizey),blc(2),trc(2)
        integer rmask(ssubimsizex,ssubimsizey)
        real*8 a(6),rms_isl,covar(6,6),alpha(6,6),alamda
        real*8 chisq,chisqold,dumr1,dumr2
        
        data ia/1,1,1,1,1,1/

c! subim(maxx,y) is ssubim(round(ssubimsize/2),ditto)
c! mask isnt regular mask but has pixel values = src id. so need to
c! create a proper mask
        maxpeak=-9.d9
        do i=1,ssubimsizex
         do j=1,ssubimsizey
          ssubim(i,j)=subim(i-1+blc(1),j-1+blc(2))
          smask(i,j)=mask(i-1+blc(1),j-1+blc(2))
          ssig(i,j)=maxv/20.d0
          if (smask(i,j).eq.k) then
           rmask(i,j)=1
          else
           rmask(i,j)=0
          end if

          if (ssubim(i,j).gt.maxpeak) then
           maxpeak=ssubim(i,j)
           a(1)=i
           a(2)=j
          end if

         end do
        end do

        dumi1=(0.5d0*(ssubimsizex+ssubimsizey)-1)/2.d0
        a(1)=maxv*0.5
        a(2)=dumi1*1.5d0
        a(3)=dumi1*1.4d0
        a(4)=bm_pix(1)/fwsig*1.8
        a(5)=bm_pix(2)/fwsig*1.1
        a(6)=bm_pix(3)*2

        dumi2=1
        alamda=-1.d0
        call mrqmin2dm(ssubim,rmask,ssig,ssubimsizex,ssubimsizey,a,ia,
     /       6,covar,alpha,6,chisq,alamda,1)
        chisqold=chisq

334     call mrqmin2dm(ssubim,rmask,ssig,ssubimsizex,ssubimsizey,a,ia,
     /       6,covar,alpha,6,chisq,alamda,1)
        if (dumi2.le.35.or.abs(chisqold-chisq)/chisqold.gt.0.01d0) then
         chisqold=chisq
         dumi2=dumi2+1
         goto 334
        end if

        alamda=0.d0
        call mrqmin2dm(ssubim,rmask,ssig,ssubimsizex,ssubimsizey,a,ia,
     /       6,covar,alpha,6,chisq,alamda,1)

        if (a(2).gt.1.d0.and.a(2).lt.ssubimsizex.and.
     /      a(3).gt.1.d0.and.a(3).lt.ssubimsizey.and.
     /      a(4).lt.ssubimsizex.and.a(5).lt.ssubimsizey) then
         maxpeak=a(1)    
        else
         maxpeak=maxv
        end if
        dumr1=a(2)-dumi1+maxx-1+delx
        dumr2=a(3)-dumi1+maxy-1+dely

        return
        end

