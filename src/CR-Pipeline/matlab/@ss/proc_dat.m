function [this,out_dat] = proc_dat(this, in_dat )
% PROC_DAT method for processing subband selector data
    
assert(size(in_dat,1)==this.p.nof_antennas,     'wrong input dimensions');
assert(size(in_dat,2)==this.p.nof_polarizations,'wrong input dimensions');
assert(size(in_dat,3)==this.p.nof_subbands     ,'wrong input dimensions');

for ia=1:this.p.nof_antennas
  for ib=1:this.p.nof_beamlets
    for ip=1:this.p.nof_polarizations
            index = this.sel(ib,ip);
            index_ip = 1+mod(index,this.p.nof_polarizations);
            index_ib = 1+floor(index/this.p.nof_polarizations);
            out_dat(ia,ip,ib,:)=in_dat(ia,index_ip,index_ib,:);
        end
    end
end