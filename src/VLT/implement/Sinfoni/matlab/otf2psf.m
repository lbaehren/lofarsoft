% -----------------------------------------------------------------------------
%  File:      otf2psf.m
%  Version:    0
% -----------------------------------------------------------------------------
%  psf = otf2psf( otf, factor )
%
%  psf      returned psf
%  otf      otf that is to be turned into a psf
%  factor     factor by which the field is zero padded before turned into image
% -----------------------------------------------------------------------------
%  Given a complex otf, this function generates the corresponding psf. 
%  Note that the intensity is not in any well defined unit. The resulting
%  image is factor times as big as the given complex otf (because of the 
%  zero padding).
% -----------------------------------------------------------------------------



function psf = otf2psf( otf, factor )

% as default, zero pad by a factor of two
if nargin < 2,
	factor = 2;
end

% fill otf out with zeros out to double size
expanded_otf = zeros( factor*length( otf ));
middle_indices = middle( expanded_otf, factor );
expanded_otf( middle_indices, middle_indices ) = otf;

psf =  abs(fftshift( fft2( expanded_otf  )));