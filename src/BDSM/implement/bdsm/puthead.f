c! puts header info in

        subroutine puthead(extn,f1,scratch)
        implicit none
        character scope*7,f1*500,scratch*500,extn*20
        real*8 freq,bw,intsec,cdelt
        integer nchar,n,m,l

cf2py   intent(in) f1,scratch,extn

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

        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_puthead(f1,scratch,extn,n,m,scope,freq,bw,intsec,cdelt)

        return
        end
c!
c!
c!
        subroutine sub_puthead(f1,scratch,extn,n,m,scope,freq,bw,
     /             intsec,cdelt)
        implicit none
        character scope*7,f1*500,scratch*500,extn*20
        integer nchar,n,m,i,j
        real*8 freq,bw,intsec,cdelt,image(n,m),dumr(m)

        call readarray_bin(n,m,image,n,m,f1,extn)
        call system('rm -fr '//scratch(1:nchar(scratch))//
     /       f1(1:nchar(f1))//extn(1:nchar(extn)))
        open(unit=22,file=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /       extn(1:nchar(extn)),form='unformatted')
        write (22) n,m,scope,freq,bw,intsec,cdelt
        do 200 i=1,n
         do 210 j=1,m
          dumr(j)=image(i,j)
210      continue
         write (22) dumr
200     continue
        close(22)

        return
        end


