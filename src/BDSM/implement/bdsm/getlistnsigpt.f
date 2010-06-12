
        subroutine getlistnsigpt(image,rmsim,x,y,n,m,npt,
     /       xpt,ypt,nm,f1)
        implicit none
        integer x,y,n,m,nm,npt,i,j
        integer xpt(nm),ypt(nm)
        real*8 image(x,y),nsig,fl(nm),rmsim(x,y)
        real*8 rmsclip,avclip,threshpix,keyvalue
        character keyword*500,keystrng*500,extn*10
        character f1*500,comment*500,dir*500,fg*500,scratch*500

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
        keyword='mean_clip'
        call get_keyword(f1,extn,keyword,keystrng,avclip,
     /       comment,'r',scratch)
        keyword='thresh_pix'
        call get_keyword(f1,extn,keyword,keystrng,threshpix,
     /       comment,'r',scratch)

        npt=0
        do i=1,n
         do j=1,m
          if (image(i,j)-avclip.ge.threshpix*rmsim(i,j)) then
           npt=npt+1
           xpt(npt)=i
           ypt(npt)=j
           fl(npt)=image(i,j)
          end if
         end do
        end do

        if (nm.lt.npt) write (*,*) '  ## ERROR ! xpt, ypt TOO SMALL'
        call sort3(nm,npt,fl,xpt,ypt)

        return
        end

