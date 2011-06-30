c! for dummy gaussian list with just a.

        subroutine fillin_gaulist(max_msrc,max_msrc6,rstd,rav,sstd,sav,
     /     chisq,q,deconv_s,error,flag,flag_s,e_amp,e_x0,e_y0,e_maj,
     /     e_min,e_pa,e_d1,e_d2,e_d3,e_tot,dumr1,dumr2,dumr3,dumr4,
     /     dumr5,dumr6)
        implicit none
        integer max_msrc,max_msrc6,error,flag(max_msrc),i
        real*8 sstd(max_msrc),sav(max_msrc),rstd,rav,chisq,q
        real*8 deconv_s(3,max_msrc),e_amp(max_msrc),e_x0(max_msrc)
        real*8 e_y0(max_msrc),e_maj(max_msrc),e_min(max_msrc)
        real*8 e_pa(max_msrc),e_d1(max_msrc),e_d2(max_msrc)
        real*8 e_d3(max_msrc),e_tot(max_msrc)
        real*8 dumr1(max_msrc),dumr2(max_msrc),dumr3(max_msrc)
        real*8 dumr4(max_msrc),dumr5(max_msrc),dumr6(max_msrc)
        character flag_s(max_msrc)

        do i=1,max_msrc
         flag(i)=0
         sstd(i)=0.d0
         sav(i)=0.d0
         rstd=0.d0
         rav=0.d0
         chisq=0.d0
         q=0.d0
         deconv_s(1,i)=0.d0
         deconv_s(2,i)=0.d0
         deconv_s(3,i)=0.d0
         e_amp(i)=0.d0
         e_x0(i)=0.d0
         e_y0(i)=0.d0
         e_maj(i)=0.d0
         e_min(i)=0.d0
         e_pa(i)=0.d0
         e_d1(i)=0.d0
         e_d2(i)=0.d0
         e_d3(i)=0.d0
         e_tot(i)=0.d0
         flag_s(i)=' '
         dumr1(i)=0.d0
         dumr2(i)=0.d0
         dumr3(i)=0.d0
         dumr4(i)=0.d0
         dumr5(i)=0.d0
         dumr6(i)=0.d0
        end do

        return
        end


