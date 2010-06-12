c! read keyword from a parameter file
        
        subroutine get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch)
        implicit none
        integer error,num,nchar,i
        real*8 keyvalue
        character f1*500,extn*10,keyword*500,fn*500,dumh*500,f2*500
        character keystrng*500,comment*500,code*1,scratch*500,equal

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        f2=f1(1:nchar(f1))//extn(1:nchar(extn))

        call getline(f2,scratch,num)
        error=1
        open(unit=21,file=fn,status='old')
        do i=1,num
         read (21,*) dumh
         if (dumh(1:nchar(dumh)).eq.keyword(1:nchar(keyword))) then
          backspace(21)
          if (code.eq.'s'.or.code.eq.'S') 
     /       read (21,*) dumh,equal,keystrng,comment
          if (code.eq.'r'.or.code.eq.'R') 
     /       read (21,*) dumh,equal,keyvalue,comment
          error=0
         end if
        end do
        close(21)
        if (error.eq.1) write (*,*) 'Keyword ',keyword(1:nchar(keyword))
     /     ,' not found in ',f1(1:nchar(f1))//extn(1:nchar(extn))

        return
        end
