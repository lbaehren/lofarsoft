c! like tvlab with lty=3

        subroutine drawcoords(blc4,trc4,f1,n,scratch)
        implicit none
        include "constants.inc"
        real*4 blc4(2),trc4(2),x4,y4
        integer subn,subm,n,nchar,error1,ira,idec,nsub1,nsub2
        character f1*500,hdrfile*500,ctype(n)*8,scratch*500
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(n),ra,dec
        real*8 blc(2),trc(2),ramin,ramax,decmin,decmax,x,y,dra,ddec
        real*8 xra,ydec,xran,ydecn,r1,r2,d1,d2,z1,z2
        logical isgp1,isgp2
        integer wcslen,error2
        parameter (wcslen=450)
        integer wcs(wcslen)

        hdrfile=f1(1:nchar(f1))//'.header'
        call read_head_coord(hdrfile,n,ctype,crpix,cdelt,
     /       crval,crota,bm_pix,scratch)

c! get wcs struct
        call wcs_struct(n,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        blc(1)=blc4(1)
        blc(2)=blc4(2)
        trc(1)=trc4(1)
        trc(2)=trc4(2)
        call sub_drawcoord_cminmax(blc,trc,ramin,ramax,decmin,decmax,
     /       wcs,wcslen)
        ramin=0.d0
        ramax=24.0d0*15/rad
        decmin=-20.d0/rad
        decmax=90.d0/rad

        nsub1=15
        nsub2=90
        dra=(ramax-ramin)/nsub1       ! in rad
        ddec=(decmax-decmin)/nsub1

c        call pgsci(3)
        do ira=1,nsub1
         r1=ramin+(ira-1)*dra
         r2=ramin+(ira-1)*dra
         do idec=1,nsub2
          d1=decmin+(idec-1)*(decmax-decmin)/nsub2
          d2=decmin+(idec+1-1)*(decmax-decmin)/nsub2    ! all coords in radians
          call wcs_radec2xy(r1,d1,xra,ydec,error1,wcs,wcslen)
          call wcs_radec2xy(r2,d2,xran,ydecn,error2,wcs,wcslen)
          if (error1.eq.0.and.error2.eq.0) then   ! exclude back of universe 
           call wcs_isgoodpix(xra,ydec,wcs,wcslen,isgp1)
           call wcs_isgoodpix(xran,ydecn,wcs,wcslen,isgp2)
           if (isgp1.and.isgp2) then
            x4=xra
            y4=ydec
            call pgmove(x4,y4)
            x4=xran
            y4=ydecn
            call pgdraw(x4,y4)
           end if
          end if
         end do
        end do

        do idec=1,nsub1
         d1=decmin+(idec-1)*ddec
         d2=decmin+(idec-1)*ddec
         do ira=1,nsub2
          r1=ramin+(ira-1)*(ramax-ramin)/nsub2
          r2=ramin+(ira+1-1)*(ramax-ramin)/nsub2
          call wcs_radec2xy(r1,d1,xra,ydec,error1,wcs,wcslen)
          call wcs_radec2xy(r2,d2,xran,ydecn,error2,wcs,wcslen)
          if (error1.eq.0.and.error2.eq.0) then   ! exclude back of universe 
           call wcs_isgoodpix(xra,ydec,wcs,wcslen,isgp1)
           call wcs_isgoodpix(xran,ydecn,wcs,wcslen,isgp2)
           if (isgp1.and.isgp2) then
            x4=xra
            y4=ydec
            call pgmove(x4,y4)
            x4=xran
            y4=ydecn
            call pgdraw(x4,y4)
           end if
          end if
         end do
        end do

        call pgsci(1)
c        call freewcs(wcs,wcslen)

        return
        end
c!
c!
c!  ------------------------------------------------------------------------------------------------------
c!
c!
        subroutine sub_drawcoord_cminmax(blc,trc,ramin,ramax,decmin,
     /        decmax,wcs,wcslen)
        implicit none
        include "constants.inc"
        integer subn,subm,nchar,err
        character f1*500,hdrfile*500
        real*8 blc(2),trc(2),ramin,ramax,decmin,decmax,x,y
        integer wcslen,wcs(wcslen)

        ramin=99.d9
        decmin=99.d9
        ramax=-99.d9
        decmax=-99.d9
        
c! terrible terrible method, will do for now.
c! 4 corners 
        call sub_drawcoords_1(blc(1),blc(2),ramin,ramax,decmin,decmax,
     /       wcs,wcslen) 
        call sub_drawcoords_1(blc(1),trc(2),ramin,ramax,decmin,decmax,
     /       wcs,wcslen) 
        call sub_drawcoords_1(trc(1),blc(2),ramin,ramax,decmin,decmax,
     /       wcs,wcslen) 
        call sub_drawcoords_1(trc(1),trc(2),ramin,ramax,decmin,decmax,
     /       wcs,wcslen) 
c! 4 diag pts
        call sub_drawcoords_1(blc(1),0.5*(blc(2)+trc(2)),ramin,ramax,
     /       decmin,decmax,wcs,wcslen)
        call sub_drawcoords_1(0.5*(blc(1)+trc(1)),blc(2),ramin,ramax,
     /       decmin,decmax,wcs,wcslen)
        call sub_drawcoords_1(trc(1),0.5*(blc(2)+trc(2)),ramin,ramax,
     /       decmin,decmax,wcs,wcslen)
        call sub_drawcoords_1(0.5*(blc(1)+trc(1)),trc(2),ramin,ramax,
     /       decmin,decmax,wcs,wcslen)

        return
        end
c!
c!
c!
        subroutine sub_drawcoords_1(x,y,ramin,ramax,decmin,decmax,
     /             wcs,wcslen)
        implicit none
        include "constants.inc"
        integer subn,subm,n
        real*8 ra,dec,ramin,ramax,decmin,decmax,x,y
        logical isgoodp
        integer wcslen,wcs(wcslen)

        call wcs_isgoodpix(x,y,wcs,wcslen,isgoodp)
        if (isgoodp) then
         if (ra.lt.ramin) ramin=ra
         if (ra.gt.ramax) ramax=ra
         if (dec.lt.decmin) decmin=dec
         if (dec.gt.decmax) decmax=dec
        end if

        return
        end




