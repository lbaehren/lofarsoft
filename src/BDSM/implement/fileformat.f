c! convert ascii or binary or fits to each other. rt now works for b2a, a2b, b2f, a2f.
c! shud check if input format is correct at some point. for now, assume i am intelligent.

        subroutine fileformat(f1,scratch,fitsdir)
        implicit none
        character f1*(*),fn*500,cmd*500,str*20,str3*3
        character fform*20,scratch*500,fitsdir*500
        integer nchar,n,m

        fn=scratch(1:nchar(scratch))//f1
        call system('rm -f a')
        cmd="file "//fn(1:nchar(fn))//" | awk '{print $2}' > a"
        call system(cmd)
        open(unit=21,file='a',status='old')
        read (21,*) str
        close(21)
        if (str.ne.'data'.and.str.ne.'ASCII'.and.str.ne.'FITS'.
     /      and.str.ne.'GLS_BINARY_LSB_FIRST') then   			! hack for what i dont know
         write (*,*) '   Illegal file (legal=ascii,binary,fits)'
         goto 334
        end if
        if (str.eq.'data') str='binary'
        if (str.eq.'GLS_BINARY_LSB_FIRST') str='binary'
        if (str.eq.'ASCII') str='ascii'
        if (str.eq.'FITS') str='fits'
333     write (*,*) '  File is ',str(1:nchar(str))
        write (*,'(a34,$)') '   Conversion (b2a a2b b2f a2f) : '
        read (*,*) str3
        if (str3(1:1).ne.str(1:1)) goto 333

        if (str.eq.'binary') fform='unformatted'
        if (str.eq.'ascii') fform='formatted'

        open(unit=21,file=fn(1:nchar(fn)),status='old',
     /       form=fform)
        if (str.eq.'binary') read (21) n,m
        if (str.eq.'ascii') read (21,*) n,m
        call sub_fileformat(f1,21,n,m,str,str3,scratch,fitsdir)
        close(21)

334     continue
        return
        end


        subroutine sub_fileformat(f1,nn,n,m,str,str3,scratch,
     /             fitsdir)
        implicit none
        character f1*(*),fn*500,lab*500,str*20,str3*3,scratch*500
        character opform*20,f2*500,fitsdir*500,fitsname*500
        integer n,m,nn,nchar,i,j
        real*8 image(n,m),dumr(m)

        if (str.eq.'binary') 
     /   call readarray_bin_data(nn,n,m,image,n,m)
        if (str.eq.'ascii') 
     /   call readarray_asc(n,m,image,n,m,nn)
        
        if (str3(3:3).eq.'b') opform='binary'
        if (str3(3:3).eq.'a') opform='ascii'
        if (str3(3:3).eq.'f') opform='fits'
        f2=f1(1:nchar(f1))//'.'//opform(1:3)
        if (str3(3:3).eq.'f') f2=f2(1:nchar(f2))//'s'

        if (opform.eq.'ascii') 
     /   call writearray_asc(image,n,m,n,m,f1,'mv')

        if (opform.eq.'binary') 
     /   call writearray_bin(image,n,m,n,m,f1,'mv')
        
        if (opform.eq.'fits') then
         fitsname=' '
         call writefits(image,n,m,n,m,f2,fitsdir,
     /        scratch,fitsname)
        end if
        
        return
        end


