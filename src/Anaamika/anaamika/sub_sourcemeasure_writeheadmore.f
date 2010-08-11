
        subroutine sub_sourcemeasure_writeheadmore(nsrc,gpi,fn,ffmt,
     /             scratch)
        implicit none
        integer nsrc,line,nchar,nffmt,gpi
        character fn*500,dums*5000,ffmt*500,str1*10,scratch*500
        character cmd*500

        call system('rm -f '//scratch(1:nchar(scratch))//'a')
        open(unit=21,file=scratch(1:nchar(scratch))//'a')
        write (21,*) '" Number_of_gaussians ',nsrc,'"'
        close(21)
        open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
        read (21,*) dums
        close(21)
        dums="sed -e '6a\\"//dums(1:nchar(dums))//"' "//
     /       fn(1:nchar(fn))//" > "//scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('sleep 5')

        cmd='mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn))
        call system(cmd)
        call system('sleep 5')

        call system('rm -f '//scratch(1:nchar(scratch))//'a')
        open(unit=21,file=scratch(1:nchar(scratch))//'a')
        write (21,*) '" Max_gaussians_per_island ',gpi,'"'
        close(21)
        open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
        read (21,*) dums
        close(21)
        dums="sed -e '7a\\"//dums(1:nchar(dums))//"' "//
     /       fn(1:nchar(fn))//" > "//scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn)))

        nffmt=nchar(ffmt)
        call int2str(nffmt,str1)
        str1=' fmt '//str1(1:nchar(str1))
        dums="sed -e '12a\\"//str1(1:nchar(str1))//' "'//
     /       ffmt(1:nchar(ffmt))//'" '//"' "//fn(1:nchar(fn))//" >"//
     /       scratch(1:nchar(scratch))//"a "
        call system(dums)
        call system('mv '//scratch(1:nchar(scratch))//'a '//
     /       fn(1:nchar(fn)))

        return  
        end

