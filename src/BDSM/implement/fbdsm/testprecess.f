c! to test precess prog of aoki's paper

        implicit none
        integer hh,mm,dd,ma,s
        real*8 ss,sa,ra,dec,raj,decj
        character sc*1
        
        write (*,*) ' Enter hh mm ss.s s dd ma sa.s '
        read (*,*) hh,mm,ss,s,dd,ma,sa

        ra=(hh+mm/60.d0+ss/3600.d0)*15.d0
        dec=s*(dd+ma/60.d0+sa/3600.d0)

        call Aoki_B1950toJ2000(ra,dec,raj,decj)
        call convertra(raj,hh,mm,ss)
        call convertdec(decj,sc,dd,ma,sa)

        write (*,*) hh,mm,ss,sc,dd,ma,sa
        write (*,*) 

        end

