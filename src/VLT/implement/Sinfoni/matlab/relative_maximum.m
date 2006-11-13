% =============================================================================
%  Maximum value extraction function
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  File:       relative_maximum.m
%  Version:    0
% -----------------------------------------------------------------------------
%  [ value, row, column ] = relative_maximum( intensity )
%                   value = relative_maximum( intensity )
%
%  value                 return value
%  row                   row of maximum value (optional)
%  column                column of maximum value (optional)
%  intensity             intensity map
% -----------------------------------------------------------------------------
%  Calculates the ratio between the maximum intensity and the total intensity
%  of the given intensity map. Optionally returns the row and column of the
%  maximum.
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: relative_maximum.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function [ value, row, column ] = relative_maximum( intensity )

maximum_intensity = max( max( intensity ));
total_intensity = sum( sum( intensity ));

if total_intensity == 0,
	total_intensity = 1;
end

%if nargout == 3,
	[ xc, yc ] = find( intensity == maximum_intensity );
	xc = xc( 1 );
	yc = yc( 1 );
    %end

% improve resolution
%xcoord = [size(intensity,1)/2+1-64:size(intensity,1)/2+1+64]; %[xc-25:xc+24];
%ycoord = [size(intensity,2)/2+1-64:size(intensity,2)/2+1+64]; %[yc-25:yc+24];
xcoord=1:length(intensity);
ycoord=1:length(intensity);

mx = 0;
for k=ycoord
    mx = mx + sum(intensity(xcoord,k).*xcoord');
end
row = mx / sum(sum(intensity(xcoord,ycoord)));

my = 0;
for k=xcoord
    my = my + sum(intensity(k,ycoord).*ycoord);
end
column = my / sum(sum(intensity(xcoord,ycoord)));

% --

value = maximum_intensity/total_intensity;
