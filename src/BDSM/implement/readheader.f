c! read header info

        subroutine readheader(f1,scratch)
        implicit none
        character f1*(*),scop*7,scratch*500
        integer nchar,n,m
        real*8 freq,bw,intsec,cdelt

        f1=scratch(1:nchar(scratch))//f1
        open(unit=21,file=f1(1:nchar(f1)),status='old',
     /       form='unformatted')
        read (21) n,m,scop,freq,bw,intsec,cdelt
        close(21)

        write (*,*) '  Image size    : ',n,' X ',m
        write (*,*) '  Telescope     :  ',scop
        write (*,*) '  Frequency     : ',freq/1.d6,' MHz'
        write (*,*) '  Bandwidth     : ',bw/1.d3,' kHz'
        write (*,*) '  Integ time    : ',intsec/3600.d0,' hrs'
        write (*,*) '  Pixel scale   : ',cdelt,' arcsec'
        
        return
        end


