c! calculate background noise map

        subroutine callrmsd(f1,runcode,scratch)
        implicit none
        character f1*500,extn*10,ch1*1,runcode*2,f2*500,scratch*500
        integer n,m

        if (runcode(2:2).eq.'q') write (*,*) '  Calculating rms image'
        extn='.img'
        call readarraysize(f1,extn,n,m)
        call rmsd(f1,n,m,runcode)

c! next step
        if (runcode(1:1).eq.'m') then
         write (*,*) 
         write (*,'(a,$)') '   Continue (form islands : form) (y)/n ? '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         f2=""
         if (ch1.eq.'y') call formislands(f1,f2,runcode,scratch)
        end if

        return
        end
c!
c! -------------------------------- SUBROUTINES ----------------------------
c!
        subroutine rmsd(f1,n,m,runcode)
        implicit none
        character f1*500,extn*10,keyword*500,keystrng*500
        character comment*500,dir*500,fg*500,scratch*500,runcode*2
        integer n,m,boxsize,stepsize
        real*8 nsig,dumr,keyvalue

c!
c! read image and headers
        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='boxsize_th'
        call get_keyword(f1,extn,keyword,keystrng,dumr,
     /       comment,'r',scratch)
        boxsize=dumr
        extn='.bstat'
        keyword='stepsize_th'
        call get_keyword(f1,extn,keyword,keystrng,dumr,
     /       comment,'r',scratch)
        stepsize=dumr
c!
c! create mean and rms images
        call bdsm_boxnoise(f1,boxsize,stepsize,n,m,runcode)
c!
c! check if variation is 'real'
        call takermsd(f1,n,m,boxsize)

        return
        end
c!
c!      ----------
c!
        subroutine takermsd(f1,n,m,boxsize)
        implicit none
        integer nchar,n,m,boxsize
        character f1*500,extn*10,f2*500,keyword*500,rmsmap*500
        character comment*500,keystrng*500,dir*500,fg*500,scratch*500
        real*8 image(n,m),stdsub,avsub,fw_pix
        real*8 std_im,av_im,bmaj,bmin,cdelt(2),keyvalue

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,std_im,
     /       comment,'r',scratch)
        keyword='mean_clip'
        call get_keyword(f1,extn,keyword,keystrng,av_im,
     /       comment,'r',scratch)
        extn='.img'
        f2=f1(1:nchar(f1))//'.rmsd'
        call readarray_bin(n,m,image,n,m,f2,extn)
        call arrstat(image,n,m,boxsize/2,boxsize/2,n-boxsize/2,
     /       m-boxsize/2,stdsub,avsub)

c! if rms_map is const, use rms_clip as estimator of constant rms 
c! else is map then use f1.rmsd for rms map, after filtering if u want (?)
        extn='.header'
        keyword='CDELT1'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch)
        keyword='CDELT2'
        call get_keyword(f1,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch)
        keyword='BMAJ'
        call get_keyword(f1,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch)
        keyword='BMIN'
        call get_keyword(f1,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch)
        fw_pix=sqrt(abs(bmaj*bmin/cdelt(1)/cdelt(2)))
        if (stdsub.gt.1.1d0*std_im/1.4142d0/boxsize*fw_pix) then
         rmsmap='map  '
        else
         rmsmap='const'
        end if
        extn='.bstat'
        keyword='rms_map'
        comment=" 'Use constant rms value or rms map'"
        call put_keyword(f1,extn,keyword,rmsmap,0.d0,
     /       comment,'S',scratch)

        return
        end


