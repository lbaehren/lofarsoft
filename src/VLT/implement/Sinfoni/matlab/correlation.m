%%%% Correlation function of 2 arrays of the same size. 
% inputs: f array of size N
%         g array of size N
%         factor  if factor set to 2 , both arrays will be put in a 2N zeros square array and centered 
%                                      1 , no change will be made in the size of arrays.
 
% fill field out with zeros out to double size

function corre = correlation(f,g,factor) ;

% as default, zero pad by a factor of two
if nargin < 3,
	factor = 1;
end

if length(f)~=length(g)
    error('Correlation f by g : Both functions must have the same dimension')
end

    l1=length(f);
	
	expanded_f=zeros(l1*factor);
	expanded_g=zeros(l1*factor);
    
% % % %         expanded_field = zeros( factor*length( field ));
% % % % middle_indices = middle( expanded_field, factor );
% % % % expanded_field( middle_indices, middle_indices ) = field;
% % % % 
	%corre=zeros(factor*l1);
	middle_indices = middle (expanded_f, factor);
	expanded_f(middle_indices ,middle_indices ) =f;
 	expanded_g(middle_indices, middle_indices ) =g;
   


corre = real(fftshift(ifft2(fft2(expanded_f).*conj(fft2(expanded_g)))));

