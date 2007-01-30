function [this,out_dat] = proc_dat(this, in_dat )
% process waveform generator data
p=this.p;
out_dat=zeros(p.nof_antennas,p.nof_polarizations,p.nof_samples);
t=0:p.nof_samples-1;

% WG read from file:
%out_dat(1,1,:) = this.dat(this.phase + 1 + t);
%this.phase=this.phase+p.nof_samples;

% WG calc:
dat=this.ampl*cos(2*pi*(this.freq*t+this.phase));
out_dat(1,1,:) = dat;
this.phase=mod(this.freq*p.nof_samples+this.phase,1);