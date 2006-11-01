c! This depends on format of srl
c! CHANGE GAUL FORMAT

        subroutine readgaul(f1,nsrc,fl,ra,dec,bmaj,bmin,bpa,dbmaj,
     /             dbmin,dbpa,sstd,sav,rstd,rav,srldir)
        implicit none
        character f1*500,fn*500,head*100,srldir*500
        integer nsrc,nchar,isrc,dumi,srcnum
        real*8 fl(nsrc),ra(nsrc),dumr,dec(nsrc),rstd(nsrc),rav(nsrc)
        real*8 sstd(nsrc),sav(nsrc)
        real*8 bmaj(nsrc),bmin(nsrc),bpa(nsrc)
        real*8 dbmaj(nsrc),dbmin(nsrc),dbpa(nsrc)

        fn=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.gaul'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
333     read (22,*) head
        if (head.ne.'fmt') goto 333
        do 100 isrc=1,nsrc
         read (22,*) dumi,dumi,dumi,dumr,dumr,fl(isrc),dumr,ra(isrc),
     /    dumr,dec(isrc),dumr,dumr,dumr,dumr,dumr,bmaj(isrc),dumr,
     /    bmin(isrc),dumr,bpa(isrc),dumr,dbmaj(isrc),dumr,dbmin(isrc),
     /    dumr,dbpa(isrc),dumr,sstd(isrc),sav(isrc),rstd(isrc),
     /    rav(isrc),srcnum
100     continue
        close(22)
        
        return
        end

