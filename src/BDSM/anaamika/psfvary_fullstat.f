c! 
        subroutine psfvary_fullstat(plotdir,scratch,ncut,bmaj,bmin,bpa,
     /   ebmaj,ebmin,ebpa,peak,bm_pix,cbmaj,cbmin,cbpa,im_rms,s_med_f,
     /   s_av_f,s_av_c_f,s_rms_f,s_rms_c_f,s_med_c_f,fullname,nsig)
        implicit none
        character fullname*500,plotdir*500,scratch*500
        integer ncut
        real*8 bmaj(ncut),bmin(ncut),bpa(ncut),ebmaj(ncut),ebmin(ncut)
        real*8 ebpa(ncut),peak(ncut),bm_pix(3),cbmaj,cbmin,cbpa
        real*8 im_rms(ncut),s_med_f(3),s_av_f(3),nsig
        real*8 s_av_c_f(3),s_rms_f(3),s_rms_c_f(3),s_med_c_f(3)

        call vec_stats(bmaj,ncut,1,ncut,s_med_f(1),s_med_c_f(1),
     /       s_rms_f(1),s_av_f(1),s_rms_c_f(1),s_av_c_f(1),nsig)
        call vec_stats(bmin,ncut,1,ncut,s_med_f(2),s_med_c_f(2),
     /       s_rms_f(2),s_av_f(2),s_rms_c_f(2),s_av_c_f(2),nsig)
        call vec_stats(bpa,ncut,1,ncut,s_med_f(3),s_med_c_f(3),
     /       s_rms_f(3),s_av_f(3),s_rms_c_f(3),s_av_c_f(3),nsig)


        return
        end
