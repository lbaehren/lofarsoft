% Program to compute pupil autocorrelation
%  F. Rigal Astron 2003




N_phase=128;
% length_D=2*N_phase;

%  check whether the size of the files in which the mirror modes are saved is
%  a 128*128 array or a 256*256 array ( 128*128 centered in a zero value array 256*256) 
%  if the mirror modes are saved in 256*256 arrays it is assumed that the valid 
%      information are centered (128*128 arrays) and  surrounded by 0 values  


    
	if N_phase~= 128 
        if N_phase~=256 
            error(' Mirror modes array must be 128*128 or 256*256')
        end
	end
	
	
	if N_phase==256 factor=1;
	else factor=2;
	end
	
	if N_phase == 256
        the_vlt_pupil = vlt_pupil( 128, 1, 0 );
        pupil_temp = get( the_vlt_pupil, 'mask' );
        pupil=expanded_zeros(pupil_temp,factor);
        else   the_vlt_pupil = vlt_pupil( N_phase, 1, 0 );
               pupil = get( the_vlt_pupil, 'mask' );
	end
	
	
	
	corre_pupil=correlation(pupil,pupil,factor);

    % To avoid rounding error during the division by the pupil correlation
    coords=(corre_pupil<1e-11);
    corre_pupil(coords)=1;

    
