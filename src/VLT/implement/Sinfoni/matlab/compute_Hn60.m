%% Function to calculate Sum(|Hn(f,gi)|^2*df)   Noise transfer function 
%   f is the frequency  - f is a vector
%   gi is the gain for the ith mode - gi is a vector 
%   Hsys is the system transfer function
%  Hwfs  is the wave front sensor transfer function
%  Hol is the open loop transfer function 
%  gi are the modal gains

%  See Roddier 1999 p 145 for more details
%%%% This is the correct expression


function Hn_sum = compute_Hn60(gi)

% Te is the integration time
Te=1./350;

% tau is the delay value
tau=0.31e-3;

% Compute the upper frequency that must be used in integrating the
% noise transfer function.
% Note: The current implementation is based on estimates that were
% taken from a graph.
f_max = 0.0;
if (min(gi) < 0 | max(gi) > 1.16)
    warning ('Gain values gi out of range.');
elseif (max(gi) > 1.0)   f_max = 133.6;
elseif (max(gi) > 0.5)   f_max = 117.5;
elseif (max(gi) > 0.25)  f_max =  42.0;
elseif (max(gi) > 0.165) f_max =  17.0;
elseif (max(gi) > 0.116) f_max =  10.1;
else                     f_max =   7.0;
end

%%% Check the dimensions of f and gi

 [sx_gi,sy_gi]=size(gi);
 if sy_gi==1 
    gi=gi' ;
 end


clear i


df=0.05;
f=df:df:f_max;



f=f';

Hwfs=sinc(Te.*f).*exp(-i.*pi.*f.*Te)*ones(1,60);
Hol=exp(-i.*2.*pi.*f.*tau)./(Te.*2.*i.*pi.*f).*sinc(Te.*f).*exp(-i.*pi.*f.*Te)*gi;
Hcl=Hol./(1+Hol);

Hn=Hcl./(Hwfs);


% % db3=-3.*ones(length(f),1);
% % 
% % 
% % Hn_db=20*log10(abs(Hn));
% % 
% % 
% % 
% % 


Hn_sum=sum(Hn.*conj(Hn).*df);