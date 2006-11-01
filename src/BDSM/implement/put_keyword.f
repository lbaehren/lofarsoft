c! put keyword in parameter file

        subroutine put_keyword(f1,extn,keyword,keystring,keyvalue,
     /             comment,code,dir)
        implicit none
        integer nchar
        real*8 keyvalue
        character f1*500,extn*10,keyword*500,keystring*500,code*1
        character comment*500,dir*500,fn*500

        fn=dir(1:nchar(dir))//f1(1:nchar(f1))//extn(1:nchar(extn))
        open(unit=21,file=fn,access='append')
        if (code.eq.'S'.or.code.eq.'s') write (21,*) keyword(1:nchar(
     /      keyword)),' = ',keystring(1:nchar(keystring)),' ',
     /      comment(1:nchar(comment))
        if (code.eq.'R'.or.code.eq.'r') write (21,*) keyword(1:nchar(
     /      keyword)),' = ',keyvalue,' ',comment(1:nchar(comment))
        close(21)

        return
        end

