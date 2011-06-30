c! read keyword from a parameter file
        
        subroutine get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch,error)
        implicit none
        integer error,ierr,nchar,i
        real*8 keyvalue
        character f1*500,extn*20,keyword*500,fn*500,dumh*500,f2*500
        character keystrng*500,comment*500,code*1,scratch*500,equal
        logical exists

Cf2py  intent(in) f1,extn,keyword,code,scratch
Cf2py  intent(out) keystrng, keyvalue, comment, error

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        f2=f1(1:nchar(f1))//extn(1:nchar(extn))
        inquire(file=fn,exist=exists)

        if (exists) then
         error=1
         open(unit=21,file=fn,status='old')

100      continue
         read (21,*,iostat=ierr) dumh
         if (ierr.lt.0) then
          error=1
          goto 335
         end if
         if (dumh(1:nchar(dumh)).eq.keyword(1:nchar(keyword))) then
          backspace(21)
          if (code.eq.'s'.or.code.eq.'S') 
     /       read (21,*) dumh,equal,keystrng,comment
          if (code.eq.'r'.or.code.eq.'R') 
     /       read (21,*) dumh,equal,keyvalue,comment
          error=0
          goto 335
         end if
         goto 100
 
335      continue
         close(21)
         if (error.eq.1.and.keyword(1:nchar(keyword)).ne.'BLANK') 
     /      write (*,*) 'Keyword ',keyword(1:nchar(keyword))
     /      ,' not found in ',f1(1:nchar(f1))//extn(1:nchar(extn))
        else
         write (*,*) 'File ',f2(1:nchar(f2)),' does not exist'
        end if

 
        return
        end
