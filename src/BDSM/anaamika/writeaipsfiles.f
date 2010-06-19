c! write star files from gaul and srl and maybe MF as well.
c! CHANGE GAUL FORMAT 
c! CHANGE SRL FORMAT
c! also now write file in degrees for importing as region in ds9 as .reg

        subroutine writeaipsfiles(srldir,fitsname,f2,fitsdir)
        implicit none
        character srldir*500,fitsname*500,f2*500,fitsdir*500
        character gname*500,sname*500,head*100,dumc*500,s*1
        character agname*500,asname*500,code*4,nam*30,regname*500
        integer nchar,dumi,ngau,nsrc,hh,mm,dd,ma,i,flag
        real*8 dumr,ss,sa,bmaj,bmin,bpa,tflux,ra,dec

        write (*,*) '  Writing aips and ds9 files '
        gname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'
        sname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'
        open(unit=21,file=gname(1:nchar(gname)),form='formatted') 
        open(unit=22,file=sname(1:nchar(sname)),form='formatted') 
        call readhead_srclist(21,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        call readhead_srclist(22,17,'Number_of_sources',dumc,nsrc,
     /       dumr,'i')
        close(21)
        close(22)

        nam='NONE'
        gname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'
        agname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.star'
        regname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.reg'
        open(unit=21,file=gname(1:nchar(gname)),form='unformatted') 
        open(unit=22,file=agname(1:nchar(agname)),form='formatted') 
        open(unit=23,file=regname(1:nchar(regname)),form='formatted') 
        do i=1,ngau
         read (21) dumi,dumi,flag,tflux,dumr,dumr,dumr,ra,dumr,dec,
     /    dumr,dumr,dumr,dumr,dumr,bmaj,dumr,bmin,dumr,bpa,dumr
         if (flag.eq.0) then
          call convertra(ra,hh,mm,ss)
          call convertdec(dec,s,dd,ma,sa)
          if (abs(dd).ge.10.d0) then
           write (22,777) hh,mm,ss,s,dd,ma,sa,bmaj,bmin,bpa,4,tflux,
     /                   nam(1:nchar(nam))
          else
           write (22,778) hh,mm,ss,s,0,dd,ma,sa,bmaj,bmin,bpa,4,tflux,
     /                   nam(1:nchar(nam))
          end if
          write (23,779) ra,dec
         end if
        end do
        close(21)
        close(22)
        
        sname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.bin'
        asname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.star'
        open(unit=21,file=sname(1:nchar(sname)),form='unformatted') 
        open(unit=22,file=asname(1:nchar(asname)),form='formatted') 
        do i=1,nsrc
         read (21) dumi,dumi,flag,code,tflux,dumr,dumr,dumr,dumr,
     /    dumr,dumr,
     /    dumr,dumr,dumr,ra,dumr,dec,dumr,bmaj,dumr,bmin,dumr,bpa,dumr
         if (flag.eq.0) then
          call convertra(ra,hh,mm,ss)
          call convertdec(dec,s,dd,ma,sa)
          if (abs(dd).ge.10.d0) then
           write (22,777) hh,mm,ss,s,dd,ma,sa,bmaj,bmin,bpa,4,tflux,
     /                   nam(1:nchar(nam))
          else
           write (22,778) hh,mm,ss,s,0,dd,ma,sa,bmaj,bmin,bpa,4,tflux,
     /                   nam(1:nchar(nam))
          end if
         end if
        end do
        close(21)
        close(22)
777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)
778     format(i2,1x,i2,1x,f6.3,1x,a1,i1,i1,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)
779     format('circle ',f8.4,'d',1x,f8.4,'d')

        return
        end


