function this = pft(p)
% constructor for pipelined fourier transform
p.n_stages = log(p.fft_size)/log(4);
for i=1:p.n_stages 
  w{i} = twiddle_gen(i);
end;
this.p=p;
this.w=w;
this=node(class(this, 'pft'));