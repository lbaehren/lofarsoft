c! associated n source lists for a master list.
c! CHANGE GAUL SRL FORMAT

        subroutine call_multi_asrl(nf,nfr,filename,solnname,scratch,
     /             srldir,n1,n2,n3,plotdir,plotspectra,gsmsolnname,
     /             tofit,spinflux,plotpairs)
        implicit none
        integer nf,nisl(nf),ngaul(nf),gpi(nf),n,m,nffmts,nsrc(nf),i
        integer maxnisl,maxngaul,maxnsrc,nchar,sumngaul,n1,n2,tofit(nf)
        integer error,nfr,n3,j
        character filename(nf)*500,solnname(nf)*500,ffmts*500,srldir*500
        character scratch*500,fullname*500,dums*500,makepdf*500
        character plotdir*500,plotspectra*500,gsmsolnname*500,extn*20
        character comment*500,dir*500,keystrng*500,keyword*500,fg*500
        character spinflux*500,plotpairs*500
        real*8 spindef,dumr

        maxnisl=0
        maxngaul=0
        maxnsrc=0
        do j=n1,n3
         write (j,*) 'Number_of_surveys = ',nf
        end do
        do i=1,nf
         fullname=filename(i)(1:nchar(filename(i)))//'.'//
     /            solnname(i)(1:nchar(solnname(i)))
         call srllistheaders(fullname,n,m,nisl(i),ngaul(i),nsrc(i),
     /        gpi(i),srldir)
         maxnisl=max(maxnisl,nisl(i))
         maxnsrc=max(maxnsrc,nsrc(i))
         maxngaul=max(maxngaul,ngaul(i))
         do j=n1,n3
          write (j,*) 'File = ',fullname(1:nchar(fullname))
          write (j,*) 'Used_for_fitting = ',tofit(i)
         end do
        end do
        sumngaul=0
        do i=1,nf
         sumngaul=sumngaul+ngaul(i)
        end do

        fg="gsmparadefine"
        extn=""
        dir="./"
        keyword="spindef"
        call get_keyword(fg,extn,keyword,keystrng,spindef,
     /    comment,"r",dir,error)
        keyword="makepdf"
        call get_keyword(fg,extn,keyword,makepdf,dumr,
     /    comment,"s",dir,error)

        call multi_asrl(nf,nfr,filename,solnname,scratch,srldir,nisl,
     /       ngaul,nsrc,gpi,maxnisl,maxngaul,maxnsrc,sumngaul,n1,n2,n3,
     /       plotdir,plotspectra,gsmsolnname,tofit,spindef,makepdf,
     /       spinflux,plotpairs)

        return
        end
c!
c!
c!
        subroutine multi_asrl(nf,nfr,filename,solnname,scratch,srldir,
     /   nisl,ngaul,nsrc,gpi,maxnisl,maxngaul,maxnsrc,sumngaul,n1,n2,n3,
     /   plotdir,plotspectra,gsmsolnname,tofit,spindef,makepdf,spinflux,
     /   plotpairs)
        implicit none
        integer nf,nisl(nf),ngaul(nf),gpi(nf),n,m,nffmts,nsrc(nf),nfr
        integer sumngaul
        character filename(nf)*500,solnname(nf)*500,ffmts*500,srldir*500
        character scratch*500,fullname(nf)*500,dums*500,fn*500,head*500
        integer maxnisl,maxngaul,maxnsrc,nchar,dumi,dumi1,i,igau
        integer gaulid(nf,maxngaul),islid(nf,maxngaul),flag(nf,maxngaul)
        integer srcid(nf,maxngaul),isrc,n1,n2,tofit(nf),n3
        real*8 ra(nf,maxngaul),dec(nf,maxngaul),rstd(nf,maxngaul)
        real*8 avra(sumngaul),avdec(sumngaul)
        real*8 eavra(sumngaul),eavdec(sumngaul)
        real*8 rav(nf,maxngaul),eypix(nf,maxngaul),dtot(nf,maxngaul)
        real*8 sstd(nf,maxngaul),sav(nf,maxngaul),tot(nf,maxngaul)
        real*8 bmaj(nf,maxngaul),bmin(nf,maxngaul),bpa(nf,maxngaul)
        real*8 xpix(nf,maxngaul),expix(nf,maxngaul),ypix(nf,maxngaul)
        real*8 era(nf,maxngaul),edec(nf,maxngaul),eflux(nf,maxngaul)
        real*8 peak(nf,maxngaul),flux(nf,maxngaul)
        real*8 ebpa(nf,maxngaul),edbmin(nf,maxngaul),q(nf,maxngaul)
        real*8 epeak(nf,maxngaul),ebmaj(nf,maxngaul),ebmin(nf,maxngaul)
        real*8 dbmaj(nf,maxngaul),edbmaj(nf,maxngaul),dbmin(nf,maxngaul)
        real*8 dbpa(nf,maxngaul),edbpa(nf,maxngaul),chisq(nf,maxngaul)
        character code(nf,maxngaul)*4,spinflux*500
        character code4(maxnsrc)*4,gsmsolnname*500
        character names(nf,maxngaul)*40,dumc*1,makepdf*500
        real*8 pbmaj(nf),pbmin(nf),pbpa(nf),bm_pixarr(3,nf),freq(nf)
        real*8 freq0
        integer wcslen,sum2fit
        parameter (wcslen=450)
        integer wcsarr(wcslen,nf),wcs(wcslen),nassoc
        integer assoc_arr(sumngaul,nf),assoc_ind(maxngaul,nf),round
        real*8 spin_arr(sumngaul,5),espin_arr(sumngaul,5)
        real*8 avsp00,stdsp00,medsp00,dumr,spindef,racen(nf),deccen(nf)
        character plotdir*500,plotspectra*500,plotpairs*500
        real*8 calcflux0(nf,maxngaul),calcflux1(nf,maxngaul)
        real*8 cdeltarr(3,nf)

        freq0=200.d6

c! read in all gaul and srl lists.
        write (*,*) '  Reading in all gaul lists'
        do i=1,nf
         fullname(i)=filename(i)(1:nchar(filename(i)))//'.'//
     /            solnname(i)(1:nchar(solnname(i)))
         fn=srldir(1:nchar(srldir))//fullname(i)(1:nchar(fullname(i)))//
     /      '.gaul.bin'   
         open(unit=22,file=fn(1:nchar(fn)),form='unformatted') 
         do igau=1,ngaul(i)
          read (22) gaulid(i,igau),islid(i,igau),flag(i,igau),
     /     tot(i,igau),dtot(i,igau),peak(i,igau),epeak(i,igau),
     /     ra(i,igau),era(i,igau),dec(i,igau),
     /     edec(i,igau),xpix(i,igau),expix(i,igau),
     /     ypix(i,igau),eypix(i,igau),bmaj(i,igau),ebmaj(i,igau),
     /     bmin(i,igau),ebmin(i,igau),bpa(i,igau),
     /     ebpa(i,igau),dbmaj(i,igau),edbmaj(i,igau),
     /     dbmin(i,igau),edbmin(i,igau),dbpa(i,igau),
     /     edbpa(i,igau),sstd(i,igau),sav(i,igau),rstd(i,igau),
     /     rav(i,igau),chisq(i,igau),q(i,igau),srcid(i,igau)
         end do
         close(22)
         fn=srldir(1:nchar(srldir))//fullname(i)(1:nchar(fullname(i)))
     /      //'.srl'
         open(unit=22,file=fn(1:nchar(fn)),form='formatted')
334      read (22,*) head
         if (head.ne.'fmt') goto 334
         do isrc=1,nsrc(i)
          read (22,*) dumi1,dumi,dumi,code4(isrc)
          if (dumi1.ne.isrc) write (*,*) ' RONG ',i,isrc
         end do
         close(22)
         do 120 igau=1,ngaul(i)
          if (srcid(i,igau).ne.0) code(i,igau)=code4(srcid(i,igau))
          if (srcid(i,igau).eq.0) code(i,igau)=' '
120      continue

         fn=srldir(1:nchar(srldir))//fullname(i)(1:nchar(fullname(i)))
     /      //'.gaul.star'
         open(unit=21,file=fn,status='old')
         do igau=1,ngaul(i)
          if (flag(i,igau).eq.0) 
     /    read (21,777) dumi,dumi,dumr,dumc,dumi,dumi,dumr,dumr,
     /          dumr,dumr,dumi,dumr,names(i,igau)
         end do
         close(21)
        end do
777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a40)  ! from writeaipsfiles.f + nam(i)

c! the flux for spectral index
        if (spinflux.eq.'peak') then
         do i=1,nf
          do igau=1,ngaul(i)
           flux(i,igau)=peak(i,igau)
           eflux(i,igau)=epeak(i,igau)
          end do
         end do
        else
         do i=1,nf
          do igau=1,ngaul(i)
           flux(i,igau)=tot(i,igau)
           eflux(i,igau)=dtot(i,igau)
          end do
         end do
        end if

c! make dir for plots
        call system('rm -fr '//plotdir(1:nchar(plotdir))// 
     /       gsmsolnname(1:nchar(gsmsolnname)))
        call system('mkdir '//plotdir(1:nchar(plotdir))// 
     /       gsmsolnname(1:nchar(gsmsolnname)))

c! get parameters
        call sub_multiasrl_getp(nf,fullname,scratch,pbmaj,pbmin,pbpa,
     /   bm_pixarr,wcsarr,wcslen,freq,racen,deccen,cdeltarr,solnname)

c! now to associate them all and make master list
        write (*,*) '  Associating all lists'
        call assoc_multiasrl(nf,maxngaul,sumngaul,ngaul,code,
     /       flag,xpix,ypix,pbmaj,pbmin,pbpa,ra,dec,assoc_arr,assoc_ind,
     /       nassoc,bmaj,bmin,bpa,n1,n2,n3)

c! calculate spectral indices for each source
        write (*,*) '  Calculating spectral indices for all assoc.s'
        call call_spin_multiasrl(sumngaul,nf,assoc_arr,nassoc,
     /       freq,flux,eflux,maxngaul,freq0,spin_arr,espin_arr,
     /       tofit,ra,dec,era,edec,avra,avdec,eavra,eavdec,filename)

c! check if it makes sense and look for systematics
        write (*,*) '  Quality control'
        call gsm_qc(nassoc,sumngaul,spin_arr,espin_arr,avdec,plotdir,
     /   gsmsolnname,freq,nf,scratch,filename,assoc_arr,flux,maxngaul,
     /   eflux,plotpairs)

c! plots all spectra if asked
        if (plotspectra.eq.'true') then
         write (*,*) '  Plotting spectra of all associations'
         call gsm_plotspectra(plotdir,plotspectra,sumngaul,
     /        nf,assoc_arr,nassoc,
     /        freq,flux,eflux,maxngaul,freq0,spin_arr,espin_arr,
     /        avsp00,stdsp00,medsp00,filename,solnname,names,ra,dec,
     /        gsmsolnname,tofit,scratch,n2,makepdf)
         end if

c! computes fluxes at freqs not fitted
         call vec_int_sum(tofit,nf,nf,sum2fit)
         if (sum2fit.lt.nf) then
          write (*,*) '  Computing fluxes at unfitted freq.s'
          call gsm_calcflux_nofit(plotdir,sumngaul,nf,assoc_arr,nassoc,
     /         freq,flux,eflux,maxngaul,freq0,spin_arr,espin_arr,
     /         filename,solnname,gsmsolnname,tofit,calcflux0,
     /         calcflux1,spindef,dec)
         end if

c! output the OR_GSM
        write (*,*) '  Writing out files'
        call write_orgsm(spin_arr,espin_arr,sumngaul,5,nf,maxngaul,
     /       nassoc,flux,eflux,freq,n1,n2,n3,assoc_arr,avsp00,stdsp00,
     /       medsp00,tofit,calcflux0,calcflux1,ra,dec,era,
     /       edec,avra,eavra,avdec,eavdec)

c! plots stuff for unfitted frequencies 
         if (sum2fit.lt.nf) then
          write (*,*) '  Plotting stuff for unfitted freq.s'
          call gsm_plot_unfitted(plotdir,plotspectra,sumngaul,
     /         nf,assoc_arr,nassoc,
     /         freq,flux,eflux,maxngaul,freq0,spin_arr,espin_arr,
     /         filename,solnname,ra,dec,era,edec,racen,deccen,
     /         gsmsolnname,tofit,scratch,calcflux0,calcflux1,
     /         bm_pixarr,cdeltarr,srldir,avra,avdec)
         end if

c! output GSM at chosen frequencies
        if (nfr.gt.0) then
         write (*,*) '  Calculating GSM at chosen frequencies'
         call calc_gsm(nf,nfr,sumngaul,scratch,spin_arr,espin_arr,
     /        avra,avdec,gsmsolnname,nassoc,filename,solnname,
     /        tofit,freq0,eavra,eavdec,assoc_arr,spindef,flux,
     /        eflux,maxngaul,freq)
        end if
        
        return
        end
c!
c!
c!
        subroutine sub_multiasrl_getp(nf,fullname,scratch,bmaj,bmin,bpa,
     /             bm_pixarr,wcsarr,wcslen,freq,racen,deccen,cdeltarr,
     /             solnname)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        integer nf,i,error,wcslen,wcsarr(wcslen,nf),wcs(wcslen),nchar,j
        integer error1
        real*8 bmaj(nf),bmin(nf),bpa(nf),bm_pixarr(3,nf)
        real*8 cdeltarr(3,nf)
        character fullname(nf)*500,extn*20,keyword*500,keystrng*500
        character solnname(nf)*500
        character scratch*500,ctype(3)*8,f3*500,comment*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),freq(nf)
        real*8 racen(nf),deccen(nf),xcen,ycen,naxis1(nf),naxis2(nf)
        logical exists
        
        do i=1,nf
         extn='.header'
         keyword='BMAJ'
         call get_keyword(fullname(i),extn,keyword,keystrng,bmaj(i),
     /        comment,'r',scratch,error)
         keyword='BMIN'
         call get_keyword(fullname(i),extn,keyword,keystrng,bmin(i),
     /        comment,'r',scratch,error)
         keyword='BPA'
         call get_keyword(fullname(i),extn,keyword,keystrng,bpa(i),
     /        comment,'r',scratch,error)
         call get_freq_cube(fullname(i),scratch,freq(i),error)
         if (error.ne.0) then
          write (*,*) ' No frequency information available'
          stop
         end if
         keyword='NAXIS1'
         call get_keyword(fullname(i),extn,keyword,keystrng,naxis1(i),
     /        comment,'r',scratch,error1)
         keyword='NAXIS2'
         call get_keyword(fullname(i),extn,keyword,keystrng,naxis2(i),
     /        comment,'r',scratch,error1)
        
         f3=scratch(1:nchar(scratch))//fullname(i)(1:nchar(fullname(i)))
     /      //'.header'
         inquire(file=f3,exist=exists)
         call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
         f3=fullname(i)(1:nchar(fullname(i)))//'.header'
         if (exists) call read_head_coord(f3(1:nchar(f3)),3,
     /      ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
         call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
         do j=1,wcslen
          wcsarr(j,i)=wcs(j)
         end do
         xcen=naxis1(i)/2.d0 
         ycen=naxis2(i)/2.d0 
         call wcs_xy2radec(xcen,ycen,racen(i),deccen(i),error,wcs,
     /        wcslen)
         racen(i)=racen(i)*rad
         deccen(i)=deccen(i)*rad
         call freewcs(wcs,wcslen)
         bm_pixarr(1,i)=bm_pix(1)
         bm_pixarr(2,i)=bm_pix(2)
         bm_pixarr(3,i)=bm_pix(3)
         cdeltarr(1,i)=cdelt(1)
         cdeltarr(2,i)=cdelt(2)
         cdeltarr(3,i)=cdelt(3)
         if (error1.eq.1.and.solnname(i)(1:nchar(solnname(i))).eq.'s2g')
     /   then 
          racen(i)=12.d0*15.d0
          deccen(i)=20.d0
         end if
        end do
        write (*,*) '## ',racen
        write (*,*) '## ',deccen

        return
        end





