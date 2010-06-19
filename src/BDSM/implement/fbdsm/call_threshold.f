
        subroutine call_threshold(f1,fname,imagename,runcode,scratch,
     /             n,m,l0,l)
        implicit none
        include "constants.inc"
        include "sourcecounts.inc"
        integer n,m,round,i,ind,nchar,error,j,l,l0
        real*8 false_p,source_p,thresh_pix,thresh_isl,minpix_isl
        character f1*500,extn*20,filen*500,keyword*500,thresh*500,fn*500
        character comment*500,code*1,keystrng*500,dir*500,fname*500
        character fg*500,scratch*500,imagename*500,rmsmap*500
        character mean_map*500,f3*500,runcode*2
        real*8 erf,smin_L,cutoff,std,freq,omega,image(n,m),dumr
        real*8 spin,n1,n2,s1,s2,alpha,A,bmaj,bmin,cdelt(3),keyvalue
        real*8 blankn,blankv,maxsize_beam,av_im,std_im,fdralpha
        real*8 pcrit,sigcrit,avimage(n,m),rmsimage(n,m)

        if (runcode(2:2).eq.'q') write (*,*) '  Calculating thresholds'
        extn='.header'
        keyword='BMAJ'
        call get_keyword(fname,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch,error)
        keyword='BMIN'
        call get_keyword(fname,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch,error)
        keyword='CDELT1'
        call get_keyword(fname,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(fname,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)
        extn='.bstat'
        keyword='blank_num'
        call get_keyword(fname,extn,keyword,keystrng,blankn,
     /       comment,'r',scratch,error)
        if (blankn.gt.0.d0) then
         extn='.bstat'
         keyword='blank_val'
         call get_keyword(fname,extn,keyword,keystrng,blankv,
     /        comment,'r',scratch,error)
        end if

        fg='paradefine'
        dir='./'
        extn=''
        keyword="thresh"
        call get_keyword(fg,extn,keyword,thresh,keyvalue,
     /    comment,"s",dir,error)
        if (thresh.ne.'hard'.and.thresh.ne.'fdr') thresh='default'
        keyword="fdr_alpha"
        call get_keyword(fg,extn,keyword,keystrng,fdralpha,
     /       comment,"r",dir,error)
        if (fdralpha.eq.0.d0) fdralpha=0.05d0
        keyword="thresh_pix"
        call get_keyword(fg,extn,keyword,keystrng,thresh_pix,
     /    comment,"r",dir,error)
        keyword="thresh_isl"
        call get_keyword(fg,extn,keyword,keystrng,thresh_isl,
     /    comment,"r",dir,error)

        extn='.bparms'
        keyword='rms_map'
        comment=" 'Use constant rms value or rms map'"
        call get_keyword(fname,extn,keyword,rmsmap,keyvalue,
     /    comment,"s",scratch,error)

        if (thresh.eq.'default') then
         cutoff=5.d0
         false_p=0.5d0*(1.d0-erf(5.d0/1.4142d0))*n*m
 
         spin=-0.8d0
         extn='.bstat'
         keyword='rms_clip'
         call get_keyword(fname,extn,keyword,keystrng,std,
     /        comment,'r',scratch,error)
         extn='.bstat'
         keyword='omega'
         call get_keyword(fname,extn,keyword,keystrng,omega,
     /        comment,'r',scratch,error)
         extn='.header'
         keyword='CRVAL3'
         call get_keyword(fname,extn,keyword,keystrng,freq,
     /        comment,'r',scratch,error)
         smin_L=std*cutoff*((1.4d9/freq)**spin)            ! n sig at 1.4 GHz
         do 100 i=1,nbin
          ss(i)=ss(i)*1.0d-3            !   Jy
          if (ss(i).lt.smin_L) ind=i
100      continue
         close(21)
         n1=nn(ind)
         n2=nn(nbin)
         s1=ss(ind)
         s2=ss(nbin)
         alpha=1.d0-dlog(n1/n2)/dlog(s1/s2)  ! dN =A omega S^-alpha dS
         A=(alpha-1.d0)*n1/(s1**(1.d0-alpha))

         source_p=2.d0*pi*A*bmaj*bmin/fwsig/fwsig/abs(cdelt(1))
     /            /abs(cdelt(2))*omega*((cutoff*std)**(1.d0-alpha))/
     /            ((1.d0-alpha)*(1.d0-alpha))
 
         if (false_p.lt.0.1d0*source_p) then
          thresh='hard'
         else
          thresh='fdr '
         end if
        end if  ! if thresh=default in paradefine

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(fname,extn,keyword,keystrng,std_im,
     /       comment,'r',scratch,error)

        f3=fname(1:nchar(fname))
        call get_avmap(f3,n,m,avimage)
c! create normalised image for fdr here
        if (thresh.eq.'fdr') then
         extn='.img'
         call readarray_bin(n,m,image,n,m,imagename,extn)
         if (rmsmap.eq.'const') then
          do j=1,m
           do i=1,n
            image(i,j)=(image(i,j)-avimage(i,j))/std_im
           end do
          end do
         else   ! use rms map
          extn=".img"
          f3=fname(1:nchar(fname))//'.rmsd'
          call readarray_bin(n,m,rmsimage,n,m,f3,extn)
          do j=1,m
           do i=1,n
            if (image(i,j).ne.blankv) then
             if (rmsimage(i,j).ne.0.d0) then
              image(i,j)=(image(i,j)-avimage(i,j))/rmsimage(i,j)
             end if
            end if
           end do
          end do
         end if
         f3=fname(1:nchar(fname))//'.norm'
         call writearray_bin2D(image,n,m,n,m,f3,runcode)
        end if

c! call fdr here
        if (thresh.eq.'fdr') then
         call callfdr(fname,bmaj,bmin,cdelt,n,m,scratch,
     /           blankn,blankv,n*m-int(blankn),fdralpha,pcrit,sigcrit)

c! image has not enuff sources, almost perfect noise, so pcrit=0, sigcrit=inf. put hard.
         if (pcrit.eq.0.d0) then  
          thresh='hard'
         else
          thresh_pix=sigcrit
         end if
        end if

        extn='.bparms'
        keyword='fdralpha'
        comment=" 'Alpha for FDR'"
        call put_keyword(fname,extn,keyword,keyword,fdralpha,
     /       comment,'R',scratch)
        
        keyword='thresh'
        comment=" 'Source pixel threshold - hard or fdr'"
        call put_keyword(fname,extn,keyword,thresh,0.d0,
     /       comment,'S',scratch)

        if (thresh_pix.eq.0.d0) thresh_pix=5.d0
        keyword='thresh_pix'
        comment=" 'Value in sigma for hard threshold'"
        call put_keyword(fname,extn,keyword,keyword,thresh_pix,
     /       comment,'R',scratch)

        if (thresh_isl.eq.0.d0) thresh_isl=3.d0
        keyword='thresh_isl'
        comment=" 'Threshold in sigma for island boundary'"
        call put_keyword(fname,extn,keyword,keyword,thresh_isl,
     /       comment,'R',scratch)

        return
        end



