function [this, out_dat]=proc_dat( this , in_dat )
% process statistics data
assert(size(in_dat,1)==this.p.nof_antennas,'wrong input dimensions');
assert(size(in_dat,2)==this.p.nof_polarizations,'wrong input dimensions');

this.count = mod(this.count + size(in_dat,4),this.p.st_nof_samples);
if this.count==0
    this.pwr=zeros(size(this.pwr));
end;    

this.pwr = this.pwr + abs(in_dat).^2;
out_dat = in_dat;