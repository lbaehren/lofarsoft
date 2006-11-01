c! this is to do basic quality checks, and set up input parameters for
c! an automated running of bdsm.
c! num of bm/src is approx n*m/(num pix > 5 sigma)/(1-alpha), alpha is diff src ct, +ve

        subroutine preprocess(f1,runcode,scratch)
        implicit none
        character f1*500,fn*500,ctype(3)*8,extn*10,ch1*1,scratch*500
        integer nchar,n,m
        logical exists
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),keyvalue
        character runcode*2,f2*500
        
        if (runcode(2:2).eq.'q') write (*,*) '  Preprocessing image'

        extn='.header'
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        if (exists) then 
         f2=f1(1:nchar(f1))//extn(1:nchar(extn))
         call read_head_coord(f2(1:nchar(f2)),3,
     /        ctype,crpix,cdelt,crval,crota,bm_pix)
        else
         call writefitshead(f1,3,ctype,crpix,cdelt,crval,crota,
     /        bm_pix,scratch)
        end if

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call pp_basicstats(f1,n,m)              ! writes in 'f1.bstat'
        
        call pp_bmpersrc(f1,n,m)                ! approx formula in notebook
        call pp_imrms_para(f1,n,m)
        call pp_thresholds(f1,n,m)

c! next step
        if (runcode(1:1).eq.'m') then 
         write (*,*) 
         write (*,'(a,$)') '   Continue (noise map : rmsd) (y)/n ? '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         if (ch1.eq.'y') call callrmsd(f1,runcode,scratch)
        end if
        
        return
        end
c!
c!  ------------------------  SUBROUTINES  -----------------------------------
c!
        subroutine pp_basicstats(f1,n,m)              ! writes in 'f1.bstat'
        implicit none
        include "constants.inc"
        integer n,m,mx(2),mn(2),nchar
        character f1*500,extn*10,fn*500,keyword*500,fg*500,scratch*500
        character comment*500,code*1,keystrng*500,dir*500
        real*8 std,av,stdclip,avclip,mxv,mnv,kappa,cdelt(2),keyvalue
        
        kappa=3.d0
        call get_imagestats(f1,kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv)

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.header'
        keyword='CDELT1'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.bstat'
        open(unit=21,file=fn,status='unknown')
         write (21,*) 'image_name = ',f1(1:nchar(f1))," 'Name of image'"
         write (21,*) 'size_x = ',n," '# pixels on x axis'"
         write (21,*) 'size_y = ',m," '# pixels on y axis'"
         write (21,*) 'mean = ',av," 'Mean over image (BUNIT)'"
         write (21,*) 'rms = ',std," 'RMS over image (BUNIT)'"
         write (21,*) 'kappa_clip = ',kappa," 'Kappa'"
         write (21,*) 'mean_clip = ',avclip,
     /                " 'Mean, kappa-clip (BUNIT)'"
         write (21,*) 'rms_clip = ',stdclip," 'RMS, kappa-clip (BUNIT)'"
         write (21,*) 'max_pixel_x = ',mx(1)," 'x value of max int pix'"
         write (21,*) 'max_pixel_y = ',mx(2)," 'y value of max int pix'"
         write (21,*) 'min_pixel_x = ',mn(1)," 'x value of min int pix'"
         write (21,*) 'min_pixel_y = ',mn(2)," 'y value of min int pix'"
         write (21,*) 'max_value = ',mxv," 'Maximum intensity (BUNIT)'"
         write (21,*) 'min_value = ',mnv," 'Minimum intensity (BUNIT)'"
         write (21,*) 'omega = ',n*m*abs(cdelt(1))*abs(cdelt(2))/
     /          rad/rad," 'Solid angle of image (str)'"
        close(21)
        
        return
        end
c!
c!      ----------
c!
        subroutine pp_bmpersrc(f1,n,m)                ! approx formula in notebook
        implicit none
        integer n,m,numpix,nchar
        character f1*500,extn*10,fn*500,keyword*500,fg*500
        character comment*500,code*1,keystrng*500,dir*500,scratch*500
        real*8 alpha,rmsclip,bmpersrc,keyvalue

        alpha=2.5d0    ! power law of diff src count

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch)

        extn='.img'
        call get_numpix_val(f1,extn,n,m,5.d0*rmsclip,'ge',numpix)
        if (numpix.eq.0) numpix=1
        bmpersrc=n*m/((alpha-1.d0)*numpix)

        extn='.bstat'
        keyword='bmpersrc_th'
        comment=" 'Estimated # beams per source'"
        call put_keyword(f1,extn,keyword,keyword,bmpersrc,
     /       comment,'R',scratch)
        
        return
        end
c!
c!      ----------
c!
        subroutine pp_imrms_para(f1,n,m)
        implicit none
        include "constants.inc"
        integer n,m,nchar,boxsize,stepsize,conv_filt,round
        character f1*500,extn*10,fn*500,keyword*500,fg*500
        character comment*500,code*1,keystrng*500,dir*500,scratch*500
        real*8 maxv,cdelt(2),bmaj,kappa,rmsclip,bmpersrc,keyvalue 
        integer brightsize,largesize,intersrcsep

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='max_value'
        call get_keyword(f1,extn,keyword,keystrng,maxv,comment,
     /       'r',scratch)
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch)
        keyword='bmpersrc_th'
        call get_keyword(f1,extn,keyword,keystrng,bmpersrc,
     /       comment,'r',scratch)
        extn='.header'
        keyword='CDELT1'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='BMAJ'
        call get_keyword(f1,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch)
        kappa=3.d0
        brightsize=round(2.d0*bmaj/abs(cdelt(1))/fwsig*sqrt(2.d0*dlog
     /             (maxv/(kappa*rmsclip))))   ! 'size' of brightest source
        largesize=round(min(n,m)/4.d0)        ! atleast 4 boxes on each side
        intersrcsep=round(sqrt(bmpersrc)*2.d0*bmaj/abs(cdelt(1)))
c!      scales in noise part of map
c!      wavelet decomp
c!      scales in dirty beam

        boxsize=round(sqrt(brightsize*largesize*1.d0))
        if (intersrcsep.gt.brightsize.and.intersrcsep.lt.largesize)
     /   boxsize=round(sqrt(intersrcsep*largesize*1.d0))
        if (boxsize.lt.40) then
         boxsize=min(40,min(n,m))
        end if
c! and so on for each estimate commented above.

        stepsize=round(min(boxsize/3.d0,min(n,m)/10.d0))
        
        extn='.bstat'
        keyword='boxsize_th'
        comment=" 'Boxsize for rms map'"
        call put_keyword(f1,extn,keyword,keyword,boxsize*1.d0,
     /       comment,'R',scratch)
        keyword='stepsize_th'
        comment=" 'Stepsize for rms map'"
        call put_keyword(f1,extn,keyword,keyword,stepsize*1.d0,
     /       comment,'R',scratch)

        return
        end
c!
c!      ----------
c!
        subroutine pp_thresholds(f1,n,m)
        implicit none
        include "constants.inc"
        integer n,m,round,nbin,i,ind,nchar
        real*8 false_p,source_p
        character f1*500,extn*10,filen*500,keyword*500,thresh*500,fn*500
        character comment*500,code*1,keystrng*500,dir*500
        character fg*500,scratch*500
        real*8 erf,ss(500),nn(500),smin_L,cutoff,std,freq,omega
        real*8 spin,n1,n2,s1,s2,alpha,A,bmaj,bmin,cdelt(2),keyvalue

        cutoff=5.d0
        false_p=0.5d0*(1.d0-erf(5.d0/1.4142d0))*n*m

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        spin=-0.8d0
        filen='data'
        call getline(filen,dir,nbin)
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,std,
     /       comment,'r',scratch)
        extn='.bstat'
        keyword='omega'
        call get_keyword(f1,extn,keyword,keystrng,omega,
     /       comment,'r',scratch)
        extn='.header'
        keyword='CRVAL3'
        call get_keyword(f1,extn,keyword,keystrng,freq,
     /       comment,'r',scratch)
        smin_L=std*cutoff*((1.4d9/freq)**spin)            ! n sig at 1.4 GHz
        open(unit=21,file=filen,status='old')
        do 100 i=1,nbin
         read (21,*) ss(i),nn(i)       !  mJy and N(>S)/str
         ss(i)=ss(i)*1.0d-3            !   Jy
         if (ss(i).lt.smin_L) ind=i
100     continue
        close(21)
        n1=nn(ind)
        n2=nn(nbin)
        s1=ss(ind)
        s2=ss(nbin)
        alpha=1.d0-dlog(n1/n2)/dlog(s1/s2)  ! dN =A omega S^-alpha dS
        A=(alpha-1.d0)*n1/(s1**(1.d0-alpha))

        extn='.header'
        keyword='BMAJ'
        call get_keyword(f1,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch)
        keyword='BMIN'
        call get_keyword(f1,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch)
        keyword='CDELT1'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        source_p=2.d0*pi*A*bmaj*bmin/fwsig/fwsig/abs(cdelt(1))
     /           /abs(cdelt(2))*omega*((cutoff*std)**(1.d0-alpha))/
     /           ((1.d0-alpha)*(1.d0-alpha))

        if (false_p.lt.0.1d0*source_p) then
         thresh='hard'
        else
         thresh='fdr '
c         call fdr
        end if
        extn='.bstat'
        keyword='thresh'
        comment=" 'Source pixel threshold - hard or fdr'"
        call put_keyword(f1,extn,keyword,thresh,0.d0,
     /       comment,'S',scratch)

        keyword='thresh_pix'
        comment=" 'Value in sigma for hard threshold'"
        call put_keyword(f1,extn,keyword,keyword,4.d0,
     /       comment,'R',scratch)

        keyword='thresh_isl'
        comment=" 'Threshold in sigma for island boundary'"
        if (thresh.eq.'hard') call put_keyword(f1,extn,keyword,keyword,
     /      3.d0,comment,'R',scratch)
        if (thresh.eq.'fdr ') call put_keyword(f1,extn,keyword,keyword,
     /      3.d0,comment,'R',scratch)
        close(21)
        
        return
        end
c!
