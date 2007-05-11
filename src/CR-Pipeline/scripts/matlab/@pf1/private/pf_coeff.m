function [coeff,scale]=pf_coeff(N,L,Q,W,r_pass, r_stop)
% COEFF - compute polyphase filterbank coefficients
%   C = PF_COEFF(N, L, Q, W, r_pass, r_stop) computes the coefficients for a 
%   polyphase filterbank with N fft channels, and subfilter length L.
%   For Q>1, the filter coefficients are computed by interpolating
%   a shorter filter with an upsampling factor Q. This reduces 
%   computation time substantially. N,L and Q must be powers of 2.
%   r_pass and r_stop are the maximum passband and stop band
%   deviations, respectively.
%   For W>1, the filter coefeff   
%   The returned value is a matrix of dimension (NxL)

% default values
if (nargin < 1)
    N=1024;
end;
if (nargin < 2)
    L=16;
end;
if (nargin < 3)
    Q=64;
end;
if (nargin < 4)
    W=16;
end;
if (nargin < 5)
    r_pass=1E-2;
end;
if (nargin < 6) 
    r_stop=1E-5;
end;
% initial filter length
M1=N*L/Q;
% interpolated filter length
M2=N*L;

% compute initial filter
c=fircls1(M1-1,Q/N,r_pass, r_stop,'trace');
%c=firceqrip(M1-1,Q/N,[r_pass r_stop]);

% use fourier interpolation method
if (Q>1) 
  f1=fft(c);
  f2=zeros(1,M2);
  % copy the lower frequency half. 
  n=0:M1/2;
  f2(1+n)=f1(1+n);
  % to make the impulse response symmetric in time,
  % we need to apply a small phase correction,
  % corresponding to a shift in the time domain.
  f2(1+n)=f2(1+n).*exp(-sqrt(-1)*(Q-1)*pi*n/M2);
  % recreate the upper part of the spectrum from the lower part
  f2(M2-n)=conj(f2(2+n));
  % back to time domain
  c=real(ifft(f2));
end;

% decimate
for i=1:N
  coeff(i,L:-1:1)=downsample(c,N,i-1);    
end;

% normalizetion.
% first, scale coeffcients to make max possible output value equal to 1.
coeff = coeff./max(sum(abs(coeff),2));

% rescale with a power of 2, to to make  0.5 < max(abs(coeff)) <= 1.0 
scale = 2^floor(log2(1/max(max(coeff))));
coeff = coeff * scale;

% quantize the coeffients, if needed
if(W>0)
  display(sprintf('quantizing W=%i',W));
  q=1/((2^(W-1))-1);
  coeff=q*round(coeff/q);
end;