
        subroutine callavspc(filename,fullname,imagename,runcode,
     /             scratch)
        implicit none
        character filename*500,fullname*500,imagename*500,runcode*2
        character scratch*500,fg*500,extn*20,dir*500,keyword*500
        integer n,m,l
        
        extn='.img'
        call readarraysize(filename,extn,n,m,l)
        call sub_callavspc(filename,fullname,imagename,runcode,
     /             scratch,n,m,l)
        
        return
        end
c!
c!
c!
        subroutine sub_callavspc(filename,fullname,imagename,runcode,
     /             scratch,n,m,l)
        implicit none
        character filename*500,fullname*500,imagename*500,runcode*2
        character scratch*500,fg*500,extn*20,dir*500,keyword*500
        character keystrng*500,comment*500,fn*500
        integer error,n,m,l,nchar,bchan,echan,ch0plane,round,k,dumi
        real*8 ch0plane8,dumr,stdclip(l),avclip(l)

        imagename=fullname(1:nchar(fullname))//'.ch0'
        if (l.eq.1) then
         call sub_callavspc_rw(scratch,filename,imagename,n,m,runcode)
        else
         write (*,*) '  Collapsing channels'
         fg="paradefine"
         extn=""
         dir="./"
         keyword="ch0plane"
         call get_keyword(fg,extn,keyword,keystrng,ch0plane8,
     /     comment,"r",dir,error)
         ch0plane=round(ch0plane8)
        
c! get rms vs channel from .qc_cc file
         fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /      '.qc_cc'
         open(unit=21,file=fn,form='unformatted') 
         read (21)
         do k=1,l
          read (21) dumr,dumi,dumi,dumr,dumi,dumi,dumi,dumr,
     /             dumr,stdclip(k),avclip(k)
         end do
         close(21)

c!
         if (ch0plane.lt.0.or.ch0plane.gt.l) ch0plane=0
         if (ch0plane.eq.0) then
          bchan=1
          echan=l
         else
          bchan=ch0plane
          echan=ch0plane
         end if
         call avspc(scratch,filename,imagename,runcode,n,m,l,
     /        bchan,echan,stdclip)
        end if

        return
        end
c!
c!
c!
        subroutine sub_callavspc_rw(scratch,filename,imagename,n,m,
     /             runcode)
        implicit none
        integer n,m,i,j
        character scratch*500,filename*500,imagename*500,extn*20
        character runcode*2
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,filename,extn)
        do j=1,m
         do i=1,n
          if (image(i,j).ne.image(i,j)) image(i,j)=-999.d0
         end do
        end do
        call writearray_bin2D(image,n,m,n,m,imagename,runcode)

        return
        end


