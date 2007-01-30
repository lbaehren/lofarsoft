% radix 4 fft, implemented as a 2^2 pipelined fft
function y=pft(x)

fft_size=length(x);
n_stages = log2(fft_size)/2;

n_data_mul=18;
n_data_out=20;
n_twiddle=16;

if(mod(2*n_stages,2))
    % TODO: generalize to odd powers of 2
    error(' fft_size must be a power of 4');
end

% calculate the (quantized) twiddle factors
for k=1:n_stages
    w{k}=pft_quantize(pft_twiddle_gen(k),n_twiddle);
end;

% perform the pipelined fft
% note: consecutive stages are numbered from n_stages downto  1
x=pft_quantize(x,n_data_out);  
for k=n_stages:-1:1
  x=pft_bf1(x,k); 
  x=pft_bf2(x,k);
  if(k>1)
    x=pft_quantize(x,n_data_mul);  
    x=pft_twiddle(x,w{k});
  end
  x=pft_quantize(x,n_data_out);  
end;
index = bitrevorder(1:fft_size);
y(index)= x;

function y = pft_bf1(x,stage)
% butterfly 1

  k=2^(2*stage-1);
  n=1:length(x);
  y=zeros(1,length(x));
  s=bitand(n-1,k);
  m=n(s>0);
  y(m)=x(m-k)-x(m);
  m=n(~s>0);
  y(m)=x(m+k)+x(m);
  y=0.5*y;  
  
function y = pft_bf2(x,stage)
% butterfly 2
  k=2^(2*stage-2);
  y=zeros(1,length(x));
  m=1:length(x);
  s1 = bitand(m-1,k);
  s2 = bitand(m-1,2*k);

  n=m(s1.*s2>0);
  y(n) = i*x(n) + x(n-k);
  n=m(~s1.*s2>0);
  y(n) = x(n) - i*x(n+k);
  n=m(s1.*~s2>0);
  y(n) = x(n-k) - x(n);
  n=m(~s1.*~s2>0);
  y(n) = x(n+k) + x(n);
  y=0.5*y;
  
  
function y = pft_twiddle(x,w)
% twiddle 
  y=zeros(1,length(x));
  k=length(w);
  n=1:length(x);
  m = 1+mod(n-1,k);
  y = x .* w(m);
  
function w=pft_twiddle_gen(stage)
% generate the twiddle factors
k=4^(stage-1);
a=[0.0 0.50 0.25 0.75]/k;
n=0:k-1;
for m=0:3
  w(m*k+n+1)=exp(-2*pi*j*n*a(m+1));
end

function y=pft_quantize(x,n_bits)

if(max(abs(real(x)))>1)
    error(sprintf('warning, saturation for real value %f',max(abs(real(x)))));
end;

if(max(abs(imag(x)))>1)
    error(sprintf('warning, saturation for imag value %f',max(abs(imag(x)))));
end;
  
if n_bits>0
  q=2^(n_bits-1);
  y=round(x*q)./q;
else
  y=x;
end;