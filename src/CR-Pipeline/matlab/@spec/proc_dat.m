function [this, out_dat]=proc_dat( this , in_dat )
% process spectral data

assert(size(in_dat,1)==this.p.nof_antennas,'wrong input dimensions');
assert(size(in_dat,2)==this.p.nof_polarizations,'wrong input dimensions');
%assert(size(in_dat,3)==this.p.nof_beamlets,'wrong input dimensions');
assert(size(in_dat,4)>0,'wrong input dimensions');


this.slice=this.slice+1;
dat=sum(abs(in_dat).^2,4);
y=10*log10(dat(1,1,:));
this.pwr(this.slice,:)=-y;
this.pwr(this.pwr<0)=0;
set(this.h,'CData',this.pwr');
drawnow;

figure(2);
plot(y(:));
str = sprintf('spec: power slice %d', this.slice);
title(str);
grid on;

out_dat=in_dat;
