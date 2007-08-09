function [y,z] = bf1(x,stage)
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