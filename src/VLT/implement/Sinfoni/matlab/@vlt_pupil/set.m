% =============================================================================
%  Bimorph Mirror Simulation
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  Class:      bimorph
%  File:       set.m
%  Version:    0
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: set.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function set( this, a_data_member, a_value )

error( nargchk( 3, 3, nargin ) );

if ~strcmp( fieldnames( this ), a_data_member )
	error( [ 'class bimorph has no data member called "' a_data_member '"' ] );
end;

eval( [ 'this', '.', a_data_member, ' = ', 'a_value', ';' ] )
assignin( 'caller', inputname(1), this );
