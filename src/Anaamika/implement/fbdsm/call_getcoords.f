
        subroutine call_getcoords(fitsname,fitsdir,scratch,runcode)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        character fitsdir*500,scratch*500,fitsname*500,filename*500
        character runcode*2,fn*500,ctype(3)*8,s*1,f2*500
        integer nchar,wcslen,hh,mm,dd,ma,status
        parameter (wcslen=450)
        integer wcs(wcslen),error1
        logical exists
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        real*8 x,y,ra,dec,ss,sa,dra,ddec

cf2py   intent(in) fitsdir,scratch,fitsname,runcode

c! get file name
        if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /     fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
         filename=fitsname(1:nchar(fitsname)-5)
        else
         filename=fitsname
        end if
        
c! read in fits file and also header
        call callreadfits(fitsname,filename,filename,fitsdir,scratch,
     /       runcode)

c! get astrometry keywords
        fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /     '.header'
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        f2=filename(1:nchar(filename))//'.header'
        if (exists) call read_head_coord(f2(1:nchar(f2)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix,scratch)

c! get wcs
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
c        status=wcsput(wcs, wcs_lonpole, 999.d0, 1, 0)

c! get coordinates
333     write (*,'(a,$)') '   Enter pixel location (0,0 to quit) : '
        read (*,*) x,y
        if (x.ne.0.and.y.ne.0) then
         call wcs_xy2radec(x,y,ra,dec,error1,wcs,wcslen)
         if (error1.ne.0) write (*,*) ' ERROR !!!! ',x,y
         call correctrarad(ra)
         dra=ra*rad
         ddec=dec*rad
         call convertra(dra,hh,mm,ss)
         call convertdec(ddec,s,dd,ma,sa)
         if ('s'.eq.'-') dd=-dd
         write (*,777) '  Coordinates at (',x,',',y,') : ',
     /               hh,mm,ss,dd,ma,sa,' (',dra,',',ddec,')'
         goto 333
        end if
777     format(a18,f8.2,a1,f8.2,a4,i2,1x,i2,1x,f5.2,1x,i3,1x,i2,1x,f5.2,
     /         a2,f9.5,a1,f10.5,a1)

c        call freewcs(wcs,wcslen)

        return
        end


