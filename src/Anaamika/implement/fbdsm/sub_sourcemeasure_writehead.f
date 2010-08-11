c! CHANGE GAUL FORMAT

        subroutine sub_sourcemeasure_writehead(nn,f1,f2,n,m,nisl,
     /             std,prog,rmsmap,thresh_pix)
        implicit none
        integer nn,n,m,nisl,nchar,error
        real*8 std,keyvalue,thresh_pix
        character f1*500,f2*(*),prog*20,dumc*500
        character rmsmap*500,dir*500

        write (nn,'(a,a)') ' Gaussian list made by '//
     /                     prog(1:nchar(prog))
        write (nn,'(a,a)') ' Image_name ',f1(1:nchar(f1))
        write (nn,*) 'Image_size_x ',n
        write (nn,*) 'Image_size_y ',m
        write (nn,'(a,a)') ' Island_list_name ',f2(1:nchar(f2))
        write (nn,*) 'Number_of_islands ',nisl
        write (nn,'(a,a)') ' RMS_map ',rmsmap(1:nchar(rmsmap))
        write (nn,*) 'Sigma ',std
        write (nn,*) 'Detect_threshold ',thresh_pix
        write (nn,'(a)') ' gaul# island# flag tot_Jy err peak_Jy err '//
     /    '  RA err DEC err  xpos_pix err '//
     /    'ypos_pix err bmaj_asec_fw err bmin_asec_fw err '//
     /    'bpa_deg err deconv_bmaj_bmin_bpa_asec_fw &errors '//
     /    'src_rms src_av isl_rms isl_av spin e_spin src#  '//
     /    'blc1  blc2  trc1 trc2  dumr1 dumr2 dumr3 dumr4 dumr5 dumr6'

        return  
        end

