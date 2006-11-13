% =============================================================================
%  Very Large Telescope Pupil Mask
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  Class:      vlt_pupil
%  File:       display.m
%  Version:    0
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: display.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function display( this )

disp( [ class( this ), ' object' ] )
eval( [ inputname( 1 ), '= struct( this )' ] )
