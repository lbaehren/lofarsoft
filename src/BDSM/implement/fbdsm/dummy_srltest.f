c! to write corrtect srl file for simul1, 2, 3 so as to not run it again.

        implicit none
        character f1*500,f2*500,f3*500,ffmt*500,srldir*500,head*1000
        integer nchar,n,m,nisl,nsrc,gpi,nffmt,headint

        write (*,*) ' Enter filename'
        read (*,*) f1
        f2=f1(1:nchar(f1))//'.ini'
        srldir='/data/niruj_pers/image/'
        call sourcelistheaders(f2,f3,n,m,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)
        
        call sub_dummy(f1,f2,srldir,n,m,nisl,nsrc)

        end

        subroutine sub_dummy(f1,f2,srldir,n,m,nisl,nsrc)
        implicit none
        character f1*500,f2*500,f3*500,ffmt*500,srldir*500,head*1000
        integer nchar,n,m,nisl,nsrc,gpi,nffmt,headint
        real*8 

        open(unit=21,file=srldir(1:nchar(srldir))
     /       //f2(1:nchar(f2))//'.gaul.bin'
        close(21)



        open(unit=21,file=srldir(1:nchar(srldir))
     /       //f2(1:nchar(f2))//'.srl'
        open(unit=22,file=srldir(1:nchar(srldir))
     /       //f2(1:nchar(f2))//'.csrl'
335     read (21,'(a)') head
        write (22,'(a)') head(1:nchar(head))
        if (head(2:4).ne.'fmt') goto 335
        backspace(21)

        close(21)
        close(22)


        return
        end
