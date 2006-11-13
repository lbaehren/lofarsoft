%
% Main PSF Reconstruction program
%  Florence Rigal Astron 2004


% N_used_modes : Number of modes (from 4 up tp N_used_modes) to be used to compute D/r0 ratio in the loop. The
% number should correspond to a good fitting of the data var_ai computed
% and diag(Caa_simul(2:60,2:60) extracted from simulation
%  in the future an automatic selection of the modes should be developed.

samples_pupil=128  


N_used_modes= 45;

% load data
%  Could be interactive in the future...
% correcting wavelenth required.


disp(' Load data')

 load target3_1.dat

 correcting_wavelength = 2200e-9;

 g=1.00;  % Gain for each subaperture (constant)

timestamp=[1];
infocus=timestamp(end)+[1:60];
outfocus=infocus(end)+[1:60];
curvature=outfocus(end)+[1:60];
CWSZernike=curvature(end)+[1:60];
DMZernike=CWSZernike(end)+[1:60];
deltavoltage=DMZernike(end)+[1:60];
TTMtiptilt=deltavoltage(end)+[1:2];
LGSTiptilt=TTMtiptilt(end)+[1:2];
EstPiston=LGSTiptilt(end)+[1];
cwstip=EstPiston(end)+1;
cwstilt=cwstip(end)+1;
cwsdefocus=cwstilt(end)+1;
wfe=cwsdefocus(end)+[1:3];
flux=wfe(end)+1;
mode=flux(end)+1;
DMsensor=mode(end)+[1:2];
DMdefocus=DMsensor(end)+[1:2];



% Compute Noise statistics
% Data size is N_samples*60 (sub-aperture)

d=target3_1(:,infocus)-target3_1(:,outfocus);
s=target3_1(:,infocus)+target3_1(:,outfocus);
potent=target3_1(:,deltavoltage);

   

% Calculation of Eq. 52 
% % Wavefront sensor measurment
%  w (N_samples*60)
w=target3_1(:,curvature);

% gi is the loop gain. At the moment it's fixed for all the electrodes but in the future it could be different
% gi needs to be intered or read
gi=g.*ones(1,60);  % 1*60 

% Dump the (processed) input data to file
dbg_dump('InputData.out', ...
         '\nsumCountsMean :\n'      , mean(s)     , '\n', ...
         '\nsumCountsVariance :\n'  , var(s)      , '\n', ...
         '\ndiffCountsMean :\n'     , mean(d)     , '\n', ...
         '\ndiffCountsVariance :\n' , var(d)      , '\n', ...
         '\ncurvatureMean :\n'      , mean(w)     , '\n', ...
         '\ncurvatureCovariance :\n', cov(w)      , '\n', ...
         '\nmirrorMean :\n'         , mean(potent), '\n', ...
         '\nmirrorCovariance :\n'   , cov(potent) , '\n', ...
         '\ngainMean :\n'           , gi          , '\n', ...
         '\ngainVariance :\n'       , zeros(1,60) , '\n');


% Eq. 53 

N_s=mean(s);     % size 1*60 


var_s=var(s);  % size 1*60 


var_d=var(d);  % size 1*60 
var_w=var(w); % size 1*60 

% Because we are using an orthogonal and normalsied basis Cww the covariance of the Wavefront sensor measurment 
% is a diagonal matrix with the variances as diagonal elements.

Cww=diag(var_w); % (60*60) matrix

% Eq. 54
var_S=var_s-N_s;   % 1*60 

 % Eq. 55
var_D=var_d-N_s;  % 1*60 
% Eq. 56

var_W=var_d./(N_s.^2)-1./N_s;  % 1*60 

% Eq. 57

var_nw=var_w-var_W;  % 1*60 

Cnwnw=diag(var_nw);   % 60*60 matrix



% load control matrices

load('v2z_3136.mat');

% v2z=v2z_3136;

% load control matrices
% params_contmat is 3600x1 vector, it needs to be reshaped 60*60

load('params_contmat.dat');
cm=reshape(params_contmat,60,60);

% params_contmat is 120x1 vector, it needs to be reshaped 2*60

load('params_contmat2.dat');
cm2=reshape(params_contmat2,2,60);


%  load cm_10s_M115_g100_f25_r10

 
% load mirror_projection  % load projection matirx  dm to mirror modes


%load Caa covariance matrix of mirror modes 
% For a given mirror modes, it is fixed.

load Caa_300_128.mat % output is Caa a 60*60 matrix 

dbg_dump('TransformedNollMatrix.out', Caa);

% load Covariance matrix of the remaining error
% contain Crr (60*60) matrix
% Crr is dependant of the focal distance of the membrane mirror
% The correcr file should be downloaded  accordingly. 


  load Crr_g6_f25

dbg_dump('RemainingError.out', Crr);

% load structure function of the high order phase 
% Dphi is (2*samples_pupil)^2 matrix    Dphi is computed only once and it's done in matlab

load Dphi_20s_data_128_1Delta.mat    % Eq. 34  

dbg_dump('StructFuncHighOrderPhase.out', Dphi_perp_Dr01);


% compute modal control matrix
v2z=v2z.*(2.*pi./correcting_wavelength);
C=v2z*cm;  % (60*60 matrix)

dbg_dump('ControlMatrix.out', C);

 % compute noise covariance
 
 disp('Compute Noise variances')
 
 
%  compute_NoiseWFS_veran
 
Cww=cov(w);
 

Cnn = C*Cnwnw*C';  % Eq. 42  % (60*60 matrix)  % Noise expression according to Veran
%  Cnn = C*diag(1./N_s)*C'; % Just there to avoid negative noise variances... equivalent expression Poisson Noise  % (60*60 matrix)

var_n=diag(Cnn); % 1*60 

dbg_dump('PropagatedMeasurementError.out', Cnn);


% variances of modal commands

m=2.*(v2z)*potent'; % N_samples*60
Cmm=cov(m');
var_m=diag(Cmm); % 1*60 


% compute Noise transfert function



switch g
   case 1.16
      f_max=133.6;
   case 1.0
      f_max=117.5;
   case 0.5
      f_max=42.0;
   case 0.25
      f_max=17.0;
   case 0.116
      f_max=7.0;
   case 0.165
      f_max=10.1;
    otherwise
          disp('Unknown gi value, cannot find corresponding f_max .')
end

%  The following values are estimatimated graphically from ...
% if g=1.16 max f=133.6

% if g=1. max f=117.5
% if g=0.5 maxf=42.0  at -3 db 
% if g=0.25 maxf=17.  at -3 db 
% if g=0.116 maxf=7.  at -3 db 
% if g=0.165 maxf=10.1  at -3 db 


%Hn_integration=compute_Hn60(gi,1./350,0.31e-3,f_max); % see word document for details of this expression  % 1*60 
Hn_integration=compute_Hn60(gi); % see word document for details of this expression  % 1*60 


% Calculation of D/r0 

Kij=diag(Caa); % 1*60 
Kij=Kij';  % % 1*60 


% load Covariance matrix of the remaining error
% contain Crr (60*60) matrix
% Crr is dependant of the focal distance of the membrane mirror
% The correcr file should be downloaded  accordingly. 

% load Crr_3136_foc28  % Contains Crr a 60*60 matrix 

% load Crr_128_2200_f25
% D/r0 loop   % paragraph 3.2.3



% % focal=[ 10 15 20 25 28 30];
% % % Crr=Crr_all{4};
% % % var_r=diag(Crr);

disp('Compute D/r_0 ratio')

D_r0_new=0.;
D_r0_error=1000.;
count=1;
D_r0_old=0;
D_r0=0.;




% % var_n=diag(Cnwnw);
while  abs(D_r0_error) >= 1.e-4
     clear D_r0i var_ai Crr_es var_r
     Crr_es=Crr.*(D_r0).^(5/3); % Eq. 46  (60*60 matrix)
  
    var_r=diag(Crr_es); % 1*60 


    
%     plot(var_r(4:60))
    title('var_r')
%     hold on
    
	var_ai=var_m-var_r-var_n.*Hn_integration';  % Eq. 49  % 1*60 
    figure(1)
    hold off
    semilogy(var_m)
    hold on
    semilogy(var_r,'r')
    semilogy(var_n.*Hn_integration','k')
    legend('var_m','var_r','var_n.*Hn')
    xlabel('Mirror modes')
    ylabel('Variances in rad^2')
    figure(2)
    
    hold off
    semilogy(var_ai(2:60))
    hold on
    semilogy(var_m(2:60),'r')
 
    
    semilogy(Kij(2:60).*(D_r0).^(5/3),'k*')
    
    
    legend('\sigma_a^2','\sigma_m^2','K_{ij}*(D/r0)^{(5/3)}');
    xlabel('Mirror modes')
    ylabel('Variances in rad^2')
    title('D/r_0 computation loop ')
	D_r0i=(var_ai./Kij').^(3/5);   % Eq. 47  % 1*60 
    x=zeros(60,1);
    % Reject non real D_r0i
    
    for ii=1:60
        x(ii)=isreal(D_r0i(ii));   
    end
    
    D_r0i=x.*D_r0i;
   figure(4)
%   coords=  (D_r0i>2*std(D_r0i) | D_r0i>(D_r0i+ 2*std(D_r0i)));
%   D_r0
	% remove the piston and the tip/tilt (first 3 modes ??)
	D_r0i=D_r0i(4:N_used_modes);
    % Check how to remove more modes according to the errors on the results . Should be done everytime 
    plot(D_r0i)
    title('D_r0i');
	D_r0_new=mean(D_r0i);
    disp([' Intermediate estimated Ratio D/r_0 =  :   ',  num2str(D_r0_new) ])

	D_r0_error = (D_r0_new-D_r0_old);
    D_r0_old=D_r0_new;
    D_r0=D_r0_new;
    if mod(count,10)==0
        count
        pause(1)
    end
   count=count+1;
   
    
end

disp([' Estimated Ratio D/r_0 =  :   ',  num2str(D_r0) ])

dbg_dump('ApertureToFriedRatio.out', D_r0);

% scale structure function ro the proper D/r0 ratio


D_perp=Dphi_perp_Dr01*(D_r0)^(5/3);   % Eq. 45  (2*samples_pupil)^2


% Compute the high order OFT 

B_perp=exp(-1/2.*D_perp);  % Eq. 33 (2*samples_pupil)^2

dbg_dump('OTFHighOrderPhaseContrib.out', B_perp);

% scale the covraiance matrix of the remaining error to the proper D/r0 ratio

Crr=Crr.*(D_r0)^(5/3);  % Eq. 46  % 60*60 matrix

% calculation of the covariance matrix of the curvature measurements

Ceses=C*Cww*(C)';  % Eq. 41 % 60*60 matrix

% Calculation of the covariance matrix of the mirror component of the residual phase
disp('Compute covariance matrix of the mirror component of the residual phase')

Cee = Ceses - Cnn + Crr;  % Eq. 40  % 60*60 matrix

% Cee=cov(e');

dbg_dump('ModalCoordResidualPhase.out', Cee);

% Calculation of the structure function of the parallel corrected phase
disp(' Computation of the structure function of the parallel corrected phase (Use Uij function)')

% load of 59*60/2=1770 U functions and computation of (eq. 36) 
% U functions Eq. 37 are produced in Matlab  only once   (2*samples_pupil)^2
D_e_par=zeros(2*samples_pupil);  
count=1;
for ii=2:60
%     if mod(ii,5)==0 
%         ii
%     end
    for jj=ii:60
        u_name=['u',num2str(ii),'_',num2str(jj),'.mat'];          
        load(u_name)
        if ii~=jj;
        D_e_par=D_e_par+(Cee(ii,jj)+Cee(jj,ii)).*u;   
    else
        D_e_par=D_e_par+(Cee(ii,jj)).*u;
    end
    
        clear u
        count=count+1;
    end
end
count

dbg_dump('StructFuncMirrorComp.out', D_e_par);

% Compute the high order OFT (eq. 35)

B_e_par=exp(-.5.*D_e_par);   % (2*samples_pupil)^2

dbg_dump('OTFMirrorCompContrib.out', B_e_par);

% No information on the static OTF 
% The telescope OTF needs to taken into account

compute_pupil_autocorrelation


B_tel = corre_pupil./max(corre_pupil(:));

dbg_dump('OTFTelescope.out', B_tel);


disp('Compute PSF and compare it with the recorded one')



%reconstructed_B = B_perp .* B_e_par .*B_tel ;   % Eq. 31  (2*samples_pupil)^2
reconstructed_B = B_e_par .* B_perp .*B_tel ;   % Eq. 31  (2*samples_pupil)^2
reconstructed_PSF = otf2psf(reconstructed_B);
PSFmax=max(reconstructed_PSF(:));
PSFnorm = reconstructed_PSF./PSFmax;

dbg_dump('PSFnorm.out', PSFnorm);

% figure;
% imagesc(PSFnorm);
% colorbar;



integrated_strehl 			= [];


pupil_samples=128;
the_vlt_pupil = vlt_pupil( 128, 1, 0 );
pupil_mask = get( the_vlt_pupil, 'mask' );

dbg_dump('pupil.out', pupil_mask);

diffraction_limited_field = pupil_mask.*ones( pupil_samples );
diffraction_limited_psf ...
    = field2image( diffraction_limited_field, 4 );
dbg_dump('optimalPsf.out', diffraction_limited_psf);
strehl_reference = relative_maximum( diffraction_limited_psf );
dbg_dump('maxOptimalPsf.out', strehl_reference);
dbg_dump('maxPsf.out', relative_maximum( reconstructed_PSF));



reconstructed_strehl = relative_maximum( reconstructed_PSF )./strehl_reference ;


% Computation of variances
% D_r0_5=8/r05
r0_estimated_correcting=8/D_r0;

r05_estimated=R01toR02(r0_estimated_correcting,correcting_wavelength.*1e6,0.5);


Sampling_number=length(d);
disp([' Number of samples to reconstruct the PSF  :   ',  num2str(Sampling_number) ])


total_var_a_estimated=sum(var_ai(2:N_used_modes));
total_var_r=sum(diag(Crr(2:N_used_modes,2:N_used_modes)));


C_bt=(C*diag(var_W)*C');
sum(diag(C_bt(2:N_used_modes,2:N_used_modes)));


Estimated_uncorrected_phase = total_var_a_estimated*(r0_estimated_correcting/r05_estimated)^(5/3);

Estimated_WFS_measurement_error = sum(var_n(2:N_used_modes));
Estimated_aliasing_error=total_var_r*(r0_estimated_correcting/r05_estimated)^(5/3);
Estimated_residual_phase=sum(diag(Cee(2:N_used_modes,2:N_used_modes)));
RTC_estimated_residual_phase=sum(diag(Ceses(2:N_used_modes,2:N_used_modes)));


disp([ 'Reconstructed strehl:                            ' ...
		num2str( reconstructed_strehl) ]);


disp([ 'Estimated r0 (m) at 500 nm:                            ' ...
		num2str( r05_estimated) '  m']);


disp([ 'Estimated uncorrected phase at 500 nm:                ' ...
		num2str( Estimated_uncorrected_phase)    '  rad^2']);
    
disp([ 'Estimated WFS measurment Error:             ' ...
		num2str(Estimated_WFS_measurement_error) '  rad^2']);
       
 disp([ 'Estimated aliasing error at 500 nm:                  ' ...
		num2str(Estimated_aliasing_error) '  rad^2']);
       
 disp([ 'Estimated residual phase:                  ' ...
		num2str(Estimated_residual_phase) '  rad^2']);
       
disp([ 'RTC estimated residual phase:               ' ...
		num2str(RTC_estimated_residual_phase) '  rad^2']);
        
        
        
        
        figure



reportPSF_real_data