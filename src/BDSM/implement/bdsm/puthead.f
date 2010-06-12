c! puts header info in

        subroutine puthead(f1,scratch)
        implicit none
        character scope*7,head*500,f1*(*),scratch*500
        real*8 freq,bw,intsec,cdelt
        integer nchar,n,m

        write (*,*) 

333     continue
        write (*,'(a44,$)') '  Scope (LOFAR LOFARVC GMRT WSRT VLA) : '
        read (*,*) scope
        if (scope.ne.'LOFAR'.and.scope.ne.'LOFARVC'.and.scope.ne.'GMRT'
     /           .and.scope.ne.'WSRT'.and.scope.ne.'VLA') goto 333

        write (*,'(a20,$)') '  Frequency (MHz) : '
        read (*,*) freq
        freq=freq*1.d6

        write (*,'(a20,$)') '  Bandwidth (MHz) : '
        read (*,*) bw
        bw=bw*1.d6

        write (*,'(a27,$)') '  Integration time (hrs) : '
        read (*,*) intsec
        intsec=intsec*3600.d0

        write (*,'(a25,$)') '  Pixel scale (arcsec) : '
        read (*,*) cdelt

        f1=scratch(1:nchar(scratch))//f1(1:nchar(f1))
        f1=f1(1:nchar(f1))
        open(unit=21,file=f1,status='old',
     /       form='unformatted')
        read (21) n,m
        close(21)
        write (head,777) n,m,scope,freq,bw,intsec,cdelt
777     format(i4,1x,i4,1x,a7,1x,1Pe12.5,1x,1Pe12.5,1x,
     /         1Pe12.5,1x,0Pf7.3)

        head="sed -e '1c\\"//head(1:nchar(head))//"' "//
     /       f1//" > a "
        call system(head)
        head="mv -f a "//f1
        call system(head)

        return
        end


