function [y1,y2]=split_fft(y)

fft_size=length(y);
n=2:fft_size/2;

y1(1)=real(y(1));
y1(n)=0.5*(conj(y(fft_size+2-n))+y(n));

y2(1)=imag(y(1));
y2(n)=0.5*complex(0,1)*(conj(y(fft_size+2-n))-y(n));