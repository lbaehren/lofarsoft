function [this,out_dat] = proc_dat(this, in_dat )
% PROC_DAT beamformer data processor

p=this.p;

assert(size(in_dat,1)==p.nof_antennas,     'wrong input dimensions');
assert(size(in_dat,2)==p.nof_polarizations,'wrong input dimensions');
assert(size(in_dat,3)==p.nof_beamlets     ,'wrong input dimensions');
assert(size(in_dat,4)>0                   ,'wrong input dimensions');

nof_slices = size(in_dat,4);
dat=zeros(p.nof_antennas,p.nof_polarizations,p.nof_beamlets,nof_slices,p.nof_phases);

dat(:,:,:,:,1)=real(in_dat);
dat(:,:,:,:,2)=imag(in_dat);

dat=permute(dat,[1 2 3 5 4]);
out_dat = zeros(size(dat));
for i=1:nof_slices
    for ip=1:p.nof_polarizations
        for iq=1:this.p.nof_phases
            out_dat(:,:,:,:,i) = out_dat(:,:,:,:,i) + dat(:,:,:,:,i) .* this.coeff(:,:,:,:,ip,iq);
        end
    end
end
out_dat = ipermute(out_dat,[1 2 3 5 4]);
out_dat = complex(out_dat(:,:,:,:,1),out_dat(:,:,:,:,2));

assert(size(in_dat)==size(out_dat), 'wrong output dimensions');