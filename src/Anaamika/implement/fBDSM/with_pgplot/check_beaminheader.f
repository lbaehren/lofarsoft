c!

        subroutine check_beaminheader(scratch,f2)
        implicit none
        character scratch*500,f2*500,extn*20,keyword*500,f1*500,dir*500
        character keystrng*500,comment*500
        integer error1,error2,error3,error
        real*8 bmaj,bmin,bpa

        extn='.header'
        keyword='BMAJ'
        call check_keyword(f2,extn,keyword,scratch,error1)
        keyword='BMIN'
        call check_keyword(f2,extn,keyword,scratch,error2)
        keyword='BPA'
        call check_keyword(f2,extn,keyword,scratch,error3)

        if (error1+error2+error3.ne.0) then
         f1='paradefine'
         dir='./'
         extn=''
         keyword='BMAJ'
         call get_keyword(f1,extn,keyword,keystrng,bmaj,
     /    comment,"r",dir,error)
         keyword='BMIN'
         call get_keyword(f1,extn,keyword,keystrng,bmin,
     /    comment,"r",dir,error)
         keyword='BPA'
         call get_keyword(f1,extn,keyword,keystrng,bpa,
     /    comment,"r",dir,error)
         if (bmaj.eq.0.d0.or.bmin.eq.0.d0) then
          write (*,*) '  !!! Write the beam in paradefine !!!'
c          stop
         else
          extn='.header'
          keyword='BMAJ'
          call put_keyword(f2,extn,keyword,keyword,bmaj,
     /         comment,'r',scratch)
          keyword='BMIN'
          call put_keyword(f2,extn,keyword,keyword,bmin,
     /         comment,'r',scratch)
          keyword='BPA'
          call put_keyword(f2,extn,keyword,keyword,bpa,
     /         comment,'r',scratch)
         end if
        end if

        return
        end



