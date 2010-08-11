c! This depends on format of srl
c! CHANGE GAUL FORMAT
c! CHANGE SRL FORMAT
c! 
c! problem - for s2g like 3C, the star file has all the sources, whereas the star file
c! for the bdsm o/p has only those which have flag=0.
c! for now, test if s2g is in the filename and if not, do accordingly. stupid.
c! Modified now so that s2g star has only flag=0 sources.

        subroutine readgaul(f1,x,nsrc,srldir,gaulid,islid,flag,tot,dtot,
     /    peak,epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /    bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /    sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,
     /    trc1,trc2,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6)
        implicit none
        character f1*500,fn*500,head*100,srldir*500,dumc*500
        integer nsrc,nchar,isrc,x,dumi,flag(x),srcid(x),nsrl,i
        integer gaulid(x),islid(x),blc1(x),blc2(x),trc1(x),trc2(x)
        real*8 ra(x),dumr,dec(x),rstd(x),rav(x)
        real*8 sstd(x),sav(x),tot(x),dtot(x)
        real*8 bmaj(x),bmin(x),bpa(x)
        real*8 xpix(x),expix(x),ypix(x),eypix(x)
        real*8 era(x),edec(x),totfl(x),etotfl(x),peak(x)
        real*8 epeak(x),ebmaj(x),ebmin(x),ebpa(x)
        real*8 dbmaj(x),edbmaj(x),dbmin(x),edbmin(x)
        real*8 dbpa(x),edbpa(x),chisq(x),q(x)
        real*8 dumr1(x),dumr2(x),dumr3(x),dumr4(x),dumr5(x),dumr6(x)
        character code4(x)*4,dmc
        character*30 names(x)
        character*4 code(x)
        character filen*500,extn*20
        logical existsq,s2g

cf2py   intent(in) f1,srldir,nsrc,x
cf2py   intent(out) gaulid,islid,flag,tot,dtot,peak,epeak,ra,era,dec,edec
cf2py   intent(out) xpix,expix,ypix,eypix,bmaj,ebmaj,bmin,ebmin,bpa,ebpa
cf2py   intent(out) dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,sstd,sav,rstd,rav
cf2py   intent(out) code,names,blc1,blc2,trc1,trc2,dumr1,chisq,dumr2,dumr3
cf2py   intent(out) dumr4,dumr5,dumr6,q

        fn=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.gaul.bin'   
        open(unit=22,file=fn(1:nchar(fn)),form='unformatted') 
        do 100 isrc=1,nsrc
         read (22) gaulid(isrc),islid(isrc),flag(isrc),tot(isrc),
     /    dtot(isrc),peak(isrc),epeak(isrc),ra(isrc),era(isrc),dec(isrc)
     /    ,edec(isrc),xpix(isrc),expix(isrc),ypix(isrc),eypix(isrc),
     /    bmaj(isrc),ebmaj(isrc),bmin(isrc),ebmin(isrc),bpa(isrc),
     /    ebpa(isrc),dbmaj(isrc),edbmaj(isrc),dbmin(isrc),edbmin(isrc),
     /    dbpa(isrc),edbpa(isrc),sstd(isrc),sav(isrc),rstd(isrc),
     /    rav(isrc),chisq(isrc),q(isrc),srcid(isrc),blc1(isrc),
     /    blc2(isrc),trc1(isrc),trc2(isrc),dumr1(isrc),dumr2(isrc),
     /    dumr3(isrc),dumr4(isrc),dumr5(isrc),dumr6(isrc)
100     continue
        close(22)

c! does f1 has s2g in it ? then all sources r there in star file.
c        s2g=.false.
c        do i=1,nchar(f1)-2
c         if (f1(i:i+2).eq.'s2g') s2g=.true.
c        end do
        s2g=.false.
        
c! now read star file to get names
        fn=""
        extn='.star'
        if (existsq(f1,srldir,extn)) then
         fn=srldir(1:nchar(srldir))//f1(1:nchar(f1))//
     /      extn(1:nchar(extn))
        else
         extn='.gaul.star'
         if (existsq(f1,srldir,extn)) then
          fn=srldir(1:nchar(srldir))//f1(1:nchar(f1))//
     /       extn(1:nchar(extn))
         else
          do isrc=1,nsrc 
           names(i)=' '
          end do
         end if
        end if
        if (fn.ne."") then
         open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
         if (s2g) then
          do isrc=1,nsrc
           read (22,777) dumi,dumi,dumr,dmc,dumi,dumi,dumr,dumr,
     /           dumr,dumr,dumi,dumr,names(isrc)
          end do
         else
          do isrc=1,nsrc
           if (flag(isrc).ne.0) then
            names(isrc)=' '
           else
            read (22,777) dumi,dumi,dumr,dmc,dumi,dumi,dumr,dumr,
     /           dumr,dumr,dumi,dumr,names(isrc)
           end if
          end do
         end if
         close(22)
        end if
777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30) 
        
c! now read srl to get the sourceid code for each gaussian.

        fn=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.srl'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,17,'Number_of_sources',dumc,nsrl,
     /       dumr,'i')
        close(22)
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
334     read (22,*) head
        if (head.ne.'fmt') goto 334
        do 110 isrc=1,nsrl
         read (22,*) i,dumi,dumi,code4(isrc)
         if (i.ne.isrc) write (*,*) ' RONG '
110     continue
        close(22)
        do 120 i=1,nsrc
         if (srcid(i).ne.0) code(i)=code4(srcid(i))
         if (srcid(i).eq.0) code(i)=' '
120     continue

        return
        end

