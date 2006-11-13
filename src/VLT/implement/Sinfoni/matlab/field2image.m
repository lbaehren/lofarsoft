% =============================================================================
%  Imaging function
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  File:       field2image.m
%  Version:    0
% -----------------------------------------------------------------------------
%  image = field2image( field, factor )
%
%  image      returned image
%  field      field that is to be turned into an image (square)
%  factor     factor by which the field is zero padded before turned into image
% -----------------------------------------------------------------------------
%  Given a complex field, this function generates the corresponding image. 
%  Note that the intensity is not in any well defined unit. The resulting
%  image is factor times as big as the given complex field (because of the 
%  zero padding).
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: field2image.m,v 1.2 2004/08/27 13:08:57 loose Exp $"
%

function image = field2image( field, factor )

% as default, zero pad by a factor of two
if nargin < 2,
	factor = 2;
end

% fill field out with zeros out to double size
expanded_field = zeros( factor*length( field ));
middle_indices = middle( expanded_field, factor );
expanded_field( middle_indices, middle_indices ) = field;

% go to image plane and get intensity
image = abs( fftshift( fft2( expanded_field  ))).^2;
