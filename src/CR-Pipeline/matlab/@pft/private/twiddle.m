function [y] = twiddle(x,w)
% twiddle 
  y=zeros(1,length(x));
  k=length(w);
  n=1:length(x);
  m = 1+mod(n-1,k);
  y = x .* w(m);
