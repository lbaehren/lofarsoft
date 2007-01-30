function [y,z] = bf2(x,stage)
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
  
  
