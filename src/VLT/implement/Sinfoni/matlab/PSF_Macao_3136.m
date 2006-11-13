
%
% PSF Reconstruction program
%
function PSFnorm = PSF_Macao_3136()

% load data
load('target3_1.dat');

samples_pupil=64;  %It could be 128 in the future
 
%% Extract the correct data out of the file
% % % coords3_1.m
% 60 corresponds to the number of sub-apertures of the system as well as the number of electrodes of the deformable mirror
% the first colunm is the time
% Next 60 columns are the infocus number of photons (1 column per sub-aperture) 
% Next 60 columns are the outfocus number of photons (1 column per sub-aperture) 
% ...
    

% Unfortunately, Octave does not define the "end" keyword for matrix indices.
% Hence we have to fake it using size()
timestamp=[1];
infocus=timestamp(size(timestamp,2))+[1:60];
outfocus=infocus(size(infocus,2))+[1:60];
curvature=outfocus(size(outfocus,2))+[1:60];
CWSZernike=curvature(size(curvature,2))+[1:60];
DMZernike=CWSZernike(size(CWSZernike,2))+[1:60];
deltavoltage=DMZernike(size(DMZernike,2))+[1:60];
TTMtiptilt=deltavoltage(size(deltavoltage,2))+[1:2];
LGSTiptilt=TTMtiptilt(size(TTMtiptilt,2))+[1:2];
EstPiston=LGSTiptilt(size(LGSTiptilt,2))+[1];
cwstip=EstPiston(size(EstPiston,2))+1;
cwstilt=cwstip(size(cwstip,2))+1;
cwsdefocus=cwstilt(size(cwstilt,2))+1;
wfe=cwsdefocus(size(cwsdefocus,2))+[1:3];
flux=wfe(size(wfe,2))+1;
mode=flux(size(flux,2))+1;
DMsensor=mode(size(mode,2))+[1:2];
DMdefocus=DMsensor(size(DMsensor,2))+[1:2];



% Compute Noise statistics
% Data size is N_samples*60 (sub-aperture)

d=target3_1(:,infocus)-target3_1(:,outfocus);
s=target3_1(:,infocus)+target3_1(:,outfocus);
potent=target3_1(:,deltavoltage);



% Calculation of Eq. 52 
% % Wavefront sensor measurment
%  w (N_samples*60)
w=d./s;

% gi is the loop gain. At the moment it's fixed for all the electrodes but in the future it could be different
% gi needs to be intered or read
gi=0.75.*ones(1,60);  % 1*60 
 

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

Cww=cov(w);

% Eq. 54
var_S=var_s-N_s;   % 1*60 

 % Eq. 55
var_D=var_d-N_s;  % 1*60 
% Eq. 56

var_W=var_d./(N_s.^2)-1./N_s;  % 1*60 

% Eq. 57

var_nw=var_w-var_W;  % 1*60 

Cnwnw=diag(var_nw);   % 60*60 matrix



% load transformation file from potential basis to zernike basis
% 1 60*60 matrix called v2z_3136

load('v2z_3136.dat');

v2z=v2z_3136;

% load control matrices
% params_contmat is 3600x1 vector, it needs to be reshaped 60*60

load('params_contmat.dat');
cm=reshape(params_contmat,60,60);

% params_contmat is 120x1 vector, it needs to be reshaped 2*60

load('params_contmat2.dat');
cm2=reshape(params_contmat2,2,60);



% compute modal control matrix

C=v2z'*cm;  % (60*60 matrix)

dbg_dump('ControlMatrix.out', C);

 % compute noise covariance
 
 
 
Cnn = C*Cnwnw*C';  % Eq. 42  % (60*60 matrix)

var_n=diag(Cnn); % 1*60 

dbg_dump('PropagatedMeasurementError.out', Cnn);

% variances of modal commands

m=2.*v2z*potent'; % N_samples*60

var_m=var(m'); % 1*60 


% compute Noise transfert function




Hn_integration=compute_Hn60(gi,90); % see word document for details of this expression  % 1*60 


% Calculation of D/r0 

%load Caa covariance matrix of mirror modes 
% For a given mirror modes, it is fixed.

load('Caa_64.mat');  % output is Caa a 60*60 matrix 

dbg_dump('TransformedNollMatrix.out', Caa);

Kij=diag(Caa); % 1*60 
Kij=Kij';  % % 1*60 


% load Covariance matrix of the remaining error
% contain Crr (60*60) matrix
% Crr is dependant of the focal distance of the membrane mirror
% The correcr file should be downloaded  accordingly. 

% How ? To ask Enrico
load('Crr_64_f28_sim_bis.mat');
% D/r0 loop   % paragraph 3.2.3

dbg_dump('RemainingError.out', Crr);


Dr0 = 1;



D_r0_new=0.;
D_r0_error=1000.;
count=1;
D_r0_old=0;
D_r0=0.;


% % var_n=diag(Cnwnw);
while  abs(D_r0_error) >= 1.e-10
    
     Crr_es=Crr.*(D_r0).^(5/3); % Eq. 46  (60*60 matrix)
  
    var_r=diag(Crr_es); % 1*60 


    
%     plot(var_r(4:60))
    title('var_r')
%     hold on
    
	var_ai=var_m'-var_r-var_n.*Hn_integration';  % Eq. 49  % 1*60 
    figure(1)
    hold off
    if (var_m > 0)
      semilogy(var_m)
    end
    hold on
    semilogy(var_r,'r')
    semilogy(var_n.*Hn_integration','k')
    legend('var_m','var_r','var_n.*Hn')
    figure(2)
    
    hold off
    semilogy(var_ai(2:60))
    hold on
    semilogy(var_m(2:60),'r')
 
    
    semilogy(Kij(2:60).*(D_r0).^(5/3),'*')
    legend('var_ai','var_m','K_{ij}*(D_r0)^{(5/3)}')
	D_r0i=(var_ai./Kij').^(3/5);   % Eq. 47  % 1*60 
    x=zeros(60,1);
    % Reject non real D_r0i
    
    for ii=1:60
        x(ii)=isreal(D_r0i(ii));   
    end
    
    D_r0i=x.*D_r0i;
   figure(4)
    
	% remove the piston and the tip/tilt (first 3 modes ??)
	D_r0i=D_r0i(4:60);
    % Check how to remove more modes according to the errors on the results . Should be done everytime 
    plot(D_r0i)
    title('D_r0i');
	D_r0_new=median(D_r0i)
	D_r0_error = D_r0_new-D_r0_old
    D_r0_old=D_r0_new;
    D_r0=D_r0_new;
    if mod(count,10)==0
        count
    end
    clear D_r0i var_ai Crr_es var_r
    
end

dbg_dump('ApertureToFriedRatio.out', D_r0);

% load structure function of the high order phase 
% Dphi is (2*samples_pupil)^2 matrix    Dphi is computed only once and it's done in matlab
load('Dphi3136_Macao_bis.mat');   % Eq. 34

dbg_dump('StructFuncHighOrderPhase.out', Dphi);

% scale structure function ro the proper D/r0 ratio


D_perp=Dphi*(D_r0)^(5/3);   % Eq. 45  (2*samples_pupil)^2

% Compute the high order OFT 

B_perp=exp(-1/2.*D_perp);  % Eq. 33 (2*samples_pupil)^2

dbg_dump('OTFHighOrderPhaseContrib.out', B_perp);

% scale the covraiance matrix of the remaining error to the proper D/r0 ratio

Crr=Crr.*(D_r0)^(5/3);  % Eq. 46  % 60*60 matrix

% calculation of the covariance matrix of the curvature measurements

Ceses=C*Cww*(C)';  % Eq. 41 % 60*60 matrix

% Calculation of the covariance matrix of the mirror component of the residual phase

Cee = Ceses - Cnn + Crr;  % Eq. 40  % 60*60 matrix

dbg_dump('ModalCoordResidualPhase.out', Cee);



% Calculation of the structure function of the parallel corrected phase

% load of 59*60/2=1770 U functions and computation of (eq. 36) 
% U functions Eq. 37 are produced in Matlab  only once   (2*samples_pupil)^2
D_e_par=zeros(2*samples_pupil);  

for ii=2:60
    if mod(ii,5)==0 
        ii
    end
    for jj=ii:60
        clear u;
%        u_name=sprintf('u%02d_%02d.dat', ii, jj);
%        u = load(u_name);
        u_name=['u',num2str(ii),'_',num2str(jj),'.mat']; 
        load(u_name);
        % Brute force approach to make U-functions well behaved.
        u = u.*(u >= 0);
        if ii~=jj;
            D_e_par=D_e_par+(Cee(ii,jj)+Cee(jj,ii)).*u;   
        else
            D_e_par=D_e_par+(Cee(ii,jj)).*u;
        end
    end
end

dbg_dump('StructFuncMirrorComp.out', D_e_par);

% Compute the high order OFT (eq. 35)

B_e_par=exp(-.5.*D_e_par);   % (2*samples_pupil)^2

dbg_dump('OTFMirrorCompContrib.out', B_e_par);

% No information on the static OTF 
% It does not need to be taken into account

% % % % % % B_stat=ones(128);




% Calculation of the long exposure OFT

B = B_perp .* B_e_par ;   % Eq. 31  (2*samples_pupil)^2
% long exposure PSF 

PSF = field2image(B); 
PSFmax=max(PSF(:));
PSFnorm = PSF./PSFmax;

dbg_dump('PSFnorm.out', PSFnorm);

figure;
imagesc(PSFnorm);
%colorbar;
