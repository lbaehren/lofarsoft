% =============================================================================
%  Bimorph Mirror Simulation
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  Class:      bimorph
%  File:       get.m
%  Version:    0
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: get.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function a_value = get( this, a_data_member )

error( nargchk( 2, 2, nargin ) );

if ~strcmp( fieldnames( this ), a_data_member )
	error( ['class bimorph has no data member called "' a_data_member '"' ] );
	return;
end;

a_value = eval( [ 'this', '.', a_data_member ] );
