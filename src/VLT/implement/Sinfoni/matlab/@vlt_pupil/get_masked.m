% =============================================================================
%  Very Large Telescope Pupil Mask
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  Class:      vlt_pupil
%  File:       get_masked.m
%  Version:    0
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: get_masked.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function masked = get_masked( this, incident )

if isempty( incident )
	masked = incident;
	return;
end

% error message if incident matrix is wrong size
if length( incident ) ~= length( this.mask ),
	error( 'vlt_pupil: get_masked: invalid incident matrix size' );
end

masked = this.mask .* incident;
