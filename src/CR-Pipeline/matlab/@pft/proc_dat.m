function [this,out_dat]=proc_dat(this,in_dat)
% process pipelined fourier transform data
  
assert(isreal(in_dat),'input must be real');
assert(size(in_dat,1)==this.p.nof_antennas,'wrong input dimensions');
assert(size(in_dat,2)==this.p.nof_polarizations,'wrong input dimensions');
assert(size(in_dat,3)==this.p.fft_size,'wrong input dimensions');
assert(size(in_dat,4)>0,'wrong input dimensions');
  
dat=fft(in_dat,this.p.fft_size,3);
  
% because we have real input, we can omit half of the fft result
out_dat=dat(:,:,1:this.p.nof_subbands,:);
assert(prod(size(in_dat))==2*prod(size(out_dat)),'wrong output dimensions');

function todo
  for stage=this.p.n_stages:-1:1
    dat=bf1(dat,stage);
    dat=bf2(dat,stage);
    dat=twiddle(dat,this.w{stage});
  end;
  %index = bitrevorder(1:fft_size);
  out_dat=dat;



