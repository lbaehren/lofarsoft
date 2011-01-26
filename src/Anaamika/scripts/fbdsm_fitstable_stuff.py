
import numpy as N

cnames = N.array(['Gaul_id','Island_id','Flag','Total_Flux', \
         'Err_total_flux','Peak_flux','Err_peak_flux','RA','Err_RA', \
         'DEC','Err_DEC','Xpos','Err_xpos','Ypos','Err_ypos','Bmaj_fw', \
         'Err_bmaj','Bmin_fw','Err_bmin','Bpa','Err_bpa', \
         'Deconv_bmaj_fw','Err_decon_bmaj','Deconv_bmin', \
         'Err_decon_bmin','Deconv_bpa','Err_decon_bpa','Src_rms', \
         'Src_av','Isl_rms','Isl_av','Spec_indx','Err_spec_indx','Srcnum','Blc1', \
         'Blc2','Trc1','Trc2','Im_rms'])

cformat = N.array(['1J','1J','1J','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1J','1J','1J','1J','1J','1D'])

cunit = N.array([' ',' ',' ','Jy','Jy','Jy','Jy','deg','deg','deg', \
         'deg','pix','pix','pix','pix','arcsec','arcsec','arcsec', \
         'arcsec','deg','deg','arcsec','arcsec','arcsec','arcsec', \
         'deg','deg','Jy','Jy','Jy','Jy',' ',' ',' ',' ',' ',' ',' ', \
         ' '])
        

