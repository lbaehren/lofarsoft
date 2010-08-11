c! write star files from gaul and srl and maybe MF as well.
c! CHANGE GAUL FORMAT 
c! CHANGE SRL FORMAT
c! also now write file in degrees for importing as region in ds9 as .reg

        implicit none
        integer hh, mm, dd, ma
        real*8 ss, sa, tot, bmaj,bmin,bpa

        open(unit=21, file='a1')
        open(unit=22, file='a2')
        open(unit=23, file='a3')
        do i=1,2226
         read (21,*) hh, mm, ss, dd, ma, sa
         read (22,*) tot, bmaj, bmin, bpa
         write (23,777) hh,mm,ss,'+',dd,ma,sa,bmaj,bmin,bpa,4,tot,
     /                   'NONE'
        end do

        close(21)
        close(22)
        close(23)

777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)

           
        end


