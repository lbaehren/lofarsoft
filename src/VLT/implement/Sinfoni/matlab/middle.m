% =============================================================================
%  Middle function
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  File:       middle.m
%  Version:    0
% -----------------------------------------------------------------------------
%  indices = middle( width, factor )
%
%  indices          vector of indices
%  matrix           target matrix (square)
%  factor           fractional factor (integer power of two)
% -----------------------------------------------------------------------------
%  Given a matrix and a factor (integer power of two), this function returns 
%  the indices in the central portion of width length( matrix )/factor.
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: middle.m,v 1.1 2004/08/24 12:47:55 loose Exp $"
%

function indices = middle( matrix, factor )

% default factor is two
if nargin < 2,
	factor = 2;
end

width = length( matrix );
lower = width/2 - width/factor/2 + 1;
upper = lower + width/factor - 1;

indices = lower:upper;
