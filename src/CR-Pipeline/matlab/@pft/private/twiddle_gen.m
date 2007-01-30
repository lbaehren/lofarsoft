function w=twiddle_gen(stage)
% generate the twiddle factors
k=4^(stage-1);
a=[0.0 0.50 0.25 0.75]/k;
n=0:k-1;
for m=0:3
  w(m*k+n+1)=exp(-2*pi*j*n*a(m+1));
end
