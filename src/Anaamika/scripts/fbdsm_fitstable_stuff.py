
import numpy as N

cnames = N.array(['gaul_id','island_id','flag','Total_Flux', \
         'Err_total_flux','Peak_flux','Err_peak_flux','RA','Err_RA', \
         'DEC','Err_DEC','Xpos','Err_xpos','Ypos','Err_ypos','Bmaj_fw', \
         'Err_bmaj','Bmin_fw','Err_bmin','Bpa','Err_bpa', \
         'Deconv_bmaj_fw','Err_decon_bmaj','Deconv_bmin', \
         'Err_decon_bmin','Deconv_bpa','Err_decon_bpa','src_rms', \
         'src_av','isl_rms','isl_av','sp_in','e_sp_in','srcnum','blc1', \
         'blc2','trc1','trc2','im_rms','dummy2','dummy3','dummy4', \
         'dummy5','dummy6'])

cformat = N.array(['1J','1J','1J','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D',\
         '1D','1D','1D','1J','1J','1J','1J','1J','1D','1D','1D','1D',\
         '1D','1D'])

cunit = N.array([' ',' ',' ','Jy','Jy','Jy','Jy','deg','deg','deg', \
         'deg','pix','pix','pix','pix','arcsec','arcsec','arcsec', \
         'arcsec','deg','deg','arcsec','arcsec','arcsec','arcsec', \
         'deg','deg','Jy','Jy','Jy','Jy',' ',' ',' ',' ',' ',' ',' ', \
         ' ',' ',' ',' ',' ',' '])
        

