% =============================================================================
%  Very Large Telescope Pupil Mask
%
%  by Thomas V.Craven-Bartle 1999.
%  Copyright ESO 1999-2000. All rights reserved.
%
% -----------------------------------------------------------------------------
%  Class:      vlt_pupil
%  File:       vlt_pupil.m
%  Version:    0
% -----------------------------------------------------------------------------
%  CONSTRUCTOR
%  new_vlt_pupil = vlt_pupil( samples, secondary_flag, spider_flag )
%
%  samples        Number of samples across pupil.
%  secondary_flag 0 = no central obstruction
%  spider_flag    0 = no spider mount 
% -----------------------------------------------------------------------------
%  PUBLIC MEMBER FUNCTIONS
%  masked =  get_masked( this, incident )
% -----------------------------------------------------------------------------
%  Takes a square incident wavefront, assumes it is 8 meters in diameter,
%  and applies a mask which corresponds to the pupil of the VLT plus the
%  shadow of M2 and the spider.
% -----------------------------------------------------------------------------

% RCS Keyword
% "@(#) $Id: vlt_pupil.m,v 1.1 2005/11/11 16:07:54 loose Exp $"
%

function new_vlt_pupil = vlt_pupil( samples, secondary_flag, spider_flag )

% =============================================================================
%  check input arguments
% -----------------------------------------------------------------------------
% if empty constructor call, create dummy input arguments
if nargin == 0,
	samples = 4;
	secondary_flag = 0;
	spider_flag = 0;
end

% =============================================================================
%  optical parameters
% -----------------------------------------------------------------------------
pupil_diameter = 8.0;			% pupil diameter [m]
secondary_diameter = 1.12;		% diameter of central obstruction [m] 1.12

% PUEO
% % % % pupil_diameter = 3.6;			% pupil diameter [m]
% % % % secondary_diameter = 1.584;		% diameter of central obstruction [m] 1.12
% % % % 
alpha = 101;					% spider angle [degrees]
spider_width = 0.039;			% spider width [m] 0.039

delta = pupil_diameter/samples;	% distance between samples

% create coordinate matrices

                  
[ x, y ] = meshgrid( -( pupil_diameter - delta )/2:delta: ...
 					  ( pupil_diameter - delta )/2 );

if secondary_flag == 0,
	secondary_diameter = 0;
end

% =============================================================================
%  member data
% -----------------------------------------------------------------------------
this.samples = samples;
% this.mask = ( sqrt( x.^2 + y.^2 ) >= pupil_diameter/2 ) | ...
% 			( sqrt( x.^2 + y.^2 ) < secondary_diameter/2 );
% 
%         
  
this.mask = ( sqrt( x.^2 + y.^2 ) >= pupil_diameter/2) | ...
			( sqrt( x.^2 + y.^2 ) < secondary_diameter/2 );
% % % %  this.mask = ( sqrt( x.^2 + y.^2 ) >= pupil_diameter/2-1.*delta ) | ...
% % % % 			( sqrt( x.^2 + y.^2 ) < secondary_diameter/2 );
% % 
% this.mask = ( sqrt( x.^2 + y.^2 ) >= pupil_diameter/2-delta/16 ) | ...
% 			( sqrt( x.^2 + y.^2 ) < secondary_diameter/2 );
% 
% 
% spiders
if spider_flag,
	alpha_rad = alpha*pi/180;
	slope = tan( alpha_rad/2 );
	this.mask = this.mask | ...
			    (( y > slope*( x - secondary_diameter/2 )) & ...
				 ( y < slope*( x - secondary_diameter/2 ...
					   + spider_width/sin( alpha_rad/2 ))) & ...
				 x > 0 & y > 0 ) | ...
				(( y > slope*( -x - secondary_diameter/2 )) & ...
				 ( y < slope*( -x - secondary_diameter/2 ...
				   + spider_width/sin( alpha_rad/2 ))) & ...
				 x < 0 & y > 0 ) | ...
				(( -y > slope*( x - secondary_diameter/2 )) & ...
				 ( -y < slope*( x - secondary_diameter/2 ...
				   + spider_width/sin( alpha_rad/2 ))) & ...
				 x > 0 & y < 0 ) | ...
				(( -y > slope*( -x - secondary_diameter/2 )) & ...
				 ( -y < slope*( -x - secondary_diameter/2 ...
				   + spider_width/sin( alpha_rad/2 ))) & ...
				 x < 0 & y < 0 );
end

this.mask = 1 - this.mask;

% =============================================================================
%  turn structure into class and return new object
% -----------------------------------------------------------------------------
this = class( this, 'vlt_pupil' );
new_vlt_pupil = this;
