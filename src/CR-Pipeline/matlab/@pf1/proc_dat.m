function [this, out_dat] = proc_dat( this, in_dat )
% process prefilter 1 data

assert( size(in_dat,1) == this.p.nof_antennas,'wrong input dimensions');
assert( size(in_dat,2) == this.p.nof_polarizations,'wrong input dimensions');
assert( size(in_dat,3) > 0 ,'wrong input dimensions');
assert( mod(size(in_dat,3),this.p.fft_size)==0,'wrong input dimensions');
    
% decimation
out_dat=zeros(this.p.nof_antennas,this.p.nof_polarizations,this.p.fft_size,size(in_dat,3)/this.p.fft_size);

assert(prod(size(in_dat))==prod(size(out_dat)),'wrong output dimensions');

for ia=1:this.p.nof_antennas
  for ip=1:this.p.nof_polarizations
    for ib=1:this.p.fft_size
      dat=downsample(in_dat(ia,ip,:),this.p.fft_size,ib-1);
      [dat,this.state{ia,ip,ib}] = filter(this.coeff(ib,:),1,dat,this.state{ia,ip,ib});
      out_dat(ia,ip,ib,:)= dat * this.rscale;
    end
  end
end

% NOTE: requantization is not performed  here, it is assumed that the data will
% be quantized when entering the pft stage.

assert(prod(size(in_dat))==prod(size(out_dat)),'wrong output dimensions');
