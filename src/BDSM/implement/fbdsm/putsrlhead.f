
        subroutine putsrlhead(nn,nisl,filename,dumin,n,m)
        implicit none
        integer nn,nisl,nchar,dumin,n,m
        character filename*500

        write (nn,'(a,a)') ' Gaussian list made by noise/callcs'
        write (nn,'(a,a)') ' Image_name ',filename(1:nchar(filename))
        write (nn,*) 'Image_size_x ',n
        write (nn,*) 'Image_size_y ',m
        write (nn,'(a,a)') ' Island_list_name ',
     /        filename(1:nchar(filename))//'.ini'
        write (nn,*) 'Number_of_islands ',nisl
        if (dumin.eq.1) write (nn,*) 'Number_of_sources ',nisl
        write (nn,*) 'Number_of_gaussians ',nisl
        write (nn,*) 'Max_gaussians_per_island 1'
        write (nn,'(a,a)') ' RMS_map map'
        write (nn,*) 'Sigma 1.d0'
        write (nn,*) 'Detect_threshold 5'
        write (nn,*) 'srl# island# flag tot_Jy err peak_Jy err   RA '//
     /    'err DEC err  xpos_pix err '//
     /    'ypos_pix err bmaj_asec_fw err bmin_asec_fw err '//
     /    'bpa_deg err deconv_bmaj_bmin_bpa_asec_fw &errors '//
     /    'src_rms src_av isl_rms isl_av spin e_spin src#'
        write (nn,*) 'fmt 90 "(2(i7,1x),a4,1x,4(1Pe11.3,1x),'//
     /      '4(0Pf13.9,1x),'//
     /      '16(0Pf10.5,1x),4(1Pe11.3,1x),0Pf7.3,1x,0Pf7.3,1x,i5)"'


        return
        end

