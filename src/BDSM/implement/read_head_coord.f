c! read coord fits keywords from file
        
        subroutine read_head_coord(hdrfile,n,ctype,crpix,cdelt,
     /                             crval,crota,bm_pix)
        implicit none
        integer n,num,i,ind,nchar
        real*8 crpix(n),cdelt(n),crval(n),crota(n),dumr
        real*8 bm_pix(3),keyvalue
        character ctype(n)*8,hdrfile*500,dums*8,dumh*6,scratch*500,equal
        character f1*500,extn*10,keyword*500,comment*500,dir*500,fg*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        cdelt(1)=2.d0
        cdelt(2)=2.d0   !  for converting beam_asec to beam_pix
        call getline(hdrfile,scratch,num)
        open(unit=21,file=scratch(1:nchar(scratch))//hdrfile,
     /       status='old')
        do i=1,num
         read (21,*) dumh
         backspace(21)
         if (dumh(1:5).eq.'CRPIX'.or.dumh(1:5).eq.'CDELT'.or.
     /       dumh(1:5).eq.'CRVAL'.or.dumh(1:5).eq.'CROTA'.or.
     /       dumh(1:4).eq.'BMAJ'.or.dumh(1:4).eq.'BMIN'.or.
     /       dumh(1:3).eq.'BPA') then 
          read (21,*) dumh,equal,dumr
         else
          if (dumh(1:5).eq.'CTYPE') then
          read (21,*) dumh,equal,dums
          else
           read (21,*) dumh
          end if
         end if
         ind=999
         call convertchar(dumh(6:6),ind)
c         if (ind.gt.n.or.ind.eq.999) write (*,*) ' ## ERROR in ',hdrfile
         if (ind.le.n) then
          if (dumh(1:5).eq.'CRPIX') crpix(ind)=dumr     
          if (dumh(1:5).eq.'CDELT') cdelt(ind)=dumr     ! deg
          if (dumh(1:5).eq.'CRVAL') crval(ind)=dumr     ! deg
          if (dumh(1:5).eq.'CROTA') crota(ind)=dumr
          if (dumh(1:5).eq.'CTYPE') ctype(ind)=dums
         end if
         if (dumh(1:4).eq.'BMAJ') bm_pix(1)=dumr        ! deg fwhm of bmaj
         if (dumh(1:4).eq.'BMIN') bm_pix(2)=dumr        ! deg fwhm of bmin
         if (dumh(1:3).eq.'BPA') bm_pix(3)=dumr         ! deg 
        end do 
        close(21)
        bm_pix(1)=bm_pix(1)/abs(cdelt(1))               ! pix fwhm
        bm_pix(2)=bm_pix(2)/abs(cdelt(2))               ! pix fwhm

        return
        end

