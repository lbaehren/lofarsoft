c! write array into text file.
c! modified to write to a binary file (is half the size for real*8 but is much much faster). 
c! fits file is same size as for real*4 which is 1/4 size of ascii file.

c! .img is binary real*8 image
c! .asc is ascii real*8 image
c! .nmg is binary integer image
c! .nsc is ascii integer image

        subroutine writearray_bin(arr,x,y,n,m,filen,runcode)
        implicit none
        integer n,m,i,j,x,y,nchar
        real*8 arr(x,y),dumr(m),keyvalue
        character filen*500,lab*500,f1*500,scratch*500,runcode*2
        character fg*500,extn*10,keyword*500,comment*500,dir*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        call check(n,m,14)
        f1=scratch(1:nchar(scratch))//filen(1:nchar(filen))//'.img'
        lab='rm -f '//f1
        call system(lab)
        if (runcode(2:2).eq.'v')
     /      write (*,*) '  Writing out ',f1(1:nchar(f1))
        open(unit=22,file=f1(1:nchar(f1)),status='unknown'
     /       ,form='unformatted')
        write (22) n,m
        do 200 i=1,n
         do 210 j=1,m
          dumr(j)=arr(i,j)
210      continue
         write (22) dumr
200     continue
        close(22)

        return
        end

        subroutine writearray_bin_int(arr,x,y,n,m,filen,str,runcode)
        implicit none
        integer n,m,i,j,x,y,nchar
        integer arr(x,y),dumr(m)
        character filen*(*),lab*500,f1*500,str*10,runcode*2
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        character scratch*500
        real*8 keyvalue

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        call check(n,m,14)
        f1=scratch(1:nchar(scratch))//filen(1:nchar(filen))//'.nmg'
        lab='rm -f '//f1
        call system(lab)
        if (runcode(2:2).eq.'v')
     /      write (*,*) '  Writing out ',f1(1:nchar(f1))
        open(unit=22,file=f1(1:nchar(f1)),status='unknown'
     /       ,form='unformatted')
        write (22) n,m,str
        do 200 i=1,n
         do 210 j=1,m
          dumr(j)=arr(i,j)
210      continue
         write (22) dumr
200     continue
        close(22)

        return
        end

        subroutine writearray_asc_int(arr,x,y,n,m,filen,str,runcode)
        implicit none
        integer n,m,i,j,x,y,nchar
        integer arr(x,y)
        character filen*(*),lab*500,f1*500,str*10,scratch*500,runcode*2
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        real*8 keyvalue

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        call check(n,m,14)
        f1=scratch(1:nchar(scratch))//filen(1:nchar(filen))//'.nsc'
        lab='rm -f '//f1
        call system(lab)
        if (runcode(2:2).eq.'v')
     /      write (*,*) '  Writing out ',f1(1:nchar(f1))
        open(unit=22,file=f1(1:nchar(f1)),status='unknown')
        write (22,*) n,m,str
        do 200 i=1,n
         do 210 j=1,m
          if (j.ne.m) write (22,'(i4,a1,$)') arr(i,j),' '
          if (j.eq.m) write (22,'(i4)') arr(i,j)
210      continue
200     continue
        close(22)

        return
        end

        subroutine writearray_asc(arr,x,y,n,m,filen,runcode)
        implicit none
        integer n,m,i,j,x,y,nchar
        real*8 arr(x,y)
        character filen*(*),lab*500,f1*100,scratch*500,runcode*2
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        real*8 keyvalue

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        call check(n,m,14)
        f1=scratch(1:nchar(scratch))//filen(1:nchar(filen))//'.asc'
        lab='rm -f '//f1
        call system(lab)
        if (runcode(2:2).eq.'v')
     /      write (*,*) '  Writing out ',f1(1:nchar(f1))
        open(unit=22,file=f1(1:nchar(f1)),status='unknown')
        write (22,*) n,m
        do 200 i=1,n
         do 210 j=1,m
          if (j.ne.m) write (22,'(1pe14.6,a1,$)') arr(i,j),' '
          if (j.eq.m) write (22,'(1pe14.6)') arr(i,j)
210      continue
200     continue
        close(22)

        return
        end

        subroutine writearray_bin3D(arr,x,y,z,filen,runcode)
        implicit none
        integer x,y,z,i,j,k,nchar
        real*8 arr(x,y,z),dumr(y)
        character filen*(*),lab*500,f1*500,scratch*500,runcode*2
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        real*8 keyvalue

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        f1=scratch(1:nchar(scratch))//filen(1:nchar(filen))//'.img'
        lab='rm -f '//f1
        call system(lab)
        if (runcode(2:2).eq.'v')
     /      write (*,*) '  Writing out ',f1(1:nchar(f1))
        open(unit=26,file=f1(1:nchar(f1)),form='unformatted')
        write (26) x,y,z
        do 300 k=1,z
         do 200 i=1,x
          do 210 j=1,y
           dumr(j)=arr(i,j,k)
210       continue
          write (26) dumr
200      continue
300     continue
        close(26)

        return
        end

