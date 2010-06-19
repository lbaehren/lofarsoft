c! associate lists in qc

        subroutine call_qc_associatelists(scratch,srldir,runcode,
     /        filename,bdsmsolnname,qcsolnname)
        implicit none
        character fg*500,extn*20,dir*500,keyword*500,keystrng*500
        character comment*500,scratch*500,srldir*500,runcode*2
        character filename*500,bdsmsolnname*500,qcsolnname*500
        character master*500,slave*500,newname*500,mas_sl*1
        character sec*500,orig*500,new*500,rcode*2
        integer error,i,nchar,num_asrl
        real*8 dumr,radius

        rcode='q '
        write (*,*) '  Associating source lists'
        fg="qcparadefine"
        extn=""
        dir="./"
        keyword="num_asrl"
        call get_keyword(fg,extn,keyword,keystrng,dumr,
     /    comment,'r',dir,error)
        num_asrl=int(dumr)

        orig=filename(1:nchar(filename))//'.'//
     /       bdsmsolnname(1:nchar(bdsmsolnname))
        open(unit=27,file=scratch(1:nchar(scratch))//'qc_asrl_list')
        open(unit=28,file=scratch(1:nchar(scratch))//'qc_pasl_list')
        do i=1,num_asrl
         read (27,*) newname,radius,mas_sl
         new=newname(1:nchar(newname))
         if (mas_sl.eq.'m') then
          master=new
          sec=orig
         else
          master=orig
          sec=new
         end if
         write (28,*) master(1:nchar(master))//'__'//sec(1:nchar(sec))
         call associatesrl(master,sec,scratch,srldir,rcode,radius)
        end do
        close(27)
        close(28)

        return
        end


