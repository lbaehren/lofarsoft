function inv = invimplms(den,n,d) ;

% d is the delay of the result since subfilters have 16 taps, max delay
% will be 15
if nargin<3, d=15;end
% n is the length of the result, since implemented subfilter is 16 taps
% long, result will also have 16 taps 
if nargin<2, n=16;end
if nargin<1, 

    % load the actual filter coefficients in 'coeff.m' file
    filtercoeff = load('coeff.m');   
    s=1;
    while s<17,         % because every subfilters have 16 taps
    
       % there are 1024 subfilters in the case of polyphase filter bank implemented at LOFAR 
       for a=1:1:1024 
           c = a+(s-1)*1024 ;     
           filtercoeff(a,s)=filtercoeff(c);
       end
s=s+1;
end
% above loop will return an array of 1024*16 dimension

% den is the denominator impulse which needs to be inverted for first
% subfilter thats why (1,:) for second it will be (2,:) and so on so forth
den = filtercoeff(1,:) ;
end

m=xcorr(den,n-1);
% m is the first column of Toeplitz matrix
m=m(n:end);

% b is the vector same length of den but first element of den will be the
% last element of b, second element will be second last of b and hence last
% element of den will be first element of b
b=[den(d+1:-1:1); zeros(n-d-1,1)];

% function which calculates the inverted coefficients
inv = toepsolve(m,b);

% inverted filter coefficients will be saved in file 'ppfcoeff_inv.m'
fid=fopen('ppfcoeff_inv.m','a');
fprintf(fid,'%6.14f\n', inv);
status=fclose(fid);

function hinv = toepsolve(r,q);
n= length(q) ;
a=zeros(n+1,2) ;
a(1,1)=1 ;
hinv=zeros(n,1) ;
hinv(1)=q(1)/r(1) ;

alpha =r(1) ;
c=1 ;
d=2 ;

for k=1:n-1
    a(k+1,c)=0 ;
    a(1,d)=1 ;
    beta=0 ;
    j=1:k ;
    beta=sum(r(k+2-j)*a(j,c))./alpha ;
    a(j+1,d)=a(j+1,c)-beta*a(k+1-j,c);
    alpha=alpha*(1-beta^2);
    hinv(k+1,1)=(q(k+1)-sum(r(k+2-j)*hinv(j,1)))/alpha ;
    hinv(j)=hinv(j)+a(k+2-j,d)*hinv(k+1);
    temp = c;
    c =d;
    d =temp;
end

