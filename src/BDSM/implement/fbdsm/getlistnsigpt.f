
        subroutine getlistnsigpt(image,rmsim,avim,x,y,n,m,npt,
     /       xpt,ypt,nm,f1,f2,blankn,blankv,wcslen,wcs)
        implicit none
        integer x,y,n,m,nm,npt,i,j,wcslen,wcs(wcslen)
        integer xpt(nm),ypt(nm),error
        real*8 image(x,y),nsig,fl(nm),rmsim(x,y),dumr
        real*8 rmsclip,threshpix,keyvalue,avim(x,y)
        character keyword*500,keystrng*500,extn*20,f2*500
        character f1*500,comment*500,dir*500,fg*500,scratch*500
        real*8 blankn,blankv,outsideuniv_num
        logical isgoodpix

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f2,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch,error)
        keyword='outsideuniv_num'
        call get_keyword(f2,extn,keyword,keystrng,outsideuniv_num,
     /       comment,'r',scratch,error)
        extn='.bparms'
        keyword='rms_clip'
        keyword='thresh_pix'
        call get_keyword(f2,extn,keyword,keystrng,threshpix,
     /       comment,'r',scratch,error)

        if (outsideuniv_num.ge.1.d0) then   ! messy but saves time.
         npt=0
         do j=1,m
          do i=1,n
           if (image(i,j)-avim(i,j).ge.threshpix*rmsim(i,j)) then
            if (image(i,j).ne.blankv) then   ! bad hack for now for WN40299HB
             if (rmsim(i,j).ne.blankv) then   ! bad hack for now for WN40299HB
              call wcs_isgoodpix(i*1.d0,j*1.d0,wcs,wcslen,isgoodpix)
              if (isgoodpix) then
               npt=npt+1
               xpt(npt)=i
               ypt(npt)=j
               fl(npt)=image(i,j)
              end if
             end if
            end if
           end if
          end do
         end do
        else
         npt=0
         do j=1,m
          do i=1,n
           if (image(i,j)-avim(i,j).ge.threshpix*rmsim(i,j)) then
            if (image(i,j).ne.blankv) then   ! bad hack for now for WN40299HB
             if (rmsim(i,j).ne.blankv) then   ! bad hack for now for WN40299HB
              npt=npt+1
              xpt(npt)=i
              ypt(npt)=j
              fl(npt)=image(i,j)
             end if
            end if
           end if
          end do
         end do
        end if

        if (nm.lt.npt) write (*,*) '  ## ERROR ! xpt, ypt TOO SMALL'
        call sort3(nm,npt,fl,xpt,ypt)

        return
        end

